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

// ---- The world registry ----

typedef struct KW_WorldDef
{
	const char *id;          // short id: save files, /WORLD
	const char *name;        // shown in the Megarocket menu
	int episode;             // the engine episode it runs on (4, 5, 6)
	const char *dir;         // own data folder under the game path, or NULL
	const char *episodeFile; // variable file, or NULL for the episode's own
} KW_WorldDef;

static const KW_WorldDef kw_worldDefs[KW_NUM_WORLDS] = {
	{"keen4", "Gnosticus IV", 4, NULL, NULL},
	{"keen5", "The Omegamatic", 5, NULL, NULL},
	{"keen6", "Fribbulus Xax", 6, NULL, NULL},
	// The Keys of Krodacia ("Keen 7", Ceilick), as converted for Omnispeak
	// in the Omnispeak modding kit (data/mod/keen7): the player copies that
	// folder to play/mods/keen7.
	{"keen7", "Krodacia", 4, "mods/keen7", "MOD.CK4"},
	// Dead in the Desert ("Keen 8", Ceilick and Levellass): the player's copy
	// of the mod's files in play/mods/keen8, with our MOD.CK6 (see there).
	{"keen8", "Calidune", 6, "mods/keen8", "MOD.CK6"},
};

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

static int kw_currentWorld = KW_KEEN4;

int KW_CurrentWorld(void)
{
	return kw_currentWorld;
}

const char *KW_WorldName(int world)
{
	return (world >= 0 && world < KW_NUM_WORLDS) ? kw_worldDefs[world].name : "";
}

const char *KW_WorldId(int world)
{
	return (world >= 0 && world < KW_NUM_WORLDS) ? kw_worldDefs[world].id : "";
}

// The world's data folder (under the game path), or NULL.
static const char *KW_WorldPath(int world)
{
	static char path[KW_NUM_WORLDS][256];
	const KW_WorldDef *def = &kw_worldDefs[world];
	if (!def->dir)
		return NULL;
	snprintf(path[world], sizeof(path[world]), "%s/%s", fs_keenPath, def->dir);
	return path[world];
}

bool KW_IsWorldPresent(int world)
{
	if (world < 0 || world >= KW_NUM_WORLDS)
		return false;
	const KW_WorldDef *def = &kw_worldDefs[world];
	CK_EpisodeDef *ep = KW_EpisodeDef(def->episode);
	if (!ep)
		return false;
	if (!def->dir)
		return ep->isPresent();
	// A fan game: its folder must hold its variable file (its levels,
	// graphics and sounds come with it; Omnispeak's support files for the
	// base episode come from the game path).
	FS_File f = FSL_OpenFileInDirCaseInsensitive(KW_WorldPath(world), def->episodeFile, false);
	if (!FS_IsFileValid(f))
		return false;
	FS_CloseFile(f);
	return FS_IsOmniFilePresent(ep->episodeFile);
}

// What each world remembers while Keen is somewhere else: where he was on
// its map, which levels are done, whether its ending was seen, and its story
// items (wetsuit, keycard, sandwich...). The items belong to their world:
// worlds sharing an engine episode (Keen 4 and Keen 7) use the same item
// fields, so sharing them would mix up their stories. Score, lives and ammo
// travel with Keen.
#define KW_NUM_ITEMS 9

typedef struct KW_WorldState
{
	bool visited;
	uint16_t mapPosX;
	uint16_t mapPosY;
	uint16_t levelsDone[25];
	bool completed; // The world's ending has been seen.
	int16_t items[KW_NUM_ITEMS];
} KW_WorldState;

static KW_WorldState kw_worlds[KW_NUM_WORLDS];

int kw_pendingWorld = KW_NO_WORLD;
static bool kw_placeArrivalAtShip = false;

// Worlds with a playable BWB Megarocket level: Keen boards inside it and
// arrives there. Keen 5 has none (its ship is docked on the world map).
// Arrivals stand at the ship's nose: the hull at Keen's height is drawn in
// front of sprites (misc bit 0x80), so standing under the ship would hide
// him. The boarding area covers the visible ship.
typedef struct KW_Ship
{
	int world;
	int level;
	int arrivalX, arrivalY;          // as a Keen spawn marker
	int boardX1, boardY1, boardX2, boardY2;
} KW_Ship;

static const KW_Ship kw_ships[] = {
	{KW_KEEN4, 18, 3, 12, 3, 9, 19, 14},
	{KW_KEEN6, 17, 4, 22, 3, 18, 18, 23},
	{KW_KEEN7, 18, 3, 15, 3, 12, 19, 17}, // Keen 4's ship, three rows lower
};

static const KW_Ship *KW_ShipFor(int world)
{
	for (int i = 0; i < (int)(sizeof(kw_ships) / sizeof(kw_ships[0])); ++i)
		if (kw_ships[i].world == world)
			return &kw_ships[i];
	return NULL;
}

