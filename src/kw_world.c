/*
Keen Worlds: travelling between Keen 4, 5 and 6 in one running game.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <string.h>

#include "id_ca.h"
#include "id_in.h"
#include "id_mm.h"
#include "id_rf.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_sd.h"
#include "ck_act.h"
#include "ck_cross.h"
#include "ck_game.h"
#include "ck_play.h"
#include "ck_def.h"
#include "ck_ep.h"
#include "ck4_ep.h"
#include "ck5_ep.h"
#include "ck6_ep.h"
#include "kw_world.h"

extern const char *ck_episodeFile;

// What each world remembers while Keen is somewhere else. Everything else in
// ck_gameState (score, lives, ammo and all items, ck_gameState.ep) travels
// with Keen (decision 12).
typedef struct KW_WorldState
{
	bool visited;
	uint16_t mapPosX;
	uint16_t mapPosY;
	uint16_t levelsDone[25];
} KW_WorldState;

static KW_WorldState kw_worlds[3];

int kw_pendingWorld = 0;

static CK_EpisodeDef *KW_EpisodeDef(int episode)
{
	switch (episode)
	{
#ifdef WITH_KEEN4
	case 4:
		return &ck4_episode;
#endif
#ifdef WITH_KEEN5
	case 5:
		return &ck5_episode;
#endif
#ifdef WITH_KEEN6
	case 6:
		return &ck6_episode;
#endif
	default:
		return NULL;
	}
}

static KW_WorldState *KW_StateFor(CK_EpisodeDef *ep)
{
	return &kw_worlds[ep->ep - EP_CK4];
}

void KW_ResetWorlds(void)
{
	memset(kw_worlds, 0, sizeof(kw_worlds));
	kw_pendingWorld = 0;
}

void KW_RequestWorld(int episode)
{
	CK_EpisodeDef *target = KW_EpisodeDef(episode);
	if (!target || target == ck_currentEpisode || !target->isPresent())
		return;

	kw_pendingWorld = episode;
	ck_gameState.levelState = LS_SwitchWorld;
}

void KW_SwitchWorld(int episode)
{
	CK_EpisodeDef *target = KW_EpisodeDef(episode);
	kw_pendingWorld = 0;
	if (!target || target == ck_currentEpisode)
		return;

	// Remember where Keen was in the world he is leaving.
	KW_WorldState *from = KW_StateFor(ck_currentEpisode);
	from->visited = true;
	from->mapPosX = ck_gameState.mapPosX;
	from->mapPosY = ck_gameState.mapPosY;
	memcpy(from->levelsDone, ck_gameState.levelsDone, sizeof(from->levelsDone));

	// Nothing may keep playing from the old world's audio data.
	SD_MusicOff();
	SD_StopSound();

	// Swap the episode: data files, then variables, then data files again
	// (their names come from the variables).
	CA_UnloadEpisodeData();
	CK_VAR_Reset();
	ck_currentEpisode = target;
	ck_episodeFile = target->episodeFile;
	CK_VAR_LoadVars(ck_episodeFile);
	CA_Startup();
	CA_InitLumps();
	CK_CacheCoreChunks();

	// The sound manager works out where the AdLib effects start from the
	// episode's sound count when the sound mode is set, so set it again.
	SD_SoundMode soundMode = SD_GetSoundMode();
	SD_SetSoundMode(sdm_Off);
	SD_SetSoundMode(soundMode);
	CA_LoadAllSounds();

	// Restore the destination world, or start it fresh. A map position of 0
	// makes the map put Keen at the world's own starting point.
	KW_WorldState *to = KW_StateFor(target);
	if (to->visited)
	{
		ck_gameState.mapPosX = to->mapPosX;
		ck_gameState.mapPosY = to->mapPosY;
		memcpy(ck_gameState.levelsDone, to->levelsDone, sizeof(to->levelsDone));
	}
	else
	{
		ck_gameState.mapPosX = 0;
		ck_gameState.mapPosY = 0;
		memset(ck_gameState.levelsDone, 0, sizeof(ck_gameState.levelsDone));
	}

	ck_gameState.currentLevel = 0;
}

// ---- The Megarocket ----

static int kw_launchSpotX = -1, kw_launchSpotY = -1;
static bool kw_launchRequested = false;

// How far (in tiles) from the launch spot Keen may stand to board the rocket.
#define KW_LAUNCH_RANGE 1

static const struct
{
	int episode;
	const char *name;
} kw_destinations[] = {
	{4, "Gnosticus IV"},
	{5, "The Omegamatic"},
	{6, "Fribbulus Xax"},
};
#define KW_NUM_DESTINATIONS ((int)(sizeof(kw_destinations) / sizeof(kw_destinations[0])))

void KW_SetLaunchSpot(int tileX, int tileY)
{
	kw_launchSpotX = tileX;
	kw_launchSpotY = tileY;
}

void KW_TryLaunch(CK_object *mapKeen)
{
	if (kw_launchSpotX < 0)
		return;
	if (mapKeen->clipRects.tileX2 < kw_launchSpotX - KW_LAUNCH_RANGE || mapKeen->clipRects.tileX1 > kw_launchSpotX + KW_LAUNCH_RANGE)
		return;
	if (mapKeen->clipRects.tileY2 < kw_launchSpotY - KW_LAUNCH_RANGE || mapKeen->clipRects.tileY1 > kw_launchSpotY + KW_LAUNCH_RANGE)
		return;
	kw_launchRequested = true;
}

static void KW_WaitForButtonsUp(void)
{
	IN_ControlFrame frame;
	do
	{
		IN_PumpEvents();
		IN_ReadControls(0, &frame);
	} while (frame.jump || frame.pogo || frame.button2 || IN_GetKeyState(IN_SC_Enter) || IN_GetKeyState(IN_SC_Space));
	IN_ClearKeysDown();
	ck_keenState.jumpIsPressed = ck_keenState.pogoIsPressed = ck_keenState.shootIsPressed = false;
}

// Draws the destination menu and returns the chosen episode, or 0 to stay.
static int KW_TravelMenu(void)
{
	int lineY[KW_NUM_DESTINATIONS];
	int available[KW_NUM_DESTINATIONS];
	int numAvailable = 0;

	for (int i = 0; i < KW_NUM_DESTINATIONS; ++i)
	{
		CK_EpisodeDef *def = KW_EpisodeDef(kw_destinations[i].episode);
		if (def && def != ck_currentEpisode && def->isPresent())
			available[numAvailable++] = i;
	}
	if (!numAvailable)
		return 0;

	SD_PlaySound(CK_SOUNDNUM(SOUND_UNKNOWN12));
	VL_FixRefreshBuffer();
	US_CenterWindow(24, 4 + 2 * numAvailable);
	US_SetPrintY(US_GetPrintY() + 3);
	US_CPrint("Bean-with-Bacon Megarocket");
	US_CPrint("Fly to:");
	US_SetPrintY(US_GetPrintY() + 4);
	for (int i = 0; i < numAvailable; ++i)
	{
		lineY[i] = US_GetPrintY() - 2;
		US_CPrint(kw_destinations[available[i]].name);
		US_SetPrintY(US_GetPrintY() + 4);
	}

	KW_WaitForButtonsUp();

	// Same blinking selection box as the "Try Again" menu.
	int sel = 0, result = 0;
	int x1 = US_GetWindowX() + 4, x2 = US_GetWindowX() + US_GetWindowW() - 4;
	while (true)
	{
		IN_PumpEvents();
		int y = lineY[sel];
		int clr = ((SD_GetTimeCount() >> 4) & 1) ? 12 : 1;
		for (int pass = 0; pass < 2; ++pass)
		{
			VHB_HLine(x1, x2, y, clr);
			VHB_HLine(x1, x2, y + 1, clr);
			VHB_HLine(x1, x2, y + 12, clr);
			VHB_HLine(x1, x2, y + 13, clr);
			VHB_VLine(y + 1, y + 11, x1, clr);
			VHB_VLine(y + 1, y + 11, x1 + 1, clr);
			VHB_VLine(y + 1, y + 11, x2, clr);
			VHB_VLine(y + 1, y + 11, x2 - 1, clr);
			if (pass == 0)
				VH_UpdateScreen();
			clr = 15; // Erase it again for the next frame.
		}

		IN_ScanCode scan = IN_GetLastScan();
		IN_ReadControls(0, &ck_inputFrame);
		if (scan == IN_SC_Escape)
			break;
		if (ck_inputFrame.jump || ck_inputFrame.pogo || scan == IN_SC_Enter || scan == IN_SC_Space)
		{
			result = kw_destinations[available[sel]].episode;
			break;
		}
		if ((ck_inputFrame.yDirection == -1 || scan == IN_SC_UpArrow) && sel > 0)
			sel--;
		else if ((ck_inputFrame.yDirection == 1 || scan == IN_SC_DownArrow) && sel < numAvailable - 1)
			sel++;
		if (scan != IN_SC_None)
			IN_SetLastScan(IN_SC_None);
		// Don't race through the list while a direction is held.
		while (ck_inputFrame.yDirection != 0)
		{
			IN_PumpEvents();
			IN_ReadControls(0, &ck_inputFrame);
		}
	}

	KW_WaitForButtonsUp();
	return result;
}

void KW_PlayLoopHook(void)
{
	if (ca_mapOn != 0)
	{
		kw_launchRequested = false;
		return;
	}

	if (kw_launchRequested)
	{
		kw_launchRequested = false;
		int destination = KW_TravelMenu();
		RF_ForceRefresh();
		SD_SetLastTimeCount(SD_GetTimeCount());
		if (destination)
			KW_RequestWorld(destination);
		return;
	}

	// Debug shortcut: hold W and press 4, 5 or 6.
	if (!IN_GetKeyState(IN_SC_W))
		return;
	if (IN_GetKeyState(IN_SC_Four))
		KW_RequestWorld(4);
	else if (IN_GetKeyState(IN_SC_Five))
		KW_RequestWorld(5);
	else if (IN_GetKeyState(IN_SC_Six))
		KW_RequestWorld(6);
}

// Developer self-test (/KWTEST): hop between all worlds several times, loading
// the world map and two levels each visit, and log memory use after every hop
// so leaks show up as steady growth.
void KW_SelfTest(void)
{
	static const int route[] = {5, 6, 4, 6, 5, 4};
	CK_NewGame();
	// AdLib effects sit after the PC ones, so this mode catches stale offsets.
	SD_SetSoundMode(sdm_AdLib);
	CA_LoadAllSounds();
	ck_gameState.difficulty = D_Normal;

	// Items from two different games must travel with Keen.
	ck_gameState.ep.ck4.wetsuit = 1;
	ck_gameState.ep.ck6.sandwich = 1;

	for (int round = 0; round < 4; ++round)
	{
		for (int i = 0; i < (int)(sizeof(route) / sizeof(route[0])); ++i)
		{
			KW_SwitchWorld(route[i]);
			for (int level = 0; level <= 2; ++level)
			{
				ck_gameState.currentLevel = level;
				CK_LoadLevel(true, true);
			}
			ck_gameState.currentLevel = 0;
			CK_LoadLevel(true, true);
			// Every sound effect must be playable in the new world.
			for (int sound = 0; sound < ca_audInfoE.numSounds; ++sound)
				SD_PlaySound((soundnames)sound);
			SD_StopSound();
			CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: round %d, Keen %d, map %d, %d bytes in %d blocks\n",
				round, route[i], ca_mapOn, MM_UsedMemory(), MM_UsedBlocks());
		}
	}

	if (!ck_gameState.ep.ck4.wetsuit || !ck_gameState.ep.ck6.sandwich)
		Quit("KW_SelfTest: Keen lost his items while travelling!");
	CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: passed\n");
}
