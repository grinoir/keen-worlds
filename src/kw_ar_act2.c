/*
Keen Worlds: Atroxian Realm's creatures, ported from AR_ACT2.C of its source
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
// Atroxian (Bluebird replacement)
// 
//===========================================================================

// Description: Files around like a keen 4 bluebird, seeking keen.
// If it's hit by a stunner, it's temporarily stunned and it falls to the ground.
// While stunned, if hit by 5 hits, then transforms to bipedal state
// If hit by 5 hits in bipedal state, then it is stunned for good

// user1 = flying variable
// user2 = packed health format (see Hornet)

void AR_SpawnAtroxian(int tileX, int tileY);
void AR_AtroxianFly(CK_object *obj);
void AR_AtroxianRecoverToFlying (CK_object *obj);
void AR_AtroxianExploding (CK_object *obj);
void AR_AtroxianWalk(CK_object *obj);
void AR_AtroxianFlyingCollide(CK_object *obj1, CK_object *obj2);
void AR_AtroxianStunnedCollide(CK_object *obj1, CK_object *obj2);
void AR_AtroxianWalkingCollide(CK_object *obj1, CK_object *obj2);
void AR_AtroxianFlyingReact(CK_object *obj);
void AR_AtroxianStunnedReact(CK_object *obj);
void AR_AtroxianBasicReact (CK_object *obj);
void AR_AtroxianExplodingReact (CK_object *obj);
void AR_AtroxianPreWalkReact(CK_object *obj);
void AR_AtroxianWalkingReact(CK_object *obj);








// K1n9_Duk3 fix: don't need these anymore
/*
*/


//Flying

// Stunned (temporarily)




// Transition to Walking (when hit ground)

// Walking state

// Dead


// Explosion Sprites

// K1n9_Duk3 fix: don't need these anymore
/*
*/

void AR_SpawnAtroxian(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Atroxian;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_Up;
	//new_object->clipped = CLIP_simple;
	new_object->user1 = 0;
	new_object->user2 = 5;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AtroxianFly1));
}

void AR_AtroxianFly(CK_object *obj)
{
	if (obj->user1)
	{
		if (obj->clipRects.unitXmid < ck_keenObj->clipRects.unitXmid)
			obj->xDirection = IN_motion_Right;
		else
			obj->xDirection = IN_motion_Left;
	}

	CK_PhysAccelHorz(obj, obj->xDirection, 16);

	if (obj->posY < ck_keenObj->posY - 0x100)
		AR_PhysAccelVert(obj, IN_motion_Down, 16);
	else
		AR_PhysAccelVert(obj, IN_motion_Up, 16);

}


void AR_AtroxianExploding(CK_object *obj)
{
	CK_Fall(obj);

	if ((obj->user4 -= SD_GetSpriteSync()) == 0)
	{
		CK_object *newobj = CK_GetNewObj(true);
		obj->user4 = 20;
		newobj->posX = obj->clipRects.unitX1 - 0x80 + ((obj->clipRects.unitX2 - obj->clipRects.unitX1) * US_RndT() >> 8);
		newobj->posY = obj->clipRects.unitY1 - 0x100 + ((obj->clipRects.unitY2 - obj->clipRects.unitY1) * US_RndT() >> 8);
		newobj->type = CT_Friendly;
		newobj->clipped = CLIP_not;
		newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
		newobj->zLayer = 2;
		CK_SetAction(newobj, CK_ACTION(AR_ACT_SmallExplosion1));
		SD_PlaySound(ATROXIANGROUNDEDSND);
	}
}

void AR_AtroxianRecoverToFlying (CK_object *obj)
{
	obj->velY = -20;
	CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxianFly1));
	ck_nextX = 0;
	ck_nextY = -0x80;
	CK_PhysFullClipToWalls(obj);
}


void AR_AtroxianWalk(CK_object *obj)
{
	if (US_RndT() < 0x20)
	{
		
		// Follow keen
		int delX = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;
		// Follow Keen
		if (delX > 0x200)
			obj->xDirection = IN_motion_Right;
		else if (delX < -0x200)
			obj->xDirection = IN_motion_Left;
	}

}

void AR_AtroxianFlyingCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}

	if (obj2->type == CT_Stunner)
	{
		CK_ShotHit(obj2);
		CK_SetAction2(obj1, CK_ACTION(AR_ACT_AtroxianStunnedFall1));
		obj1->velX = 0;
		obj1->velY = -20;
	}


}

void AR_AtroxianStunnedCollide(CK_object *obj1, CK_object *obj2)
{
	int health;

	if (obj2->type == CT_Stunner)
	{
		CK_ShotHit(obj2);

		health = obj1->user2 & 0xFF;

		if (--health == 0)
		{
			// Set new health to 5
			obj1->user2 = 5;
			obj1->clipped = CLIP_normal;
			obj1->user4 = 10;
			CK_SetAction2(obj1, CK_ACTION(AR_ACT_AtroxianWalkPre1));
			// TODO: Spawn an explosion sprite here
		}
		else
		{
			obj1->user2 = (14 << 8) | health; 
		}
	}

}


void AR_AtroxianWalkingCollide(CK_object *obj1, CK_object *obj2)
{
	extern CK_action AR_ACT_MagmineExplosion1;	// K1n9_Duk3 fix

	int health;
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}


	if (obj2->type == CT_Stunner)
	{
		CK_ShotHit(obj2);

		health = obj1->user2 & 0xFF;

		if (--health == 0)
		{
			CK_object *newobj = CK_GetNewObj(true);

			newobj->type = CT_Friendly;
			newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
			newobj->clipped = CLIP_not;
			newobj->posX = obj1->posX + 0xC0;
			newobj->posY = obj1->posY;
			newobj->zLayer = 2;
			// K1n9_Duk3 fix (requested by Gridlock):
			//CK_SetAction(newobj, CK_ACTION(AR_ACT_AtroxianBigExplosion1));
			CK_SetAction(newobj, CK_ACTION(AR_ACT_MagmineExplosion1));

			CK_SetAction(obj1, CK_ACTION(AR_ACT_AtroxianDead1));
			SD_PlaySound(MAGMINEEXPLODESND);
		}
		else
		{
			obj1->user2 = (14 << 8) | health; 
		}
		return;

	}

}


