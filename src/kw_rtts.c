/* Return to the Shadowlands v1.8 (Ceilick; programming K1n9_Duk3, 2022).
 * Its game code is a source mod of the reconstructed Keen 4 source (GPL v2
 * or later); here it runs on Omnispeak's Keen 4 code. This file holds what
 * the mod changed or added: its four new creatures (K4_ACT4.C), the Treasure
 * Eater's rules, the scouts' words, the new spawn values (K4_LEVEL.C) and
 * the score taken back on dying (CK_GAME.C). Audit:
 * research/return-to-the-shadowlands.md in the workspace.
 * GPL-2.0-or-later.
 */
#include "kw_rtts.h"
#include "kw_world.h"
#include "kw_music.h"
#include "ck_def.h"
#include "ck_act.h"
#include "ck_game.h"
#include "ck_play.h"
#include "ck_phys.h"
#include "ck4_ep.h"
#include "id_ca.h"
#include "id_in.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_ti.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include <stdio.h>
#include <stdlib.h>

bool RT_IsActive(void) { return KW_CurrentWorld() == KW_RTTS; }

// Its object classes (K4_DEF.H): numbered past Keen 4's own.
enum { CT_RTBadoing = 40, CT_RTWitchDoctor, CT_RTJanitor, CT_RTStingaru };

void CK_BasicDrawFunc1(CK_object *obj);
void CK_BasicDrawFunc4(CK_object *obj);
void CK4_ShowJanitorMessage(void);
void CK_SpawnScore(void);

#define RT_TICS (SD_GetSpriteSync())
#define RT_SPR(name) CK_CHUNKNUM(name)

// ---- Badoing: hides in the ground, pops up near Keen, stares, sinks back ----

enum { BD_WAIT, BD_RISE1, BD_RISE2, BD_RISE3, BD_RISE4, BD_TURN, BD_LOOK, BD_VANISH, BD_COOLDOWN };

static void RT_SpawnBadoing(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_RTBadoing;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = PRIORITIES - 4;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->gfxChunk = RT_SPR(SPR_BADOINGHIDDEN);
	CK_SetAction(obj, CK_ACTION(RT_ACT_Badoing));
}

static void RT_BadoingThink(CK_object *obj)
{
	obj->actionTimer += RT_TICS;
	switch (obj->user1)
	{
	case BD_WAIT:
		if (abs(ck_keenObj->posX - obj->posX) < 3 * 256 + 8 * 16 &&
			ck_keenObj->clipRects.unitY2 < obj->clipRects.unitY2 + 256 &&
			ck_keenObj->clipRects.unitY2 > obj->clipRects.unitY2 - 40 * 16)
		{
			SD_PlaySound(CK_SOUNDNUM(SOUND_RTBADOINGPOPUP));
			obj->gfxChunk = RT_SPR(SPR_BADOINGRISE1);
			obj->user1++;
			obj->actionTimer = 0;
		}
		break;
	case BD_RISE1:
	case BD_RISE2:
	case BD_RISE3:
		if (obj->actionTimer >= 10)
		{
			obj->gfxChunk++;
			obj->user1++;
			obj->actionTimer = 0;
		}
		break;
	case BD_RISE4:
		if (obj->actionTimer >= 35) // half a second
		{
			obj->gfxChunk = ck_keenObj->posX < obj->posX ? RT_SPR(SPR_BADOINGTURNL) : RT_SPR(SPR_BADOINGTURNR);
			obj->user1++;
			obj->actionTimer = 0;
		}
		break;
	case BD_TURN:
		if (obj->actionTimer >= 10)
		{
			obj->gfxChunk++;
			obj->user1++;
			obj->actionTimer = 0;
		}
		break;
	case BD_LOOK:
		if (obj->actionTimer >= 3 * 70) // three seconds
		{
			obj->gfxChunk--;
			obj->user1++;
			obj->actionTimer = 0;
			SD_PlaySound(CK_SOUNDNUM(SOUND_RTSHOWSTATUS));
		}
		break;
	case BD_VANISH:
		if (obj->actionTimer >= 10)
		{
			if (obj->gfxChunk == RT_SPR(SPR_BADOINGTURNR))
				obj->gfxChunk = RT_SPR(SPR_BADOINGRISE4);
			else
				obj->gfxChunk--;
			if (obj->gfxChunk < RT_SPR(SPR_BADOINGRISE1))
			{
				obj->gfxChunk = RT_SPR(SPR_BADOINGHIDDEN);
				obj->user1++;
			}
			obj->actionTimer = 0;
		}
		break;
	case BD_COOLDOWN:
		if (abs(ck_keenObj->posX - obj->posX) > 4 * 256)
			obj->user1 = BD_WAIT;
		break;
	}
}

