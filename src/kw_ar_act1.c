/*
Keen Worlds: Atroxian Realm's creatures, ported from AR_ACT1.C of its source
(Atroxian Realm, Gridlock 2016, programming by Lemm; GPL v2 or later) to
today's Omnispeak. The actions are in ARACTION.CKD (tools/kw_arealm.py).
*/

#include "kw_arealm.h"

// KEEN WORLDS: the source tests "is this one of the five attack actions" and
// steps to the matching "attack stop" action by position in its action
// table. Omnispeak's actions are looked up by name.
static const char *ar_hornetAttack[5] = {"AR_ACT_HornetAttack1", "AR_ACT_HornetAttack2", "AR_ACT_HornetAttack3",
	"AR_ACT_HornetAttack4", "AR_ACT_HornetAttack5"};
static const char *ar_hornetAttackStop[5] = {"AR_ACT_HornetAttackStop1", "AR_ACT_HornetAttackStop2",
	"AR_ACT_HornetAttackStop3", "AR_ACT_HornetAttackStop4", "AR_ACT_HornetAttackStop5"};

static bool AR_HornetIsAttacking(CK_object *obj)
{
	for (int i = 0; i < 5; ++i)
		if (obj->currentAction == CK_GetActionByName(ar_hornetAttack[i]))
			return true;
	return false;
}

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

// KD_MISC.C


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
// LARVAC (Slug replacement)
// 
//===========================================================================


void AR_SpawnLarvac(int tileX, int tileY);
void AR_LarvacWalk(CK_object *obj);
void AR_LarvacGoop(CK_object *obj);
void AR_LarvacCollide(CK_object *larvac, CK_object *obj2);
void AR_LarvacGoopReact(CK_object *obj);







// Note: Spawn actor slightly above ground so that
// clipping function pushes it on to the ground, as stick to ground
// option is set in action
void AR_SpawnLarvac(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Larvac;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x80; 
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_LarvacWalk0));
}

void AR_LarvacWalk(CK_object *obj)
{
	if (US_RndT() < 0x10)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_LarvacSliming));
		return;
	}

}

void AR_LarvacGoop(CK_object *obj)
{
	// Spawn a larvac slime
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_LarvacGoop;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	new_object->zLayer = 0;
	new_object->posX = obj->posX;
	new_object->posY = obj->posY;
	new_object->xDirection = IN_motion_None;
	new_object->yDirection = IN_motion_Up;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_LarvacGoop1));
	SD_PlaySound(SLUGPOOSND);
}
	
void AR_LarvacCollide(CK_object *obj1, CK_object *obj2)
{

	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
	}
	else if (obj2->type == CT_Stunner)
	{
		CK_StunCreature(obj1, obj2, CK_ACTION(AR_ACT_LarvacStunned));
	}
}


// The slime dissipates when it touches a surface
void AR_LarvacGoopReact(CK_object *obj)
{
	if (obj->topTI || obj->bottomTI || obj->leftTI || obj->rightTI)
	{
		CK_RemoveObj(obj);
		return;
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}


//===========================================================================
//
// BEETLEBONE (Mad Mushroom Replacement)
// 
//===========================================================================

// Deadly on contact; starts bouncing up and down when keen within 3 tiles


void AR_SpawnBeetle(int tileX, int tileY);
void AR_BeetleIdle (CK_object *obj);
void AR_BeetleFallReact (CK_object *obj);






void AR_SpawnBeetle(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Beetle;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = (tileX << 8) - 0;
	new_object->posY = (tileY << 8) - 0x100; 
	new_object->xDirection = IN_motion_None;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_BeetleIdle1));
}

void AR_BeetleIdle (CK_object *obj)
{
	int deltaX = ck_keenObj->posX - obj->posX;
	int deltaY = ck_keenObj->posY - obj->posY;

	// Wait for player
	if (deltaX < 0x300 && deltaX > -0x300 && deltaY < 0)
	{
		obj->velY = -80;
		obj->currentAction = CK_ACTION(AR_ACT_BeetleJump1);
		SD_PlaySound(BEETLEJUMPSND);
	}	
}


void AR_BeetleFallReact (CK_object *obj)
{
	if (obj->topTI)
		CK_SetAction2(obj, CK_ACTION(AR_ACT_BeetleWait));

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}