void AR_AtroxianFlyingReact(CK_object *obj)
{

	if ((obj->bottomTI || obj->topTI) && !obj->user1)
		obj->user1++;


	if (obj->rightTI && obj->velX < 0 || obj->leftTI && obj->velX > 0)
	{
		obj->velX = 0;
		obj->xDirection = -obj->xDirection;
	}

	if (!obj->bottomTI && !obj->topTI)
		obj->user1 = 0;

	AR_DrawHitFlash(obj, (uint16_t *)&(obj->user2), 0, 0);
	//RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}


void AR_AtroxianStunnedReact(CK_object *obj)
{
	if (obj->topTI)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxianStunned1));
	}

	AR_DrawHitFlash(obj, (uint16_t *)&(obj->user2), 0, 0);
	//RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

void AR_AtroxianBasicReact (CK_object *obj)
{
	AR_DrawHitFlash(obj, (uint16_t *)&(obj->user2), 0, 0);
}

void AR_AtroxianExplodingReact(CK_object *obj)
{
	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
	

}

void AR_AtroxianPreWalkReact(CK_object *obj)
{
	if (obj->topTI)
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxianWalk1));

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}

void AR_AtroxianWalkingReact(CK_object *obj)
{

	// Hit wall walking right; turn around and go left
	if (obj->xDirection == IN_motion_Right && obj->leftTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = IN_motion_Left;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
		// Hit wall walking left; turn around and go right
	else if (obj->xDirection == IN_motion_Left && obj->rightTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = IN_motion_Right;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
		// Walked off of ledge; turn around
	else if (obj->topTI == 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
	
	AR_DrawHitFlash(obj, (uint16_t *)&(obj->user2), 0x80, 0x40);

}


//===========================================================================
//
// Atroxite (Lick replacement)
// 
//===========================================================================

// Description:  Jumps in a high arc; when it gets close to keen, it slides forward along the ground





void AR_SpawnAtroxite(int tileX, int tileY);
void AR_AtroxiteMove(CK_object *obj);
void AR_AtroxiteAttack(CK_object *obj);
void AR_AtroxiteCollide(CK_object *obj1, CK_object *obj2);
void AR_AtroxiteJumpReact(CK_object *obj1);





// Stunned

void AR_SpawnAtroxite(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Atroxite;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AtroxiteHop2));
}


void AR_AtroxiteMove(CK_object *obj)
{
	int dx, dy;

	dx = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;	
	dy = ck_keenObj->clipRects.unitY1 - obj->clipRects.unitY1;

	// follow keen
	if (dx > 0)
		obj->xDirection = IN_motion_Right;
	else
		obj->xDirection = IN_motion_Left;


	// Attack if within range
	if (dy >= -0x100 && dy <= 0x100 && dx >= -0x400 && dx <= 0x400)
	{
		SD_PlaySound(ATROXITEATTACKSND);
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxiteAttack1));
		return;
	}


	// Otherwise, jump
	// Adapt jump height based on tiles in front

	{
	// KEEN WORLDS: the source reads the map and tile properties directly
	int wallX = obj->clipRects.tileXmid + obj->xDirection, wallY = obj->clipRects.tileY2;
	#define AR_WALL(up) (obj->xDirection == IN_motion_Right ? TI_ForeLeft(CA_TileAtPos(wallX, wallY - (up), 1)) : TI_ForeRight(CA_TileAtPos(wallX, wallY - (up), 1)))
	if (!AR_WALL(0) && !AR_WALL(1) && (AR_WALL(2) || AR_WALL(3)))
	{
		obj->velX = obj->xDirection << 4;
		obj->velY = -30;
		return;
	}
	}

	obj->velX = obj->xDirection << 3;
	obj->velY = -64;

}

void AR_AtroxiteAttack(CK_object *obj)
{

	if (ck_gameState.difficulty == 1)
		ck_nextX -= 16 * obj->xDirection;
	else if (ck_gameState.difficulty == 3)
		ck_nextX += 16 * obj->xDirection;
}

void AR_AtroxiteCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Stunner)
	{
		CK_StunCreature(obj1, obj2, CK_ACTION(AR_ACT_AtroxiteStunned1));
		SD_PlaySound(ATROXITESTUNNEDSND);
		return;
	}

	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}

}

void AR_AtroxiteJumpReact(CK_object *obj)
{
	if (obj->topTI)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AtroxiteHop3));
		ck_nextX = ck_nextY = 0;
		obj->velX = obj->velY = 0;
	}

	else if (obj->bottomTI)
	{
		obj->velY = 0;
		obj->yDirection = IN_motion_Down;
	}

	else if (obj->leftTI || obj->rightTI)
	{
		// Just keep moving over walls

		//obj->xDirection = -obj->xDirection;
		//obj->velX = -obj->velX;
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}


//===========================================================================
//
// Axoclaw (Thundercloud replacement)
// 
//===========================================================================


// Description: Hovers in the air, moves back and forth.
// Periodically shoots a homing missile that moves at a constant horizontal speed and homes in on keen vertically

void AR_SpawnAxoclaw(int tileX, int tileY);
void AR_AxoclawThink(CK_object *obj);
void AR_AxoclawShoot(CK_object *obj);
void AR_AxoclawReact(CK_object *obj);
void AR_AxoclawShot(CK_object *obj);
void AR_AxoclawShotReact(CK_object *obj);













void AR_SpawnAxoclaw(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Axoclaw;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AxoclawMove1));
}

void AR_AxoclawThink(CK_object *obj)
{
	int dx = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;

	// Follow keen
	if (US_RndT() < 0x40)
	{
		if (dx > 0x400)
			obj->xDirection = IN_motion_Right;
		else if (dx < -0x400)
			obj->xDirection = IN_motion_Left;
	}

	// Switch to shooting state if keen in range
	if (abs(dx) < 0x900 && ck_keenObj->posY > obj->posY && US_RndT() < 0x20)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AxoclawShoot1));
		return;
	}



}

