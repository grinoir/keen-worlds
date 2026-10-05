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
#include <ctype.h>
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
#include "kw_os5_ep.h"
#include "kw_osi.h"
#include "ck6_ep.h"
#include "kw_world.h"
#include "kw_music.h"
#include "kw_art.h"
#include "kw_habitat.h"

extern const char *ck_episodeFile;

// ---- The world registry ----

// Atroxian Realm's own episode definition (kw_arealm.c): Keen 5's code with
// its own files and level setup.
#define KW_EPISODE_AREALM 10
#define KW_EPISODE_OSI 12 // Operation Station Infiltration (kw_osi.c, kw_os5_*.c)
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
	{"keen9", "HUB Command Center", 5, "mods/keen9", "MOD.CK5",
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
	// The credits are from its own credits screens (the main team and the
	// leads of each craft; the game's help screens name everyone).
	{"fitf", "Gnosticus Pi", KW_EPISODE_FITF, "mods/fitf", "FITF.CKS",
		"Foray in the Forest",
		"A Commander Keen fan game\nby Nisaba and K1n9_Duk3",
		"Creative director: Nisaba\nLead programmer: K1n9_Duk3\nTool smith: Fleexy\n"
		"Music: kvee, eggs_box,\nKohntarkosz_, Mr_M\n\nStarted by Mr. M in 2009"
		"\f"
		"Co-programmers: Nisaba, Lemm,\nNY00123, Spleen\n\n"
		"Story: Nisaba, nanomekia,\nobsidian29, Rolly, Lotte\n\n"
		"Level design: Nisaba\nMini-games: K1n9_Duk3, Spleen"
		"\f"
		"Sprite art: Nisaba, Zilem,\nXky Rauh, Plasma Captain, Neorice,\nFred Strom, siskavard_, DoomJedi\n\n"
		"Pixel art: Nisaba, ProYorp,\nBernie, Pandakeen, Mobiethian,\nZilem, Roobar and others"
		"\f"
		"Many more are named in the\ngame's own credits (its Help)." KW_NOT_OURS,
		"A fan game by Nisaba and\nK1n9_Duk3. Download Foray in\nthe Forest v2.0 (see its page\non keenwiki.shikadi.net) and\nput its files in mods/fitf."},
	// Operation Station Infiltration (Ceilick; programmed by Multimania,
	// 2022): a game built on Omnispeak itself, with GPL source. The player's
	// copy of its files (all of them, its variable files too) goes in
	// play/mods/osi; its code is brought in by tools/kw_osi_port.py.
	{"osi", "The Shikadi Station", KW_EPISODE_OSI, "mods/osi", "EPISODE.CK5",
		"Operation Station Infiltration",
		"A Commander Keen fan game\nby Ceilick, 2022",
		"Created by Ceilick\n\nProgramming:\nMultimania (David Gow)\n\nBuilt on Omnispeak, by\nDavid Gow, NY00123,\nAndy Durdin, Lemm and others" KW_NOT_OURS,
		"A fan game by Ceilick (2022).\nDownload Operation Station\nInfiltration v1.4 (see its page\non keenwiki.shikadi.net) and\nput its files in mods/osi."},
	// The Eight Accumulators ("Keen 9.5", Bubbatom, 2011): a Keen 4 level
	// pack. tools/kw_accumulators.py copies the player's files of it to
	// play/mods/keen95 and writes MOD.CK4 from its patch file.
	{"keen95", "Aspermontis", 4, "mods/keen95", "MOD.CK4",
		"The Eight Accumulators",
		"After the Battle of the Brains\nA fan episode by Bubbatom, 2011",
		"Created by Bubbatom\n\nWith help from\nCeilick, Mink and Gridlock" KW_NOT_OURS,
		"A fan episode by Bubbatom\n(2011). Download \"The Eight\nAccumulators\" (see its page on\nkeenwiki.shikadi.net) and run\ntools/kw_accumulators.py on it."},
	// The Megarocket: Keen Worlds' own world, the inside of Keen's ship
	// (tools/kw_shipworld.py writes its map; its folder comes with Keen
	// Worlds). It runs on Keen 4 and is not a destination in the menu.
	{"megarocket", "The Megarocket", 4, "megarocket", "SHIP.CK4",
		"The Bean-with-Bacon\nMegarocket",
		"Keen's ship, as drawn for\nForay in the Forest",
		"The inside of the Megarocket\nis from Foray in the Forest\nby Nisaba and K1n9_Duk3,\ntidied up for Keen Worlds.\n\nIt is built on the tiles of\nCommander Keen 4\n(id Software, 1991).",
		"It comes with Keen Worlds:\nthe folder megarocket is missing\nfrom the game folder."},
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
	case KW_EPISODE_OSI:
		return &osi_episode;
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
	if (def->episode == KW_EPISODE_OSI)
	{
		// everything is its own, in its own folder
		f = FSL_OpenFileInDirCaseInsensitive(KW_WorldPath(world), "GAMEMAPS.CK5", false);
		if (!FS_IsFileValid(f))
			return false;
		FS_CloseFile(f);
		return true;
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
// Foray in the Forest's own arrival: the ship flies in over its map and
// lands, then Keen is beside it (first visit only; see KW_SwitchWorld).
static bool kw_fitfFlyIn = false;

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
	// Aspermontis: the ship on the palm beach of its own Megarocket level,
	// where that level starts Keen at its nose.
	{KW_KEEN95, 18, 3, 17, 3, 13, 15, 18},
	// Keen 9's ship stands on its platform in level 1, the HUB Command
	// Center, where the level itself starts Keen at its nose.
	{KW_KEEN9, 1, 93, 30, 93, 26, 108, 31},
	// Atroxian Realm: the wreck of the Megarocket in its "BWB Crash Site"
	// level (25), where that level starts Keen beside it.
	{KW_AREALM, 25, 5, 16, 3, 13, 14, 17},
	// Foray in the Forest: the landed Megarocket outside, in its own BwB
	// level (1). Keen arrives and boards at the ship's nose; the boarding
	// area stops short of the ship's door (x 14-17), where "up" goes inside.
	{KW_FITF, 1, 5, 16, 4, 11, 11, 16},
};
// Foray's level 1: the flight deck in its own copy of the ship's rooms (the
// row Keen's feet are in when he stands on it, in the first copy).
#define KW_FITF_DECK_X1 12
#define KW_FITF_DECK_X2 15
#define KW_FITF_DECK_Y 26
#define KW_FITF_ROOMS_APART 28


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
static int kw_boardedFrom = KW_KEEN4; // the world the Megarocket stands on (see KW_ShipRoomFrame)

#define KW_SAVE_VERSION 7
#define KW_LEVELS_EXTRA (CK_LEVELS_TRACKED - CK_LEVELS_SAVED)

// Atroxian Realm's extra game state (kw_arealm.c)
bool AR_WriteState(FS_File fp);
bool AR_ReadState(FS_File fp, bool withStars);
// Foray in the Forest's (kw_fitf.c)
bool FF_WriteState(FS_File fp);
bool FF_ReadState(FS_File fp);
void FF_ResetState(void);
void FF_LeaveWorld(void);
void FF_ChooseGraphics(void);
const char *FF_TestSaveRoundTrip(bool (*save)(void), bool (*load)(void));
#define KW_SAVE_ID_LEN 8

static bool kw_introduced[KW_NUM_WORLDS]; // worlds whose arrival card has been shown
static bool kw_storyShown[KW_NUM_WORLDS]; // worlds whose own story has been shown (this game)
static int kw_storyWorld = KW_NO_WORLD;   // the world whose story plays before the ship lands on it
// The Chronicle: a way to play in which the worlds open in the order of the
// story (Free Flight, the other way, opens them all). Chosen when a game
// begins and kept in its saved games.
static int16_t kw_chronicle = 0;
static int16_t kw_courseTold[KW_NUM_WORLDS]; // the ship computer has announced this world's course
static int16_t kw_savedTold[KW_NUM_WORLDS];  // ... has said its piece about this world being saved
static int16_t kw_chronicleEndTold = 0;      // the Chronicle's last card has been shown
static bool kw_crashLanding = false;      // the ship is being shot down over Krisle (see KW_CrashLanding)
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

void FF_GetSongsHeard(int16_t *out, int count);
void FF_SetSongsHeard(const int16_t *in, int count);

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
	// Version 5: Foray in the Forest's state follows Atroxian Realm's.
	// Version 6: then the world the Megarocket stands on (for a game saved
	// inside the ship).
	// Version 7: then 261 words. (For a short while the scenery around the
	// ship was kept here.) The first is now whether the game is a Chronicle,
	// the next ones which worlds' courses the ship computer has announced.
	int16_t extra[261] = {0};
	extra[0] = kw_chronicle;
	for (int w = 0; w < KW_NUM_WORLDS; ++w)
	{
		extra[1 + w] = kw_courseTold[w];
		extra[40 + w] = kw_savedTold[w]; // (from word 40: which worlds' saving it has spoken of)
	}
	extra[80] = kw_chronicleEndTold;
	OSI_GetState(&extra[90]); // (from word 90: what dying takes back on the Shikadi Station)
	FF_GetSongsHeard(&extra[100], 64); // (from word 100: Foray in the Forest's songs heard, for its jukebox)
	return AR_WriteState(fp) && FF_WriteState(fp) && KW_WriteId(fp, kw_worldDefs[kw_boardedFrom].id) &&
		FS_WriteInt16LE(extra, 261, fp) == 261;
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
		// Version 5: Foray in the Forest's extra state
		if (version >= 5 && !FF_ReadState(fp))
			return false;
		if (version < 5)
			FF_ResetState();
		kw_chronicle = 0;
		memset(kw_courseTold, 0, sizeof(kw_courseTold));
		kw_boardedFrom = KW_KEEN4;
		if (version >= 6)
		{
			char from[KW_SAVE_ID_LEN];
			if (FS_Read(from, KW_SAVE_ID_LEN, 1, fp) != 1)
				return false;
			from[KW_SAVE_ID_LEN - 1] = 0;
			int w = KW_WorldById(from);
			if (w != KW_NO_WORLD && w != KW_SHIP)
				kw_boardedFrom = w;
		}
		if (version >= 7)
		{
			int16_t extra[261];
			if (FS_ReadInt16LE(extra, 261, fp) != 261)
				return false;
			// (a game saved while the scenery was kept here has other numbers:
			// only 0 and 1 mean anything)
			kw_chronicle = extra[0] == 1;
			for (int w = 0; w < KW_NUM_WORLDS; ++w)
			{
				kw_courseTold[w] = kw_chronicle && extra[1 + w] == 1;
				kw_savedTold[w] = extra[40 + w] == 1;
			}
			kw_chronicleEndTold = extra[80] == 1;
			OSI_SetState(&extra[90]);
			FF_SetSongsHeard(&extra[100], 64);
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
	FF_ResetState();
	memset(kw_worlds, 0, sizeof(kw_worlds));
	memset(kw_introduced, 0, sizeof(kw_introduced)); // a new game introduces each world again
	memset(kw_storyShown, 0, sizeof(kw_storyShown));
	kw_storyWorld = KW_NO_WORLD;
	kw_crashLanding = false;
	kw_chronicle = 0;
	memset(kw_courseTold, 0, sizeof(kw_courseTold));
	memset(kw_savedTold, 0, sizeof(kw_savedTold));
	kw_chronicleEndTold = 0;
	kw_pendingWorld = KW_NO_WORLD;
	kw_placeArrivalAtShip = false;
}

#define KW_SHIP_LEVEL 1 // the Megarocket's one room (see KW_ShipRoomFrame)

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
	FF_LeaveWorld(); // the standard colours again (Foray in the Forest sets its own)

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
	FF_ChooseGraphics(); // Foray in the Forest: which Keen (kw_fitf.c)
	OSI_ChooseFiles(); // Operation Station Infiltration: its tile properties (kw_osi.c)
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
	// Gnosticus Pi, the first time: the game's own arrival. Its map comes up
	// and the ship flies in and lands (FF_SpawnWorldKeen, FF_T_MapShip), which
	// leads into its level of the landed ship, where Keen is put beside it.
	kw_fitfFlyIn = world == KW_FITF && !kw_worlds[world].visited;
	if (kw_fitfFlyIn)
	{
		ck_gameState.currentLevel = 0;
		kw_placeArrivalAtShip = false;
	}
	// The inside of the Megarocket is one room, entered where its map says.
	if (world == KW_SHIP)
	{
		ck_gameState.currentLevel = KW_SHIP_LEVEL;
		kw_placeArrivalAtShip = false;
		kw_introduced[KW_SHIP] = true; // no arrival card for Keen's own ship
	}
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

// The world's own story, scrolling away into the distance as in the game's
// title sequence, the first time Keen sets foot in it. (Foray in the Forest
// shows its story at the start of every new game; the others only in their
// title sequence, which a traveller never sees.) Any key skips it.
void CK_DrawStarWarsEx(bool handleDemoKeys);
bool FF_IsActive(void);
void FF_LevelPalette(void);
void StartMusic(int16_t level);

// Should the current world's story be shown now? (Once a game for a world
// Keen has not been to; not in the scripted developer runs unless asked.)
static bool KW_StoryIsDue(int world)
{
	if (world == KW_SHIP || kw_storyShown[world] || kw_worlds[world].visited)
		return false;
	return !kw_cardsOff || getenv("KW_AUTOSTORY");
}

// The scroll itself, for the current world (whose graphics must be loaded).
static void KW_PlayWorldStory(void)
{
	kw_storyShown[kw_currentWorld] = true;
	if (!*CK_STRING(ck_str_storyText))
		return;
	printf("KW_AUTOSTORY: the story of %s\n", kw_worldDefs[kw_currentWorld].id);
	KW_StopVoice();
	SD_MusicOff();
	SD_FadeAdLibVolume(256, 0);
	IN_ClearKeysDown();
	CK_DrawStarWarsEx(false);
	IN_ClearKeysDown();
}

// In a level (Keen came to a world some other way than by choosing it at
// the ship's controls): the story, then back to the level.
static void KW_ShowWorldStory(void)
{
	if (!KW_StoryIsDue(kw_currentWorld))
		return;
	VL_FadeToBlack();
	CA_UpLevel();
	KW_PlayWorldStory();
	CA_DownLevel();
	// back to the level: its colours, its picture, its music
	VL_FadeToBlack();
	if (FF_IsActive())
		FF_LevelPalette();
	RF_ForceRefresh();
	RF_Refresh();
	VL_FadeFromBlack();
	StartMusic(ca_mapOn);
	SD_SetLastTimeCount(SD_GetTimeCount());
}

// Between levels (the game loop, when the ship is about to land): the story
// of the world chosen at the controls plays first, with that world's own
// pictures and letters loaded for it; then the ship's world again.
void KW_StoryBeforeLanding(void)
{
	int world = kw_storyWorld;
	kw_storyWorld = KW_NO_WORLD;
	if (world == KW_NO_WORLD || kw_currentWorld != KW_SHIP || !KW_IsWorldPresent(world))
		return;
	if (world == kw_pendingWorld)
	{
		// Keen is not coming back to the ship (Krisle: it is shot down): the
		// story, then straight on to the world.
		KW_SwitchWorld(world);
		KW_PlayWorldStory();
		return;
	}
	KW_WorldState untouched = kw_worlds[world];
	bool introduced = kw_introduced[world];
	KW_SwitchWorld(world);
	KW_PlayWorldStory();
	KW_SwitchWorld(KW_SHIP);
	kw_worlds[world] = untouched;   // Keen has not set foot in it yet
	kw_introduced[world] = introduced;
}

// The game's own title, its makers and its full credits, the first time Keen
// steps out of the ship onto a world.
static void KW_ShowArrivalCard(int world)
{
	// with everyone who made it: Keen Worlds only links the worlds
	char text[1024];
	KW_WorldCardText(world, true, text, sizeof(text));
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
				// (US_CPrint takes at most 255 characters at a time)
				if (n >= sizeof(page) || n > 255)
				{
					snprintf(problem, sizeof(problem), "%s card page \"%.20s...\" has %d characters (255 at most)",
						kw_worldDefs[world].id, p, (int)n);
					return problem;
				}
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
	// (inside the ship: where the ship stands)
	int where = kw_currentWorld == KW_SHIP ? kw_boardedFrom : kw_currentWorld;
	return where == KW_AREALM && !kw_worlds[KW_AREALM].completed;
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
// ---- The Chronicle ----
// The story's order: the Mortimer line, Keen 4 to Keen 9, and the journey
// home that follows it (The Eight Accumulators). After it the wider
// journey (Krisle, Gnosticus Pi), open together. Worlds that are not
// installed are passed over. A world Keen has been to stays open.
static const int kw_chronicleLine[] = {KW_KEEN4, KW_KEEN5, KW_KEEN6, KW_KEEN7, KW_KEEN8, KW_KEEN9, KW_KEEN95};
#define KW_CHRONICLE_LINE ((int)(sizeof(kw_chronicleLine) / sizeof(kw_chronicleLine[0])))

static bool KW_WorldIsSaved(int world)
{
	return kw_worlds[world].completed;
}

// The world that must be saved before this one opens (KW_NO_WORLD: none).
static int KW_ChronicleKey(int world)
{
	int before = KW_NO_WORLD;
	for (int i = 0; i < KW_CHRONICLE_LINE; ++i)
	{
		if (kw_chronicleLine[i] == world)
			return before;
		if (KW_IsWorldPresent(kw_chronicleLine[i]))
			before = kw_chronicleLine[i];
	}
	return before; // the wider journey: after the last world of the line
}

static bool KW_CourseIsOpen(int world)
{
	if (!kw_chronicle || world == KW_SHIP || kw_worlds[world].visited || world == kw_boardedFrom)
		return true;
	int key = KW_ChronicleKey(world);
	return key == KW_NO_WORLD || KW_WorldIsSaved(key);
}

// Where a Chronicle begins: the first world of the line that is installed.
static int KW_ChronicleStart(void)
{
	for (int i = 0; i < KW_CHRONICLE_LINE; ++i)
		if (KW_IsWorldPresent(kw_chronicleLine[i]))
			return kw_chronicleLine[i];
	return kw_currentWorld;
}

// A card with a few lines to choose from (the blinking box of the flight
// menu); returns the line chosen, or -1 for Esc.
static int KW_ChooseCard(const char *title, const char *const *lines, int count)
{
	int lineY[8];
	VL_FixRefreshBuffer();
	US_CenterWindow(30, 4 + 2 * count);
	US_SetPrintY(US_GetPrintY() + 3);
	US_CPrint(title);
	US_SetPrintY(US_GetPrintY() + 4);
	for (int i = 0; i < count; ++i)
	{
		lineY[i] = US_GetPrintY() - 2;
		US_CPrint(lines[i]);
		US_SetPrintY(US_GetPrintY() + 4);
	}
	KW_WaitForButtonsUp();
	int sel = 0, result = -1;
	int x1 = US_GetWindowX() + 4, x2 = US_GetWindowX() + US_GetWindowW() - 4;
	while (true)
	{
		IN_PumpEvents();
		int y = lineY[sel];
		int clr = ((SD_GetTimeCount() >> 4) & 1) ? 12 : 1;
		for (int pass = 0; pass < 2; ++pass)
		{
			VHB_HLine(x1, x2, y, clr);
			VHB_HLine(x1, x2, y + 13, clr);
			VHB_VLine(y, y + 13, x1, clr);
			VHB_VLine(y, y + 13, x2, clr);
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
			result = sel;
			break;
		}
		if ((ck_inputFrame.yDirection == -1 || scan == IN_SC_UpArrow) && sel > 0)
			sel--;
		else if ((ck_inputFrame.yDirection == 1 || scan == IN_SC_DownArrow) && sel < count - 1)
			sel++;
		if (scan != IN_SC_None)
			IN_SetLastScan(IN_SC_None);
		while (ck_inputFrame.yDirection != 0)
		{
			IN_PumpEvents();
			IN_ReadControls(0, &ck_inputFrame);
		}
	}
	KW_WaitForButtonsUp();
	return result;
}

// A new game: which way to play? (KW_AUTOCHRONICLE=1 answers for the
// scripted developer runs.)
static void KW_ChooseJourney(void)
{
	kw_chronicle = getenv("KW_AUTOCHRONICLE") != NULL;
	if (kw_cardsOff || getenv("KW_AUTOSHOT") || getenv("KW_AUTOCHRONICLE"))
		return;
	static const char *const ways[] = {"Free Flight: any world, any order", "The Chronicle: the story, in order"};
	VL_ClearScreen(0);
	VL_SetScrollCoords(0, 0);
	VL_SetDefaultPalette();
	IN_ClearKeysDown();
	kw_chronicle = KW_ChooseCard("How will Keen travel?", ways, 2) == 1;
	if (kw_chronicle)
		KW_ShowCards("The Chronicle\n\nThe worlds open one after\nanother, as their story goes.\nSave a world and the ship\nlearns the course to the next.");
	VL_ClearScreen(0);
}

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

	// Developer tool: KW_AUTODEST=id answers the menu (scripted runs)
	if (getenv("KW_AUTODEST"))
	{
		int w = KW_WorldById(getenv("KW_AUTODEST"));
		printf("KW_AUTODEST: the travel menu was opened in world %s; choosing %s\n",
			kw_worldDefs[kw_currentWorld].id, getenv("KW_AUTODEST"));
		if (w >= 0 && KW_IsWorldPresent(w) && !KW_CourseIsOpen(w))
		{
			printf("KW_AUTODEST: the Chronicle has no course to %s yet\n", kw_worldDefs[w].id);
			return KW_NO_WORLD;
		}
		return (w >= 0 && KW_IsWorldPresent(w)) ? w : KW_NO_WORLD;
	}

	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (w != kw_currentWorld && w != KW_SHIP) // (the ship is where one travels from, not to)
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
		else if (KW_IsWorldPresent(entries[i]) && !KW_CourseIsOpen(entries[i]))
			snprintf(line, sizeof(line), "%s (locked)", kw_worldDefs[entries[i]].name);
		else if (KW_IsWorldPresent(entries[i]) && KW_WorldIsSaved(entries[i]))
			snprintf(line, sizeof(line), "%s (saved)", kw_worldDefs[entries[i]].name);
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
		// (inside the ship: the world it stands on)
		KW_ShowAboutWorld(kw_currentWorld == KW_SHIP ? kw_boardedFrom : kw_currentWorld);
		return KW_NO_WORLD;
	}
	if (result != KW_NO_WORLD && KW_IsWorldPresent(result) && !KW_CourseIsOpen(result))
	{
		// The Chronicle: not yet.
		char text[160];
		int key = KW_ChronicleKey(result);
		snprintf(text, sizeof(text), "The Chronicle\n\nThe ship has no course to\n%s yet.\nSave %s first.",
			kw_worldDefs[result].name, kw_worldDefs[key].name);
		SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
		KW_ShowCards(text);
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

// (A small plaque used to stand beside the ship in every world, to be read
// for the game's makers. The makers are now named when Keen first steps out
// onto a world, and under "About this world" at the ship's controls.)
void KW_DrawOverlay(void)
{
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

// ---- Inside the Megarocket (Keen Worlds' own world, tools/kw_shipworld.py).
// Its map marks two places in the info plane: the flight console, where
// "up" opens the list of worlds, and the hatch, where "up" takes Keen back
// out to the world the ship stands on. ----

#define KW_SHIP_CONSOLE 0x7701
#define KW_SHIP_HATCH 0x7702
#define KW_SHIP_WAY_AFT 0x7703  // walking here takes Keen to the exit room
#define KW_SHIP_WAY_FORE 0x7704 // ... and here, back to the flight room
#define KW_SHIP_BED 0x7706      // the bed: "up" and Keen falls asleep (Keen Dreams)
#define KW_SHIP_AMMO 0x7705     // the box of stunners: "up" tops Keen's shots up
#define KW_SHIP_AMMO_SHOTS 20


// The ship computer says where the ship has landed, with a warning (words
// and voice clips: tools/kw_landing_voice.py). The clip plays on while the
// card with the same words is up, and after.
#include "kw_landing_text.h"
static int kw_landingToAnnounce = KW_NO_WORLD;

static void KW_AnnounceLanding(int world)
{
	for (int i = 0; i < (int)(sizeof(kw_landings) / sizeof(kw_landings[0])); ++i)
	{
		if (strcmp(kw_landings[i].id, kw_worldDefs[world].id))
			continue;
		KW_Speak(kw_landings[i].file, !kw_cardsOff);
		if (!kw_cardsOff)
		{
			KW_ShowCards(kw_landings[i].text);
			KW_StopVoice(); // the card is put away: no more of the clip
		}
		return;
	}
}

// A new game: Keen begins inside his ship, landed on the world the game
// starts in, and steps out through the hatch.
void KW_BeginInShip(void)
{
	bool tool = (us_tedLevel || getenv("KW_AUTOLEVEL")) && !getenv("KW_AUTOSHIP");
	if (tool || kw_currentWorld == KW_SHIP || !KW_IsWorldPresent(KW_SHIP))
		return;
	// Free Flight or the Chronicle? A Chronicle begins where the story does.
	KW_ChooseJourney();
	if (kw_chronicle && KW_ChronicleStart() != kw_currentWorld)
	{
		KW_SwitchWorld(KW_ChronicleStart());
		ck_gameState.currentLevel = 0;
		kw_placeArrivalAtShip = false;
		kw_worlds[kw_currentWorld].visited = false;
	}
	int from = kw_currentWorld;
	kw_courseTold[from] = 1;
	// the game that was chosen tells its story first
	if (KW_StoryIsDue(from))
		KW_PlayWorldStory();
	if (from == KW_AREALM && !kw_worlds[KW_AREALM].completed)
	{
		// Krisle's story begins with the ship shot down: no cabin to stand
		// in. Keen is thrown clear (KW_CrashLanding, once the level is up).
		kw_boardedFrom = from;
		kw_crashLanding = true;
		return;
	}
	KW_SwitchWorld(KW_SHIP);
	kw_worlds[from].visited = false; // Keen has not set foot in it yet
	kw_boardedFrom = from;
	kw_landingToAnnounce = from;
	KW_VoiceComingUp(true);
}

// ---- Keen Dreams: the bed, not the flight console. It is another game on
// another engine (the dream is not a place the ship can fly to), so it is
// run as a program of its own: the player's copy of Keen Dreams and an
// engine for it (Reflection Keen) in the folder "dreams" beside the game.
// Keen lies down, the dream runs in its own window, and when it is left
// Keen wakes in his cabin. Nothing of his travels goes into the dream or
// comes out of it.
//   dreams/dream.sh        if there: run this instead of the line below
//   dreams/reflectionhle   the engine; run with -gamever kdreamse113
//                          (the shareware version 1.13) -datadir . -cfgdir cfg
//   KW_DREAMCMD            (developer) a command to run instead

#ifndef _WIN32
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

static bool KW_FileIsThere(const char *dir, const char *name)
{
	char path[600];
	snprintf(path, sizeof(path), "%s/%s", dir, name);
	FILE *f = fopen(path, "rb");
	if (f)
		fclose(f);
	return f != NULL;
}

static void KW_Dream(void)
{
	char dir[512], command[1400];
	snprintf(dir, sizeof(dir), "%s/dreams", fs_keenPath);
	const char *forced = getenv("KW_DREAMCMD");
	bool script = KW_FileIsThere(dir, "dream.sh");
	bool engine = KW_FileIsThere(dir, "reflectionhle");
	bool game = KW_FileIsThere(dir, "KDREAMS.EGA") || KW_FileIsThere(dir, "kdreams.ega");
	if (!forced && !script && !(engine && game))
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
		if (!kw_cardsOff)
			KW_ShowCards("Keen lies down, but he\nis not sleepy.\n\n(Keen Dreams is not here:\nits files and an engine for\nit go in the folder dreams.)");
		return;
	}
	if (forced)
		snprintf(command, sizeof(command), "%s", forced);
	else if (script)
		snprintf(command, sizeof(command), "cd '%s' && sh ./dream.sh", dir);
	else
		snprintf(command, sizeof(command), "cd '%s' && mkdir -p cfg && ./reflectionhle -gamever kdreamse113 -datadir . -cfgdir cfg", dir);
	printf("KW_AUTODREAM: Keen falls asleep (%s)\n", forced ? forced : "Keen Dreams");

	if (!kw_cardsOff)
	{
		KW_ShowCards("Keen lies down on his bunk.\nJust for a minute...");
		int tics = KW_FadeMusicOut();
		VL_FadeToBlack();
		VL_DelayTics(tics);
	}
	KW_MusicQuiet();
	SD_StopSound();

#ifndef _WIN32
	// The dream runs beside us; our window stays alive (and dark) meanwhile.
	pid_t child = fork();
	if (child == 0)
	{
		execl("/bin/sh", "sh", "-c", command, (char *)NULL);
		_exit(127);
	}
	if (child > 0)
	{
		int status;
		while (waitpid(child, &status, WNOHANG) == 0)
		{
			IN_PumpEvents();
			VL_Present();
			VL_DelayTics(7);
		}
	}
#else
	system(command);
#endif

	IN_ClearKeysDown();
	RF_ForceRefresh();
	if (!kw_cardsOff)
	{
		RF_Refresh();
		VL_FadeFromBlack();
		KW_ShowCards("Keen wakes up in his bunk.\n\nWhat a dream!");
	}
	SD_SetLastTimeCount(SD_GetTimeCount());
	printf("KW_AUTODREAM: Keen wakes up\n");
}

// The ship has two rooms, each its own screen. Walking into the open end of
// one puts Keen at the open end of the other.
static void KW_ShipChangeRoom(uint16_t from, int fromY)
{
	uint16_t to = from == KW_SHIP_WAY_AFT ? KW_SHIP_WAY_FORE : KW_SHIP_WAY_AFT;
	for (int y = 0; y < CA_GetMapHeight(); ++y)
		for (int x = 0; x < CA_GetMapWidth(); ++x)
			if (CA_TileAtPos(x, y, 2) == to)
			{
				// beside the other way through, not on it
				ck_keenObj->posX = RF_TileToUnit(to == KW_SHIP_WAY_FORE ? x + 1 : x - 1);
				ck_keenObj->posY += RF_TileToUnit(y - fromY);
				CK_ResetClipRects(ck_keenObj);
				CK_CentreCamera(ck_keenObj);
				RF_ForceRefresh();
				return;
			}
}

static void KW_ShipRoomFrame(void)
{
	if (!ck_keenObj)
		return;
	// the marker is at Keen's feet or head, in the column he stands in
	int x = ck_keenObj->clipRects.tileXmid;
	uint16_t marker = 0;
	int markerY = 0;
	for (int y = ck_keenObj->clipRects.tileY1; y <= ck_keenObj->clipRects.tileY2 && !marker; ++y)
	{
		uint16_t info = CA_TileAtPos(x, y, 2);
		if (info >= KW_SHIP_CONSOLE && info <= KW_SHIP_BED)
		{
			marker = info;
			markerY = y;
		}
	}
	if (marker == KW_SHIP_WAY_AFT || marker == KW_SHIP_WAY_FORE)
	{
		KW_ShipChangeRoom(marker, markerY);
		return;
	}
	// One press of "up", one use (the scripted developer runs hold it down).
	static bool upUsed = false;
	if (ck_inputFrame.yDirection != IN_motion_Up)
		upUsed = false;
	if (!marker || !ck_keenObj->topTI || ck_inputFrame.yDirection != IN_motion_Up || upUsed)
		return;
	upUsed = true;
	if (marker == KW_SHIP_BED)
	{
		KW_Dream();
		return;
	}
	if (marker == KW_SHIP_AMMO)
	{
		// As in Foray in the Forest's ship: Keen never leaves without shots.
		if (ck_gameState.numShots < KW_SHIP_AMMO_SHOTS)
		{
			ck_gameState.numShots = KW_SHIP_AMMO_SHOTS;
			SD_PlaySound(CK_SOUNDNUM(SOUND_GOTSTUNNER));
		}
		else
			SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
		while (!kw_cardsOff && ck_inputFrame.yDirection != 0)
		{
			IN_PumpEvents();
			IN_ReadControls(0, &ck_inputFrame);
		}
		return;
	}
	int destination = KW_NO_WORLD;
	if (marker == KW_SHIP_CONSOLE)
	{
		destination = KW_TravelMenu();
		RF_ForceRefresh();
		SD_SetLastTimeCount(SD_GetTimeCount());
	}
	// Don't act again until the key has been let go.
	while (!kw_cardsOff && ck_inputFrame.yDirection != 0)
	{
		IN_PumpEvents();
		IN_ReadControls(0, &ck_inputFrame);
	}
	if (marker == KW_SHIP_HATCH)
	{
		// out, to the world the ship stands on
		KW_RequestWorld(KW_IsWorldPresent(kw_boardedFrom) ? kw_boardedFrom : KW_KEEN4);
		return;
	}
	if (destination != KW_NO_WORLD)
	{
		// The flight: the screen goes dark, and when it comes back the ship
		// stands on the chosen world. Keen is still inside; the computer
		// says where he is, and the hatch is the way out.
		if (!kw_cardsOff)
		{
			int tics = KW_FadeMusicOut(); // the music goes down with the lights
			VL_FadeToBlack();
			VL_DelayTics(tics > 35 ? tics : 35);
		}
		KW_MusicQuiet();
		if (destination == KW_AREALM && !kw_worlds[KW_AREALM].completed)
		{
			// Krisle: the ship never lands. Its story, then the crash, and
			// Keen is outside by the wreck, stranded until that world is saved.
			kw_boardedFrom = destination;
			kw_storyWorld = KW_StoryIsDue(destination) ? destination : KW_NO_WORLD;
			kw_crashLanding = true;
			KW_RequestWorld(destination);
			return;
		}
		if (KW_StoryIsDue(destination))
		{
			// A world Keen has not been to: its own story plays during the
			// flight (KW_StoryBeforeLanding, from the game loop, which then
			// loads the ship again: Keen is by the hatch when it has landed).
			kw_boardedFrom = destination;
			kw_storyWorld = destination;
			kw_landingToAnnounce = destination;
			KW_VoiceComingUp(true);
			kw_pendingWorld = KW_SHIP;
			ck_gameState.levelState = LS_SwitchWorld;
			return;
		}
		kw_boardedFrom = destination;
		RF_ForceRefresh();
		if (!kw_cardsOff)
		{
			RF_Refresh();
			VL_FadeFromBlack();
		}
		SD_SetLastTimeCount(SD_GetTimeCount());
		kw_landingToAnnounce = destination;
		KW_VoiceComingUp(true);
	}
}

// The Megarocket is shot down over Krisle: flashes and bangs over the first
// sight of the crash site, then the ship computer's last announcement.
static void KW_CrashLanding(void)
{
	printf("KW_AUTOCRASH: Keen is stranded on %s\n", kw_worldDefs[kw_currentWorld].name);
	if (!kw_cardsOff)
	{
		SD_MusicOff();
		static const int colours[] = {15, 12, 4, 15, 12, 4, 0};
		for (int i = 0; i < (int)(sizeof(colours) / sizeof(colours[0])); ++i)
		{
			if (i % 3 == 0)
				SD_PlaySound(CK_SOUNDNUM(SOUND_KEENDIE));
			VL_ScreenRect(0, 0, 320, 200, colours[i]);
			VL_Present();
			VL_DelayTics(i % 3 == 2 ? 14 : 5);
		}
		RF_ForceRefresh();
		RF_Refresh();
	}
	KW_AnnounceLanding(kw_currentWorld);
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
}

void KW_PlayLoopHook(void)
{
	// Foray's fly-in is under way on its map: when it has landed, Keen
	// arrives beside the ship in the next level.
	if (kw_fitfFlyIn && kw_currentWorld == KW_FITF)
	{
		if (ca_mapOn == 0)
			kw_placeArrivalAtShip = true;
		else
			kw_fitfFlyIn = false;
	}
	else
		kw_fitfFlyIn = false;

	// Keen has just set foot in a world for the first time: its own title
	// and makers, once the screen is up.
	if (getenv("KW_AUTOSHOT"))
		kw_cardsOff = true; // the developer tools have nobody to press a key either
	if ((!kw_cardsOff || getenv("KW_AUTOSTORY")) && !kw_introduced[kw_currentWorld] && !kw_worlds[kw_currentWorld].visited &&
		(ca_mapOn == 0 || KW_IsShipLevel()) && !(kw_fitfFlyIn && ca_mapOn == 0))
		kw_arrivalCard = kw_currentWorld;
	if (kw_crashLanding && kw_currentWorld == KW_AREALM && !vl_screenFaded && IN_DemoGetMode() == IN_Demo_Off)
	{
		kw_crashLanding = false;
		KW_CrashLanding();
	}
	if (kw_arrivalCard == kw_currentWorld && !vl_screenFaded && IN_DemoGetMode() == IN_Demo_Off)
	{
		kw_arrivalCard = KW_NO_WORLD;
		kw_introduced[kw_currentWorld] = true;
		KW_ShowWorldStory();
		KW_ShowArrivalCard(kw_currentWorld);
	}

	// Inside the Megarocket: the flight console and the hatch.
	if (kw_currentWorld == KW_SHIP)
	{
		if (kw_landingToAnnounce != KW_NO_WORLD && !vl_screenFaded && IN_DemoGetMode() == IN_Demo_Off)
		{
			int world = kw_landingToAnnounce;
			kw_landingToAnnounce = KW_NO_WORLD;
			KW_VoiceComingUp(false);
			printf("KW_AnnounceLanding: %s\n", kw_worldDefs[world].id);
			if (kw_chronicle && !kw_cardsOff && !kw_worlds[world].visited)
			{
				// The Chronicle counts its chapters: the worlds in the order
				// Keen has come to them.
				static const char *const numbers[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve"};
				int chapter = 0;
				for (int w = 0; w < KW_NUM_WORLDS; ++w)
					if (w != KW_SHIP && w != world && (kw_worlds[w].visited || KW_WorldIsSaved(w)))
						++chapter;
				char text[160];
				snprintf(text, sizeof(text), "The Chronicle\n\nChapter %s:\n%s", numbers[chapter < 12 ? chapter : 11], kw_worldDefs[world].name);
				KW_ShowCards(text);
			}
			KW_AnnounceLanding(world);
		}
		// A world has been saved since Keen was last in the cabin: the ship
		// computer has something to say about it (once for each world).
		if (!kw_cardsOff && !vl_screenFaded && !KW_VoiceSpeaking())
		{
			for (int w = 0; w < KW_NUM_WORLDS; ++w)
			{
				if (w == KW_SHIP || kw_savedTold[w] || !KW_WorldIsSaved(w))
					continue;
				kw_savedTold[w] = 1;
				for (int i = 0; i < (int)(sizeof(kw_savedLines) / sizeof(kw_savedLines[0])); ++i)
				{
					if (strcmp(kw_savedLines[i].id, kw_worldDefs[w].id))
						continue;
					KW_Speak(kw_savedLines[i].file, true);
					KW_ShowCards(kw_savedLines[i].text);
					KW_StopVoice();
				}
				break; // one at a time
			}
		}
		// The Chronicle: every world of it is saved.
		if (kw_chronicle && !kw_chronicleEndTold && !kw_cardsOff && !vl_screenFaded && !KW_VoiceSpeaking())
		{
			bool all = true, any = false;
			for (int w = 0; w < KW_NUM_WORLDS; ++w)
				if (w != KW_SHIP && KW_IsWorldPresent(w))
				{
					any = true;
					all = all && KW_WorldIsSaved(w) && kw_savedTold[w];
				}
			if (any && all)
			{
				kw_chronicleEndTold = 1;
				KW_ShowCards("The Chronicle\n\nEvery world is saved.\nThe galaxy is quiet, and\nthe Megarocket knows the\nway to all of it."
					"\fThe ship is yours, Keen.\nFly where you like.");
			}
		}
		// The Chronicle: a world has been saved, and the ship has learned
		// the course to the next.
		if (kw_chronicle && !kw_cardsOff && !vl_screenFaded && !KW_VoiceSpeaking())
		{
			for (int w = 0; w < KW_NUM_WORLDS; ++w)
			{
				if (w == KW_SHIP || kw_courseTold[w] || !KW_IsWorldPresent(w) || !KW_CourseIsOpen(w))
					continue;
				kw_courseTold[w] = 1;
				if (kw_worlds[w].visited)
					continue;
				char text[160], file[32];
				snprintf(text, sizeof(text), "Ship computer:\n\nA new course is open:\n%s.", kw_worldDefs[w].name);
				snprintf(file, sizeof(file), "COURSE_%s.RAW", kw_worldDefs[w].id);
				for (char *c = file; *c && *c != '.'; ++c)
					*c = (char)toupper((unsigned char)*c);
				KW_Speak(file, true);
				KW_ShowCards(text);
				KW_StopVoice();
				break; // one at a time
			}
		}
		KW_ShipRoomFrame();
		return;
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
	// Foray in the Forest: its landed ship has the game's own rooms inside
	// (its ammo station, its collected things, its transceiver). The way
	// from there into the Megarocket's cabin, where a world is picked, is
	// the flight deck: "up" while standing at the controls. The rooms are
	// lower in the same level, in copies 28 rows apart.
	if (kw_currentWorld == KW_FITF && KW_IsShipLevel() && ck_keenObj && KW_IsWorldPresent(KW_SHIP) &&
		ck_keenObj->topTI && ck_inputFrame.yDirection == IN_motion_Up)
	{
		int x = ck_keenObj->clipRects.tileXmid, y = ck_keenObj->clipRects.tileY2;
		if (x >= KW_FITF_DECK_X1 && x <= KW_FITF_DECK_X2 && y >= KW_FITF_DECK_Y &&
			(y - KW_FITF_DECK_Y) % KW_FITF_ROOMS_APART == 0)
			kw_launchRequested = true;
	}


	if (kw_launchRequested)
	{
		kw_launchRequested = false;
		// Boarding takes Keen inside his ship (Keen Worlds' own world),
		// where the flight console is. Without that world's folder, the
		// menu opens here as it used to.
		if (KW_IsWorldPresent(KW_SHIP))
		{
			if (KW_IsStranded())
			{
				SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
				KW_ShowCards(KW_KRISLE_WRECK);
				return;
			}
			kw_boardedFrom = kw_currentWorld;
			KW_RequestWorld(KW_SHIP);
			return;
		}
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
	else if (IN_GetKeyState(IN_SC_K))
		KW_RequestWorld(KW_AREALM); // Krisle
	else if (IN_GetKeyState(IN_SC_F))
		KW_RequestWorld(KW_FITF); // Gnosticus Pi
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

extern CK_object *ck_scoreBoxObj;
extern bool ck_scoreBoxEnabled;
void CK_UpdateScoreBox(CK_object *scorebox);

// Save what is on the screen as a PPM picture.
static void KW_DumpScreen(const char *path)
{
	void *screen = vl_emuegavgaadapter.screen;
	int sx = VL_GetScrollX() & 15, sy = VL_GetScrollY() & 15;
	FILE *f = fopen(path, "wb");
	if (!f)
		QuitF("KW_DumpScreen: can't write %s", path);
	fprintf(f, "P6\n320 200\n255\n");
	for (int y = 0; y < 200; ++y)
		for (int x = 0; x < 320; ++x)
			fwrite(VL_EGARGBColorTable[VL_SurfacePGet(screen, x + sx, y + sy) & 15], 1, 3, f); // the colours shown now
	fclose(f);
}

// Developer tool for the real play loop, driven by the environment:
//   KW_AUTOINPUT="x,y,jump,pogo,fire"  held every frame (x, y: -1, 0 or 1),
//                                      or steps "frames:x,y,jump,pogo,fire;..."
//   KW_AUTOSHOT="frames:file.ppm"      after that many frames, save the
//                                      screen, print where Keen is, and quit
// Used with /TEDLEVEL n to try out a level without a keyboard.
void KW_DebugInput(void)
{
	// KW_AUTOINPUT is a list of steps "frames:x,y,jump,pogo,fire;..." (the
	// last step is held to the end; "x,y,..." alone is one step).
	static const char *step;
	static int left, x, y, jump, pogo, fire, parsed;
	if (!parsed)
	{
		parsed = 1;
		step = getenv("KW_AUTOINPUT");
		if (step)
			parsed = 2;
	}
	if (parsed != 2)
		return;
	if (left <= 0 && step)
	{
		int frames = 1 << 30, n = 0;
		x = y = jump = pogo = fire = 0;
		if (sscanf(step, "%d:%d,%d,%d,%d,%d%n", &frames, &x, &y, &jump, &pogo, &fire, &n) < 2)
		{
			frames = 1 << 30;
			sscanf(step, "%d,%d,%d,%d,%d", &x, &y, &jump, &pogo, &fire);
		}
		left = frames;
		step = strchr(step, ';');
		if (step)
			++step;
	}
	--left;
	ck_inputFrame.xDirection = (IN_Motion)x;
	ck_inputFrame.yDirection = (IN_Motion)y;
	static const IN_Direction dirs[3][3] = {{IN_dir_NorthWest, IN_dir_North, IN_dir_NorthEast},
		{IN_dir_West, IN_dir_None, IN_dir_East}, {IN_dir_SouthWest, IN_dir_South, IN_dir_SouthEast}};
	ck_inputFrame.dir = dirs[y + 1][x + 1];
	ck_inputFrame.jump = ck_keenState.jumpIsPressed = jump != 0;
	ck_inputFrame.pogo = ck_keenState.pogoIsPressed = pogo != 0;
	ck_inputFrame.button2 = ck_keenState.shootIsPressed = fire != 0;
}

// KW_AUTOWAIT="file": whenever the game waits for a key, save the screen as
// file1.ppm, file2.ppm... and go on as if one was pressed. Returns true if it
// did.
bool KW_DebugWait(void)
{
	static int count;
	const char *s = getenv("KW_AUTOWAIT");
	if (!s)
		return false;
	char path[600];
	snprintf(path, sizeof(path), "%s%d.ppm", s, ++count);
	VL_Present();
	KW_DumpScreen(path);
	printf("KW_AUTOWAIT: a wait for a key -> %s\n", path);
	return true;
}

void KW_DebugFrame(void)
{
	static int frames = -1, count;
	static char path[512];
	if (frames < 0)
	{
		frames = 0;
		const char *s = getenv("KW_AUTOSHOT");
		if (s)
			sscanf(s, "%d:%511s", &frames, path);
	}
	// KW_AUTOKILL=n: Keen is killed at frame n (to try out what dying does)
	static int killAt = -1;
	if (killAt < 0)
		killAt = getenv("KW_AUTOKILL") ? atoi(getenv("KW_AUTOKILL")) : 0;
	// KW_AUTOPLACE="x,y": Keen is put at that tile on the first frame
	if (count == 0 && getenv("KW_AUTOPLACE"))
	{
		int px, py;
		if (sscanf(getenv("KW_AUTOPLACE"), "%d,%d", &px, &py) == 2)
		{
			ck_keenObj->posX = RF_TileToUnit(px);
			ck_keenObj->posY = RF_TileToUnit(py);
			CK_SetAction(ck_keenObj, ck_keenObj->currentAction);
			CK_CentreCamera(ck_keenObj);
		}
	}
	// KW_AUTOLIVES=n: Keen has n lives from the first frame
	if (count == 0 && getenv("KW_AUTOLIVES"))
		ck_gameState.numLives = atoi(getenv("KW_AUTOLIVES"));
	if (killAt && count + 1 == killAt)
		CK_KillKeen();
	if (!frames || ++count < frames)
		return;
	KW_DumpScreen(path);
	printf("KW_AUTOSHOT: world %s level %d, Keen at tile %d,%d, sprite %d, level state %d -> %s\n",
		kw_worldDefs[kw_currentWorld].id, ca_mapOn,
		(int)RF_UnitToTile(ck_keenObj->posX), (int)RF_UnitToTile(ck_keenObj->posY), ck_keenObj->gfxChunk,
		(int)ck_gameState.levelState, path);
	Quit(0);
}

// For the Options menu (ck_us_2.c): Keen's look on Gnosticus Pi
bool KW_IsForestInstalled(void)
{
	return KW_IsWorldPresent(KW_FITF);
}
bool KW_IsInForest(void)
{
	return kw_currentWorld == KW_FITF;
}

// Developer tool (/KWSHOT world level file.ppm): load a level of a world, draw
// one frame and save the screen as a picture. Needs a renderer that keeps
// pixels (not the null one): build with RENDERER=sdl2sw and run with
// SDL_VIDEODRIVER=dummy to use it without a display.
void KW_Screenshot(const char *worldId, int level, const char *path)
{
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
			if (obj->visible && obj->currentAction && obj->currentAction->draw && CK_ObjectVisible(obj))
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
	ck_scoreBoxEnabled = true;
	if (ck_scoreBoxObj)
		CK_UpdateScoreBox(ck_scoreBoxObj);
	RF_Refresh();
	KW_DumpScreen(path);
	int objects = 0;
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
		++objects;
	// KW_SHOT_LIST=1: what stands on the screen
	if (getenv("KW_SHOT_LIST"))
		for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
			if (CK_ObjectVisible(obj))
				printf("  type %d sprite %d layer %d at %d,%d (pixel %d,%d on screen)\n", obj->type, obj->gfxChunk, obj->zLayer,
					(int)RF_UnitToTile(obj->posX), (int)RF_UnitToTile(obj->posY),
					(obj->posX - rf_scrollXUnit) / 16, (obj->posY - rf_scrollYUnit) / 16);
	printf("KW_Screenshot: %s level %d (%dx%d), view at tile %d,%d, %d objects, Keen at tile %d,%d sprite %d layer %d -> %s\n",
		worldId, level, CA_GetMapWidth(), CA_GetMapHeight(), (int)RF_UnitToTile(rf_scrollXUnit),
		(int)RF_UnitToTile(rf_scrollYUnit), objects, (int)RF_UnitToTile(ck_keenObj->posX),
		(int)RF_UnitToTile(ck_keenObj->posY), ck_keenObj->gfxChunk, ck_keenObj->zLayer, path);
}

// For /KWTEST: write and read a whole saved game (header and all).
static const char *kw_testSaveFile = "KWTEST2.SAV";
static bool KW_TestWriteSave(void)
{
	FS_File fp = FS_CreateUserFile(kw_testSaveFile);
	US_Savefile header = {KW_SAVE_TAG, KW_SAVE_MAGIC, true, "Self-test"};
	uint8_t padding = 0;
	bool ok = FS_IsFileValid(fp) &&
		FS_Write(header.id, sizeof(header.id), 1, fp) == 1 &&
		FS_WriteInt16LE(&header.printXOffset, 1, fp) == 1 &&
		FS_WriteBoolTo16LE(&header.used, 1, fp) == 1 &&
		FS_Write(header.name, sizeof(header.name), 1, fp) == 1 &&
		FS_Write(&padding, 1, 1, fp) == 1 &&
		CK_SaveGame(fp);
	if (FS_IsFileValid(fp))
		FS_CloseFile(fp);
	return ok;
}
static bool KW_TestReadSave(void)
{
	FS_File fp = FS_OpenUserFile(kw_testSaveFile);
	US_Savefile header;
	uint8_t padding;
	kw_loadingFileName = kw_testSaveFile;
	bool ok = FS_IsFileValid(fp) &&
		FS_Read(header.id, sizeof(header.id), 1, fp) == 1 &&
		FS_ReadInt16LE(&header.printXOffset, 1, fp) == 1 &&
		FS_ReadBoolFrom16LE(&header.used, 1, fp) == 1 &&
		FS_Read(header.name, sizeof(header.name), 1, fp) == 1 &&
		FS_Read(&padding, 1, 1, fp) == 1 &&
		CK_LoadGame(fp, false);
	if (FS_IsFileValid(fp))
		FS_CloseFile(fp);
	return ok && !KW_FinishDeferredLoad();
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
		// Its completed levels are peaceful, by its own lists.
		for (int lv = 2; lv < 27; ++lv)
		{
			if (!CA_MapExists(lv) || lv == 21 /* the high scores */)
				continue;
			ck_gameState.currentLevel = lv;
			ck_gameState.levelsDone[lv] = 1;
			CK_LoadLevel(true, true);
			int hostiles, residents;
			CK_object *first;
			KW_CountLevelPeople(&hostiles, &residents, &first);
			if (!KW_IsLevelPeaceful() || hostiles)
				QuitF("KW_SelfTest: completed Foray in the Forest level %d isn't peaceful (%d hostile)!", lv, hostiles);
			ck_gameState.levelsDone[lv] = 0;
		}
		// The map with every level done: flags stand, nothing breaks.
		for (int lv = 1; lv < 27; ++lv)
			ck_gameState.levelsDone[lv] = 3; // done and perfect
		ck_gameState.currentLevel = 0;
		CK_LoadLevel(true, true);
		KW_TestLevelSpritesCached("fitf", 0);
		for (int lv = 1; lv < 27; ++lv)
			ck_gameState.levelsDone[lv] = 0;
		// A game saved inside one of its levels loads back with its state
		// and every object in its place.
		for (int lv = 2; lv <= 6; lv += 4)
		{
			ck_gameState.currentLevel = lv;
			CK_LoadLevel(true, true);
			const char *wrong = FF_TestSaveRoundTrip(KW_TestWriteSave, KW_TestReadSave);
			if (wrong)
				QuitF("KW_SelfTest: Foray in the Forest save in level %d: %s!", lv, wrong);
		}
		remove(kw_testSaveFile);
		// Arriving by Megarocket puts Keen beside the landed ship in its BwB level.
		const KW_Ship *ffShip = KW_ShipFor(KW_FITF);
		kw_placeArrivalAtShip = true;
		ck_gameState.currentLevel = ffShip->level;
		CK_LoadLevel(true, true);
		if (!KW_IsAtShipHatch() || RF_UnitToTile(ck_keenObj->posX) != ffShip->arrivalX)
			Quit("KW_SelfTest: Foray in the Forest's ship arrival is wrong!");
		KW_TestLevelSpritesCached("fitf", ffShip->level);
		KW_SwitchWorld(KW_KEEN4);
	}

	// The Megarocket, Keen Worlds' own world: its room loads with Keen in it.
	if (!KW_IsWorldPresent(KW_SHIP))
		Quit("KW_SelfTest: the Megarocket world (folder megarocket) is missing!");
	KW_SwitchWorld(KW_SHIP);
	ck_gameState.currentLevel = 1;
	CK_LoadLevel(true, true);
	if (!ck_keenObj || !ck_keenObj->currentAction || ca_mapOn != 1)
		Quit("KW_SelfTest: the Megarocket's room has no Keen!");
	KW_TestLevelSpritesCached("megarocket", 1);
	{
		bool console = false, hatch = false, aft = false, fore = false;
		for (int y = 0; y < CA_GetMapHeight(); ++y)
			for (int x = 0; x < CA_GetMapWidth(); ++x)
			{
				console |= CA_TileAtPos(x, y, 2) == KW_SHIP_CONSOLE;
				hatch |= CA_TileAtPos(x, y, 2) == KW_SHIP_HATCH;
				aft |= CA_TileAtPos(x, y, 2) == KW_SHIP_WAY_AFT;
				fore |= CA_TileAtPos(x, y, 2) == KW_SHIP_WAY_FORE;
			}
		if (!console || !hatch)
			Quit("KW_SelfTest: the Megarocket's room has no flight console or no hatch!");
		if (!aft || !fore)
			Quit("KW_SelfTest: the Megarocket's two rooms are not joined!");
	}
	KW_TestRunObjects(60);
	KW_SwitchWorld(KW_KEEN4);

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
