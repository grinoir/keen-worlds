/*
Keen Worlds: travelling between Keen 4, 5 and 6 in one running game.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kw_version.h"
#include "id_ca.h"
#include "id_fs.h"
#include "id_in.h"
#include "id_mm.h"
#include "id_rf.h"
#include <ctype.h>
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_vl_private.h"
#include "id_sd.h"
#include "id_ti.h"
#include "ck_act.h"
#include "ck_config.h"
#include "ck_cross.h"
#include "ck_game.h"
#include "ck_text.h"
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
#include "kw_arcade.h"
#include "kw_mars.h"
#include "kw_alphamatic.h"
#include "kw_story_scenes.h"
#include "kw_suburb.h"
#include "kw_music.h"
#include "kw_skip.h"
#include "kw_art.h"
#include "kw_alphamatic_dock_art.h"
#include "kw_habitat.h"
#include "kw_title_art.h"

extern const char *ck_episodeFile;
extern CK_Difficulty ck_startingDifficulty;

// ---- The world registry ----

// Atroxian Realm's own episode definition (kw_arealm.c): Keen 5's code with
// its own files and level setup.
#define KW_EPISODE_AREALM 10
#define KW_EPISODE_OSI 12 // Operation Station Infiltration (kw_osi.c, kw_os5_*.c)
extern CK_EpisodeDef ar_episode;
// Foray in the Forest's (kw_fitf.c), the same way with extension CKS.
#define KW_EPISODE_FITF 11
#define KW_EPISODE_DREAMS 13 // Keen Dreams (kw_dreams.c, kw_kd_*.c)
extern CK_EpisodeDef kd_episode;
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
		"A fan episode by Bubbatom\n(2011). Download \"The Eight\nAccumulators\" (see its page on\nkeenwiki.shikadi.net) and put\nits files in mods/keen95."},
	// Keen 5.5 (The CK Guy) and Keen Asylum, as converted for Omnispeak in
	// its modding kit (data/mod/ck55, data/mod/asylum): the player copies
	// those folders to play/mods/ck55 and play/mods/asylum.
	{"ck55", "The Omegamatic II", 5, "mods/ck55", "ck55.ck5",
		"The Armageddon Machine\nMark II",
		"Keen 5.5\nA fan episode by The CK Guy",
		"Created by The CK Guy\n\nOmnispeak version:\nDavid Gow (Multimania)" KW_NOT_OURS,
		"A fan episode by The CK Guy.\nGet its Omnispeak version from\nthe Omnispeak modding kit\n(github.com/sulix/omnispeak)\nand put it in mods/ck55."},
	{"asylum", "Address Unknown", 5, "mods/asylum", "ASYLUM.CK5",
		"Keen Asylum",
		"A level pack of tricks and traps\nby the Keen Asylum creators",
		"Created by the\nKeen Asylum creators\n(Keendom)\n\nOmnispeak version:\nDavid Gow (Multimania)" KW_NOT_OURS,
		"A fan level pack. Get its\nOmnispeak version from the\nOmnispeak modding kit\n(github.com/sulix/omnispeak)\nand put it in mods/asylum."},
	// Bounty Hunting Brouhaha (Kohntarkosz_, 2019): a Keen 4 mod with its own
	// levels, graphics, music and rules. tools/kw_bounty.py copies the
	// player's files of it to play/mods/bounty and writes MOD.CK4 from its
	// patch file.
	{"bounty", "Meurglys III", 4, "mods/bounty", "MOD.CK4",
		"Bounty Hunting Brouhaha",
		"A Commander Keen fan game\nby Kohntarkosz_, 2019",
		"Created by Kohntarkosz_\n\nLevels, graphics and music\nare its own" KW_NOT_OURS,
		"A fan game by Kohntarkosz_\n(2019). Download \"Bounty\nHunting Brouhaha\" v1.0 (see\nits page on keenwiki.shikadi.net)\nand put its files in mods/bounty."},
	// The Megarocket: Keen Worlds' own world, the inside of Keen's ship
	// (tools/kw_shipworld.py writes its map; its folder comes with Keen
	// Worlds). It runs on Keen 4 and is not a destination in the menu.
	{"megarocket", "The Megarocket", 4, "megarocket", "SHIP.CK4",
		"The Bean-with-Bacon\nMegarocket",
		"Keen's ship, as drawn for\nKeen Worlds",
		"The inside of the Megarocket\nis Keen Worlds' own art,\nshaped after Keen's ship\nin Commander Keen 4.\n\nIt is built on the tiles of\nCommander Keen 4\n(id Software, 1991).",
		"It comes with Keen Worlds:\nthe folder megarocket is missing\nfrom the game folder."},
	// Tuberia: Keen Dreams, id Software's "lost episode" (1991, published
	// by Softdisk; its source is GPL since 2014). It is the dream Keen has
	// in the Megarocket's bed: not a planet, so not a destination in the
	// menu, and nothing is carried in or out of it. Its code is ported from
	// its source (kw_dreams.c, kw_kd_*.c); kw_kd_convert.c rewrites the
	// player's copy of the game's data in the engine's form.
	{"dreams", "Tuberia", KW_EPISODE_DREAMS, "mods/dreams", "DREAMS.KDR",
		"Commander Keen in\nKeen Dreams",
		"The lost episode\nid Software, 1991",
		"Programming:\nJohn Carmack, John Romero\n\nCreative director: Tom Hall\nArt: Adrian Carmack\n\n"
		"Published by Softdisk.\nIts source code was released\nunder the GPL in 2014." KW_NOT_OURS,
		"Keen Dreams' shareware release\n(version 1.13) is free to pass\non. Put its files in\nmods/dreams."},
	{"mars", "Mars", 5, "arcade/mars", "MARS.CK5",
		"Marooned on Mars - Galaxy", "A Keen 1 retelling\nby Dr. Colossus",
		"Created by Dr. Colossus\n\nBug fixes and optimization:\nK1n9_Duk3\n\nNative arcade integration:\nKeen Worlds" KW_NOT_OURS,
		"Install your copy of\nMarooned on Mars - Galaxy\nin arcade/mars."},
	{"alpham", "The Alphamatic", 5, "mods/alphamatic", "ALPHAM.CK5",
		"The Alphamatic", "Atroxian Realm's sequel\nby Gridlock, version 3.0",
		"Created by Gridlock\n\nPatching: Levellass, Fleex,\nK1n9_Duk3\nMusic: Mr. M and Gridlock"
		"\fTesting: Nisaba, Fleex,\nMobiethian, Levellass, Lemm\n\nFull credits in the game's Help." KW_NOT_OURS,
		"The Alphamatic v3.0 by Gridlock.\nUnpack the original archive in\nmods/alphamatic. See the guide."},
	{"suburb", "Earth", 5, "mods/suburb", "SUBURB.CK5",
		"Suburb Shenanigans", "A school day on Earth\nby Bubbatom, v1.2",
		"Created by Bubbatom\nPatching: Levellass\nMusic: thehackercat, Mr. M\n\n"
		"Bug fixes and optimization:\nK1n9_Duk3 (2020-11-25)\n\nFull credits in the game's Help." KW_NOT_OURS,
		"Suburb Shenanigans v1.2.\nUnpack your copy in mods/suburb.\nSee the local testing guide."},
	// Return to the Shadowlands (Ceilick; programmed by K1n9_Duk3, v1.8
	// 2022): a source mod of the reconstructed Keen 4 source, GPL. The
	// player's copy of its files goes in play/mods/rtts, beside our
	// RTTS.CK4 (kwtools/kw_rtts.py); its code is in kw_rtts.c.
	{"rtts", "Far Shadowlands", 4, "mods/rtts", "RTTS.CK4",
		"Return to the Shadowlands",
		"The Spot story, episode one\nA fan game by Ceilick, 2021-22",
		"Created by Ceilick:\nlevels, story, graphics\nand sound effects\n\nProgramming: K1n9_Duk3\n\n"
		"Built on K1n9_Duk3's\nreconstructed Keen 4 source" KW_NOT_OURS,
		"Return to the Shadowlands v1.8\nby Ceilick (2022-12-21). Unpack\nit (see its page on\nkeenwiki.shikadi.net) in\nmods/rtts."},
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
	case KW_EPISODE_DREAMS:
		return &kd_episode;
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

// Is this file in the world's folder (or in a folder inside it)?
static bool KW_WorldHasFile(int world, const char *name)
{
	FS_File f = FS_OpenWorldFile(KW_WorldPath(world), name);
	if (!FS_IsFileValid(f))
		return false;
	FS_CloseFile(f);
	return true;
}

const char *KD_ConvertGame(const char *dir);
bool KD_IsConverted(const char *dir);
static const char *kw_dreamProblem; // why Keen Dreams isn't here, if it was looked for

static bool KW_FindWorld(int world)
{
	const KW_WorldDef *def = &kw_worldDefs[world];
	CK_EpisodeDef *ep = KW_EpisodeDef(def->episode);
	if (!ep)
		return false;
	if (!def->dir)
		return ep->isPresent();
	// A fan game: its folder must hold its variable file, which comes
	// with Keen Worlds, and the game itself, which the player puts there
	// (told by one file of the game's own). Omnispeak's support files for
	// the base episode come from the game path.
	if (!KW_WorldHasFile(world, def->episodeFile))
		return false;
	if (world == KW_MARS)
		return KW_WorldHasFile(world, "editmaps.ck5") &&
			KW_WorldHasFile(world, "EGAGRAPH.CK5") && KW_WorldHasFile(world, "AUDIO.CK5");
	if (def->episode == KW_EPISODE_AREALM || def->episode == KW_EPISODE_FITF)
	{
		// Its support files are in its own folder too; Keen 5's variables
		// (Omnispeak's own files) are its base.
		return KW_WorldHasFile(world, def->episode == KW_EPISODE_FITF ? "MAPTEMP.CKS" : "maptemp.ckd")
			&& FS_IsOmniFilePresent("EPISODE.CK5");
	}
	if (def->episode == KW_EPISODE_DREAMS)
	{
		// The engine reads the game in a form of its own, written here the
		// first time the game is found (kw_kd_convert.c). Keen 4's
		// variables (Omnispeak's own files) are its base.
		// Written again whenever any of them is missing, so that a part of
		// them (or a GAMEMAPS.KDR of some other copy) is never taken for all.
		if (!KD_IsConverted(KW_WorldPath(world)))
		{
			kw_dreamProblem = KD_ConvertGame(KW_WorldPath(world));
			if (kw_dreamProblem)
				CK_Cross_LogMessage(CK_LOG_MSG_WARNING, "Keen Dreams in %s: %s\n", KW_WorldPath(world), kw_dreamProblem);
		}
		return KD_IsConverted(KW_WorldPath(world)) && FS_IsOmniFilePresent("EPISODE.CK4");
	}
	if (def->episode == KW_EPISODE_OSI)
	{
		// everything is its own, in its own folder
		return KW_WorldHasFile(world, "GAMEMAPS.CK5");
	}
	if (!strncmp(def->dir, "mods/", 5))
	{
		char graphics[16];
		snprintf(graphics, sizeof(graphics), "EGAGRAPH.CK%d", def->episode);
		if (!KW_WorldHasFile(world, graphics))
			return false;
	}
	return FS_IsOmniFilePresent(ep->episodeFile);
}

bool KW_IsWorldPresent(int world)
{
	// Looked for once: the folders don't change while the game runs.
	static int8_t found[KW_NUM_WORLDS];
	if (world < 0 || world >= KW_NUM_WORLDS)
		return false;
	if (!found[world])
		found[world] = KW_FindWorld(world) ? 1 : -1;
	return found[world] > 0;
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
// Flown to from the ship's controls: when the fly-in has landed, Keen is
// still inside the Megarocket (the computer says where he is, the hatch is
// the way out), as on every other world.
static bool kw_fitfLandsInShip = false;
static bool kw_fitfNoFlyIn = false;  // stepping out of a ship that has landed already
static bool kw_fitfCardDue = false;  // its title and makers are still to be shown, when Keen steps out

// Megarockets parked on fan games' world maps (see below).
typedef struct KW_MapShip KW_MapShip;
static const KW_MapShip *KW_MapShipFor(int world);
static bool KW_TransportedAlone(int world);

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
	// The Shikadi Station: the ship in the hangar of its own Megarocket level
	// (18, entered from its map where the ship is docked), where that level
	// starts Keen at its nose.
	{KW_OSI, 18, 3, 38, 3, 33, 18, 39},
	// Meurglys III: the ship (WASH ME on its side) in the woods of its own
	// Megarocket level, where that level starts Keen at its nose.
	{KW_BOUNTY, 18, 3, 13, 3, 9, 18, 14},
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
	// The original covered Megarocket in Billy's backyard clubhouse.
	{KW_SUBURB, 10, 28, 14, 14, 11, 29, 17},
	// Far Shadowlands: its Megarocket level (20) is Keen 4's, five rows lower.
	{KW_RTTS, 20, 3, 17, 3, 14, 19, 19},
};
// Foray's level 1: the forest with the landed ship is the top of the map;
// the game's own rooms of the ship start below this row.
#define KW_FITF_ROOMS_Y 20


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

static void KW_Finale(void);
static bool KW_CardsOff(void);

void KW_AfterWorldEnding(void)
{
	// help_endgame() pushes a cache level and, as in the original, never
	// pops it: the original quit to the title afterwards. Pop it here.
	CA_DownLevel();

	// Calidune, saved for the first time: Keen flies up in the copy of his
	// ship and meets the Megarocket.
	if (kw_currentWorld == KW_KEEN8 && !kw_worlds[KW_KEEN8].completed && !KW_CardsOff())
		CD_PlayEnding();
	// The Shikadi Station, the first time: on to Fribbulus Xax, and Spot.
	if (kw_currentWorld == KW_OSI && !kw_worlds[KW_OSI].completed && !KW_CardsOff())
		SP_PlayRescue();
	// Krisle: the Krile's knock-off ship. The Alphamatic: the messages that
	// lead on to Gnosticus IV and home.
	if (kw_currentWorld == KW_AREALM && !kw_worlds[KW_AREALM].completed && !KW_CardsOff())
		KR_PlayNewShip();
	if (kw_currentWorld == KW_ALPHAMATIC && !kw_worlds[KW_ALPHAMATIC].completed && !KW_CardsOff())
		AB_PlayMessages();
	kw_worlds[kw_currentWorld].completed = true;
	if (!ck_gameState.levelsDone[ca_mapOn])
		ck_gameState.levelsDone[ca_mapOn] = 1;
	ck_gameState.currentLevel = 0;
	if (MARS_IsActive())
	{
		// The arcade's ending is its own; completion cannot unlock planets.
		if (!KW_ArcadeOver(true)) ck_gameState.levelState = LS_AbortGame;
		return;
	}

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
	// That was the last world: Keen Worlds' own ending follows its ending.
	KW_Finale();
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
// Reserved save word 215 identifies the story-aware course rules. Old saves
// are migrated once, retaining courses that were open before the reorder.
#define KW_CHRONICLE_RULES 0x4352
static void KW_KeepLegacyCourses(const KW_WorldState *worlds, const bool *known);
static bool kw_crashLanding = false;      // the ship is being shot down over Krisle (see KW_CrashLanding)
static int kw_dreamEnded = 0;             // Keen has just woken: 1 as usual, 2 the dream was lost, 3 it was won
static bool kw_epilogueBackdrop = false;  // the last-world-saved crawl has its own sky
// Tuberia is a dream and the ship is the ship: the rest are places to fly to.
static bool KW_IsPlanet(int world)
{
	return world != KW_SHIP && world != KW_DREAM && world != KW_MARS;
}
static void KW_DreamLeaving(int from);

// (The dream: see "The bed" below.)
typedef struct KW_Tally
{
	int32_t score, nextKeenAt;
	int16_t lives, shots;
} KW_Tally;
static KW_Tally kw_awake;        // Keen's own, while he dreams
static KW_Tally kw_dreamTally;   // the dream's, while he is awake
typedef struct KW_ArcadeTally
{
	KW_Tally tally;
	int16_t centilife, difficulty;
} KW_ArcadeTally;
static KW_ArcadeTally kw_journeyTally, kw_marsTally;
static bool kw_marsLinked, kw_arcadeCabinDue, kw_arcadeFadeIn, kw_cabinReturnRequested;
static int16_t kw_arcadeCabinX, kw_arcadeCabinY, kw_arcadeCabinDirection;
static void KW_WakeUp(void);
static void KW_ArcadeFadeOut(void);
static void KW_ArcadeLeaving(int from);
static void KW_ArcadeArriving(int to);
static bool kw_wakeInBed = false; // the next arrival in the ship is in the bunk
static bool kw_shipUpUsed = false; // "up" has done its one thing in the ship's rooms and is still held
void KD_NewGame(void);
void KD_GetState(int16_t *out);
void KD_SetState(const int16_t *in);
bool KD_IsVegetable(const CK_object *ob);
void KD_ChangeToFlower(CK_object *ob);
bool KD_IsActive(void);

static void KW_PackTally(const KW_Tally *t, int16_t *out)
{
	out[0] = t->score & 0xFFFF;
	out[1] = (t->score >> 16) & 0xFFFF;
	out[2] = t->nextKeenAt & 0xFFFF;
	out[3] = (t->nextKeenAt >> 16) & 0xFFFF;
	out[4] = t->lives;
	out[5] = t->shots;
}

static void KW_UnpackTally(KW_Tally *t, const int16_t *in)
{
	t->score = (uint16_t)in[0] | ((int32_t)in[1] << 16);
	t->nextKeenAt = (uint16_t)in[2] | ((int32_t)in[3] << 16);
	t->lives = in[4];
	t->shots = in[5];
}

static void KW_DreamArriving(int to);
const char *kw_loadingFileName = NULL;
static int kw_deferredWorld = KW_NO_WORLD;
static char kw_deferredFile[FS_MAX_FILENAME_LEN];

static int KW_WorldById(const char *id)
{
	if (!strncmp(id,"alphamatic",8)) return KW_ALPHAMATIC;
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
	// (from word 170: the dream - Keen's own tally while he dreams, the
	// dream's while he is awake, and Keen Dreams' bombs and keys)
	KW_PackTally(&kw_awake, &extra[170]);
	KW_PackTally(&kw_dreamTally, &extra[176]);
	KD_GetState(&extra[182]);
	// Reserved version-7 words: the in-engine arcade keeps its own tally.
	// A marker distinguishes these fields from old scenery/save padding.
	KW_PackTally(&kw_journeyTally.tally, &extra[190]);
	extra[196]=kw_journeyTally.centilife; extra[197]=kw_journeyTally.difficulty;
	KW_PackTally(&kw_marsTally.tally, &extra[198]);
	extra[204]=kw_marsTally.centilife; extra[205]=kw_marsTally.difficulty;
	extra[210]=0x4152; extra[211]=kw_marsLinked;
	extra[212]=kw_arcadeCabinX; extra[213]=kw_arcadeCabinY;
	extra[214]=kw_arcadeCabinDirection;
	extra[215]=KW_CHRONICLE_RULES;
	return AR_WriteState(fp) && FF_WriteState(fp) && KW_WriteId(fp, kw_worldDefs[kw_boardedFrom].id) &&
		FS_WriteInt16LE(extra, 261, fp) == 261;
}

bool KW_ReadSaveBlock(FS_File fp, bool *deferred)
{
	char id[4];
	int16_t version;
	KW_WorldState worlds[KW_NUM_WORLDS];
	bool known[KW_NUM_WORLDS] = {0};
	int world = KW_NO_WORLD;
	memset(worlds, 0, sizeof(worlds));
	*deferred = false;

	if (FS_Read(id, 4, 1, fp) != 1 || memcmp(id, KW_SAVE_BLOCK_ID, 4) ||
		FS_ReadInt16LE(&version, 1, fp) != 1)
		return false;

	if (version < 7) kw_marsLinked=false;
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
			{
				worlds[w] = state;   // worlds that are no longer known are dropped
				known[w] = true;
			}
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
			if (w != KW_NO_WORLD && KW_IsPlanet(w))
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
			if (kw_chronicle && extra[215] != KW_CHRONICLE_RULES)
				KW_KeepLegacyCourses(worlds, known);
			OSI_SetState(&extra[90]);
			FF_SetSongsHeard(&extra[100], 64);
			KW_UnpackTally(&kw_awake, &extra[170]);
			KW_UnpackTally(&kw_dreamTally, &extra[176]);
			KD_SetState(&extra[182]);
			kw_marsLinked=extra[210]==0x4152 && extra[211]==1;
			if (kw_marsLinked)
			{
				KW_UnpackTally(&kw_journeyTally.tally,&extra[190]);
				kw_journeyTally.centilife=extra[196]; kw_journeyTally.difficulty=extra[197];
				KW_UnpackTally(&kw_marsTally.tally,&extra[198]);
				kw_marsTally.centilife=extra[204]; kw_marsTally.difficulty=extra[205];
				kw_arcadeCabinX=extra[212]; kw_arcadeCabinY=extra[213];
				kw_arcadeCabinDirection=extra[214];
			}
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
		if (MARS_IsActive()) ck_gameState.numShots = 0;
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
	kw_marsLinked=kw_arcadeCabinDue=kw_arcadeFadeIn=kw_cabinReturnRequested=false;
	kw_marsTally=(KW_ArcadeTally){{0,20000,3,0},0,D_Normal};
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
	KW_DreamLeaving(kw_currentWorld); // out of the dream: Keen's own score, lives and shots again
	KW_ArcadeLeaving(kw_currentWorld);
	if (world!=KW_SHIP) kw_arcadeCabinDue=false;
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
	kw_fitfFlyIn = world == KW_FITF && !kw_worlds[world].visited && !kw_fitfNoFlyIn;
	if (world == KW_FITF && kw_fitfNoFlyIn && !kw_worlds[world].visited)
		ck_gameState.levelsDone[1] = 1; // (its level of the landed ship: its map then shows the ship standing)
	kw_fitfNoFlyIn = false;
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
	KW_DreamArriving(world); // into the dream: its own score, lives and flower power
	KW_ArcadeArriving(world);
}

// ---- The Megarocket ----

static int kw_launchSpotX = -1, kw_launchSpotY = -1;
static bool kw_launchRequested = false;

// How far (in tiles) from the launch spot Keen may stand to board the rocket.
#define KW_LAUNCH_RANGE 1

// Alphamatic v3.0's shuttle sits five tiles east of Keen's map start. The
// gangway is fitted to that route, like the walkway on Keen 5's dock.
#define KW_ALPHAMATIC_GANGWAY_DX 2
#define KW_ALPHAMATIC_SHUTTLE_DX 5

static bool KW_AlDockSolidCell(int x, int y);

static KW_Art kw_alDockArt = {
	0, 0, KW_AL_DOCK_TILES_W, KW_AL_DOCK_TILES_H, 1,
	&kw_alDockSlots[0][0][0], &kw_alDockTiles[0][0][0][0],
	1, 0, NULL, false, false, KW_AlDockSolidCell, false,
};

static bool KW_AlDockSolidCell(int x, int y)
{
	// A solid deck under the bridge keeps Keen from falling into space; the
	// nose cell stops him beside the shuttle, as Keen 5's gate does.
	return y == kw_alDockArt.originY + 2 ||
		(x == kw_alDockArt.originX + 3 && y == kw_alDockArt.originY + 1);
}

static void KW_BuildAlphamaticDock(int spawnX, int spawnY)
{
	kw_alDockArt.originX = spawnX + KW_ALPHAMATIC_GANGWAY_DX;
	kw_alDockArt.originY = spawnY - 1;
	KW_PlaceArt(&kw_alDockArt);
}


// ---- Keen 5 boarding gate ----
// Art drawn by tools/kw_gate_art.py, placed by kw_art.c. It covers map tiles
// 29-31 x 94-96: the dock edge, the gap and the ship's nose. Three animation
// frames run the lights; Keen walks along the middle row.

#include "kw_gate_art.h"

// The gate also stands on the maps of the level packs that keep Keen 5's
// dock: The Omegamatic II (Keen 5.5: the same map) and Address Unknown
// (Asylum: the same dock, elsewhere on a larger map). Their levels use none
// of the gate's tile slots. kw_gateDX/DY: how far this world's dock is
// from Keen 5's.
static int kw_gateDX, kw_gateDY;

static bool KW_HasKeen5Dock(void)
{
	kw_gateDX = kw_currentWorld == KW_ASYLUM ? 21 : 0;
	kw_gateDY = kw_currentWorld == KW_ASYLUM ? -22 : 0;
	return kw_currentWorld == KW_KEEN5 || kw_currentWorld == KW_CK55 || kw_currentWorld == KW_ASYLUM;
}

// The solid gangway knob is replaced, not drawn over.
static bool KW_GateDropsUnderlay(int x, int y)
{
	x -= kw_gateDX;
	y -= kw_gateDY;
	return y == 95 && (x == 29 || x == 30);
}

// Where the gangway meets the ship's nose (tile 31 of the walkway row) is
// solid, so Keen stops right beside the ship instead of walking onto it.
#define KW_KEEN5_SHIP_NOSE_X 31
static bool KW_GateNoseIsSolid(int x, int y)
{
	return x - kw_gateDX == KW_KEEN5_SHIP_NOSE_X && y - kw_gateDY == KW_GATE_ORIGIN_Y + 1;
}

static const KW_Art kw_gateArt = {
	KW_GATE_ORIGIN_X, KW_GATE_ORIGIN_Y, KW_GATE_TILES_W, KW_GATE_TILES_H, KW_GATE_FRAMES,
	&kw_gateSlots[0][0][0], &kw_gateTiles[0][0][0][0],
	1,  // walkway row
	12, // tics per frame
	KW_GateDropsUnderlay,
	false, false, KW_GateNoseIsSolid,
};

static const KW_Art kw_asylumGateArt = {
	KW_GATE_ORIGIN_X + 21, KW_GATE_ORIGIN_Y - 22, KW_GATE_TILES_W, KW_GATE_TILES_H, KW_GATE_FRAMES,
	&kw_gateSlots[0][0][0], &kw_gateTiles[0][0][0][0],
	1, 12, KW_GateDropsUnderlay,
	false, false, KW_GateNoseIsSolid,
};

static void KW_BuildKeen5Gate(void)
{
	KW_PlaceArt(kw_currentWorld == KW_ASYLUM ? &kw_asylumGateArt : &kw_gateArt);
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
	if (KW_TransportedAlone(world))
		return NULL; // (the ship comes to Calidune only once Keen has left it)
	for (int i = 0; i < (int)(sizeof(kw_mapShips) / sizeof(kw_mapShips[0])); ++i)
		if (kw_mapShips[i].world == world)
			return &kw_mapShips[i];
	return NULL;
}

void KW_SetLaunchSpot(int tileX, int tileY)
{
	if (MARS_IsActive()) { kw_launchSpotX = kw_launchSpotY = -1; return; }
	if (KW_HasKeen5Dock())
	{
		// Replace Keen 5's unused yellow map border with a sparse starfield
		// tile already used beside the dock. This affects only the loaded map.
		for (int y = 0; kw_currentWorld == KW_KEEN5 && y < CA_GetMapHeight(); ++y)
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
	else if (KW_MapShipFor(kw_currentWorld))
	{
		// A Megarocket parked on the map (our art).
		const KW_MapShip *ms = KW_MapShipFor(kw_currentWorld);
		KW_PlaceArt(ms->ship);
		kw_launchSpotX = ms->launchX;
		kw_launchSpotY = ms->launchY;
	}
	else
	{
		if (kw_currentWorld == KW_ALPHAMATIC)
		{
			// Build the bridge from the real map start to the visible shuttle.
			// The map's shuttle is at start + 5 tiles; the boarding point is
			// beside its nose, just as on Keen 5.
			KW_BuildAlphamaticDock(tileX, tileY);
			kw_launchSpotX = tileX + KW_ALPHAMATIC_SHUTTLE_DX;
			kw_launchSpotY = tileY;
		}
		else
		{
			// Other worlds without a dedicated dock return to their arrival.
			kw_launchSpotX = tileX;
			kw_launchSpotY = tileY;
		}
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
	if (kw_arcadeCabinDue && kw_currentWorld==KW_SHIP && ck_keenObj)
	{
		kw_arcadeCabinDue=false;
		ck_keenObj->posX=kw_arcadeCabinX; ck_keenObj->posY=kw_arcadeCabinY;
		ck_keenObj->xDirection=kw_arcadeCabinDirection;
		CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_keenStanding));
		CK_ResetClipRects(ck_keenObj);
		return;
	}
	// Waking from the dream: in the bunk, not at the hatch.
	if (kw_wakeInBed && kw_currentWorld == KW_SHIP && ck_keenObj)
	{
		kw_wakeInBed = false;
		for (int y = 0; y < CA_GetMapHeight(); ++y)
			for (int x = CA_GetMapWidth() - 1; x >= 0; --x)
				if (CA_TileAtPos(x, y, 2) == 0x7706 /* KW_SHIP_BED */)
				{
					ck_keenObj->posX = RF_TileToUnit(x - 1); // the middle of the bunk
					ck_keenObj->posY = RF_TileToUnit(y) - 241;
					ck_keenObj->xDirection = IN_motion_Left;
					CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenStanding));
					CK_ResetClipRects(ck_keenObj);
					return;
				}
		return;
	}
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
	if (!KW_HasKeen5Dock() || ca_mapOn != 0)
		return;

	int x = (mapKeen->posX >> 8) - kw_gateDX;
	// Once Keen is on the bridge, it is a horizontal boarding route only.
	if (x >= 29)
		mapKeen->yDirection = IN_motion_None;
	// (The ship's nose is solid: Keen stops right beside it.)
}