void AR_SpawnAxoclawMissile(uint16_t x, uint16_t y, int dir)
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
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AxoclawShotMove1));


}

void AR_AxoclawShoot(CK_object *obj)
{
	int dir = ck_keenObj->clipRects.unitXmid > obj->clipRects.unitXmid ? IN_motion_Right : IN_motion_Left;
	AR_SpawnAxoclawMissile(obj->clipRects.unitXmid - 0x40, obj->posY + 0x120, dir);
}


void AR_AxoclawReact(CK_object *obj)
{

	if (obj->leftTI)
		obj->xDirection = IN_motion_Left;

	if (obj->rightTI)
		obj->xDirection = IN_motion_Right;

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
	
}


void AR_AxoclawShot(CK_object *obj)
{

	int dy = ck_keenObj->posY - obj->posY;

	if (dy > 0)
		obj->yDirection = IN_motion_Down;
	else
		obj->yDirection = IN_motion_Up;


	AR_PhysAccelVert(obj, obj->yDirection, 4);

}


void AR_AxoclawShotReact(CK_object *obj)
{

	if (obj->bottomTI || obj->topTI || obj->leftTI || obj->rightTI)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AxoclawShotDie1));
	}

	
	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

//===========================================================================
//
// Axosis and Krile Ghost (Smirky / Arachnut Replacement)
// 
//===========================================================================

// Description
// The Axosis walks back and forth and is deadly to touch.  It only vulnerable if contacted by a Krile Ghost.
// If the Krile Ghost touches the Axosis, then the Axosis is stunned and vulnerable to Keen's pogo stick.

// The Krile ghost teleports around the level, alternating between random locations and locations near the top left
// axosis.

// Ghost searches for an appropriately large space to warp in, then teleports and falls to floor
// If in attack mode, warps to next axosis; otherwise, just roams around for awhile
// Keen must be in visual range of the top left axosis for the ghost to warp and attack it




void AR_SpawnAxosis(int tileX, int tileY);
void AR_AxosisWalk(CK_object *obj);
void AR_AxosisStunCount(CK_object *obj);
void AR_AxosisCollide(CK_object *obj1, CK_object *obj2);
void AR_AxosisStunnedCollide(CK_object *obj1, CK_object *obj2);

#define KRILEGHOSTSTUNDELAY 400





void AR_SpawnKrileGhost(int tileX, int tileY);
void AR_KrileGhostHide(CK_object *obj);
void AR_KrileGhostFindAxosis(CK_object *obj);
void AR_KrileGhostHunt(CK_object *obj);
void AR_KrileGhostHuntCollide(CK_object *obj1, CK_object *obj2);
void AR_KrileGhostRoamReact(CK_object *obj);








// user1; holds index in list of Axosises

void AR_SpawnAxosis(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Axosis;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x180;  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AxosisWalk1));

	new_object->user1 = ar_state.axosisNum++;
}


void AR_AxosisWalk(CK_object *obj)
{
	int dx = ck_keenObj->posX - obj->posX;

	// Seek keen
	if (US_RndT() < 0x8)
		obj->xDirection = dx > 0 ? IN_motion_Right : IN_motion_Left;

}

void AR_AxosisStunCount(CK_object *obj)
{
	if (obj->user2++ == 3)
	{
		obj->user2 = 0;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_AxosisStunned10));
	}

}

void AR_AxosisCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}

	if (obj2->type == CT_Stunner)
	{
		CK_ShotHit(obj2);
		return;
	}

}

void AR_AxosisStunnedCollide(CK_object *obj1, CK_object *obj2)
{
	// K1n9_Duk3 fix: nextAxosis is not used in here
	/*
	CK_object *nextAxosis;

	if (ar_state.krileGhost)
		nextAxosis = AR_ObjFromUser(ar_state.krileGhost->user4);
	*/
		
	// Squish Axosis if keen is pogoing
	if (obj2->type == CT_Player)
	{
		if (obj2->currentAction >= CK_ACTION(CK_ACT_keenPogo1) && obj2->currentAction <= CK_ACTION(CK_ACT_keenPogo3))
		{
			// Play a squish sound
			SD_PlaySound(AXOSISSQUISHSND);
			
			CK_SetAction2(obj1, CK_ACTION(AR_ACT_AxosisDead1));

			// Tell the ghost to find the next axosis
			if (ar_state.krileGhost)
			{
				// K1n9_Duk3 fix:
				// This makes sure the ghost is teleporting out, so it won't
				// get stuck near the dead axosis. You could just change the
				// ghost's action to AR_ACT_KrileGhostTeleOut5, but matching
				// the teleport state to the current sprite looks much better.
				{
					register CK_action *nextaction;

					switch (ar_state.krileGhost->gfxChunk)
					{
					case 0:
					case -1:
						nextaction = CK_ACTION(AR_ACT_KrileGhostTeleOut5);
						break;
					case SPR_KRILEGHOSTTELE4:
						nextaction = CK_ACTION(AR_ACT_KrileGhostTeleOut4);
						break;
					case SPR_KRILEGHOSTTELE3:
						nextaction = CK_ACTION(AR_ACT_KrileGhostTeleOut3);
						break;
					case SPR_KRILEGHOSTTELE2:
						nextaction = CK_ACTION(AR_ACT_KrileGhostTeleOut2);
						break;
					default:
						nextaction = CK_ACTION(AR_ACT_KrileGhostTeleOut1);
						break;
					}

					CK_SetAction2(ar_state.krileGhost, nextaction);
				}

				if (++(ar_state.krileGhost->user3) < ar_state.axosisNum)
				{
					CK_object *o;
					for (o = ck_keenObj; o; o = o->next)
					{
						if (o->type == AR_CT_Axosis && o->user1 == ar_state.krileGhost->user3)
						{
							// HACK: Ghosts are reviving the last stunned axosis
							// Therefore, explicitly tell the ghost to ignore dead axosises
							// and hope that stops it from happening

							// K1n9_Duk3 mod: don't need the check anymore
							/*
							if (o->currentAction == CK_ACTION(AR_ACT_AxosisDead1))
							{
								ar_state.krileGhost->user4 = 0;
								continue;
							}
							*/

							ar_state.krileGhost->user4 = AR_ObjToUser(o);
							return;
						}
					}
				}
				// K1n9_Duk3 fix: removed the else and added a return in the loop
				// to make sure user4 is set to NULL when no Axosis is found.

				// No more axosises
				ar_state.krileGhost->user4 = 0;

			}

			return;
		}
	}
}








