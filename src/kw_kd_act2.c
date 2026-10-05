#include "kw_dreams.h"

/*
Keen Worlds: Keen Dreams - the rest of the vegetables and Boobus Tuber
Converted from KD_ACT2.C of the Keen Dreams source (id Software 1991; GPL release
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


void	KD_ProjectileReact (CK_object *ob);

/*
=============================================================================

						   TATER TROOPER

temp1 = chasing player
temp2 = nothink time

=============================================================================
*/

void KD_TaterThink (CK_object *ob);
void KD_BackupReact (CK_object *ob);








/*
====================
=
= SpawnTater
=
====================
*/

void KD_SpawnTater (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_taterobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE + 15;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_taterwalk1));
	kd_new->topTI = 1;
}

/*
====================
=
= TaterThink
=
====================
*/

void KD_TaterThink (CK_object *ob)
{
	int16_t delta;

	if (ob->clipRects.unitY1 > ck_keenObj->clipRects.unitY2 || ob->clipRects.unitY2 < ck_keenObj->clipRects.unitY1)
		return;

	if ( ob->xDirection == -1 )
	{
		delta = ob->clipRects.unitX1 - ck_keenObj->clipRects.unitX2;
		if (delta > 256)
			return;
		if (delta < -8*16)
		{
			ob->xDirection = 1;
			return;
		}
		SD_PlaySound (KD_TATERSWINGSND);
		ob->currentAction = CK_ACTION(KD_ACT_taterattack1);
		return;
	}
	else
	{
		delta = ck_keenObj->clipRects.unitX1 - ob->clipRects.unitX2;
		if (delta > 256)
			return;
		if (delta < -8*16)
		{
			ob->xDirection = -1;
			return;
		}
		SD_PlaySound (KD_TATERSWINGSND);
		ob->currentAction = CK_ACTION(KD_ACT_taterattack1);
		return;
	}
}


/*
====================
=
= BackupReact
=
====================
*/

void KD_BackupReact (CK_object *ob)
{
	if (!ob->topTI)
	{
		ob->posX-=ob->deltaPosX;
		ob->posY-=ob->deltaPosY;
	}

	PLACESPRITE;
}

/*
=============================================================================

						   CANTELOUPE CART

=============================================================================
*/


void KD_CartReact (CK_object *ob);




/*
====================
=
= SpawnCart
=
====================
*/

void KD_SpawnCart (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_cartobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-3*16;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	kd_new->active = OBJ_ALWAYS_ACTIVE;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_cartroll1));
}



/*
====================
=
= CartReact
=
====================
*/

void KD_CartReact (CK_object *ob)
{
	uint16_t  *map;

	if (ob->xDirection == 1 && ob->leftTI)
	{
		ob->xDirection = -1;
	}
	else if (ob->xDirection == -1 && ob->rightTI)
	{
		ob->xDirection = 1;
	}

	map = CA_mapPlanes[1] + ((ob->clipRects.tileY2+1)*CA_GetMapWidth());
	if (ob->xDirection == 1)
		map += ob->clipRects.tileX2;
	else
		map += ob->clipRects.tileX1;

	if ( !KD_NORTHWALL(*map) )
		ob->xDirection = -ob->xDirection;

	PLACESPRITE;
}


/*
=============================================================================

							FRENCHY

=============================================================================
*/

#define FRYXSPEED	40
#define FRYYSPEED	-20


void KD_FrenchyThink (CK_object *ob);
void KD_FrenchyRunThink (CK_object *ob);
void KD_FrenchyThrow (CK_object *ob);













/*
====================
=
= SpawnFrenchy
=
====================
*/

void KD_SpawnFrenchy (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_frenchyobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_frenchywalk1));
}


/*
====================
=
= FrenchyRunThink
=
====================
*/

void KD_FrenchyRunThink (CK_object *ob)
{
	ob->currentAction = CK_ACTION(KD_ACT_frenchywalk1);
}


/*
====================
=
= FrenchyThrow
=
====================
*/

