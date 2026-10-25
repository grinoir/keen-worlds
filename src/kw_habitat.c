/*
Keen Worlds: habitable levels. Once Keen has completed a level (in any
world), he can come back and finds it peaceful: the enemies have gone, and
some levels have residents Keen can talk to (stand next to one, press up).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdio.h>
#include <stdlib.h>

#include "id_ca.h"
#include "id_in.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_us.h"
#include "id_ti.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_ep.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "kw_art.h"
#include "kw_habitat.h"
#include "kw_mars.h"
#include "kw_music.h"
#include "id_fs.h"
#include <string.h>

// Atroxian Realm has its own lists (kw_arealm.c).
bool AR_IsActive(void);
bool AR_IsHostileType(int type);
bool AR_IsStoryTrigger(const CK_object *obj);
bool FF_IsActive(void);
bool FF_IsHostileType(int type);
bool FF_IsStoryTrigger(const CK_object *obj);


#include "kw_world.h"
#include "kw_suburb.h"

typedef enum KW_When
{
	KW_ALWAYS = 0,
	KW_AFTER = 1,  // Only once the level is completed (the default for art).
	KW_BEFORE = 2, // Only until the level is completed.
} KW_When;

// One tile change in a level (built by a generator in tools/).
typedef struct KW_MapEdit
{
	uint8_t when; // KW_ALWAYS or KW_AFTER
	uint8_t plane;
	uint8_t x, y;
	uint16_t value;
} KW_MapEdit;

#include "kw_tavern_art.h"

// ---- Residents ----
// Who lives where. Positions are the tile the resident stands on (as an
// info-plane spawn marker would give). Lines may use \n; keep them short, the
// dialogue box is narrow.
typedef enum KW_ResidentKind
{
	KW_ELDER,   // A Keen 4 Oracle Council elder, spawned as a character.
	KW_PAINTED, // Drawn into level art (e.g. the barmaid); just a talk spot.
} KW_ResidentKind;

// What Keen can say, and the resident's answer.
typedef struct KW_Choice
{
	const char *keen;
	const char *reply;
} KW_Choice;

#define KW_MAX_CHOICES 3

// A longer talk (the barmaid's): at each place the resident says something
// and Keen picks one of up to four lines; the resident answers (or not) and
// the talk goes on at another place, or ends (-1). Text may run over several
// cards (pages parted by \f). Every line of the resident's has a voice clip.
#define KW_MAX_TALK_CHOICES 4
typedef struct KW_TalkChoice
{
	const char *keen;
	const char *reply, *replyVoice;
	int next;
	int world; // offered only if this world is installed (-1: always)
} KW_TalkChoice;

typedef struct KW_TalkNode
{
	const char *says, *voice;
	KW_TalkChoice choices[KW_MAX_TALK_CHOICES];
} KW_TalkNode;

// The barmaid's talk, written by tools/kw_barmaid_voice.py.
#include "kw_barmaid_talk.h"

typedef struct KW_Resident
{
	int episode; // the world (KW_World)
	int level;
	int tileX, tileY;
	const char *says;                     // The resident's greeting.
	KW_Choice choices[KW_MAX_CHOICES];    // Keen picks one (unused: NULL).
	KW_ResidentKind kind;
	const KW_TalkNode *talk;              // A longer talk instead (says and choices unused).
} KW_Resident;

static const KW_Resident kw_residents[] = {
	// Keen 4, Border Village: an elder of the Oracle Council has moved into
	// the first house (its door leads to the small room at the bottom left).
	{KW_KEEN4, 1, 4, 48,
		"Welcome back, Keen!\nThe Border Village\nis safe again,\nthanks to you.",
		{
			{"How's the village?", "Peaceful at last.\nThe villagers are\nback in their homes."},
			{"What's the new inn?", "The barmaid opened\nit once the slugs\nleft. Say hello!"},
			{"Goodbye!", "Farewell, Keen.\nMay the Oracle\nguide you."},
		},
		KW_ELDER},
	// Keen 4, Border Village: the barmaid behind the bar in the tavern
	// (drawn and animated by tools/kw_barhouse.py).
	{KW_KEEN4, 1, KW_TAVERN_BARMAID_X + 1, KW_TAVERN_BARMAID_Y,
		NULL, {{NULL, NULL}}, KW_PAINTED, kw_barmaidTalk},
};
#define KW_NUM_RESIDENTS ((int)(sizeof(kw_residents) / sizeof(kw_residents[0])))

// (when every world is saved: see KW_BragAtTheBar)
static bool kw_bragDue;
static int kw_talkStart;

// How close (in tiles) Keen must be to talk.
#define KW_TALK_RANGE 1

// Resident objects carry their table index in user4 (saved with the game),
// tagged so they can be recognised among other objects.
#define KW_RESIDENT_TAG 0x4B00
#define KW_RESIDENT_TAG_MASK 0xFF00

static bool kw_levelIsPeaceful = false;

// Keen Worlds additions belong to a world (see KW_World), not to every world
// sharing its engine episode: Keen 7 runs on Keen 4's code but is not Keen 4.
static int KW_CurrentEpisode(void)
{
	return KW_CurrentWorld();
}

bool KW_IsHabitableLevel(int level)
{
	// Every completed level can be visited again (residents or not).
	(void)level;
	return !MARS_IsActive();
}

bool KW_IsLevelPeaceful(void)
{
	return kw_levelIsPeaceful;
}

// ---- Changes to the levels ----
// Keen Worlds changes belong to the village after it is freed: by default
// they only appear once the level is completed ("the villagers are back with
// their things"). A piece can instead be shown only before, or always.


// Border Village's tavern (tools/kw_barhouse.py): a long stone inn on the
// street in place of the fourth house, its tavern door boarded up until the
// village is freed, and the bar room carved into the rock below: portrait,
// tables, and the bar with the barmaid.
// (kw_tavern_art.h is included above, with the residents.)
// The creator's portrait (tools/kw_portrait_art.py), in the tavern room.
#include "kw_portrait_art.h"

// The inn's front: one long stone house replacing the fourth house, with its
// original door on the left and the tavern door on the right (always there).
const KW_Art kw_innArt = {
	KW_INN_ORIGIN_X, KW_INN_ORIGIN_Y, KW_INN_TILES_W, KW_INN_TILES_H, KW_INN_FRAMES,
	&kw_innSlots[0][0][0], &kw_innTiles[0][0][0][0], -1, 0, NULL,
};
// The inn's BAR sign, on a wall bracket beside the tavern door (always
// there; the boards on the door show it is closed).
const KW_Art kw_signArt = {
	KW_SIGN_ORIGIN_X, KW_SIGN_ORIGIN_Y, KW_SIGN_TILES_W, KW_SIGN_TILES_H, 1,
	&kw_signSlots[0][0][0], &kw_signTiles[0][0][0][0], -1, 0, NULL,
};
const KW_Art kw_tavernBoardsArt = {
	KW_BOARDS_ORIGIN_X, KW_BOARDS_ORIGIN_Y, KW_BOARDS_TILES_W, KW_BOARDS_TILES_H, KW_BOARDS_FRAMES,
	&kw_boardsSlots[0][0][0], &kw_boardsTiles[0][0][0][0], -1, 0, NULL,
	true, // nailed shut: the door can't be entered
};
const KW_Art kw_tavernBarArt = {
	KW_TAVERN_ORIGIN_X, KW_TAVERN_ORIGIN_Y, KW_TAVERN_TILES_W, KW_TAVERN_TILES_H, KW_TAVERN_FRAMES,
	&kw_tavernSlots[0][0][0], &kw_tavernTiles[0][0][0][0], -1,
	14, // the barmaid's drying motion
	NULL,
};
const KW_Art kw_tavernFurnitureArt = {
	KW_FURNITURE_ORIGIN_X, KW_FURNITURE_ORIGIN_Y, KW_FURNITURE_TILES_W, KW_FURNITURE_TILES_H, KW_FURNITURE_FRAMES,
	&kw_furnitureSlots[0][0][0], &kw_furnitureTiles[0][0][0][0], -1, 0, NULL,
};
const KW_Art kw_portraitArt = {
	KW_PORTRAIT_ORIGIN_X, KW_PORTRAIT_ORIGIN_Y, KW_PORTRAIT_TILES_W, KW_PORTRAIT_TILES_H, 1,
	&kw_portraitSlots[0][0][0], &kw_portraitTiles[0][0][0][0], -1, 0, NULL,
};

// The inside of the Megarocket (Keen Worlds' own world): original low-wedge
// cabin art, generated by tools/kw_shiproom_art.py. One picture, in three
// pieces.
#include "kw_shiproom_art.h"
#define KW_SHIPROOM_PIECE(n) { \
	KW_SHIPROOM_ORIGIN_X, KW_SHIPROOM_ORIGIN_Y + (n) * KW_SHIPROOM_PIECE_H, KW_SHIPROOM_TILES_W, KW_SHIPROOM_PIECE_H, 1, \
	&kw_shipRoomSlots[n][0][0], &kw_shipRoomTiles[n][0][0][0], -1, 0, NULL, false, false, NULL, true, \
}
const KW_Art kw_shipRoomArt[KW_SHIPROOM_PIECES] = {KW_SHIPROOM_PIECE(0), KW_SHIPROOM_PIECE(1), KW_SHIPROOM_PIECE(2)};
// A blinking "up" sign beside the navigation screen.
const KW_Art kw_shipHintArt = {
	KW_SHIPHINT_X, KW_SHIPHINT_Y, 1, 1, 2,
	&kw_shipHintSlots[0][0][0], &kw_shipHintTiles[0][0][0][0], -1, 30, NULL,
};
static const KW_Art kw_shipArcadeArt = {
	KW_SHIPARCADE_X, KW_SHIPARCADE_Y, 2, 1, 2,
	&kw_shipArcadeSlots[0][0][0], &kw_shipArcadeTiles[0][0][0][0], -1, 35, NULL,
};

static const struct
{
	int episode, level;
	const KW_MapEdit *edits;
	int numEdits;
} kw_levelEdits[] = {
	{KW_KEEN4, 1, kw_tavernEdits, (int)(sizeof(kw_tavernEdits) / sizeof(kw_tavernEdits[0]))},
};

static const struct
{
	int episode, level;
	const KW_Art *art;
	KW_When when;
} kw_levelArt[] = {
	{KW_KEEN4, 1, &kw_innArt, KW_ALWAYS},          // first: the boards go on top
	{KW_KEEN4, 1, &kw_tavernBoardsArt, KW_BEFORE},
	{KW_KEEN4, 1, &kw_signArt, KW_ALWAYS},
	{KW_KEEN4, 1, &kw_tavernBarArt, KW_AFTER},
	{KW_KEEN4, 1, &kw_tavernFurnitureArt, KW_AFTER},
	{KW_KEEN4, 1, &kw_portraitArt, KW_AFTER},
	{KW_SHIP, 1, &kw_shipRoomArt[0], KW_ALWAYS},
	{KW_SHIP, 1, &kw_shipRoomArt[1], KW_ALWAYS},
	{KW_SHIP, 1, &kw_shipRoomArt[2], KW_ALWAYS},
	{KW_SHIP, 1, &kw_shipHintArt, KW_ALWAYS},
	{KW_SHIP, 1, &kw_shipArcadeArt, KW_ALWAYS},
};

static bool KW_IsShown(KW_When when, bool completed)
{
	return when == KW_ALWAYS || (when == KW_AFTER) == completed;
}

// ---- Krisle: the Megarocket, repaired ----
// Atroxian Realm's wreck stands whole again in its crash site level once
// that world is completed (the Krile repair it, as its story says): Keen 4's
// own Megarocket, drawn over the wreck (tools/kw_krisle_ship_art.py).

#include "kw_krisle_ship_art.h"

static const KW_Art kw_krisleShipArt = {
	KW_KRISLE_SHIP_X, KW_KRISLE_SHIP_Y, KW_KRISLE_SHIP_W, KW_KRISLE_SHIP_H, 1,
	&kw_krisleShipSlots[0][0][0], &kw_krisleShipTiles[0][0][0][0],
	-1, 0, NULL, false, false, NULL,
};

static void KW_PlaceKrisleShip(void)
{
	if (KW_CurrentWorld() != KW_AREALM || ca_mapOn != KW_KRISLE_SHIP_LEVEL || !KW_IsWorldCompleted(KW_AREALM))
		return;
	// What shows of the wreck above the hull goes (background plane).
	for (int i = 0; kw_krisleWreckClear[i][0]; ++i)
		CA_SetTileAtPos(kw_krisleWreckClear[i][0], kw_krisleWreckClear[i][1], 0,
			CA_TileAtPos(kw_krisleWreckClear[i][2], kw_krisleWreckClear[i][3], 0));
	KW_PlaceArt(&kw_krisleShipArt);
}

bool KW_KrisleShipIsPlaced(void)
{
	return CA_TileAtPos(KW_KRISLE_SHIP_X + 7, KW_KRISLE_SHIP_Y + 2, 1) == kw_krisleShipSlots[0][2][7];
}

static void KW_ChangeLevel(void)
{
	KW_PlaceKrisleShip();
	bool completed = ca_mapOn > 0 && ca_mapOn < CK_LEVELS_TRACKED && ck_gameState.levelsDone[ca_mapOn];
	for (int i = 0; i < (int)(sizeof(kw_levelEdits) / sizeof(kw_levelEdits[0])); ++i)
	{
		if (kw_levelEdits[i].episode != KW_CurrentEpisode() || kw_levelEdits[i].level != ca_mapOn)
			continue;
		for (int e = 0; e < kw_levelEdits[i].numEdits; ++e)
		{
			const KW_MapEdit *ed = &kw_levelEdits[i].edits[e];
			if (KW_IsShown((KW_When)ed->when, completed))
				CA_SetTileAtPos(ed->x, ed->y, ed->plane, ed->value);
		}
	}
	for (int i = 0; i < (int)(sizeof(kw_levelArt) / sizeof(kw_levelArt[0])); ++i)
	{
		if (kw_levelArt[i].episode != KW_CurrentEpisode() || kw_levelArt[i].level != ca_mapOn)
			continue;
		if (KW_IsShown(kw_levelArt[i].when, completed))
			KW_PlaceArt(kw_levelArt[i].art);
	}
}

// ---- Coming back ----
// Keen re-enters a completed level from the opposite side to where he first
// started — at its exit — so it looks like he walks back in. This is found
// automatically for every level of every world:
// - a level with an exit door (info value 0xB1B1, Keen 5): Keen stands at it;
// - a Keen 5 fuse level (no door: it ends when the last fuse breaks): beside
//   the fuses, which stay broken; on the QED level where the QED stood;
// - a level that ends at a goal — rescuing an Oracle Council member,
//   finding Keen 6's rope, sandwich or passcard, or Molly: where it was;
// - otherwise (levels left by walking off a side): the side opposite his
//   normal start, at a spot whose floor runs out to the map's edge.
// He faces back into the level. kw_revisitStarts[] can override a level.
static const struct
{
	int episode, level;
	int tileX, tileY; // as a Keen spawn marker: the row above the floor
} kw_revisitStarts[] = {
	{KW_NO_WORLD, 0, 0, 0}, // (none needed so far)
};

static bool KW_Solid(int x, int y)
{
	if (x < 0 || y < 0 || x >= CA_GetMapWidth() || y >= CA_GetMapHeight())
		return true;
	uint16_t t = CA_TileAtPos(x, y, 1);
	return TI_ForeTop(t) || TI_ForeBottom(t) || TI_ForeLeft(t) || TI_ForeRight(t) ||
		(TI_ForeMisc(t) & 0x7F) == MISCFLAG_DEADLY;
}

// Keen (about 2x2 tiles) can stand with his feet in row y at columns x, x+1.
static bool KW_Standable(int x, int y)
{
	// Inside the map's border only: a goal near the top (Keen 9's Warpcore)
	// must not put Keen above the map.
	if (x < 2 || y < 2 || x > CA_GetMapWidth() - 4 || y > CA_GetMapHeight() - 3)
		return false;
	if (KW_Solid(x, y) || KW_Solid(x + 1, y) || KW_Solid(x, y - 1) || KW_Solid(x + 1, y - 1))
		return false;
	return TI_ForeTop(CA_TileAtPos(x, y + 1, 1)) || TI_ForeTop(CA_TileAtPos(x + 1, y + 1, 1));
}

// The standing spot nearest a tile.
static bool KW_FindSpotNear(int tx, int ty, int *outX, int *outY)
{
	for (int r = 0; r <= 10; ++r)
		for (int dy = -r; dy <= r; ++dy)
			for (int dx = -r; dx <= r; ++dx)
				if ((abs(dx) == r || abs(dy) == r) && KW_Standable(tx + dx, ty + dy))
				{
					*outX = tx + dx;
					*outY = ty + dy;
					return true;
				}
	return false;
}

// The open space at Keen's height runs from x out to the map edge (column 2
// or width - 3, inside the border), so this is a walk-off exit.
static bool KW_ReachesEdge(int x, int y, int dir)
{
	int edge = dir < 0 ? 2 : CA_GetMapWidth() - 3;
	for (int cx = x; dir < 0 ? cx >= edge : cx <= edge; cx += dir)
		if (KW_Solid(cx, y) || KW_Solid(cx, y - 1))
			return false;
	return true;
}

static bool KW_FindExitDoor(int *outX, int *outY)
{
	bool found = false;
	for (int y = 0; y < CA_GetMapHeight(); ++y)
		for (int x = 0; x < CA_GetMapWidth(); ++x)
			if (CA_TileAtPos(x, y, 2) == 0xB1B1 && (!found || y > *outY || (y == *outY && x < *outX)))
			{
				*outX = x;
				*outY = y;
				found = true;
			}
	return found;
}

// The standing spot nearest the given side's edge (at least 3 tiles in, so
// Keen doesn't leave straight away) whose space runs out to the edge.
static bool KW_FindSideExit(int dir, int nearY, int *outX, int *outY)
{
	int w = CA_GetMapWidth(), h = CA_GetMapHeight();
	int start = dir < 0 ? 5 : w - 7, stop = dir < 0 ? w / 2 : w / 2;
	for (int x = start; dir < 0 ? x <= stop : x >= stop; x -= dir)
	{
		int best = -1;
		for (int y = 1; y < h - 1; ++y)
			if (KW_Standable(x, y) && KW_ReachesEdge(dir < 0 ? x : x + 1, y, dir) &&
				(best < 0 || abs(y - nearY) < abs(best - nearY)))
				best = y;
		if (best >= 0)
		{
			*outX = x;
			*outY = best;
			return true;
		}
	}
	// Something in the way (a pillar, a closed passage): stand as near as
	// possible to the opening in the map's edge column.
	int edge = dir < 0 ? 1 : w - 2, bestDist = 1 << 30;
	bool found = false;
	for (int y = 2; y < h - 1; ++y)
	{
		int sx, sy;
		if (KW_Solid(edge, y) || KW_Solid(edge, y - 1) || !KW_FindSpotNear(edge - dir * 2, y, &sx, &sy))
			continue;
		int dist = abs(sx - edge) + abs(sy - y);
		if (dist < bestDist && abs(sx - edge) < w / 2)
		{
			bestDist = dist;
			*outX = sx;
			*outY = sy;
			found = true;
		}
	}
	return found;
}

// Keen 5's fuses: a two-tile machine whose lower tile has this top flag
// (see CK_KeenBreakFuse). Broken, they look like the map's tiles at (0,0)
// and (0,1). Returns how many there were; the last is in *outX, *outY.
#define KW_FUSE_TOP 0x39
static int KW_BreakFuses(int *outX, int *outY)
{
	int fuses = 0;
#ifdef WITH_KEEN5
	if (ck_currentEpisode->ep != EP_CK5 || AR_IsActive()) // Keen 5's own fuse tiles
		return 0;
	for (int y = 1; y < CA_GetMapHeight(); ++y)
		for (int x = 0; x < CA_GetMapWidth(); ++x)
			if (TI_ForeTop(CA_TileAtPos(x, y, 1)) == KW_FUSE_TOP)
			{
				CA_SetTileAtPos(x, y - 1, 1, CA_TileAtPos(0, 0, 1));
				CA_SetTileAtPos(x, y, 1, CA_TileAtPos(0, 1, 1));
				*outX = x;
				*outY = y;
				++fuses;
			}
	// The QED level: where the QED (info 41, see CK5_ScanInfoLayer) stood.
	if (!fuses && ca_mapOn == 12)
		for (int y = 1; y < CA_GetMapHeight(); ++y)
			for (int x = 0; x < CA_GetMapWidth(); ++x)
				if (CA_TileAtPos(x, y, 2) == 41)
				{
					*outX = x;
					*outY = y;
					++fuses;
				}
#else
	(void)outX;
	(void)outY;
#endif
	return fuses;
}

static KW_RevisitPlacement kw_revisitPlacement;
static int kw_goalX = -1, kw_goalY;
// The level's own start, where Keen first came in, and the direction he came
// from: walking back out that way there leaves a completed level.
static int kw_entryX = -1, kw_entryY, kw_entryFrom;

KW_RevisitPlacement KW_LastRevisitPlacement(void)
{
	return kw_revisitPlacement;
}

static void KW_PutKeen(int tileX, int tileY, int facing)
{
	ck_keenObj->posX = RF_TileToUnit(tileX);
	ck_keenObj->posY = RF_TileToUnit(tileY) - 241; // as CK_SpawnKeen
	ck_keenObj->xDirection = facing;
	CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenStanding));
	CK_ResetClipRects(ck_keenObj);
}

static void KW_PlaceRevisitStart(void)
{
	for (int i = 0; i < (int)(sizeof(kw_revisitStarts) / sizeof(kw_revisitStarts[0])); ++i)
		if (kw_revisitStarts[i].episode == KW_CurrentEpisode() && kw_revisitStarts[i].level == ca_mapOn)
		{
			KW_PutKeen(kw_revisitStarts[i].tileX, kw_revisitStarts[i].tileY, IN_motion_Right);
			kw_revisitPlacement = KW_REVISIT_TABLE;
			return;
		}

	int w = CA_GetMapWidth();
	int startX = RF_UnitToTile(ck_keenObj->posX), startY = ck_keenObj->clipRects.tileY2;
	int x, y, fx, fy;
	kw_revisitPlacement = KW_REVISIT_KEPT;
	kw_entryX = startX;
	kw_entryY = startY;
	kw_entryFrom = ck_keenObj->xDirection == IN_motion_Right ? IN_motion_Left : IN_motion_Right;
	// The Megarocket level keeps its own start beside the ship (an arrival
	// by ship is placed there too), and leaves only by its normal exits:
	// walking back out by the start would end it as soon as Keen arrives.
	if (KW_IsInShipLevel())
	{
		kw_entryX = -1;
		kw_revisitPlacement = KW_REVISIT_SPECIAL;
		return;
	}
	if (SS_RevisitStart())
	{
		kw_revisitPlacement = KW_REVISIT_GOAL;
		return;
	}
	// Swimming (Keen 4's Well of Wishes) and the high score "level" keep
	// their own start.
	if (ck_keenObj->currentAction != CK_ACTION(CK_ACT_keenStanding))
	{
		kw_revisitPlacement = KW_REVISIT_SPECIAL;
		return;
	}
	if (KW_FindExitDoor(&x, &y))
	{
		// The door's marker is one row above Keen's feet as he stands at it.
		KW_PutKeen(x, y + 1, x < w / 2 ? IN_motion_Right : IN_motion_Left);
		kw_revisitPlacement = KW_REVISIT_DOOR;
		return;
	}
	bool goal = KW_BreakFuses(&fx, &fy);
	if (!goal && kw_goalX >= 0)
	{
		fx = kw_goalX;
		fy = kw_goalY;
		goal = true;
	}
	if (goal && KW_FindSpotNear(fx, fy, &x, &y))
	{
		// He leaves the way he first came in, so he faces that way.
		KW_PutKeen(x, y, startX > x ? IN_motion_Right : IN_motion_Left);
		kw_revisitPlacement = KW_REVISIT_GOAL;
		return;
	}
	// The side opposite his normal start: of the exits at both ends, the one
	// farther from it (some levels start mid-map).
	int lx, ly, rx, ry;
	bool left = KW_FindSideExit(-1, startY, &lx, &ly), right = KW_FindSideExit(1, startY, &rx, &ry);
	if (left && right)
	{
		if (abs(lx - startX) > abs(rx - startX))
			right = false;
		else
			left = false;
	}
	if (left || right)
	{
		x = left ? lx : rx;
		KW_PutKeen(x, left ? ly : ry, left ? IN_motion_Right : IN_motion_Left);
		// A level whose only way out is where it starts: he comes back in there.
		kw_revisitPlacement = abs(x - startX) >= w / 3 ? KW_REVISIT_SIDE : KW_REVISIT_ONLY_EXIT;
	}
	// No exit found: keep the level's own start.
}

// ---- Fully open ----
// A completed level is completely walkable: gem doors start open (as if
// their gems had been placed), Keen 5's keycard doors need no card, and
// deadly tiles (spikes, fire) don't hurt (see KW_IsDeadlyTileHarmless).

static void KW_OpenGemDoors(void)
{
	int w = CA_GetMapWidth(), h = CA_GetMapHeight();
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x)
		{
			uint16_t holder = CA_TileAtPos(x, y, 1);
			int misc = TI_ForeMisc(holder) & 0x7F;
			if (misc < MISCFLAG_GEMHOLDER0 || misc > MISCFLAG_GEMHOLDER3)
				continue;
			uint16_t target = CA_TileAtPos(x, y, 2);
			int doorX = target >> 8, doorY = target & 0xFF;
			if (doorX < 2 || doorY < 2 || doorX > w || doorY > h)
				continue;
			// As CK_KeenPlaceGem: the holder shows its gem, the door opens.
			uint16_t filled = holder + 18;
			CA_SetTileAtPos(x, y, 1, filled);
			int height = 0;
			uint16_t doorTile = CA_TileAtPos(doorX, doorY, 1);
			for (int dy = doorY; dy < h && CA_TileAtPos(doorX, dy, 1) == doorTile; ++dy)
				++height;
			CK_object *door = CK_GetNewObj(false);
			door->posX = doorX;
			door->posY = doorY;
			door->user1 = height;
			door->clipped = CLIP_not;
			door->type = CT_Friendly;
			door->active = OBJ_ALWAYS_ACTIVE;
			CK_SetAction(door, CK_ACTION(CK_ACT_DoorOpen1));
		}
}

bool KW_IsDeadlyTileHarmless(void)
{
	return kw_levelIsPeaceful;
}

// ---- Clearing out enemies ----

static bool KW_IsHostile(CK_object *obj)
{
	if (AR_IsActive())
		return AR_IsHostileType(obj->type);
	if (FF_IsActive())
		return FF_IsHostileType(obj->type);
	switch (ck_currentEpisode->ep)
	{
#ifdef WITH_KEEN4
	case EP_CK4:
		switch (obj->type)
		{
		case CT4_Slug: case CT4_Egg: case CT4_Mushroom: case CT4_Arachnut:
		case CT4_Skypest: case CT4_Wormmouth: case CT4_Cloud: case CT4_Bolt:
		case CT4_Berkeloid: case CT4_Bounder: case CT4_Inchworm: case CT4_Lick:
		case CT4_Mimrock: case CT4_Dopefish: case CT4_Schoolfish: case CT4_Sprite:
		case CT4_Smirky: case CT4_Bird: case CT4_EnemyShot: case CT4_Mine:
		case CT4_StunnedCreature:
			return true;
		default:
			// TODO: dart guns share type 1 with harmless objects (e.g. the
			// Miragia controller); recognise them by action when a peaceful
			// level contains one.
			return false;
		}
#endif
#ifdef WITH_KEEN5
	case EP_CK5:
		switch (obj->type)
		{
		case CT5_EnemyShot: case CT5_StunnedCreature: case CT5_Sparky: case CT5_Mine:
		case CT5_SliceStar: case CT5_Robo: case CT5_Spirogrip: case CT5_Ampton:
		case CT5_Turret: case CT5_Volte: case CT5_0x11: case CT5_Spindred:
		case CT5_Master: case CT5_Shikadi: case CT5_Shocksund: case CT5_Sphereful:
		case CT5_Korath:
			return true;
		default:
			return false;
		}
#endif
#ifdef WITH_KEEN6
	case EP_CK6:
		switch (obj->type)
		{
		case CT6_EnemyShot: case CT6_StunnedCreature: case CT6_Bloog: case CT6_Blooglet:
		case CT6_Fleex: case CT6_Babobba: case CT6_Bobba: case CT6_Nospike: case CT6_Gik:
		case CT6_Turret: case CT6_Orbatrix: case CT6_Bip: case CT6_Flect: case CT6_Blorb:
		case CT6_Ceilick: case CT6_Bloogguard: case CT6_Bipship: case CT6_Grabbiter:
			return true;
		default:
			return false;
		}
#endif
	default:
		return false;
	}
}

// Characters and things whose encounter ends a level or the game: meeting
// them again in a cleared level would replay the story (Keen 5's QED, Keen
// 6's Molly and the rope, sandwich and passcard Keen already has).
static bool KW_IsStoryTrigger(CK_object *obj)
{
	if (SS_IsActive())
		return SS_IsStoryTrigger(obj);
	if (AR_IsActive())
		return AR_IsStoryTrigger(obj);
	if (FF_IsActive())
		return FF_IsStoryTrigger(obj);
#ifdef WITH_KEEN5
	if (ck_currentEpisode->ep == EP_CK5 && obj->type == CT5_QED)
		return true;
#endif
#ifdef WITH_KEEN6
	if (ck_currentEpisode->ep == EP_CK6 && (obj->type == CT6_Molly || obj->type == CT6_Rope ||
												obj->type == CT6_Sandwich || obj->type == CT6_Passcard))
		return true;
#endif
	return false;
}

// What ended the level, if it wasn't an exit: the Oracle Council member
// Keen rescued (Keen 4, and Keen 7 on its engine) or a story trigger.
static bool KW_IsLevelGoal(CK_object *obj)
{
#ifdef WITH_KEEN4
	if (ck_currentEpisode->ep == EP_CK4 && obj->type == CT4_CouncilMember)
		return true;
#endif
	return KW_IsStoryTrigger(obj);
}

static void KW_SpawnResident(int index)
{
	const KW_Resident *r = &kw_residents[index];
#ifdef WITH_KEEN4
	if (r->kind == KW_ELDER)
	{
		// An Oracle Council elder, as in the rescue levels, but "friendly":
		// Keen's collision code only rescues CT4_CouncilMember objects.
		CK_object *obj = CK_GetNewObj(false);
		obj->type = CT_Friendly;
		obj->active = OBJ_ACTIVE;
		obj->zLayer = PRIORITIES - 4;
		obj->posX = RF_TileToUnit(r->tileX);
		obj->posY = RF_TileToUnit(r->tileY) - CK_INT(CK4_CouncilSpawnYOffset, 369);
		obj->xDirection = IN_motion_Right;
		obj->yDirection = IN_motion_Down;
		obj->user4 = KW_RESIDENT_TAG | index;
		CK_SetAction(obj, CK_ACTION(CK4_ACT_CouncilWalk0));
		CA_MARKLUMP(LUMP_ORACLEMEMBER);
	}
#endif
}

void KW_SetupLevelResidents(void)
{
	KW_ChangeLevel();

	// Every completed level is peaceful when Keen comes back (in every world).
	kw_levelIsPeaceful = !MARS_IsActive() && ca_mapOn > 0 && ca_mapOn < CK_LEVELS_TRACKED && ck_gameState.levelsDone[ca_mapOn];
	if (!kw_levelIsPeaceful)
		return;

	// Where the level ended, if at a goal (see KW_PlaceRevisitStart).
	kw_goalX = -1;
	for (CK_object *obj = ck_keenObj->next; obj; obj = obj->next)
		if (KW_IsLevelGoal(obj))
		{
			kw_goalX = RF_UnitToTile(obj->posX);
			kw_goalY = RF_UnitToTile(obj->posY) + 1;
			break;
		}
	// The enemies have moved out, and story moments don't replay...
	for (CK_object *obj = ck_keenObj, *next; obj; obj = next)
	{
		next = obj->next;
		if (KW_IsHostile(obj) || KW_IsStoryTrigger(obj))
			CK_RemoveObj(obj);
#ifdef WITH_KEEN4
		// ...and a rescued Oracle Council elder stays on as a villager: still
		// there, but touching him is no longer a rescue.
		else if (ck_currentEpisode->ep == EP_CK4 && obj->type == CT4_CouncilMember)
			obj->type = CT_Friendly;
#endif
	}

	// Keen walks back in from the opposite side, and everything is open.
	KW_PlaceRevisitStart();
	// (every world is saved: Keen is at the bar, facing the barmaid)
	if (kw_bragDue && KW_CurrentEpisode() == KW_KEEN4 && ca_mapOn == 1)
		KW_PutKeen(KW_TAVERN_BARMAID_X - 1, KW_TAVERN_ROOM_STAND, IN_motion_Right);
	KW_OpenGemDoors();

	// ...and the residents have moved in.
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
		if (kw_residents[i].episode == KW_CurrentEpisode() && kw_residents[i].level == ca_mapOn)
			KW_SpawnResident(i);
	CA_MarkAllLumps();
}

// ---- Talking ----

// The barmaid's picture for her dialogue cards (tools/kw_barmaid_portrait.py).
#include "kw_barmaid_portrait.h"
static const KW_Art kw_barmaidFaceArt = {
	0, 0, KW_BARMAIDFACE_TILES_W, KW_BARMAIDFACE_TILES_H, 1,
	&kw_barmaidfaceSlots[0][0][0], &kw_barmaidfaceTiles[0][0][0][0], -1, 0, NULL,
};

// A card with the resident's picture on the left and their text.
static void KW_ResidentCard(const KW_Resident *r, const char *text)
{
	US_CenterWindow(26, 8);
	if (r->kind == KW_ELDER)
		VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), CK_CHUNKNUM(PIC_ORACLE));
	else
		KW_DrawArt(&kw_barmaidFaceArt, US_GetWindowX(), US_GetWindowY());
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetWindowX(US_GetWindowX() + 0x30);
	US_SetPrintY(US_GetPrintY() + 2);
	US_CPrint(text);
	VH_UpdateScreen();
	IN_ClearKeysDown();
	if (!getenv("KW_AUTOFINALE"))
		IN_WaitButton();
	KW_WaitForButtonsUp();
}

// The resident's words over as many cards as they need (pages parted by \f),
// spoken if there is a clip.
static void KW_ResidentSays(const KW_Resident *r, const char *pages, const char *voice)
{
	if (voice)
		KW_Speak(voice, true);
	char page[256];
	while (*pages)
	{
		size_t n = strcspn(pages, "\f");
		if (n >= sizeof(page))
			n = sizeof(page) - 1;
		memcpy(page, pages, n);
		page[n] = 0;
		KW_ResidentCard(r, page);
		pages += n + (pages[n] == '\f');
	}
	// the cards are put away: no more of the clip
	if (voice)
		KW_StopVoice();
}

// The line Keen picked stands on a card of his own and he says it (clips
// KEENSAY_*.RAW, named after the line: FNV-1a of its text, as
// keen_says_file in tools/kw_keen4_voice.py names them).
static void KW_KeenSaysClip(const char *text, char *file, size_t size)
{
	uint32_t h = 0x811C9DC5u;
	for (const unsigned char *c = (const unsigned char *)text; *c; ++c)
		h = (h ^ *c) * 0x01000193u;
	snprintf(file, size, "KEENSAY_%08X.RAW", (unsigned)h);
}

static void KW_KeenSays(const char *text)
{
	char clip[32];
	uint16_t w, h;
	KW_KeenSaysClip(text, clip, sizeof(clip));
	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	CK_MeasureMultiline(text, &w, &h);
	US_SetPrintY(US_GetPrintY() + (US_GetWindowH() - h) / 2 - 4);
	US_CPrint(text);
	VH_UpdateScreen();
	KW_Speak(clip, true);
	IN_ClearKeysDown();
	if (!getenv("KW_AUTOFINALE"))
		IN_WaitButton();
	KW_WaitForButtonsUp();
	KW_StopVoice(); // the card is put away: no more of the clip
}

static int KW_NumChoices(const KW_Resident *r)
{
	int n = 0;
	while (n < KW_MAX_CHOICES && r->choices[n].keen)
		++n;
	return n;
}

// Keen's card: his picture on the right and what he can say, chosen with the
// same blinking box as the Megarocket's destination menu. Returns the chosen
// line, or -1 if the player backs out with Esc.
static int KW_ChooseLine(const char *const *lines, int n)
{
	if (!n)
		return -1;
	int lineY[KW_MAX_TALK_CHOICES];

	US_CenterWindow(26, n > 3 ? 10 : 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 4);
	for (int i = 0; i < n; ++i)
	{
		lineY[i] = US_GetPrintY() - 2;
		US_CPrint(lines[i]);
		US_SetPrintY(US_GetPrintY() + 4);
	}

	int sel = 0, x1 = US_GetWindowX() + 2, x2 = US_GetWindowX() + US_GetWindowW() - 2;
	if (getenv("KW_AUTOFINALE"))
	{
		// (the scripted test takes the last line offered, and says which)
		printf("KW_AUTOFINALE: Keen says \"%s\" (of %d)\n", lines[n - 1], n);
		VH_UpdateScreen();
		return n - 1;
	}
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
		{
			KW_WaitForButtonsUp();
			return -1;
		}
		if (ck_inputFrame.jump || ck_inputFrame.pogo || scan == IN_SC_Enter || scan == IN_SC_Space)
		{
			KW_WaitForButtonsUp();
			return sel;
		}
		if ((ck_inputFrame.yDirection == -1 || scan == IN_SC_UpArrow) && sel > 0)
			sel--;
		else if ((ck_inputFrame.yDirection == 1 || scan == IN_SC_DownArrow) && sel < n - 1)
			sel++;
		if (scan != IN_SC_None)
			IN_SetLastScan(IN_SC_None);
		while (ck_inputFrame.yDirection != 0)
		{
			IN_PumpEvents();
			IN_ReadControls(0, &ck_inputFrame);
		}
	}
}

static int KW_ChooseReply(const KW_Resident *r)
{
	const char *lines[KW_MAX_CHOICES];
	int n = KW_NumChoices(r);
	for (int i = 0; i < n; ++i)
		lines[i] = r->choices[i].keen;
	return KW_ChooseLine(lines, n);
}

// When every world is saved Keen is put at the barmaid's bar and her talk
// begins at another place: he tells her what he has done (KW_Finale in
// kw_world.c; the talk from KW_BARMAID_BRAG on in tools/kw_barmaid_voice.py).
void KW_BragAtTheBar(void)
{
	kw_bragDue = true;
}

// A longer talk: round and round until it ends or the player backs out.
// A line that belongs to a world is only offered if that world is
// installed; a place with nothing left to offer is passed over.
static void KW_ShowTalk(const KW_Resident *r)
{
	int at = kw_talkStart;
	kw_talkStart = 0;
	while (at >= 0)
	{
		const KW_TalkNode *node = &r->talk[at];
		const char *lines[KW_MAX_TALK_CHOICES];
		int which[KW_MAX_TALK_CHOICES];
		int n = 0, all = 0;
		while (all < KW_MAX_TALK_CHOICES && node->choices[all].keen)
		{
			if (node->choices[all].world < 0 || KW_IsWorldPresent(node->choices[all].world))
			{
				lines[n] = node->choices[all].keen;
				which[n++] = all;
			}
			++all;
		}
		if (!n)
		{
			if (!all)
				return;
			at = node->choices[0].next;
			continue;
		}
		KW_ResidentSays(r, node->says, node->voice);
		int choice = KW_ChooseLine(lines, n);
		if (choice < 0)
			return;
		choice = which[choice];
		KW_KeenSays(node->choices[choice].keen);
		if (node->choices[choice].reply)
			KW_ResidentSays(r, node->choices[choice].reply, node->choices[choice].replyVoice);
		at = node->choices[choice].next;
	}
}

static void KW_ShowConversation(const KW_Resident *r)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_ORACLE));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_KEENTALK1));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_KEENTALK2));
	CA_CacheMarks(0);
	KW_BuildArtTiles(&kw_barmaidFaceArt);

	VL_FixRefreshBuffer();
	KW_WaitForButtonsUp();

	if (r->talk)
		KW_ShowTalk(r);
	else
	{
		// The resident greets Keen, Keen picks what to say, the resident answers.
		// (the elder of Gnosticus IV speaks: tools/kw_keen4_voice.py)
		bool voiced = r->episode == KW_KEEN4 && r->kind == KW_ELDER;
		KW_ResidentSays(r, r->says, voiced ? "K4_ELDER.RAW" : NULL);
		int choice = KW_ChooseReply(r);
		if (choice >= 0)
			KW_KeenSays(r->choices[choice].keen);
		if (choice >= 0 && r->choices[choice].reply)
		{
			char clip[32];
			snprintf(clip, sizeof(clip), "K4_ELDER_%d.RAW", choice);
			KW_ResidentSays(r, r->choices[choice].reply, voiced ? clip : NULL);
		}
	}

	CA_DownLevel();
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
}

// One card's text (a page) against the card: 26x8 characters minus the
// 48-pixel picture.
static bool KW_PageFits(const char *text, uint16_t *w, uint16_t *h)
{
	CK_MeasureMultiline(text, w, h);
	return *w <= 26 * 8 - 0x30 - 8 && *h <= 8 * 8 - 12;
}

// For /KWTEST: every dialogue line must fit its card.
const char *KW_CheckDialogueFits(void)
{
	static char problem[96];
	// the barmaid's talk: her pages, Keen's lines, where it goes on, her clips
	int places = (int)(sizeof(kw_barmaidTalk) / sizeof(kw_barmaidTalk[0]));
	for (int at = 0; at < places; ++at)
	{
		const KW_TalkNode *node = &kw_barmaidTalk[at];
		const char *texts[1 + 2 * KW_MAX_TALK_CHOICES];
		const char *voices[1 + 2 * KW_MAX_TALK_CHOICES];
		int n = 0;
		voices[n] = node->voice;
		texts[n++] = node->says;
		for (int c = 0; c < KW_MAX_TALK_CHOICES && node->choices[c].keen; ++c)
		{
			if (node->choices[c].next >= places)
				return "the barmaid's talk goes on at a place that does not exist";
			// (Keen says the line he picks: its clip is named after it)
			static char keenClips[KW_MAX_TALK_CHOICES][32];
			KW_KeenSaysClip(node->choices[c].keen, keenClips[c], sizeof(keenClips[c]));
			voices[n] = keenClips[c];
			texts[n++] = node->choices[c].keen;
			if (node->choices[c].reply)
			{
				voices[n] = node->choices[c].replyVoice;
				texts[n++] = node->choices[c].reply;
			}
		}
		for (int t = 0; t < n; ++t)
		{
			char page[256];
			for (const char *p = texts[t]; *p;)
			{
				size_t len = strcspn(p, "\f");
				if (len >= sizeof(page))
					return "a page of the barmaid's talk is too long";
				memcpy(page, p, len);
				page[len] = 0;
				uint16_t w, h;
				if (!KW_PageFits(page, &w, &h))
				{
					snprintf(problem, sizeof(problem), "barmaid place %d line \"%.24s\" is %dx%d", at, page, w, h);
					return problem;
				}
				p += len + (p[len] == '\f');
			}
			if (voices[t])
			{
				if (!KW_ClipIsThere(voices[t]))
				{
					snprintf(problem, sizeof(problem), "the barmaid's clip %s is missing", voices[t]);
					return problem;
				}
			}
		}
	}
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
	{
		const KW_Resident *r = &kw_residents[i];
		if (r->talk)
			continue;
		const char *texts[1 + 2 * KW_MAX_CHOICES];
		int n = 0;
		texts[n++] = r->says;
		for (int c = 0; c < KW_NumChoices(r); ++c)
		{
			texts[n++] = r->choices[c].keen;
			texts[n++] = r->choices[c].reply;
			// what Keen says to the elder of Gnosticus IV has a clip
			if (r->episode == KW_KEEN4 && r->kind == KW_ELDER)
			{
				char clip[32];
				KW_KeenSaysClip(r->choices[c].keen, clip, sizeof(clip));
				if (!KW_ClipIsThere(clip))
				{
					snprintf(problem, sizeof(problem), "Keen's clip %s (\"%.20s\") is missing", clip, r->choices[c].keen);
					return problem;
				}
			}
		}
		for (int t = 0; t < n; ++t)
		{
			if (!texts[t])
				continue;
			uint16_t w, h;
			CK_MeasureMultiline(texts[t], &w, &h);
			// Card: 26x8 characters minus the 48-pixel picture.
			if (w > 26 * 8 - 0x30 - 8 || h > 8 * 8 - 12)
			{
				snprintf(problem, sizeof(problem), "resident %d line \"%.24s\" is %dx%d", i, texts[t], w, h);
				return problem;
			}
		}
	}
	return NULL;
}

void KW_CheckTalk(void)
{
	if (!kw_levelIsPeaceful || !ck_keenObj)
		return;
	// Every world is saved, and Keen has just been put at the bar: he
	// starts bragging without being asked.
	if (kw_bragDue && KW_CurrentEpisode() == KW_KEEN4 && ca_mapOn == 1 && ck_keenObj->topTI)
	{
		kw_bragDue = false;
		for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
			if (kw_residents[i].talk == kw_barmaidTalk)
			{
				kw_talkStart = KW_BARMAID_BRAG;
				KW_ShowConversation(&kw_residents[i]);
				if (getenv("KW_AUTOFINALE"))
				{
					printf("KW_AUTOFINALE: Keen is free, at tile %d,%d of level %d\n",
						ck_keenObj->clipRects.tileX1, ck_keenObj->clipRects.tileY2, ca_mapOn);
					Quit(NULL);
				}
				return;
			}
	}
	// Talk by pressing up while standing (not jumping, not on a pole).
	if (ck_inputFrame.yDirection != IN_motion_Up || !ck_keenObj->topTI)
		return;

	for (CK_object *npc = ck_keenObj->next; npc; npc = npc->next)
	{
		if (npc->type != CT_Friendly || (npc->user4 & KW_RESIDENT_TAG_MASK) != KW_RESIDENT_TAG ||
			(npc->user4 & 0xFF) >= KW_NUM_RESIDENTS)
			continue;
		if (ck_keenObj->clipRects.tileX2 + KW_TALK_RANGE < npc->clipRects.tileX1 ||
			ck_keenObj->clipRects.tileX1 - KW_TALK_RANGE > npc->clipRects.tileX2 ||
			ck_keenObj->clipRects.tileY2 < npc->clipRects.tileY1 ||
			ck_keenObj->clipRects.tileY1 > npc->clipRects.tileY2)
			continue;
		KW_ShowConversation(&kw_residents[npc->user4 & 0xFF]);
		return;
	}

	// Painted residents: Keen stands at their spot.
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
	{
		const KW_Resident *r = &kw_residents[i];
		if (r->kind != KW_PAINTED || r->episode != KW_CurrentEpisode() || r->level != ca_mapOn)
			continue;
		if (ck_keenObj->clipRects.tileX2 < r->tileX - KW_TALK_RANGE ||
			ck_keenObj->clipRects.tileX1 > r->tileX + KW_TALK_RANGE ||
			ck_keenObj->clipRects.tileY2 < r->tileY || ck_keenObj->clipRects.tileY1 > r->tileY)
			continue;
		KW_ShowConversation(r);
		return;
	}
}

// For /KWTEST: count hostile objects and residents in the loaded level.
void KW_CountLevelPeople(int *hostiles, int *residents, CK_object **firstResident)
{
	*hostiles = *residents = 0;
	*firstResident = NULL;
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
	{
		if (KW_IsHostile(obj))
			++*hostiles;
		else if (obj->type == CT_Friendly && (obj->user4 & KW_RESIDENT_TAG_MASK) == KW_RESIDENT_TAG)
		{
			if (!*residents)
				*firstResident = obj;
			++*residents;
		}
	}
}

// In a peaceful level Keen can also leave through either end: walking into
// the first or last tiles of the map ends the level, as walking off a side
// does (some levels are walled at the end Keen first came in by).
#define KW_EXIT_ZONE 3 // tiles from the left or right edge of the map

void KW_CheckPeacefulExit(void)
{
	if (!kw_levelIsPeaceful || !ck_keenObj || ck_gameState.levelState != LS_Playing)
		return;
#ifdef WITH_KEEN5
	// Keycard doors open without a card in a completed level.
	if (ck_currentEpisode->ep == EP_CK5)
		ck_gameState.ep.ck5.securityCard = 1;
#endif
	if ((ck_inputFrame.xDirection == IN_motion_Right &&
		 ck_keenObj->clipRects.tileX2 >= CA_GetMapWidth() - KW_EXIT_ZONE) ||
		(ck_inputFrame.xDirection == IN_motion_Left && ck_keenObj->clipRects.tileX1 < KW_EXIT_ZONE))
		ck_gameState.levelState = LS_LevelComplete;
	// ...and back out where he first came in (some levels start mid-map).
	if (kw_entryX >= 0 && ck_inputFrame.xDirection == kw_entryFrom &&
		abs(ck_keenObj->clipRects.tileX1 - kw_entryX) <= 1 && abs(ck_keenObj->clipRects.tileY2 - kw_entryY) <= 1)
		ck_gameState.levelState = LS_LevelComplete;
}

void KW_GetLevelEntry(int *tileX, int *tileY, int *fromDirection)
{
	*tileX = kw_entryX;
	*tileY = kw_entryY;
	*fromDirection = kw_entryFrom;
}

void KW_TavernDoors(int *streetX, int *streetY, int *roomX, int *roomY)
{
	*streetX = KW_TAVERN_DOOR_X;
	*streetY = KW_TAVERN_DOOR_Y + 1;
	*roomX = KW_TAVERN_ROOM_DOOR_X;
	*roomY = KW_TAVERN_ROOM_STAND;
}

int KW_TavernBarmaidX(void)
{
	return KW_TAVERN_BARMAID_X + 1;
}
