/*
Keen Worlds: behaviour that "Battle of the Brains" (Keen 9, a Keen 5 mod by
Ceilick, patched by Levellass and Lemm) adds as new program code in its
CKPatch file. Its variable file (MOD.CK5) points actions at these functions;
nothing here runs in Keen 5 itself. Each function names the patch address it
was converted from. Licensed under the GNU GPL v2 or later, like Omnispeak.
*/

#include "id_ca.h"
#include "id_in.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "ck5_ep.h"

extern uint8_t ca_levelbit;
extern uint8_t ca_graphChunkNeeded[CA_MAX_GRAPH_CHUNKS];

// $6D1B: a friend's message where Keen 5 has its fuse message. Who speaks
// depends on the level; the window is Keen 5's (CK5_FuseMessage).
static void KW9_FriendMessage(void)
{
	int level = ck_gameState.currentLevel;
	int pic = level == 5 ? 0x2E : level == 11 ? 0x2C : level == 2 ? 0x2F : 0x2D;
	const char *text = level == 5 ? CK_STRING(kw9_str_lindsey) : level == 11 ? CK_STRING(kw9_str_barker) :
		level == 2 ? CK_STRING(kw9_str_spot) : CK_STRING(kw9_str_janitor);
	int pic2 = CK_CHUNKNUM(PIC_KEENTALK2);

	SD_WaitSoundDone();
	CA_UpLevel();
	ca_graphChunkNeeded[pic] |= ca_levelbit;
	ca_graphChunkNeeded[pic2] |= ca_levelbit;
	CA_CacheMarks(0);

	VL_FixRefreshBuffer();
	US_CenterWindow(0x1A, 8);
	US_SetWindowW(US_GetWindowW() - 0x30);
	VHB_DrawBitmap(US_GetWindowW() + US_GetWindowX(), US_GetWindowY(), pic);
	US_SetPrintY(US_GetPrintY() + 0xC);
	US_CPrint(text);
	VH_UpdateScreen();
	VL_DelayTics(30);
	IN_ClearKeysDown();
	IN_WaitButton();

	VHB_DrawBitmap(US_GetWindowW() + US_GetWindowX(), US_GetWindowY(), pic2);
	VH_UpdateScreen();
	VL_DelayTics(30);
	IN_ClearKeysDown();
	IN_WaitButton();
	CA_DownLevel();
}

// $CAA7 (think of the "level end" object a touched fuse leaves): in the
// Warpcore (level 15) the game is won; anywhere else a friend speaks and the
// level goes on.
void KW9_FuseTouched(CK_object *obj)
{
	if (ca_mapOn == 15)
	{
		ck_gameState.levelState = LS_DestroyedQED;
		return;
	}
	SD_PlaySound(3);
	KW9_FriendMessage();
	RF_ForceRefresh();
}

// $120AE (Keen 5's mine shrapnel collision, rewritten): it no longer kills
// Keen; touching a fuse sets it off, with the Warpcore's explosion only in
// level 15. Keen's first running frame and Mortimer's shots collide with this.
void KW9_ShrapCol(CK_object *o1, CK_object *o2)
{
	if (o2->type == CT_Stunner)
	{
		CK_ShotHit(o2);
		return;
	}
	if (o2->type != CT5_QED)
		return;
	if (ck_gameState.currentLevel == 15)
	{
		CK5_SpawnFuseExplosion(o2->clipRects.tileX1, o2->clipRects.tileY1);
		CK5_SpawnFuseExplosion(o2->clipRects.tileX2, o2->clipRects.tileY1);
		CK5_SpawnFuseExplosion(o2->clipRects.tileX1, o2->clipRects.tileY2);
		CK5_SpawnFuseExplosion(o2->clipRects.tileX2, o2->clipRects.tileY2);
		RF_ReplaceTileBlock(0, 0, 0x10, 0xB, 4, 2);
		RF_ReplaceTileBlock(4, 0, 0x10, 0xD, 4, 2);
	}
	CK5_SpawnLevelEnd();
	CK_RemoveObj(o2);
}

// $9E88 (the fuse/Warpcore object's own collision): enemy shots wear it
// down; when its health is gone it explodes and is cleared from the level
// (the 3x3 tiles come from the map's top row, as the patch has it), and the
// last one ends the level.
void KW9_WarpcoreCol(CK_object *qed, CK_object *other)
{
	if (other->type != CT5_EnemyShot)
		return;
	CK_SetAction2(other, CK_ACTION(CK5_ACT_BarkShotDie0));
	other->type = CT_Nothing;
	if (--qed->user2)
		return;

	CK5_SpawnFuseExplosion(qed->clipRects.tileX1, qed->clipRects.tileY1);
	CK5_SpawnFuseExplosion(qed->clipRects.tileX2, qed->clipRects.tileY1);
	CK5_SpawnFuseExplosion(qed->clipRects.tileX1, qed->clipRects.tileY2);
	CK5_SpawnFuseExplosion(qed->clipRects.tileX2, qed->clipRects.tileY2);
	uint16_t tiles[9];
	for (int i = 0; i < 9; ++i)
		tiles[i] = CA_TileAtPos(16 + i, 0, 0);
	RF_ReplaceTiles(tiles, 1, qed->clipRects.tileX1, qed->clipRects.tileY1, 3, 3);
	CK_RemoveObj(qed);
	if (--ck_gameState.ep.ck5.fusesRemaining == 0)
		CK5_SpawnLevelEnd();
}

