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

// Atroxian Realm's own episode definition (kw_arealm.c): Keen 5's code with
// its own files and level setup.
#define KW_EPISODE_AREALM 10
extern CK_EpisodeDef ar_episode;
// Foray in the Forest's (kw_fitf.c), the same way with extension CKS.
#define KW_EPISODE_FITF 11
extern CK_EpisodeDef ff_episode;


typedef struct KW_WorldDef
{
	const char *id;          // short id: save files, /WORLD
	const char *name;        // shown in the Megarocket menu
	int episode;             // the engine episode it runs on (4, 5, 6)
	const char *dir;         // own data folder under the game path, or NULL
	const char *episodeFile; // variable file, or NULL for the episode's own
	// The game this world is, in its makers' words: shown when Keen first
	// arrives, and under "About this world" in the Megarocket. Credits are
	// pages separated by \f; fan games' come from their own Read Me files.
	const char *title;
	const char *byline;
	const char *credits;
	const char *getIt;       // for the menu when it isn't installed
} KW_WorldDef;

#define KW_ID_CREDITS \
	"Programming:\nJohn Carmack, John Romero,\nJason Blochowiak\n\n" \
	"Creative director: Tom Hall\nArt: Adrian Carmack\nMusic: Bobby Prince"
#define KW_NOT_OURS "\n\nKeen Worlds links this world\nto the others. It did not make it."

static const KW_WorldDef kw_worldDefs[KW_NUM_WORLDS] = {
	{"keen4", "Gnosticus IV", 4, NULL, NULL,
		"Commander Keen in\nSecret of the Oracle",
		"Goodbye, Galaxy! episode one\nid Software, 1991",
		KW_ID_CREDITS "\n\nPublished by Apogee Software" KW_NOT_OURS,
		"Episode 4 is shareware.\nPut GAMEMAPS.CK4, EGAGRAPH.CK4\nand AUDIO.CK4 (version 1.4)\nin the game folder."},
	{"keen5", "The Omegamatic", 5, NULL, NULL,
		"Commander Keen in\nThe Armageddon Machine",
		"Goodbye, Galaxy! episode two\nid Software, 1991",
		KW_ID_CREDITS "\n\nPublished by Apogee Software" KW_NOT_OURS,
		"Use your own copy of Keen 5.\nPut GAMEMAPS.CK5, EGAGRAPH.CK5\nand AUDIO.CK5 (version 1.4)\nin the game folder."},
	{"keen6", "Fribbulus Xax", 6, NULL, NULL,
		"Commander Keen in\nAliens Ate My Baby Sitter!",
		"id Software, 1991",
		KW_ID_CREDITS "\n\nPublished by FormGen" KW_NOT_OURS,
		"Use your own copy of Keen 6.\nPut GAMEMAPS.CK6, EGAGRAPH.CK6\nand AUDIO.CK6 (version 1.4)\nin the game folder."},
	// The Keys of Krodacia ("Keen 7", Ceilick), as converted for Omnispeak
	// in the Omnispeak modding kit (data/mod/keen7): the player copies that
	// folder to play/mods/keen7.
	{"keen7", "Krodacia", 4, "mods/keen7", "MOD.CK4",
		"The Keys of Krodacia",
		"The Universe is Toast, part one\nA fan episode by Ceilick",
		"Created by Ceilick\n\nMusic: KeenRush, Ceilick\n\nOmnispeak version:\nDavid Gow (Multimania)" KW_NOT_OURS,
		"A fan episode by Ceilick.\nGet its Omnispeak version from\nthe Omnispeak modding kit\n(github.com/sulix/omnispeak)\nand put it in mods/keen7."},
	// Dead in the Desert ("Keen 8", Ceilick and Levellass): the player's copy
	// of the mod's files in play/mods/keen8, with our MOD.CK6 (see there).
	{"keen8", "Calidune", 6, "mods/keen8", "MOD.CK6",
		"Dead in the Desert",
		"The Universe is Toast, part two\nA fan episode by Ceilick, 2009",
		"Mod by Ceilick, 2009\nPatching by Levellass\n\n"
		"Music: Mr. M, J. Greek,\nAntone S. Wallach, ZidaneA\n\n"
		"Beta testing:\nArjak, Iv4n, Mink, Mr. M" KW_NOT_OURS,
		"A fan episode by Ceilick (2009).\nDownload \"Commander Keen 8 -\nDead in the Desert\", for\ninstance from archive.org, and\nput its files in mods/keen8."},
	// Battle of the Brains ("Keen 9", Ceilick and Levellass): play/mods/keen9
	// with our MOD.CK5.
	{"keen9", "The Universe", 5, "mods/keen9", "MOD.CK5",
		"Battle of the Brains",
		"The Universe is Toast, part three\nA fan episode by\nCeilick, Mink and Tulip, 2011",
		"Mod by Ceilick, Mink and Tulip\nPatching by Levellass and Lemm\n\n"
		"Graphics contributions:\nMobiethian, VikingBoyBilly,\nXkyRauh, Mr. M, Fleexy\n\n"
		"Sound effects:\nCeilick, Levellass, Mr. M"
		"\f"
		"Music:\nGenius314, Mr. M, XkyRauh,\nthehackercat, Kosmyn, Umbrella" KW_NOT_OURS,
		"A fan episode by Ceilick,\nMink and Tulip (2011).\nDownload \"Commander Keen 9 -\nBattle of the Brains\", for\ninstance from archive.org, and\nput its files in mods/keen9."},
	// Atroxian Realm (Gridlock and many others, 2016): a game of its own with
	// GPL source. play/mods/arealm holds the player's copy of its data files
	// and the variable files tools/kw_arealm.py writes from its source.
	{"arealm", "Krisle", KW_EPISODE_AREALM, "mods/arealm", "AREALM.CKD",
		"Atroxian Realm",
		"A Commander Keen fan game\nby Gridlock, 2016",
		"Created by Gridlock\nProgramming: Lemm\nMusic: Gamebird, Gridlock\nStory graphics: T-Squared\n\n"
		"Artwork: Bubbatom, Ceilick,\nT-Squared, Gamebird, Mobiethian,\nTulip, Preston, Fantomx,\nDoomjedi, Grimson, Szemi"
		"\f"
		"Testing: Fleex, Keenfan,\nT-Squared, Gamebird, Lemm\n\n"
		"Engine programming: NY00123,\nAndy Durdin, Lemm, David Gow\n\nVersion 1.5 fixes: K1n9_Duk3" KW_NOT_OURS,
		"A fan game by Gridlock (2016).\nDownload Atroxian Realm v1.5\n(see its thread on pckf.com)\nand put its files in\nmods/arealm."},
	// Foray in the Forest (Nisaba and K1n9_Duk3, v2.0 2025): a game of its
	// own with GPL source. play/mods/fitf holds the player's copy of its data
	// files and the variable files tools/kw_fitf.py writes from its source.
	// TODO: the full credits from its own credits screens.
	{"fitf", "The Ancient Forest", KW_EPISODE_FITF, "mods/fitf", "FITF.CKS",
		"Foray in the Forest",
		"A Commander Keen fan game\nby Nisaba and K1n9_Duk3",
		"Created by Nisaba\nProgramming: K1n9_Duk3" KW_NOT_OURS,
		"A fan game by Nisaba and\nK1n9_Duk3. Download Foray in\nthe Forest v2.0 (see its page\non keenwiki.shikadi.net) and\nput its files in mods/fitf."},
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
	case KW_EPISODE_AREALM:
		return &ar_episode;
	case KW_EPISODE_FITF:
		return &ff_episode;
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
	if (def->episode == KW_EPISODE_AREALM || def->episode == KW_EPISODE_FITF)
	{
		// Its support files are in its own folder too; Keen 5's variables
		// (Omnispeak's own files) are its base.
		f = FSL_OpenFileInDirCaseInsensitive(KW_WorldPath(world),
			def->episode == KW_EPISODE_FITF ? "MAPTEMP.CKS" : "maptemp.ckd", false);
		if (!FS_IsFileValid(f))
			return false;
		FS_CloseFile(f);
		return FS_IsOmniFilePresent("EPISODE.CK5");
	}
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
	uint16_t levelsDone[CK_LEVELS_TRACKED];
	bool completed; // The world's ending has been seen.
	int16_t items[KW_NUM_ITEMS];
} KW_WorldState;

