/*
Keen Worlds: Atroxian Realm's creatures, ported from AR_ACT3.C of its source
(Atroxian Realm, Gridlock 2016, programming by Lemm; GPL v2 or later) to
today's Omnispeak. The actions are in ARACTION.CKD (tools/kw_arealm.py).
*/

#include "kw_arealm.h"

/* Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// AR_ACT3.C


/*
=============================================================================

LOCAL CONSTANTS

=============================================================================
*/


/*
=============================================================================

GLOBAL VARIABLES

=============================================================================
*/


/*
=============================================================================

LOCAL VARIABLES

=============================================================================
*/


//===========================================================================
//
// Atroxus (Dopefish replacement)
// 
//===========================================================================

// Description: 


// Floats around and chases keen; deadly to touch
// Ocassionally launches an axoclaw missile
// Can only be shot from bottom; if hit, flashes white, and drops a bomb
// The bomb explodes when it touches the ground, and sends shockwaves that travel horizontally outwards along
// the ground
















void AR_SpawnAtroxus(int tx, int ty);
void AtroxusMove(CK_object *obj);
void AR_AtroxusZoomAway (CK_object *obj);
void AR_AtroxusFireBomb (CK_object *obj);
void AtroxusExplode(CK_object *obj);
void AR_AtroxusPain(CK_object *obj);
void AtroxusExplodeTransition(CK_object *obj);
void AtroxusSpawnBomb(CK_object *obj);
void AtroxusSpawnMissile(CK_object *obj);
void AtroxusCollide(CK_object *obj1, CK_object *obj2);
void AtroxusPainCollide(CK_object *obj1, CK_object *obj2);
void AtroxusReact(CK_object *obj);
void AR_ABombBurn(CK_object *obj);
void AR_ABombReact(CK_object *obj);
void AR_SpawnShockwave(CK_object *obj, int direction);
void AR_ShockwaveMove(CK_object *obj);
void AR_ABombShockwaveReact(CK_object *obj);
void AR_SpawnAtroxusDebris(uint16_t posX, uint16_t posY, int health, bool flash);
void AR_SpawnAtroxusExplosion(CK_object *obj);
void AR_FireBombReact(CK_object *obj);
void AR_AtroxusExplodingReact(CK_object *obj);
void AR_GameWinTimer(CK_object *obj);



















#define ATROXUSMISSILETIME 70
#define ATROXUSINITIALPAINTIME 70

// user1 = health
// user2 = missile launch delay
// user3 = pain delay after being hit

void AR_SpawnAtroxus(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Atroxus;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->zLayer = 1;
	new_object->posX = tileX << 8;
	new_object->posY = tileY << 8; 
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	if (ck_gameState.difficulty == D_Easy)
		new_object->user1 = 45; 
	else if (ck_gameState.difficulty == D_Normal) 
		new_object->user1 = 60;
	else if (ck_gameState.difficulty == D_Hard) 
		new_object->user1 = 75; 
	new_object->user2 = ATROXUSMISSILETIME;
	new_object->user3 = ATROXUSINITIALPAINTIME;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AtroxusMove1));

}


void AR_DoAtroxusDamageEffects(CK_object *obj)
{
	int debris, explosion;

	debris = false;
	if ((obj->user1&0xFF) < 10 && (US_RndT() & 0xF) == 0xF)
		debris = true;
	else if ((obj->user1&0xFF) < 25 && (US_RndT() & 0x1F) == 0x1F)
		debris = true;
	else if ((obj->user1&0xFF) < 40 && (US_RndT() & 0x3F) == 0x3F)
		debris = true;

	if (debris)
	{
		// Spawn one atroxus bit as health decreases
		AR_SpawnAtroxusDebris(obj->posX + US_RndT()*4, obj->posY + 0x180 + US_RndT(), 100, true);
	}
		
	
	explosion = false;
	if ((obj->user1&0xFF) < 10 && (US_RndT() & 0xF) == 0xF)
		explosion = true;
	else if ((obj->user1&0xFF) < 25 && (US_RndT() & 0x1F) == 0x1F)
		explosion = true;
	else if ((obj->user1&0xFF) < 40 && (US_RndT() & 0x3F) == 0x3F)
		explosion = true;

	if (explosion)
	{
		// Spawn one atroxus bit as health decreases
		AR_SpawnAtroxusExplosion(obj);
	}
		
}