static void RT_BadoingCol(CK_object *a, CK_object *b)
{
	if (a->gfxChunk == RT_SPR(SPR_BADOINGHIDDEN))
		return; // nothing while it hides
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
	{
		CK_StunCreature(a, b, CK_ACTION(RT_ACT_BadoingStunned));
		a->velY = -70;
	}
}

// ---- The witch doctor: walks, stops to look for Keen, then charges ----

enum { WD_STAND, WD_WALK, WD_RUN, WD_SEEK };

static void RT_SpawnWitchDoctor(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_RTWitchDoctor;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = PRIORITIES - 4;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY - 1);
	obj->xDirection = (US_RndT() & 2) - 1;
	obj->yDirection = IN_motion_Down;
	CK_SetAction(obj, CK_ACTION(RT_ACT_WitchDoctorJump));
}

static void RT_WitchDoctorThink(CK_object *obj)
{
	// (Its standing and jumping frames are its walking ones: "just for testing")
	const int stand = RT_SPR(SPR_WITCHDOCWALKL2);
	obj->actionTimer += RT_TICS;
	switch (obj->user1)
	{
	case WD_RUN:
		if (obj->actionTimer >= 5)
		{
			obj->actionTimer -= 5;
			// stop when no longer running towards Keen
			if ((obj->xDirection > 0 && obj->clipRects.unitXmid > ck_keenObj->clipRects.unitXmid + 3 * 256) ||
				(obj->xDirection < 0 && obj->clipRects.unitXmid < ck_keenObj->clipRects.unitXmid - 3 * 256))
			{
				obj->user2 = 2;
				obj->user1 = WD_SEEK;
				SD_PlaySound(CK_SOUNDNUM(SOUND_RTWITCHDOCSEARCH));
				obj->gfxChunk = stand + (++obj->user2 & 1);
				break;
			}
			ck_nextX = obj->xDirection * 8 * 16;
			obj->gfxChunk = RT_SPR(SPR_WITCHDOCWALKL1) + 1 + obj->xDirection + (++obj->user2 & 1);
		}
		break;
	case WD_WALK:
		if (obj->actionTimer >= 10)
		{
			obj->actionTimer -= 10;
			// randomly stop and look for Keen after at least twelve steps
			if (obj->user2 > 12 && US_RndT() <= 2)
			{
				obj->user2 = 0;
				obj->user1 = WD_SEEK;
				SD_PlaySound(CK_SOUNDNUM(SOUND_RTWITCHDOCSEARCH));
				obj->gfxChunk = stand + (++obj->user2 & 1);
				break;
			}
			ck_nextX = obj->xDirection * 8 * 16;
			obj->gfxChunk = RT_SPR(SPR_WITCHDOCWALKL1) + 1 + obj->xDirection + (++obj->user2 & 1);
		}
		break;
	case WD_SEEK:
	case WD_STAND:
		if (obj->actionTimer >= 10)
		{
			obj->actionTimer -= 10;
			if (obj->user2 >= 4)
			{
				obj->user2 = 0;
				if (obj->user1 != WD_STAND)
				{
					obj->xDirection = ck_keenObj->clipRects.unitXmid < obj->clipRects.unitXmid ? -1 : 1;
					// charge if Keen is about at the same ground level
					if (abs(obj->clipRects.unitY2 - ck_keenObj->clipRects.unitY2) < 2 * 256)
					{
						SD_PlaySound(CK_SOUNDNUM(SOUND_RTWITCHDOCCHARGE));
						obj->user1 = WD_RUN;
						obj->gfxChunk = stand + (++obj->user2 & 1);
						break;
					}
				}
				obj->user1 = WD_WALK;
			}
			obj->gfxChunk = stand + (++obj->user2 & 1);
		}
		break;
	}
}