void KW_TryLaunch(CK_object *mapKeen)
{
	if (ck_inputFrame.yDirection != IN_motion_Up || kw_launchSpotX < 0)
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
static bool KW_CardsOff(void) {return kw_cardsOff;}
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
	if (!kw_cardsOff && !getenv("KW_AUTOFINALE")) // developer runs have nobody to press a key
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
// title sequence, which a traveller never sees.) Hold Escape to skip it.
bool CK_DrawStarWarsEx(bool handleDemoKeys);
bool FF_IsActive(void);
void FF_LevelPalette(void);
bool KW_DrawWorldTitlePicture(void);
void StartMusic(int16_t level);

// Should the current world's story be shown now? (Once a game for a world
// Keen has not been to; not in the scripted developer runs unless asked.)
static bool KW_StoryIsDue(int world)
{
	if (!KW_IsPlanet(world) || kw_storyShown[world] || kw_worlds[world].visited)
		return false;
	return !kw_cardsOff || getenv("KW_AUTOSTORY");
}

// Two worlds came with no story: Bounty Hunting Brouhaha switches its
// base game's off, and Keen Asylum says in its Read Me that it has none.
// Left alone they would scroll the base game's story (Keen 4's, Keen 5's),
// which is about somewhere else. They get a page from the ship's chart
// instead, in Keen Worlds' words and saying so: it tells nothing about
// the game that the game's title and its makers' notes don't.
// (The narrator's clips: tools/kw_story_voice.py reads these.)
static const char kw_chartBounty[] =
	"Meurglys III\n"
	"\n"
	"From the ship's chart\n"
	"\n"
	"This world came with\n"
	"no story of its own.\n"
	"\n"
	"The chart has a name,\n"
	"a mountain, and a\n"
	"title written across\n"
	"the top in large\n"
	"letters:\n"
	"\n"
	"Bounty Hunting\n"
	"Brouhaha.\n"
	"\n"
	"Keen has been called\n"
	"many things. This is\n"
	"a new one.\n"
	"\n"
	"He will find out what\n"
	"it means down there.\n";

static const char kw_chartAsylum[] =
	"Address Unknown\n"
	"\n"
	"From the ship's chart\n"
	"\n"
	"This place came with\n"
	"no story and no name.\n"
	"\n"
	"Its builders left a\n"
	"note instead: the\n"
	"rooms in here were\n"
	"made to show what a\n"
	"level can do. They\n"
	"were not made to be\n"
	"kind.\n"
	"\n"
	"Keen likes a puzzle.\n"
	"\n"
	"Mostly.\n";

static const char *KW_ChartStory(int world)
{
	return world == KW_BOUNTY ? kw_chartBounty : world == KW_ASYLUM ? kw_chartAsylum : NULL;
}

// The scroll itself, for the current world (whose graphics must be loaded).
static void KW_PlayWorldStory(void)
{
	extern const char *ck_starWarsTextOverride;
	const char *chart = KW_ChartStory(kw_currentWorld);
	kw_storyShown[kw_currentWorld] = true;
	// The game's own title first, for a few seconds or until a key.
	if (!getenv("KW_AUTOSHOT") || getenv("KW_AUTOSTORY"))
	{
		KW_StopVoice();
		VL_ClearScreen(0);
		VL_SetScrollCoords(0, 0);
		VL_SetDefaultPalette();
		bool ownTitle = KW_DrawWorldTitlePicture();
		if (!ownTitle)
			CA_CacheGrChunk(CK_CHUNKNUM(PIC_TITLESCREEN));
		if (ownTitle || ca_graphChunks[CK_CHUNKNUM(PIC_TITLESCREEN)])
		{
			printf("KW_AUTOSTORY: the title of %s\n", kw_worldDefs[kw_currentWorld].id);
			// (a title of Keen Worlds' own is drawn already: the game's,
			// which for Address Unknown is Keen 5's, must not cover it)
			if (!ownTitle)
				VH_DrawBitmap(0, 0, CK_CHUNKNUM(PIC_TITLESCREEN));
			VH_UpdateScreen();
			IN_ClearKeysDown();
			KW_WaitPresentation(5 * 70, false);
			IN_ClearKeysDown();
		}
		VL_ClearScreen(0);
	}
	if (!chart && !*CK_STRING(ck_str_storyText))
		return;
	printf("KW_AUTOSTORY: the story of %s\n", kw_worldDefs[kw_currentWorld].id);
	if (kw_currentWorld==KW_ALPHAMATIC)
	{
		AL_PlayOpening();
		return;
	}
	KW_StopVoice();
	SD_MusicOff();
	SD_FadeAdLibVolume(256, 0);
	IN_ClearKeysDown();
	// The narrator reads it (STORY_<ID>.RAW, tools/kw_story_voice.py), in
	// place of the story's music; the scroll waits for him to finish unless
	// Escape is held to skip.
	extern bool ck_starWarsNarrated;
	char clip[40];
	snprintf(clip, sizeof(clip), "STORY_%s.RAW", kw_worldDefs[kw_currentWorld].id);
	for (char *c = clip; *c && *c != '.'; ++c)
		*c = (char)toupper((unsigned char)*c);
	// Calidune, the first time: its own tune, quietly under the narrator.
	bool ownTune = kw_currentWorld == KW_KEEN8 && !kw_worlds[KW_KEEN8].completed;
	if (ownTune)
		KW_StartCaliduneMusic(KW_VoicesEnabled() && KW_VoiceVolume() ? 64 : 256);
	ck_starWarsNarrated = KW_Narrate(clip) || ownTune;
	// A chart page is Keen Worlds' own text, so it scrolls over Keen
	// Worlds' own picture, as its own story does, not over the world's.
	// (Bounty Hunting Brouhaha's picture for this, which its game never
	// shows, is a note to people taking the mod apart: "HEY YOU! STOP
	// TEARING MY MOD APART! haha jk have fun".)
	ck_starWarsTextOverride = chart;
	extern bool ck_starWarsKeepMusic;
	ck_starWarsKeepMusic = ownTune; // (it plays on into the arrival scene)
	CK_DrawStarWarsEx(false);
	ck_starWarsKeepMusic = false;
	ck_starWarsTextOverride = NULL;
	if (chart)
		VL_SetCustomColors(NULL);
	ck_starWarsNarrated = false;
	KW_StopVoice();
	// Keen Worlds' own scenes after a story: why Keen hunts Rick Uller, and
	// the Shikadi signal that sends him to Gnosticus IV (Keen 4's story
	// has it on his radio at home; here he is already in the ship).
	if (kw_currentWorld == KW_BOUNTY)
		BH_PlayBountyNotice();
	if (kw_currentWorld == KW_KEEN4)
		K4_PlayShikadiSignal();
	if (ownTune)
	{
		// Mortimer's transporter takes Keen, alone, down to the sand.
		CD_PlayArrival();
		SD_MusicOff();
		SD_FadeAdLibVolume(256, 0);
	}
	IN_ClearKeysDown();
}

// In a level (Keen came to a world some other way than by choosing it at
// the ship's controls): the story, then back to the level.
extern CK_object *ck_scoreBoxObj;
void CK_UpdateScoreBox(CK_object *scorebox);

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
	return (where == KW_AREALM || where == KW_KEEN8) && !kw_worlds[where].completed;
}