//user3; index of Axosis to hunt;
//user4: pointer to said axosis; if NULL, then don't search 

void AR_SpawnKrileGhost(int tileX, int tileY)
{
	CK_object *new_object;

	if (ar_state.krileGhost)
		Quit("More than one Krile ghost!");

	new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_KrileGhost;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->zLayer = 1;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x300;  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_full;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_KrileGhostHide));
	ar_state.krileGhost = new_object;
	
}

void AR_KrileGhostHide(CK_object *obj)
{
	CK_object *nextAxosis = AR_ObjFromUser(obj->user4);

	if (nextAxosis)
	{
		int dx = nextAxosis->posX - ck_keenObj->posX;
		int dy = nextAxosis->posY - ck_keenObj->posY;
		if (dy > -0x200 && dy < 0x900 && dx > -0x600 && dx < 0x600)
			CK_SetAction2(obj, CK_ACTION(AR_ACT_KrileGhostMoveToAxosis));
	}
	else
	{
		CK_RemoveObj(obj);
	}
}

void AR_KrileGhostFindAxosis(CK_object *obj)
{
	CK_object *nextAxosis = AR_ObjFromUser(obj->user4);

	if (!nextAxosis)
	{
		CK_RemoveObj(obj);
		return;
	}
	// Move to Ghost coords

	obj->posX = nextAxosis->posX;
	obj->posY = nextAxosis->posY - 0x180;
	obj->user2 = KRILEGHOSTSTUNDELAY;
	CK_SetAction(obj, CK_ACTION(AR_ACT_KrileGhostTeleIn1));
}

void AR_KrileGhostHunt(CK_object *obj)
{
	CK_object *nextAxosis = AR_ObjFromUser(obj->user4);

	if (!nextAxosis)
	{
		CK_RemoveObj(obj);
		return;
	}

	// Track down the next Axosis
	if ((obj->user2 -= SD_GetSpriteSync())< 0)
		obj->user2 = 0;

	if (nextAxosis && obj->user2 < KRILEGHOSTSTUNDELAY/2)
	{
		obj->xDirection = nextAxosis->posX > obj->posX? IN_motion_Right : IN_motion_Left;
	}

}

void AR_KrileGhostHuntCollide(CK_object *obj1, CK_object *obj2)
{

	CK_object *nextAxosis = AR_ObjFromUser(obj1->user4);

	if (obj2 && obj2 == nextAxosis && (int)obj1->user2 == 0)
	{
		// make sure it's centered
		int dx = obj1->clipRects.unitXmid - obj2->clipRects.unitXmid;
		if (abs(dx) < 0x40)
		{
			CK_SetAction2(obj2, CK_ACTION(AR_ACT_AxosisStunned1));
			CK_SetAction2(obj1, CK_ACTION(AR_ACT_KrileGhostZap1));
			obj1->user2 = KRILEGHOSTSTUNDELAY;
			SD_PlaySound(KRILEGHOSTATTACKSND);
			return;
		}
	}
}

void AR_KrileGhostRoamReact(CK_object *obj)
{

	// Hit wall walking right; turn around and go left
	if (obj->xDirection == IN_motion_Right && obj->leftTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = IN_motion_Left;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
		// Hit wall walking left; turn around and go right
	else if (obj->xDirection == IN_motion_Left && obj->rightTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = IN_motion_Right;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
		// Walked off of ledge; turn around
	else if (obj->topTI == 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, US_RndT()>0xC0?true:false, obj->zLayer);
	
}



//===========================================================================
//
// MOXEL (Sprite replacement)
// 
//===========================================================================

// Description: An immobile device that sits on vertical walls and launches fireballs if keen passes before it
// It always points to the left.




void AR_SpawnMoxel(int tileX, int tileY);
bool AR_KeenWithinMoxelRange(CK_object *obj);
void AR_MoxelThink(CK_object *obj);
void AR_MoxelWaitToShoot(CK_object *obj);
void AR_MoxelShotBurnout(CK_object *obj);
void AR_MoxelShotReact(CK_object *obj);




void AR_SpawnMoxel(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Moxel;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);  
	new_object->xDirection = 0;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_MoxelWait));
}

bool AR_KeenWithinMoxelRange(CK_object *obj)
{
	int dx = ck_keenObj->posX - obj->posX;
	int dy = ck_keenObj->posY - obj->posY;

	if (dx > -0x980 && dx < 0 && dy < 0x200 && dy > -0x200)
	{
		return true;
	}

	return false;

}

void AR_MoxelThink(CK_object *obj)
{

	// Switch to throwing state is keen is in front of the moxel
	if (AR_KeenWithinMoxelRange(obj))
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_MoxelWaitToShoot));
		obj->user1 = 55;
		return;
	}

}