static void RT_WitchDoctorCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
	{
		// shots don't stun it: it runs at Keen
		CK_ShotHit(b);
		a->xDirection = ck_keenObj->clipRects.unitXmid < a->clipRects.unitXmid ? -1 : 1;
		a->user1 = WD_RUN;
	}
}

static void RT_WitchDoctorDraw(CK_object *obj)
{
	if (obj->user1 == WD_RUN && !obj->topTI)
	{
		obj->velX = obj->xDirection * 48;
		obj->velY = -24; // jump up a bit
		CK_SetAction2(obj, CK_ACTION(RT_ACT_WitchDoctorJump));
	}
	else if ((obj->xDirection > 0 && obj->leftTI) || (obj->xDirection < 0 && obj->rightTI) || !obj->topTI)
	{
		obj->user1 = WD_STAND;
		obj->user2 = 0;
		obj->posX -= obj->deltaPosX;
		obj->posY -= obj->deltaPosY;
		obj->xDirection = -obj->xDirection;
		obj->gfxChunk += obj->xDirection * 2;
		CK_SetAction2(obj, obj->currentAction);
	}
	CK_BasicDrawFunc1(obj);
}

static void RT_WitchDoctorJumpDraw(CK_object *obj)
{
	if (obj->leftTI || obj->rightTI)
		obj->velX = 0;
	if (obj->bottomTI)
		obj->velY = 0;
	if (obj->topTI)
	{
		obj->user1 = WD_SEEK;
		obj->user2 = 0;
		obj->gfxChunk = RT_SPR(SPR_WITCHDOCWALKL2);
		CK_SetAction2(obj, CK_ACTION(RT_ACT_WitchDoctor));
	}
	CK_BasicDrawFunc1(obj);
}

// ---- The Oracle Janitor: walks about, and talks when Keen reaches him ----

static void RT_SpawnJanitor(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_RTJanitor;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = PRIORITIES - 4;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0x171;
	obj->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	obj->yDirection = IN_motion_Down;
	CK_SetAction(obj, CK_ACTION(RT_ACT_JanitorWalk0));
}

static void RT_JanitorWalk(CK_object *obj)
{
	if (RT_TICS * 8 > US_RndT())
	{
		ck_nextX = 0; // (RTTS: no step this time)
		obj->currentAction = CK_ACTION(RT_ACT_JanitorStand);
	}
}

static void RT_JanitorCol(CK_object *a, CK_object *b)
{
	// Keen touching him and standing on the ground (so not dying)
	if (b->type == CT_Player && b->topTI)
	{
		CK_RemoveObj(a);
		CK4_ShowJanitorMessage();
		ck_scoreBoxObj->user1 = ck_scoreBoxObj->user2 = ck_scoreBoxObj->user3 = ck_scoreBoxObj->user4 = -1;
		RF_ForceRefresh();
	}
}

// ---- The stingaru: walks, and leaps at Keen when he comes close ----

enum { ST_WALK1, ST_WALK2, ST_WALK3, ST_WALK4, ST_JUMP1, ST_JUMP2, ST_JUMP3 };

static void RT_SpawnStingaru(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT_RTStingaru;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = PRIORITIES - 4;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 15 * 16;
	obj->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	obj->yDirection = IN_motion_Down;
	CK_SetAction(obj, CK_ACTION(RT_ACT_StingaruJump));
}