void AtroxusMove(CK_object *obj)
{

	obj->xDirection = (obj->clipRects.unitXmid>ck_keenObj->clipRects.unitXmid ? IN_motion_Left : IN_motion_Right);

	if ((obj->user2 -= SD_GetSpriteSync()) <= 0)
	{
		obj->user2 = 0;
		if (US_RndT() < 0x2)
		{
			CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxusSpawnMissile1));
			return;
		}
	}

	// Do second attack if almost dead
	if ((obj->user1 & 0xFF) < 20 && (ar_state.firebombresettimer-=SD_GetSpriteSync()) < 0)
	{
		int delta = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;
		obj->xDirection = (delta > 0)? 1 : -1;
		ar_state.firebombresettimer = 600;
		SD_PlaySound(ATROXUSCHARGESND);
		obj->currentAction = CK_ACTION(AR_ACT_AtroxusFirePrepare1);

	}


	AR_DoAtroxusDamageEffects(obj);

	CK_PhysAccelHorz(obj, obj->xDirection, (ck_gameState.difficulty == D_Easy)? 12 : 16);	// K1n9_Duk3 fix: 25% slower on easy
	if (obj->clipRects.unitY2 > ck_keenObj->posY)
		AR_PhysAccelVert(obj, -1, 8);
	else
		AR_PhysAccelVert(obj, 1, (ck_gameState.difficulty == D_Easy)? 6 : 8);	// K1n9_Duk3 fix: 25% slower on easy

}

// This happens if atroxus is hit with very little health remaining
void AR_AtroxusZoomAway (CK_object *obj)
{

	int deltaX;

	deltaX = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;

	if (obj->xDirection < 0)
	{
		if (deltaX > 0x800 || obj->clipRects.unitX1 < 0x400)
		{
			// transition to firebombing and move to the right
			obj->currentAction = CK_ACTION(AR_ACT_AtroxusFireBomb1);
			obj->xDirection = 1;
			ar_state.firebombcount = 5;
			ar_state.firebombtics = 50;
		}
		else
		{
			CK_PhysAccelHorz(obj, -1, 32);
		}

	}
	else
	{
		if (deltaX < -0x800 || obj->clipRects.unitX2 > (CA_GetMapWidth()<<G_T_SHIFT)-0x400)
		{

			// transition to firebombing and move to the left
			obj->currentAction = CK_ACTION(AR_ACT_AtroxusFireBomb1);
			obj->xDirection = -1;
			ar_state.firebombcount = 5;
			ar_state.firebombtics = 50;
		}
		else
		{
			CK_PhysAccelHorz(obj, 1, 32);
		}
	}

	if (obj->posY > 0x400)
		AR_PhysAccelVert(obj, -1, 8);
	else
		AR_PhysAccelVert(obj, 1, 8);

	AR_DoAtroxusDamageEffects(obj);
}


void AR_AtroxusFireBomb(CK_object *obj)
{

	int deltaX = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;

	if ((ar_state.firebombtics -= SD_GetSpriteSync()) < 0)
	{
		CK_object *newobj;

		// spawn a firebomb
		newobj = CK_GetNewObj(true);
		newobj->type = CT5_EnemyShot;
		newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
		newobj->posX = obj->clipRects.unitXmid;
		newobj->posY = obj->clipRects.unitY2;
		newobj->clipped = CLIP_normal;

		SD_PlaySound(ATROXUSBOMBDROPSND);
		CK_SetAction(newobj, CK_ACTION(AR_ACT_FireBombFall1));

		ar_state.firebombtics = 50;
	
		//if (!--ar_state.firebombcount)

		if ((obj->xDirection == 1 && (deltaX < -0x800 ||obj->clipRects.unitX2 > (CA_GetMapWidth()<<G_T_SHIFT)-0x400))|| (obj->xDirection == -1 && (deltaX > 0x800 || obj->clipRects.unitX1 < 0x400)))
		{
			obj->currentAction = CK_ACTION(AR_ACT_AtroxusMove1);
			return;
		}

	}

	if (obj->posY > 0x800)
		AR_PhysAccelVert(obj, -1, 8);
	else
		AR_PhysAccelVert(obj, 1, 8);

	CK_PhysAccelHorz(obj, obj->xDirection, 32);

	AR_DoAtroxusDamageEffects(obj);
}

void AR_AtroxusPain(CK_object *obj)
{

	if ((obj->user4 -=SD_GetSpriteSync()) < 0)
	{
		obj->user4 = 0;
		CK_SetAction2(obj, obj->currentAction->next);
		SD_PlaySound(ATROXUSCHARGESND);
	}

	// force React function to be called in order to
	// advance whiteflash ticker
	ck_nextY -=1;


}