void AR_MoxelWaitToShoot(CK_object *obj)
{
	if (AR_KeenWithinMoxelRange(obj))
	{
		if ((obj->user1 -= SD_GetSpriteSync()) < 0) 
		{
			// Spawn a missile
			CK_object *new_object = CK_GetNewObj(true);
			new_object->type = AR_CT_MoxelShot;
			new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
			new_object->zLayer = 2;
			new_object->posX = obj->posX - 0x80;
			new_object->posY = obj->posY + 0x40;
			new_object->xDirection = IN_motion_Left;
			new_object->velX = -40;
			new_object->velY = -20;
			new_object->clipped = CLIP_normal;
			CK_SetAction(new_object, CK_ACTION(AR_ACT_MoxelShotFall1));

			// 
			SD_PlaySound(SENTRYSHOOTSND);
			CK_SetAction2(obj, CK_ACTION(AR_ACT_MoxelShoot));
			return;
		}
	}
	else
	{
		// Reset
		CK_SetAction2(obj, CK_ACTION(AR_ACT_MoxelWait));
		return;
	}
}
	

void AR_MoxelShotBurnout(CK_object *obj)
{
	if ((obj->user1 -= SD_GetSpriteSync()) < 0)
	{
		CK_RemoveObj(obj);
		return;
	}

}

void AR_MoxelShotReact(CK_object *obj)
{
	if (obj->topTI)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_MoxelShotGround1));
		obj->user1 = 70;
		SD_PlaySound(SENTRYSHOTIMPACTSND);
		return;
	}

	if (obj->leftTI || obj->rightTI)
	{
		obj->velX = 0;
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);

}


//===========================================================================
//
// Dark Krile Guardian (Berkeloid Replacement)
// 
//===========================================================================

// Description: A berkeloid that shoots projectiles in a straight line.
// Deadly to keen, turns around if it detects keen






void AR_SpawnKrileGuardian(int tileX, int tileY);
void AR_KrileGuardianWalk(CK_object *obj);
void AR_KrileGuardianShoot(CK_object *obj);
void AR_KrileGuardianReact(CK_object *obj);
void AR_KrileGuardianShotReact(CK_object *obj);





void AR_SpawnKrileGuardian(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_KrileGuardian;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x500;  
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	new_object->user2 = 8;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_KrileGuardianWalk1));

}

void AR_KrileGuardianWalk(CK_object *obj)
{

#if 0
	// Search for keen
	if (US_RndT() < 0x4)
	{
		int dx = ck_keenObj->clipRects.unitXmid - obj->clipRects.unitXmid;
		obj->xDirection = dx > 0? IN_motion_Right : IN_motion_Left;
	}
#endif

	// Shoot randomly
	if (US_RndT() < 12)
	{
		CK_SetAction2(obj, CK_ACTION(AR_ACT_KrileGuardianShoot1));
		return;
	}
}

void AR_KrileGuardianShoot(CK_object *obj)
{
	// Spawn a missile
	CK_object *new_object = CK_GetNewObj(true);
	new_object->type = AR_CT_GuardShot;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	new_object->zLayer = PRIORITIES - 2;
	new_object->posX = obj->clipRects.unitXmid + (obj->xDirection << 5);
	new_object->posY = obj->posY + 0x280;
	new_object->xDirection = obj->xDirection;
	new_object->yDirection = IN_motion_None;
	new_object->clipped = CLIP_normal;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_KrileGuardShotMove1));
	SD_PlaySound(KRILEGUARDTHROWSND);
}

void AR_KrileGuardianReact(CK_object *obj)
{
	// Hit wall walking right; turn around and go left
	if (obj->xDirection == IN_motion_Right && obj->leftTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = IN_motion_Left;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
		// Hit wall walking left; turn around and go right
	else if (obj->xDirection == IN_motion_Left && obj->rightTI != 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = IN_motion_Right;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}
		// Walked off of ledge; turn around
	else if (obj->topTI == 0)
	{
		obj->posX -= obj->deltaPosX;
		obj->xDirection = -obj->xDirection;
		obj->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(obj, obj->currentAction);
	}

	// Hovering action
	obj->user1 += (int)((int)obj->user2 * (int)SD_GetSpriteSync());
	if ((int)obj->user1 > 0)
	{

		obj->user1 = 0;
		obj->user2 = -8;
	}
	else if ((int)obj->user1 < -0x100)
	{
		obj->user1 = -0x100;
		obj->user2 = 8;
	}
	
	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY + obj->user1, obj->gfxChunk, false, obj->zLayer);
}

void AR_KrileGuardianShotReact(CK_object *obj)
{
	if (obj->topTI || obj->bottomTI || obj->leftTI || obj->rightTI)
	{
		// obj->posY += 0x140;
		obj->clipped = CLIP_not;
		CK_SetAction2(obj, CK_ACTION(AR_ACT_KrileGuardShotDie1));
		return;
	}

	RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

//===========================================================================
//
// GENERATOR CORE 
// 
//===========================================================================

// description: Sits there, generating, untill Keen shoots it.  Then it blows up,
// displays a textbox, and the level ends

// IF KEEN DIES before it blows up, then no textbox and levelend are performed



void AR_SpawnGenerator(int tileX, int tileY);
void AR_GeneratorCollide(CK_object *obj1, CK_object *obj2);
void AR_GeneratorExplode(CK_object *obj);
void AR_GeneratorWin(CK_object *obj);



char ck_generatorMsg1[] = 
	"That Atroxian generator\n"
	"core has been fried!\n"
	"Now to search for\n"
	"the other generator." ;
	
char ck_generatorMsg2[] = 
	"That Atroxian generator\n"
	"core has been fried!\n"
	"Now I can enter the\n"
	"Krile city in the\n"
	"northeast.\n";

char *ck_generatorMsgs[] = {
	ck_generatorMsg1,
	ck_generatorMsg2,
};
void AR_SpawnGenerator(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Generator;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8) - 0x180;
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_GeneratorIdle));
}

void AR_GeneratorCollide(CK_object *obj1, CK_object *obj2)
{

	if (obj2->type == CT_Stunner)
	{
		CK_ShotHit(obj2);
		CK_SetAction2(obj1, CK_ACTION(AR_ACT_GeneratorExplode1));
		SD_PlaySound(GENERATOREXPLODESND);
	}	
}