void KD_FrenchyThrow (CK_object *ob)
{
	KD_GetNewObj (true);
	kd_new->type = KD_shotobj;
	if (ob->xDirection == 1)
	{
		kd_new->posX = ob->posX+24*16;
		kd_new->posY = ob->posY+8*16;
	}
	else
	{
		kd_new->posX = ob->posX;
		kd_new->posY = ob->posY+8*16;
	}
	kd_new->xDirection = ob->xDirection;
	kd_new->yDirection = 1;
	kd_new->velX = ob->xDirection * FRYXSPEED-(US_RndT()>>4);
	kd_new->velY = FRYYSPEED;
	kd_new->active = OBJ_EXISTS_ONLY_ONSCREEN;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_fry1));

	ob->timeUntillThink = 2;
}


/*
====================
=
= FrenchyThink
=
====================
*/

void KD_FrenchyThink (CK_object *ob)
{
	int16_t delta;

	if ( abs(ob->posY - ck_keenObj->posY) > 3*256 )
	{
		if (US_RndT()<8)
			ob->xDirection = -ob->xDirection;		// turn randomly
		return;
	}

	delta = ck_keenObj->posX - ob->posX;

	if (delta < -8*256)
	{
	// walk closer
		ob->xDirection = -1;
	}
	if (delta < -4*256)
	{
	// throw
		ob->xDirection = -1;
		ob->currentAction = CK_ACTION(KD_ACT_frenchythrow1);
	}
	else if (delta < 0)
	{
	// run away
		ob->xDirection = 1;
		ob->currentAction = CK_ACTION(KD_ACT_frenchyrun1);
		ob->timeUntillThink = 8;
	}
	else if (delta < 4*256)
	{
	// run away
		ob->xDirection = -1;
		ob->currentAction = CK_ACTION(KD_ACT_frenchyrun1);
		ob->timeUntillThink = 8;
	}
	else if (delta < 8*256)
	{
	// throw and walk closer
		ob->xDirection = 1;
		ob->currentAction = CK_ACTION(KD_ACT_frenchythrow1);
	}
	else
	{
	// walk closer
		ob->xDirection = 1;
	}
}


/*
=============================================================================

						  MELON LIPS

ob->user1 = direction : 0 = left, 1 = right, 2 = down

=============================================================================
*/

#define SPITXSPEED	48
#define SPITYSPEED	-20

void KD_MelonSpitThink (CK_object *ob);
void	KD_ProjectileReact (CK_object *ob);











/*
====================
=
= SpawnMelon
=
====================
*/

void KD_SpawnMelon (int16_t tilex, int16_t tiley,int16_t dir)
{
	KD_GetNewObj (false);

	kd_new->type = KD_melonobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = tiley<<8;
	if (dir)
		kd_new->xDirection = 1;
	else
		kd_new->xDirection = -1;
	if (dir <2)
		KD_NewState (kd_new,CK_ACTION(KD_ACT_melonside));
	else
		KD_NewState (kd_new,CK_ACTION(KD_ACT_melondown));

	kd_new->actionTimer = US_RndT()>>1;
	kd_new->user1 = dir;
}


/*
====================
=
= MelonSpitThink
=
====================
*/

void KD_MelonSpitThink (CK_object *ob)
{
	KD_GetNewObj (false);
	kd_new->type = KD_shotobj;
	switch (ob->user1)
	{
	case 0:
		kd_new->posX = ob->posX+24*16;
		kd_new->posY = ob->posY+8*16;
		kd_new->xDirection = ob->xDirection;
		kd_new->yDirection = 1;
		kd_new->velX = -SPITXSPEED-(US_RndT()>>4);
		kd_new->velY = SPITYSPEED;
		KD_NewState (kd_new,CK_ACTION(KD_ACT_melonseed1));
		break;
	case 1:
		kd_new->posX = ob->posX;
		kd_new->posY = ob->posY+8*16;
		kd_new->xDirection = ob->xDirection;
		kd_new->yDirection = 1;
		kd_new->velX = SPITXSPEED+(US_RndT()>>4);
		kd_new->velY = SPITYSPEED;
		KD_NewState (kd_new,CK_ACTION(KD_ACT_melonseed1));
		break;
	case 2:
		kd_new->posX = ob->posX+8*16;
		kd_new->posY = ob->posY+24*16;
		kd_new->yDirection = 1;
		kd_new->velY = -SPITYSPEED;
		KD_NewState (kd_new,CK_ACTION(KD_ACT_melonseedd1));
		break;
	}

	kd_new->active = OBJ_EXISTS_ONLY_ONSCREEN;
}

