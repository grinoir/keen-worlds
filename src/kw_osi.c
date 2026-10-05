/*
Keen Worlds: Operation Station Infiltration as a world (see kw_osi.h).
The functions here are from the mod's own changes to Omnispeak's shared
files (ck_keen.c, ck_game.c), which it marks MOD_OSI.
*/

#include <stdio.h>
#include <string.h>

#include "id_in.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_us.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_ep.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "kw_os5_ep.h"
#include "kw_osi.h"

bool OSI_IsActive(void)
{
	return ck_currentEpisode == &osi_episode;
}

const char *OSI_FunctionName(const char *name, char *buf, int size)
{
	if (!OSI_IsActive() || strncmp(name, "CK5_", 4))
		return name;
	snprintf(buf, size, "OS5_%s", name + 4);
	return buf;
}

// ---- Keen stunned (ck_keen.c)

int16_t ck_stunInvincibilityTimer;

void CK_StunKeen(CK_object *keen, CK_object *stunner)
{
	(void)stunner;
	if (ck_invincibilityTimer || ck_stunInvincibilityTimer)
		return;
	if (CK_INT(OSI_GodModeNoStun, 1) && ck_godMode)
		return;
	if (keen->currentAction == CK_GetActionByName("CK_ACT_keenDie0") || keen->currentAction == CK_GetActionByName("CK_ACT_keenDie1"))
		return;
	if (keen->currentAction == CK_GetActionByName("OSI_ACT_keenStunned0"))
		return;

	keen->user1 = 70;
	keen->velX = 0;
	/* If keen is on a pole, he needs to be clipped again. */
	keen->clipped = CLIP_normal;
	ck_stunInvincibilityTimer = 30;
	CK_SetAction2(keen, CK_GetActionByName("OSI_ACT_keenStunned0"));
}

void CK_KeenStunnedThink(CK_object *obj)
{
	CK_PhysGravityHigh(obj);
	CK_PhysDampHorz(obj);
	ck_nextX = obj->velX * SD_GetSpriteSync();
	ck_nextY = obj->velY * SD_GetSpriteSync();
	if (!obj->user1--)
	{
		ck_stunInvincibilityTimer = 30;
		if (!obj->topTI)
			CK_SetAction(obj, CK_GetActionByName("CK_ACT_keenFall1"));
		else
			CK_SetAction(obj, CK_GetActionByName("CK_ACT_keenStanding"));
	}
}