static void RT_StingaruThink(CK_object *obj)
{
	obj->actionTimer += RT_TICS;
	switch (obj->user1)
	{
	case ST_WALK1:
	case ST_WALK2:
	case ST_WALK3:
	case ST_WALK4:
	{
		int xdist = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;
		int ydist = ck_keenObj->clipRects.unitY2 - obj->clipRects.unitY2;
		if (abs(ydist) <= 2 * 256 && abs(xdist) <= 4 * 256)
		{
			obj->xDirection = ck_keenObj->clipRects.unitXmid < obj->clipRects.unitXmid ? -1 : 1;
			obj->gfxChunk = RT_SPR(SPR_STINGARUJUMPL1) + 4 + obj->xDirection * 4;
			obj->user1 = ST_JUMP1;
			SD_PlaySound(CK_SOUNDNUM(SOUND_RTSTINGARUJUMP));
		}
		else if (obj->actionTimer >= 8)
		{
			obj->actionTimer -= 8;
			ck_nextX = obj->xDirection * 8 * 16;
			if (obj->user1 == ST_WALK4)
			{
				obj->user1 = ST_WALK1;
				obj->gfxChunk += RT_SPR(SPR_STINGARUWALKL1) - RT_SPR(SPR_STINGARUWALKL4);
			}
			else
			{
				obj->user1++;
				obj->gfxChunk++;
			}
		}
		break;
	}
	case ST_JUMP1:
	case ST_JUMP2:
		if (obj->actionTimer >= 12)
		{
			obj->actionTimer -= 12;
			obj->gfxChunk++;
			obj->user1++;
			if (obj->user1 == ST_JUMP3)
			{
				obj->velX = 32 * obj->xDirection;
				obj->velY = -32;
				obj->currentAction = CK_ACTION(RT_ACT_StingaruJump);
			}
		}
		break;
	}
}

static void RT_StingaruCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
	{
		CK_StunCreature(a, b, CK_ACTION(RT_ACT_StingaruStunned));
		a->velY -= 24;
	}
}

static void RT_StingaruDraw(CK_object *obj)
{
	if (!obj->topTI || (obj->xDirection < 0 && obj->rightTI) || (obj->xDirection > 0 && obj->leftTI))
	{
		obj->posX -= obj->deltaPosX;
		obj->posY -= obj->deltaPosY;
		obj->xDirection = -obj->xDirection;
		obj->gfxChunk += 8 * obj->xDirection;
		CK_SetAction2(obj, obj->currentAction);
	}
	CK_BasicDrawFunc1(obj);
}

static void RT_StingaruJumpDraw(CK_object *obj)
{
	if (obj->bottomTI)
		obj->velY = 0;
	if ((obj->rightTI && obj->velX < 0) || (obj->leftTI && obj->velX > 0))
		obj->velX = 0;
	if (obj->topTI)
	{
		obj->user1 = ST_WALK1;
		obj->gfxChunk = RT_SPR(SPR_STINGARUWALKL1) + 4 + obj->xDirection * 4;
		obj->velX = obj->velY = 0;
		CK_SetAction2(obj, CK_ACTION(RT_ACT_Stingaru));
	}
	CK_BasicDrawFunc1(obj);
}

// ---- The Treasure Eater, with RTTS's rules: it eats only bonus items that
// aren't key gems, aims for the nearest one, and turns on slopes ----

static bool RT_CanEat(CK_object *o)
{
	return o->type == CT4_Item && o->user1 >= 4;
}

static bool RT_IsTreasureTile(int x, int y)
{
	int mf = TI_ForeMisc(CA_TileAtPos(x, y, 1)) & 0x7F;
	return mf == MF_Centilife || (mf >= MF_Points100 && mf <= MF_Stunner);
}