/*
============================
=
= ProjectileReact
=
============================
*/

void	KD_ProjectileReact (CK_object *ob)
{
	uint16_t wall,absx,absy,angle,newangle;
	uint32_t speed;

	PLACESPRITE;
	if (ob->rightTI || ob->leftTI)
		ob->velX= -ob->velX/2;

	if (ob->bottomTI)
		ob->velY= -ob->velY/2;

	wall = ob->topTI;
	if (wall)
	{
		if (ob->velY < 0)
			ob->velY = 0;

		absx = abs(ob->velX);
		absy = ob->velY;
		if (absx>absy)
		{
			if (absx>absy*2)	// 22 degrees
			{
				angle = 0;
				speed = absx*286;	// x*sqrt(5)/2
			}
			else				// 45 degrees
			{
				angle = 1;
				speed = absx*362;	// x*sqrt(2)
			}
		}
		else
		{
			if (absy>absx*2)	// 90 degrees
			{
				angle = 3;
				speed = absy*256;
			}
			else
			{
				angle = 2;		// 67 degrees
				speed = absy*286;	// y*sqrt(5)/2
			}
		}
		if (ob->velX > 0)
			angle = 7-angle;

		speed >>= 1;
		newangle = kd_bounceangle[ob->topTI][angle];
		switch (newangle)
		{
		case 0:
			ob->velX = speed / 286;
			ob->velY = -ob->velX / 2;
			break;
		case 1:
			ob->velX = speed / 362;
			ob->velY = -ob->velX;
			break;
		case 2:
			ob->velY = -(speed / 286);
			ob->velX = -ob->velY / 2;
			break;
		case 3:

		case 4:
			ob->velX = 0;
			ob->velY = -(speed / 256);
			break;
		case 5:
			ob->velY = -(speed / 286);
			ob->velX = ob->velY / 2;
			break;
		case 6:
			ob->velX = ob->velY = -(speed / 362);
			break;
		case 7:
			ob->velX = -(speed / 286);
			ob->velY = ob->velX / 2;
			break;

		case 8:
			ob->velX = -(speed / 286);
			ob->velY = -ob->velX / 2;
			break;
		case 9:
			ob->velX = -(speed / 362);
			ob->velY = -ob->velX;
			break;
		case 10:
			ob->velY = speed / 286;
			ob->velX = -ob->velY / 2;
			break;
		case 11:

		case 12:
			ob->velX = 0;
			ob->velY = -(speed / 256);
			break;
		case 13:
			ob->velY = speed / 286;
			ob->velX = ob->velY / 2;
			break;
		case 14:
			ob->velX = speed / 362;
			ob->velY = speed / 362;
			break;
		case 15:
			ob->velX = speed / 286;
			ob->velY = ob->velX / 2;
			break;
		}

		if (speed < 256*16)
			CK_RemoveObj (ob);
	}
}

/*
=============================================================================

							SQUASHER

=============================================================================
*/

#define SPDSQUASHLEAPY  50


void KD_SquasherThink (CK_object *ob);
void KD_SquasherJumpReact (CK_object *ob);









/*
====================
=
= SpawnSquasher
=
====================
*/

void KD_SpawnSquasher (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_squashobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_squasherwalk1));
}

/*
====================
=
= SquasherThink
=
====================
*/

void KD_SquasherThink (CK_object *ob)
{
	int16_t delta;

	if ( abs(ob->posY - ck_keenObj->posY) > 3*256 )
	{
		if (US_RndT()<8)
			ob->xDirection = -ob->xDirection;		// turn randomly
		return;
	}


	delta = ck_keenObj->posX - ob->posX;

	if ( ob->xDirection == -1 )
	{
		if (delta < -6*256)
			return;
		if (delta > 8*16)
		{
			ob->xDirection = 1;
			return;
		}
	}
	else
	{
		if (delta > 6*256)
			return;
		if (delta < -8*16)
		{
			ob->xDirection = 1;
			return;
		}
	}

	ob->velY = -SPDSQUASHLEAPY;
	ob->velX = delta/60;
	ob->currentAction = CK_ACTION(KD_ACT_squasherjump1);
}

/*
====================
=
= SquasherJumpReact
=
====================
*/

void KD_SquasherJumpReact (CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;

	if (ob->topTI)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_squasherwait));

	PLACESPRITE;
}


