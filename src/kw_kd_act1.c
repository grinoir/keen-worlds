#include "kw_dreams.h"

/*
Keen Worlds: Keen Dreams - shared movement, bonuses, and the first vegetables
Converted from KD_ACT1.C of the Keen Dreams source (id Software 1991; GPL release
2014) by tools/kw_dreams_port.py, then ported by hand to Omnispeak.
Licensed under the GNU GPL v2 or later.
*/

/* Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
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

// KD_ACT1.C


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


#define PLACESPRITE RF_AddSpriteDraw (&ob->sde,ob->posX,ob->posY,ob->gfxChunk, \
	false,0);

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

int16_t	flowertime[4] = {700,700,350,175};

/*
=============================================================================

						MISC ACTOR STUFF

=============================================================================
*/

/*
==================
=
= DoGravity
=
= Changes speed and location
=
==================
*/


void KD_DoGravity (CK_object *ob)
{
	int32_t	i;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=SD_GetLastTimeCount()-SD_GetSpriteSync();i<SD_GetLastTimeCount();i++)
	{
		if (i&1)
		{
			if (ob->velY < 0 && ob->velY >= -ACCGRAVITY)
			{
			// stop at apex of jump
				ck_nextY += ob->velY;
				ob->velY = 0;
				return;
			}
			ob->velY+=ACCGRAVITY;
			if (ob->velY>SPDMAXY)
			  ob->velY=SPDMAXY;
		}
		ck_nextY+=ob->velY;
	}
}


/*
===============
=
= AccelerateX
=
===============
*/

void KD_AccelerateX (CK_object *ob,int16_t dir,int16_t max)
{
	int32_t	i;
	uint16_t	olddir;

	olddir = ob->velX & 0x8000;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=SD_GetLastTimeCount()-SD_GetSpriteSync();i<SD_GetLastTimeCount();i++)
	{
		if (i&1)
		{
			ob->velX+=dir;
			if ( (ob->velX & 0x8000) != olddir)
			{
				olddir = ob->velX & 0x8000;
				ob->xDirection = olddir ? -1 : 1;
			}
			if (ob->velX>max)
			  ob->velX=max;
			else if (ob->velX<-max)
			  ob->velX=-max;
		}
		ck_nextX+=ob->velX;
	}
}


/*
===============
=
= FrictionX
=
===============
*/

void KD_FrictionX (CK_object *ob)
{
	int32_t	i;
	int16_t		dir;
	uint16_t	olddir;

	olddir = ob->velX & 0x8000;

	if (ob->velX > 0)
		dir = -1;
	else if (ob->velX < 0)
		dir = 1;
	else
		dir = 0;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=SD_GetLastTimeCount()-SD_GetSpriteSync();i<SD_GetLastTimeCount();i++)
	{
		if (i&1)
		{
			ob->velX+=dir;
			if ( (ob->velX & 0x8000) != olddir)
				ob->velX = 0;
		}
		ck_nextX+=ob->velX;
	}
}


/*
===============
=
= ProjectileThink
=
===============
*/

void KD_ProjectileThink (CK_object *ob)
{
	KD_DoGravity (ob);
	ck_nextX = SD_GetSpriteSync()*ob->velX;
}

/*
===============
=
= VelocityThink
=
===============
*/

void KD_VelocityThink (CK_object *ob)
{
	ck_nextX = SD_GetSpriteSync()*ob->velX;
	ck_nextY = SD_GetSpriteSync()*ob->velY;
}

/*
===============
=
= DrawReact
=
===============
*/

void KD_DrawReact (CK_object *ob)
{
	RF_AddSpriteDraw (&ob->sde,ob->posX,ob->posY,ob->gfxChunk,false,1);
}

void KD_DrawReact2 (CK_object *ob)
{
	RF_AddSpriteDraw (&ob->sde,ob->posX,ob->posY,ob->gfxChunk,false,2);
}

void KD_DrawReact3 (CK_object *ob)
{
	RF_AddSpriteDraw (&ob->sde,ob->posX,ob->posY,ob->gfxChunk,false,3);
}