static void RT_EaterSearch(CK_object *obj)
{
	obj->currentAction = CK_ACTION(CK4_ACT_SmirkyJump0);

	// jump straight up if under an item
	for (CK_object *o = ck_keenObj->next; o; o = o->next)
		if (RT_CanEat(o) && o->active == OBJ_ACTIVE &&
			o->clipRects.unitX2 > obj->clipRects.unitX1 && o->clipRects.unitX1 < obj->clipRects.unitX2 &&
			o->clipRects.unitY2 < obj->clipRects.unitY1 && o->clipRects.unitY2 + 3 * 256 > obj->clipRects.unitY1)
		{
			obj->velX = 0;
			obj->velY = -48;
			return;
		}

	// ... or under a treasure tile
	for (int y = obj->clipRects.tileY1 - 3; y < obj->clipRects.tileY1; y++)
		for (int x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; x++)
			if (y >= 0 && RT_IsTreasureTile(x, y))
			{
				obj->velX = 0;
				obj->velY = -48;
				return;
			}

	// vanish once both directions have been tried
	if (obj->user1 >= 2)
	{
		obj->currentAction = CK_ACTION(CK4_ACT_SmirkyTele0);
		return;
	}

	// hop on where there is floor ahead
	for (int i = 0; i < 4; i++)
		if (TI_ForeTop(CA_TileAtPos(obj->clipRects.tileXmid + obj->xDirection * 4, obj->clipRects.tileY2 - 2 + i, 1)))
		{
			obj->velX = obj->xDirection * 20;
			obj->velY = -24;
			return;
		}

	if (++obj->user1 == 2)
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_SMIRKYTELE));
		obj->currentAction = CK_ACTION(CK4_ACT_SmirkyTele0);
		return;
	}
	obj->xDirection = -obj->xDirection;
	obj->velX = obj->xDirection * 20;
	obj->velY = -24;
}

static void RT_EaterVanish(CK_object *obj)
{
	CK_object *nearest = NULL;
	unsigned best = 0;
	obj->user1 = 0;
	for (CK_object *o = ck_keenObj->next; o; o = o->next)
		if (RT_CanEat(o))
		{
			// Manhattan distance: straight lines before diagonals
			unsigned dist = ((unsigned)abs(o->posX - obj->posX) >> 1) + ((unsigned)abs(o->posY - obj->posY) >> 1);
			if (!nearest || dist < best)
			{
				nearest = o;
				best = dist;
			}
		}
	if (!nearest)
	{
		CK_RemoveObj(obj);
		return;
	}
	obj->posX = nearest->posX - 8 * 16;
	obj->posY = nearest->posY;
	CK_SetAction(obj, CK_ACTION(CK4_ACT_SmirkyTele4));
}

static void RT_EaterCol(CK_object *a, CK_object *b)
{
	if (RT_CanEat(b))
	{
		b->type = CT_Friendly;
		b->zLayer = PRIORITIES - 1;
		CK_SetAction2(b, CK_ACTION(CK4_ACT_StolenItem0));
		SD_PlaySound(CK_SOUNDNUM(SOUND_SMIRKYSTEAL));
	}
	else if (b->type == CT_Stunner)
	{
		a->user1 = a->user2 = a->user3 = 0;
		a->user4 = a->type;
		a->type = CT4_StunnedCreature;
		CK_ShotHit(b);
		CK_SetAction2(a, CK_ACTION(CK4_ACT_SmirkyStunned0));
		a->velY -= 16;
	}
}

static void RT_EaterDraw(CK_object *obj)
{
	for (int y = obj->clipRects.tileY1; y <= obj->clipRects.tileY2; y++)
		for (int x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; x++)
			if (RT_IsTreasureTile(x, y))
			{
				static uint16_t emptyTile = 0;
				RF_ReplaceTiles(&emptyTile, 1, x, y, 1, 1);
				CK_object *o = CK_GetNewObj(true);
				o->type = CT_Friendly;
				o->zLayer = PRIORITIES - 1;
				o->clipped = CLIP_not;
				o->posX = RF_TileToUnit(x);
				o->posY = RF_TileToUnit(y);
				o->active = OBJ_EXISTS_ONLY_ONSCREEN;
				CK_SetAction2(o, CK_ACTION(CK4_ACT_StolenItem0));
				SD_PlaySound(CK_SOUNDNUM(SOUND_SMIRKYSTEAL)); // (RTTS: with a sound)
				break;
			}

	// RTTS: also turn on a slope, and stop rising at a ceiling
	if (obj->leftTI || obj->rightTI || (uint16_t)obj->topTI > 1)
	{
		obj->user1++;
		obj->xDirection = -obj->xDirection;
		obj->velX = 0;
	}
	if (obj->topTI || obj->bottomTI)
		obj->velY = 0;
	if (obj->topTI)
		CK_SetAction2(obj, CK_ACTION(CK4_ACT_Smirky0));
	CK_BasicDrawFunc1(obj);
}