void AtroxusExplode(CK_object *obj)
{

	obj->velX = 0;

	if (obj->currentAction == CK_ACTION(AR_ACT_AtroxusExploding2))
		CK_Fall(obj);
	else
		obj->velY = 0;


	// Spawn small sparky explosions
	if ((int)(obj->user4 -= SD_GetSpriteSync()) < 0)
	{
		CK_object *newobj = CK_GetNewObj(true);
		obj->user4 = 20;
		newobj->posX = obj->clipRects.unitX1 + ((obj->clipRects.unitX2 - obj->clipRects.unitX1) * US_RndT() >> 6);
		newobj->posY = obj->clipRects.unitY1 + ((obj->clipRects.unitY2 - obj->clipRects.unitY1) * US_RndT() >> 7);
		newobj->type = CT_Friendly;
		newobj->clipped = CLIP_not;
		newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
		newobj->zLayer = 2;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_SmallExplosion1));
		SD_PlaySound(ATROXIANGROUNDEDSND);
	}

	// Spawn Magmine Explosions
	if ((int)(obj->user3 -= SD_GetSpriteSync()) < 0)
	{
		CK_object *newobj;

		// Don't spawn big explosions when falling
		if (obj->currentAction == CK_ACTION(AR_ACT_AtroxusExploding2))
			return;

		newobj = CK_GetNewObj(true);
		if (obj->actionTimer > 460)
			obj->user3 = 30000; // XTREME HACK: just set the explosion timer large enough so that it
							  // doesn't trigger again before AR_ACT_AtroxusExploding2 starts
		else if (obj->actionTimer > 280)
			obj->user3 = 15;
		else
			obj->user3 = 30;
		newobj->posX = obj->clipRects.unitX1-0x200 + ((obj->clipRects.unitX2 - obj->clipRects.unitX1) * US_RndT() >> 5);
		newobj->posY = obj->clipRects.unitY1-0x100 + ((obj->clipRects.unitY2 - obj->clipRects.unitY1) * US_RndT() >> 6);
		newobj->type = CT_Friendly;
		newobj->clipped = CLIP_not;
		newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
		newobj->zLayer = 2;
		newobj->xDirection = IN_motion_None;
		newobj->yDirection = IN_motion_None;
		newobj->clipped = CLIP_not;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_MagmineExplosion1));
		SD_PlaySound(MAGMINEEXPLODESND);
	}

}

void AtroxusExplodeTransition(CK_object *obj)
{

	CK_object *newobj;

	// obj->posX += 0x200;
	// obj->posY += 0x100;
	//SD_PlaySound(MAGMINEEXPLODESND);


	newobj = CK_GetNewObj(true);
	newobj->type = CT_Friendly;
	newobj->active = OBJ_ALWAYS_ACTIVE;
	newobj->posX = obj->posX;
	newobj->posY = obj->posY;
	newobj->clipped = CLIP_not;
	newobj->user1 = 1;

	CK_SetAction(newobj, CK_ACTION(AR_ACT_GameWinTimer));

	
}



void AtroxusSpawnBomb(CK_object *obj)
{

	CK_object *newobj;

	newobj = CK_GetNewObj(true);
	newobj->type = CT5_EnemyShot;
	newobj->active = OBJ_ALWAYS_ACTIVE;
	newobj->posX = obj->clipRects.unitXmid;
	newobj->posY = obj->clipRects.unitY2;
	newobj->clipped = CLIP_normal;
	newobj->user1 = 210;

	// Don't spawn a fireball right after dropping a bomb
	obj->user2 = ATROXUSMISSILETIME;

	SD_PlaySound(ATROXUSBOMBDROPSND);
	CK_SetAction(newobj, CK_ACTION(AR_ACT_ABombFall1));
}


void AR_SpawnAtroxusMissile(uint16_t x, uint16_t y, int dir)
{

	// Spawn a missile
	CK_object *new_object = CK_GetNewObj(true);
	new_object->type = AR_CT_Axoclawshot;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	new_object->zLayer = 2;
	new_object->posX = x; 
	new_object->posY = y; 
	new_object->xDirection = dir;
	new_object->velY = 0;
	new_object->clipped = CLIP_normal;
	SD_PlaySound(AXOCLAWSHOOTSND);
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AtroxusShotMove1));


}

void AtroxusSpawnMissile(CK_object *obj)
{

	int dir = ck_keenObj->clipRects.unitXmid > obj->clipRects.unitXmid ? IN_motion_Right : IN_motion_Left;
	AR_SpawnAtroxusMissile(obj->clipRects.unitXmid, (obj->clipRects.unitY1+obj->clipRects.unitY2)/2, dir);
	obj->user2 = ATROXUSMISSILETIME;


}