// (Its ChangeState is the engine's CK_SetAction2.)


/*
====================
=
= WalkReact
=
====================
*/

void KD_WalkReact (CK_object *ob)
{
	if (ob->xDirection == 1 && ob->leftTI)
	{
		ob->posX -= ob->deltaPosX;
		ob->xDirection = -1;
		ob->timeUntillThink = US_RndT()>>5;
		CK_SetAction2 (ob,ob->currentAction);
	}
	else if (ob->xDirection == -1 && ob->rightTI)
	{
		ob->posX -= ob->deltaPosX;
		ob->xDirection = 1;
		ob->timeUntillThink = US_RndT()>>5;
		CK_SetAction2 (ob,ob->currentAction);
	}
	else if (!ob->topTI)
	{
		ob->posX -= 2*ob->deltaPosX;
		ob->posY -= ob->deltaPosY;

		ob->xDirection = -ob->xDirection;
		ob->timeUntillThink = US_RndT()>>5;
		CK_SetAction2 (ob,ob->currentAction);
	}

	PLACESPRITE;
}


/*
=============================================================================

							DOOR

=============================================================================
*/

void	KD_DoorContact (CK_object *ob, CK_object *hit);



/*
======================
=
= SpawnDoor
=
======================
*/

void	KD_SpawnDoor (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_doorobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	kd_new->yDirection = -1;
	kd_new->clipped = CLIP_not;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_door));
}

/*
======================
=
= DoorContact
=
======================
*/

void	KD_DoorContact (CK_object *ob, CK_object *hit)
{
	CK_PhysPushX (hit,ob);
}

/*
=============================================================================

							FLOWER

temp1 = original class
temp2 = original state
temp3 = flower count

=============================================================================
*/

void KD_ChangeToFlower (CK_object *ob);
void KD_FlowerThink (CK_object *ob);
void KD_ChangeFromFlower (CK_object *ob);












/*
======================
=
= ChangeToFlower
=
======================
*/

void KD_ChangeToFlower (CK_object *ob)
{
	SD_PlaySound (KD_FLOWERPOWERSND);
	ob->posY = ob->clipRects.unitY2-256*2;
	ob->user1 = ob->type;
	ob->user2 = ob->currentAction->compatDosPointer;
	ob->user3 = 0;
	ob->clipped = CLIP_normal;
	ob->type = KD_inertobj;
	ob->velX = 0;
	CK_SetAction2 (ob,CK_ACTION(KD_ACT_flower1));
	ob->active = OBJ_ALWAYS_ACTIVE;			// flower never deactivated
	KD_GetNewObj (true);
	kd_new->posX = ob->posX;
	kd_new->posY = ob->posY;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_poofto1));
	kd_new->active = OBJ_EXISTS_ONLY_ONSCREEN;
}


/*
======================
=
= FlowerThink
=
======================
*/

void KD_FlowerThink (CK_object *ob)
{
	KD_ProjectileThink (ob);
	if ( (ob->user3+=SD_GetSpriteSync()) >= flowertime[ck_gameState.difficulty])
	{
		KD_GetNewObj (true);
		kd_new->active = OBJ_ALWAYS_ACTIVE;
		kd_new->user1 = CK_ConvertObjPointerTo16BitOffset(ob);
		kd_new->posX = ob->posX;
		kd_new->posY = ob->posY;
		KD_NewState (kd_new,CK_ACTION(KD_ACT_pooffrom1));
		ob->user3 = 0;
	}
}


/*
======================
=
= ChangeFromFlower
=
======================
*/

void KD_ChangeFromFlower (CK_object *ob)
{
	CK_object *flower;
	CK_action *state;
	uint16_t	oldbottom;

	SD_PlaySound (KD_UNFLOWERPOWERSND);
	flower = CK_ConvertObj16BitOffsetToPointer(ob->user1);

	oldbottom = flower->clipRects.unitY2;
	CK_SetAction2 (flower,CK_LookupActionFrom16BitOffset((uint16_t)flower->user2));
	flower->posY += oldbottom - flower->clipRects.unitY2;

	flower->type = flower->user1;
	flower->active = OBJ_ACTIVE;			// allow it to unspawn now if off screen
}