// Every episode item field, in a fixed order.
static int16_t *KW_ItemField(int i)
{
	int16_t *fields[KW_NUM_ITEMS] = {
		&ck_gameState.ep.ck4.wetsuit, &ck_gameState.ep.ck4.membersRescued,
		&ck_gameState.ep.ck5.securityCard, &ck_gameState.ep.ck5.word_4729C, &ck_gameState.ep.ck5.fusesRemaining,
		&ck_gameState.ep.ck6.sandwich, &ck_gameState.ep.ck6.rope, &ck_gameState.ep.ck6.passcard, &ck_gameState.ep.ck6.inRocket,
	};
	return fields[i];
}

// The current world's live state (in ck_gameState) as a world record.
static KW_WorldState KW_LiveState(void)
{
	KW_WorldState w = kw_worlds[kw_currentWorld];
	w.visited = true;
	// mapPos is always a world-map position (inside a level: its entrance).
	w.mapPosX = ck_gameState.mapPosX;
	w.mapPosY = ck_gameState.mapPosY;
	memcpy(w.levelsDone, ck_gameState.levelsDone, sizeof(w.levelsDone));
	for (int i = 0; i < KW_NUM_ITEMS; ++i)
		w.items[i] = *KW_ItemField(i);
	return w;
}

static void KW_LoadState(const KW_WorldState *w)
{
	ck_gameState.mapPosX = w->mapPosX;
	ck_gameState.mapPosY = w->mapPosY;
	memcpy(ck_gameState.levelsDone, w->levelsDone, sizeof(w->levelsDone));
	for (int i = 0; i < KW_NUM_ITEMS; ++i)
		*KW_ItemField(i) = w->items[i];
}

void KW_AfterWorldEnding(void)
{
	// help_endgame() pushes a cache level and, as in the original, never
	// pops it: the original quit to the title afterwards. Pop it here.
	CA_DownLevel();

	kw_worlds[kw_currentWorld].completed = true;
	if (!ck_gameState.levelsDone[ca_mapOn])
		ck_gameState.levelsDone[ca_mapOn] = 1;
	ck_gameState.currentLevel = 0;
}

bool KW_IsWorldCompleted(int world)
{
	if (world < 0 || world >= KW_NUM_WORLDS)
		return false;
	return kw_worlds[world].completed;
}

// ---- Saved games ----
// Version 2: the current world's id, then each world's record by id (so a
// save stays valid when fan games are added or removed). Version 1 (Keen
// 4-6 only, items shared) is still read.

#define KW_SAVE_BLOCK_ID "KWLD"
#define KW_SAVE_VERSION 2
#define KW_SAVE_ID_LEN 8

const char *kw_loadingFileName = NULL;
static int kw_deferredWorld = KW_NO_WORLD;
static char kw_deferredFile[FS_MAX_FILENAME_LEN];

static int KW_WorldById(const char *id)
{
	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (!strncmp(kw_worldDefs[w].id, id, KW_SAVE_ID_LEN))
			return w;
	return KW_NO_WORLD;
}

static bool KW_WriteWorld(FS_File fp, const KW_WorldState *w)
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

static bool KW_WriteId(FS_File fp, const char *id)
{
	char buf[KW_SAVE_ID_LEN] = {0};
	strncpy(buf, id, KW_SAVE_ID_LEN);
	return FS_Write(buf, KW_SAVE_ID_LEN, 1, fp) == 1;
}

bool KW_WriteSaveBlock(FS_File fp)
{
	int16_t version = KW_SAVE_VERSION, count = KW_NUM_WORLDS;
	if (FS_Write(KW_SAVE_BLOCK_ID, 4, 1, fp) != 1 ||
		FS_WriteInt16LE(&version, 1, fp) != 1 ||
		!KW_WriteId(fp, kw_worldDefs[kw_currentWorld].id) ||
		FS_WriteInt16LE(&count, 1, fp) != 1)
		return false;
	for (int w = 0; w < KW_NUM_WORLDS; ++w)
	{
		KW_WorldState state = (w == kw_currentWorld) ? KW_LiveState() : kw_worlds[w];
		if (!KW_WriteId(fp, kw_worldDefs[w].id) || !KW_WriteWorld(fp, &state) ||
			FS_WriteInt16LE(state.items, KW_NUM_ITEMS, fp) != KW_NUM_ITEMS)
			return false;
	}
	return true;
}