// Calidune (Dead in the Desert): Mortimer's transporter from Krodacia sets
// Keen down there alone, "without his trusty Bean with Bacon Megarocket".
// He leaves in the strange copy of it at the end of the game; until then
// the Megarocket can't reach him.
static bool KW_TransportedAlone(int world)
{
	return world == KW_KEEN8 && !kw_worlds[KW_KEEN8].completed;
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
#define KW_CALIDUNE_WARNING \
	"Calidune\n\nMortimer's transporter still has\na lock on Keen. If we go, it\nwill pull him down alone. The\nMegarocket cannot follow.\n\n" \
	"Keen cannot leave Calidune\nuntil he finds a way off.\n\nFly there? Press Y for yes,\nany other key to stay."
#define KW_CALIDUNE_ALONE \
	"The Megarocket is not here.\n\nMortimer's transporter left Keen\non Calidune alone. He needs a\npower source, and a ship.\n\nUntil then Keen stays on Calidune."
// What Keen is told when he tries to leave a world that strands him.
static const char *KW_StrandedText(void)
{
	int where = kw_currentWorld == KW_SHIP ? kw_boardedFrom : kw_currentWorld;
	return where == KW_KEEN8 ? KW_CALIDUNE_ALONE : KW_KRISLE_WRECK;
}

// Draws the destination menu and returns the chosen world, or KW_NO_WORLD to
// stay. Every other world is listed: one that isn't installed opens a card
// saying where to get it. The last line is "About this world".
#define KW_MENU_ABOUT (-2)
// ---- The Chronicle ----
// The story's order: the Mortimer line, Keen 4 to Keen 9, and the journey
// home that follows it (The Eight Accumulators), each opened by saving the
// one before. Off the line, a world is bound to the time of one world of it
// or to none. Krisle, the Omegamatic II and Address Unknown follow Keen 5.
// Spot's rescue on the Shikadi Station waits until the journey home, and
// Alphamatic requires both that journey and Krisle. Gnosticus Pi and
// Meurglys III remain free. Missing worlds are passed over; known courses
// and places Keen has visited stay open. See STORY_SEAMS.md in the workspace.
static const int kw_chronicleLine[] = {KW_KEEN4, KW_KEEN5, KW_KEEN6, KW_KEEN7, KW_KEEN8, KW_KEEN9, KW_KEEN95, KW_SUBURB};
#define KW_CHRONICLE_LINE ((int)(sizeof(kw_chronicleLine) / sizeof(kw_chronicleLine[0])))

static bool KW_WorldIsSaved(int world)
{
	return kw_worlds[world].completed;
}

static int KW_LastChronicleWorld(void)
{
	// Later Earth chapters do not become new requirements of side adventures.
	int end=0;
	while(end<KW_CHRONICLE_LINE-1 && kw_chronicleLine[end]!=KW_KEEN95) ++end;
	for (int i = end; i >= 0; --i)
		if (KW_IsWorldPresent(kw_chronicleLine[i])) return kw_chronicleLine[i];
	return KW_NO_WORLD;
}

static void KW_KeepLegacyCourses(const KW_WorldState *worlds, const bool *known)
{
	// These three courses moved. Preserve even unvisited courses in old
	// Chronicles, without opening worlds added after that save was made.
	if (known[KW_AREALM]) kw_courseTold[KW_AREALM] = 1;
	if (known[KW_OSI] && (!KW_IsWorldPresent(KW_KEEN5) || worlds[KW_KEEN5].completed))
		kw_courseTold[KW_OSI] = 1;
	if (known[KW_ALPHAMATIC] && (!KW_IsWorldPresent(KW_AREALM) || worlds[KW_AREALM].completed))
		kw_courseTold[KW_ALPHAMATIC] = 1;
}

// The next prerequisite (KW_NO_WORLD: none). When all are saved, retain a
// prerequisite so the ship still announces a newly learned course.
static int KW_ChronicleKey(int world)
{
	switch (world)
	{
	case KW_RTTS:
		return KW_LastChronicleWorld();
	case KW_OSI:
		// Spot's story: the Oracle names the Omegamatrix station at the end
		// of Return to the Shadowlands, so the station follows it.
		if (KW_IsWorldPresent(KW_RTTS) && !KW_WorldIsSaved(KW_RTTS)) return KW_RTTS;
		return KW_LastChronicleWorld();
	case KW_CK55:
	case KW_ASYLUM:
	case KW_AREALM:
		return KW_IsWorldPresent(KW_KEEN5) ? KW_KEEN5 : KW_NO_WORLD;
	case KW_ALPHAMATIC:
		if (KW_IsWorldPresent(KW_AREALM) && !KW_WorldIsSaved(KW_AREALM)) return KW_AREALM;
		return KW_LastChronicleWorld() != KW_NO_WORLD ? KW_LastChronicleWorld() :
			KW_IsWorldPresent(KW_AREALM) ? KW_AREALM : KW_NO_WORLD;
	case KW_FITF:
	case KW_BOUNTY:
		return KW_NO_WORLD;
	}
	int before = KW_NO_WORLD;
	for (int i = 0; i < KW_CHRONICLE_LINE; ++i)
	{
		if (kw_chronicleLine[i] == world)
			return before;
		if (KW_IsWorldPresent(kw_chronicleLine[i]))
			before = kw_chronicleLine[i];
	}
	return before;
}

static bool KW_CourseIsOpen(int world)
{
	if (!kw_chronicle || !KW_IsPlanet(world) || kw_worlds[world].visited ||
		kw_worlds[world].completed || kw_courseTold[world] || world == kw_boardedFrom)
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
void KW_PlayOwnStory(bool handleDemoKeys);
void StopMusic(void);

static void KW_ChooseJourney(void)
{
	kw_chronicle = getenv("KW_AUTOCHRONICLE") != NULL;
	if (getenv("KW_AUTOOWNSTORY")) // developer tool: Keen Worlds' story in a scripted run
		KW_PlayOwnStory(false);
	if (kw_cardsOff || getenv("KW_AUTOSHOT") || getenv("KW_AUTOCHRONICLE"))
		return;
	static const char *const ways[] = {"Free Flight: any world, any order", "The Chronicle: the story, in order"};
	KW_StopVoice();
	StopMusic(); // (the title tune, if the menu was opened over the title)
	SD_FadeAdLibVolume(256, 0);
	IN_ClearKeysDown();
	VL_ClearScreen(0);
	VL_SetScrollCoords(0, 0);
	VL_SetDefaultPalette();
	IN_ClearKeysDown();
	kw_chronicle = KW_ChooseCard("How will Keen travel?", ways, 2) == 1;
	// (one card, not KW_ShowCards: that redraws the level, and none is loaded yet)
	if (kw_chronicle)
		KW_ShowCard("The Chronicle\n\nThe worlds open one after\nanother, as their story goes.\nSave a world and the ship\nlearns the course to the next.");
	VL_ClearScreen(0);
}

static void KW_DumpScreen(const char *path);

// "About Keen Worlds": cards from the file ABOUT.TXT in the Megarocket's
// folder, which whoever put this galaxy together writes. A line of three
// dashes starts a new card. Shown from the flight menu and after the ending.
#define KW_MENU_CREDITS (-3)

static void KW_ShowAboutKeenWorlds(void)
{
	static char text[4096];
	char path[600];
	const char *dir = KW_WorldPath(KW_SHIP);
	snprintf(path, sizeof(path), "%s/ABOUT.TXT", dir ? dir : ".");
	FILE *f = fopen(path, "rb");
	size_t n = 0;
	if (f)
	{
		char line[128];
		while (fgets(line, sizeof(line), f) && n + sizeof(line) + 2 < sizeof(text))
		{
			size_t len = strcspn(line, "\r\n");
			line[len] = 0;
			if (!strcmp(line, "---"))
			{
				while (n && text[n - 1] == '\n')
					--n; // no empty lines at the foot of a card
				text[n++] = '\f';
				continue;
			}
			if (len > 36)
				line[len = 36] = 0; // a card is no wider
			memcpy(text + n, line, len);
			n += len;
			text[n++] = '\n';
		}
		fclose(f);
	}
	while (n && (text[n - 1] == '\n' || text[n - 1] == '\f'))
		--n;
	if (!n)
		n = (size_t)snprintf(text, sizeof(text), "Commander Keen: Worlds\n\nOne Megarocket.\nA galaxy that remembers.");
	// The last card is the game's own: which version this is, and what
	// is still to come (kw_version.h).
	snprintf(text + n, sizeof(text) - n, "\fVersion " KW_VERSION " (" KW_VERSION_NAME ")\n\n"
		"Every world can be played.\n\n"
		"Still to come: more life in\nthe levels Keen has saved.\n"
		"Only the Border Village has\nits people so far.");
	KW_ShowCards(text);
}

typedef struct KW_FlightEntry {int world,column,row,step;} KW_FlightEntry;
static const struct KW_FlightGroup
{
	const char *title;
	int column,count,firstStep,worlds[3];
} kw_flightGroups[]={
	{"Goodbye, Galaxy!",0,3,1,{KW_KEEN4,KW_KEEN5,KW_KEEN6}},
	{"The Universe Is Toast!",0,3,4,{KW_KEEN7,KW_KEEN8,KW_KEEN9}},
	{"Journey home",0,2,7,{KW_KEEN95,KW_SUBURB}},
	{NULL,0,1,0,{KW_MENU_CREDITS}},
	{"Independent trips",1,2,0,{KW_FITF,KW_BOUNTY}},
	{"Atroxian story",1,2,1,{KW_AREALM,KW_ALPHAMATIC}},
	{"More levels",1,2,0,{KW_CK55,KW_ASYLUM}},
	{"Spot's rescue",1,2,1,{KW_RTTS,KW_OSI}},
};

static int KW_FlightLayout(KW_FlightEntry *entries,int *rows)
{
	int count=0;rows[0]=rows[1]=0;
	for(unsigned g=0;g<sizeof(kw_flightGroups)/sizeof(kw_flightGroups[0]);++g)
	{
		const struct KW_FlightGroup *group=&kw_flightGroups[g];
		if(group->title) ++rows[group->column];
		for(int i=0;i<group->count;++i)
			entries[count++]=(KW_FlightEntry){group->worlds[i],group->column,rows[group->column]++,group->firstStep?group->firstStep+i:0};
	}
	return count;
}

static int KW_FlightMove(const KW_FlightEntry *entries,int count,int selection,int visibleRows,
	const int *top,IN_ScanCode scan,int dx,int dy)
{
	KW_FlightEntry current=entries[selection];
	int column=current.column,target=current.row,direction=0;
	if(scan==IN_SC_Home) target=-1000;
	else if(scan==IN_SC_End) target=1000;
	else if(scan==IN_SC_PgUp) {target-=visibleRows;direction=-1;}
	else if(scan==IN_SC_PgDown) {target+=visibleRows;direction=1;}
	else if(scan==IN_SC_UpArrow || dy==IN_motion_Up) {--target;direction=-1;}
	else if(scan==IN_SC_DownArrow || dy==IN_motion_Down) {++target;direction=1;}
	else if(scan==IN_SC_LeftArrow || dx==IN_motion_Left) column=0;
	else if(scan==IN_SC_RightArrow || dx==IN_motion_Right) column=1;
	else return selection;
	if(column!=current.column) target=current.row-top[current.column]+top[column];
	int best=selection,distance=10000;
	for(int i=0;i<count;++i)
	{
		if(entries[i].column!=column) continue;
		if(direction<0 && entries[i].row>=current.row) continue;
		if(direction>0 && entries[i].row<=current.row) continue;
		int delta=abs(entries[i].row-target);
		if(delta<distance) {best=i;distance=delta;}
	}
	return best;
}

static void KW_FlightRelationship(int world,char *text,size_t size)
{
	for(int i=0;i<KW_CHRONICLE_LINE;++i) if(kw_chronicleLine[i]==world)
	{
		if(i+1<KW_CHRONICLE_LINE) snprintf(text,size,"Main story %d/%d. Next: %s",i+1,KW_CHRONICLE_LINE,KW_WorldName(kw_chronicleLine[i+1]));
		else snprintf(text,size,"Main story %d/%d. Home after Aspermontis.",i+1,KW_CHRONICLE_LINE);
		return;
	}
	const char *relationship="Story order on the left; side trips on the right.";
	switch(world)
	{
	case KW_FITF:case KW_BOUNTY:relationship="Independent adventure. Choose any time.";break;
	case KW_AREALM:relationship="After The Omegamatic. Next: The Alphamatic.";break;
	case KW_ALPHAMATIC:relationship="After BOTH Krisle and Aspermontis.";break;
	case KW_CK55:case KW_ASYLUM:relationship="Optional levels after The Omegamatic.";break;
	case KW_RTTS:relationship="After Aspermontis. Next: the Station.";break;
	case KW_OSI:relationship="After the Far Shadowlands.";break;
	}
	CK_Cross_strscpy(text,relationship,size);
}

static int KW_TravelMenu(void)
{
	if (KW_IsStranded())
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
		KW_ShowCards(KW_StrandedText());
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

	// Main story and side trips retain their groups; each column scrolls.
	KW_FlightEntry entries[KW_NUM_WORLDS+1];int rows[2];
	const int COUNT=KW_FlightLayout(entries,rows);
	enum { ROW = 12, VISIBLE = 6, NORMAL = 15, HEADING = 11, GREY = 8, SAVED = 13, HERE = 14 };
	int here = kw_currentWorld == KW_SHIP ? kw_boardedFrom : kw_currentWorld;
	int sel = 0, top[2] = {0,0}, result = KW_NO_WORLD;
	for (int i = 0; i < COUNT; ++i) if (entries[i].world == here) sel = i;
	if (getenv("KW_AUTOMENUITEM")) sel = atoi(getenv("KW_AUTOMENUITEM"));
	sel = CK_Cross_max(0, CK_Cross_min(COUNT-1, sel));
	bool allShots = getenv("KW_AUTOMENUALL") != NULL;
	if (allShots) sel = 0;
	int8_t oldColour = US_GetPrintColour();
	uint16_t oldFont=US_GetPrintFont();
	CA_CacheGrChunk(CK_CHUNKNUM(FON_WATCHFONT));
	SD_PlaySound(CK_SOUNDNUM(SOUND_UNKNOWN12));
	KW_WaitForButtonsUp();
	for (;;)
	{
		int column=entries[sel].column;
		if(entries[sel].row<top[column]) top[column]=entries[sel].row;
		if(entries[sel].row>=top[column]+VISIBLE) top[column]=entries[sel].row-VISIBLE+1;
		// Redraw the complete card, including its frame. Never erase a
		// moving cursor with white paint over the border or adjacent text.
		VL_FixRefreshBuffer();
		US_CenterWindow(38, 22);
		int x = US_GetWindowX(), y = US_GetWindowY(), width = US_GetWindowW();
		US_SetPrintColour(NORMAL);
		US_SetPrintY(y + 2); US_CPrint("Bean-with-Bacon Megarocket");
		US_CPrint("FLIGHT CHART");
		int listY = y + 40;
		int columnWidth=(width-24)/2;
		VHB_VLine(y+26,listY+VISIBLE*ROW-3,x+width/2,7);
		US_SetPrintFont(1);
		US_SetPrintColour(NORMAL);
		US_SetPrintX(x+14);US_SetPrintY(y+26);US_Print("MAIN STORY - IN ORDER");
		US_SetPrintX(x+14+columnWidth+8);US_SetPrintY(y+26);US_Print("OTHER ADVENTURES");
		int headingRow[2]={0,0};
		for(unsigned g=0;g<sizeof(kw_flightGroups)/sizeof(kw_flightGroups[0]);++g)
		{
			const struct KW_FlightGroup *group=&kw_flightGroups[g];int col=group->column;
			if(group->title)
			{
				int row=headingRow[col]++;
				if(row>=top[col] && row<top[col]+VISIBLE)
				{
					US_SetPrintColour(HEADING);US_SetPrintX(x+14+col*(columnWidth+8));
					US_SetPrintY(listY+(row-top[col])*ROW);US_Print(group->title);
				}
			}
			headingRow[col]+=group->count;
		}
		for (int index = 0; index < COUNT; ++index)
		{
			KW_FlightEntry entry=entries[index];int col=entry.column,world=entry.world;
			if(entry.row<top[col] || entry.row>=top[col]+VISIBLE) continue;
			bool available = world == KW_MENU_CREDITS ||
				(KW_IsWorldPresent(world) && KW_CourseIsOpen(world));
			US_SetPrintColour(world == here ? HERE : !available ? GREY :
				(world >= 0 && KW_WorldIsSaved(world)) ? SAVED : NORMAL);
			US_SetPrintX(x+14+col*(columnWidth+8)); US_SetPrintY(listY+(entry.row-top[col])*ROW);
			char name[100];const char *label=world==KW_MENU_CREDITS?"About Keen Worlds":kw_worldDefs[world].name;
			if(entry.step) snprintf(name,sizeof(name),"%d. %s",entry.step,label);
			else CK_Cross_strscpy(name,label,sizeof(name));
			if(allShots)
			{
				uint16_t textWidth,textHeight;VH_MeasurePropString(name,&textWidth,&textHeight,US_GetPrintFont());
				if(textWidth>columnWidth-12 || textHeight>ROW-2) QuitF("Flight chart: destination does not fit: %s",name);
			}
			US_Print(name);
		}
		US_SetPrintFont(oldFont);
		int cursorX=x+8+column*(columnWidth+8);
		int cursorY = listY + (entries[sel].row-top[column]) * ROW - 2;
		int cursorColour = ((SD_GetTimeCount() >> 4) & 1) ? 12 : 1;
		VHB_HLine(cursorX,cursorX+columnWidth-1,cursorY,cursorColour);
		VHB_HLine(cursorX,cursorX+columnWidth-1,cursorY+11,cursorColour);
		VHB_VLine(cursorY,cursorY+11,cursorX,cursorColour);
		VHB_VLine(cursorY,cursorY+11,cursorX+columnWidth-1,cursorColour);
		US_SetPrintColour(NORMAL);
		US_SetPrintFont(1);
		for(int col=0;col<2;++col)
		{
			char more[40];snprintf(more,sizeof(more),"%s More %s %s",top[col]?"^":" ",col?"trips":"story",top[col]+VISIBLE<rows[col]?"v":" ");
			US_SetPrintX(x+14+col*(columnWidth+8));US_SetPrintY(listY+VISIBLE*ROW+4);US_Print(more);
		}
		char relationship[120];KW_FlightRelationship(entries[sel].world,relationship,sizeof(relationship));
		if(allShots)
		{
			uint16_t textWidth,textHeight;VH_MeasurePropString(relationship,&textWidth,&textHeight,US_GetPrintFont());
			if(textWidth>width-16) QuitF("Flight chart: story line does not fit: %s",relationship);
		}
		US_SetPrintY(y+128);US_CPrint(relationship);US_SetPrintFont(oldFont);
		int footY = y + 140;
		VHB_HLine(x+4,x+width-5,footY-3,8);
		int world = entries[sel].world;
		char title[80],status[100];
		CK_Cross_strscpy(title,world==KW_MENU_CREDITS?"Commander Keen: Worlds":kw_worldDefs[world].title,sizeof(title));
		for (char *c=title;*c;++c) if (*c=='\n') *c=' ';
		if (world==KW_MENU_CREDITS) snprintf(status,sizeof(status),"Who put this galaxy together.");
		else if (world==here) snprintf(status,sizeof(status),"Keen is here. Choose to read about it.");
		else if (!KW_IsWorldPresent(world)) snprintf(status,sizeof(status),"Not installed. Choose for instructions.");
		else if (!KW_CourseIsOpen(world)) snprintf(status,sizeof(status),"The Chronicle: save %s first.",kw_worldDefs[KW_ChronicleKey(world)].name);
		else if (KW_WorldIsSaved(world)) snprintf(status,sizeof(status),"Saved.");
		else
		{
			int done=0;
			for(int level=1;level<CK_LEVELS_TRACKED;++level) if(kw_worlds[world].levelsDone[level]) ++done;
			if(done) snprintf(status,sizeof(status),"Been here: %d level%s done. Not saved yet.",done,done==1?"":"s");
			else snprintf(status,sizeof(status),kw_worlds[world].visited?"Keen has been here. Not saved yet.":"Keen has not been here yet.");
		}
		if(allShots)
		{
			const char *lines[2]={title,status};
			for(int l=0;l<2;++l)
			{
				uint16_t textWidth,textHeight;VH_MeasurePropString(lines[l],&textWidth,&textHeight,US_GetPrintFont());
				if(textWidth>width-16) QuitF("Flight chart: footer line does not fit: %s",lines[l]);
			}
		}
		US_SetPrintY(footY);US_CPrint(title);US_CPrint(status);
		US_SetPrintY(y+164);US_CPrint("Arrows: move  Enter: choose  Esc: back");
		VH_UpdateScreen();
		if (allShots || getenv("KW_AUTOMENU"))
		{
			char path[600];
			if (allShots) snprintf(path,sizeof(path),"%s/flight-%02d.ppm",getenv("KW_AUTOMENUALL"),sel);
			else CK_Cross_strscpy(path,getenv("KW_AUTOMENU"),sizeof(path));
			KW_DumpScreen(path);
			printf("KW_AUTOMENU: grouped flight chart, item %d/%d, world %d, column %d, row %d\n",sel+1,COUNT,world,column,entries[sel].row);
			if(allShots && sel+1<COUNT)
			{
				int next=KW_FlightMove(entries,COUNT,sel,VISIBLE,top,IN_SC_DownArrow,0,0);
				if(entries[sel+1].column!=column)
					next=KW_FlightMove(entries,COUNT,KW_FlightMove(entries,COUNT,sel,VISIBLE,top,IN_SC_RightArrow,0,0),VISIBLE,top,IN_SC_Home,0,0);
				if(next!=sel+1) Quit("Flight chart: arrow navigation cannot reach the next destination");
				sel=next;continue;
			}
			if(allShots)
			{
				// Return across every row, exercising Up and both column edges.
				for(int back=sel;back>0;)
				{
					int previous=KW_FlightMove(entries,COUNT,back,VISIBLE,top,IN_SC_UpArrow,0,0);
					if(entries[back-1].column!=entries[back].column)
						previous=KW_FlightMove(entries,COUNT,KW_FlightMove(entries,COUNT,back,VISIBLE,top,IN_SC_LeftArrow,0,0),VISIBLE,top,IN_SC_End,0,0);
					if(previous!=back-1) Quit("Flight chart: reverse arrow navigation skips a destination");
					back=previous;
				}
			}
			US_SetPrintColour(oldColour);return KW_NO_WORLD;
		}
		IN_PumpEvents();
		IN_ScanCode scan=IN_GetLastScan();IN_ReadControls(0,&ck_inputFrame);
		if(scan==IN_SC_Escape || ck_inputFrame.button3) break;
		if(ck_inputFrame.jump || ck_inputFrame.pogo || scan==IN_SC_Enter || scan==IN_SC_Space)
		{result=entries[sel].world;break;}
		int next=KW_FlightMove(entries,COUNT,sel,VISIBLE,top,scan,ck_inputFrame.xDirection,ck_inputFrame.yDirection);
		bool move=next!=sel;sel=next;
		IN_SetLastScan(IN_SC_None);
		if(move)
			while(ck_inputFrame.yDirection!=IN_motion_None || ck_inputFrame.xDirection!=IN_motion_None)
			{IN_PumpEvents();IN_ReadControls(0,&ck_inputFrame);VL_Present();}
		US_SetPrintColour(oldColour);
		VL_DelayTics(2);
	}
	US_SetPrintColour(oldColour);

	KW_WaitForButtonsUp();
	if (result == KW_MENU_CREDITS)
	{
		KW_ShowAboutKeenWorlds();
		return KW_NO_WORLD;
	}
	if (result == here)
		result = KW_MENU_ABOUT;
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
	if (KW_TransportedAlone(result) && !KW_AskCard(KW_CALIDUNE_WARNING))
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
#define KW_SHIP_ARCADE 0x7707   // optional earlier adventures, never flight destinations
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
	if (MARS_IsActive()) return;
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
	// The game that begins shows its own title and tells its story first
	// (Keen Worlds' own title is the one seen when the program is launched).

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


// ---- The bed: Keen Dreams ----
// "Up" at the bunk and Keen falls asleep: the dream is Keen Dreams, running
// in this engine as a world of its own (kw_dreams.c). It is not a planet: the
// flight console doesn't list it, the ship computer says nothing about it,
// and nothing is carried in or out. Keen's score, lives and shots wait in
// the ship while he dreams, and the dream keeps its own (flower power for
// shots) until he sleeps again. Waking puts him back in his bunk.

static KW_Tally KW_LiveTally(void)
{
	KW_Tally t = {ck_gameState.keenScore, ck_gameState.nextKeenAt, ck_gameState.numLives, ck_gameState.numShots};
	return t;
}

static void KW_SetTally(const KW_Tally *t)
{
	ck_gameState.keenScore = t->score;
	ck_gameState.nextKeenAt = t->nextKeenAt;
	ck_gameState.numLives = t->lives;
	ck_gameState.numShots = t->shots;
}

static KW_ArcadeTally KW_LiveArcadeTally(void)
{
	KW_ArcadeTally t={KW_LiveTally(),ck_gameState.numCentilife,(int16_t)ck_gameState.difficulty};
	return t;
}

static void KW_SetArcadeTally(const KW_ArcadeTally *t)
{
	KW_SetTally(&t->tally);
	ck_gameState.numCentilife=t->centilife;
	ck_gameState.difficulty=(CK_Difficulty)t->difficulty;
}

static void KW_ArcadeLeaving(int from)
{
	if (from!=KW_MARS || !kw_marsLinked) return;
	kw_marsTally=KW_LiveArcadeTally();
	KW_SetArcadeTally(&kw_journeyTally);
	kw_arcadeCabinDue=kw_arcadeCabinX>0 && kw_arcadeCabinY>0;
	kw_shipUpUsed=true;
}

static void KW_ArcadeArriving(int to)
{
	if (to!=KW_MARS) return;
	kw_journeyTally=KW_LiveArcadeTally();
	if (!kw_worlds[KW_MARS].visited)
		kw_marsTally=(KW_ArcadeTally){{0,20000,3,0},0,D_Normal};
	kw_marsLinked=true;
	KW_SetArcadeTally(&kw_marsTally);
}

static void KW_ArcadeFadeOut(void)
{
	kw_arcadeFadeIn=!kw_cardsOff;
	KW_LinesOver();
	SD_MusicOff();
	SD_StopSound();
	if (!kw_cardsOff) VL_FadeToBlack();
}

bool KW_FadeArcadeIn(void)
{
	if (!kw_arcadeFadeIn) return false;
	kw_arcadeFadeIn=false;
	VL_FadeFromBlack();
	return true;
}

bool KW_BeginArcade(void)
{
 if (kw_currentWorld!=KW_SHIP || !ck_keenObj || !KW_IsWorldPresent(KW_MARS)) return false;
 kw_arcadeCabinX=ck_keenObj->posX; kw_arcadeCabinY=ck_keenObj->posY;
 kw_arcadeCabinDirection=ck_keenObj->xDirection;
 KW_ArcadeFadeOut();
 KW_RequestWorld(KW_MARS);
 return true;
}

bool KW_CanReturnToCabin(void)
{
	return (kw_currentWorld==KW_MARS || kw_currentWorld==KW_DREAM) && KW_IsWorldPresent(KW_SHIP);
}

void KW_RequestCabinReturn(void) { kw_cabinReturnRequested=KW_CanReturnToCabin(); }

void KW_FinishMenuActions(void)
{
	if (!kw_cabinReturnRequested) return;
	kw_cabinReturnRequested=false;
	if (kw_currentWorld==KW_DREAM) { KW_WakeUp(); return; }
	if (!KW_CanReturnToCabin()) return;
	// A pre-integration Mars save has no suspended journey to restore.
	if (!kw_marsLinked)
	{
		kw_journeyTally=(KW_ArcadeTally){{0,20000,3,5},0,D_Normal};
		kw_marsLinked=true;
		kw_arcadeCabinX=kw_arcadeCabinY=0;
	}
	RF_ForceRefresh(); RF_Refresh();
	KW_ArcadeFadeOut();
	KW_RequestWorld(KW_SHIP);
}

bool KW_RestartArcade(int difficulty)
{
	if (!MARS_IsActive() || !kw_marsLinked) return false;
	memset(&kw_worlds[KW_MARS],0,sizeof(kw_worlds[KW_MARS]));
	KW_LoadState(&kw_worlds[KW_MARS]);
	kw_marsTally=(KW_ArcadeTally){{0,20000,3,0},0,(int16_t)difficulty};
	KW_SetArcadeTally(&kw_marsTally);
	ck_gameState.currentLevel=0;
	ck_gameState.levelState=LS_Playing;
	return true;
}

bool KW_ArcadeOver(bool won)
{
	if (!MARS_IsActive() || !kw_marsLinked) return false;
	KW_ArcadeFadeOut();
	KW_SwitchWorld(KW_SHIP);
	bool completed=won || kw_worlds[KW_MARS].completed;
	memset(&kw_worlds[KW_MARS],0,sizeof(kw_worlds[KW_MARS]));
	kw_worlds[KW_MARS].completed=completed;
	kw_marsTally=(KW_ArcadeTally){{0,20000,3,0},0,D_Normal};
	ck_gameState.levelState=LS_Playing;
	return true;
}

// A dream from its beginning (Keen Dreams' NewGame).
static void KW_NewDream(void)
{
	KW_Tally fresh = {0, 20000, 3, 0};
	kw_dreamTally = fresh;
	memset(&kw_worlds[KW_DREAM], 0, sizeof(kw_worlds[KW_DREAM]));
	KD_NewGame();
}

// KW_SwitchWorld, leaving a world: out of the dream, Keen's own tally again.
static void KW_DreamLeaving(int from)
{
	if (from != KW_DREAM)
		return;
	kw_dreamTally = KW_LiveTally();
	KW_SetTally(&kw_awake);
}

// ...and arriving in one: into the dream, the dream's tally.
static void KW_DreamArriving(int to)
{
	if (to != KW_DREAM)
		return;
	if (!kw_worlds[KW_DREAM].visited)
		KW_NewDream();
	kw_awake = KW_LiveTally();
	KW_SetTally(&kw_dreamTally);
	ck_gameState.currentLevel = 0; // the map of Tuberia
	kw_launchSpotX = kw_launchSpotY = -1; // no Megarocket stands in a dream
	kw_placeArrivalAtShip = false;
}

static bool kw_dreamFadeIn;

bool KW_FadeDreamIn(void)
{
	if (!kw_dreamFadeIn) return false;
	kw_dreamFadeIn = false;
	VL_DelayTics(28);
	VL_FadeFromBlackTimed(14);
	return true;
}

static void KW_DreamFadeOut(void)
{
	kw_dreamFadeIn = !kw_cardsOff;
	if (!kw_cardsOff)
	{
		KW_LinesOver();
		KW_FadeMusicOut();
		VL_FadeToBlackTimed(14);
		VL_DelayTics(28);
	}
}

// Keen Dreams Gold Edition runs in its own native companion, like the
// arcade's originals: it is not a world, and never touches the journey.
static void KW_DreamGold(void)
{
	KW_ArcadeStatus status = KW_PlayDreamsGold();
	if (status == KW_ARCADE_NO_DATA)
		KW_ShowCards("Keen Dreams Gold Edition\nBy Szemigi\n\nNot installed. Unpack its\nKDGOLD.ZIP (2026-05-31)\ninto mods/dreams-gold,\nincluding the KDGSRC folder.\f"
			"Gold keeps its own saves\nand settings, separate\nfrom this journey.\n\nSee mods/README.txt.");
	else if (status == KW_ARCADE_WRONG_VERSION)
		KW_ShowCards("Keen Dreams Gold Edition\n\nThese files are another\nrelease. Keen Worlds plays\nthe 2026-05-31 KDGOLD.ZIP.\n\nSee mods/README.txt.");
	else if (status == KW_ARCADE_FAILED)
		KW_ShowCards("Keen Dreams Gold could\nnot finish normally.\n\nKeen is back in his bunk.\nSee keenworlds-log.txt\nfor the game's message.");
	StartMusic(ca_mapOn);
}

static void KW_Dream(void)
{
	// With the Gold companion installed, the bunk offers both dreams.
	if (!kw_cardsOff && KW_DreamsGoldCheck() != KW_ARCADE_NO_ENGINE)
	{
		const char *dreams[] = {
			KW_IsWorldPresent(KW_DREAM) ? "Keen Dreams" : "Keen Dreams (not installed)",
			KW_DreamsGoldCheck() == KW_ARCADE_READY ? "Keen Dreams Gold Edition" : "Gold Edition (not installed)",
			"Stay awake"};
		int dream = KW_ChooseCard("Keen's bunk\nWhich dream tonight?", dreams, 3);
		IN_ClearKeysDown();
		if (dream < 0 || dream == 2)
		{
			RF_ForceRefresh();
			SD_SetLastTimeCount(SD_GetTimeCount());
			return;
		}
		if (dream == 1)
		{
			KW_DreamGold();
			RF_ForceRefresh();
			SD_SetLastTimeCount(SD_GetTimeCount());
			return;
		}
	}
	if (!KW_IsWorldPresent(KW_DREAM))
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_NEEDKEYCARD));
		if (!kw_cardsOff)
		{
			char text[320];
			snprintf(text, sizeof(text), "Keen lies down, but he\nis not sleepy.\n\nKeen Dreams is not here:\n%s\f%s",
				kw_dreamProblem ? kw_dreamProblem : "it needs Keen 4.",
				"Put Keen Dreams' shareware\nrelease, version 1.13, in\nmods/dreams: KDREAMS.EXE,\n.MAP, .EGA and .AUD.\nSee mods/README.txt.");
			KW_ShowCards(text);
		}
		return;
	}
	printf("KW_AUTODREAM: Keen falls asleep\n");
	if (!kw_cardsOff)
	{
		KW_ShowCards(kw_worlds[KW_DREAM].visited ? "Keen lies down on his bunk.\nJust for a minute..." :
			"Keen lies down on his bunk.\nJust for a minute...\n\n\"Billy! No dessert until you\neat your vegetables!\"");
	}
	KW_DreamFadeOut();
	KW_MusicQuiet();
	KW_RequestWorld(KW_DREAM);
}