/*
=============================================================================

							BONUS

temp1 = bonus type
temp2 = base shape number
temp3 = last animated shape number +1

=============================================================================
*/

void KD_BonusThink (CK_object *ob);






/*
====================
=
= SpawnBonus
=
====================
*/

static int16_t bonusshape[12] = {KD_PEPPERMINT1SPR,KD_COOKIE1SPR,KD_CANDYCANE1SPR,KD_CANDYBAR1SPR,
	KD_LOLLIPOP1SPR,KD_COTTONCANDY1SPR,KD_EXTRAKEEN1SPR,KD_SUPERBONUS1SPR,KD_FLOWERPOWER1SPR,
	KD_FLOWERPOWERUP1SPR,KD_BOOBUSBOMB1SPR,KD_MAGICKEY1SPR};

void KD_SpawnBonus (int16_t tilex, int16_t tiley, int16_t type)
{
	KD_GetNewObj (false);

	kd_new->clipped = CLIP_not;
	kd_new->type = KD_bonusobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = tiley<<8;

	if (type == 9)
		kd_new->posY -= 8*16;	// flower power up one block

	kd_new->yDirection = -1;			// bonus stuff flies up when touched

	kd_new->user1 = type;
	kd_new->user2 = kd_new->gfxChunk = bonusshape[type];
	if (type != 7)
		kd_new->user3 = kd_new->user2 + 2;
	else
		kd_new->user3 = kd_new->user2 + 4;	// super bonus is 4 stage animation

	KD_NewState (kd_new,CK_ACTION(KD_ACT_bonus));
}

/*
====================
=
= BonusThink
=
====================
*/

void KD_BonusThink (CK_object *ob)
{
	if (++ob->gfxChunk == ob->user3)
		ob->gfxChunk = ob->user2;
}



/*
=============================================================================

						BROCCOLASH

=============================================================================
*/

void KD_BroccoThink (CK_object *ob);
void KD_BroccoGetUp (CK_object *ob);







/*
====================
=
= SpawnBrocco
=
====================
*/

void KD_SpawnBrocco (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_broccoobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_broccowalk1));
}

/*
====================
=
= BroccoGetUp
=
====================
*/

void KD_BroccoGetUp (CK_object *ob)
{
	ob->clipped = CLIP_normal;
}


/*
====================
=
= BroccoThink
=
====================
*/

void KD_BroccoThink (CK_object *ob)
{
	int16_t delta;

	if (ob->clipRects.unitY1 > ck_keenObj->clipRects.unitY2 || ob->clipRects.unitY2 < ck_keenObj->clipRects.unitY1)
		return;

	delta = ck_keenObj->posX - ob->posX;

	if ( ob->xDirection == -1 )
	{
		if (delta < -3*256)
			return;
		if (delta > 256/2)
		{
			ob->xDirection = 1;
			return;
		}
		ob->currentAction = CK_ACTION(KD_ACT_broccosmash1);
		ob->clipped = CLIP_not;
		ck_nextX = 0;
		return;
	}
	else
	{
		delta = ck_keenObj->clipRects.unitX1 - ob->clipRects.unitX2;
		if (delta > 3*256)
			return;
		if (delta < -256/2)
		{
			ob->xDirection = -1;
			return;
		}
		ob->currentAction = CK_ACTION(KD_ACT_broccosmash1);
		ob->clipped = CLIP_not;
		ck_nextX = 0;
		return;
	}
}


/*
=============================================================================

						 TOMATOOTH

ob->user1 = jumptime

=============================================================================
*/

#define	SPDTOMATBOUNCE	30
#define TICTOMATJUMP	10
#define SPDTOMAT		16

void KD_TomatBounceThink (CK_object *ob);
void KD_TomatReact (CK_object *ob);





/*
====================
=
= SpawnTomat
=
====================
*/