#define FLASHTIME 12
void AtroxusCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	else if (obj2->type == CT_Stunner)
	{
		int dx = obj2->clipRects.unitXmid - obj1->clipRects.unitXmid;
		CK_ShotHit(obj2);
		
		if (obj2->yDirection == IN_motion_Up && dx > -0xC0 && dx < 0xC0)
		{
			int health = obj1->user1 & 0xFF;
			SD_PlaySound(ATROXIANGROUNDEDSND);
			AR_SpawnAtroxusDebris(obj2->posX, obj2->posY, obj1->user1 & 0xFF, 0);

			if (--health == 0)
			{
				CK_SetAction2(obj1, CK_ACTION(AR_ACT_AtroxusExploding1));
				obj1->user3 = 140;
				obj1->user4 = 20;
				return;
			}

			// set whiteflash
			obj1->user1 = ((FLASHTIME<<8) | health);

			// decrease pain length
			if ((obj1->user3 -= 3) < 25)
				obj1->user3 = 25;

			obj1->user4 = obj1->user3;

			// drop a bomb
#if 0
			if (health < 10)
			{
				if (obj1->currentAction >= CK_ACTION(AR_ACT_AtroxusFireBomb1) && obj1->currentAction <= CK_ACTION(AR_ACT_AtroxusFireBomb3))
					return;

				else
					obj1->currentAction = CK_ACTION(AR_ACT_AtroxusFirePre1);
			}
			else
#endif
			if (obj1->currentAction < CK_ACTION(AR_ACT_AtroxusFirePrepare1) || obj1->currentAction > CK_ACTION(AR_ACT_AtroxusFireBomb3))
			{
				CK_SetAction2(obj1, CK_ACTION(AR_ACT_AtroxusPain1));
			}

			return;
		}
	}
}

void AtroxusPainCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	else if (obj2->type == CT_Stunner)
	{
		int dx = obj2->clipRects.unitXmid - obj1->clipRects.unitXmid;
		CK_ShotHit(obj2);
		
		if (obj2->yDirection == IN_motion_Up && dx > -0xC0 && dx < 0xC0)
		{
			int health = obj1->user1 & 0xFF;
			SD_PlaySound(ATROXIANGROUNDEDSND);
			AR_SpawnAtroxusDebris(obj2->posX, obj2->posY, obj1->user1 & 0xFF, 0);

			if (--health == 0)
			{
				CK_SetAction2(obj1, CK_ACTION(AR_ACT_AtroxusExploding1));
				obj1->user3 = 140;
				obj1->user4 = 20;
				return;
			}

			// set whiteflash
			obj1->user1 = ((FLASHTIME<<8) | health);

			// decrease pain length
			if ((obj1->user3 -= 2) < 35)
				obj1->user3 = 35;

			return;
		}
	}
}

void AtroxusReact(CK_object *obj)
{

	if (obj->leftTI || obj->rightTI)
	{
		obj->velX = 0;
		obj->xDirection = -obj->xDirection;
	}


	AR_DrawHitFlash(obj, (uint16_t *)&(obj->user1), 0, 0);

}


void AR_ABombBurn(CK_object *obj)
{
	if ((int)(obj->user1 -= SD_GetSpriteSync()) < 0)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_ABombDie1));
	}
}