/*
=============================================================================

								APEL

temp4 = pole x coordinate

=============================================================================
*/

void KD_ApelThink (CK_object *ob);
void KD_ApelClimbThink (CK_object *ob);
void KD_ApelSlideThink (CK_object *ob);
void ApelFallThink (CK_object *ob);

void KD_ApelClimbReact (CK_object *ob);
void ApelSlideReact (CK_object *ob);
void KD_ApelFallReact (CK_object *ob);












/*
====================
=
= SpawnApel
=
====================
*/

void KD_SpawnApel (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_apelobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_apelwalk1));
}


/*
====================
=
= ApelThink
=
====================
*/

void KD_ApelThink (CK_object *ob)
{
	int16_t	x,y;
	uint16_t  *map;

	if (ob->clipRects.unitY1 > ck_keenObj->clipRects.unitY2 || ob->clipRects.unitY2 < ck_keenObj->clipRects.unitY1)
	{
	//
	// try to climb a pole to reach player
	//
		if (ob->posY < ck_keenObj->posY)
			y = ob->clipRects.tileY2;
		else
			y = ob->clipRects.tileY1;

		map = CA_mapPlanes[1]+
			((y)*CA_GetMapWidth()) + ob->clipRects.tileXmid;

		if ((KD_INTILE(*map)&0x7f) == 1)
		{
			ck_nextX = (ob->clipRects.tileXmid<<8) - ob->posX;
			ck_nextY = 0;
			ob->user4 = ob->clipRects.tileXmid;	// for future reference
			ob->clipped = CLIP_not;		// can climb through pole holes
			if (ob->posY < ck_keenObj->posY)
				ob->currentAction = CK_ACTION(KD_ACT_apelslide1);
			else
				ob->currentAction = CK_ACTION(KD_ACT_apelclimb1);
			return;
		}
	}

	if (US_RndT()>32)		// don't turn around all the time
		return;

	if (ob->posX < ck_keenObj->posX)
		ob->xDirection = 1;
	else
		ob->xDirection = -1;

}


/*
====================
=
= ApelClimbThink
=
====================
*/

void KD_ApelClimbThink (CK_object *ob)
{
	uint16_t  *map;

	map = CA_mapPlanes[1]+
		((ob->clipRects.tileY1)*CA_GetMapWidth()) + ob->user4;

	if ((KD_INTILE(*map)&0x7f) != 1)
	{
		ob->clipped = CLIP_normal;
		ob->currentAction = CK_ACTION(KD_ACT_apelfall);
	}
}


/*
====================
=
= ApelSlideThink
=
====================
*/

void KD_ApelSlideThink (CK_object *ob)
{
	uint16_t  *map;

	map = CA_mapPlanes[1]+
		((ob->clipRects.tileY2)*CA_GetMapWidth()) + ob->user4;

	if ((KD_INTILE(*map)&0x7f) != 1)
	{
		ob->clipped = CLIP_normal;
		ob->currentAction = CK_ACTION(KD_ACT_apelfall);
	}
}


/*
====================
=
= ApelClimbReact
=
====================
*/

void KD_ApelClimbReact (CK_object *ob)
{
	if (ob->bottomTI)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_apelfall));
	PLACESPRITE;
}


/*
====================
=
= ApelFallReact
=
====================
*/

void KD_ApelFallReact (CK_object *ob)
{
	if (ob->topTI)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_apelwalk1));
	PLACESPRITE;
}


/*
=============================================================================

								PEA BRAIN

=============================================================================
*/

void KD_PeaBrainThink (CK_object *ob);
void KD_PeaFlyReact (CK_object *ob);







/*
====================
=
= SpawnPeaBrain
=
====================
*/

void KD_SpawnPeaBrain (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_peabrainobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = tiley<<8;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_peabrainwalk1));
}

/*
====================
=
= PeaBrainThink
=
====================
*/

void KD_PeaBrainThink (CK_object *ob)
{
	ob++;
}


/*
====================
=
= PeaFlyReact
=
====================
*/

void KD_PeaFlyReact (CK_object *ob)
{
	if (ob->topTI)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_peabrainwalk1));

	PLACESPRITE;
}


/*
=============================================================================

								PEA POD

temp1 = number of peas spit

=============================================================================
*/

#define MAXPEASPIT	4

#define PEAXSPEED	48
#define PEAYSPEED	-20