//===========================================================================
//
// MAGMINE (Inchworm replacement)
// 
//===========================================================================

// Description: Harmless, follow keen.  When two collide, spawn deadly explosion actor of transient existence.



void AR_SpawnMagmine(int tileX, int tileY);
void AR_MagmineWalk(CK_object *obj);
void AR_MagmineCol(CK_object *obj1, CK_object *obj2);
void AR_MagmineReact(CK_object *obj);





void AR_SpawnMagmine(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Magmine;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8); 
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_MagmineWalking1));
}

void AR_MagmineWalk(CK_object *obj)
{
	// Follow Keen 
	if (!obj->xDirection)
		obj->xDirection = obj->clipRects.unitXmid>ck_keenObj->clipRects.unitXmid?IN_motion_Left:IN_motion_Right;


	if (obj->posX < ck_keenObj->posX - 0x80)
		obj->xDirection = IN_motion_Right;

	if (obj->posX > ck_keenObj->posX + 0x80)
		obj->xDirection = IN_motion_Left;

}

void AR_MagmineCol(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == AR_CT_Magmine)
	{
		CK_object *leftmost = obj1->posX < obj2->posX? obj1 : obj2;

		// Spawn an explostion sprite
		CK_object *new_object = CK_GetNewObj(false);
		new_object->type = AR_CT_MagmineExplosion;
		new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
		new_object->zLayer = 2;
		new_object->posX = leftmost->posX;
		new_object->posY = leftmost->posY-0x100;
		new_object->xDirection = IN_motion_None;
		new_object->yDirection = IN_motion_None;
		new_object->clipped = CLIP_not;
		CK_SetAction(new_object, CK_ACTION(AR_ACT_MagmineExplosion1));
		SD_PlaySound(MAGMINEEXPLODESND);

		// Remove both objects
		obj1->type = obj2->type = CT_Nothing;
		CK_RemoveObj(obj1);
		CK_RemoveObj(obj2);
		return;
	}
}

void AR_MagmineReact (CK_object *obj)
{


	// IF a bridge was removed from underneath, then just hover
	if (!obj->topTI)
	{
		int x,y;
		for (y = obj->clipRects.tileY1; y <= obj->clipRects.tileY2 + 1; y++)
		{
			x = obj->clipRects.tileXmid;
			//for (x = obj->clipRects.tileX1; x <= obj->clipRects.tileX2; x++)
			{
				uint16_t tile = CA_TileAtPos(x, y, 1);
				if (TI_ForeTop(tile))
				{
					goto normalDraw;
				}
			}
		}

		obj->posX -= obj->deltaPosX;
		obj->posY -= obj->deltaPosY;
		CK_BasicDrawFunc1(obj);
		return;
	}

normalDraw:
	CK_BasicDrawFunc2(obj);


}


//===========================================================================
//
// FIREBALL (Mine replacement)
// 
//===========================================================================

// Description: Fireball moves back and forth, deadly to touch.



void AR_SpawnFireball(int tileX, int tileY, int direction)
{
	CK_object *obj = CK_GetNewObj(false);

	obj->type = AR_CT_Fireball;
	obj->active = OBJ_ALWAYS_ACTIVE;
	obj->zLayer = 0;
	obj->posX = tileX << 8;
	obj->posY = tileY << 8;
	obj->clipped = CLIP_not;

	switch (direction)
	{
	case 0:
		obj->xDirection = 1;
		obj->yDirection = 0;
		break;
	case 1:
		obj->xDirection = 0;
		obj->yDirection = 1;
		break;
	case 2:
		obj->xDirection = -1;
		obj->yDirection = 0;
		break;
	}

	CK_SetAction(obj, CK_ACTION(AR_ACT_Fireball1));
	//obj->gfxChunk = obj->currentAction->chunkLeft;

	CK_ResetClipRects(obj);
}


//===========================================================================
//
// DARTFROG (Bounder replacement)
// 
//===========================================================================


void AR_SpawnDartfrog(int tileX, int tileY);
void AR_DartFrogIdle(CK_object *obj);
void AR_DartFrogCol(CK_object *obj1, CK_object *obj2);
void AR_DartFrogJumpReact(CK_object *obj);