#if 0
void AR_GeneratorExplode(CK_object *obj)
{
	CK_object *newobj = CK_GetNewObj(true); 
	newobj->posX = obj->clipRects.unitX1 - 0x80 + ((obj->clipRects.unitX2 - obj->clipRects.unitX1) * US_RndT() >> 8);
	newobj->posY = obj->clipRects.unitY1 - 0x80 + ((obj->clipRects.unitY2 - obj->clipRects.unitY1) * US_RndT() >> 8);
	newobj->type = CT_Friendly;
	newobj->clipped = CLIP_not;
	newobj->active = OBJ_EXISTS_ONLY_ONSCREEN;
	newobj->zLayer = 2;
	CK_SetAction(newobj, CK_ACTION(AR_ACT_GeneratorExplosion1));
	SD_PlaySound(4);
}
#endif

void AR_GeneratorWin(CK_object *obj)
{
	if (ar_state.generatorsDestroyed >= 2)
	{
		ck_gameState.levelState = LS_LevelComplete;
	}
	else if (ck_keenObj->currentAction != CK_ACTION(CK_ACT_keenDie0) && ck_keenObj->currentAction != CK_ACTION(CK_ACT_keenDie1))
	{
		ck_gameState.levelState = LS_ARGenerator;
	}
}



void AR_GeneratorWinMessage (void)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_ClearMarks();	// K1n9_Duk3 fix (just in case...)
	ca_graphChunkNeeded[STARTPICS+105] |= ca_levelbit;
	ca_graphChunkNeeded[STARTPICS+106] |= ca_levelbit;
	CA_CacheMarks(0);
	VH_UpdateScreen();
	StopMusic();	// K1n9_Duk3 fix (2026-02-11): added repeat parameter

	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), 105 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_SetWindowX(US_GetWindowX() + (48));
	US_CPrint(ck_generatorMsgs[ar_state.generatorsDestroyed++]);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();

	// K1n9_Duk3 fix: stop music before re-caching the graphics to make more memory available
	StopMusic();
	CA_DownLevel();

	// K1n9_Duk3 says: scorebox sprite might have been re-cached
	// by CA_DownLevel, but the level ends after this anyway.
}

char ck_generatorMapMsg[] = 
	"I must destroy\n"
	"the generator that\n"
	"powers this\n"
	"electric barrier.";

void AR_GeneratorMapMessage (void)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_ClearMarks();	// K1n9_Duk3 fix (just in case...)
	ca_graphChunkNeeded[STARTPICS+105] |= ca_levelbit;
	CA_CacheMarks(0);
	VH_UpdateScreen();

	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), 105 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_SetWindowX(US_GetWindowX() + (48));
	US_CPrint(ck_generatorMapMsg);
	VH_UpdateScreen();
	VL_DelayTics(20);
	IN_ClearKeysDown();
	IN_WaitButton();

	CA_DownLevel();

	// K1n9_Duk3 fix: force scorebox to update (in case the sprite was re-cached by CA_DownLevel)
	ck_scoreBoxObj->user1 = -1;
	ck_scoreBoxObj->user2 = -1;
	ck_scoreBoxObj->user3 = -1;
	ck_scoreBoxObj->user4 = -1;
}
//===========================================================================
//
// KRILE ELDER
// 
//===========================================================================



void AR_SpawnElder(int tileX, int tileY);
void AR_ElderCollide(CK_object *obj1, CK_object *obj2);
void AR_ElderWinMessage(void);


#if 0
char ck_elderMsg1[] = 
	"Keen, I am the Krile's\n" 
	"elder leader. My people's\n" 
	"fate rests in your hands.\n";

char ck_elderMsg2[] = 
	"Use this orb of light\n" 
	"to brighten the crystal\n" 
	"core and save our world.\n";

char ck_elderMsg3[] =
	"Thanks. I will stop\n" 
	"Atroxus once and for all.\n";

char ck_elderMsg4[] =
	"Use this travel pad to\n" 
	"reach the Sky Sanctum.\n" 
	"Good luck!\n";

char *ck_elderMsgs[] = {
	ck_elderMsg1,
	ck_elderMsg2,
	ck_elderMsg3,
	ck_elderMsg4,
};
#endif

void AR_SpawnElder(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Elder;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_ElderIdle));
}

void AR_ElderCollide(CK_object *obj1, CK_object *obj2)
{

	if (obj2->type == CT_Player)
	{
		ck_gameState.levelState = LS_ARElder;
	}	
}


void AR_ElderWinMessage (void)
{
#if 0
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_ClearMarks();	// K1n9_Duk3 fix (just in case...)
	ca_graphChunkNeeded[STARTPICS+105] |= ca_levelbit;
	ca_graphChunkNeeded[STARTPICS+12] |= ca_levelbit;
	CA_CacheMarks(0);
	VH_UpdateScreen();
	StopMusic();	// K1n9_Duk3 fix (2026-02-11): added repeat parameter

	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), 12 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_SetWindowX(US_GetWindowX() + (48));
	US_CPrint(ck_elderMsgs[0]);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();

	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), 12 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_SetWindowX(US_GetWindowX() + (48));
	US_CPrint(ck_elderMsgs[1]);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();


	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowW()-48+US_GetWindowX(), US_GetWindowY(), 105 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_CPrint(ck_elderMsgs[2]);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();

	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), 12 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_SetWindowX(US_GetWindowX() + (48));
	US_CPrint(ck_elderMsgs[3]);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();

	// K1n9_Duk3 fix: stop music before re-caching the graphics to make more memory available
	StopMusic();
	CA_DownLevel();
#endif

	AR_EndScreens(2);
	ar_state.endscreen_played[2] = true;

}

//===========================================================================
//
// MAP BRIDGE SWITCH
// 
//===========================================================================


void AR_SpawnBridge(int tileX, int tileY);
void AR_BridgeCollide(CK_object *obj1, CK_object *obj2);
void AR_BridgeWinMessage(void);


char ck_bridgeMsg[] = 
	"Cool! This activates\n" 
	"Atroxian bridges.\n";


void AR_SpawnBridge(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = AR_CT_Bridge;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_BridgeIdle));
}