bool KW_ReadSaveBlock(FS_File fp, bool *deferred)
{
	char id[4];
	int16_t version;
	KW_WorldState worlds[KW_NUM_WORLDS];
	int world = KW_NO_WORLD;
	memset(worlds, 0, sizeof(worlds));
	*deferred = false;

	if (FS_Read(id, 4, 1, fp) != 1 || memcmp(id, KW_SAVE_BLOCK_ID, 4) ||
		FS_ReadInt16LE(&version, 1, fp) != 1)
		return false;

	if (version == 1)
	{
		// Keen 4-6 only; items were shared, so every world gets the set.
		int16_t episode, items[KW_NUM_ITEMS];
		if (FS_ReadInt16LE(&episode, 1, fp) != 1 || episode < 4 || episode > 6)
			return false;
		for (int w = 0; w < 3; ++w)
			if (!KW_ReadWorld(fp, &worlds[w]))
				return false;
		if (FS_ReadInt16LE(items, KW_NUM_ITEMS, fp) != KW_NUM_ITEMS)
			return false;
		for (int w = 0; w < 3; ++w)
			memcpy(worlds[w].items, items, sizeof(items));
		world = KW_KEEN4 + (episode - 4);
	}
	else if (version == KW_SAVE_VERSION)
	{
		char wid[KW_SAVE_ID_LEN];
		int16_t count;
		if (FS_Read(wid, KW_SAVE_ID_LEN, 1, fp) != 1 || FS_ReadInt16LE(&count, 1, fp) != 1)
			return false;
		world = KW_WorldById(wid);
		for (int i = 0; i < count; ++i)
		{
			KW_WorldState state;
			memset(&state, 0, sizeof(state));
			if (FS_Read(wid, KW_SAVE_ID_LEN, 1, fp) != 1 || !KW_ReadWorld(fp, &state) ||
				FS_ReadInt16LE(state.items, KW_NUM_ITEMS, fp) != KW_NUM_ITEMS)
				return false;
			int w = KW_WorldById(wid);
			if (w != KW_NO_WORLD)
				worlds[w] = state;   // worlds that are no longer known are dropped
		}
	}
	else
		return false;

	// The save's world must be installed.
	if (world == KW_NO_WORLD || !KW_IsWorldPresent(world))
		return false;

	if (world != kw_currentWorld)
	{
		// Finish after the menu has closed, in the save's own world.
		if (!kw_loadingFileName)
			return false;
		CK_Cross_strscpy(kw_deferredFile, kw_loadingFileName, sizeof(kw_deferredFile));
		kw_deferredWorld = world;
		*deferred = true;
		return true;
	}

	memcpy(kw_worlds, worlds, sizeof(kw_worlds));
	// The current world's items come from the record; the rest of its state
	// (map position, levels done) from the normal save that follows.
	for (int i = 0; i < KW_NUM_ITEMS; ++i)
		*KW_ItemField(i) = kw_worlds[world].items[i];
	kw_pendingWorld = KW_NO_WORLD;
	kw_placeArrivalAtShip = false;
	return true;
}

bool KW_FinishDeferredLoad(void)
{
	if (kw_deferredWorld == KW_NO_WORLD)
		return false;
	int world = kw_deferredWorld;
	kw_deferredWorld = KW_NO_WORLD;

	KW_SwitchWorld(world);
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

static int kw_startWorld = KW_KEEN4;

void KW_SetStartWorld(void)
{
	// The running episode is one of the original worlds.
	kw_currentWorld = KW_KEEN4 + (ck_currentEpisode->ep - EP_CK4);
	kw_startWorld = kw_currentWorld;

	// /WORLD <id> starts new games in another world (e.g. a fan game).
	for (int i = 1; i < us_argc - 1; ++i)
	{
		if (CK_Cross_strcasecmp(us_argv[i], "/WORLD"))
			continue;
		int w = KW_WorldById(us_argv[i + 1]);
		if (w == KW_NO_WORLD || !KW_IsWorldPresent(w))
			QuitF("World \"%s\" is not installed.", us_argv[i + 1]);
		kw_startWorld = w;
	}
}

void KW_ReturnToStartWorld(void)
{
	// CK_NewGame() has already cleared Keen's progress (and /TEDLEVEL may
	// have chosen a level since), so only the world needs changing.
	if (kw_startWorld != kw_currentWorld)
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
	kw_pendingWorld = KW_NO_WORLD;
	kw_placeArrivalAtShip = false;
}

void KW_RequestWorld(int world)
{
	if (world == kw_currentWorld || !KW_IsWorldPresent(world))
		return;

	kw_pendingWorld = world;
	ck_gameState.levelState = LS_SwitchWorld;
}

void KW_SwitchWorld(int world)
{
	kw_pendingWorld = KW_NO_WORLD;
	if (world < 0 || world >= KW_NUM_WORLDS || world == kw_currentWorld)
		return;
	const KW_WorldDef *def = &kw_worldDefs[world];
	CK_EpisodeDef *target = KW_EpisodeDef(def->episode);
	if (!target)
		return;

	// Remember the world Keen is leaving.
	kw_worlds[kw_currentWorld] = KW_LiveState();

	// Nothing may keep playing from the old world's audio data.
	SD_MusicOff();
	SD_StopSound();

	// Swap the world: data files, then variables, then data files again
	// (their names come from the variables). A fan game's own folder is
	// searched first, so its levels, graphics and variable files replace the
	// base episode's.
	CA_UnloadEpisodeData();
	CK_VAR_Reset();
	fs_worldPath = KW_WorldPath(world);
	ck_currentEpisode = target;
	ck_episodeFile = def->episodeFile ? def->episodeFile : target->episodeFile;
	kw_currentWorld = world;
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
	KW_WorldState fresh;
	memset(&fresh, 0, sizeof(fresh));
	KW_LoadState(kw_worlds[world].visited ? &kw_worlds[world] : &fresh);

	// Arrive by the destination's ship: in its Megarocket level if it has
	// one, otherwise on its world map (Keen 5's docked ship, other games).
	const KW_Ship *ship = KW_ShipFor(world);
	ck_gameState.currentLevel = ship ? ship->level : 0;
	kw_placeArrivalAtShip = ship != NULL;
}

// ---- The Megarocket ----

static int kw_launchSpotX = -1, kw_launchSpotY = -1;
static bool kw_launchRequested = false;

// How far (in tiles) from the launch spot Keen may stand to board the rocket.
#define KW_LAUNCH_RANGE 1


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
	if (kw_currentWorld == KW_KEEN5)
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
	else if (KW_ShipFor(kw_currentWorld))
	{
		// Worlds with a Megarocket level board there, not on the map.
		kw_launchSpotX = kw_launchSpotY = -1;
	}
	else
	{
		// Others: the ship waits where Keen first lands on the world map.
		kw_launchSpotX = tileX;
		kw_launchSpotY = tileY;
	}
}