void KD_PeaPodThink (CK_object *ob);
void KD_SpitPeaBrain (CK_object *ob);







/*
====================
=
= SpawnPeaPod
=
====================
*/

void KD_SpawnPeaPod (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_peapodobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-2*BLOCKSIZE;
	kd_new->xDirection = 1;
	kd_new->yDirection = 1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_peapodwalk1));
}

/*
====================
=
= SpitPeaBrain
=
====================
*/

void KD_SpitPeaBrain (CK_object *ob)
{
	KD_GetNewObj (true);
	kd_new->type = KD_peabrainobj;
	if (ob->xDirection == 1)
	{
		kd_new->posX = ob->posX+8*16;
		kd_new->posY = ob->posY+8*16;
	}
	else
	{
		kd_new->posX = ob->posX;
		kd_new->posY = ob->posY+8*16;
	}
	kd_new->xDirection = ob->xDirection;
	kd_new->yDirection = 1;
	kd_new->velX = ob->xDirection * PEAXSPEED-(US_RndT()>>4);
	kd_new->velY = PEAYSPEED;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_peabrainfly));
}



/*
====================
=
= PeaPodThink
=
====================
*/

void KD_PeaPodThink (CK_object *ob)
{
	int16_t delta;

	if ( abs(ob->posY - ck_keenObj->posY) > 3*256 )
		return;

	if (ck_keenObj->posX < ob->posX && ob->xDirection == 1)
		return;

	if (ck_keenObj->posX > ob->posX && ob->xDirection == -1)
		return;

	if (US_RndT()<8 && ob->user1 < MAXPEASPIT)
	{
		ob->user1 ++;
		ob->currentAction = CK_ACTION(KD_ACT_peapodspit1);
		ck_nextX = 0;
	}
}


/*
=============================================================================

							BOOBUS TUBOR

temp4 = hit points

=============================================================================
*/

#define PREFRAGTHINK	60
#define POSTFRAGTHINK	60

#define SPDBOOBUSJUMP	60
#define SPDBOOBUSRUNJUMP 24

void KD_BoobusThink (CK_object *ob);
void KD_FinishThink (CK_object *ob);
void KD_FragThink (CK_object *ob);
void KD_BoobusGroundReact (CK_object *ob);
void KD_BoobusAirReact (CK_object *ob);











/*
====================
=
= SpawnBoobus
=
====================
*/

void KD_SpawnBoobus (int16_t tilex, int16_t tiley)
{
	KD_GetNewObj (false);

	kd_new->type = KD_boobusobj;
	kd_new->posX = tilex<<8;
	kd_new->posY = (tiley<<8)-11*BLOCKSIZE;
	kd_new->xDirection = -1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_boobuswalk1));
	kd_new->user4 = 12;			// hit points
}


/*
===================
=
= FragThink
=
===================
*/

void KD_FragThink (CK_object *ob)
{
	if (++ob->user1 == PREFRAGTHINK)
		ob->currentAction = CK_ACTION(KD_ACT_boobusdie2);
	if (++ob->user1 == POSTFRAGTHINK)
	{
		RF_RemoveSpriteDraw (&ob->sde);
		ob->currentAction = CK_ACTION(KD_ACT_boobusdie3);
	}

	SD_PlaySound (KD_BOMBBOOMSND);
	KD_GetNewObj (true);
	kd_new->posX = ob->posX-BLOCKSIZE + 5*US_RndT();
	kd_new->posY = ob->posY-BLOCKSIZE + 5*US_RndT();
	kd_new->velX = 0; //US_RndT()/4-32;
	kd_new->velY = 0; //US_RndT()/4-32;
	US_RndT();					// keep rnd from even wrapping
	CK_SetAction2 (kd_new,CK_ACTION(KD_ACT_deathboom1));
}


/*
===================
=
= FinishThink
=
===================
*/

void KD_FinishThink (CK_object *ob)
{
	ck_gameState.levelState = (CK_LevelState)KD_LS_Victorious;
	ob++;
	SD_PlaySound (KD_BOOBUSGONESND);
}


/*
====================
=
= BoobusThink
=
====================
*/

