/*
Keen Worlds: travelling between Keen 4, 5 and 6 in one running game.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "id_ca.h"
#include "id_fs.h"
#include "id_in.h"
#include "id_mm.h"
#include "id_rf.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_sd.h"
#include "id_ti.h"
#include "ck_act.h"
#include "ck_config.h"
#include "ck_cross.h"
#include "ck_game.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "ck_def.h"
#include "ck_ep.h"
#include "ck4_ep.h"
#include "ck5_ep.h"
#include "ck6_ep.h"
#include "kw_world.h"
#include "kw_art.h"
#include "kw_habitat.h"

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
	bool completed; // The world's ending has been seen.
} KW_WorldState;

static KW_WorldState kw_worlds[3];

int kw_pendingWorld = 0;
static bool kw_placeArrivalAtShip = false;

// Existing BWB Megarocket levels. Keen 5 has no equivalent level: its ship
// remains docked on the world map.
#define KW_CK4_SHIP_LEVEL 18
#define KW_CK6_SHIP_LEVEL 17

// Where cross-world arrivals stand in those levels: at the ship's nose. The
// hull at Keen's height is drawn in front of sprites (misc bit 0x80), so
// standing under the ship would hide him; this is the closest visible spot.
#define KW_CK4_ARRIVAL_X 3
#define KW_CK4_ARRIVAL_Y 12
#define KW_CK6_ARRIVAL_X 4
#define KW_CK6_ARRIVAL_Y 22

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

void KW_AfterWorldEnding(void)
{
	// help_endgame() pushes a cache level and, as in the original, never
	// pops it: the original quit to the title afterwards. Pop it here.
	CA_DownLevel();

	kw_worlds[ck_currentEpisode->ep - EP_CK4].completed = true;
	if (!ck_gameState.levelsDone[ca_mapOn])
		ck_gameState.levelsDone[ca_mapOn] = 1;
	ck_gameState.currentLevel = 0;
}

bool KW_IsWorldCompleted(int episode)
{
	if (episode < 4 || episode > 6)
		return false;
	return kw_worlds[episode - 4].completed;
}

// ---- Saved games ----

#define KW_SAVE_BLOCK_ID "KWLD"
#define KW_SAVE_VERSION 1

typedef struct KW_SaveBlock
{
	int16_t episode;
	KW_WorldState worlds[3];
	int16_t items[9];
} KW_SaveBlock;

const char *kw_loadingFileName = NULL;
static int kw_deferredEpisode = 0;
static char kw_deferredFile[FS_MAX_FILENAME_LEN];

// Every episode item, in a fixed order, so the block holds all of them.
static int16_t *KW_ItemField(int i)
{
	int16_t *fields[] = {
		&ck_gameState.ep.ck4.wetsuit, &ck_gameState.ep.ck4.membersRescued,
		&ck_gameState.ep.ck5.securityCard, &ck_gameState.ep.ck5.word_4729C, &ck_gameState.ep.ck5.fusesRemaining,
		&ck_gameState.ep.ck6.sandwich, &ck_gameState.ep.ck6.rope, &ck_gameState.ep.ck6.passcard, &ck_gameState.ep.ck6.inRocket,
	};
	return fields[i];
}

static bool KW_WriteWorld(FS_File fp, KW_WorldState *w)
{
	int16_t visited = w->visited, completed = w->completed;
	return FS_WriteInt16LE(&visited, 1, fp) == 1 &&
		FS_WriteInt16LE(&w->mapPosX, 1, fp) == 1 &&
		FS_WriteInt16LE(&w->mapPosY, 1, fp) == 1 &&
		FS_WriteInt16LE(w->levelsDone, 25, fp) == 25 &&
		FS_WriteInt16LE(&completed, 1, fp) == 1;
}

static bool KW_ReadWorld(FS_File fp, KW_WorldState *w)
{
	int16_t visited, completed;
	if (FS_ReadInt16LE(&visited, 1, fp) != 1 ||
		FS_ReadInt16LE(&w->mapPosX, 1, fp) != 1 ||
		FS_ReadInt16LE(&w->mapPosY, 1, fp) != 1 ||
		FS_ReadInt16LE(w->levelsDone, 25, fp) != 25 ||
		FS_ReadInt16LE(&completed, 1, fp) != 1)
		return false;
	w->visited = visited;
	w->completed = completed;
	return true;
}

bool KW_WriteSaveBlock(FS_File fp)
{
	int16_t version = KW_SAVE_VERSION, episode = ck_currentEpisode->ep - EP_CK4 + 4;

	// The current world's live progress is in ck_gameState, not kw_worlds.
	KW_WorldState current = kw_worlds[episode - 4];
	current.visited = true;
	current.mapPosX = ck_gameState.mapPosX;
	current.mapPosY = ck_gameState.mapPosY;
	memcpy(current.levelsDone, ck_gameState.levelsDone, sizeof(current.levelsDone));

	if (FS_Write(KW_SAVE_BLOCK_ID, 4, 1, fp) != 1 ||
		FS_WriteInt16LE(&version, 1, fp) != 1 ||
		FS_WriteInt16LE(&episode, 1, fp) != 1)
		return false;
	for (int i = 0; i < 3; ++i)
		if (!KW_WriteWorld(fp, i == episode - 4 ? &current : &kw_worlds[i]))
			return false;
	for (int i = 0; i < 9; ++i)
		if (FS_WriteInt16LE(KW_ItemField(i), 1, fp) != 1)
			return false;
	return true;
}

int KW_ReadSaveBlock(FS_File fp, bool *deferred)
{
	char id[4];
	int16_t version;
	KW_SaveBlock block;
	*deferred = false;

	if (FS_Read(id, 4, 1, fp) != 1 || memcmp(id, KW_SAVE_BLOCK_ID, 4) ||
		FS_ReadInt16LE(&version, 1, fp) != 1 || version != KW_SAVE_VERSION ||
		FS_ReadInt16LE(&block.episode, 1, fp) != 1 || !KW_EpisodeDef(block.episode))
		return 0;
	for (int i = 0; i < 3; ++i)
		if (!KW_ReadWorld(fp, &block.worlds[i]))
			return 0;
	if (FS_ReadInt16LE(block.items, 9, fp) != 9)
		return 0;

	if (KW_EpisodeDef(block.episode) != ck_currentEpisode)
	{
		// Finish after the menu has closed, in the save's own world.
		if (!kw_loadingFileName)
			return 0;
		CK_Cross_strscpy(kw_deferredFile, kw_loadingFileName, sizeof(kw_deferredFile));
		kw_deferredEpisode = block.episode;
		*deferred = true;
		return block.episode;
	}

	memcpy(kw_worlds, block.worlds, sizeof(kw_worlds));
	for (int i = 0; i < 9; ++i)
		*KW_ItemField(i) = block.items[i];
	kw_pendingWorld = 0;
	kw_placeArrivalAtShip = false;
	return block.episode;
}

bool KW_FinishDeferredLoad(void)
{
	if (!kw_deferredEpisode)
		return false;
	int episode = kw_deferredEpisode;
	kw_deferredEpisode = 0;

	KW_SwitchWorld(episode);
	kw_placeArrivalAtShip = false; // The save says where Keen is.

	// Same layout as US_LoadMain(): the slot header, then the game.
	FS_File fp = FS_OpenUserFile(kw_deferredFile);
	US_Savefile header;
	uint8_t padding;
	bool ok = FS_IsFileValid(fp) &&
		FS_Read(header.id, sizeof(header.id), 1, fp) == 1 &&
		FS_ReadInt16LE(&header.printXOffset, 1, fp) == 1 &&
		FS_ReadBoolFrom16LE(&header.used, 1, fp) == 1 &&
		FS_Read(header.name, sizeof(header.name), 1, fp) == 1 &&
		FS_Read(&padding, sizeof(padding), 1, fp) == 1 &&
		CK_LoadGame(fp, false);
	if (FS_IsFileValid(fp))
		FS_CloseFile(fp);
	if (!ok)
		QuitF("Couldn't load saved game %s", kw_deferredFile);
	ck_gameState.levelState = LS_LoadedGame;
	return true;
}

// ---- New games ----

static int kw_startWorld = 4;

void KW_SetStartWorld(void)
{
	kw_startWorld = ck_currentEpisode->ep - EP_CK4 + 4;
}

void KW_ReturnToStartWorld(void)
{
	// CK_NewGame() has already cleared Keen's progress (and /TEDLEVEL may
	// have chosen a level since), so only the world needs changing.
	if (KW_EpisodeDef(kw_startWorld) != ck_currentEpisode)
	{
		KW_SwitchWorld(kw_startWorld);
		ck_gameState.currentLevel = 0; // the map, not the ship arrival
	}
	KW_ResetWorlds();
	kw_placeArrivalAtShip = false;
}

void KW_ResetWorlds(void)
{
	memset(kw_worlds, 0, sizeof(kw_worlds));
	kw_pendingWorld = 0;
	kw_placeArrivalAtShip = false;
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

	// Remember where Keen was in the world he is leaving. While he is inside a
	// ship level, mapPos is already the world-map position of that level's
	// entrance; do not replace it with an in-level coordinate.
	KW_WorldState *from = KW_StateFor(ck_currentEpisode);
	from->visited = true;
	if (ca_mapOn == 0)
	{
		from->mapPosX = ck_gameState.mapPosX;
		from->mapPosY = ck_gameState.mapPosY;
	}
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

	// Arrive by boarding the destination's ship. Keen 4 and 6 already have
	// playable Megarocket levels; Keen 5 arrives at its docked ship on map 0.
	if (target->ep == EP_CK4)
	{
		ck_gameState.currentLevel = KW_CK4_SHIP_LEVEL;
		kw_placeArrivalAtShip = true;
	}
	else if (target->ep == EP_CK6)
	{
		ck_gameState.currentLevel = KW_CK6_SHIP_LEVEL;
		kw_placeArrivalAtShip = true;
	}
	else
	{
		ck_gameState.currentLevel = 0;
		kw_placeArrivalAtShip = false;
	}
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

// ---- Keen 5 boarding gate ----
// Art drawn by tools/kw_gate_art.py, placed by kw_art.c. It covers map tiles
// 29-31 x 94-96: the dock edge, the gap and the ship's nose. Three animation
// frames run the lights; Keen walks along the middle row.

#include "kw_gate_art.h"

// The solid gangway knob is replaced, not drawn over.
static bool KW_GateDropsUnderlay(int x, int y)
{
	return y == 95 && (x == 29 || x == 30);
}

static const KW_Art kw_gateArt = {
	KW_GATE_ORIGIN_X, KW_GATE_ORIGIN_Y, KW_GATE_TILES_W, KW_GATE_TILES_H, KW_GATE_FRAMES,
	&kw_gateSlots[0][0][0], &kw_gateTiles[0][0][0][0],
	1,  // walkway row
	12, // tics per frame
	KW_GateDropsUnderlay,
};

static void KW_BuildKeen5Gate(void)
{
	KW_PlaceArt(&kw_gateArt);
}

void KW_SetLaunchSpot(int tileX, int tileY)
{
	if (ck_currentEpisode->ep == EP_CK5)
	{
		// Replace Keen 5's unused yellow map border with a sparse starfield
		// tile already used beside the dock. This affects only the loaded map.
		for (int y = 0; y < CA_GetMapHeight(); ++y)
			for (int x = 0; x < CA_GetMapWidth(); ++x)
				if (CA_TileAtPos(x, y, 0) == 0x352)
					CA_SetTileAtPos(x, y, 0, 0x327);

		// The boarding gate between the dock and the ship (our own art).
		KW_BuildKeen5Gate();

		// The gangway ends at Keen's docked ship, three tiles east of the
		// normal map-Keen spawn.
		kw_launchSpotX = tileX + 3;
		kw_launchSpotY = tileY;
	}
	else
	{
		// Keen 4 and 6 use their playable Megarocket levels, not map tiles.
		kw_launchSpotX = kw_launchSpotY = -1;
	}
}

void KW_PlaceArrivalAtShip(void)
{
	if (!kw_placeArrivalAtShip || !ck_keenObj)
		return;

	int tileX, tileY;
	if (ck_currentEpisode->ep == EP_CK4 && ca_mapOn == KW_CK4_SHIP_LEVEL)
	{
		tileX = KW_CK4_ARRIVAL_X;
		tileY = KW_CK4_ARRIVAL_Y;
	}
	else if (ck_currentEpisode->ep == EP_CK6 && ca_mapOn == KW_CK6_SHIP_LEVEL)
	{
		tileX = KW_CK6_ARRIVAL_X;
		tileY = KW_CK6_ARRIVAL_Y;
	}
	else
	{
		return;
	}

	ck_keenObj->posX = RF_TileToUnit(tileX);
	// Match CK_SpawnKeen: its origin is 241 units above the floor tile.
	// Placing the origin directly at tileY embeds Keen in the ground.
	ck_keenObj->posY = RF_TileToUnit(tileY) - 241;
	// Cross-world arrivals face the ship. Normal entries retain their
	// episode-defined facing.
	ck_keenObj->xDirection = IN_motion_Right;
	CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenStanding));
	CK_ResetClipRects(ck_keenObj);
	kw_placeArrivalAtShip = false;
}

void KW_ConstrainKeen5Dock(CK_object *mapKeen)
{
	if (ck_currentEpisode->ep != EP_CK5 || ca_mapOn != 0)
		return;

	int x = mapKeen->posX >> 8;
	// Once Keen is on the bridge, it is a horizontal boarding route only.
	if (x >= 29)
		mapKeen->yDirection = IN_motion_None;
	// Stop at the docked ship; the player can still walk left back inside.
	if (x >= 31 && mapKeen->xDirection == IN_motion_Right)
		mapKeen->xDirection = IN_motion_None;
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

void KW_WaitForButtonsUp(void)
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

static bool KW_IsShipLevel(void)
{
	return (ck_currentEpisode->ep == EP_CK4 && ca_mapOn == KW_CK4_SHIP_LEVEL) ||
		(ck_currentEpisode->ep == EP_CK6 && ca_mapOn == KW_CK6_SHIP_LEVEL);
}

static bool KW_IsAtShipHatch(void)
{
	if (!ck_keenObj)
		return false;

	int x = ck_keenObj->clipRects.tileXmid;
	int y = (ck_keenObj->clipRects.tileY1 + ck_keenObj->clipRects.tileY2) / 2;
	if (ck_currentEpisode->ep == EP_CK4)
		return x >= 3 && x <= 19 && y >= 9 && y <= 14;
	if (ck_currentEpisode->ep == EP_CK6)
		return x >= 3 && x <= 18 && y >= 18 && y <= 23;
	return false;
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
	if (ca_mapOn != 0 && !KW_IsShipLevel())
	{
		kw_launchRequested = false;
		return;
	}
	if (KW_IsShipLevel() &&
		(ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed) &&
		KW_IsAtShipHatch())
		kw_launchRequested = true;

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
			if ((route[i] == EP_CK4 && ck_gameState.currentLevel != KW_CK4_SHIP_LEVEL) ||
				(route[i] == EP_CK5 && ck_gameState.currentLevel != 0) ||
				(route[i] == EP_CK6 && ck_gameState.currentLevel != KW_CK6_SHIP_LEVEL))
				Quit("KW_SelfTest: wrong arrival location!");
			CK_LoadLevel(true, true);
			if ((route[i] == EP_CK4 && (ck_keenObj->posX != RF_TileToUnit(KW_CK4_ARRIVAL_X) || ck_keenObj->posY != RF_TileToUnit(KW_CK4_ARRIVAL_Y) - 241)) ||
				(route[i] == EP_CK6 && (ck_keenObj->posX != RF_TileToUnit(KW_CK6_ARRIVAL_X) || ck_keenObj->posY != RF_TileToUnit(KW_CK6_ARRIVAL_Y) - 241)))
				Quit("KW_SelfTest: wrong ship arrival spawn!");
			if (route[i] == EP_CK5)
			{
				// The boarding gate must be on the map, drawable and walkable.
				if (!KW_IsArtPlaced(&kw_gateArt))
					Quit("KW_SelfTest: Keen 5 gate not placed or a tile is missing!");
				for (int c = 0; c < KW_GATE_TILES_W; ++c)
				{
					uint16_t t = kw_gateSlots[0][1][c];
					if (TI_ForeTop(t) || TI_ForeBottom(t) || TI_ForeLeft(t) || TI_ForeRight(t) || TI_ForeMisc(t))
						Quit("KW_SelfTest: Keen 5 gate walkway is blocked!");
					// The frames must be linked as a foreground animation.
					if (TI_ForeAnimTile(t) != kw_gateSlots[1][1][c] - t || !TI_ForeAnimTime(t))
						Quit("KW_SelfTest: Keen 5 gate lights are not animated!");
				}
			}
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

	// Ending behaviour: help_endgame() pushes a cache level; afterwards the
	// run must continue on the map with the cache level restored.
	{
		int levelBefore = ca_levelnum;
		ck_gameState.currentLevel = 3;
		CK_LoadLevel(true, true);
		CA_UpLevel(); // as help_endgame() does
		KW_AfterWorldEnding();
		if (ca_levelnum != levelBefore || ck_gameState.currentLevel != 0 ||
			!KW_IsWorldCompleted(ck_currentEpisode->ep - EP_CK4 + 4) || !ck_gameState.levelsDone[3])
			Quit("KW_SelfTest: ending did not return to the map properly!");
		CK_LoadLevel(true, true);
		if (ca_mapOn != 0)
			Quit("KW_SelfTest: map did not load after an ending!");
	}

	// Saved games: one shared slot list; a save made in one world loads from
	// any other (postponed until the world has switched).
	{
		const char *testFile = "KWTEST.SAV";
		KW_SwitchWorld(5);
		ck_gameState.currentLevel = 0;
		CK_LoadLevel(true, true);
		ck_gameState.keenScore = 12345;
		ck_gameState.ep.ck6.rope = 1;
		uint16_t savedMapX = ck_gameState.mapPosX;
		bool completed[3];
		for (int w = 0; w < 3; ++w)
			completed[w] = KW_IsWorldCompleted(w + 4);

		for (int pass = 0; pass < 2; ++pass)
		{
			FS_File fp = FS_CreateUserFile(testFile);
			US_Savefile header = {KW_SAVE_TAG, KW_SAVE_MAGIC, true, "Self-test"};
			uint8_t padding = 0;
			if (!FS_IsFileValid(fp) ||
				FS_Write(header.id, sizeof(header.id), 1, fp) != 1 ||
				FS_WriteInt16LE(&header.printXOffset, 1, fp) != 1 ||
				FS_WriteBoolTo16LE(&header.used, 1, fp) != 1 ||
				FS_Write(header.name, sizeof(header.name), 1, fp) != 1 ||
				FS_Write(&padding, 1, 1, fp) != 1 ||
				!CK_SaveGame(fp))
				Quit("KW_SelfTest: couldn't write a save!");
			FS_CloseFile(fp);

			// Pass 0 loads from Keen 6 (postponed); pass 1 from Keen 5 itself.
			if (pass == 0)
			{
				KW_SwitchWorld(6);
				ck_gameState.currentLevel = 0;
				CK_LoadLevel(true, true);
			}
			ck_gameState.keenScore = 0;
			ck_gameState.ep.ck6.rope = 0;

			fp = FS_OpenUserFile(testFile);
			kw_loadingFileName = testFile;
			bool ok = FS_IsFileValid(fp) &&
				FS_Read(header.id, sizeof(header.id), 1, fp) == 1 &&
				FS_ReadInt16LE(&header.printXOffset, 1, fp) == 1 &&
				FS_ReadBoolFrom16LE(&header.used, 1, fp) == 1 &&
				FS_Read(header.name, sizeof(header.name), 1, fp) == 1 &&
				FS_Read(&padding, 1, 1, fp) == 1 &&
				CK_LoadGame(fp, false);
			FS_CloseFile(fp);
			if (!ok)
				Quit("KW_SelfTest: couldn't read a save!");
			if (KW_FinishDeferredLoad() != (pass == 0))
				Quit("KW_SelfTest: cross-world load was not postponed (or same-world load was)!");

			if (ck_currentEpisode->ep != EP_CK5 || ca_mapOn != 0 || ck_gameState.keenScore != 12345 ||
				ck_gameState.ep.ck6.rope != 1 || ck_gameState.mapPosX != savedMapX)
				Quit("KW_SelfTest: save did not load back correctly!");
			for (int w = 0; w < 3; ++w)
				if (KW_IsWorldCompleted(w + 4) != completed[w])
					Quit("KW_SelfTest: save lost which worlds are completed!");
		}
		remove(testFile);
	}

	// Habitable levels: Border Village is peaceful once completed, with an
	// elder in the first house.
	{
		int hostiles, residents;
		CK_object *elder;
		KW_SwitchWorld(4);
		ck_gameState.currentLevel = 1;
		ck_gameState.levelsDone[1] = 0;
		CK_LoadLevel(true, true);
		KW_CountLevelPeople(&hostiles, &residents, &elder);
		if (!hostiles || residents || KW_IsLevelPeaceful())
			Quit("KW_SelfTest: an uncompleted Border Village should keep its enemies!");
		if (!KW_IsArtPlaced(&kw_stallArt) || KW_IsArtPlaced(&kw_barArt) || KW_IsArtPlaced(&kw_portraitArt))
			Quit("KW_SelfTest: before completion only the closed stall should be there!");
		ck_gameState.levelsDone[1] = 1;
		CK_LoadLevel(true, true);
		KW_CountLevelPeople(&hostiles, &residents, &elder);
		if (!KW_IsArtPlaced(&kw_portraitArt) || !KW_IsArtPlaced(&kw_barArt) || KW_IsArtPlaced(&kw_stallArt))
			Quit("KW_SelfTest: after completion the bar and portrait should be there, not the stall!");
		if (hostiles || residents != 1 || !KW_IsLevelPeaceful() ||
			RF_UnitToTile(elder->posX) != 4 || RF_UnitToTile(elder->posY) < 46 || RF_UnitToTile(elder->posY) > 48)
			Quit("KW_SelfTest: a completed Border Village should be peaceful with one elder in the house!");

		// ...and its entrance must stay open on the world map.
		ck_gameState.currentLevel = 0;
		CK_LoadLevel(true, true);
		bool entrance = false;
		for (int y = 0; y < CA_GetMapHeight(); ++y)
			for (int x = 0; x < CA_GetMapWidth(); ++x)
				if (CA_TileAtPos(x, y, 2) == 0xC001)
					entrance = true;
		if (!entrance)
			Quit("KW_SelfTest: completed Border Village can't be entered from the map!");
	}

	if (!ck_gameState.ep.ck4.wetsuit || !ck_gameState.ep.ck6.sandwich)
		Quit("KW_SelfTest: Keen lost his items while travelling!");
	CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: passed\n");
}