void KW_PlaceArrivalAtShip(void)
{
	if (!kw_placeArrivalAtShip || !ck_keenObj)
		return;

	const KW_Ship *ship = KW_ShipFor(kw_currentWorld);
	if (!ship || ca_mapOn != ship->level)
		return;
	int tileX = ship->arrivalX, tileY = ship->arrivalY;

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
	if (kw_currentWorld != KW_KEEN5 || ca_mapOn != 0)
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
	const KW_Ship *ship = KW_ShipFor(kw_currentWorld);
	return ship && ca_mapOn == ship->level;
}

static bool KW_IsAtShipHatch(void)
{
	if (!ck_keenObj)
		return false;

	int x = ck_keenObj->clipRects.tileXmid;
	int y = (ck_keenObj->clipRects.tileY1 + ck_keenObj->clipRects.tileY2) / 2;
	const KW_Ship *ship = KW_ShipFor(kw_currentWorld);
	return ship && x >= ship->boardX1 && x <= ship->boardX2 && y >= ship->boardY1 && y <= ship->boardY2;
}

// Draws the destination menu and returns the chosen world, or KW_NO_WORLD to
// stay. Only installed worlds are listed.
static int KW_TravelMenu(void)
{
	int lineY[KW_NUM_WORLDS];
	int available[KW_NUM_WORLDS];
	int numAvailable = 0;

	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (w != kw_currentWorld && KW_IsWorldPresent(w))
			available[numAvailable++] = w;
	if (!numAvailable)
		return KW_NO_WORLD;

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
		US_CPrint(kw_worldDefs[available[i]].name);
		US_SetPrintY(US_GetPrintY() + 4);
	}

	KW_WaitForButtonsUp();

	// Same blinking selection box as the "Try Again" menu.
	int sel = 0, result = KW_NO_WORLD;
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
			result = available[sel];
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
		if (destination != KW_NO_WORLD)
			KW_RequestWorld(destination);
		return;
	}

	// Debug shortcut: hold W and press 4, 5, 6, 7 or 8.
	if (!IN_GetKeyState(IN_SC_W))
		return;
	if (IN_GetKeyState(IN_SC_Four))
		KW_RequestWorld(KW_KEEN4);
	else if (IN_GetKeyState(IN_SC_Five))
		KW_RequestWorld(KW_KEEN5);
	else if (IN_GetKeyState(IN_SC_Six))
		KW_RequestWorld(KW_KEEN6);
	else if (IN_GetKeyState(IN_SC_Seven))
		KW_RequestWorld(KW_KEEN7);
	else if (IN_GetKeyState(IN_SC_Eight))
		KW_RequestWorld(KW_KEEN8);
}