// ---- The scouts: each says his own line, in his level ----

void RT_ScoutDialog(void)
{
	int level = ca_mapOn;
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_ORACLE));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_KEENTALK1));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_KEENTALK2));
	CA_CacheMarks(0);
	StartMusic(-1);
	VL_FixRefreshBuffer();

	char clip[24];
	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), CK_CHUNKNUM(PIC_ORACLE));
	US_SetPrintY(US_GetPrintY() + 6);
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetWindowX(US_GetWindowX() + 0x30);
	US_CPrint(CK_STRINGELEMENT(rtts_str_scout, level));
	VH_UpdateScreen();
	snprintf(clip, sizeof(clip), "SCOUT%02d", level);
	KW_SayWorldLine(clip);
	IN_ClearKeysDown();
	IN_WaitButton();

	US_CenterWindow(26, 8);
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW() - 0x30, US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK1));
	US_SetWindowW(US_GetWindowW() - 0x30);
	US_SetPrintY(US_GetPrintY() + 12);
	US_CPrint(CK_STRINGELEMENT(rtts_str_keenScout, level));
	VH_UpdateScreen();
	snprintf(clip, sizeof(clip), "KEEN_SCOUT%02d", level);
	KW_SayWorldLine(clip);
	IN_ClearKeysDown();
	IN_WaitButton();
	VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW(), US_GetWindowY(), CK_CHUNKNUM(PIC_KEENTALK2));
	VH_UpdateScreen();
	IN_WaitButton();

	ck_gameState.ep.ck4.membersRescued++;
	KW_LinesOver();
	CA_DownLevel();
	StopMusic();
}

// ---- Dying takes back the score made in the level (RTTS's oldscore) ----

static int32_t rt_levelScore;

void RT_RememberScore(void)
{
	if (RT_IsActive())
		rt_levelScore = ck_gameState.keenScore;
}

void RT_RestoreScore(void)
{
	if (RT_IsActive() && rt_levelScore <= ck_gameState.keenScore)
		ck_gameState.keenScore = rt_levelScore;
}

// ---- Its new spawn values; the Miragia's (33) is now the Janitor's ----

bool RT_ScanObject(int info, int x, int y)
{
	if (!RT_IsActive())
		return false;
	int difficulty = ck_gameState.difficulty;
	switch (info)
	{
	case 33:
		RT_SpawnJanitor(x, y);
		CA_MARKLUMP(LUMP_JANITOR);
		return true;
	case 38:
	case 37:
	case 36:
		if ((info == 38 && difficulty < D_Hard) || (info == 37 && difficulty < D_Normal))
			return true;
		RT_SpawnBadoing(x, y);
		CA_MARKLUMP(LUMP_BADOING);
		return true;
	case 41:
	case 40:
	case 39:
		if ((info == 41 && difficulty < D_Hard) || (info == 40 && difficulty < D_Normal))
			return true;
		RT_SpawnWitchDoctor(x, y);
		CA_MARKLUMP(LUMP_WITCHDOC);
		return true;
	case 107:
	case 106:
	case 89:
		if ((info == 107 && difficulty < D_Hard) || (info == 106 && difficulty < D_Normal))
			return true;
		RT_SpawnStingaru(x, y);
		CA_MARKLUMP(LUMP_STINGARU);
		return true;
	}
	return false;
}