// Description: Hops along ground toward keen, pausing in between hops.  Deadly to touch, takes 1 shot to stun.



void AR_SpawnDartfrog(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_DartFrog;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = (tileX << G_T_SHIFT);
	new_object->posY = (tileY << G_T_SHIFT) - 0x80; 
	new_object->xDirection = new_object->posX > ck_keenObj->posX? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_DartFrogIdle1));

	if (!new_object->topTI)
		CK_SetAction2(new_object, CK_ACTION(AR_ACT_DartFrogJump1));
}

void AR_DartFrogIdle(CK_object *obj)
{
	
	if (obj->user1++ >= 3 && US_RndT() < 0x80)
	{
		obj->user1 = 0;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_DartFrogFunny1));
		return;
	}
		
	// Face Keen
	if (ck_keenObj->posX < obj->posX)
		obj->xDirection = IN_motion_Left;
	else
		obj->xDirection = IN_motion_Right;

	// Jump
	obj->velY = -50;
	obj->velX = obj->xDirection * 16;
	SD_PlaySound(DARTFROGJUMPSND);
	CK_SetAction2(obj, CK_ACTION(AR_ACT_DartFrogJump1));

}

void AR_DartFrogCol(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
	}
	else if (obj2->type == CT_Stunner)
	{
		CK_StunCreature(obj1, obj2, CK_ACTION(AR_ACT_DartFrogStunned));
	}
}

void AR_DartFrogJumpReact(CK_object *obj)
{
	if (obj->topTI)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_DartFrogIdle1));
		obj->velX = obj->velY = 0;
	}

	if (obj->leftTI || obj->rightTI)
	{
		obj->xDirection = -obj->xDirection;
		obj->velX = -obj->velX;
	}
	


	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
	
}

//===========================================================================
//
// Splider (Mimrock replacement)
// 
//===========================================================================

// Description: Walks around, invulnerable.  Slides at keen when close, exposing it's beak
// to stunner susceptibility.





void AR_SpawnSplider(int tileX, int tileY);
void AR_SpliderWalk(CK_object *obj);
void AR_SpliderSearch(CK_object *obj);
void AR_SpliderSlideEnd(CK_object *obj);
void AR_SpliderSlideReact(CK_object *obj);
void AR_SpliderFallingSlideReact(CK_object *obj);
void AR_SpliderAbortSlideReact(CK_object *obj);
void AR_SpliderSlideCol(CK_object *obj1, CK_object *obj2);









void AR_SpawnSplider(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Splider;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = PRIORITIES - 4;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x100; 
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_SpliderWalk1));
}


void AR_SpliderWalk(CK_object *obj)
{
	uint16_t distanceToKeenX;
	int distanceToKeenY;

	// Turn around randomly 
	if (US_RndT() < 0x4)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderIdle));
		return;
	}


	// Slide attack if Keen is within range
	distanceToKeenX = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;
	distanceToKeenY = ck_keenObj->posY - obj->posY;

	if ((abs(distanceToKeenY) < 0x300) &&
	 ((obj->xDirection == IN_motion_Right && distanceToKeenX < 0x500) ||
	 obj->xDirection == IN_motion_Left && distanceToKeenX > (uint16_t)-0x500))
	{
		SD_PlaySound(SPLIDERSND);
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderSlide1));
		obj->velX = obj->xDirection * 30;
		return;
	}
	
}

void AR_SpliderSearch(CK_object *obj)
{

	if (obj->posX < ck_keenObj->posX)
	{
		obj->xDirection = IN_motion_Right;
	}
	else
	{
		obj->xDirection = IN_motion_Left;
	}

}

void AR_SpliderSlideEnd(CK_object *obj)
{
	// TODO: Play a sound when the decelleration starts
	
	// Glide to a stop
	CK_PhysDampHorz(obj);
	CK_PhysGravityMid(obj);



}