static KW_WorldState kw_worlds[KW_NUM_WORLDS];

int kw_pendingWorld = KW_NO_WORLD;
static bool kw_placeArrivalAtShip = false;

// Megarockets parked on fan games' world maps (see below).
typedef struct KW_MapShip KW_MapShip;
static const KW_MapShip *KW_MapShipFor(int world);

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
	// Keen 9's ship stands on its platform in level 1, the HUB Command
	// Center, where the level itself starts Keen at its nose.
	{KW_KEEN9, 1, 93, 30, 93, 26, 108, 31},
	// Atroxian Realm: the wreck of the Megarocket in its "BWB Crash Site"
	// level (25), where that level starts Keen beside it.
	{KW_AREALM, 25, 5, 16, 3, 13, 14, 17},
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

static bool kw_debugEnding; // the ending was started with W + E
void AR_DebugCompleteState(void); // Atroxian Realm's part of it (kw_arealm.c)

void KW_AfterWorldEnding(void)
{
	// help_endgame() pushes a cache level and, as in the original, never
	// pops it: the original quit to the title afterwards. Pop it here.
	CA_DownLevel();

	kw_worlds[kw_currentWorld].completed = true;
	if (!ck_gameState.levelsDone[ca_mapOn])
		ck_gameState.levelsDone[ca_mapOn] = 1;
	ck_gameState.currentLevel = 0;

	// The debug ending (W + E) skipped the game, so the map may not let Keen
	// reach his ship: put him at the Megarocket instead.
	if (kw_debugEnding)
	{
		kw_debugEnding = false;
		// The whole world counts as played through: every level is completed
		// (so each can be entered again, peaceful), not only the last one.
		for (int lv = 1; lv < CK_LEVELS_TRACKED; ++lv)
			if (CA_MapExists(lv) && lv != CK_INT(CK_highScoreLevel, -1) && !ck_gameState.levelsDone[lv])
				ck_gameState.levelsDone[lv] = 1;
		// ...and Keen has the world's story items, as after a real play-through.
		switch (ck_currentEpisode->ep)
		{
#ifdef WITH_KEEN4
		case EP_CK4:
			ck_gameState.ep.ck4.wetsuit = 1;
			ck_gameState.ep.ck4.membersRescued = CK_INT(CK4_NumCouncilsToRescue, 8);
			break;
#endif
#ifdef WITH_KEEN6
		case EP_CK6: // as the game's own item cheat gives them
			ck_gameState.ep.ck6.sandwich = ck_gameState.ep.ck6.rope = ck_gameState.ep.ck6.passcard = 1;
			break;
#endif
		default:
			break;
		}
		AR_DebugCompleteState();
		const KW_Ship *ship = KW_ShipFor(kw_currentWorld);
		if (ship)
			ck_gameState.currentLevel = ship->level;
		kw_placeArrivalAtShip = ship != NULL || KW_MapShipFor(kw_currentWorld) != NULL;
	}
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
#define KW_SAVE_VERSION 4
#define KW_LEVELS_EXTRA (CK_LEVELS_TRACKED - CK_LEVELS_SAVED)

// Atroxian Realm's extra game state (kw_arealm.c)
bool AR_WriteState(FS_File fp);
bool AR_ReadState(FS_File fp, bool withStars);
#define KW_SAVE_ID_LEN 8

static bool kw_introduced[KW_NUM_WORLDS]; // worlds whose arrival card has been shown
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
			FS_WriteInt16LE(state.items, KW_NUM_ITEMS, fp) != KW_NUM_ITEMS ||
			FS_WriteInt16LE(state.levelsDone + CK_LEVELS_SAVED, KW_LEVELS_EXTRA, fp) != KW_LEVELS_EXTRA)
			return false;
	}
	return AR_WriteState(fp);
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
	else if (version >= 2 && version <= KW_SAVE_VERSION)
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
			// Version 3: the levels past the engine's 25
			if (version >= 3 &&
				FS_ReadInt16LE(state.levelsDone + CK_LEVELS_SAVED, KW_LEVELS_EXTRA, fp) != KW_LEVELS_EXTRA)
				return false;
			int w = KW_WorldById(wid);
			if (w != KW_NO_WORLD)
				worlds[w] = state;   // worlds that are no longer known are dropped
		}
		// Version 3: Atroxian Realm's extra state
		if (version >= 3 && !AR_ReadState(fp, version >= 4))
			return false;
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
		kw_introduced[world] = true; // a saved game: Keen has been here
		*deferred = true;
		return true;
	}

	memcpy(kw_worlds, worlds, sizeof(kw_worlds));
	// The current world's items come from the record; the rest of its state
	// (map position, levels done) from the normal save that follows.
	for (int i = 0; i < KW_NUM_ITEMS; ++i)
		*KW_ItemField(i) = kw_worlds[world].items[i];
	// ...and the levels past the 25 the normal save holds.
	memcpy(ck_gameState.levelsDone + CK_LEVELS_SAVED, kw_worlds[world].levelsDone + CK_LEVELS_SAVED,
		KW_LEVELS_EXTRA * sizeof(ck_gameState.levelsDone[0]));
	kw_pendingWorld = KW_NO_WORLD;
	kw_placeArrivalAtShip = false;
	if (world >= 0 && world < KW_NUM_WORLDS)
		kw_introduced[world] = true; // a saved game: Keen has been here
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

void AR_ResetState(void); // Atroxian Realm's extra game state (kw_arealm.c)