// Keen wakes: back to the ship, in his bunk. The dream is kept as it is.
static void KW_WakeUp(void)
{
	if (kw_currentWorld != KW_DREAM)
		return;
	printf("KW_AUTODREAM: Keen wakes up\n");
	KW_DreamFadeOut();
	kw_wakeInBed = true;
	kw_shipUpUsed = true;
	kw_dreamEnded = 1;
	KW_RequestWorld(KW_SHIP);
}

// The dream is over (the game loop): Keen ran out of lives in it, or he
// switched off the Dream Machine. He wakes, and the next dream starts from
// the beginning. Returns true: the ship's room is to be loaded.
bool KW_DreamOver(bool won)
{
	if (kw_currentWorld != KW_DREAM)
		return false;
	printf("KW_AUTODREAM: the dream is over (%s)\n", won ? "won" : "lost");
	KW_DreamFadeOut();
	kw_wakeInBed = true;
	KW_SwitchWorld(KW_SHIP);
	bool completed = won || kw_worlds[KW_DREAM].completed;
	KW_NewDream();
	kw_worlds[KW_DREAM].completed = completed;
	kw_dreamEnded = won ? 3 : 2;
	return true;
}

// Keen has no wrist computer in his pyjamas: in the dream, the keys that
// open its menus ask whether to wake up. (Games are saved from the ship.)
void KW_DreamKeys(void)
{
	IN_ScanCode scan = IN_GetLastScan();
	if (kw_currentWorld != KW_DREAM || vl_screenFaded)
		return;
	// The bed on the map of Tuberia is the way back too, as the ship is on
	// other worlds: walk up into it from its foot. (One push, one question.)
	static bool pushed = false;
	bool atBed = false;
	if (ca_mapOn == 0 && ck_keenObj && ck_inputFrame.yDirection == IN_motion_Up)
	{
		for (int x = ck_keenObj->clipRects.tileX1; x <= ck_keenObj->clipRects.tileX2; ++x)
		{
			uint16_t above = CA_TileAtPos(x, ck_keenObj->clipRects.tileY1 - 1, 0);
			uint16_t here = CA_TileAtPos(x, ck_keenObj->clipRects.tileY1, 0);
			// (the bed's four background tiles)
			atBed |= above == 1042 || above == 1043 || here == 1042 || here == 1043 || here == 1024 || here == 1025;
		}
	}
	bool bed = atBed && !pushed;
	pushed = atBed;
	if (!bed && !((scan >= IN_SC_F1 && scan <= IN_SC_F7) || scan == IN_SC_Escape || ck_inputFrame.button3 ||
		scan == in_kbdControls.quickSave || scan == in_kbdControls.quickLoad))
		return;
	ck_inputFrame.button3 = false;
	static const char *const choices[] = {"Dream on", "Wake up"};
	if (bed)
		printf("KW_AUTODREAM: Keen is at his bed in Tuberia\n");
	bool wake = getenv("KW_AUTOSHOT") ? bed : KW_ChooseCard(bed ? "Keen's own bed. He could\nlie down and wake up." : "Keen is fast asleep.", choices, 2) == 1;
	IN_ClearKeysDown();
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
	if (wake)
		KW_WakeUp();
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

// The cabinet is outside the world registry: its games never affect the
// Chronicle or shared inventory. Original episodes run in a native companion.
static void KW_ArcadeOriginal(int episode)
{
	KW_ArcadeStatus status = KW_PlayArcade(episode);
	char text[256];
	if (status == KW_ARCADE_NO_ENGINE)
		KW_ShowCards("The arcade needs its\nnative game player.\n\nInstall the complete\nKeen Worlds download\nand try again.");
	else if (status == KW_ARCADE_NO_DATA)
	{
		snprintf(text, sizeof(text), "Commander Keen %d\nBy id Software\n\nNot installed. Unpack your\noriginal v1.31 game into\narcade/keen%d, including\nKEEN%d.EXE and all .CK%d files.\n\nYour arcade saves stay\nseparate from this journey.", episode, episode, episode, episode);
		KW_ShowCards(text);
	}
	else if (status == KW_ARCADE_WRONG_VERSION)
		KW_ShowCards("This arcade game needs\nthe original v1.31 files.\n\nSee the arcade installation\nguide for the supported\nversion.");
	else if (status == KW_ARCADE_FAILED)
		KW_ShowCards("The arcade game could\nnot finish normally.\n\nYour journey is still here.\nSee keenworlds-log.txt\nfor the game's message.");
	StartMusic(ca_mapOn);
}

static void KW_ShowArcade(void)
{
	// The original text-only introduction already set kw_arcadeIntroduced.
	// Replay it once after installing the voiced introduction, keeping all
	// game progress and the player's voice preferences intact.
	if (CFG_GetConfigInt("kw_arcadeIntroductionVersion", 0) < 2)
	{
		KW_Speak("ARCADE_HELLO.RAW", true);
		KW_ShowCards("Keen:\n\nHmm, don't have time to\nplay with myself.\n\nWell... maybe one game!");
		KW_LinesOver();
		CFG_SetConfigInt("kw_arcadeIntroductionVersion", 2);
	}
	for (;;)
	{
		char second[64], third[64];
		snprintf(second, sizeof(second), "Keen 2%s", KW_ArcadeCheck(2) == KW_ARCADE_READY ? ": The Earth Explodes" : " (not installed)");
		snprintf(third, sizeof(third), "Keen 3%s", KW_ArcadeCheck(3) == KW_ARCADE_READY ? ": Keen Must Die!" : " (not installed)");
		const char *games[] = {"Keen 1: Marooned on Mars", second, third, "Back to the cabin"};
		int game = KW_ChooseCard("Keen's arcade\nEarlier adventures", games, 4);
		if (game < 0 || game == 3) break;
		if (game == 0)
		{
			KW_Speak("ARCADE_REMAKE.RAW", true);
			KW_ShowCards("Keen:\n\nHey! There's an updated\nversion of my first\nadventure with enhanced\ngraphics!\n\nWhich one should I play?");
			KW_LinesOver();
			const char *versions[] = {
				KW_ArcadeCheck(1) == KW_ARCADE_READY ? "Original Keen 1" : "Original Keen 1 (not installed)",
				KW_IsWorldPresent(KW_MARS) ? "Galaxy remake" : "Galaxy remake (not installed)", "Back"};
			int version = KW_ChooseCard("Marooned on Mars", versions, 3);
			if (version < 0 || version == 2) continue;
			if (version == 1)
			{
                if (!KW_IsWorldPresent(KW_MARS))
                {
                    KW_ShowCards("Marooned on Mars - Galaxy\n\nInstall its converted game\nfiles in arcade/mars.\n\nSee arcade/README.txt.");
                    continue;
                }
                KW_BeginArcade();
                break;
			}
		}
		KW_ArcadeOriginal(game + 1);
	}
	KW_StopVoice();
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
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
		if (info >= KW_SHIP_CONSOLE && info <= KW_SHIP_ARCADE)
		{
			marker = info;
			markerY = y;
		}
	}
	// Animated foreground tiles use their info-plane word for an animation
	// timer. The blinking Up sign therefore replaces its own console marker.
	// Anchor this interaction to the sign's art coordinates instead.
	if (x >= kw_shipHintArt.originX && x <= kw_shipHintArt.originX + 1 &&
		ck_keenObj->clipRects.tileY1 <= kw_shipHintArt.originY &&
		ck_keenObj->clipRects.tileY2 >= kw_shipHintArt.originY)
	{
		marker = KW_SHIP_CONSOLE;
		markerY = kw_shipHintArt.originY;
	}
	if (marker == KW_SHIP_WAY_AFT || marker == KW_SHIP_WAY_FORE)
	{
		KW_ShipChangeRoom(marker, markerY);
		return;
	}
	// One press of "up", one use (the scripted developer runs hold it down).
	// (kw_shipUpUsed: also set when Keen wakes in his bunk with "up" still
	// held from pushing at the bed in Tuberia, or he would lie down again.)
	if (ck_inputFrame.yDirection != IN_motion_Up)
		kw_shipUpUsed = false;
	if (!marker || !ck_keenObj->topTI || ck_inputFrame.yDirection != IN_motion_Up || kw_shipUpUsed)
		return;
	kw_shipUpUsed = true;
	if (marker == KW_SHIP_ARCADE)
	{
		KW_ShowArcade();
		return;
	}
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
		// out, to the world the ship stands on (it has landed: no fly-in
		// over Gnosticus Pi now)
		kw_fitfNoFlyIn = true;
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
		if (KW_TransportedAlone(destination))
		{
			// Calidune: the transporter takes Keen; the ship stays in orbit.
			// Its story, then Keen on the sand at the map's own start.
			kw_boardedFrom = destination;
			kw_storyWorld = KW_StoryIsDue(destination) ? destination : KW_NO_WORLD;
			KW_RequestWorld(destination);
			return;
		}
		if (destination == KW_FITF && !kw_worlds[KW_FITF].visited)
		{
			// Gnosticus Pi, the first time: its story, then the game's own
			// arrival (the ship flies in over its map and lands), and then
			// Keen is inside the landed ship (KW_FitfShipLanded).
			kw_boardedFrom = destination;
			kw_storyWorld = KW_StoryIsDue(destination) ? destination : KW_NO_WORLD;
			kw_fitfLandsInShip = true;
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

// Foray in the Forest's fly-in has landed (FF_T_MapShip, kw_ff_map.c). After
// a flight from the ship's controls Keen is still aboard: into the cabin.
void KW_FitfShipLanded(void)
{
	if (!kw_fitfLandsInShip)
		return;
	kw_fitfLandsInShip = false;
	if (!KW_IsWorldPresent(KW_SHIP))
		return;
	printf("KW_AUTOFLYIN: the ship has landed on Gnosticus Pi\n");
	kw_fitfCardDue = true;
	kw_landingToAnnounce = KW_FITF;
	KW_VoiceComingUp(true);
	KW_RequestWorld(KW_SHIP);
}

// ---- Keen Worlds' own title, story and ending picture ----
// The pictures are in kw_title_art.h (tools/kw_title_art.py, from art/title).

static void KW_DrawPicture(const uint8_t *packed)
{
	for (int y = 0; y < 200; ++y)
		for (int x = 0; x < 320; x += 2)
		{
			uint8_t b = packed[y * 160 + x / 2];
			VH_Plot(x, y, b >> 4);
			VH_Plot(x + 1, y, b & 15);
		}
}

bool KW_DrawStoryBackdrop(void);

bool KW_DrawWorldTitlePicture(void)
{
	if (kw_currentWorld != KW_ASYLUM)
		return false;
	VL_ClearScreen(0);
	KW_DrawPicture(kw_asylumTitlePicture);
	return true;
}

// The title, in place of the running episode's (ck_inter.c).
bool KW_DrawTitlePicture(void)
{
	if (MARS_IsActive()) return false;
	VL_ClearScreen(0);
	// Address Unknown used Keen 5's title art.  Its own title must be used
	// here too (the actual game-start screen), not only before its story.
	KW_DrawPicture(kw_currentWorld == KW_ASYLUM ? kw_asylumTitlePicture : kw_titlePicture);
	// Developer tool: KW_AUTOTITLE=file.ppm saves the title (or, with
	// KW_AUTOENDING set, the ending picture) and quits
	if (getenv("KW_AUTOTITLE"))
	{
		if (getenv("KW_AUTOENDING") && !strcmp(getenv("KW_AUTOENDING"), "story"))
			KW_DrawStoryBackdrop();
		else if (getenv("KW_AUTOENDING"))
		{
			KW_DrawPicture(kw_endingPicture);
			VL_SetCustomColors(kw_endingColors);
		}
		VL_SetScrollCoords(0, 0);
		KW_DumpScreen(getenv("KW_AUTOTITLE"));
		Quit(NULL);
	}
	return true;
}

// What the story scrolls over (ck_inter.c): the picture, in fifteen colours
// of its own; the sixteenth is the colour of the letters. The scroller writes
// each pixel whole (KW_StoryPixel: the picture's, where the text has moved
// on). The standard colours come back when the scroll is over
// (KW_PlayOwnStory).
bool KW_DrawStoryBackdrop(void)
{
	const uint8_t *picture = kw_storyPicture;
	const uint8_t (*colours)[3] = kw_storyColors;
	if (kw_epilogueBackdrop)
	{
		picture = kw_epilogueStoryPicture;
		colours = kw_epilogueStoryColors;
	}
	else if (kw_currentWorld == KW_BOUNTY)
	{
		picture = kw_bountyStoryPicture;
		colours = kw_bountyStoryColors;
	}
	else if (kw_currentWorld == KW_ASYLUM)
	{
		picture = kw_asylumStoryPicture;
		colours = kw_asylumStoryColors;
	}
	VL_ClearScreen(0);
	KW_DrawPicture(picture);
	VL_SetDefaultPalette();
	VL_SetCustomColors(colours);
	return true;
}

int KW_StoryPixel(int x, int y)
{
	if (x < 0 || x >= 320 || y < 0 || y >= 200)
		return 0;
	const uint8_t *picture = kw_epilogueBackdrop ? kw_epilogueStoryPicture :
		kw_currentWorld == KW_BOUNTY ? kw_bountyStoryPicture :
		kw_currentWorld == KW_ASYLUM ? kw_asylumStoryPicture : kw_storyPicture;
	uint8_t b = picture[y * 160 + x / 2];
	return (x & 1) ? (b & 15) : (b >> 4);
}

// The story, told the way each episode tells its own: scrolling away into
// the distance over that episode's star field. It plays in the opening, and
// when a game begins.
static const char kw_ownStory[] =
	"Keen Worlds\n"
	"\n"
	"One Megarocket.\n"
	"A galaxy that remembers.\n"
	"\n"
	"Billy Blaze, eight years\n"
	"old and of unlimited\n"
	"genius, has saved the\n"
	"galaxy several times\n"
	"before bedtime.\n"
	"\n"
	"Each time he flew home,\n"
	"parked the Bean-with-\n"
	"Bacon Megarocket behind\n"
	"the shed, and the world\n"
	"he had saved was\n"
	"never heard of again.\n"
	"\n"
	"Not any more.\n"
	"\n"
	"The ship's computer has\n"
	"been keeping a chart.\n"
	"Every planet, station\n"
	"and peculiar moon Keen\n"
	"has set foot on is on\n"
	"it, and a good many he\n"
	"has not: worlds found\n"
	"by other explorers, who\n"
	"wrote down the way.\n"
	"\n"
	"They are all still out\n"
	"there. Some are in\n"
	"trouble. Some have an\n"
	"inn now.\n"
	"\n"
	"So Keen has packed his\n"
	"pogo stick, his stunner\n"
	"and a spare helmet,\n"
	"told his parents he is\n"
	"going to bed early,\n"
	"and set a course\n"
	"for all of them.\n"
	"\n"
	"A world that is saved\n"
	"stays saved. The ones\n"
	"that are not are\n"
	"waiting.\n"
	"\n"
	"Fly well, Commander.\n";

// What scrolls when the last world is saved, before the picture of Keen on
// the beach (KW_Finale). The narrator reads it: STORY_EPILOGUE.RAW, made by
// tools/kw_story_voice.py from these words.
static const char kw_epilogue[] =
	"The last world\n"
	"is saved.\n"
	"\n"
	"The ship's computer\n"
	"went through the chart\n"
	"twice, to be sure.\n"
	"Every planet, every\n"
	"station, every\n"
	"peculiar moon: quiet.\n"
	"\n"
	"Nobody gave Keen a\n"
	"medal. Nobody on\n"
	"Earth knows he was\n"
	"away. His parents\n"
	"think he slept well.\n"
	"\n"
	"That is how he\n"
	"likes it.\n"
	"\n"
	"So he set one more\n"
	"course, for nowhere\n"
	"in particular,\n"
	"\n"
	"and took the\n"
	"afternoon off.\n";

extern const char *ck_starWarsTextOverride;

void KW_PlayOwnStory(bool handleDemoKeys)
{
	printf("KW_AUTOSTORY: the story of Keen Worlds\n");
	fflush(stdout);
	ck_starWarsTextOverride = kw_ownStory;
	// The narrator reads it (STORY_WORLDS.RAW, tools/kw_story_voice.py).
	// The title tune, which plays on from the title picture, fades away
	// under him and comes back when he has finished; if the last line has
	// scrolled away before that, the final frame waits for him (or a held Escape).
	KW_StartTitleMusic();
	IN_ClearKeysDown();
	KW_NarrateOverMusic("STORY_WORLDS.RAW");
	// (the keys of the opening loop are handled below, once the standard
	// colours are back: a menu opened from here must not be in the story's)
	bool skipped = CK_DrawStarWarsEx(false);
	ck_starWarsTextOverride = NULL;
	VL_SetCustomColors(NULL);
	KW_LinesOver();
	if (handleDemoKeys && skipped)
	{
		IN_SetLastScan(IN_SC_Space);
		CK_HandleDemoKeys();
	}
}

// A picture on the whole screen, in its own sixteen colours, until Escape
// is deliberately held.
static void KW_ShowPicture(const uint8_t *packed, const uint8_t colours[16][3])
{
	VL_FadeToBlack();
	VL_FixRefreshBuffer();
	VL_ClearScreen(0);
	VL_SetScrollCoords(0, 0);
	KW_DrawPicture(packed);
	VH_UpdateScreen();
	VL_SetCustomColors(colours);
	VL_FadeFromBlack();
	KW_WaitForButtonsUp();
	IN_ClearKeysDown();
	KW_WaitPresentation(0, false);
	KW_WaitForButtonsUp();
	VL_FadeToBlack();
	VL_SetCustomColors(NULL);
	RF_ForceRefresh();
	RF_Refresh();
	VL_FadeFromBlack();
	SD_SetLastTimeCount(SD_GetTimeCount());
}

// ---- Keen Worlds' ending: when the last world is saved, straight after
// that world's own ending (KW_AfterWorldEnding; no level is loaded here).
//   1. a last story scrolls, read by the narrator, the calm tune low
//      under him (the one place music plays under speech: asked for so);
//   2. the picture of Keen on the beach, the tune at its full level,
//      until a key is pressed;
//   3. Keen is put at the bar of the Border Village's inn on Gnosticus IV
//      and tells the barmaid what he has done (KW_BragAtTheBar);
//   4. and then he is free: the ship stands on Gnosticus IV as always.
// Once a game (kw_chronicleEndTold; W + A makes it due again).
#define KW_EPILOGUE_MUSIC 77 // the tune's level under the narrator, of 256

static bool KW_EveryWorldIsSaved(void)
{
	bool any = false;
	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (KW_IsPlanet(w) && KW_IsWorldPresent(w))
		{
			if (!KW_WorldIsSaved(w))
				return false;
			any = true;
		}
	return any;
}

static void KW_Finale(void)
{
	extern bool ck_starWarsNarrated, ck_starWarsKeepMusic;
	if (kw_chronicleEndTold || kw_cardsOff || !KW_EveryWorldIsSaved())
		return;
	kw_chronicleEndTold = 1;
	printf("KW_AUTOENDING: every world is saved\n");
	fflush(stdout);

	KW_StopVoice();
	KW_StartEndingMusic(KW_VoicesEnabled() && KW_VoiceVolume() ? KW_EPILOGUE_MUSIC : 256);
	IN_ClearKeysDown();
	KW_Narrate("STORY_EPILOGUE.RAW");
	ck_starWarsNarrated = true; // (no music of the scroll's own)
	ck_starWarsKeepMusic = true;
	ck_starWarsTextOverride = kw_epilogue;
	kw_epilogueBackdrop = true;
	CK_DrawStarWarsEx(false);
	kw_epilogueBackdrop = false;
	ck_starWarsTextOverride = NULL;
	ck_starWarsNarrated = ck_starWarsKeepMusic = false;
	VL_SetCustomColors(NULL);
	KW_StopVoice();

	// The picture: the tune comes up.
	SD_FadeAdLibVolume(256, 1500);
	VL_FadeToBlack();
	VL_ClearScreen(0);
	VL_SetScrollCoords(0, 0);
	KW_DrawPicture(kw_endingPicture);
	VH_UpdateScreen();
	VL_SetCustomColors(kw_endingColors);
	VL_FadeFromBlack();
	KW_WaitForButtonsUp();
	IN_ClearKeysDown();
	if (!getenv("KW_AUTOFINALE"))
		KW_WaitPresentation(0, false);
	KW_WaitForButtonsUp();
	VL_DelayTics(KW_FadeMusicOut());
	VL_FadeToBlack();
	VL_SetCustomColors(NULL);
	VL_ClearScreen(0);
	SD_MusicOff();
	SD_FadeAdLibVolume(256, 0);

	// To the bar. (Keen 4 is always there: the ship is made of it.)
	if (KW_IsWorldPresent(KW_KEEN4))
	{
		KW_SwitchWorld(KW_KEEN4);
		if (!ck_gameState.levelsDone[1])
			ck_gameState.levelsDone[1] = 1;
		ck_gameState.currentLevel = 1;
		kw_placeArrivalAtShip = false;
		KW_BragAtTheBar();
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
	SS_AfterLoad();
	if (MARS_IsActive()) { MARS_MapFrame(); return; }
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
	if ((!kw_cardsOff || getenv("KW_AUTOSTORY")) && !kw_introduced[kw_currentWorld] &&
		(!kw_worlds[kw_currentWorld].visited || (kw_fitfCardDue && kw_currentWorld == KW_FITF)) &&
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
		kw_fitfCardDue = false;
		KW_ShowWorldStory();
		KW_ShowArrivalCard(kw_currentWorld);
	}

	// Inside the Megarocket: the flight console and the hatch.
	if (kw_currentWorld == KW_SHIP)
	{
		// Keen has just woken from the dream (Keen Dreams).
		if (kw_dreamEnded && !vl_screenFaded && IN_DemoGetMode() == IN_Demo_Off)
		{
			int how = kw_dreamEnded;
			kw_dreamEnded = 0;
			if (!kw_cardsOff)
				KW_ShowCards(how == 3 ? "Keen wakes up in his bunk,\nwell rested.\n\nNo vegetables to be seen.\nWas it all just a dream?" :
					how == 2 ? "Keen wakes with a start.\n\nOnly a dream! And it is\ngone: next time it will\nbegin all over again." :
					"Keen wakes up in his bunk.\n\nWhat a dream! It will be\nthere when he sleeps again.");
			SD_SetLastTimeCount(SD_GetTimeCount());
		}
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
				static const char *const numbers[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen"};
				int chapter = 0;
				for (int w = 0; w < KW_NUM_WORLDS; ++w)
					if (KW_IsPlanet(w) && w != world && (kw_worlds[w].visited || KW_WorldIsSaved(w)))
						++chapter;
				char text[160];
				int lastChapter = (int)(sizeof(numbers) / sizeof(numbers[0])) - 1;
				snprintf(text, sizeof(text), "The Chronicle\n\nChapter %s:\n%s", numbers[chapter <= lastChapter ? chapter : lastChapter], kw_worldDefs[world].name);
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
				if (!KW_IsPlanet(w) || kw_savedTold[w] || !KW_WorldIsSaved(w))
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
		// The Chronicle: a world has been saved, and the ship has learned
		// the course to the next.
		if (kw_chronicle && !kw_cardsOff && !vl_screenFaded && !KW_VoiceSpeaking())
		{
			for (int w = 0; w < KW_NUM_WORLDS; ++w)
			{
				if (!KW_IsPlanet(w) || kw_courseTold[w] || !KW_IsWorldPresent(w) || !KW_CourseIsOpen(w))
					continue;
				kw_courseTold[w] = 1;
				if (KW_ChronicleKey(w) == KW_NO_WORLD)
					continue; // open from the start: nothing was learnt
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
		ck_inputFrame.yDirection == IN_motion_Up &&
		KW_IsAtShipHatch())
		kw_launchRequested = true;
	else if (ca_mapOn == 0)
		KW_TryLaunch(ck_keenObj);
	// Foray in the Forest: its landed ship's door goes into the Megarocket
	// (the same ship: there is one inside to it, Keen Worlds' own, where a
	// world is picked), not into the game's own copy of the rooms. The game
	// has by now put Keen in one of those rooms, which lie below the forest
	// in the same level.
	if (kw_currentWorld == KW_FITF && KW_IsShipLevel() && ck_keenObj && KW_IsWorldPresent(KW_SHIP) &&
		ck_keenObj->clipRects.tileY2 >= KW_FITF_ROOMS_Y)
		kw_launchRequested = true;


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
				KW_ShowCards(KW_StrandedText());
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

	// Debug shortcuts: hold W and press 4 to 9 to fly to a world, E to see
	// the ending of the world Keen is in (on its map or at its ship), or A
	// to make that world the last one unsaved.
	// (KW_AUTOFINALE: the scripted test of Keen Worlds' ending. On a
	// world's map it makes that world the last one and wins it; the
	// ending's waits pass by themselves, and it leaves when Keen stands
	// free at the bar.)
	if (getenv("KW_AUTOFINALE") && ca_mapOn == 0 && KW_IsPlanet(kw_currentWorld) && !kw_chronicleEndTold)
	{
		for (int w = 0; w < KW_NUM_WORLDS; ++w)
			if (KW_IsPlanet(w) && w != kw_currentWorld && KW_IsWorldPresent(w))
				kw_worlds[w].completed = true;
		kw_cardsOff = false;
		printf("KW_AUTOFINALE: winning %s, the last world\n", kw_worldDefs[kw_currentWorld].id);
		fflush(stdout);
		KW_DebugEnding();
		return;
	}
	if (!IN_GetKeyState(IN_SC_W))
		return;
	if (IN_GetKeyState(IN_SC_E))
	{
		KW_WaitForButtonsUp();
		KW_DebugEnding();
		return;
	}
	// A: every other world counts as saved, so that this one is the last.
	// Its own ending (play it, or W + E) is then followed by Keen Worlds'
	// (KW_Finale).
	if (IN_GetKeyState(IN_SC_A))
	{
		int last = kw_currentWorld == KW_SHIP ? kw_boardedFrom : kw_currentWorld;
		KW_WaitForButtonsUp();
		for (int w = 0; w < KW_NUM_WORLDS; ++w)
			if (KW_IsPlanet(w) && w != last && KW_IsWorldPresent(w))
			{
				kw_worlds[w].completed = true;
				kw_savedTold[w] = 1; // (without the computer saying so for each)
			}
		kw_chronicleEndTold = 0; // (to see it again)
		char text[200];
		if (KW_IsPlanet(last) && !KW_WorldIsSaved(last))
			snprintf(text, sizeof(text), "Debug\n\nEvery other world now\ncounts as saved.\n%s is the last:\nfinish it, or press W + E\non its map.", kw_worldDefs[last].name);
		else
			snprintf(text, sizeof(text), "Debug\n\nEvery world counts as\nsaved. To see the ending\nagain, press W + E on\na world's map.");
		KW_ShowCards(text);
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
void KW_TestLevelSpritesCached(const char *world, int level)
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

void KW_TestRunObjects(int frames)
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

extern CK_object *ck_scoreBoxObj;
extern bool ck_scoreBoxEnabled;
void CK_UpdateScoreBox(CK_object *scorebox);

void KW_DumpScreenTo(const char *path); // (for other files: ck_text.c)

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

void KW_DumpScreenTo(const char *path)
{
	KW_DumpScreen(path);
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
	// KW_AUTOTALK=n: at frame n, everyone in Keen 4 says their lines (the
	// windows of its dialogues, one after another)
	if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && (kw_currentWorld == KW_KEEN4 || kw_currentWorld == KW_KEEN7 || kw_currentWorld == KW_KEEN95 || kw_currentWorld == KW_BOUNTY))
	{
		void CK4_ShowCouncilMessage(void);
		void CK4_ShowPrincessMessage(void);
		void CK4_ShowJanitorMessage(void);
		void CK4_ShowCantSwimMessage(void);
		void CK4_ShowWetsuitMessage(void);
		CK4_ShowCouncilMessage();
		CK4_ShowPrincessMessage();
		CK4_ShowJanitorMessage();
		CK4_ShowCantSwimMessage();
		CK4_ShowWetsuitMessage();
	}
	if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && (kw_currentWorld == KW_KEEN5 || kw_currentWorld == KW_CK55 || kw_currentWorld == KW_ASYLUM))
	{
		void CK5_FuseMessage(void);
		CK5_FuseMessage();
	}
	if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && kw_currentWorld == KW_KEEN9)
	{
		// (its friends' hints: who speaks depends on the level)
		void KW9_FriendMessage(void);
		int level = ck_gameState.currentLevel;
		static const int levels[] = {5, 11, 2, 1};
		for (int i = 0; i < 4; ++i)
		{
			ck_gameState.currentLevel = levels[i];
			KW9_FriendMessage();
		}
		ck_gameState.currentLevel = level;
	}
	if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && (kw_currentWorld == KW_KEEN6 || kw_currentWorld == KW_KEEN8))
	{
		void CK6_GrabbiterCol(CK_object *a, CK_object *b);
		void CK6_RocketCol(CK_object *a, CK_object *b);
		void CK6_MapCliffCol(CK_object *a, CK_object *b);
		void CK6_ShowGetSandwich(void);
		void CK6_ShowGetRope(void);
		void CK6_ShowGetPasscard(void);
		CK6_GrabbiterCol(ck_keenObj, ck_keenObj);
		CK6_RocketCol(ck_keenObj, ck_keenObj);
		CK6_MapCliffCol(ck_keenObj, ck_keenObj);
		CK6_ShowGetSandwich();
		CK6_ShowGetRope();
		CK6_ShowGetPasscard();
		CK6_GrabbiterCol(ck_keenObj, ck_keenObj);
	}
	if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && kw_currentWorld == KW_FITF && getenv("KW_AUTOCHUNK"))
	{
		// (Gnosticus Pi: one of its ending texts, by chunk number; KW_AUTOFLAGS:
		// what Keen has collected, as its bits)
		void CK_ShowTextPages(int textChunk, int song, bool blackBackground);
		void FF_TestCollected(int bits, int cans);
		extern const char *ck_textPagesVoice;
		if (getenv("KW_AUTOFLAGS"))
			FF_TestCollected(atoi(getenv("KW_AUTOFLAGS")), getenv("KW_AUTOCANS") ? atoi(getenv("KW_AUTOCANS")) : 0);
		ck_textPagesVoice = getenv("KW_AUTOVOICE");
		CK_ShowTextPages(atoi(getenv("KW_AUTOCHUNK")), -1, false);
		ck_textPagesVoice = NULL;
		RF_ForceRefresh();
	}
	else if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && kw_currentWorld == KW_FITF)
	{
		// (Gnosticus Pi: its talks with the janitor and Lindsey, and its other windows)
		void FF_JanitorDialog(int16_t number);
		void FF_LindseyDialog(int16_t number);
		void FF_MiscDialog(int16_t number);
		void FF_JanitorSleepMonolog(void);
		void FF_JanitorStunnedMonolog(void);
		for (int i = 0; i < 16; ++i)
			FF_JanitorDialog(i);
		for (int i = 0; i < 4; ++i)
			FF_LindseyDialog(i);
		for (int i = -2; i < 54; ++i)
			if (i < 1 || i > 3) // (1 to 3 are its jukebox and its two arcade games)
				FF_MiscDialog(i);
		FF_JanitorSleepMonolog();
		FF_JanitorStunnedMonolog();
	}
	if (getenv("KW_AUTOTALK") && count + 1 == atoi(getenv("KW_AUTOTALK")) && kw_currentWorld == KW_AREALM)
	{
		// (Krisle: every one of its text screens)
		void AR_EndScreens(int dialogue);
		for (int i = 0; i < 12; ++i)
			AR_EndScreens(i);
		RF_ForceRefresh();
	}
	// KW_AUTOWAKE=n: in the dream (Keen Dreams), Keen wakes up at frame n
	static int dreamFrames;
	if (kw_currentWorld!=KW_DREAM) dreamFrames=0;
	else if (getenv("KW_AUTOWAKE") && ++dreamFrames==atoi(getenv("KW_AUTOWAKE")))
		KW_WakeUp();
	// KW_AUTOLIVES=n: Keen has n lives from the first frame
	if (count == 0 && getenv("KW_AUTOLIVES"))
		ck_gameState.numLives = atoi(getenv("KW_AUTOLIVES"));
	if (killAt && count + 1 == killAt)
		CK_KillKeen();
	++count;
	if (!frames || count < frames)
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
bool KW_TestWriteSave(void)
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
bool KW_TestReadSave(void)
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
	return ok && (kw_deferredWorld==KW_NO_WORLD || KW_FinishDeferredLoad());
}

void KW_ChronicleSelfTest(void)
{
#define CHRONICLE_CHECK(test) do { if (!(test)) Quit("Chronicle check failed: " #test); } while (0)
	CK_NewGame();
	kw_chronicle = 1;
	kw_boardedFrom = KW_KEEN4;
	bool full = KW_IsWorldPresent(KW_KEEN5) && KW_IsWorldPresent(KW_KEEN95) && KW_IsWorldPresent(KW_AREALM);
	if (full)
	{
		CHRONICLE_CHECK(!KW_CourseIsOpen(KW_AREALM) && !KW_CourseIsOpen(KW_OSI) && !KW_CourseIsOpen(KW_ALPHAMATIC));
		kw_worlds[KW_KEEN5].completed = true;
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_AREALM) && !KW_CourseIsOpen(KW_OSI));
		if (KW_IsWorldPresent(KW_SUBURB)) CHRONICLE_CHECK(!KW_CourseIsOpen(KW_SUBURB));
		kw_worlds[KW_KEEN95].completed = true;
		// Spot's story: the Far Shadowlands first, then the station
		if (KW_IsWorldPresent(KW_RTTS))
		{
			CHRONICLE_CHECK(KW_CourseIsOpen(KW_RTTS) && !KW_CourseIsOpen(KW_OSI) && KW_ChronicleKey(KW_OSI) == KW_RTTS);
			kw_worlds[KW_RTTS].completed = true;
			puts("PASS: the Far Shadowlands opens after the journey home, and the Shikadi Station after it");
		}
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_OSI) && !KW_CourseIsOpen(KW_ALPHAMATIC));
		if (KW_IsWorldPresent(KW_SUBURB)) CHRONICLE_CHECK(KW_CourseIsOpen(KW_SUBURB));
		kw_worlds[KW_AREALM].completed = true;
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_ALPHAMATIC));
		kw_worlds[KW_KEEN95].completed = false;
		CHRONICLE_CHECK(!KW_CourseIsOpen(KW_ALPHAMATIC) && KW_ChronicleKey(KW_ALPHAMATIC) == KW_KEEN95);
		kw_worlds[KW_AREALM].completed = false;
		kw_worlds[KW_OSI].visited = true;
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_OSI));
		kw_worlds[KW_OSI].visited = false;
		kw_worlds[KW_OSI].completed = true;
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_OSI));
		kw_worlds[KW_OSI].completed = false;
		puts("PASS: story gates, both Alphamatic prerequisites and visited/completed access");
	}
	kw_worlds[KW_RTTS].completed = false;
	CHRONICLE_CHECK(KW_ChronicleKey(KW_OSI) == (KW_IsWorldPresent(KW_RTTS) ? KW_RTTS : KW_LastChronicleWorld()));
	kw_worlds[KW_RTTS].completed = true; // (the checks below are about the station's other gate)
	if (!KW_IsWorldPresent(KW_KEEN95) && KW_IsWorldPresent(KW_KEEN9))
	{
		CHRONICLE_CHECK(KW_ChronicleKey(KW_OSI) == KW_KEEN9 && !KW_CourseIsOpen(KW_OSI));
		kw_worlds[KW_KEEN9].completed = true;
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_OSI));
		kw_worlds[KW_KEEN9].completed = false;
		puts("PASS: missing Eight Accumulators falls back to Battle of the Brains");
	}
	if (!KW_IsWorldPresent(KW_AREALM) && KW_IsWorldPresent(KW_KEEN95))
	{
		kw_worlds[KW_KEEN95].completed = false;
		CHRONICLE_CHECK(!KW_CourseIsOpen(KW_ALPHAMATIC));
		kw_worlds[KW_KEEN95].completed = true;
		CHRONICLE_CHECK(KW_CourseIsOpen(KW_ALPHAMATIC));
		kw_worlds[KW_KEEN95].completed = false;
		puts("PASS: missing Krisle is passed over while Alphamatic retains its late-story gate");
	}
	kw_worlds[KW_KEEN5].completed = true;
	// Real version-7 save blocks: new rules must not be mistaken for old
	// ones, and old courses stay open through a second save/load.
	for (int legacy = 0; legacy < 3; ++legacy)
	{
		kw_worlds[KW_AREALM].completed = legacy == 2;
		memset(kw_courseTold, 0, sizeof(kw_courseTold));
		FS_File fp = tmpfile();
		CHRONICLE_CHECK(fp && KW_WriteSaveBlock(fp));
		if (legacy)
		{
			long size = ftell(fp);
			CHRONICLE_CHECK(size > 522);
			FS_SeekTo(fp, size - (261 - 215) * 2);
			int16_t zero = 0;
			CHRONICLE_CHECK(FS_WriteInt16LE(&zero, 1, fp) == 1);
		}
		FS_SeekTo(fp, 0);
		bool deferred;
		CHRONICLE_CHECK(KW_ReadSaveBlock(fp, &deferred) && !deferred);
		FS_CloseFile(fp);
		if (full)
		{
			CHRONICLE_CHECK(KW_CourseIsOpen(KW_OSI) == (legacy != 0));
			CHRONICLE_CHECK(KW_CourseIsOpen(KW_ALPHAMATIC) == (legacy == 2));
			CHRONICLE_CHECK(kw_courseTold[KW_AREALM] == (legacy != 0));
			if (KW_IsWorldPresent(KW_SUBURB)) CHRONICLE_CHECK(!KW_CourseIsOpen(KW_SUBURB));
		}
		if (legacy)
		{
			fp = tmpfile();
			CHRONICLE_CHECK(fp && KW_WriteSaveBlock(fp));
			memset(kw_courseTold, 0, sizeof(kw_courseTold));
			FS_SeekTo(fp, 0);
			CHRONICLE_CHECK(KW_ReadSaveBlock(fp, &deferred) && !deferred);
			FS_CloseFile(fp);
			CHRONICLE_CHECK(kw_courseTold[KW_AREALM] && kw_courseTold[KW_OSI]);
		}
	}
	bool known[KW_NUM_WORLDS] = {0};
	memset(kw_courseTold, 0, sizeof(kw_courseTold));
	KW_KeepLegacyCourses(kw_worlds, known);
	CHRONICLE_CHECK(!kw_courseTold[KW_ALPHAMATIC]);
	kw_chronicle = 0;
	CHRONICLE_CHECK(KW_CourseIsOpen(KW_ALPHAMATIC) && KW_CourseIsOpen(KW_OSI));
	KW_ResetWorlds();
	puts("PASS: legacy unvisited courses survive migration and resaving; new saves retain gates; Free Flight stays open");