void AR_SpliderSlideReact(CK_object *obj)
{
	if (obj->xDirection == IN_motion_Right && obj->leftTI || obj->xDirection == IN_motion_Left && obj->rightTI)
	{
		obj->velX = 0;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderAbortSlide));
	}

	if (obj->topTI)
	{
		obj->velY = 0;
		if (obj->velX == 0)
			CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderIdle));
	}
	else
	{
		// Splider's fallen off a ledge; switch to slide-while-falling
		obj->posX -= obj->deltaPosX; 
		obj->posY -= 0x10;
		obj->velY = -10;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderFallingSlide));
	}


	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);


}

void AR_SpliderFallingSlideReact(CK_object *obj)
{
	if (obj->xDirection == IN_motion_Right && obj->leftTI || obj->xDirection == IN_motion_Left && obj->rightTI)
	{
		obj->velX = 0;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderAbortSlide));
	}


	if (obj->topTI)
	{
		// If it hits the ground, it immediately slows down
		obj->velY = 0;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderSlide2));
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}

void AR_SpliderAbortSlideReact(CK_object *obj)
{
	// Splider must transition to walking
	
	if (obj->topTI)
	{
		obj->velY = 0;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_SpliderWalk1));
		obj->timeUntillThink = US_RndT() >> 5;
	}
	

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}

void AR_SpliderSlideCol(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
	}
	else if (obj2->type == CT_Stunner)
	{
		int delX = obj2->clipRects.unitXmid - obj1->clipRects.unitXmid;

		// Splider's mouth is vulnerable during the slide
		if ((obj2->yDirection == IN_motion_None && (obj1->xDirection == -obj2->xDirection)) ||
		 obj2->yDirection != IN_motion_None && ((obj1->xDirection == IN_motion_Left && delX < 0 /*&& delX > -0x100*/) || (obj1->xDirection == IN_motion_Right && delX > 0 /*&& delX < 0x100*/))) 
		{
			CK_StunCreature(obj1, obj2, CK_ACTION(AR_ACT_SpliderStunned));
		}

		else
		{
			CK_ShotHit(obj2);
		}
	}

}

//===========================================================================
//
// Larvbat (Skypest)
// 
//===========================================================================



void AR_SpawnLarvbat(int tileX, int tileY);
void AR_LarvbatFly(CK_object *obj);
void AR_LarvbatCollide(CK_object *obj1, CK_object *obj2);
void AR_LarvbatReact(CK_object *obj);



void AR_SpawnLarvbat(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Larvbat;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_Up;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_LarvbatFly1));
}




void AR_LarvbatFly(CK_object *obj)
{

	if (US_RndT() < SD_GetSpriteSync())
		obj->xDirection = -obj->xDirection;

	if (obj->yDirection == IN_motion_Up && US_RndT() < SD_GetSpriteSync())
		obj->yDirection = IN_motion_Down;

	if (obj->yDirection == IN_motion_Down && US_RndT() < SD_GetSpriteSync() * 2)
		obj->yDirection = -obj->yDirection;

	CK_PhysAccelHorz(obj, (int)(obj->xDirection), 20);
	AR_PhysAccelVert(obj, (int)(obj->yDirection), 20);

}

void AR_LarvbatCollide(CK_object *obj1, CK_object *obj2)
{


	if (obj2->type == CT_Stunner)
	{
		CK_SetAction2(obj1, CK_ACTION(AR_ACT_LarvbatDie1));
		obj1->velX = obj1->velY = 0;
		CK_ShotHit(obj2);
		return;
	}
		


}

void AR_LarvbatReact(CK_object *obj)
{

	if (obj->bottomTI)
	{
		obj->velY = 8;
		obj->yDirection = IN_motion_Down;
	}

	if (obj->leftTI)
	{
		obj->velX = 0;
		obj->xDirection = IN_motion_Left;
	}

	if (obj->rightTI)
	{
		obj->velX = 0;
		obj->xDirection = IN_motion_Right;
	}
	
	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}


//===========================================================================
//
// HORNET (wormmouth replacement)
// 
//===========================================================================
//
// Description: Walks back and forth along the ground, darts forward to attack keen when it gets close
// takes two hits to stun

void AR_SpawnHornet(int tileX, int tileY);
void AR_HornetWalk(CK_object *obj);
void AR_HornetCollide(CK_object *obj1, CK_object *obj2);
void AR_HornetReact(CK_object *obj);
void AR_DrawHitFlash(CK_object *obj, uint16_t *healthinfo, int adjX, int adjY);