void KW_ResetWorlds(void)
{
	AR_ResetState();
	memset(kw_worlds, 0, sizeof(kw_worlds));
	memset(kw_introduced, 0, sizeof(kw_introduced)); // a new game introduces each world again
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
	kw_placeArrivalAtShip = ship != NULL || KW_MapShipFor(world) != NULL;
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

// Where the gangway meets the ship's nose (tile 31 of the walkway row) is
// solid, so Keen stops right beside the ship instead of walking onto it.
#define KW_KEEN5_SHIP_NOSE_X 31
static bool KW_GateNoseIsSolid(int x, int y)
{
	return x == KW_KEEN5_SHIP_NOSE_X && y == KW_GATE_ORIGIN_Y + 1;
}

static const KW_Art kw_gateArt = {
	KW_GATE_ORIGIN_X, KW_GATE_ORIGIN_Y, KW_GATE_TILES_W, KW_GATE_TILES_H, KW_GATE_FRAMES,
	&kw_gateSlots[0][0][0], &kw_gateTiles[0][0][0][0],
	1,  // walkway row
	12, // tics per frame
	KW_GateDropsUnderlay,
	false, false, KW_GateNoseIsSolid,
};

static void KW_BuildKeen5Gate(void)
{
	KW_PlaceArt(&kw_gateArt);
}

// ---- Megarockets parked on fan games' world maps ----
// Art drawn by tools/kw_mapship_art.py, for worlds without a Megarocket
// level or a docked ship (Keen 8). Keen boards standing next to the ship.

#include "kw_mapship_art.h"

static const KW_Art kw_keen8ShipArt = {
	KW_KEEN8_SHIP_X, KW_KEEN8_SHIP_Y, KW_MAPSHIP_TILES_W, KW_MAPSHIP_TILES_H, 1,
	&kw_keen8ShipSlots[0][0][0], &kw_keen8ShipTiles[0][0][0][0],
	-1, 0, NULL, true, true,
};

struct KW_MapShip
{
	int world;
	const KW_Art *ship;
	int launchX, launchY; // where Keen stands to board
};

static const KW_MapShip kw_mapShips[] = {
	// Calidune: on the sand, a few steps below the map's own start; Keen
	// lands (and starts a new game) right beside its nose.
	{KW_KEEN8, &kw_keen8ShipArt, KW_KEEN8_SHIP_X - 1, KW_KEEN8_SHIP_Y},
};

static const KW_MapShip *KW_MapShipFor(int world)
{
	for (int i = 0; i < (int)(sizeof(kw_mapShips) / sizeof(kw_mapShips[0])); ++i)
		if (kw_mapShips[i].world == world)
			return &kw_mapShips[i];
	return NULL;
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
	else if (const KW_MapShip *ms = KW_MapShipFor(kw_currentWorld))
	{
		// A Megarocket parked on the map (our art).
		KW_PlaceArt(ms->ship);
		kw_launchSpotX = ms->launchX;
		kw_launchSpotY = ms->launchY;
	}
	else
	{
		// Others: the ship waits where Keen first lands on the world map.
		kw_launchSpotX = tileX;
		kw_launchSpotY = tileY;
	}
}

void KW_PlaceMapKeenAtShip(CK_object *mapKeen)
{
	const KW_MapShip *ms = KW_MapShipFor(kw_currentWorld);
	if (!ms || ca_mapOn != 0)
		return;
	// On arrival, and when the world starts fresh (no map position yet),
	// Keen stands right beside the ship's nose: his right edge just short of
	// the (solid) ship, his feet on its row.
	if (kw_placeArrivalAtShip || ck_gameState.mapPosX == 0)
	{
		mapKeen->posX = RF_TileToUnit(ms->ship->originX) - 0x100;
		mapKeen->posY = RF_TileToUnit(ms->ship->originY) - 0x80;
		CK_ResetClipRects(mapKeen);
		int overlap = mapKeen->clipRects.unitX2 - (RF_TileToUnit(ms->ship->originX) - 1);
		if (overlap > 0)
		{
			mapKeen->posX -= overlap;
			CK_ResetClipRects(mapKeen);
		}
		mapKeen->xDirection = mapKeen->yDirection = IN_motion_None;
	}
	kw_placeArrivalAtShip = false;
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
	// (The ship's nose is solid: Keen stops right beside it.)
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

bool KW_IsInShipLevel(void)
{
	return KW_IsShipLevel();
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

// ---- World cards: who made each world ----

static bool kw_cardsOff = false;             // the self-test has nobody to press a key
static int kw_arrivalCard = KW_NO_WORLD;     // world whose arrival card is due

#define KW_CARD_MAX_W 38
#define KW_CARD_MAX_H 22

// One card: centred text in a window sized to it; waits for a button.
static void KW_ShowCard(const char *text)
{
	uint16_t w, h;
	CK_MeasureMultiline(text, &w, &h);
	VL_FixRefreshBuffer();
	US_CenterWindow((w + 15) / 8 + 2, (h + 7) / 8 + 2);
	US_SetPrintY(US_GetPrintY() + 4);
	US_CPrint(text);
	VH_UpdateScreen();
	KW_WaitForButtonsUp();
	IN_WaitButton();
	KW_WaitForButtonsUp();
}

// Pages separated by \f, one card each.
static void KW_ShowCards(const char *pages)
{
	char page[512];
	while (*pages)
	{
		size_t n = strcspn(pages, "\f");
		if (n >= sizeof(page))
			n = sizeof(page) - 1;
		memcpy(page, pages, n);
		page[n] = 0;
		KW_ShowCard(page);
		pages += n + (pages[n] == '\f');
	}
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
}

static void KW_WorldCardText(int world, bool withCredits, char *out, size_t size)
{
	const KW_WorldDef *def = &kw_worldDefs[world];
	if (withCredits)
		snprintf(out, size, "%s\n\n%s\f%s", def->title, def->byline, def->credits);
	else
		snprintf(out, size, "%s\n\n%s", def->title, def->byline);
}

// The game's own title and makers, the first time Keen sets foot in a world.
static void KW_ShowArrivalCard(int world)
{
	char text[512];
	KW_WorldCardText(world, false, text, sizeof(text));
	KW_ShowCards(text);
}

// "About this world" in the Megarocket: title, makers and the full credits.
static void KW_ShowAboutWorld(int world)
{
	char text[1024];
	KW_WorldCardText(world, true, text, sizeof(text));
	KW_ShowCards(text);
}

// A world in the menu that isn't installed: what it is and where it goes.
static void KW_ShowGetWorld(int world)
{
	char text[512];
	snprintf(text, sizeof(text), "%s\n\n%s", kw_worldDefs[world].title, kw_worldDefs[world].getIt);
	KW_ShowCards(text);
}

// For /KWTEST: every card fits the screen. Returns the first that doesn't.
static const char *KW_CheckCardsFit(void)
{
	static char problem[96];
	for (int world = 0; world < KW_NUM_WORLDS; ++world)
	{
		char text[1024], get[512];
		KW_WorldCardText(world, true, text, sizeof(text));
		snprintf(get, sizeof(get), "%s\n\n%s", kw_worldDefs[world].title, kw_worldDefs[world].getIt);
		const char *all[2] = {text, get};
		for (int t = 0; t < 2; ++t)
			for (const char *p = all[t]; *p;)
			{
				char page[512];
				size_t n = strcspn(p, "\f");
				if (n >= sizeof(page))
					return "a card page is too long";
				memcpy(page, p, n);
				page[n] = 0;
				uint16_t w, h;
				CK_MeasureMultiline(page, &w, &h);
				if ((w + 15) / 8 + 2 > KW_CARD_MAX_W || (h + 7) / 8 + 2 > KW_CARD_MAX_H)
				{
					snprintf(problem, sizeof(problem), "%s card \"%.20s\" is %dx%d", kw_worldDefs[world].id, page, w, h);
					return problem;
				}
				p += n + (p[n] == '\f');
			}
	}
	return NULL;
}

// ---- Krisle: no way back until it is saved ----
// Atroxian Realm's story: the Megarocket is shot down over Krisle, and the
// Krile repair it once Atroxus is defeated. So Keen can't leave that world
// before its ending, and the menu says so before he flies there.

static bool KW_IsStranded(void)
{
	return kw_currentWorld == KW_AREALM && !kw_worlds[KW_AREALM].completed;
}

// A card with a question; true if the player presses Y.
static bool KW_AskCard(const char *text)
{
	uint16_t w, h;
	CK_MeasureMultiline(text, &w, &h);
	VL_FixRefreshBuffer();
	US_CenterWindow((w + 15) / 8 + 2, (h + 7) / 8 + 2);
	US_SetPrintY(US_GetPrintY() + 4);
	US_CPrint(text);
	VH_UpdateScreen();
	KW_WaitForButtonsUp();
	IN_SetLastScan(IN_SC_None);
	bool yes = false;
	while (true)
	{
		IN_PumpEvents();
		IN_ScanCode scan = IN_GetLastScan();
		if (scan != IN_SC_None)
		{
			yes = scan == IN_SC_Y;
			break;
		}
		IN_ControlFrame frame;
		IN_ReadControls(0, &frame);
		if (frame.jump || frame.pogo || frame.button2)
			break;
		VL_Present();
	}
	KW_WaitForButtonsUp();
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
	return yes;
}

#define KW_KRISLE_WARNING \
	"Krisle\n\nA distress signal comes from\nthis planet. The Megarocket\nwill be shot down there.\n\n" \
	"Keen cannot leave Krisle\nuntil he has saved it.\n\nFly there? Press Y for yes,\nany other key to stay."
#define KW_KRISLE_WRECK \
	"The Megarocket is a wreck.\n\nThe Krile will repair it\nonce Atroxus is defeated.\n\nUntil then Keen stays on Krisle."

// Draws the destination menu and returns the chosen world, or KW_NO_WORLD to
// stay. Every other world is listed: one that isn't installed opens a card
// saying where to get it. The last line is "About this world".
#define KW_MENU_ABOUT (-2)
static int KW_TravelMenu(void)
{
	int lineY[KW_NUM_WORLDS];
	int entries[KW_NUM_WORLDS];
	int numEntries = 0;

	if (KW_IsStranded())
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
		KW_ShowCards(KW_KRISLE_WRECK);
		return KW_NO_WORLD;
	}

	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (w != kw_currentWorld)
			entries[numEntries++] = w;
	entries[numEntries++] = KW_MENU_ABOUT;

	SD_PlaySound(CK_SOUNDNUM(SOUND_UNKNOWN12));
	VL_FixRefreshBuffer();
	US_CenterWindow(30, 4 + 2 * numEntries);
	US_SetPrintY(US_GetPrintY() + 3);
	US_CPrint("Bean-with-Bacon Megarocket");
	US_CPrint("Fly to:");
	US_SetPrintY(US_GetPrintY() + 4);
	for (int i = 0; i < numEntries; ++i)
	{
		char line[64];
		lineY[i] = US_GetPrintY() - 2;
		if (entries[i] == KW_MENU_ABOUT)
			snprintf(line, sizeof(line), "About this world");
		else if (KW_IsWorldPresent(entries[i]))
			snprintf(line, sizeof(line), "%s", kw_worldDefs[entries[i]].name);
		else
			snprintf(line, sizeof(line), "%s (not installed)", kw_worldDefs[entries[i]].name);
		US_CPrint(line);
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
			result = entries[sel];
			break;
		}
		if ((ck_inputFrame.yDirection == -1 || scan == IN_SC_UpArrow) && sel > 0)
			sel--;
		else if ((ck_inputFrame.yDirection == 1 || scan == IN_SC_DownArrow) && sel < numEntries - 1)
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
	if (result == KW_MENU_ABOUT)
	{
		KW_ShowAboutWorld(kw_currentWorld);
		return KW_NO_WORLD;
	}
	if (result != KW_NO_WORLD && !KW_IsWorldPresent(result))
	{
		KW_ShowGetWorld(result);
		return KW_NO_WORLD;
	}
	// Krisle is a one-way trip until it is saved: say so first.
	if (result == KW_AREALM && !kw_worlds[KW_AREALM].completed && !KW_AskCard(KW_KRISLE_WARNING))
		return KW_NO_WORLD;
	return result;
}

// ---- The registry plaque ----
// A small plaque stands beside the Megarocket in every world that has a
// ship level. Pressing up at the ship reads it: the game's title, its
// makers and the full credits. It is drawn over the level each frame (no
// tile or sprite of the world's own graphics is used or changed).

static bool KW_PlaquePixel(int *px, int *py)
{
	const KW_Ship *ship = KW_ShipFor(kw_currentWorld);
	if (ship)
	{
		if (ca_mapOn != ship->level)
			return false;
		// One tile to the side of where Keen arrives (away from the ship),
		// standing on the ground he stands on.
		*px = (ship->arrivalX - 1) * 16;
		*py = (ship->arrivalY + 1) * 16;
		return true;
	}
	// A ship on the world map: beside the boarding spot, away from the ship
	// (Keen 5: on the dock, a step before the map's own start).
	if (ca_mapOn != 0 || kw_launchSpotX < 0)
		return false;
	*px = (kw_launchSpotX - (kw_currentWorld == KW_KEEN5 ? 4 : 1)) * 16;
	*py = (kw_launchSpotY + 1) * 16;
	return true;
}

void KW_DrawOverlay(void)
{
	int wx, wy;
	if (kw_cardsOff || !KW_PlaquePixel(&wx, &wy))
		return;
	// The screen buffer starts at the tile the view is scrolled to.
	int x = wx - (RF_UnitToTile(rf_scrollXUnit) << 4);
	int y = wy - (RF_UnitToTile(rf_scrollYUnit) << 4);
	if (x < 0 || y < 26 || x + 16 > RF_BUFFER_WIDTH_PIXELS || y > RF_BUFFER_HEIGHT_PIXELS)
		return;
	// A post and a board, in the EGA palette: brown post, grey board with a
	// dark frame, a blue "i" and two lines of writing.
	VL_ScreenRect(x + 7, y - 12, 2, 12, 6);
	VL_ScreenRect(x + 1, y - 26, 14, 14, 0);
	VL_ScreenRect(x + 2, y - 25, 12, 12, 7);
	VL_ScreenRect(x + 3, y - 24, 10, 10, 15);
	VL_ScreenRect(x + 7, y - 23, 2, 2, 1);
	VL_ScreenRect(x + 7, y - 20, 2, 1, 1);
	VL_ScreenRect(x + 4, y - 18, 8, 1, 8);
	VL_ScreenRect(x + 4, y - 16, 8, 1, 8);
}

// Debug (W + E): end the level the way this world's game is won, so its
// ending plays and the world counts as completed, with every level of it
// completed too (see KW_AfterWorldEnding).
static void KW_DebugEnding(void)
{
	kw_debugEnding = true;
	switch (ck_currentEpisode->ep)
	{
#ifdef WITH_KEEN4
	case EP_CK4:
		// The last Council member: his message, then the ending.
		ck_gameState.ep.ck4.membersRescued = CK_INT(CK4_NumCouncilsToRescue, 8) - 1;
		ck_gameState.levelState = LS_CouncilRescued;
		break;
#endif
#ifdef WITH_KEEN5
	case EP_CK5: // Keen 5, Keen 9 and Atroxian Realm
		ck_gameState.levelState = LS_DestroyedQED;
		break;
#endif
#ifdef WITH_KEEN6
	case EP_CK6:
		ck_gameState.levelState = LS_Molly;
		break;
#endif
	default:
		break;
	}
}

void KW_PlayLoopHook(void)
{
	// Keen has just set foot in a world for the first time: its own title
	// and makers, once the screen is up.
	if (!kw_cardsOff && !kw_introduced[kw_currentWorld] && !kw_worlds[kw_currentWorld].visited &&
		(ca_mapOn == 0 || KW_IsShipLevel()))
		kw_arrivalCard = kw_currentWorld;
	if (kw_arrivalCard == kw_currentWorld && !vl_screenFaded && IN_DemoGetMode() == IN_Demo_Off)
	{
		kw_arrivalCard = KW_NO_WORLD;
		kw_introduced[kw_currentWorld] = true;
		KW_ShowArrivalCard(kw_currentWorld);
	}

	if (ca_mapOn != 0 && !KW_IsShipLevel())
	{
		kw_launchRequested = false;
		return;
	}
	if (KW_IsShipLevel() &&
		(ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed) &&
		KW_IsAtShipHatch())
		kw_launchRequested = true;

	// Reading the plaque: up, standing at the ship.
	if (KW_IsShipLevel() && !kw_cardsOff && ck_keenObj && ck_keenObj->topTI &&
		ck_inputFrame.yDirection == IN_motion_Up && KW_IsAtShipHatch())
	{
		KW_ShowAboutWorld(kw_currentWorld);
		// Don't open it again until the key has been let go.
		while (ck_inputFrame.yDirection != 0)
		{
			IN_PumpEvents();
			IN_ReadControls(0, &ck_inputFrame);
		}
		return;
	}

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

	// Debug shortcuts: hold W and press 4 to 9 to fly to a world, or E to
	// see the ending of the world Keen is in (on its map or at its ship).
	if (!IN_GetKeyState(IN_SC_W))
		return;
	if (IN_GetKeyState(IN_SC_E))
	{
		KW_WaitForButtonsUp();
		KW_DebugEnding();
		return;
	}
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
	else if (IN_GetKeyState(IN_SC_Nine))
		KW_RequestWorld(KW_KEEN9);
}

// For /KWTEST: Keen 8's converted behaviour (its PAT.PAT, see MOD.CK6) is in
// force in its world.
static void KW_TestKeen8Behaviour(void)
{
	CK_action *sit = CK_ACTION(CK6_ACT_NospikeSit0);
	CK_action *stunned = CK_ACTION(CK6_ACT_NospikeStunned0);
	CK_action *shoot = CK_ACTION_ALIAS(CK6_ACT_BobbaShootStart, CK6_ACT_BobbaShoot0);
	if (CK_INT(CK6_NospikeHealth, 4) != 16 || CK_INT(CK6_OrbatrixObjType, CT6_Orbatrix) != CT6_Item ||
		CK_INT(CK6_FleexHealth, 4) != 2 || CK_INTELEMENT(CK_LevelMusic, 2) != 8)
		Quit("KW_SelfTest: Keen 8's settings aren't loaded!");
	if (CK_ACTION_ALIAS(CK6_ACT_NospikeLanded, CK6_ACT_NospikeStunned0) != sit ||
		sit->next != CK_ACTION(CK6_ACT_NospikeCharge0) || sit->timer != 4 ||
		CK_ACTION_ALIAS(CK6_ACT_OrbatrixNearKeen, CK6_ACT_OrbatrixCurl0) != CK_ACTION(CK6_ACT_OrbatrixFloat1) ||
		CK_ACTION_ALIAS(CK6_ACT_BabobbaNapStart, CK6_ACT_BabobbaNap0) != CK_ACTION(CK6_ACT_BabobbaSit0))
		Quit("KW_SelfTest: Keen 8's redirected actions are wrong!");
	// Bobba's shot: two new frames, then the original one, then it hops on.
	if (shoot == CK_ACTION(CK6_ACT_BobbaShoot0) || !shoot->next ||
		shoot->next->next != CK_ACTION(CK6_ACT_BobbaShoot0) ||
		shoot->next->next->next != CK_ACTION(CK6_ACT_BobbaJump0) || shoot->think)
		Quit("KW_SelfTest: Keen 8's Bobba shooting frames are wrong!");
	if (CK_ACTION(CK6_ACT_GikJump0)->think != CK_ACT_GetFunction("CK6_OrbatrixUncurlThink") ||
		CK_ACTION(CK6_ACT_CeilickStrike10)->collide || CK_ACTION(CK6_ACT_Blorb0)->velY != 21)
		Quit("KW_SelfTest: Keen 8's creature actions are wrong!");
	// Touching the stunned boss wins the game.
	if (!stunned->collide)
		Quit("KW_SelfTest: Keen 8's stunned boss can't be touched!");
	int state = ck_gameState.levelState;
	stunned->collide(ck_keenObj, ck_keenObj);
	if (ck_gameState.levelState != LS_Molly)
		Quit("KW_SelfTest: touching Keen 8's stunned boss doesn't win!");
	ck_gameState.levelState = state;
}

// For /KWTEST: Keen 8 borrows sprites for the Bobbas' new shooting frames.
// They must be in memory in every level that has such a creature.
static void KW_TestKeen8Sprites(int level)
{
	static const struct { int type, first, last; const char *who; } needs[] = {
		{CT6_Bobba, 427, 430, "Bobba"},
		{CT6_Babobba, 431, 432, "Babobba"},
	};
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
		for (size_t i = 0; i < sizeof(needs) / sizeof(needs[0]); ++i)
			for (int chunk = needs[i].first; obj->type == needs[i].type && chunk <= needs[i].last; ++chunk)
				if (!ca_graphChunks[chunk])
					QuitF("KW_SelfTest: Keen 8 level %d has a %s but not its sprite %d!", level, needs[i].who, chunk);
}

// For /KWTEST: Keen 9's converted behaviour (its PATCH5.PAT, see MOD.CK5 and
// kw_keen9.c) is in force in its world.
static void KW_TestKeen9Behaviour(void)
{
	if (ca_audInfoE.startMusic != 187 || ca_audInfoE.numSongs != 19 || CK_INTELEMENT(CK_LevelMusic, 0) != 3 ||
		CK_INT(CK5_ShocksundHealth, 2) != 30 || CK_INT(CK5_QEDHealth, 0) != 2 || CK_INT(CK_WinGameExitLevel, -1) != 15 ||
		CK_INTELEMENT(lumpStarts, CK_INT(LUMP_KORATH, 0)) != 0x122 || CK_INTELEMENT(lumpEnds, CK_INT(LUMP_KORATH, 0)) != 0x129)
		QuitF("KW_SelfTest: Keen 9's settings aren't loaded! (music %d+%d, level 0 song %d, health %d/%d, exit %d, Korath lump %d-%d)",
			ca_audInfoE.startMusic, ca_audInfoE.numSongs, (int)CK_INTELEMENT(CK_LevelMusic, 0), (int)CK_INT(CK5_ShocksundHealth, 2),
			(int)CK_INT(CK5_QEDHealth, 0), (int)CK_INT(CK_WinGameExitLevel, -1),
			(int)CK_INTELEMENT(lumpStarts, CK_INT(LUMP_KORATH, 0)), (int)CK_INTELEMENT(lumpEnds, CK_INT(LUMP_KORATH, 0)));
	if (CK_ACTION_ALIAS(CK5_ACT_RoboShootStart, CK5_ACT_RoboShoot0) != CK_ACTION(CK5_ACT_ShikadiPole0) ||
		CK_ACTION_ALIAS(CK5_ACT_MasterShootStart, CK5_ACT_MasterShoot0) != CK_ACTION(CK5_ACT_RoboShoot0) ||
		CK_ACTION_ALIAS(CK5_ACT_ShikadiPoleStart, CK5_ACT_ShikadiPole0) != CK_ACTION(CK5_ACT_MasterShoot0) ||
		CK_ACTION_ALIAS(CK5_ACT_MineShrapA, CK5_ACT_MineShrap0) != CK_ACTION(CK5_ACT_Sphereful0))
		Quit("KW_SelfTest: Keen 9's redirected actions are wrong!");
	// The functions its patch file adds are there and wired to their actions.
	CK_action *korathoid = CK_ACTION(KW9_ACT_Korathoid0);
	if (!CK_ACTION(KW9_ACT_KeenStunned) || !CK_ACTION(KW9_ACT_SpherefulStunned) || !korathoid ||
		!korathoid->next || !korathoid->next->next || !korathoid->next->next->next ||
		korathoid->next->next->next->next != korathoid ||
		korathoid->think != CK_ACT_GetFunction("KW9_KorathoidWalk") || !korathoid->think ||
		korathoid->collide != CK_ACT_GetColFunction("KW9_KorathoidCol") || !korathoid->collide ||
		CK_ACTION(CK5_ACT_QED)->collide != CK_ACT_GetColFunction("KW9_WarpcoreCol") || !CK_ACTION(CK5_ACT_QED)->collide ||
		CK_ACTION(CK5_ACT_LevelEnd)->think != CK_ACT_GetFunction("KW9_FuseTouched") ||
		CK_ACTION(CK_ACT_keenRun1)->collide != CK_ACT_GetColFunction("KW9_ShrapCol") ||
		CK_ACTION(CK5_ACT_Ampton0)->collide != CK_ACT_GetColFunction("KW9_YetiCol") ||
		CK_ACTION(CK5_ACT_Ampton0)->think != CK_ACT_GetFunction("CK5_ShikadiWalk") ||
		CK_ACTION(CK5_ACT_Mine0)->collide != CK_ACT_GetColFunction("KW9_MineCol") ||
		CK_ACTION(CK5_ACT_MasterBall0)->collide != CK_ACT_GetColFunction("KW9_SnowballCol") ||
		CK_ACTION(CK5_ACT_Sphereful0)->collide != CK_ACT_GetColFunction("KW9_SpherefulCol") ||
		CK_ACTION(CK5_ACT_KorathWalk1)->collide != CK_ACT_GetColFunction("KW9_KorathCol"))
		Quit("KW_SelfTest: Keen 9's new creature code isn't wired up!");
}

// For /KWTEST: every creature in the loaded level has its picture in memory
// (a fan game moves sprites between caches; a wrong cache table shows here).
static void KW_TestLevelSpritesCached(const char *world, int level)
{
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
	{
		const CK_action *act = obj->currentAction;
		if (!act)
			continue;
		int chunks[2] = {act->chunkLeft, act->chunkRight};
		for (int i = 0; i < 2; ++i)
			if (chunks[i] > 0 && chunks[i] < CA_MAX_GRAPH_CHUNKS && !ca_graphChunks[chunks[i]])
				QuitF("KW_SelfTest: %s level %d: an object of type %d needs sprite %d, which isn't loaded!", world, level, obj->type, chunks[i]);
	}
}

// For /KWTEST: let the objects of the loaded level think, move, collide and
// draw for a number of frames, as the play loop does (without Keen's input).
// Like the game, only a screenful is active at a time: the window moves from
// object to object until the sprite table is nearly full. Catches crashes and
// bad memory use in creature code under the sanitizers.
static bool KW_TestNear(const CK_object *obj, const CK_object *focus)
{
	return abs(obj->clipRects.tileXmid - focus->clipRects.tileXmid) <= 12 &&
		abs(obj->clipRects.tileY2 - focus->clipRects.tileY2) <= 8;
}

static void KW_TestRunObjects(int frames)
{
	SD_SetSpriteSync(2);
	for (CK_object *focus = ck_keenObj->next; focus && RF_NumSpriteDraws() < 30; focus = focus->next)
	{
		if (!focus->currentAction)
			continue;
		CK_object snapshot = *focus; // where the window is, even if the object goes
		for (int frame = 0; frame < frames && ck_gameState.levelState == LS_Playing && RF_NumSpriteDraws() < 50; ++frame)
		{
			SD_SetLastTimeCount(SD_GetLastTimeCount() + 2);
			for (CK_object *obj = ck_keenObj->next; obj;)
			{
				CK_object *next = obj->next;
				if (obj->currentAction && KW_TestNear(obj, &snapshot))
				{
					obj->active = OBJ_ACTIVE;
					CK_RunAction(obj);
				}
				obj = next;
			}
			for (CK_object *a = ck_keenObj; a; a = a->next)
				for (CK_object *b = a->next; b; b = b->next)
				{
					if (!a->currentAction || !b->currentAction || a->type == CT_Nothing || b->type == CT_Nothing)
						continue;
					if (!KW_TestNear(b, &snapshot) || (a != ck_keenObj && !KW_TestNear(a, &snapshot)))
						continue;
					if (a->clipRects.unitX2 > b->clipRects.unitX1 && a->clipRects.unitX1 < b->clipRects.unitX2 &&
						a->clipRects.unitY1 < b->clipRects.unitY2 && a->clipRects.unitY2 > b->clipRects.unitY1)
					{
						if (a != ck_keenObj && a->currentAction->collide)
							a->currentAction->collide(a, b);
						if (b->currentAction && b->currentAction->collide)
							b->currentAction->collide(b, a);
					}
				}
			for (CK_object *obj = ck_keenObj->next; obj; obj = obj->next)
				if (obj->visible && obj->currentAction && obj->currentAction->draw && KW_TestNear(obj, &snapshot))
				{
					obj->visible = false;
					obj->currentAction->draw(obj);
				}
		}
		ck_gameState.levelState = LS_Playing;
	}
}

void CK_DrawStatusWindow(void);
// Atroxian Realm's checkpoints (kw_arealm.c), for the self-test
#define AR_CT_CheckPoint 33
void AR_CheckPointCol(CK_object *obj1, CK_object *obj2);
void AR_CheckpointFrame(void);
void AR_AfterLevelLoad(void);
void AR_ClearCheckpoint(void);
bool AR_HasCheckpoint(void);

#include "id_vl_private.h"

// Developer tool (/KWSHOT world level file.ppm): load a level of a world, draw
// one frame and save the screen as a picture. Needs a renderer that keeps
// pixels (not the null one): build with RENDERER=sdl2sw and run with
// SDL_VIDEODRIVER=dummy to use it without a display.
void KW_Screenshot(const char *worldId, int level, const char *path)
{
	static const uint8_t ega[16][3] = {{0, 0, 0}, {0, 0, 170}, {0, 170, 0}, {0, 170, 170}, {170, 0, 0}, {170, 0, 170},
		{170, 85, 0}, {170, 170, 170}, {85, 85, 85}, {85, 85, 255}, {85, 255, 85}, {85, 255, 255}, {255, 85, 85},
		{255, 85, 255}, {255, 255, 85}, {255, 255, 255}};
	int world = KW_WorldById(worldId);
	if (world < 0 || !KW_IsWorldPresent(world))
		QuitF("KW_Screenshot: world %s isn't installed", worldId);
	kw_cardsOff = true;
	CK_NewGame();
	ck_gameState.difficulty = D_Normal;
	if (world != KW_CurrentWorld())
		KW_SwitchWorld(world);
	ck_gameState.currentLevel = level;
	CK_LoadLevel(true, true);
	ck_gameState.levelState = LS_Playing;
	SD_SetSpriteSync(2);
	CK_CentreCamera(ck_keenObj);
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
	{
		ck_nextX = ck_nextY = 0;
		if (obj->currentAction && obj->currentAction->draw && CK_ObjectVisible(obj))
			obj->currentAction->draw(obj);
	}
	// KW_SHOT_FRAMES=n in the environment: let the level run n frames first
	// (objects think, move and draw as in the play loop; no input, no
	// collisions between objects).
	int frames = getenv("KW_SHOT_FRAMES") ? atoi(getenv("KW_SHOT_FRAMES")) : 0;
	for (int frame = 0; frame < frames && ck_gameState.levelState == LS_Playing; ++frame)
	{
		SD_SetLastTimeCount(SD_GetLastTimeCount() + 2);
		for (CK_object *obj = ck_keenObj; obj;)
		{
			CK_object *next = obj->next;
			if (obj->active == OBJ_INACTIVE && CK_ObjectVisible(obj))
				obj->active = OBJ_ACTIVE;
			if (obj->active != OBJ_INACTIVE && obj->currentAction)
				CK_RunAction(obj);
			obj = next;
		}
		for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
			if (obj->visible && obj->currentAction && obj->currentAction->draw)
			{
				obj->visible = false;
				obj->currentAction->draw(obj);
			}
		if (ca_mapOn == 0)
			CK_MapCamera(ck_keenObj);
		else
			CK_NormalCamera(ck_keenObj);
		RF_Refresh();
	}
	RF_Refresh();
	void *screen = vl_emuegavgaadapter.screen;
	int sx = VL_GetScrollX() & 15, sy = VL_GetScrollY() & 15;
	FILE *f = fopen(path, "wb");
	if (!f)
		QuitF("KW_Screenshot: can't write %s", path);
	fprintf(f, "P6\n320 200\n255\n");
	for (int y = 0; y < 200; ++y)
		for (int x = 0; x < 320; ++x)
			fwrite(ega[VL_SurfacePGet(screen, x + sx, y + sy) & 15], 1, 3, f);
	fclose(f);
	int objects = 0;
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
		++objects;
	printf("KW_Screenshot: %s level %d (%dx%d), view at tile %d,%d, %d objects, Keen at tile %d,%d sprite %d layer %d -> %s\n",
		worldId, level, CA_GetMapWidth(), CA_GetMapHeight(), (int)RF_UnitToTile(rf_scrollXUnit),
		(int)RF_UnitToTile(rf_scrollYUnit), objects, (int)RF_UnitToTile(ck_keenObj->posX),
		(int)RF_UnitToTile(ck_keenObj->posY), ck_keenObj->gfxChunk, ck_keenObj->zLayer, path);
}

// Developer self-test (/KWTEST): hop between all worlds several times, loading
// the world map and two levels each visit, and log memory use after every hop
// so leaks show up as steady growth.
void KW_SelfTest(void)
{
	static const int route[] = {KW_KEEN5, KW_KEEN6, KW_KEEN4, KW_KEEN7, KW_KEEN6, KW_KEEN5, KW_KEEN7, KW_KEEN4};
	bool haveKeen7 = KW_IsWorldPresent(KW_KEEN7);
	kw_cardsOff = true;
	uint16_t askW, askH;
	CK_MeasureMultiline(KW_KRISLE_WARNING, &askW, &askH);
	if ((askW + 15) / 8 + 2 > KW_CARD_MAX_W || (askH + 7) / 8 + 2 > KW_CARD_MAX_H)
		Quit("KW_SelfTest: the Krisle warning doesn't fit the screen!");
	const char *cardProblem = KW_CheckCardsFit();
	if (cardProblem)
		QuitF("KW_SelfTest: a world card doesn't fit the screen: %s", cardProblem);
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
					bool nose = KW_GATE_ORIGIN_X + c == KW_KEEN5_SHIP_NOSE_X;
					if (!nose && (TI_ForeTop(t) || TI_ForeBottom(t) || TI_ForeLeft(t) || TI_ForeRight(t) || TI_ForeMisc(t)))
						Quit("KW_SelfTest: Keen 5 gate walkway is blocked!");
					if (nose && !TI_ForeLeft(t))
						Quit("KW_SelfTest: Keen 5's ship nose isn't solid!");
					// The frames must be linked as a foreground animation.
					if (TI_ForeAnimTile(t) != kw_gateSlots[1][1][c] - t || !TI_ForeAnimTime(t))
						Quit("KW_SelfTest: Keen 5 gate lights are not animated!");
				}
				// Walking right along the gangway, Keen stops right beside the
				// ship's nose, not on the ship, and can board there.
				memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
				ck_inputFrame.dir = IN_dir_East;
				ck_inputFrame.xDirection = IN_motion_Right;
				CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_MapKeenWalk0));
				SD_SetSpriteSync(2);
				for (int t = 0; t < 400; ++t)
					CK_RunAction(ck_keenObj);
				int nose = RF_TileToUnit(KW_KEEN5_SHIP_NOSE_X);
				kw_launchRequested = false;
				KW_TryLaunch(ck_keenObj);
				if (ck_keenObj->clipRects.unitX2 >= nose || ck_keenObj->clipRects.unitX2 < nose - 96 || !kw_launchRequested)
				{
					CK_Cross_LogMessage(CK_LOG_MSG_ERROR, "KW_SelfTest: dock: Keen's right edge %d, nose %d, can board %d\n",
						ck_keenObj->clipRects.unitX2, nose, kw_launchRequested);
					Quit("KW_SelfTest: on Keen 5's dock Keen must stop beside the ship and board there!");
				}
				kw_launchRequested = false;
				memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
				ck_inputFrame.dir = IN_dir_None;
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

	// A completed Megarocket level is still entered beside the ship and left
	// by its normal exit: neither the far-end re-entry nor the "walk back out
	// where you came in" exit of other completed levels applies.
	for (int sw = 0; sw < KW_NUM_WORLDS; ++sw)
	{
		const KW_Ship *ship = KW_ShipFor(sw);
		if (!ship || !KW_IsWorldPresent(sw))
			continue;
		KW_SwitchWorld(sw);
		ck_gameState.levelsDone[ship->level] = 1;
		ck_gameState.currentLevel = ship->level;
		CK_LoadLevel(true, true);
		if (KW_LastRevisitPlacement() != KW_REVISIT_SPECIAL)
			Quit("KW_SelfTest: a completed ship level moved Keen away from its start!");
		kw_placeArrivalAtShip = true;
		CK_LoadLevel(true, true);
		memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
		ck_inputFrame.dir = IN_dir_West;
		ck_inputFrame.xDirection = IN_motion_Left;
		ck_gameState.levelState = LS_Playing;
		KW_CheckPeacefulExit();
		if (ck_gameState.levelState != LS_Playing)
			Quit("KW_SelfTest: arriving by ship in a completed ship level, one step left leaves it!");
		memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
		ck_inputFrame.dir = IN_dir_None;
		ck_gameState.levelsDone[ship->level] = 0;
	}

	// Keen 6 itself keeps its own behaviour: none of Keen 8's settings or
	// redirected actions leak into it.
	KW_SwitchWorld(KW_KEEN6);
	if (CK_INT(CK6_NospikeHealth, 4) != 4 || CK_INT(CK6_OrbatrixObjType, CT6_Orbatrix) != CT6_Orbatrix ||
		CK_INT(CK6_FlectKillsKeen, 0) || CK_INT(CK6_GrabbiterTalkPic, CK_CHUNKNUM(PIC_KEENTALK1)) != CK_CHUNKNUM(PIC_KEENTALK1) ||
		CK_ACTION_ALIAS(CK6_ACT_NospikeLanded, CK6_ACT_NospikeStunned0) != CK_ACTION(CK6_ACT_NospikeStunned0) ||
		CK_ACTION_ALIAS(CK6_ACT_BobbaShootStart, CK6_ACT_BobbaShoot0) != CK_ACTION(CK6_ACT_BobbaShoot0) ||
		CK_ACTION(CK6_ACT_NospikeStunned0)->collide || !CK_ACTION(CK6_ACT_CeilickStrike10)->collide)
		Quit("KW_SelfTest: Keen 6 has picked up Keen 8's behaviour!");

	// Keen 5 itself has none of Keen 9's.
	KW_SwitchWorld(KW_KEEN5);
	if (ca_audInfoE.startMusic != 192 || CK_INT(CK5_ShocksundHealth, 2) != 2 || CK_INT(CK5_QEDEveryLevel, 0) ||
		CK_INT(CK5_StandPlatformIsKorathoid, 0) || CK_INT(CK_WinGameExitLevel, -1) != -1 ||
		CK_ACTION_ALIAS(CK5_ACT_RoboShootStart, CK5_ACT_RoboShoot0) != CK_ACTION(CK5_ACT_RoboShoot0) ||
		CK_ACTION(CK5_ACT_QED)->collide || CK_ACTION(CK5_ACT_LevelEnd)->think != CK_ACT_GetFunction("CK5_LevelEnd") ||
		CK_ACTION(CK_ACT_keenRun1)->collide != CK_ACT_GetColFunction("CK_KeenColFunc"))
		Quit("KW_SelfTest: Keen 5 has picked up Keen 9's behaviour!");

	// Atroxian Realm (when installed): every map loads, with every object's
	// sprites in memory. (Its creatures aren't ported yet.)
	if (KW_IsWorldPresent(KW_AREALM))
	{
		KW_SwitchWorld(KW_AREALM);
		int loaded = 0;
		for (int lv = 0; lv < 26 /* the game uses maps 0-25; the file has spares */; ++lv)
		{
			if (!CA_MapExists(lv))
				continue;
			ck_gameState.currentLevel = lv;
			if (lv < (int)(sizeof(ck_gameState.levelsDone) / sizeof(ck_gameState.levelsDone[0])))
				ck_gameState.levelsDone[lv] = 0;
			CK_LoadLevel(true, true);
			if (!ck_keenObj || !ck_keenObj->currentAction)
				QuitF("KW_SelfTest: Atroxian Realm map %d has no Keen!", lv);
			KW_TestLevelSpritesCached("arealm", lv);
			StartMusic(lv); // must not refuse the level number (levels go past 20)
			StopMusic();
			// A checkpoint: touching it saves the level; loading the level
			// again puts Keen and every object back.
			for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
				if (obj->type == AR_CT_CheckPoint)
				{
					int objects = 0, restored = 0;
					for (CK_object *o = ck_keenObj; o; o = o->next)
						++objects;
					int cx = obj->posX, cy = obj->posY;
					AR_CheckPointCol(obj, ck_keenObj);
					AR_CheckpointFrame();
					if (!AR_HasCheckpoint())
						Quit("KW_SelfTest: touching an Atroxian Realm checkpoint doesn't save it!");
					CK_LoadLevel(true, true);
					AR_AfterLevelLoad();
					for (CK_object *o = ck_keenObj; o; o = o->next)
						++restored;
					// (+1: the rising checkpoint sign spawned after the save isn't in it)
					if (restored != objects || ck_keenObj->posX != cx || !AR_HasCheckpoint())
						QuitF("KW_SelfTest: Atroxian Realm level %d doesn't restart from its checkpoint (%d objects, %d before)!", lv, restored, objects);
					AR_ClearCheckpoint();
					break;
				}

			bool god = ck_godMode;
			ck_godMode = true; // creatures may touch Keen; he isn't the subject here
			KW_TestRunObjects(150);
			ck_godMode = god;
			++loaded;
		}
		if (loaded < 20)
			Quit("KW_SelfTest: Atroxian Realm is missing levels!");
		// Its completed levels are peaceful, by its own lists.
		for (int lv = 1; lv < 25; ++lv)
		{
			if (!CA_MapExists(lv))
				continue;
			ck_gameState.currentLevel = lv;
			ck_gameState.levelsDone[lv] = 1;
			CK_LoadLevel(true, true);
			int hostiles, residents;
			CK_object *first;
			KW_CountLevelPeople(&hostiles, &residents, &first);
			if (!KW_IsLevelPeaceful() || hostiles)
				QuitF("KW_SelfTest: completed Atroxian Realm level %d isn't peaceful (%d hostile)!", lv, hostiles);
			ck_gameState.levelsDone[lv] = 0;
		}
		// Arriving by Megarocket puts Keen at the wreck in the crash site level.
		const KW_Ship *arShip = KW_ShipFor(KW_AREALM);
		kw_placeArrivalAtShip = true;
		ck_gameState.currentLevel = arShip->level;
		CK_LoadLevel(true, true);
		if (!KW_IsAtShipHatch() || RF_UnitToTile(ck_keenObj->posX) != arShip->arrivalX)
			Quit("KW_SelfTest: Atroxian Realm's ship arrival is wrong!");
		int plaqueX, plaqueY;
		if (!KW_PlaquePixel(&plaqueX, &plaqueY) || plaqueX != (arShip->arrivalX - 1) * 16)
			Quit("KW_SelfTest: Atroxian Realm's registry plaque is missing!");
		// Keen is stranded there until the world is completed; then the
		// Megarocket stands repaired where the wreck was.
		if (!KW_IsStranded() || KW_KrisleShipIsPlaced())
			Quit("KW_SelfTest: Keen should be stranded on Krisle beside a wreck!");
		kw_worlds[KW_AREALM].completed = true;
		CK_LoadLevel(true, true);
		if (KW_IsStranded() || !KW_KrisleShipIsPlaced())
			Quit("KW_SelfTest: Krisle's Megarocket isn't repaired once the world is completed!");
		KW_TestLevelSpritesCached("arealm", arShip->level);
		kw_worlds[KW_AREALM].completed = false;
		KW_SwitchWorld(KW_KEEN4);
	}

	// Foray in the Forest (when installed): every map of the game loads, with
	// every object's sprites in memory.
	if (KW_IsWorldPresent(KW_FITF))
	{
		KW_SwitchWorld(KW_FITF);
		int loaded = 0;
		for (int lv = 0; lv < 27 /* GAMELEVELS; the file has more maps */; ++lv)
		{
			if (!CA_MapExists(lv))
				continue;
			ck_gameState.currentLevel = lv;
			ck_gameState.levelsDone[lv] = 0;
			CK_LoadLevel(true, true);
			if (!ck_keenObj || !ck_keenObj->currentAction)
				QuitF("KW_SelfTest: Foray in the Forest map %d has no Keen!", lv);
			KW_TestLevelSpritesCached("fitf", lv);
			CK_DrawStatusWindow(); // its own window; the engine's would use Keen 5's pictures
			bool god = ck_godMode;
			ck_godMode = true;
			KW_TestRunObjects(150);
			ck_godMode = god;
			++loaded;
		}
		if (loaded < 20)
			Quit("KW_SelfTest: Foray in the Forest is missing levels!");
		KW_SwitchWorld(KW_KEEN4);
	}

	// Keen 8 and 9 (when installed): every level loads, and every level when
	// completed is peaceful.
	for (int fan = KW_KEEN8; fan <= KW_KEEN9; ++fan)
	{
		if (!KW_IsWorldPresent(fan))
			continue;
		KW_SwitchWorld(fan);
		if (fan == KW_KEEN8)
			KW_TestKeen8Behaviour();
		else
			KW_TestKeen9Behaviour();
		int loaded = 0;
		for (int lv = 0; lv < CA_NUMMAPS; ++lv)
		{
			if (!CA_MapExists(lv))
				continue;
			ck_gameState.currentLevel = lv;
			ck_gameState.levelsDone[lv] = 0;
			CK_LoadLevel(true, true);
			if (fan == KW_KEEN8)
				KW_TestKeen8Sprites(lv);
			KW_TestLevelSpritesCached(KW_WorldId(fan), lv);
			ck_gameState.levelsDone[lv] = 1;
			CK_LoadLevel(true, true);
			int hostiles, residents;
			CK_object *first;
			KW_CountLevelPeople(&hostiles, &residents, &first);
			if (lv && (!KW_IsLevelPeaceful() || hostiles))
				Quit("KW_SelfTest: a completed Keen 8/9 level isn't peaceful!");
			CK_Cross_LogMessage(CK_LOG_MSG_NORMAL, "KW_SelfTest: %s level %d \"%s\" %dx%d, re-entry %d\n", KW_WorldId(fan), lv,
				CA_MapHeaders[lv]->name, CA_GetMapWidth(), CA_GetMapHeight(), (int)KW_LastRevisitPlacement());
			ck_gameState.levelsDone[lv] = 0;
			++loaded;
		}
		if (loaded < 18)
			Quit("KW_SelfTest: Keen 8/9 is missing levels!");
		const KW_Ship *ship = KW_ShipFor(fan);
		if (ship)
		{
			// Its ship stands in one of its levels (Keen 9: the HUB Command
			// Center): arriving puts Keen there, at the ship, ready to board.
			kw_placeArrivalAtShip = true;
			ck_gameState.currentLevel = ship->level;
			CK_LoadLevel(true, true);
			if (!KW_IsAtShipHatch() || RF_UnitToTile(ck_keenObj->posX) != ship->arrivalX)
				Quit("KW_SelfTest: a fan game's ship arrival is wrong!");
			ck_gameState.currentLevel = 0;
			CK_LoadLevel(true, true);
			if (kw_launchSpotX >= 0)
				Quit("KW_SelfTest: a world with a ship level also boards on its map!");
			continue;
		}
		// Otherwise its Megarocket is parked on the map, solid, with Keen's
		// boarding spot beside it, away from level entrances.
		ck_gameState.currentLevel = 0;
		CK_LoadLevel(true, true);
		const KW_MapShip *ms = KW_MapShipFor(fan);
		if (!ms || !KW_IsArtPlaced(ms->ship) ||
			kw_launchSpotX != ms->launchX || kw_launchSpotY != ms->launchY ||
			!TI_ForeTop(CA_TileAtPos(ms->ship->originX, ms->ship->originY, 1)))
			Quit("KW_SelfTest: a fan game's parked Megarocket is missing!");
		for (int y = kw_launchSpotY - KW_LAUNCH_RANGE - 1; y <= kw_launchSpotY + KW_LAUNCH_RANGE + 1; ++y)
			for (int x = kw_launchSpotX - KW_LAUNCH_RANGE - 1; x <= kw_launchSpotX + KW_LAUNCH_RANGE + 1; ++x)
				if ((CA_TileAtPos(x, y, 2) & 0xFF00) == 0xC000)
					Quit("KW_SelfTest: a parked Megarocket's boarding spot is next to a level entrance!");
		// Keen lands right beside it (not on it) and can board at once.
		{
			int shipX = RF_TileToUnit(ms->ship->originX);
			kw_launchRequested = false;
			KW_TryLaunch(ck_keenObj);
			if (ck_keenObj->clipRects.unitX2 >= shipX || ck_keenObj->clipRects.unitX2 < shipX - 64 ||
				ck_keenObj->clipRects.tileY2 < ms->ship->originY || ck_keenObj->clipRects.tileY1 > ms->ship->originY ||
				!kw_launchRequested)
			{
				CK_Cross_LogMessage(CK_LOG_MSG_ERROR, "KW_SelfTest: map ship: Keen %d-%d x %d-%d, ship at %d,%d, can board %d\n",
					ck_keenObj->clipRects.unitX1, ck_keenObj->clipRects.unitX2, ck_keenObj->clipRects.tileY1,
					ck_keenObj->clipRects.tileY2, shipX, ms->ship->originY, kw_launchRequested);
				Quit("KW_SelfTest: Keen doesn't land right beside a fan game's parked Megarocket!");
			}
			kw_launchRequested = false;
		}

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
	// Printed whatever the log level: which worlds were there to be tested.
	printf("KW_SelfTest: passed. Worlds tested:");
	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (KW_IsWorldPresent(w))
			printf(" %s", kw_worldDefs[w].id);
	printf("\n");
}