void KD_BoobusThink (CK_object *ob)
{
	uint16_t	move;
	bool	inLine = false;

	if (ob->clipRects.unitX1 > ck_keenObj->clipRects.unitX2)
		ob->xDirection = -1;
	else if (ob->clipRects.unitX2 < ck_keenObj->clipRects.unitX1)
		ob->xDirection = 1;
	else
		inLine = true;

	if (ck_keenObj->clipRects.unitY1 < ob->clipRects.unitY2 && ck_keenObj->clipRects.unitY2 > ob->clipRects.unitY1)
	{
	// on same level as player, so charge!

	}
	else
	{
	// above or below player, so get directly in line and jump
		if (inLine)
		{
			if (ob->posY < ck_keenObj->posY)
			{
			// drop down
				move = 16*8;
				ob->clipRects.tileY2++;
				ob->clipRects.unitY2 += move;
				ob->posY += move;
				ob->currentAction = CK_ACTION(KD_ACT_boobusjump);
				ob->velY = ck_nextX = ob->velX = ck_nextY = 0;
			}
			else
			{
			// jump up
				ob->currentAction = CK_ACTION(KD_ACT_boobusjump);
				ob->velY = -SPDBOOBUSJUMP;
				ob->velX = 0;
			}

		}
	}
}

/*
====================
=
= BoobusGroundReact
=
====================
*/

void KD_BoobusGroundReact (CK_object *ob)
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
		if (ob->clipRects.unitY2 >= ck_keenObj->clipRects.unitY2)
		{
		 // jump over
			ob->posX -= ob->deltaPosX;
			ob->posY -= ob->deltaPosY;
			ob->velY = -SPDBOOBUSJUMP;
			ob->velX = ob->xDirection * SPDBOOBUSRUNJUMP;
		}
		else
		{
		// drop down
			ob->velX = ob->velY = 0;
		}
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_boobusjump));
	}

	PLACESPRITE;
}



/*
====================
=
= BoobusAirReact
=
====================
*/

void KD_BoobusAirReact (CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;

	if (ob->topTI)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_boobuswalk1));

	PLACESPRITE;
}

void KD_Act2_SetupFunctions(void)
{
	CK_ACT_AddFunction("KD_ApelClimbReact", &KD_ApelClimbReact);
	CK_ACT_AddFunction("KD_ApelClimbThink", &KD_ApelClimbThink);
	CK_ACT_AddFunction("KD_ApelFallReact", &KD_ApelFallReact);
	CK_ACT_AddFunction("KD_ApelSlideThink", &KD_ApelSlideThink);
	CK_ACT_AddFunction("KD_ApelThink", &KD_ApelThink);
	CK_ACT_AddFunction("KD_BackupReact", &KD_BackupReact);
	CK_ACT_AddFunction("KD_BoobusAirReact", &KD_BoobusAirReact);
	CK_ACT_AddFunction("KD_BoobusGroundReact", &KD_BoobusGroundReact);
	CK_ACT_AddFunction("KD_BoobusThink", &KD_BoobusThink);
	CK_ACT_AddFunction("KD_CartReact", &KD_CartReact);
	CK_ACT_AddFunction("KD_FinishThink", &KD_FinishThink);
	CK_ACT_AddFunction("KD_FragThink", &KD_FragThink);
	CK_ACT_AddFunction("KD_FrenchyRunThink", &KD_FrenchyRunThink);
	CK_ACT_AddFunction("KD_FrenchyThink", &KD_FrenchyThink);
	CK_ACT_AddFunction("KD_FrenchyThrow", &KD_FrenchyThrow);
	CK_ACT_AddFunction("KD_MelonSpitThink", &KD_MelonSpitThink);
	CK_ACT_AddFunction("KD_PeaBrainThink", &KD_PeaBrainThink);
	CK_ACT_AddFunction("KD_PeaFlyReact", &KD_PeaFlyReact);
	CK_ACT_AddFunction("KD_PeaPodThink", &KD_PeaPodThink);
	CK_ACT_AddFunction("KD_ProjectileReact", &KD_ProjectileReact);
	CK_ACT_AddFunction("KD_SpitPeaBrain", &KD_SpitPeaBrain);
	CK_ACT_AddFunction("KD_SquasherJumpReact", &KD_SquasherJumpReact);
	CK_ACT_AddFunction("KD_SquasherThink", &KD_SquasherThink);
	CK_ACT_AddFunction("KD_TaterThink", &KD_TaterThink);
}