#undef CHRONICLE_CHECK
}

// Developer self-test (/KWTEST): hop between all worlds several times, loading
// the world map and two levels each visit, and log memory use after every hop
// so leaks show up as steady growth.
static bool kw_keen8Saved;

void KW_SelfTest(void)
{
	static const int route[] = {KW_KEEN5, KW_KEEN6, KW_KEEN4, KW_KEEN7, KW_KEEN6, KW_KEEN5, KW_KEEN7, KW_KEEN4};
	bool haveKeen7 = KW_IsWorldPresent(KW_KEEN7);
	kw_cardsOff = true;
	uint16_t askW, askH;
	CK_MeasureMultiline(KW_KRISLE_WARNING, &askW, &askH);
	if ((askW + 15) / 8 + 2 > KW_CARD_MAX_W || (askH + 7) / 8 + 2 > KW_CARD_MAX_H)
		Quit("KW_SelfTest: the Krisle warning doesn't fit the screen!");
	{
		const char *cards[] = {KW_CALIDUNE_WARNING, KW_CALIDUNE_ALONE, KW_KRISLE_WRECK};
		for (unsigned c = 0; c < sizeof(cards) / sizeof(cards[0]); ++c)
		{
			CK_MeasureMultiline(cards[c], &askW, &askH);
			if ((askW + 15) / 8 + 2 > KW_CARD_MAX_W || (askH + 7) / 8 + 2 > KW_CARD_MAX_H)
				QuitF("KW_SelfTest: a stranding card doesn't fit the screen: %.30s", cards[c]);
		}
	}
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
				ck_inputFrame.yDirection = IN_motion_Up;
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

	if (KW_IsWorldPresent(KW_ALPHAMATIC))
	{
		KW_SwitchWorld(KW_ALPHAMATIC);
		for (int lv=0;lv<=25;++lv)
		{
			ck_gameState.currentLevel=lv;
			CK_LoadLevel(true,true);
			KW_TestLevelSpritesCached("alpham",lv);
			KW_TestRunObjects(90);
		}
		KW_SwitchWorld(KW_KEEN4);
	}
	if (KW_IsWorldPresent(KW_SUBURB))
	{
		KW_SwitchWorld(KW_SUBURB);
		for(int lv=0;lv<=23;++lv)
		{
			ck_gameState.currentLevel=lv;
			CK_LoadLevel(true,true);
			KW_TestLevelSpritesCached("suburb",lv);
			ck_gameState.levelState=LS_Playing;
			KW_TestRunObjects(90);
		}
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
		bool console = false, hatch = false, aft = false, fore = false, arcade = false;
		for (int y = 0; y < CA_GetMapHeight(); ++y)
			for (int x = 0; x < CA_GetMapWidth(); ++x)
			{
				console |= CA_TileAtPos(x, y, 2) == KW_SHIP_CONSOLE;
				if (CA_TileAtPos(x, y, 2) == KW_SHIP_ARCADE)
				{
					arcade = true;
					uint16_t tile = CA_TileAtPos(x, y, 1);
					if (TI_ForeTop(tile) || TI_ForeBottom(tile) || TI_ForeLeft(tile) || TI_ForeRight(tile))
						Quit("KW_SelfTest: arcade obstructs the cabin!");
				}
				hatch |= CA_TileAtPos(x, y, 2) == KW_SHIP_HATCH;
				aft |= CA_TileAtPos(x, y, 2) == KW_SHIP_WAY_AFT;
				fore |= CA_TileAtPos(x, y, 2) == KW_SHIP_WAY_FORE;
			}
		if (!arcade)
			Quit("KW_SelfTest: the Megarocket has no arcade cabinet!");
		if (!console || !hatch)
			Quit("KW_SelfTest: the Megarocket's room has no flight console or no hatch!");
		if (CA_TileAtPos(kw_shipHintArt.originX+1, kw_shipHintArt.originY, 2) != KW_SHIP_CONSOLE)
			Quit("KW_SelfTest: flight controls are not under the cabin's blinking Up sign!");
		if (!aft || !fore)
			Quit("KW_SelfTest: the Megarocket's two rooms are not joined!");
	}
	KW_TestRunObjects(60);
	KW_SwitchWorld(KW_KEEN4);

	// Tuberia, the dream (Keen Dreams, when its converted files are there):
	// Keen's score, lives and shots stay behind and come back; every level
	// loads with its sprites in memory and its creatures run, as vegetables
	// and as the flowers that flower power makes of them; and a lost dream
	// begins again.
	if (KW_IsWorldPresent(KW_DREAM))
	{
		KW_Tally mine = KW_LiveTally(), test = {4321, 60000, 5, 7};
		KW_WorldState dream = kw_worlds[KW_DREAM];
		KW_Tally dreamWas = kw_dreamTally;
		KW_SetTally(&test);
		kw_worlds[KW_DREAM].visited = false;
		KW_SwitchWorld(KW_DREAM);
		if (ck_gameState.keenScore != 0 || ck_gameState.numLives != 3 || ck_gameState.numShots != 0 ||
			ck_gameState.currentLevel != 0)
			Quit("KW_SelfTest: a new dream doesn't begin as Keen Dreams does!");
		int loaded = 0;
		for (int lv = 0; lv <= 16; ++lv)
		{
			if (!CA_MapExists(lv))
				continue;
			ck_gameState.currentLevel = lv;
			CK_LoadLevel(true, true);
			if (!ck_keenObj || !ck_keenObj->currentAction || ca_mapOn != lv)
				QuitF("KW_SelfTest: Keen Dreams' map %d has no Keen!", lv);
			KW_TestLevelSpritesCached("dreams", lv);
			bool god = ck_godMode;
			ck_godMode = true;
			KW_TestRunObjects(150);
			for (CK_object *obj = ck_keenObj->next; obj; obj = obj->next)
				if (obj->currentAction && KD_IsVegetable(obj))
					KD_ChangeToFlower(obj);
			KW_TestRunObjects(150);
			KW_TestLevelSpritesCached("dreams", lv);
			ck_godMode = god;
			ck_gameState.levelState = LS_Playing;
			++loaded;
		}
		if (loaded < 12)
			Quit("KW_SelfTest: Keen Dreams' levels are missing!");
		ck_gameState.keenScore = 100;
		ck_gameState.numShots = 9;
		KW_SwitchWorld(KW_SHIP);
		if (ck_gameState.keenScore != 4321 || ck_gameState.numLives != 5 || ck_gameState.numShots != 7)
			Quit("KW_SelfTest: Keen's own score, lives and shots didn't wait for him while he dreamt!");
		KW_SwitchWorld(KW_DREAM);
		if (ck_gameState.keenScore != 100 || ck_gameState.numShots != 9)
			Quit("KW_SelfTest: the dream forgot its score!");
		ck_gameState.numLives = -1;
		if (!KW_DreamOver(false) || kw_currentWorld != KW_SHIP || ck_gameState.numLives != 5 ||
			kw_worlds[KW_DREAM].visited || kw_dreamTally.lives != 3 || kw_dreamTally.score != 0)
			Quit("KW_SelfTest: a lost dream doesn't end with Keen awake in the ship and a new dream to come!");
		kw_dreamEnded = 0;
		kw_wakeInBed = false;
		KW_SwitchWorld(KW_KEEN4);
		kw_worlds[KW_DREAM] = dream;
		kw_dreamTally = dreamWas;
		KW_SetTally(&mine);
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
		// boarding spot beside it, away from level entrances. Calidune has no
		// ship until it is saved (Mortimer's transporter left Keen alone).
		ck_gameState.currentLevel = 0;
		if (fan == KW_KEEN8)
		{
			kw_keen8Saved = kw_worlds[fan].completed;
			kw_worlds[fan].completed = false;
			CK_LoadLevel(true, true);
			if (KW_MapShipFor(fan) || KW_IsArtPlaced(&kw_keen8ShipArt) || !KW_IsStranded())
				Quit("KW_SelfTest: Calidune should have no Megarocket before it is saved!");
			kw_worlds[fan].completed = true; // (until the checks below are done)
			CK_LoadLevel(true, true);
		}
		else
			CK_LoadLevel(true, true);
		const KW_MapShip *ms = KW_MapShipFor(fan);
		if (!ms || !KW_IsArtPlaced(ms->ship) ||
			kw_launchSpotX != ms->launchX || kw_launchSpotY != ms->launchY ||
			!TI_ForeTop(CA_TileAtPos(ms->ship->originX, ms->ship->originY, 1)))
			Quit("KW_SelfTest: a fan game's parked Megarocket is missing!");
		if (fan == KW_KEEN8)
			kw_worlds[fan].completed = kw_keen8Saved;
		for (int y = kw_launchSpotY - KW_LAUNCH_RANGE - 1; y <= kw_launchSpotY + KW_LAUNCH_RANGE + 1; ++y)
			for (int x = kw_launchSpotX - KW_LAUNCH_RANGE - 1; x <= kw_launchSpotX + KW_LAUNCH_RANGE + 1; ++x)
				if ((CA_TileAtPos(x, y, 2) & 0xFF00) == 0xC000)
					Quit("KW_SelfTest: a parked Megarocket's boarding spot is next to a level entrance!");
		// Keen lands right beside it (not on it) and can board at once.
		{
			int shipX = RF_TileToUnit(ms->ship->originX);
			kw_launchRequested = false;
			ck_inputFrame.yDirection = IN_motion_Up;
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
			ck_inputFrame.yDirection = IN_motion_None;
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
	KW_TestBoarding();
	if ((getenv("KW_AUTOMENU") || getenv("KW_AUTOMENUALL")) && KW_IsWorldPresent(KW_SHIP))
	{
		KW_SwitchWorld(KW_SHIP);
		CK_LoadLevel(true, true);
		KW_TravelMenu();
	}
	// Printed whatever the log level: which worlds were there to be tested.
	printf("KW_SelfTest: passed. Worlds tested:");
	for (int w = 0; w < KW_NUM_WORLDS; ++w)
		if (KW_IsWorldPresent(w))
			printf(" %s", kw_worldDefs[w].id);
	printf("\n");
}

// Real watch drawing and the cabin's blinking console interaction.
void KW_TestMenuScreens(void)
{
	kw_cardsOff=true;
	KW_SwitchWorld(KW_SHIP);CK_NewGame();
	ck_gameState.difficulty=D_Normal;ck_gameState.currentLevel=1;CK_LoadLevel(true,true);
	const char *watchWorld=getenv("KW_WATCHWORLD");
	if(watchWorld)
	{
		int world;
		for(world=0;world<KW_NUM_WORLDS;++world)
			if(!strcmp(watchWorld,KW_WorldId(world))) break;
		if(world==KW_NUM_WORLDS || !KW_IsWorldPresent(world)) Quit("Menu test: requested watch world is unavailable");
		KW_SwitchWorld(world);CK_LoadLevel(true,true);
	}
	CK_GameState before=ck_gameState;
	extern int game_in_progress;
	game_in_progress=1;
	US_RunCards();
	if(getenv("KW_AUTOENDGAME"))
	{
		extern int load_game_error;
		if(!load_game_error) Quit("End Game test: the watch did not end the game");
		load_game_error=0;ck_gameState.levelState=LS_AbortGame;
		CK_GameState ended=ck_gameState;game_in_progress=0;
		CK_DoHighScores();
		if(memcmp(&ended,&ck_gameState,sizeof(ended))) Quit("End Game test: the high-score panel changed the journey");
		char path[600];snprintf(path,sizeof(path),"%s/highscores.ppm",getenv("KW_AUTOWATCH"));KW_DumpScreen(path);
		puts("PASS: End Game and the shared high-score panel avoid episode demo maps");
		CK_ShowTitleScreen();
		Quit("End Game test: expected the automated title capture");
	}
	if(memcmp(&before,&ck_gameState,sizeof(before))) Quit("Menu test: watch changed game state");
	if(KW_CurrentWorld()!=KW_SHIP) {KW_SwitchWorld(KW_SHIP);CK_LoadLevel(true,true);}
	ck_keenObj->posX=RF_TileToUnit(kw_shipHintArt.originX);
	ck_keenObj->posY=RF_TileToUnit(kw_shipHintArt.originY)-241;
	CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_keenStanding));
	memset(&ck_inputFrame,0,sizeof(ck_inputFrame));SD_SetSpriteSync(2);
	for(int frame=0;frame<3;++frame) CK_RunAction(ck_keenObj);
	if(ck_keenObj->clipRects.tileXmid!=kw_shipHintArt.originX || !ck_keenObj->topTI)
		Quit("Menu test: Keen cannot stand at the cabin's blinking console");
	kw_shipUpUsed=false;ck_inputFrame.yDirection=IN_motion_Up;
	KW_PlayLoopHook();
	if(!kw_shipUpUsed) Quit("Menu test: Up at blinking console did not open flight controls");
	ck_inputFrame.yDirection=IN_motion_None;
	puts("PASS: cabin flight chart opens with Up under the blinking arrow; watch and chart preserve game progress");
}