// $11635 (the Ampton's collision, rewritten for the yeti): deadly to touch,
// and it takes three shots, flashing on the first two.
void KW9_YetiCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
	}
	else if (b->type == CT_Stunner)
	{
		if (--a->user2 <= -3)
		{
			SD_PlaySound(0x16);
			CK_StunCreature(a, b, CK_ACTION(CK5_ACT_AmptonStunned0));
		}
		else
		{
			a->user3 = 1;
			CK_ShotHit(b);
		}
	}
}

// $18614: a snowball knocks Keen over instead of killing him.
static void KW9_StunKeen(void)
{
	CK_action *stunned = CK_ACTION(KW9_ACT_KeenStunned);
	if (ck_keenObj->currentAction == CK_ACTION(CK_ACT_keenDie0) || ck_keenObj->currentAction == CK_ACTION(CK_ACT_keenDie1))
		return;
	if (!ck_godMode)
	{
		if (ck_keenObj->currentAction == stunned)
			return;
		CK_SetAction2(ck_keenObj, stunned);
		SD_PlaySound(0);
	}
	ck_keenObj->velY = 1;
	ck_keenObj->velX = 0;
}

// $12DC3 (the Master's ball collision, now the yeti's snowball)
void KW9_SnowballCol(CK_object *o1, CK_object *o2)
{
	if (o2->type == CT_Stunner)
	{
		CK_ShotHit(o2);
		CK_RemoveObj(o1);
	}
	else if (o2->type == CT_Player)
	{
		KW9_StunKeen();
		CK_RemoveObj(o1);
	}
}

// $6CEF: the mine explodes when shot
void KW9_MineCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
		CK_StunCreature(a, b, CK_ACTION(CK5_ACT_MineExplode0));
}

// $6E93: Korath's collision with a sound and its own stunned action
void KW9_KorathCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		if (b->currentAction->collide)
			CK_PhysPushX(b, a);
	}
	else if (b->type == CT_Stunner)
	{
		SD_PlaySound(0x21);
		CK_StunCreature(a, b, CK_ACTION(CK5_ACT_KorathStun0));
	}
}

// $137B2: the Sphereful can be stunned with one shot
void KW9_SpherefulCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	if (b->type == CT_Stunner)
	{
		a->velX = 0;
		a->velY = 0;
		a->clipped = (CK_ClipType)0x100; // as the patch writes it
		CK_StunCreature(a, b, CK_ACTION(KW9_ACT_SpherefulStunned));
	}
}

// $645C, $6498: the Korathoid, a walker that stops now and then to face Keen
void KW9_KorathoidWalk(CK_object *obj)
{
	if (US_RndT() < 0x10)
	{
		ck_nextX = 0;
		US_RndT();
		obj->xDirection = obj->posX > ck_keenObj->posX ? IN_motion_Left : IN_motion_Right;
		obj->currentAction = CK_ACTION(KW9_ACT_KorathoidStand0);
	}
}

void KW9_KorathoidCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player && b->currentAction->collide)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
		CK_StunCreature(a, b, CK_ACTION(KW9_ACT_KorathoidStunned));
}

// $10620: where Keen 5 spawns a standing platform, Keen 9 spawns a Korathoid
void KW9_SpawnKorathoid(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT5_Korath;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0x80;
	obj->xDirection = IN_motion_Left;
	obj->yDirection = IN_motion_Down;
	CK_SetAction(obj, CK_ACTION(KW9_ACT_Korathoid0));
}

void KW9_SetupFunctions(void)
{
	CK_ACT_AddFunction("KW9_FuseTouched", &KW9_FuseTouched);
	CK_ACT_AddFunction("KW9_KorathoidWalk", &KW9_KorathoidWalk);
	CK_ACT_AddColFunction("KW9_ShrapCol", &KW9_ShrapCol);
	CK_ACT_AddColFunction("KW9_WarpcoreCol", &KW9_WarpcoreCol);
	CK_ACT_AddColFunction("KW9_YetiCol", &KW9_YetiCol);
	CK_ACT_AddColFunction("KW9_SnowballCol", &KW9_SnowballCol);
	CK_ACT_AddColFunction("KW9_MineCol", &KW9_MineCol);
	CK_ACT_AddColFunction("KW9_KorathCol", &KW9_KorathCol);
	CK_ACT_AddColFunction("KW9_SpherefulCol", &KW9_SpherefulCol);
	CK_ACT_AddColFunction("KW9_KorathoidCol", &KW9_KorathoidCol);
}