// user1 = health



void AR_SpawnHornet(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Hornet;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x100; 
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	new_object->user1 = 2; 
	CK_SetAction(new_object, CK_ACTION(AR_ACT_HornetWalk1));
}

void AR_HornetWalk(CK_object *obj)
{
	uint16_t dx;
	int dy;

	// Slide at Keen if he gets within range
	
	dx = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;
	dy = ck_keenObj->clipRects.unitY2 - obj->clipRects.unitY2;


	if (abs(dy) < 0x200 && (obj->xDirection == IN_motion_Right && dx < 0x400 || obj->xDirection == IN_motion_Left && dx > 0xFC00))
	{
		//Play a sound
		obj->timeUntillThink = 30 + 20;
		SD_PlaySound(HORNETATTACKSND);
		CK_SetAction2(obj, CK_ACTION(AR_ACT_HornetAttack1));
		return;
	}


}

void AR_HornetCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		// Fudgy check to see if contact was not made by Keen hanging on to
		// ledge 

		if ((obj2->currentAction == CK_ACTION(CK_ACT_keenHang1) || obj2->currentAction == CK_ACTION(CK_ACT_keenHang2))
		 && (obj2->posY > obj1->posY + 0x100))
		{
			return;
		}

		CK_KillKeen();
		return;
	}

	if (obj2->type == CT_Stunner)
	{
		int flashtime, health;

		health = obj1->user1 & 0xFF;

		if (--health == 0)
		{
			CK_StunCreature(obj1, obj2, CK_ACTION(AR_ACT_HornetStunned1));
			return;
		}
		else
		{
			obj1->user1 = (10 << 8) + health;
			CK_ShotHit(obj2);
			return;
		}
	}
}


void AR_HornetReact(CK_object *obj)
{
	// Hit wall walking right; turn around and go left
	if (obj->xDirection == IN_motion_Right && obj->leftTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		if (AR_HornetIsAttacking(obj))
			obj->xDirection = IN_motion_Right;
		else
		{
		obj->xDirection = IN_motion_Left;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
		}
	}
		// Hit wall walking left; turn around and go right
	else if (obj->xDirection == IN_motion_Left && obj->rightTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		if (AR_HornetIsAttacking(obj))
			obj->xDirection = IN_motion_Left;
		else
		{
		obj->xDirection = IN_motion_Right;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
		}
	}
		// Walked off of ledge; turn around
	else if (obj->topTI == 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		// KEEN WORLDS: the source steps from the n-th attack action to the n-th
		// "attack stop" action by pointer arithmetic on its action table; here
		// the actions are looked up by name.
		CK_action *turned = obj->currentAction;
		for (int i = 0; i < 5; ++i)
			if (obj->currentAction == CK_GetActionByName(ar_hornetAttack[i]))
				turned = CK_GetActionByName(ar_hornetAttackStop[i]);
		CK_SetAction2(obj, turned);
	}
	
	AR_DrawHitFlash(obj, (uint16_t *)&(obj->user1), 0, -0x80);

}

void AR_DrawHitFlash(CK_object *obj, uint16_t *healthinfo, int adjX, int adjY)
{
	// high byte of healthinfo holds remaining whileflash time; lowbyte holds health

	int flashtime = *healthinfo >> 8;

	if ((flashtime -= SD_GetSpriteSync()) < 0)
		flashtime = 0;

	*healthinfo = (*healthinfo & 0xFF) | (flashtime << 8);

	RF_AddSpriteDraw(&(obj->sde), obj->posX + adjX, obj->posY + adjY, obj->gfxChunk, flashtime?true:false, obj->zLayer);


}


//===========================================================================
//
// DARTGUNS
// 
//===========================================================================



void AR_DartGun(CK_object *obj);