void KD_SpawnTomat (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_tomatobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-1*BLOCKSIZE;
	kd_new->xDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_tomatbounce));
}

/*
====================
=
= TomatBounceThink
=
====================
*/

void KD_TomatBounceThink (CK_object *ob)
{
	KD_AccelerateX (ob,ob->posX > ck_keenObj->posX ? -1 : 1,SPDTOMAT);
	if (ob->velX > 0)
		ob->xDirection = 1;
	else
		ob->xDirection = -1;

	if (ob->user1)
	{
		if (ob->user1<SD_GetSpriteSync())
		{
			ck_nextY = ob->velY*ob->user1;
			ob->user1 = 0;
		}
		else
		{
			ck_nextY = ob->velY*SD_GetSpriteSync();
			ob->user1-=SD_GetSpriteSync();
		}
	}
	else
		KD_DoGravity(ob);

}


/*
====================
=
= TomatReactThink
=
====================
*/

void KD_TomatReact (CK_object *ob)
{
	if (ob->rightTI || ob->leftTI)
	{
		ob->xDirection = -ob->xDirection;
		ob->velX = -ob->velX;
	}

	if (ob->bottomTI)
	{
		if (ob->clipRects.tileX2 >= KD_ORIGINXTILE
		&& ob->clipRects.tileX1 <= KD_ORIGINXTILEMAX
		&& ob->clipRects.tileY1 >= KD_ORIGINYTILE
		&& ob->clipRects.tileY2 <= KD_ORIGINYTILEMAX)
			SD_PlaySound (KD_BOUNCESND);
		ob->velY = -ob->velY;
	}

	if (ob->topTI)
	{
		if (ob->clipRects.tileX2 >= KD_ORIGINXTILE
		&& ob->clipRects.tileX1 <= KD_ORIGINXTILEMAX
		&& ob->clipRects.tileY1 >= KD_ORIGINYTILE
		&& ob->clipRects.tileY2 <= KD_ORIGINYTILEMAX)
			SD_PlaySound (KD_BOUNCESND);
		ob->velY = -SPDTOMATBOUNCE-(US_RndT()>>4);
		ob->user1 = TICTOMATJUMP;
	}

	PLACESPRITE;
}


/*
=============================================================================

						 CARROT COURIER

=============================================================================
*/

#define	SPDCARROTLEAPX	32
#define SPDCARROTLEAPY  40

void CarrotThink (CK_object *ob);
void KD_CarrotReact (CK_object *ob);
void KD_CarrotAirReact (CK_object *ob);







/*
====================
=
= SpawnCarrot
=
====================
*/

void KD_SpawnCarrot (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_carrotobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_carrotwalk1));
	kd_new->topTI = 1;
}


/*
====================
=
= CarrotReact
=
====================
*/

void KD_CarrotReact (CK_object *ob)
{
	uint16_t x, width, bot,  *map;

	if (ob->xDirection == 1 && ob->leftTI)
	{
		ob->xDirection = -1;
	}
	else if (ob->xDirection == -1 && ob->rightTI)
	{
		ob->xDirection = 1;
	}
	else if (!ob->topTI)
	{
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;

		ob->velY = -SPDCARROTLEAPY;
		ob->velX = SPDCARROTLEAPX*ob->xDirection;
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_carrotleap));
	}

	PLACESPRITE;
}


/*
====================
=
= CarrotAirReact
=
====================
*/

void KD_CarrotAirReact (CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;

	if (ob->topTI)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_carrotwalk1));

	PLACESPRITE;
}



/*
=============================================================================

						   ASPARAGUSTO

=============================================================================
*/

void AsparThink (CK_object *ob);





/*
====================
=
= SpawnAspar
=
====================
*/

void KD_SpawnAspar (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_asparobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = tiley<<8;
	kd_new->xDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_asparwalk1));
}


/*
=============================================================================

						   SOUR GRAPE

=============================================================================
*/

void KD_GrapeThink (CK_object *ob);
void KD_GrapeRiseReact (CK_object *ob);
void KD_GrapeFallReact (CK_object *ob);







