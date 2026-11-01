/*
Keen Worlds: Revival of Bliss (Aurora Bliss III, Dave216, 2024, v1.1), a
Keen 4 patch mod in two builds: as Keen and as Craig. Its CKPatch files
replace some of the program's routines with their own (mostly nwe74's and
K1n9_Duk3's); these are those routines in C, read from the patches
(kwtools/kw_revival.py names which action uses which; research/ocflore-and-
aurora-bliss.md in the workspace root has the audit). The rest of its
changes are settings in the Keen 4 code, off for every other world. Each
function names the patch address it was read from.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <string.h>

#include "ck_act.h"
#include "ck_def.h"
#include "ck_phys.h"
#include "id_ca.h"
#include "id_rf.h"
#include "id_ti.h"

// ---- Allain, the boss (the Dopefish's object, nwe74 and K1n9_Duk3)

// $3DA8: its health, and the most it had (for the bar)
static int rv_bossHealth, rv_bossMaxHealth, rv_bossDrawn = -1;

void RV_SetBossHealth(int health, int max)
{
	rv_bossHealth = health;
	if (max)
		rv_bossMaxHealth = max;
}

// $DA3F: in its level (CK4_BossBarLevel) the score box shows "BOSS:" and a
// bar of four tiles that empties with its health. buf gets the score box's
// characters (each 6 more than its small tile). Returns -1 outside that
// level, 1 when the bar must be drawn again, 0 when it hasn't changed.
int RV_BossBar(char *buf, size_t size)
{
	if (ca_mapOn != CK_INT(CK4_BossBarLevel, -1) || size < 10)
		return -1;
	if (rv_bossDrawn == rv_bossHealth)
		return 0;
	rv_bossDrawn = rv_bossHealth;
	static const char empty[10] = {6, 7, 8, 8, 9, 10, 10, 10, 10, 0}; // "BOSS:" and an empty bar
	memcpy(buf, empty, 10);
	if (rv_bossHealth > 0 && rv_bossMaxHealth > 0)
	{
		int fill = rv_bossHealth * 31 / rv_bossMaxHealth + 1; // (never 0)
		int at = 8;
		for (; fill >= 8; fill -= 8)
			buf[at--] = 0x12; // a full piece
		if (fill)
			buf[at] = (char)(fill + 0x0A); // a part
	}
	return 1;
}

// $123D0 (the Dopefish's contact): it kills Keen; each shot costs it health,
// and it flashes (its old eating action); with none left it is stunned (its
// old burping action), and when that is done the game is won.
void RV_BossCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
	{
		if (--a->user2 == 0)
		{
			a->clipped = CLIP_normal;
			CK_StunCreature(a, b, CK_ACTION(CK4_ACT_DopefishBurp0));
		}
		else
		{
			CK_ShotHit(b);
			CK_SetAction2(a, CK_ACTION(CK4_ACT_DopefishEat0));
		}
	}
	RV_SetBossHealth(a->user2, 0);
}

void CK4_DopefishMove(CK_object *obj);

// $12197: it swims as the Dopefish, and the bar follows its health.
void RV_BossMove(CK_object *obj)
{
	CK4_DopefishMove(obj);
	RV_SetBossHealth(obj->user2, 0);
}

void CK_Fall(CK_object *obj);

// $12451: the end of its fall: the game is won (and it falls on).
void RV_BossDefeated(CK_object *obj)
{
	ck_gameState.levelState = LS_RVBossDown;
	CK_Fall(obj);
}

// ---- the Treasure Eater opens gem doors (nwe74)

// $11A99 (its think): it hops after Keen, smaller hops near him.
void RV_SmirkyJump(CK_object *obj)
{
	obj->currentAction = CK_ACTION(CK4_ACT_SmirkyJump0);
	int dx = ck_keenObj->posX - obj->posX, vx = 0x20, dir = IN_motion_Right;
	if (dx < 0)
	{
		dx = -dx;
		vx = -vx;
		dir = IN_motion_Left;
	}
	int vy = -0x20;
	obj->xDirection = dir;
	if (dx < 0x300)
	{
		vx >>= 1;
		vy >>= 1;
	}
	obj->velX = vx;
	obj->velY = vy;
}

// $11CA4 (its contact): harmless; Keen's shots burst on it.
void RV_SmirkyCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
		CK_ShotHit(b);
}

void CK_KeenPlaceGem(CK_object *obj);

// $11D53, $11AD5 (where it is): a gem holder it passes opens its door, as
// Keen's gem would (it eats nothing any more).
static void RV_SmirkyCheckTiles(CK_object *obj)
{
	for (int y = obj->clipRects.tileY1; y <= obj->clipRects.tileY2; y++)
		for (int x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; x++)
		{
			int mf = TI_ForeMisc(CA_TileAtPos(x, y, 1)) & 0x7F;
			if (mf < MISCFLAG_GEMHOLDER0 || mf > MISCFLAG_GEMHOLDER0 + 3)
				continue;
			// (Keen's own code opens it, from where he would stand)
			CK_object probe = *obj;
			probe.clipRects.tileXmid = x;
			probe.clipRects.tileY2 = y;
			CK_KeenPlaceGem(&probe);
			return;
		}
}

// $11E2C (its draw): it turns at a wall only when it also hits a ceiling
// (K1n9_Duk3: it could get stuck at the top of slopes).
void RV_SmirkyDraw(CK_object *obj)
{
	RV_SmirkyCheckTiles(obj);
	if (obj->topTI)
		CK_SetAction2(obj, CK_ACTION(CK4_ACT_Smirky0));
	if ((obj->leftTI || obj->rightTI) && obj->bottomTI)
	{
		obj->xDirection = -obj->xDirection;
		obj->velX = 0;
	}
	if (obj->topTI)
		obj->velY = 0;
	RF_AddSpriteDraw(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

// ---- the Devil Feet (inchworms) and Muha (schoolfish), nwe74

// $10F9C: deadly, and a shot stuns (the foot's old action is the stunned one).
void RV_InchwormCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
		CK_StunCreature(a, b, CK_ACTION(CK4_ACT_Foot0));
}

// $10FC1: deadly; a shot stops it for a while (the foot's old puff).
void RV_SchoolfishCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	if (b->type == CT_Stunner)
	{
		a->velX = 0;
		CK_ShotHit(b);
		CK_SetAction2(a, CK_ACTION(CK4_ACT_FootPoof0));
	}
}

// $10FF5: while stopped it is harmless; another shot stops it again.
void RV_SchoolfishStunnedCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
		a->velX = 0;
		CK_ShotHit(b);
		CK_SetAction2(a, CK_ACTION(CK4_ACT_FootPoof0));
	}
}

// ---- Craig's shots (the Craig build)

// $E722: his thrown shots burst on any wall, floor or ceiling.
void RV_CraigShotDraw(CK_object *obj)
{
	if (obj->topTI || obj->bottomTI || obj->rightTI || obj->leftTI)
		CK_ShotHit(obj);
	RF_AddSpriteDraw(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

void RV_SetupFunctions(void)
{
	CK_ACT_AddColFunction("RV_BossCol", &RV_BossCol);
	CK_ACT_AddFunction("RV_BossMove", &RV_BossMove);
	CK_ACT_AddFunction("RV_BossDefeated", &RV_BossDefeated);
	CK_ACT_AddFunction("RV_SmirkyJump", &RV_SmirkyJump);
	CK_ACT_AddColFunction("RV_SmirkyCol", &RV_SmirkyCol);
	CK_ACT_AddFunction("RV_SmirkyDraw", &RV_SmirkyDraw);
	CK_ACT_AddColFunction("RV_InchwormCol", &RV_InchwormCol);
	CK_ACT_AddColFunction("RV_SchoolfishCol", &RV_SchoolfishCol);
	CK_ACT_AddColFunction("RV_SchoolfishStunnedCol", &RV_SchoolfishStunnedCol);
	CK_ACT_AddFunction("RV_CraigShotDraw", &RV_CraigShotDraw);
}