void CK_KeenStunnedDraw(CK_object *obj)
{
	if ((obj->topTI & 0xFFF8) == 8)
	{
		CK_KillKeen();
	}
	else if (obj->topTI == 0x29)
	{
		// Keen6 conveyor belt right
		ck_nextX = SD_GetSpriteSync() * 8;
		ck_nextY = 0;
		CK_PhysUpdateNormalObj(obj);
	}
	else if (obj->topTI == 0x31)
	{
		// Keen6 conveyor belt left
		ck_nextX = SD_GetSpriteSync() * -8;
		ck_nextY = 0;
		CK_PhysUpdateNormalObj(obj);
	}
	RF_AddSpriteDraw(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

// ---- What dying takes back (ck_game.c). The mod keeps these in the game
// state and its saved games; here they are kept beside it, and saved in the
// Keen Worlds block of a saved game (OSI_GetState).

static int32_t osi_oldScore, osi_oldKeenAt;
static int16_t osi_oldShots, osi_oldCentilife, osi_oldLives;

void OSI_RememberLevelStart(void)
{
	osi_oldScore = ck_gameState.keenScore;
	osi_oldKeenAt = ck_gameState.nextKeenAt;
	osi_oldShots = ck_gameState.numShots;
	osi_oldCentilife = ck_gameState.numCentilife;
	osi_oldLives = ck_gameState.numLives;
}

// For saved games (the Keen Worlds block): eight 16-bit words.
void OSI_GetState(int16_t out[8])
{
	out[0] = (int16_t)(osi_oldScore & 0xFFFF);
	out[1] = (int16_t)((osi_oldScore >> 16) & 0xFFFF);
	out[2] = (int16_t)(osi_oldKeenAt & 0xFFFF);
	out[3] = (int16_t)((osi_oldKeenAt >> 16) & 0xFFFF);
	out[4] = osi_oldShots;
	out[5] = osi_oldCentilife;
	out[6] = osi_oldLives;
	out[7] = 1; // (there is something here)
}

void OSI_SetState(const int16_t in[8])
{
	if (in[7] != 1)
	{
		OSI_RememberLevelStart(); // an older saved game: from where it was saved
		return;
	}
	osi_oldScore = (int32_t)((uint16_t)in[0] | ((uint32_t)(uint16_t)in[1] << 16));
	osi_oldKeenAt = (int32_t)((uint16_t)in[2] | ((uint32_t)(uint16_t)in[3] << 16));
	osi_oldShots = in[4];
	osi_oldCentilife = in[5];
	osi_oldLives = in[6];
}

void OSI_TakeBackOnDeath(void)
{
	if (!OSI_IsActive())
		return;
	if (CK_INT(OSI_ResetScoreOnDeath, 0))
		ck_gameState.keenScore = osi_oldScore;
	if (CK_INT(OSI_ResetNextKeenAtOnDeath, 0))
		ck_gameState.nextKeenAt = osi_oldKeenAt;
	if (CK_INT(OSI_ResetShotsOnDeath, 0))
		ck_gameState.numShots = osi_oldShots;
	if (CK_INT(OSI_ResetCentilifeOnDeath, 0))
		ck_gameState.numCentilife = osi_oldCentilife;
	if (CK_INT(OSI_ResetLivesOnDeath, 0))
		ck_gameState.numLives = osi_oldLives;
}

void OSI_SetupFunctions(void)
{
	OS5_SetupFunctions();
	CK_ACT_AddFunction("CK_KeenStunnedThink", &CK_KeenStunnedThink);
	CK_ACT_AddFunction("CK_KeenStunnedDraw", &CK_KeenStunnedDraw);
}

// Its variable files are written for the Omnispeak of its day, which kept a
// list as numbered variables (ck_str_levelName0, CK_LevelMusic0...) or in
// the program itself. The engine now reads lists as arrays: make them.
void CK_VAR_SetIntArray(const char *name, intptr_t *array, size_t arrayLen);
void CK_VAR_SetStringArray(const char *name, const char **array, size_t arrayLen);

static void OSI_NumberedStrings(const char *numbered, const char *array)
{
	const char *items[32];
	int n = 0;
	for (; n < 32; ++n)
	{
		char name[64];
		snprintf(name, sizeof(name), "%s%d", numbered, n);
		items[n] = CK_VAR_GetString(name, NULL);
		if (!items[n])
			break;
	}
	if (n)
		CK_VAR_SetStringArray(array, items, n);
}

static void OSI_NumberedInts(const char *numbered, const char *array)
{
	intptr_t items[32];
	int n = 0;
	for (; n < 32; ++n)
	{
		char name[64];
		snprintf(name, sizeof(name), "%s%d", numbered, n);
		items[n] = CK_VAR_GetInt(name, -12345);
		if (items[n] == -12345)
			break;
	}
	if (n)
		CK_VAR_SetIntArray(array, items, n);
}

void OSI_ChooseFiles(void)
{
	if (!OSI_IsActive())
		return;
	// Its tile properties are in its map header, not in the TILEINFO file
	// the engine would find (Keen 5's).
	CK_VAR_SetString("ca_tileInfo", "TILEINFO.OSI"); // (no such file)

	OSI_NumberedStrings("ck_str_levelName", "ck_str_levelNames");
	OSI_NumberedStrings("ck_str_levelEntryText", "ck_str_levelEntryText");
	OSI_NumberedInts("CK_LevelMusic", "CK_LevelMusic");
	// Names the engine has come to use since, for things that are numbered
	// as in Keen 5 in this game too.
	static const struct { const char *name; int value; } ints[] = {
		{"SPR_BONUS100", 195}, {"SPR_BONUS200", 196}, {"SPR_BONUS500", 197}, {"SPR_BONUS1000", 198},
		{"SPR_BONUS2000", 199}, {"SPR_BONUS5000", 200}, {"SPR_BONUS1UP", 201}, {"SPR_BONUSCLIP", 202},
		{"SPR_VIVASPLASH1", 203}, {"SPR_BONUSSECURITYCARD", 209}, {"SPR_BONUSGEM", 232},
		{"SPR_MAPKEEN_WALK1_W", 242}, {"SPR_MAPKEEN_WALK1_E", 245}, {"SPR_MAPKEEN_WALK1_SE", 254},
		{"SPR_MAPKEEN_WALK1_SW", 257}, {"SPR_MAPKEEN_WALK1_NW", 260}, {"SPR_MAPKEEN_WALK1_NE", 263},
	};
	for (int i = 0; i < (int)(sizeof(ints) / sizeof(ints[0])); ++i)
		if (CK_VAR_GetInt(ints[i].name, -12345) == -12345)
			CK_VAR_SetInt(ints[i].name, ints[i].value);
	static const char *const strings[][2] = {
		{"ck_str_extraVBLs", "  Add how many extra VBLs(0-8):"}, {"ck_str_levelWarp", "  Warp to which level(1-18):"},
		{"ck_str_paddleWarKeen", "KEEN:"}, {"ck_str_paddleWarComp", "COMP:"}, {"ck_str_paddleWarDefeat", "You lost!"},
		{"ck_str_paddleWarVictory", "You won!"}, {"ck_str_paddleWarPressAKey", "Press any key"},
	};
	for (int i = 0; i < (int)(sizeof(strings) / sizeof(strings[0])); ++i)
		if (!CK_VAR_GetString(strings[i][0], NULL))
			CK_VAR_SetString(strings[i][0], strings[i][1]);
	// (from its ck5_misc.c)
	static intptr_t itemSounds[] = {19, 19, 19, 19, 8, 8, 8, 8, 8, 8, 17, 9, 55};
	static intptr_t starWars[] = {0x00, 0x01, 0x18, 0x1E, 0x1F, 0x1C, 0x06, 0x07, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x00};
	static intptr_t terminator1[] = {0, 2, 2, 26, 1, 1, 1, 1, 0x11, 0x11, 0x11, 0x11, 0x13, 0x13, 0x13, 0x13, 0};
	static intptr_t terminator2[] = {0, 2, 2, 26, 1, 1, 1, 1, 0x11, 0x11, 0x11, 0x11, 0x13, 0x13, 0x13, 2, 0};
	CK_VAR_SetIntArray("ck_itemSounds", itemSounds, sizeof(itemSounds) / sizeof(itemSounds[0]));
	CK_VAR_SetIntArray("ck_starWarsPalette", starWars, sizeof(starWars) / sizeof(starWars[0]));
	CK_VAR_SetIntArray("ck_terminatorPalette1", terminator1, sizeof(terminator1) / sizeof(terminator1[0]));
	CK_VAR_SetIntArray("ck_terminatorPalette2", terminator2, sizeof(terminator2) / sizeof(terminator2[0]));
}