/*
====================
=
= SpawnGrape
=
====================
*/

void KD_SpawnGrape (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_grapeobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = tiley<<8;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_grapewait));
}


/*
====================
=
= GrapeThink
=
====================
*/

void KD_GrapeThink (CK_object *ob)
{
	uint16_t y,starty,endy,  *map;

	if (ck_keenObj->clipRects.unitX1 > ob->clipRects.unitX2
		|| ck_keenObj->clipRects.unitX2 < ob->clipRects.unitX1
		|| ck_keenObj->posY < ob->posY )
		return;

//
// see if there are any walls between grape and player
//
	starty = ob->clipRects.tileY2;
	endy = ck_keenObj->clipRects.tileY1;

	map = CA_mapPlanes[1] + ((starty)*CA_GetMapWidth()) + ob->clipRects.tileXmid;
	for (y = starty ; y<endy ; y++,map+=CA_GetMapWidth())
		if (KD_NORTHWALL(*map))
			return;

	ob->currentAction = CK_ACTION(KD_ACT_grapefall);
	SD_PlaySound (KD_GRAPESCREAMSND);

}


/*
====================
=
= GrapeRiseReact
=
====================
*/

void KD_GrapeRiseReact (CK_object *ob)
{
	if (ob->bottomTI)
		CK_SetAction2(ob,CK_ACTION(KD_ACT_grapewait));
	PLACESPRITE;
}


/*
====================
=
= GrapeFallReact
=
====================
*/

void KD_GrapeFallReact (CK_object *ob)
{
	if (ob->topTI)
	{
		SD_PlaySound (KD_BOUNCESND);
		ob->velY = -(ob->velY<<1)/3;
		if (ob->velY > -32)
			CK_SetAction2(ob,CK_ACTION(KD_ACT_grapesit));
	}
	PLACESPRITE;
}


void KD_Act1_SetupFunctions(void)
{
	CK_ACT_AddFunction("KD_BonusThink", &KD_BonusThink);
	CK_ACT_AddFunction("KD_BroccoGetUp", &KD_BroccoGetUp);
	CK_ACT_AddFunction("KD_BroccoThink", &KD_BroccoThink);
	CK_ACT_AddFunction("KD_CarrotAirReact", &KD_CarrotAirReact);
	CK_ACT_AddFunction("KD_CarrotReact", &KD_CarrotReact);
	CK_ACT_AddFunction("KD_ChangeFromFlower", &KD_ChangeFromFlower);
	CK_ACT_AddFunction("KD_ChangeToFlower", &KD_ChangeToFlower);
	CK_ACT_AddFunction("KD_DoGravity", &KD_DoGravity);
	CK_ACT_AddFunction("KD_DrawReact", &KD_DrawReact);
	CK_ACT_AddFunction("KD_DrawReact2", &KD_DrawReact2);
	CK_ACT_AddFunction("KD_DrawReact3", &KD_DrawReact3);
	CK_ACT_AddFunction("KD_FlowerThink", &KD_FlowerThink);
	CK_ACT_AddFunction("KD_FrictionX", &KD_FrictionX);
	CK_ACT_AddFunction("KD_GrapeFallReact", &KD_GrapeFallReact);
	CK_ACT_AddFunction("KD_GrapeRiseReact", &KD_GrapeRiseReact);
	CK_ACT_AddFunction("KD_GrapeThink", &KD_GrapeThink);
	CK_ACT_AddFunction("KD_ProjectileThink", &KD_ProjectileThink);
	CK_ACT_AddFunction("KD_TomatBounceThink", &KD_TomatBounceThink);
	CK_ACT_AddFunction("KD_TomatReact", &KD_TomatReact);
	CK_ACT_AddFunction("KD_VelocityThink", &KD_VelocityThink);
	CK_ACT_AddFunction("KD_WalkReact", &KD_WalkReact);
	CK_ACT_AddColFunction("KD_DoorContact", &KD_DoorContact);
}