void RT_SetupFunctions(void)
{
	CK_ACT_AddFunction("RT_BadoingThink", &RT_BadoingThink);
	CK_ACT_AddColFunction("RT_BadoingCol", &RT_BadoingCol);
	CK_ACT_AddFunction("RT_WitchDoctorThink", &RT_WitchDoctorThink);
	CK_ACT_AddColFunction("RT_WitchDoctorCol", &RT_WitchDoctorCol);
	CK_ACT_AddFunction("RT_WitchDoctorDraw", &RT_WitchDoctorDraw);
	CK_ACT_AddFunction("RT_WitchDoctorJumpDraw", &RT_WitchDoctorJumpDraw);
	CK_ACT_AddFunction("RT_JanitorWalk", &RT_JanitorWalk);
	CK_ACT_AddColFunction("RT_JanitorCol", &RT_JanitorCol);
	CK_ACT_AddFunction("RT_StingaruThink", &RT_StingaruThink);
	CK_ACT_AddColFunction("RT_StingaruCol", &RT_StingaruCol);
	CK_ACT_AddFunction("RT_StingaruDraw", &RT_StingaruDraw);
	CK_ACT_AddFunction("RT_StingaruJumpDraw", &RT_StingaruJumpDraw);
	CK_ACT_AddFunction("RT_EaterSearch", &RT_EaterSearch);
	CK_ACT_AddFunction("RT_EaterVanish", &RT_EaterVanish);
	CK_ACT_AddColFunction("RT_EaterCol", &RT_EaterCol);
	CK_ACT_AddFunction("RT_EaterDraw", &RT_EaterDraw);
}

// ---- Self-test (/RTTSTEST) ----

void KW_TestLevelSpritesCached(const char *world, int level);
void KW_TestRunObjects(int frames);
#define RT_CHECK(test) do { if (!(test)) QuitF("Shadowlands test line %d: %s", __LINE__, #test); } while (0)

static int RT_Count(int type)
{
	int n = 0;
	for (CK_object *o = ck_keenObj->next; o; o = o->next)
		if (o->type == type || (o->type == CT4_StunnedCreature && o->user4 == type))
			++n;
	return n;
}

void RT_SelfTest(void)
{
	KW_SwitchWorld(KW_RTTS);
	CK_NewGame();
	ck_godMode = true;
	int seen[4] = {0};
	for (int difficulty = D_Easy; difficulty <= D_Hard; ++difficulty)
	{
		ck_gameState.difficulty = (CK_Difficulty)difficulty;
		for (int level = 0; level <= 20; ++level)
		{
			if (level == 19)
				continue; // the high score map
			printf("Shadowlands test: difficulty %d, map %d\n", difficulty, level);
			fflush(stdout);
			ck_gameState.currentLevel = level;
			ck_gameState.levelsDone[level] = 0;
			ck_gameState.levelState = LS_Playing;
			CK_LoadLevel(true, true);
			KW_TestLevelSpritesCached("rtts", level);
			for (int t = 0; t < 4; ++t)
				seen[t] += RT_Count(CT_RTBadoing + t);
			ck_gameState.levelState = LS_Playing;
			KW_TestRunObjects(120);
			RT_CHECK(ck_gameState.levelState == LS_Playing || ck_gameState.levelState == LS_LevelComplete ||
				ck_gameState.levelState == LS_CouncilRescued || ck_gameState.levelState == LS_Died);
		}
	}
	printf("Shadowlands test: badoings %d, witch doctors %d, janitors %d, stingarus %d\n", seen[0], seen[1], seen[2], seen[3]);
	RT_CHECK(seen[0] && seen[1] && seen[2] && seen[3]);
	puts("PASS: Shadowlands maps 0-18 and 20 load, cache and run on all three difficulties, with its new creatures");
	fflush(stdout);

	// Score made in a level is taken back on dying
	ck_gameState.keenScore = 1000;
	RT_RememberScore();
	ck_gameState.keenScore = 5000;
	RT_RestoreScore();
	RT_CHECK(ck_gameState.keenScore == 1000);
	puts("PASS: dying takes back the level's score");
	fflush(stdout);
}
