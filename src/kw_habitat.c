/*
Keen Worlds: habitable levels. Once Keen has completed a level, coming back
finds it peaceful: the enemies have gone and residents live there, who Keen
can talk to (stand next to one and press up).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdlib.h>

#include "id_ca.h"
#include "id_in.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_ep.h"
#include "ck_play.h"
#include "kw_art.h"
#include "kw_habitat.h"
#include "kw_world.h"

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

typedef struct KW_Resident
{
	int episode;
	int level;
	int tileX, tileY;
	const char *says;      // The resident's line.
	const char *keenSays;  // Keen's reply.
	KW_ResidentKind kind;
} KW_Resident;

static const KW_Resident kw_residents[] = {
	// Keen 4, Border Village: an elder of the Oracle Council has moved into
	// the first house (its door leads to the small room at the bottom left).
	{4, 1, 4, 48,
		"Welcome back, Keen!\nThe Border Village\nis safe again, thanks\nto you. Make yourself\nat home.",
		"Thanks! Nice place\nyou've got here.", KW_ELDER},
	// Keen 4, Border Village: the barmaid behind the bar in the tavern
	// (drawn and animated by tools/kw_barhouse.py).
	{4, 1, KW_TAVERN_BARMAID_X + 1, KW_TAVERN_BARMAID_Y,
		"Well, if it isn't the\nhero of the Shadowlands!\nThe first drink's on\nthe house, Keen.",
		"Thanks! I'll have a\nroot beer, please.", KW_PAINTED},
};
#define KW_NUM_RESIDENTS ((int)(sizeof(kw_residents) / sizeof(kw_residents[0])))

// How close (in tiles) Keen must be to talk.
#define KW_TALK_RANGE 1

// Resident objects carry their table index in user4 (saved with the game),
// tagged so they can be recognised among other objects.
#define KW_RESIDENT_TAG 0x4B00
#define KW_RESIDENT_TAG_MASK 0xFF00

static bool kw_levelIsPeaceful = false;

static int KW_CurrentEpisode(void)
{
	return ck_currentEpisode->ep - EP_CK4 + 4;
}

// A level becomes peaceful once completed, if anyone lives there.
static bool KW_LevelHasResidents(void)
{
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
		if (kw_residents[i].episode == KW_CurrentEpisode() && kw_residents[i].level == ca_mapOn)
			return true;
	return false;
}

bool KW_IsHabitableLevel(int level)
{
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
		if (kw_residents[i].episode == KW_CurrentEpisode() && kw_residents[i].level == level)
			return true;
	return false;
}

bool KW_IsLevelPeaceful(void)
{
	return kw_levelIsPeaceful;
}

// ---- Changes to the levels ----
// Keen Worlds changes belong to the village after it is freed: by default
// they only appear once the level is completed ("the villagers are back with
// their things"). A piece can instead be shown only before, or always.


// Border Village's tavern (tools/kw_barhouse.py): a two-storey stone house on
// the street, boarded up until the village is freed, and its bar room carved
// into the rock below: portrait, tables, and the bar with the barmaid.
// (kw_tavern_art.h is included above, with the residents.)
// The creator's portrait (tools/kw_portrait_art.py), in the tavern room.
#include "kw_portrait_art.h"

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

static const struct
{
	int episode, level;
	const KW_MapEdit *edits;
	int numEdits;
} kw_levelEdits[] = {
	{4, 1, kw_tavernEdits, (int)(sizeof(kw_tavernEdits) / sizeof(kw_tavernEdits[0]))},
};

static const struct
{
	int episode, level;
	const KW_Art *art;
	KW_When when;
} kw_levelArt[] = {
	{4, 1, &kw_tavernBoardsArt, KW_BEFORE},
	{4, 1, &kw_tavernBarArt, KW_AFTER},
	{4, 1, &kw_tavernFurnitureArt, KW_AFTER},
	{4, 1, &kw_portraitArt, KW_AFTER},
};

static bool KW_IsShown(KW_When when, bool completed)
{
	return when == KW_ALWAYS || (when == KW_AFTER) == completed;
}

static void KW_ChangeLevel(void)
{
	bool completed = ca_mapOn != 0 && ck_gameState.levelsDone[ca_mapOn];
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

// ---- Clearing out enemies ----

static bool KW_IsHostile(CK_object *obj)
{
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
	default:
		return false;
	}
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

	kw_levelIsPeaceful = ca_mapOn != 0 && ck_gameState.levelsDone[ca_mapOn] && KW_LevelHasResidents();
	if (!kw_levelIsPeaceful)
		return;

	// The enemies have moved out...
	for (CK_object *obj = ck_keenObj, *next; obj; obj = next)
	{
		next = obj->next;
		if (KW_IsHostile(obj))
			CK_RemoveObj(obj);
	}

	// ...and the residents have moved in.
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
		if (kw_residents[i].episode == KW_CurrentEpisode() && kw_residents[i].level == ca_mapOn)
			KW_SpawnResident(i);
	CA_MarkAllLumps();
}

// ---- Talking ----

static void KW_ShowConversation(const KW_Resident *r)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_ORACLE));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_KEENTALK1));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_KEENTALK2));
	CA_CacheMarks(0);

	VL_FixRefreshBuffer();

	// The resident speaks (same layout as the council rescue message; painted
	// residents have no portrait picture, so their text fills the box).
	US_CenterWindow(26, 8);
	if (r->kind == KW_ELDER)
	{
		VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), CK_CHUNKNUM(PIC_ORACLE));
		US_SetWindowW(US_GetWindowW() - 0x30);
		US_SetWindowX(US_GetWindowX() + 0x30);
	}
	US_SetPrintY(US_GetPrintY() + 2);
	US_CPrint(r->says);
	VH_UpdateScreen();
	IN_ClearKeysDown();
	IN_WaitButton();

	// Keen answers.
	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 12);
	US_CPrint(r->keenSays);
	VH_UpdateScreen();
	IN_ClearKeysDown();
	IN_WaitButton();

	CA_DownLevel();
	KW_WaitForButtonsUp();
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
}

void KW_CheckTalk(void)
{
	if (!kw_levelIsPeaceful || !ck_keenObj)
		return;
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

// In a peaceful level Keen can also leave through the right side: walking
// right into the wall at the map's right edge ends the level, as walking off
// the left side does.
#define KW_RIGHT_EXIT_ZONE 5 // tiles from the right edge of the map

void KW_CheckPeacefulExit(void)
{
	if (!kw_levelIsPeaceful || !ck_keenObj || ck_gameState.levelState != LS_Playing)
		return;
	if (ck_inputFrame.xDirection == IN_motion_Right && ck_keenObj->rightTI &&
		ck_keenObj->clipRects.tileX2 >= CA_GetMapWidth() - KW_RIGHT_EXIT_ZONE)
		ck_gameState.levelState = LS_LevelComplete;
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
