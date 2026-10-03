/*
Keen Worlds: habitable levels. Once Keen has completed a level, coming back
finds it peaceful: the enemies have gone and residents live there, who Keen
can talk to (stand next to one and press up).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdio.h>
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
#include "ck_phys.h"
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

// What Keen can say, and the resident's answer.
typedef struct KW_Choice
{
	const char *keen;
	const char *reply;
} KW_Choice;

#define KW_MAX_CHOICES 3

typedef struct KW_Resident
{
	int episode;
	int level;
	int tileX, tileY;
	const char *says;                     // The resident's greeting.
	KW_Choice choices[KW_MAX_CHOICES];    // Keen picks one (unused: NULL).
	KW_ResidentKind kind;
} KW_Resident;

static const KW_Resident kw_residents[] = {
	// Keen 4, Border Village: an elder of the Oracle Council has moved into
	// the first house (its door leads to the small room at the bottom left).
	{4, 1, 4, 48,
		"Welcome back, Keen!\nThe Border Village\nis safe again,\nthanks to you.",
		{
			{"How's the village?", "Peaceful at last.\nThe villagers are\nback in their homes."},
			{"What's the new inn?", "The barmaid opened\nit once the slugs\nleft. Say hello!"},
			{"Goodbye!", "Farewell, Keen.\nMay the Oracle\nguide you."},
		},
		KW_ELDER},
	// Keen 4, Border Village: the barmaid behind the bar in the tavern
	// (drawn and animated by tools/kw_barhouse.py).
	{4, 1, KW_TAVERN_BARMAID_X + 1, KW_TAVERN_BARMAID_Y,
		"Well, if it isn't the\nhero of the\nShadowlands! What\ncan I get you?",
		{
			{"A root beer, please.", "Coming right up!\nThe first one's on\nthe house, hon."},
			{"Who's in the picture?", "That's Hacker\nGriNoir. They say he\nbuilt this world!"},
			{"Just looking around.", "Make yourself at\nhome, Keen. Stay as\nlong as you like."},
		},
		KW_PAINTED},
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
	{4, 1, &kw_innArt, KW_ALWAYS},          // first: the boards go on top
	{4, 1, &kw_tavernBoardsArt, KW_BEFORE},
	{4, 1, &kw_signArt, KW_ALWAYS},
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

// ---- Coming back ----
// A peaceful level is entered from its left end, Keen facing right, so it
// looks like he walks back in (he can leave through the right end too).
static const struct
{
	int episode, level;
	int tileX, tileY; // as a Keen spawn marker: the row above the floor
} kw_revisitStarts[] = {
	{4, 1, 3, 16}, // Border Village: by the sign at the far left
};

static void KW_PlaceRevisitStart(void)
{
	for (int i = 0; i < (int)(sizeof(kw_revisitStarts) / sizeof(kw_revisitStarts[0])); ++i)
	{
		if (kw_revisitStarts[i].episode != KW_CurrentEpisode() || kw_revisitStarts[i].level != ca_mapOn)
			continue;
		ck_keenObj->posX = RF_TileToUnit(kw_revisitStarts[i].tileX);
		ck_keenObj->posY = RF_TileToUnit(kw_revisitStarts[i].tileY) - 241; // as CK_SpawnKeen
		ck_keenObj->xDirection = IN_motion_Right;
		CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenStanding));
		CK_ResetClipRects(ck_keenObj);
		return;
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

	// Keen walks back in from the left.
	KW_PlaceRevisitStart();

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
	IN_WaitButton();
	KW_WaitForButtonsUp();
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
static int KW_ChooseReply(const KW_Resident *r)
{
	int n = KW_NumChoices(r);
	if (!n)
		return -1;
	int lineY[KW_MAX_CHOICES];

	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 4);
	for (int i = 0; i < n; ++i)
	{
		lineY[i] = US_GetPrintY() - 2;
		US_CPrint(r->choices[i].keen);
		US_SetPrintY(US_GetPrintY() + 4);
	}

	int sel = 0, x1 = US_GetWindowX() + 2, x2 = US_GetWindowX() + US_GetWindowW() - 2;
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

	// The resident greets Keen, Keen picks what to say, the resident answers.
	KW_ResidentCard(r, r->says);
	int choice = KW_ChooseReply(r);
	if (choice >= 0 && r->choices[choice].reply)
		KW_ResidentCard(r, r->choices[choice].reply);

	CA_DownLevel();
	RF_ForceRefresh();
	SD_SetLastTimeCount(SD_GetTimeCount());
}

// For /KWTEST: every dialogue line must fit its card.
const char *KW_CheckDialogueFits(void)
{
	static char problem[96];
	for (int i = 0; i < KW_NUM_RESIDENTS; ++i)
	{
		const KW_Resident *r = &kw_residents[i];
		const char *texts[1 + 2 * KW_MAX_CHOICES];
		int n = 0;
		texts[n++] = r->says;
		for (int c = 0; c < KW_NumChoices(r); ++c)
		{
			texts[n++] = r->choices[c].keen;
			texts[n++] = r->choices[c].reply;
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

// In a peaceful level Keen can also leave through the right end (where he
// first came in): walking right into the last tiles of the map ends the
// level, as walking off the left side does.
#define KW_RIGHT_EXIT_ZONE 3 // tiles from the right edge of the map

void KW_CheckPeacefulExit(void)
{
	if (!kw_levelIsPeaceful || !ck_keenObj || ck_gameState.levelState != LS_Playing)
		return;
	if (ck_inputFrame.xDirection == IN_motion_Right &&
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