void AR_BridgeCollide(CK_object *obj1, CK_object *obj2)
{

	if (obj2->type == CT_Player)
	{
		ck_gameState.levelState = LS_ARBridge;
	}	
}

void AR_BridgeWinMessage (void)
{
	SD_WaitSoundDone();
	CA_UpLevel();
	CA_ClearMarks();	// K1n9_Duk3 fix (just in case...)
	ca_graphChunkNeeded[STARTPICS+105] |= ca_levelbit;
	CA_CacheMarks(0);
	VH_UpdateScreen();
	StopMusic();	// K1n9_Duk3 fix (2026-02-11): added repeat parameter

	// Draw Talking pic + msg
	US_CenterWindow(26,8);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY(), 105 + STARTPICS);
	US_SetPrintY(US_GetPrintY() + (6));
	US_SetWindowW(US_GetWindowW() - (48));
	US_SetWindowX(US_GetWindowX() + (48));
	US_CPrint(ck_bridgeMsg);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();

	// K1n9_Duk3 fix: stop music before re-caching the graphics to make more memory available
	StopMusic();
	CA_DownLevel();

	// K1n9_Duk3 says: scorebox sprite might have been re-cached
	// by CA_DownLevel, but the level ends after this anyway.
}
//===========================================================================
//
// KRILE BWB ELDER
// 
//===========================================================================



void AR_SpawnBWBElder(int tileX, int tileY);
void AR_ElderBWBCollide(CK_object *obj1, CK_object *obj2);



void AR_SpawnBWBElder(int tileX, int tileY)
{
	CK_object *new_object;

	if (ar_state.endscreen_played[1])
		return;

	new_object = CK_GetNewObj(false);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_ElderBWBIdle));
}

void AR_ElderBWBCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type != CT_Player)
		return;

	if (ar_state.endscreen_played[1])
		return;

	AR_EndScreens(1);
	ar_state.endscreen_played[1] = true;
	//RF_ForceRefresh();	// K1n9_Duk3 fix
	CK_RemoveObj(obj1);
	StartMusic(25);	// K1n9_Duk3 fix (2026-02-11): added repeat parameter

	// Force scorebox redraw if it's enabled
	if (ck_scoreBoxEnabled)
	{
		ck_scoreBoxObj->user1 = ck_scoreBoxObj->user2 = ck_scoreBoxObj->user3 = ck_scoreBoxObj->user4 = -1;
		// Non-vanilla addition: redraw the scorebox immediately without
		// waiting for a PlayLoop
		CK_UpdateScoreBox(ck_scoreBoxObj);
	}
	RF_ForceRefresh();	// K1n9_Duk3 fix: refresh here to avoid flicker
}

//===========================================================================
//
// Atroxus Pre Battle
// 
//===========================================================================


void AR_SpawnAtroxusDialogue(int tileX, int tileY);
void AR_AtroxusDialogueCollide(CK_object *obj1, CK_object *obj2);



void AR_SpawnAtroxusDialogue(int tileX, int tileY)
{
	CK_object *new_object;

	if (ar_state.endscreen_played[3])
		return;

	new_object = CK_GetNewObj(false);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AtroxusDialogueIdle));
}

void AR_AtroxusDialogueCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type != CT_Player)
		return;

	ck_gameState.levelState = LS_ARAtroxus;
}

//===========================================================================
//
// Secret Krile
// 
//===========================================================================



void AR_SpawnSecretKrile(int tileX, int tileY);
void AR_SecretKrileCollide(CK_object *obj1, CK_object *obj2);



void AR_SpawnSecretKrile(int tileX, int tileY)
{
	CK_object *new_object;

	if (ar_state.endscreen_played[4])
		return;

	new_object = CK_GetNewObj(false);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_SecretKrileIdle));
}

void AR_SecretKrileCollide(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type != CT_Player)
		return;

	if (ar_state.endscreen_played[4])
		return;

	AR_EndScreens(4);
	ar_state.endscreen_played[4] = true;
	//RF_ForceRefresh();	// K1n9_Duk3 fix
	CK_RemoveObj(obj1);
	StartMusic(18);	// K1n9_Duk3 fix (2026-02-11): added repeat parameter

	ar_state.doSecretEnding - true;	// K1n9_Duk3 fixme: this can't be right

	// Force scorebox redraw if it's enabled
	if (ck_scoreBoxEnabled)
	{
		ck_scoreBoxObj->user1 = ck_scoreBoxObj->user2 = ck_scoreBoxObj->user3 = ck_scoreBoxObj->user4 = -1;
		// Non-vanilla addition: redraw the scorebox immediately without
		// waiting for a PlayLoop
		CK_UpdateScoreBox(ck_scoreBoxObj);
	}
	RF_ForceRefresh();	// K1n9_Duk3 fix: refresh here to avoid flicker
}

//===========================================================================
//
// Atroxus Box
// 
//===========================================================================


void AR_SpawnAtroxusBox(int tileX, int tileY);
void AR_AtroxusBoxCollide(CK_object *obj1, CK_object *obj2);


int atroxus_endscreens[] = {9, 10, 11};

void AR_SpawnAtroxusBox(int tileX, int tileY)
{
	int i;
	CK_object *new_object;

	if (ar_state.atroxusBoxesPlayed >= 3)
		return;

	for (i = 0; i < 3; i++)
		if (ar_state.atroxusBoxesLevels[i] == ck_gameState.currentLevel)
			return;

	new_object = CK_GetNewObj(false);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_ACTIVE;
	new_object->zLayer = 0;
	new_object->posX = tileX << 8;
	new_object->posY = (tileY << 8);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(AR_ACT_AtroxusBoxIdle));
}