// Player feedback: deliberate Up boarding on every installed destination.
void KW_TestBoarding(void)
{
#define BOARD_CHECK(test) do { if (!(test)) QuitF("Boarding test %s line %d: %s", KW_WorldId(kw_currentWorld), __LINE__, #test); } while (0)
	bool cards = kw_cardsOff;
	kw_cardsOff = true;
	CK_NewGame();
	ck_gameState.difficulty = D_Normal;
	int checked = 0;
	for (int world = 0; world < KW_NUM_WORLDS; ++world)
	{
		if (!KW_IsPlanet(world) || !KW_IsWorldPresent(world)) continue;
		KW_SwitchWorld(world);
		kw_worlds[world].visited = kw_introduced[world] = true;
		kw_crashLanding = false;
		kw_arrivalCard = kw_landingToAnnounce = KW_NO_WORLD;
		kw_fitfFlyIn = false;
		if (world == KW_KEEN8)
			kw_worlds[world].completed = true; // (Calidune's ship stands there only once it is saved)
		const KW_Ship *ship = KW_ShipFor(world);
		ck_gameState.currentLevel = ship ? ship->level : 0;
		kw_placeArrivalAtShip = ship || KW_MapShipFor(world);
		CK_LoadLevel(true, true);
		ck_gameState.levelState = LS_Playing;
		kw_pendingWorld = KW_NO_WORLD;
		kw_launchRequested = false;
		memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
		ck_keenState.jumpIsPressed = ck_keenState.pogoIsPressed = ck_keenState.shootIsPressed = false;
		if (!ship && (KW_HasKeen5Dock() || world == KW_ALPHAMATIC))
		{
			// Walk from the actual map spawn along the dock to the ship.
			ck_inputFrame.dir = IN_dir_East;
			ck_inputFrame.xDirection = IN_motion_Right;
			CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_MapKeenWalk0));
			SD_SetSpriteSync(2);
			for (int frame = 0; frame < 400; ++frame) CK_RunAction(ck_keenObj);
			memset(&ck_inputFrame, 0, sizeof(ck_inputFrame));
			if (world == KW_ALPHAMATIC)
				BOARD_CHECK(KW_IsArtPlaced(&kw_alDockArt));
		}
		CK_object parked = *ck_keenObj;
		// The three gameplay buttons must never cause an accidental boarding.
		bool *buttons[] = {&ck_keenState.jumpIsPressed, &ck_keenState.pogoIsPressed, &ck_keenState.shootIsPressed};
		for (int button = 0; button < 3; ++button)
		{
			*buttons[button] = true;
			KW_TryLaunch(ck_keenObj);
			KW_PlayLoopHook();
			BOARD_CHECK(kw_pendingWorld == KW_NO_WORLD && !kw_launchRequested);
			*buttons[button] = false;
		}
		ck_inputFrame.yDirection = IN_motion_Up;
		if (world == KW_AREALM || world == KW_KEEN8)
		{
			kw_worlds[world].completed = false;
			KW_PlayLoopHook();
			BOARD_CHECK(kw_pendingWorld == KW_NO_WORLD); // stranded: the wreck, or no ship on Calidune
			kw_worlds[world].completed = true;
		}
		KW_PlayLoopHook();
		BOARD_CHECK(kw_pendingWorld == KW_SHIP && kw_boardedFrom == world);
		kw_pendingWorld = KW_NO_WORLD;
		// Up elsewhere in the level/map is ordinary movement or a local door.
		ck_keenObj->posX = RF_TileToUnit(parked.clipRects.tileXmid < CA_GetMapWidth()/2 ? CA_GetMapWidth()-3 : 3);
		CK_ResetClipRects(ck_keenObj);
		KW_PlayLoopHook();
		BOARD_CHECK(kw_pendingWorld == KW_NO_WORLD);
		*ck_keenObj = parked;
		ck_inputFrame.yDirection = IN_motion_None;
		if (world == KW_FITF)
		{
			// Foray's own Up-entered ship door still hands over to the cabin,
			// even if the key was released during its door animation.
			ck_keenObj->posY = RF_TileToUnit(KW_FITF_ROOMS_Y + 2);
			CK_ResetClipRects(ck_keenObj);
			KW_PlayLoopHook();
			BOARD_CHECK(kw_pendingWorld == KW_SHIP);
			kw_pendingWorld = KW_NO_WORLD;
		}
		++checked;
		printf("PASS: %s boards with Up at the ship; jump/pogo/fire and distant Up do not board\n", KW_WorldId(world));
	}
	BOARD_CHECK(checked > 0);
	kw_cardsOff = cards;
	printf("PASS: boarding checked on %d destinations, including map docks, ship levels, Krisle's wreck and Foray's door when installed\n", checked);
