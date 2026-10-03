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

// ---- Residents ----
// Who lives where. Positions are the tile the resident stands on (as an
// info-plane spawn marker would give). Lines may use \n; keep them short, the
// dialogue box is narrow.
typedef struct KW_Resident
{
	int episode;
	int level;
	int tileX, tileY;
	const char *says;      // The resident's line.
	const char *keenSays;  // Keen's reply.
} KW_Resident;

static const KW_Resident kw_residents[] = {
	// Keen 4, Border Village: an elder of the Oracle Council has moved into
	// the first house (its door leads to the small room at the bottom left).
	{4, 1, 4, 48,
		"Welcome back, Keen!\nThe Border Village\nis safe again, thanks\nto you. Make yourself\nat home.",
		"Thanks! Nice place\nyou've got here."},
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

// ---- Art in the levels ----
// Keen Worlds art belongs to the village after it is freed: by default it
// only appears once the level is completed ("the villagers are back with
// their things"). A piece can instead be shown only before, or always.

#include "kw_portrait_art.h"
#include "kw_bar_art.h"
#include "kw_stall_art.h"

typedef enum KW_ArtWhen
{
	KW_ART_AFTER,  // Only once the level is completed (the default).
	KW_ART_BEFORE, // Only until the level is completed.
	KW_ART_ALWAYS,
} KW_ArtWhen;

// The creator's portrait (tools/kw_portrait_art.py): Border Village's
// basement hall, on the cave wall right of the big statue.
const KW_Art kw_portraitArt = {
	KW_PORTRAIT_ORIGIN_X, KW_PORTRAIT_ORIGIN_Y, KW_PORTRAIT_TILES_W, KW_PORTRAIT_TILES_H, 1,
	&kw_portraitSlots[0][0][0], &kw_portraitTiles[0][0][0][0], -1, 0, NULL,
};

// The bar and, on the same spot, the boarded-up stall it used to be
// (tools/kw_bar_art.py): Border Village's street, just inside the entrance.
const KW_Art kw_barArt = {
	KW_BAR_ORIGIN_X, KW_BAR_ORIGIN_Y, KW_BAR_TILES_W, KW_BAR_TILES_H, 1,
	&kw_barSlots[0][0][0], &kw_barTiles[0][0][0][0], -1, 0, NULL,
};
const KW_Art kw_stallArt = {
	KW_STALL_ORIGIN_X, KW_STALL_ORIGIN_Y, KW_STALL_TILES_W, KW_STALL_TILES_H, 1,
	&kw_stallSlots[0][0][0], &kw_stallTiles[0][0][0][0], -1, 0, NULL,
};

static const struct
{
	int episode, level;
	const KW_Art *art;
	KW_ArtWhen when;
} kw_levelArt[] = {
	{4, 1, &kw_portraitArt, KW_ART_AFTER},
	{4, 1, &kw_barArt, KW_ART_AFTER},
	{4, 1, &kw_stallArt, KW_ART_BEFORE},
};

static void KW_PlaceLevelArt(void)
{
	bool completed = ca_mapOn != 0 && ck_gameState.levelsDone[ca_mapOn];
	for (int i = 0; i < (int)(sizeof(kw_levelArt) / sizeof(kw_levelArt[0])); ++i)
	{
		if (kw_levelArt[i].episode != KW_CurrentEpisode() || kw_levelArt[i].level != ca_mapOn)
			continue;
		if ((kw_levelArt[i].when == KW_ART_AFTER && !completed) ||
			(kw_levelArt[i].when == KW_ART_BEFORE && completed))
			continue;
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
	if (r->episode == 4)
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
	KW_PlaceLevelArt();

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

	// The resident speaks (same layout as the council rescue message).
	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), CK_CHUNKNUM(PIC_ORACLE));
	US_SetPrintY(US_GetPrintY() + 2);
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetWindowX(US_GetWindowX() + 0x30);
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