void AR_AtroxusBoxCollide(CK_object *obj1, CK_object *obj2)
{

	if (obj2->type != CT_Player)
		return;

	ar_state.atroxusBoxesLevels[ar_state.atroxusBoxesPlayed] = ck_gameState.currentLevel;
	AR_EndScreens(atroxus_endscreens[ar_state.atroxusBoxesPlayed++]);

	//RF_ForceRefresh();	// K1n9_Duk3 fix
	CK_RemoveObj(obj1);
	StartMusic(ck_gameState.currentLevel);	// K1n9_Duk3 fix (2026-02-11): added repeat parameter

	// Force scorebox redraw if it's enabled
	if (ck_scoreBoxEnabled)
	{
		ck_scoreBoxObj->user1 = ck_scoreBoxObj->user2 = ck_scoreBoxObj->user3 = ck_scoreBoxObj->user4 = -1;
		// Non-vanilla addition: redraw the scorebox immediately without
		// waiting for a PlayLoop
		CK_UpdateScoreBox(ck_scoreBoxObj);
	}
	RF_ForceRefresh();	// K1n9_Duk3 fix: refresh here to avoid flicker
}

void AR_Act2_SetupFunctions(void)
{
	CK_ACT_AddFunction("AR_AtroxianBasicReact", &AR_AtroxianBasicReact);
	CK_ACT_AddFunction("AR_AtroxianExploding", &AR_AtroxianExploding);
	CK_ACT_AddFunction("AR_AtroxianExplodingReact", &AR_AtroxianExplodingReact);
	CK_ACT_AddFunction("AR_AtroxianFly", &AR_AtroxianFly);
	CK_ACT_AddFunction("AR_AtroxianFlyingReact", &AR_AtroxianFlyingReact);
	CK_ACT_AddFunction("AR_AtroxianPreWalkReact", &AR_AtroxianPreWalkReact);
	CK_ACT_AddFunction("AR_AtroxianRecoverToFlying", &AR_AtroxianRecoverToFlying);
	CK_ACT_AddFunction("AR_AtroxianStunnedReact", &AR_AtroxianStunnedReact);
	CK_ACT_AddFunction("AR_AtroxianWalk", &AR_AtroxianWalk);
	CK_ACT_AddFunction("AR_AtroxianWalkingReact", &AR_AtroxianWalkingReact);
	CK_ACT_AddFunction("AR_AtroxiteAttack", &AR_AtroxiteAttack);
	CK_ACT_AddFunction("AR_AtroxiteJumpReact", &AR_AtroxiteJumpReact);
	CK_ACT_AddFunction("AR_AtroxiteMove", &AR_AtroxiteMove);
	CK_ACT_AddFunction("AR_AxoclawReact", &AR_AxoclawReact);
	CK_ACT_AddFunction("AR_AxoclawShoot", &AR_AxoclawShoot);
	CK_ACT_AddFunction("AR_AxoclawShot", &AR_AxoclawShot);
	CK_ACT_AddFunction("AR_AxoclawShotReact", &AR_AxoclawShotReact);
	CK_ACT_AddFunction("AR_AxoclawThink", &AR_AxoclawThink);
	CK_ACT_AddFunction("AR_AxosisStunCount", &AR_AxosisStunCount);
	CK_ACT_AddFunction("AR_AxosisWalk", &AR_AxosisWalk);
	CK_ACT_AddFunction("AR_GeneratorWin", &AR_GeneratorWin);
	CK_ACT_AddFunction("AR_KrileGhostFindAxosis", &AR_KrileGhostFindAxosis);
	CK_ACT_AddFunction("AR_KrileGhostHide", &AR_KrileGhostHide);
	CK_ACT_AddFunction("AR_KrileGhostHunt", &AR_KrileGhostHunt);
	CK_ACT_AddFunction("AR_KrileGhostRoamReact", &AR_KrileGhostRoamReact);
	CK_ACT_AddFunction("AR_KrileGuardianReact", &AR_KrileGuardianReact);
	CK_ACT_AddFunction("AR_KrileGuardianShoot", &AR_KrileGuardianShoot);
	CK_ACT_AddFunction("AR_KrileGuardianShotReact", &AR_KrileGuardianShotReact);
	CK_ACT_AddFunction("AR_KrileGuardianWalk", &AR_KrileGuardianWalk);
	CK_ACT_AddFunction("AR_MoxelShotBurnout", &AR_MoxelShotBurnout);
	CK_ACT_AddFunction("AR_MoxelShotReact", &AR_MoxelShotReact);
	CK_ACT_AddFunction("AR_MoxelThink", &AR_MoxelThink);
	CK_ACT_AddFunction("AR_MoxelWaitToShoot", &AR_MoxelWaitToShoot);
	CK_ACT_AddColFunction("AR_AtroxianFlyingCollide", &AR_AtroxianFlyingCollide);
	CK_ACT_AddColFunction("AR_AtroxianStunnedCollide", &AR_AtroxianStunnedCollide);
	CK_ACT_AddColFunction("AR_AtroxianWalkingCollide", &AR_AtroxianWalkingCollide);
	CK_ACT_AddColFunction("AR_AtroxiteCollide", &AR_AtroxiteCollide);
	CK_ACT_AddColFunction("AR_AtroxusBoxCollide", &AR_AtroxusBoxCollide);
	CK_ACT_AddColFunction("AR_AtroxusDialogueCollide", &AR_AtroxusDialogueCollide);
	CK_ACT_AddColFunction("AR_AxosisCollide", &AR_AxosisCollide);
	CK_ACT_AddColFunction("AR_AxosisStunnedCollide", &AR_AxosisStunnedCollide);
	CK_ACT_AddColFunction("AR_BridgeCollide", &AR_BridgeCollide);
	CK_ACT_AddColFunction("AR_ElderBWBCollide", &AR_ElderBWBCollide);
	CK_ACT_AddColFunction("AR_ElderCollide", &AR_ElderCollide);
	CK_ACT_AddColFunction("AR_GeneratorCollide", &AR_GeneratorCollide);
	CK_ACT_AddColFunction("AR_KrileGhostHuntCollide", &AR_KrileGhostHuntCollide);
	CK_ACT_AddColFunction("AR_SecretKrileCollide", &AR_SecretKrileCollide);
}