#undef BOARD_CHECK
}

// Real-data regression for the cabinet's in-engine remake. Developer-only.
void KW_TestArcadeSession(void)
{
#define ARCADE_CHECK(test) do { if (!(test)) QuitF("Arcade session test line %d: %s",__LINE__,#test); } while (0)
 bool cards=kw_cardsOff;kw_cardsOff=true;
 KW_SwitchWorld(KW_SHIP);
 CK_NewGame();ck_gameState.difficulty=D_Hard;
 ck_gameState.keenScore=54321;ck_gameState.nextKeenAt=80000;
 ck_gameState.numLives=7;ck_gameState.numShots=29;ck_gameState.numCentilife=67;
 ck_gameState.currentLevel=1;CK_LoadLevel(true,true);
 kw_worlds[KW_KEEN4].completed=true;kw_chronicle=1;kw_boardedFrom=KW_KEEN5;
 int x=ck_keenObj->posX,y=ck_keenObj->posY;
 ARCADE_CHECK(KW_BeginArcade() && kw_pendingWorld==KW_MARS);
 KW_SwitchWorld(kw_pendingWorld);
 ARCADE_CHECK(ck_gameState.numShots==0 && ck_gameState.numLives==3 && ck_gameState.keenScore==0);
 ARCADE_CHECK(ck_gameState.numCentilife==0 && ck_gameState.difficulty==D_Normal);
 ck_gameState.currentLevel=1;CK_LoadLevel(true,true);
 ck_gameState.keenScore=1234;ck_gameState.numShots=9;ck_gameState.numCentilife=11;
 ck_gameState.ep.ck5.securityCard=1;ck_gameState.levelsDone[2]=1;
 const char *oldFile=kw_testSaveFile;kw_testSaveFile="KW-ARCADE-TEST.SAV";
 ARCADE_CHECK(KW_TestWriteSave());
 KW_RequestCabinReturn();KW_FinishMenuActions();
 ARCADE_CHECK(kw_pendingWorld==KW_SHIP);
 KW_SwitchWorld(kw_pendingWorld);CK_LoadLevel(true,true);
 ARCADE_CHECK(ck_gameState.keenScore==54321 && ck_gameState.nextKeenAt==80000);
 ARCADE_CHECK(ck_gameState.numLives==7 && ck_gameState.numShots==29 && ck_gameState.numCentilife==67);
 ARCADE_CHECK(ck_gameState.difficulty==D_Hard && ck_keenObj->posX==x && ck_keenObj->posY==y);
 // Load the remake's unified save from the cabin through the real deferred loader.
 ck_gameState.keenScore=99999;
 ARCADE_CHECK(KW_TestReadSave());
 ARCADE_CHECK(KW_CurrentWorld()==KW_MARS && ck_gameState.keenScore==1234);
 ARCADE_CHECK(ck_gameState.ep.ck5.securityCard==1 && ck_gameState.levelsDone[2]==1);
 KW_RequestCabinReturn();KW_FinishMenuActions();
 KW_SwitchWorld(kw_pendingWorld);CK_LoadLevel(true,true);
 ARCADE_CHECK(ck_gameState.keenScore==54321 && ck_gameState.numShots==29 && ck_gameState.numCentilife==67);
 ARCADE_CHECK(kw_chronicle==1 && kw_boardedFrom==KW_KEEN5 && kw_worlds[KW_KEEN4].completed);
 ARCADE_CHECK(KW_BeginArcade());KW_SwitchWorld(kw_pendingWorld);
 ARCADE_CHECK(ck_gameState.keenScore==1234 && ck_gameState.numShots==9 && ck_gameState.numCentilife==11);
 ck_startingDifficulty=D_Hard;CA_UpLevel();CK_ExitMenu();CA_DownLevel();
 ARCADE_CHECK(kw_marsLinked && kw_worlds[KW_KEEN4].completed);
 ARCADE_CHECK(KW_RestartArcade(D_Hard));ck_startingDifficulty=D_NotPlaying;
 ARCADE_CHECK(!ck_gameState.levelsDone[2] && !ck_gameState.ep.ck5.securityCard && ck_gameState.numLives==3);
 ARCADE_CHECK(kw_worlds[KW_KEEN4].completed && kw_chronicle==1);
 ARCADE_CHECK(KW_ArcadeOver(false) && KW_CurrentWorld()==KW_SHIP);
 ARCADE_CHECK(ck_gameState.keenScore==54321 && ck_gameState.numLives==7 && ck_gameState.numShots==29);
 ARCADE_CHECK(KW_BeginArcade());KW_SwitchWorld(kw_pendingWorld);
 ck_gameState.currentLevel=0;CK_LoadLevel(true,true);
 CA_UpLevel();KW_AfterWorldEnding();
 ARCADE_CHECK(KW_CurrentWorld()==KW_SHIP && kw_worlds[KW_MARS].completed);
 ARCADE_CHECK(kw_worlds[KW_KEEN4].completed && kw_chronicle==1 && ck_gameState.keenScore==54321);
 kw_testSaveFile=oldFile;kw_loadingFileName=NULL;kw_cardsOff=cards;
 puts("PASS: Mars same-window entry/return, isolated inventory/difficulty, cabinet position, shared cross-world save, restart, game over and ending");
#undef ARCADE_CHECK
}