void AR_ABombReact(CK_object *obj)
{
	if (obj->topTI)
	{
		if (ck_gameState.difficulty != D_Easy)	// K1n9_Duk3 fix: no shockwave on easy
		{
			AR_SpawnShockwave(obj, IN_motion_Left);
			AR_SpawnShockwave(obj, IN_motion_Right);
		}
		CK_SetAction2(obj, CK_ACTION(AR_ACT_ABombBurn1));
		SD_PlaySound(ATROXUSBOMBEXPLODESND);
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}

void AR_SpawnShockwave(CK_object *obj, int direction)
{

	CK_object *newobj;

	newobj = CK_GetNewObj(true);
	newobj->type = CT5_EnemyShot;
	newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
	newobj->posX = obj->clipRects.unitXmid - 0x100;
	newobj->posY = obj->posY - 0x200;
	newobj->xDirection = direction;
	newobj->yDirection = IN_motion_None;
	newobj->clipped = CLIP_normal;

	CK_SetAction(newobj, CK_ACTION(AR_ACT_ABombShockwave1));
}

void AR_ShockwaveMove(CK_object *obj)
{
	int xmove;
	switch (ck_gameState.difficulty)
	{
		case D_Easy:
		case D_Normal:
			xmove = 72;
			break;
		case D_Hard:
			xmove = 96;
			break;
	}

	ck_nextX += obj->xDirection * xmove;
}

void AR_ABombShockwaveReact(CK_object *obj)
{
	if (!obj->topTI)
	{
		//CK_RemoveObj(obj);
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}	


void AR_SpawnAtroxusDebris (uint16_t posX, uint16_t posY, int health, bool drop)
{
	int i;

	if (health > 40)
		i = 1;
	else if (health > 20)
		i = 2;
	else 
		i = 3;

	do
	{
		CK_object *newobj = CK_GetNewObj(true);
		newobj->type = CT_Friendly;
		newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
		newobj->posX = posX;
		newobj->posY = posY - 0x100;
		newobj->velX = drop? 0 : US_RndT()/2 - 64;
		newobj->velY = drop? 0 : -30 - US_RndT()/8;
		newobj->clipped = CLIP_normal;
		newobj->zLayer = 0; //drop? 2 : 0;
		CK_SetAction2(newobj, US_RndT() > 0x80? CK_ACTION(AR_ACT_AtroxusDebris1) : CK_ACTION(AR_ACT_AtroxusDebris2));
	} while (--i);
	
}

void AR_SpawnAtroxusExplosion (CK_object *obj)
{

	CK_object *newobj = CK_GetNewObj(true);
	newobj->posX = obj->clipRects.unitX1 + ((obj->clipRects.unitX2 - obj->clipRects.unitX1) * US_RndT() >> 6);
	newobj->posY = obj->clipRects.unitY1 + ((obj->clipRects.unitY2 - obj->clipRects.unitY1) * US_RndT() >> 7);
	newobj->type = CT_Friendly;
	newobj->clipped = CLIP_not;
	newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
	newobj->zLayer = 2;
	CK_SetAction(newobj, CK_ACTION(AR_ACT_SmallExplosion1));
	SD_PlaySound(ATROXIANGROUNDEDSND);
	
}

void AR_FireBombReact(CK_object *obj)
{


	if (obj->topTI)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_ABombBurn1));
	}
	
	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

void AR_AtroxusExplodingReact(CK_object *obj)
{
	if (obj->topTI)
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxusExploding3));

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}

void AR_GameWinTimer(CK_object *obj)
{

	if ((obj->user1 -= SD_GetSpriteSync())<0)
	{
		ck_gameState.levelState = 15;
	}
}

void AR_Act3_SetupFunctions(void)
{
	CK_ACT_AddFunction("AR_ABombBurn", &AR_ABombBurn);
	CK_ACT_AddFunction("AR_ABombReact", &AR_ABombReact);
	CK_ACT_AddFunction("AR_ABombShockwaveReact", &AR_ABombShockwaveReact);
	CK_ACT_AddFunction("AR_AtroxusExplodingReact", &AR_AtroxusExplodingReact);
	CK_ACT_AddFunction("AR_AtroxusFireBomb", &AR_AtroxusFireBomb);
	CK_ACT_AddFunction("AR_AtroxusPain", &AR_AtroxusPain);
	CK_ACT_AddFunction("AR_AtroxusZoomAway", &AR_AtroxusZoomAway);
	CK_ACT_AddFunction("AR_DoAtroxusDamageEffects", &AR_DoAtroxusDamageEffects);
	CK_ACT_AddFunction("AR_FireBombReact", &AR_FireBombReact);
	CK_ACT_AddFunction("AR_GameWinTimer", &AR_GameWinTimer);
	CK_ACT_AddFunction("AR_ShockwaveMove", &AR_ShockwaveMove);
	CK_ACT_AddFunction("AR_SpawnAtroxusExplosion", &AR_SpawnAtroxusExplosion);
	CK_ACT_AddFunction("AtroxusExplode", &AtroxusExplode);
	CK_ACT_AddFunction("AtroxusExplodeTransition", &AtroxusExplodeTransition);
	CK_ACT_AddFunction("AtroxusMove", &AtroxusMove);
	CK_ACT_AddFunction("AtroxusReact", &AtroxusReact);
	CK_ACT_AddFunction("AtroxusSpawnBomb", &AtroxusSpawnBomb);
	CK_ACT_AddFunction("AtroxusSpawnMissile", &AtroxusSpawnMissile);
	CK_ACT_AddColFunction("AtroxusCollide", &AtroxusCollide);
	CK_ACT_AddColFunction("AtroxusPainCollide", &AtroxusPainCollide);
}