// Developer self-test (/KWTEST): hop between all worlds several times, loading
// the world map and two levels each visit, and log memory use after every hop
// so leaks show up as steady growth.
void KW_SelfTest(void)
{
	static const int route[] = {KW_KEEN5, KW_KEEN6, KW_KEEN4, KW_KEEN7, KW_KEEN6, KW_KEEN5, KW_KEEN7, KW_KEEN4};
	bool haveKeen7 = KW_IsWorldPresent(KW_KEEN7);
	CK_NewGame();
	// AdLib effects sit after the PC ones, so this mode catches stale offsets.
	SD_SetSoundMode(sdm_AdLib);
	CA_LoadAllSounds();
	ck_gameState.difficulty = D_Normal;

	// Story items belong to their world: Keen 4's wetsuit must stay in Keen 4
	// (Keen 7 runs on the same episode code and item fields).
	if (KW_CurrentWorld() != KW_KEEN4)
		Quit("KW_SelfTest: run it with /EPISODE 4");
	ck_gameState.ep.ck4.wetsuit = 1;
	ck_gameState.keenScore = 777; // score travels with Keen

	for (int round = 0; round < 4; ++round)
	{
		for (int i = 0; i < (int)(sizeof(route) / sizeof(route[0])); ++i)
		{
			if (route[i] == KW_KEEN7 && !haveKeen7)
				continue;
			KW_SwitchWorld(route[i]);
			if (KW_CurrentWorld() != route[i] || ck_gameState.keenScore != 777)
				Quit("KW_SelfTest: didn't arrive in the requested world (or lost the score)!");
			const KW_Ship *ship = KW_ShipFor(route[i]);
			if (ck_gameState.currentLevel != (ship ? ship->level : 0))
				Quit("KW_SelfTest: wrong arrival location!");
			CK_LoadLevel(true, true);
			if (ship && (ck_keenObj->posX != RF_TileToUnit(ship->arrivalX) ||
					ck_keenObj->posY != RF_TileToUnit(ship->arrivalY) - 241 || !KW_IsAtShipHatch()))
				Quit("KW_SelfTest: wrong ship arrival spawn (or not at the ship)!");
			if ((route[i] == KW_KEEN4) != (ck_gameState.ep.ck4.wetsuit == 1))
				Quit("KW_SelfTest: Keen 4's wetsuit leaked into another world (or was lost)!");
			if (route[i] == KW_KEEN7)
			{
				// Keen 7 is a world of its own: none of Keen 4's or Keen 5's
				// Keen Worlds additions apply, and Keen boards on its map.
				ck_gameState.currentLevel = 0;
				CK_LoadLevel(true, true);
				if (KW_IsArtPlaced(&kw_gateArt) || kw_launchSpotX >= 0)
					Quit("KW_SelfTest: Keen 7's map got Keen 5's gate (or a map launch spot despite its ship level)!");
				for (int lv = 0; lv <= 18; ++lv)
				{
					ck_gameState.currentLevel = lv;
					ck_gameState.levelsDone[lv] = 1; // even "completed", nothing of Keen 4's appears
					CK_LoadLevel(true, true);
					int hostiles, residents;
					CK_object *first;
					KW_CountLevelPeople(&hostiles, &residents, &first);
					if (residents || KW_IsArtCellPlaced(&kw_innArt, 77, 15))
						Quit("KW_SelfTest: Keen 4's residents or tavern appeared in Keen 7!");
					if (lv && (!KW_IsLevelPeaceful() || hostiles))
						Quit("KW_SelfTest: a completed Keen 7 level isn't peaceful!");
				}
				memset(ck_gameState.levelsDone, 0, sizeof(ck_gameState.levelsDone));
			}
			// Every completed level is re-enterable and peaceful, in every
			// world (level 1 of each, marked done for the check).
			if (round == 0)
			{
				ck_gameState.currentLevel = 1;
				ck_gameState.levelsDone[1] = 1;
				CK_LoadLevel(true, true);
				int hostiles, residents;
				CK_object *first;
				KW_CountLevelPeople(&hostiles, &residents, &first);
				if (!KW_IsLevelPeaceful() || hostiles)
					Quit("KW_SelfTest: a completed level still has enemies!");
				ck_gameState.currentLevel = 0;
				CK_LoadLevel(true, true);
				bool entrance = false;
				for (int y = 0; y < CA_GetMapHeight(); ++y)
					for (int x = 0; x < CA_GetMapWidth(); ++x)
						if (CA_TileAtPos(x, y, 2) == 0xC001)
							entrance = true;
				if (!entrance)
					Quit("KW_SelfTest: a completed level lost its map entrance!");
				ck_gameState.levelsDone[1] = 0;

				// Every level of the world, once completed: Keen comes back in at
				// its exit (the opposite side, or Keen 5's exit door), standing,
				// facing into the level, and every gem door is open.
				for (int lv = 1; lv < CA_NUMMAPS; ++lv)
				{
					if (!CA_MapExists(lv))
						continue;
					ck_gameState.currentLevel = lv;
					ck_gameState.levelsDone[lv] = 0;
					CK_LoadLevel(true, true);
					int startX = RF_UnitToTile(ck_keenObj->posX);
					ck_gameState.levelsDone[lv] = 1;
					CK_LoadLevel(true, true);
					int x = RF_UnitToTile(ck_keenObj->posX), w = CA_GetMapWidth();
					int feet = ck_keenObj->clipRects.tileY2;
					bool atDoor = CA_TileAtPos(ck_keenObj->clipRects.tileX1, feet - 1, 2) == 0xB1B1;
					bool opposite = abs(x - startX) >= w / 3;
					KW_RevisitPlacement how = KW_LastRevisitPlacement();
					if (how == KW_REVISIT_SPECIAL || !strcmp(CA_MapHeaders[lv]->name, "HighScores"))
					{
						ck_gameState.levelsDone[lv] = 0;
						continue;
					}
					bool facingIn = how == KW_REVISIT_GOAL || (x < w / 2) == (ck_keenObj->xDirection == IN_motion_Right);
					bool standing = TI_ForeTop(CA_TileAtPos(ck_keenObj->clipRects.tileX1, feet + 1, 1)) ||
						TI_ForeTop(CA_TileAtPos(ck_keenObj->clipRects.tileX2, feet + 1, 1));
					int closedGems = 0;
					for (int gy = 0; gy < CA_GetMapHeight(); ++gy)
						for (int gx = 0; gx < w; ++gx)
						{
							int misc = TI_ForeMisc(CA_TileAtPos(gx, gy, 1)) & 0x7F;
							if (misc >= MISCFLAG_GEMHOLDER0 && misc <= MISCFLAG_GEMHOLDER3)
								++closedGems;
						}
					if (how == KW_REVISIT_KEPT || (how == KW_REVISIT_DOOR && !atDoor) || (how == KW_REVISIT_SIDE && !opposite) || !facingIn || !standing || closedGems)
					{
						CK_Cross_LogMessage(CK_LOG_MSG_ERROR, "KW_SelfTest: %s level %d: start %d -> %d,%d door=%d opposite=%d facingIn=%d standing=%d closedGems=%d (below: tile %d top %d) how=%d\n",
							KW_WorldName(KW_CurrentWorld()), lv, startX, x, feet, atDoor, opposite, facingIn, standing, closedGems,
							CA_TileAtPos(ck_keenObj->clipRects.tileX1, feet + 1, 1), TI_ForeTop(CA_TileAtPos(ck_keenObj->clipRects.tileX1, feet + 1, 1)), (int)how);
						Quit("KW_SelfTest: a completed level isn't entered from its exit, or isn't fully open!");
					}
					// ...and can leave where he first came in, walking back out.
					int ex, ey, efrom;
					KW_GetLevelEntry(&ex, &ey, &efrom);
					ck_keenObj->posX = RF_TileToUnit(ex);
					ck_keenObj->posY = RF_TileToUnit(ey) - 241;
					CK_ResetClipRects(ck_keenObj);
					ck_inputFrame.xDirection = (IN_Motion)efrom;
					ck_gameState.levelState = LS_Playing;
					KW_CheckPeacefulExit();
					ck_inputFrame.xDirection = IN_motion_None;
					if (ck_gameState.levelState != LS_LevelComplete)
						Quit("KW_SelfTest: a completed level can't be left where Keen first came in!");
					ck_gameState.levelState = LS_Playing;
					if (how == KW_REVISIT_ONLY_EXIT)
						CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: %s level %d has one exit, by its start\n",
							KW_WorldName(KW_CurrentWorld()), lv);
					ck_gameState.levelsDone[lv] = 0;
				}
				ck_gameState.currentLevel = 0;
				CK_LoadLevel(true, true);
			}
			if (route[i] == KW_KEEN4 && round == 0)
			{
				// A cleared level's rescued elder stays as a villager, never to
				// be rescued again (Perilous Pit, level 3, has one).
				ck_gameState.currentLevel = 3;
				ck_gameState.levelsDone[3] = 1;
				CK_LoadLevel(true, true);
				int elders = 0, villagers = 0;
				for (CK_object *o = ck_keenObj; o; o = o->next)
				{
					if (o->type == CT4_CouncilMember)
						++elders;
					else if (o->type == CT_Friendly && o->currentAction == CK_ACTION(CK4_ACT_CouncilWalk0))
						++villagers;
				}
				if (elders)
					Quit("KW_SelfTest: a cleared Keen 4 level has an elder to rescue again!");
				ck_gameState.levelsDone[3] = 0;
				(void)villagers;
			}
			if (route[i] == KW_KEEN5)
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
			CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: round %d, %s, map %d, %d bytes in %d blocks\n",
				round, KW_WorldId(route[i]), ca_mapOn, MM_UsedMemory(), MM_UsedBlocks());
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
			!KW_IsWorldCompleted(KW_CurrentWorld()) || !ck_gameState.levelsDone[3])
			Quit("KW_SelfTest: ending did not return to the map properly!");
		CK_LoadLevel(true, true);
		if (ca_mapOn != 0)
			Quit("KW_SelfTest: map did not load after an ending!");
	}

	// Saved games: one shared slot list; a save made in one world loads from
	// any other (postponed until the world has switched).
	{
		const char *testFile = "KWTEST.SAV";
		KW_SwitchWorld(KW_KEEN5);
		ck_gameState.currentLevel = 0;
		CK_LoadLevel(true, true);
		ck_gameState.keenScore = 12345;
		ck_gameState.ep.ck6.rope = 1;
		uint16_t savedMapX = ck_gameState.mapPosX;
		bool completed[KW_NUM_WORLDS];
		for (int w = 0; w < KW_NUM_WORLDS; ++w)
			completed[w] = KW_IsWorldCompleted(w);

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

			// Pass 0 loads from another world (postponed); pass 1 from Keen 5.
			if (pass == 0)
			{
				KW_SwitchWorld(haveKeen7 ? KW_KEEN7 : KW_KEEN6);
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

			if (KW_CurrentWorld() != KW_KEEN5 || ca_mapOn != 0 || ck_gameState.keenScore != 12345 ||
				ck_gameState.ep.ck6.rope != 1 || ck_gameState.mapPosX != savedMapX)
				Quit("KW_SelfTest: save did not load back correctly!");
			for (int w = 0; w < KW_NUM_WORLDS; ++w)
				if (KW_IsWorldCompleted(w) != completed[w])
					Quit("KW_SelfTest: save lost which worlds are completed!");
		}
		remove(testFile);

		// A save made in Keen 7 (a fan game) loads back into Keen 7, with
		// its own story items.
		if (haveKeen7)
		{
			KW_SwitchWorld(KW_KEEN7);
			ck_gameState.currentLevel = 0;
			CK_LoadLevel(true, true);
			ck_gameState.ep.ck4.membersRescued = 3;
			FS_File fp = FS_CreateUserFile(testFile);
			US_Savefile header = {KW_SAVE_TAG, KW_SAVE_MAGIC, true, "Keen 7"};
			uint8_t padding = 0;
			bool ok = FS_IsFileValid(fp) &&
				FS_Write(header.id, sizeof(header.id), 1, fp) == 1 &&
				FS_WriteInt16LE(&header.printXOffset, 1, fp) == 1 &&
				FS_WriteBoolTo16LE(&header.used, 1, fp) == 1 &&
				FS_Write(header.name, sizeof(header.name), 1, fp) == 1 &&
				FS_Write(&padding, 1, 1, fp) == 1 && CK_SaveGame(fp);
			FS_CloseFile(fp);
			KW_SwitchWorld(KW_KEEN4);
			ck_gameState.currentLevel = 0;
			CK_LoadLevel(true, true);
			if (ck_gameState.ep.ck4.membersRescued == 3)
				Quit("KW_SelfTest: Keen 7's rescued elders count leaked into Keen 4!");
			fp = FS_OpenUserFile(testFile);
			kw_loadingFileName = testFile;
			ok = ok && FS_IsFileValid(fp) &&
				FS_Read(header.id, sizeof(header.id), 1, fp) == 1 &&
				FS_ReadInt16LE(&header.printXOffset, 1, fp) == 1 &&
				FS_ReadBoolFrom16LE(&header.used, 1, fp) == 1 &&
				FS_Read(header.name, sizeof(header.name), 1, fp) == 1 &&
				FS_Read(&padding, 1, 1, fp) == 1 &&
				CK_LoadGame(fp, false);
			FS_CloseFile(fp);
			if (!ok || !KW_FinishDeferredLoad() || KW_CurrentWorld() != KW_KEEN7 ||
				ck_gameState.ep.ck4.membersRescued != 3)
				Quit("KW_SelfTest: a Keen 7 save didn't load back into Keen 7!");
			remove(testFile);
		}
	}

	// Keen 8 (when installed): every level loads, and every level when
	// completed is peaceful.
	if (KW_IsWorldPresent(KW_KEEN8))
	{
		KW_SwitchWorld(KW_KEEN8);
		int loaded = 0;
		for (int lv = 0; lv < CA_NUMMAPS; ++lv)
		{
			if (!CA_MapExists(lv))
				continue;
			ck_gameState.currentLevel = lv;
			ck_gameState.levelsDone[lv] = 0;
			CK_LoadLevel(true, true);
			ck_gameState.levelsDone[lv] = 1;
			CK_LoadLevel(true, true);
			int hostiles, residents;
			CK_object *first;
			KW_CountLevelPeople(&hostiles, &residents, &first);
			if (lv && (!KW_IsLevelPeaceful() || hostiles))
				Quit("KW_SelfTest: a completed Keen 8 level isn't peaceful!");
			CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: Keen 8 level %d \"%s\" %dx%d, re-entry %d\n", lv,
				CA_MapHeaders[lv]->name, CA_GetMapWidth(), CA_GetMapHeight(), (int)KW_LastRevisitPlacement());
			ck_gameState.levelsDone[lv] = 0;
			++loaded;
		}
		if (loaded < 18)
			Quit("KW_SelfTest: Keen 8 is missing levels!");
	}

	// Habitable levels: Border Village is peaceful once completed, with an
	// elder in the first house.
	{
		int hostiles, residents;
		CK_object *elder;
		KW_SwitchWorld(KW_KEEN4);
		ck_gameState.currentLevel = 1;
		ck_gameState.levelsDone[1] = 0;
		CK_LoadLevel(true, true);
		KW_CountLevelPeople(&hostiles, &residents, &elder);
		if (!hostiles || residents || KW_IsLevelPeaceful())
			Quit("KW_SelfTest: an uncompleted Border Village should keep its enemies!");
		{
			const char *problem = KW_CheckDialogueFits();
			if (problem)
				QuitF("KW_SelfTest: dialogue doesn't fit its card: %s", problem);
		}

		// The tavern: boarded up and not enterable before liberation.
		int streetX, streetY, roomX, roomY;
		KW_TavernDoors(&streetX, &streetY, &roomX, &roomY);
		if (!KW_IsArtPlaced(&kw_tavernBoardsArt) || KW_IsArtPlaced(&kw_tavernBarArt) ||
			KW_IsArtPlaced(&kw_tavernFurnitureArt) || KW_IsArtPlaced(&kw_portraitArt))
			Quit("KW_SelfTest: before completion only the tavern's boards should be there!");
		if (!KW_IsArtPlaced(&kw_signArt))
			Quit("KW_SelfTest: the inn's BAR sign is missing!");
		if (TI_ForeMisc(CA_TileAtPos(streetX, streetY, 1)) || CA_TileAtPos(streetX, streetY, 2))
			Quit("KW_SelfTest: the boarded-up tavern door can be entered!");
		// The inn keeps the fourth house's original door (x 77, to 0x641e).
		// (The boards cover part of the inn, so check the door's own cell.)
		if (!KW_IsArtCellPlaced(&kw_innArt, 77, 15) || (TI_ForeMisc(CA_TileAtPos(77, 15, 1)) & 0x7F) != MISCFLAG_DOOR ||
			CA_TileAtPos(77, 15, 2) != 0x641E)
			Quit("KW_SelfTest: the inn broke the fourth house's door!");
		ck_gameState.levelsDone[1] = 1;
		CK_LoadLevel(true, true);
		KW_CountLevelPeople(&hostiles, &residents, &elder);
		if (!KW_IsArtPlaced(&kw_tavernBarArt) || !KW_IsArtPlaced(&kw_tavernFurnitureArt) ||
			!KW_IsArtPlaced(&kw_portraitArt) || KW_IsArtPlaced(&kw_tavernBoardsArt))
			Quit("KW_SelfTest: after completion the tavern should be open and furnished!");
		if (!KW_IsArtPlaced(&kw_signArt))
			Quit("KW_SelfTest: the inn's BAR sign is missing after liberation!");
		// Its doors lead in and out, and the barmaid is animated.
		if ((TI_ForeMisc(CA_TileAtPos(streetX, streetY, 1)) & 0x7F) != MISCFLAG_DOOR ||
			CA_TileAtPos(streetX, streetY, 2) != ((roomX << 8) | roomY) ||
			(TI_ForeMisc(CA_TileAtPos(roomX, roomY - 1, 1)) & 0x7F) != MISCFLAG_DOOR ||
			CA_TileAtPos(roomX, roomY, 2) != ((streetX << 8) | (streetY + 1)))
			Quit("KW_SelfTest: the tavern doors don't connect the street and the room!");
		if (!KW_IsArtCellPlaced(&kw_innArt, 77, 15) || (TI_ForeMisc(CA_TileAtPos(77, 15, 1)) & 0x7F) != MISCFLAG_DOOR ||
			CA_TileAtPos(77, 15, 2) != 0x641E)
			Quit("KW_SelfTest: the inn broke the fourth house's door after liberation!");
		if (!TI_ForeAnimTile(CA_TileAtPos(KW_TavernBarmaidX(), roomY - 1, 1)))
			Quit("KW_SelfTest: the barmaid is not animated!");

		// A conversation pushes a cache level (the level's graphics become
		// purgeable) and caches pictures; if memory blocks run short the
		// memory manager purges. Art exists only in memory, so it must
		// survive that. Force it: use up every free block, then give them back.
		{
			CA_UpLevel();
			CA_CacheMarks(NULL);
			int want = 8192 - MM_UsedBlocks() + 300;
			mm_ptr_t *hog = (mm_ptr_t *)calloc(want, sizeof(mm_ptr_t));
			for (int b = 0; b < want; ++b)
				MM_GetPtr(&hog[b], 16);
			for (int b = 0; b < want; ++b)
				MM_FreePtr(&hog[b]);
			free(hog);
			CA_DownLevel();
			if (!KW_IsArtPlaced(&kw_tavernBarArt) || !KW_IsArtPlaced(&kw_tavernFurnitureArt) ||
				!KW_IsArtPlaced(&kw_portraitArt) || !KW_IsArtCellPlaced(&kw_innArt, 77, 15))
				Quit("KW_SelfTest: art was purged from memory (a conversation would crash)!");
		}
		// Coming back: Keen starts at the left end (his first start was at
		// the right), facing right...
		if (RF_UnitToTile(ck_keenObj->posX) > 8 || ck_keenObj->xDirection != IN_motion_Right)
			Quit("KW_SelfTest: a peaceful Border Village should start at the left, facing right!");
		// ...and can leave through either end.
		ck_keenObj->posX = RF_TileToUnit(1);
		CK_ResetClipRects(ck_keenObj);
		ck_inputFrame.xDirection = IN_motion_Left;
		ck_gameState.levelState = LS_Playing;
		KW_CheckPeacefulExit();
		if (ck_gameState.levelState != LS_LevelComplete)
			Quit("KW_SelfTest: a peaceful Border Village can't be left through the left end!");
		ck_keenObj->posX = RF_TileToUnit(CA_GetMapWidth() - 3);
		CK_ResetClipRects(ck_keenObj);
		ck_inputFrame.xDirection = IN_motion_Right;
		ck_gameState.levelState = LS_Playing;
		KW_CheckPeacefulExit();
		if (ck_gameState.levelState != LS_LevelComplete)
			Quit("KW_SelfTest: a peaceful Border Village can't be left through the right end!");
		ck_gameState.levelState = LS_Playing;
		ck_inputFrame.xDirection = IN_motion_None;
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

		// Keen Worlds changes to Keen 4's tile properties must not break any
		// level (e.g. an original animation chain).
		for (int lv = 0; lv <= 18; ++lv)
		{
			ck_gameState.currentLevel = lv;
			CK_LoadLevel(true, true);
		}
		ck_gameState.currentLevel = 0;
	}

	if (!ck_gameState.ep.ck4.wetsuit)
		Quit("KW_SelfTest: Keen 4 lost its wetsuit while Keen travelled!");
	CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: passed\n");
}