void AR_SpawnDartgun (int tileX, int tileY, int dir)
{
	CK_object *newobj;

	newobj = CK_GetNewObj(false);
	newobj->type = AR_CT_Dartgun;
	newobj->active = OBJ_ACTIVE;
	newobj->clipRects.tileX1 = newobj->clipRects.tileX2 = tileX;
	newobj->clipRects.tileY1 = newobj->clipRects.tileY2 = tileY;
	newobj->posX = newobj->clipRects.unitX1 = newobj->clipRects.unitX2 = tileX << G_T_SHIFT;
	newobj->posY = newobj->clipRects.unitY1 = newobj->clipRects.unitY2 = tileY << G_T_SHIFT;
	newobj->user1 = dir;
	CK_SetAction(newobj, CK_ACTION(AR_ACT_DartGun));
}


void AR_DartGun(CK_object *obj)
{

	CK_object *newobj;

	newobj = CK_GetNewObj(true);
	newobj->type = CT5_EnemyShot;
	newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
	newobj->posX = obj->posX;
	newobj->posY = obj->posY;
	newobj->clipped = CLIP_dart;

	switch (obj->user1)
	{
	case 0:
		newobj->xDirection = IN_motion_None;
		newobj->yDirection = IN_motion_Up;
		newobj->posX += 0x80;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_DartUp1));
		break;
	case 1:
		newobj->xDirection = IN_motion_Right;
		newobj->yDirection = IN_motion_None;
		newobj->posY += 0x40;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_DartHorz1));
		break;
	case 2:
		newobj->xDirection = IN_motion_None;
		newobj->yDirection = IN_motion_Down;
		newobj->posX += 0x80;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_DartDown1));
		break;
	case 3:
		newobj->xDirection = IN_motion_Left;
		newobj->yDirection = IN_motion_None;
		newobj->posY += 0x40;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_DartHorz1));
		break;
	}

	SD_PlaySound(DARTSHOOTSND);
}

void AR_Act1_SetupFunctions(void)
{
	CK_ACT_AddFunction("AR_BeetleFallReact", &AR_BeetleFallReact);
	CK_ACT_AddFunction("AR_BeetleIdle", &AR_BeetleIdle);
	CK_ACT_AddFunction("AR_DartFrogIdle", &AR_DartFrogIdle);
	CK_ACT_AddFunction("AR_DartFrogJumpReact", &AR_DartFrogJumpReact);
	CK_ACT_AddFunction("AR_DartGun", &AR_DartGun);
	CK_ACT_AddFunction("AR_HornetReact", &AR_HornetReact);
	CK_ACT_AddFunction("AR_HornetWalk", &AR_HornetWalk);
	CK_ACT_AddFunction("AR_LarvacGoop", &AR_LarvacGoop);
	CK_ACT_AddFunction("AR_LarvacGoopReact", &AR_LarvacGoopReact);
	CK_ACT_AddFunction("AR_LarvacWalk", &AR_LarvacWalk);
	CK_ACT_AddFunction("AR_LarvbatFly", &AR_LarvbatFly);
	CK_ACT_AddFunction("AR_LarvbatReact", &AR_LarvbatReact);
	CK_ACT_AddFunction("AR_MagmineReact", &AR_MagmineReact);
	CK_ACT_AddFunction("AR_MagmineWalk", &AR_MagmineWalk);
	CK_ACT_AddFunction("AR_SpliderAbortSlideReact", &AR_SpliderAbortSlideReact);
	CK_ACT_AddFunction("AR_SpliderFallingSlideReact", &AR_SpliderFallingSlideReact);
	CK_ACT_AddFunction("AR_SpliderSearch", &AR_SpliderSearch);
	CK_ACT_AddFunction("AR_SpliderSlideEnd", &AR_SpliderSlideEnd);
	CK_ACT_AddFunction("AR_SpliderSlideReact", &AR_SpliderSlideReact);
	CK_ACT_AddFunction("AR_SpliderWalk", &AR_SpliderWalk);
	CK_ACT_AddColFunction("AR_DartFrogCol", &AR_DartFrogCol);
	CK_ACT_AddColFunction("AR_HornetCollide", &AR_HornetCollide);
	CK_ACT_AddColFunction("AR_LarvacCollide", &AR_LarvacCollide);
	CK_ACT_AddColFunction("AR_LarvbatCollide", &AR_LarvbatCollide);
	CK_ACT_AddColFunction("AR_MagmineCol", &AR_MagmineCol);
	CK_ACT_AddColFunction("AR_SpliderSlideCol", &AR_SpliderSlideCol);
}
