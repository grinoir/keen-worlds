#include "kw_dreams.h"
#include "kw_music.h"

/*
Keen Worlds: Keen Dreams - Keen, flower power and the map Keen
Converted from KD_KEEN.C of the Keen Dreams source (id Software 1991; GPL release
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

// KD_KEEN.C


/*

player->user1 = pausetime / pointer to zees when sleeping
player->user2 = pausecount / stagecount
player->user3 =
player->user4 =


*/


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define KEENPRIORITY	1

#define PLACESPRITE RF_AddSpriteDraw (&ob->sde,ob->posX,ob->posY,ob->gfxChunk, \
	false,KEENPRIORITY);

#define	MINPOLEJUMPTICS	19	// wait tics before allowing a pole regram

#define SPDRUNJUMP		16
#define SPDPOLESIDEJUMP	8
#define WALKAIRSPEED	8
#define DIVESPEED		32
#define	JUMPTIME		16
#define	POLEJUMPTIME	10
#define	SPDJUMP			40
#define	SPDDIVEUP		16
#define	SPDPOLEJUMP		20

#define SPDPOWERUP		-64
#define SPDPOWER		64
#define SPDPOWERY		-20
#define POWERCOUNT		50

#define MAXXSPEED   24

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

int16_t	kd_singlegravity;
KD_State kd_state;
KD_Control kd_c;
bool kd_button0held, kd_button1held;
CK_object *kd_new;
uint16_t	kd_bounceangle[8][8] =
{
{0,0,0,0,0,0,0,0},
{7,6,5,4,3,2,1,0},
{5,4,3,2,1,0,15,14},
{5,4,3,2,1,0,15,14},
{3,2,1,0,15,14,13,12},
{9,8,7,6,5,4,3,2},
{9,8,7,6,5,4,3,2},
{11,10,9,8,7,6,5,4}
};


/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

int16_t	jumptime;
uint32_t	leavepoletime;		// TimeCount when jumped off pole

// (The score box is the engine's: CK_UpdateScoreBox in ck_map.c.)

/*
=============================================================================

						FLOWER POWER ROUTINES

temp1 = 8, same as power bonus
temp2 = initial direction

=============================================================================
*/

void	KD_PowerCount			(CK_object *ob);
void	KD_PowerContact 		(CK_object *ob, CK_object *hit);
void	KD_PowerReact			(CK_object *ob);










/*
===============
=
= ThrowPower
=
===============
*/

// What the last throw threw, or NULL: with nothing to throw, the original's
// air throws went on to change the speed of whatever "new" last was.
static CK_object *kd_thrown;

bool KD_ThrowPower (uint16_t x, uint16_t y, int16_t dir)
{
	kd_thrown = NULL;
	CK_action *startstate;

	if (ca_mapOn == 15)
	{
		if (!kd_state.boobusbombs)
		{
			SD_PlaySound (KD_NOWAYSND);
			return false;					// no bombs to throw
		}
		SD_PlaySound (KD_THROWBOMBSND);
		kd_state.bombsthislevel--;
		kd_state.boobusbombs--;
	}
	else
	{
		if (!ck_gameState.numShots)
		{
			SD_PlaySound (KD_NOWAYSND);
			return false;					// no flower power to throw
		}
		SD_PlaySound (KD_THROWSND);
		ck_gameState.numShots--;
	}



	KD_GetNewObj (true);
	kd_thrown = kd_new;
	kd_new->type = KD_powerobj;
	kd_new->user2 = dir;
	kd_new->posX = x;
	kd_new->posY = y;
	kd_new->clipRects.tileX1 = kd_new->clipRects.tileX2 = x>>8;
	kd_new->clipRects.tileY1 = kd_new->clipRects.tileY2 = y>>8;
	kd_new->yDirection = -1;

	switch (dir)
	{
	case IN_dir_North:
		kd_new->velX = 0;
		kd_new->velY = SPDPOWERUP;
		break;
	case IN_dir_East:
		kd_new->velX = SPDPOWER;
		kd_new->velY = SPDPOWERY;
		break;
	case IN_dir_South:
		kd_new->velX = 0;
		kd_new->velY = SPDPOWER;
		break;
	case IN_dir_West:
		kd_new->velX = -SPDPOWER;
		kd_new->velY = SPDPOWERY;
		break;
	default:
		Quit ("ThrowPower: Bad dir!");
	}

	if (ca_mapOn != 15)
	{
		kd_new->user1 = 8;  				// flower power bonus
		startstate = CK_ACTION(KD_ACT_flowerpower1);
	}
	else
	{
		kd_new->user1 = 10;  				// boobus bomb bonus
		startstate = CK_ACTION(KD_ACT_boobusbomb1);
	}
	kd_new->active = OBJ_EXISTS_ONLY_ONSCREEN;

#if 0
	kd_new->posX -= 5*kd_new->velX;		// make sure they hit nearby walls
	kd_new->posY -= 5*kd_new->velY;
#endif
	KD_NewState (kd_new,startstate);
#if 0
	ck_nextX = 5*kd_new->velX;
	ck_nextY = 5*kd_new->velY;
	CK_PhysUpdateNormalObj (kd_new);
#endif
	return true;
}

/*
============================
=
= PowerCount
=
============================
*/

void	KD_PowerCount (CK_object *ob)
{
	ob->user2+=SD_GetSpriteSync();

	ob->gfxChunk = 0;

	if (ob->user2 > POWERCOUNT)
		CK_RemoveObj(ob);
}


/*
============================
=
= CalcSingleGravity
=
============================
*/

void	KD_CalcSingleGravity (void)
{
	uint16_t	speed;
	int32_t	i;
//
// only accelerate on odd tics, because of limited precision
//
	speed = 0;
	kd_singlegravity = 0;
	for (i=SD_GetLastTimeCount()-SD_GetSpriteSync();i<SD_GetLastTimeCount();i++)
	{
		if (i&1)
		{
			speed+=ACCGRAVITY;
			if (speed>SPDMAXY)
			  speed=SPDMAXY;
		}
		kd_singlegravity+=speed;
	}

	kd_singlegravity/=2;
}


/*
============================
=
= PowerContact
=
============================
*/

void	KD_PowerContact (CK_object *ob, CK_object *hit)
{
	uint16_t	x,y,yspot,xspot;

	switch (hit->type)
	{
	case	KD_broccoobj:
	case	KD_tomatobj:
	case	KD_carrotobj:
	case	KD_celeryobj:
	case	KD_asparobj:
	case	KD_taterobj:
	case	KD_frenchyobj:
	case	KD_squashobj:
	case	KD_apelobj:
	case	KD_peapodobj:
	case	KD_peabrainobj:
		KD_ChangeToFlower (hit);
		CK_RemoveObj (ob);
		break;
	case	KD_boobusobj:
		if (!--hit->user4)
		{
		// killed boobus
			CK_IncreaseScore (50000);
			CK_IncreaseScore (50000);
			hit->type = KD_inertobj;
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_boobusdie));
			hit->user1 = 0;		// death count
		}
		SD_PlaySound (KD_BOMBBOOMSND);
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_bombexplode));
		break;
	}
}


/*
============================
=
= PowerReact
=
============================
*/

void	KD_PowerReact (CK_object *ob)
{
	uint16_t wall,absx,absy,angle,newangle;
	uint32_t speed;

	PLACESPRITE;
	if (ob->rightTI || ob->leftTI)
	{
		ob->velX= -ob->velX/2;
		ob->type = KD_bonusobj;
	}

	if (ob->bottomTI)
	{
		if ( ob->bottomTI == 17)	// go through pole holes
		{
			if (ob->user2 != IN_dir_North)
				ob->type = KD_bonusobj;
			ob->clipRects.unitY1-=32;
			ob->posY-=32;
			ob->velX = 0;
			ob->posX = ob->clipRects.tileXmid*256 + 4*16;
		}
		else
		{
			ob->velY= -ob->velY/2;
		}
		return;
	}

	wall = ob->topTI;
	if ( wall == 17)	// go through pole holes
	{
		if (ob->user2 != IN_dir_South)
			ob->type = KD_bonusobj;
		ob->clipRects.unitY2+=32;
		ob->posY+=32;
		ob->velX = 0;
		ob->posX = ob->clipRects.tileXmid*256 + 4*16;
	}
	else if (wall)
	{
		ob->type = KD_bonusobj;
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
		{
			if (ca_mapOn == 15)
			{
				ob->yDirection = -1;			// bonus stuff flies up when touched
				ob->user2 = ob->gfxChunk = KD_BOOBUSBOMB1SPR;
				ob->user3 = ob->user2 + 2;
				ob->clipped = CLIP_not;
				CK_SetAction2 (ob,CK_ACTION(KD_ACT_bonus));
			}
			else
			{
				CK_SetAction2 (ob,CK_ACTION(KD_ACT_powerblink1));
				ob->clipped = CLIP_not;
				ob->user2 = 0;		// blink time
			}
		}
	}
}


/*
=============================================================================

							   MINI KEEN

player->user1 = dir
player->user2 = animation stage

#define WORLDKEENSLEEP1SPR		238
#define WORLDKEENSLEEP2SPR		239
#define WORLDKEENWAVE1SPR		240
#define WORLDKEENWAVE2SPR		241
=============================================================================
*/

void	KD_SpawnWorldKeen (int16_t tilex, int16_t tiley);
void	KD_KeenWorldThink		(CK_object *ob);
void	KD_KeenWorldWalk		(CK_object *ob);













/*
======================
=
= SpawnWorldKeen
=
======================
*/

void	KD_SpawnWorldKeen (int16_t tilex, int16_t tiley)
{
	ck_keenObj->type = KD_keenobj;
	if (!ck_gameState.mapPosX)
	{
		ck_keenObj->posX = tilex<<8;
		ck_keenObj->posY = tiley<<8;
	}
	else
	{
		ck_keenObj->posX = ck_gameState.mapPosX;
		ck_keenObj->posY = ck_gameState.mapPosY;
	}

	ck_keenObj->active = OBJ_ALWAYS_ACTIVE;
	ck_keenObj->clipped = CLIP_normal;
	ck_keenObj->xDirection = 0;
	ck_keenObj->yDirection = 0;
	ck_keenObj->user1 = IN_dir_West;
	ck_keenObj->user2 = 3;
	ck_keenObj->gfxChunk = 3+KD_WORLDKEENL1SPR-1;				// feet together
	KD_NewState (ck_keenObj,CK_ACTION(KD_ACT_worldkeen));
}


/*
======================
=
= CheckEnterLevel
=
======================
*/

void	KD_CheckEnterLevel (CK_object *ob)
{
	int16_t	x,y,tile;

	for (y=ob->clipRects.tileY1;y<=ob->clipRects.tileY2;y++)
		for (x=ob->clipRects.tileX1;x<=ob->clipRects.tileX2;x++)
		{
			tile = *(CA_mapPlanes[2]+((y)*CA_GetMapWidth())+x);
			if (tile >= 3 && tile <=18)
			{
			//
			// enter level!
			//
				if (tile == 17)
				{
					if (kd_state.boobusbombs < 12)
					{
						VL_FixRefreshBuffer ();
						US_CenterWindow (26,6);
						US_SetPrintY(US_GetPrintY() + (8));
						US_CPrint ("You can't possibly\n"
								   "defeat King Boobus Tuber\n"
								   "with less than 12 bombs!");
						VH_UpdateScreen ();
						KW_SayWorldLine ("BOMBS");	// KEEN WORLDS: read out (tools/kw_packs_voice.py)
						IN_WaitButton ();
						KW_LinesOver ();
						RF_ForceRefresh ();
						return;
					}
				}
				ck_gameState.mapPosX = ob->posX;
				ck_gameState.mapPosY = ob->posY;
				ck_gameState.currentLevel = tile-2;
				ck_gameState.levelState = (CK_LevelState)LS_LevelComplete;
				SD_PlaySound (KD_ENTERLEVELSND);
			}
		}
}


/*
======================
=
= KeenWorldThink
=
======================
*/

void	KD_KeenWorldThink (CK_object *ob)
{
	if (kd_c.dir!=IN_dir_None)
	{
		ob->currentAction = CK_ACTION(KD_ACT_worldwalk);
		ob->user2 = 0;
		KD_KeenWorldWalk (ob);
	}

	if (kd_c.button0 || kd_c.button1)
		KD_CheckEnterLevel(ob);
}


/*
======================
=
= KeenWorldWalk
=
======================
*/

int16_t worldshapes[8] = {KD_WORLDKEENU1SPR-1,KD_WORLDKEENUR1SPR-1,KD_WORLDKEENR1SPR-1,
	KD_WORLDKEENDR1SPR-1,KD_WORLDKEEND1SPR-1,KD_WORLDKEENDL1SPR-1,KD_WORLDKEENL1SPR-1,
	KD_WORLDKEENUL1SPR-1};

int16_t worldanims[4] ={2,3,1,3};

void	KD_KeenWorldWalk (CK_object *ob)
{
	ob->xDirection = kd_c.xaxis;
	ob->yDirection = kd_c.yaxis;

	if (++ob->user2==4)
		ob->user2 = 0;

	if (kd_c.button0 || kd_c.button1)
		KD_CheckEnterLevel(ob);

	if (kd_c.dir == IN_dir_None)
	{
		ob->currentAction = CK_ACTION(KD_ACT_worldkeen);
		ob->gfxChunk = worldshapes[ob->user1]+3;
		return;
	}

	ob->user1 = kd_c.dir;
	ob->gfxChunk = worldshapes[ob->user1]+worldanims[ob->user2];
}


/*
=============================================================================

								KEEN

player->user1 = pausetime / pointer to zees when sleeping
player->user2 = pausecount / stagecount
player->user3 =
player->user4 = pole x location

=============================================================================
*/

void	KD_KeenStandThink		(CK_object *ob);
void	KD_KeenPauseThink		(CK_object *ob);
void	KD_KeenGoSleepThink	(CK_object *ob);
void	KD_KeenSleepThink		(CK_object *ob);
void	KD_KeenDieThink		(CK_object *ob);
void	KD_KeenDuckThink		(CK_object *ob);
void	KD_KeenDropDownThink	(CK_object *ob);
void	KD_KeenWalkThink		(CK_object *ob);
void	KD_KeenAirThink		(CK_object *ob);
void	KD_KeenPoleThink		(CK_object *ob);
void	KD_KeenClimbThink		(CK_object *ob);
void	KD_KeenDropThink		(CK_object *ob);
void	KeenDive			(CK_object *ob);
void	KD_KeenThrow			(CK_object *ob);

void	KD_KeenContact 		(CK_object *ob, CK_object *hit);
void	KD_KeenSimpleReact		(CK_object *ob);
void	KD_KeenStandReact		(CK_object *ob);
void	KD_KeenWalkReact		(CK_object *ob);
void	KeenPoleReact		(CK_object *ob);
void	KD_KeenAirReact		(CK_object *ob);
void	KeenDiveReact		(CK_object *ob);
void	KD_KeenSlideReact		(CK_object *ob);

//-------------------


//-------------------







//-------------------








//-------------------





//-------------------







//-------------------


//-------------------









//-------------------








//-------------------




//-------------------







//===========================================================================



/*
===============
=
= SpawnKeen
=
===============
*/

void KD_SpawnKeen (int16_t tilex, int16_t tiley, int16_t dir)
{
	ck_keenObj->type = KD_keenobj;
	ck_keenObj->active = OBJ_ALWAYS_ACTIVE;
	ck_keenObj->clipped = CLIP_normal;
	ck_keenObj->posX = tilex<<8;
	ck_keenObj->posY = (tiley<<8) - BLOCKSIZE*2;

	ck_keenObj->xDirection = dir;
	ck_keenObj->yDirection = 1;
	KD_NewState (ck_keenObj,CK_ACTION(KD_ACT_keenstand));
}

//==========================================================================

/*
======================
=
= CheckGrabPole
=
======================
*/

bool	KD_CheckGrabPole (CK_object *ob)
{
	int16_t	x;
	uint16_t  *map;

//
// kludgy bit to not let you grab a pole the instant you jump off it
//
	if (SD_GetTimeCount() < leavepoletime)
		leavepoletime = 0;
	else if (SD_GetTimeCount() - leavepoletime < MINPOLEJUMPTICS)
		return false;

	if (kd_c.yaxis == -1)
		map = CA_mapPlanes[1]+
			(((ob->clipRects.unitY1+6*16)/256)*CA_GetMapWidth());
	else
		map = CA_mapPlanes[1]+
			((ob->clipRects.tileY2)*CA_GetMapWidth());

	x = (ob->clipRects.unitX1 + (ob->clipRects.unitX2 - ob->clipRects.unitX1)/2) >>8;

	map += x;

	if ((KD_INTILE(*map)&0x7f) == 1)
	{
		ck_nextX = ((x<<8)-8*16) - ob->posX;
		ck_nextY = kd_c.yaxis*32;
		ob->user4 = x;				// for future reference
		ob->clipped = CLIP_not;		// can climb through pole holes
		ob->currentAction = CK_ACTION(KD_ACT_keenpole);
		return true;
	}

	return false;
}

//==========================================================================


/*
============================
=
= KeenStandThink
=
============================
*/

void KD_KeenStandThink (CK_object *ob)
{
	if (kd_c.xaxis)
	{
	// started walking
		ob->xDirection = kd_c.xaxis;
		ob->currentAction = CK_ACTION(KD_ACT_keenwalk1);
		ck_nextX = ob->xDirection*CK_ACTION(KD_ACT_keenwalk1)->velX*SD_GetSpriteSync();
		KD_KeenWalkThink(ob);
		return;
	}

	if (kd_c.button0 && !kd_button0held)
	{
	// jump straight up
		SD_PlaySound (KD_JUMPSND);
		ob->velX = 0;
		ob->velY = -SPDJUMP;
		ck_nextX = 0;
		ck_nextY = 0;		// ob->velY*tics;			// DEBUG
		jumptime = JUMPTIME;	// -tics;
		ob->currentAction = CK_ACTION(KD_ACT_keenjumpup1);
		kd_button0held = true;
		return;
	}

	if (kd_c.button1 && !kd_button1held)
	{
	// throw current xdir
		if (kd_c.yaxis == -1)
			ob->currentAction = CK_ACTION(KD_ACT_keenthrowup1);
		else
			ob->currentAction = CK_ACTION(KD_ACT_keenthrow1);
		kd_button1held = true;
		return;
	}

	switch (kd_c.yaxis)
	{
	case -1:
		if (!KD_CheckGrabPole(ob))			// try to go up/down pole first
			ob->currentAction = CK_ACTION(KD_ACT_keenlookup);
		return;
	case 0:
		ob->currentAction = CK_ACTION(KD_ACT_keenstand);
		return;
	case 1:
		if (!KD_CheckGrabPole(ob))			// try to go up/down pole first
			ob->currentAction = CK_ACTION(KD_ACT_keenduck);
		return;
	}
}

//===========================================================================

/*
=======================
=
= KeenPauseThink
=
= Do special animations in time
=
=======================
*/

void KD_KeenPauseThink (CK_object *ob)
{
	if (kd_c.dir != IN_dir_None || kd_c.button0 || kd_c.button1)
	{
		ob->user1 = ob->user2 = 0;			// not paused any more
		ob->currentAction = CK_ACTION(KD_ACT_keenstand);
		KD_KeenStandThink(ob);
		return;
	}

	if ((ob->topTI & ~7) != 24)	// if it is 0 now, keen is standing on a sprite
		ob->user1 += SD_GetSpriteSync();

	switch (ob->user2)
	{
	case 0:
		if (ob->user1 > 200)
		{
			ob->user2++;
			ob->currentAction = CK_ACTION(KD_ACT_keenpauselook);
			ob->user1 = 0;
		}
		break;
	case 1:
		if (ob->user1 > 300)
		{
			ob->user2++;
			ob->currentAction = CK_ACTION(KD_ACT_keenwait1);
			ob->user1 = 0;
		}
		break;
	case 2:
		if (ob->user1 > 700)
		{
			ob->user2++;
			ob->currentAction = CK_ACTION(KD_ACT_keenyawn1);
			ob->user1 = 0;
		}
		break;
	case 3:
		if (ob->user1 > 400)
		{
			ob->user2++;
			ob->currentAction = CK_ACTION(KD_ACT_keenpauselook);
			ob->user1 = 0;
		}
		break;
	case 4:
		if (ob->user1 > 700)
		{
			ob->user2++;
			ob->currentAction = CK_ACTION(KD_ACT_keenyawn1);
			ob->user1 = 0;
		}
		break;
	case 5:
		if (ob->user1 > 400)
		{
			ob->user2++;
			ob->currentAction = CK_ACTION(KD_ACT_keengosleep1);
		}
		break;
	}
}


//===========================================================================

/*
=======================
=
= KeenGoSleepThink
=
=======================
*/

void KD_KeenGoSleepThink (CK_object *ob)
{
//
// spawn the zees above keens head
//
	KD_GetNewObj (true);

	kd_new->type = KD_inertobj;
	kd_new->posX = ck_keenObj->posX;
	kd_new->posY = ck_keenObj->posY-4*16;
	kd_new->xDirection = 1;
	kd_new->yDirection = -1;
	KD_NewState (kd_new,CK_ACTION(KD_ACT_keenzee1));

	ob->user1 = CK_ConvertObjPointerTo16BitOffset(kd_new);				// so they can be removed later
}


//===========================================================================

/*
=======================
=
= KeenSleepThink
=
=======================
*/

void KD_KeenSleepThink (CK_object *ob)
{
	if (kd_c.dir != IN_dir_None || kd_c.button0 || kd_c.button1)
	{
		CK_object *zees = CK_ConvertObj16BitOffsetToPointer(ob->user1);
		if (zees && zees->currentAction && zees->type == KD_inertobj)
			CK_RemoveObj (zees);	// remove the zees
		ob->user1 = ob->user2 = 0;			// not paused any more
		ob->currentAction = CK_ACTION(KD_ACT_keengetup);
	}
}


//===========================================================================

/*
=======================
=
= KeenDieThink
=
=======================
*/

void KD_KeenDieThink (CK_object *ob)
{
	ob++;			// shut up compiler
	ck_gameState.levelState = (CK_LevelState)LS_Died;
}


//===========================================================================

/*
=======================
=
= KeenDuckThink
=
=======================
*/

void KD_KeenDuckThink (CK_object *ob)
{
	uint16_t  *map, tile;
	int16_t midtile,bottomtile,move;

	if (kd_c.yaxis != 1)
	{
		ob->currentAction = CK_ACTION(KD_ACT_keenstand);
		KD_KeenStandThink(ob);
		return;
	}

	if (kd_c.xaxis)
		ob->xDirection = kd_c.xaxis;

	if (kd_c.button0 && !kd_button0held)
	{
	//
	// drop down a level
	//
		kd_button0held = true;

		midtile = (ob->clipRects.tileX1 + ob->clipRects.tileX2) >> 1;
		bottomtile = ob->clipRects.tileY2;
		map = CA_mapPlanes[1] + ((bottomtile)*CA_GetMapWidth())
			+ midtile;
		tile = *map;
		if (KD_WESTWALL(tile) || KD_EASTWALL(tile)
			|| KD_SOUTHWALL(tile))
			return;				// wall prevents drop down

		map += CA_GetMapWidth();
		tile = *map;
		if (KD_WESTWALL(tile) || KD_EASTWALL(tile)
			|| KD_SOUTHWALL(tile))
			return;				// wall prevents drop down

		move = 16*(SD_GetSpriteSync()<4 ? 4: SD_GetSpriteSync());
		ob->clipRects.unitY2 += move;
		ob->posY += move;
		ck_nextX = ck_nextY = 0;
		ob->currentAction = CK_ACTION(KD_ACT_keenjumpup3);
		ob->user2 = 1;		// allready in last stage
		ob->velX = ob->velY = 0;
		SD_PlaySound (KD_PLUMMETSND);
	}
}


//===========================================================================

/*
=======================
=
= KeenDropDownThink
=
=======================
*/

void KD_KeenDropDownThink (CK_object *ob)
{
	ob->clipped = CLIP_normal;
}


//===========================================================================

/*
=======================
=
= KeenWalkThink
=
=======================
*/

static int16_t slopespeed[8] = {0,0,4,4,8,-4,-4,-8};

void KD_KeenWalkThink (CK_object *ob)
{
	int16_t move;

	if (!kd_c.xaxis)
	{
	//
	// stopped running
	//
		KD_KeenStandThink (ob);
		return;
	}

	ob->xDirection = kd_c.xaxis;

	if (kd_c.yaxis && KD_CheckGrabPole(ob))		// try to go up/down pole
		return;

	if (kd_c.button0 && !kd_button0held)
	{
	//
	// running jump
	//
		SD_PlaySound (KD_JUMPSND);
		ob->velX = ob->xDirection * SPDRUNJUMP;
		ob->velY = -SPDJUMP;
		ck_nextX = 0;
		ck_nextY = 0;					// ob->velY*tics;
		kd_button0held = true;
		jumptime = JUMPTIME; //-tics;		// DEBUG
		ob->currentAction = CK_ACTION(KD_ACT_keenjump1);
	}

	if (kd_c.button1 && !kd_button1held)
	{
	//
	// throw
	//
		ob->currentAction = CK_ACTION(KD_ACT_keenthrow1);
		kd_button1held = true;
		return;
	}

	//
	// give speed for slopes
	//
	move = SD_GetSpriteSync()*slopespeed[ob->topTI&7];

	ck_nextX += move;
}

//===========================================================================

/*
=======================
=
= KeenAirThink
=
=======================
*/

void	KD_KeenAirThink		(CK_object *ob)
{
	if (jumptime)
	{
		if (jumptime<SD_GetSpriteSync())
		{
			ck_nextY = ob->velY*jumptime;
			jumptime = 0;
		}
		else
		{
			ck_nextY = ob->velY*SD_GetSpriteSync();
			if (!ck_gameState.jumpCheat)
				jumptime-=SD_GetSpriteSync();
		}
		if (!kd_c.button0)
			jumptime = 0;

		if (!jumptime)
		{
			ob->user2 = 0;
			ob->currentAction = ob->currentAction->next;	// switch to second jump stage
		}
	}
	else
	{
		KD_DoGravity(ob);

		if (ob->velY>0 && !ob->user2)
		{
			ob->currentAction = ob->currentAction->next;	// switch to third jump stage
			ob->user2 = 1;
		}
	}

//-------------

	if (kd_c.xaxis)
	{
		ob->xDirection = kd_c.xaxis;
		KD_AccelerateX(ob,kd_c.xaxis*2,MAXXSPEED);
	}
	else
		KD_FrictionX(ob);

	if (kd_c.button1 && !kd_button1held)
	{
		kd_button1held = true;
	//
	// throw
	//
		switch (kd_c.yaxis)
		{
		case -1:
			ob->currentAction = CK_ACTION(KD_ACT_keenairthrowup1);
			return;
		case 0:
			ob->currentAction = CK_ACTION(KD_ACT_keenairthrow1);
			return;
		case 1:
			ob->currentAction = CK_ACTION(KD_ACT_keenairthrowdown1);
			return;
		}
	}

	if (ob->bottomTI==17)		// going through a pole hole
	{
		ob->velX = ck_nextX = 0;
	}


	if (kd_c.yaxis == -1)
		KD_CheckGrabPole (ob);

}

//===========================================================================

/*
=======================
=
= KeenPoleThink
=
=======================
*/

int16_t	polexspeed[3] = {-SPDPOLESIDEJUMP,0,SPDPOLESIDEJUMP};

void	KD_PoleActions (CK_object *ob)
{
	if (kd_c.xaxis)
		ob->xDirection = kd_c.xaxis;

	if (kd_c.button0 && !kd_button0held)		// jump off the pole
	{
		SD_PlaySound (KD_JUMPSND);
		ob->velX = polexspeed[kd_c.xaxis+1];
		ob->velY = -SPDPOLEJUMP;
		ob->clipped = CLIP_normal;
		jumptime = POLEJUMPTIME;
		ob->currentAction = CK_ACTION(KD_ACT_keenjump1);
		ob->yDirection = 1;
		kd_button0held = true;
		leavepoletime = SD_GetTimeCount();
		return;
	}

	if (kd_c.button1 && !kd_button1held)
	{
		kd_button1held = true;

		switch (kd_c.yaxis)
		{
		case -1:
			ob->currentAction = CK_ACTION(KD_ACT_keenpolethrowup1);
			break;
		case 0:
			ob->currentAction = CK_ACTION(KD_ACT_keenpolethrow1);
			break;
		case 1:
			ob->currentAction = CK_ACTION(KD_ACT_keenpolethrowdown1);
			break;
		}
	}
}


void	KD_KeenPoleThink		(CK_object *ob)
{
	uint16_t  *map, tile;

	switch (kd_c.yaxis)
	{
	case -1:
		ob->currentAction = CK_ACTION(KD_ACT_keenclimb1);
		ob->yDirection = -1;
		return;

	case 1:
		ob->currentAction = CK_ACTION(KD_ACT_keenslide1);
		ob->yDirection = 1;
		KD_KeenDropThink (ob);
		return;
	}

	if (kd_c.xaxis)
	{
	//
	// walk off pole if right next to ground
	//
		map = CA_mapPlanes[1] + (((ob->clipRects.tileY2+1)*CA_GetMapWidth()) + ob->clipRects.tileXmid);
		tile = *map;
		if (KD_NORTHWALL(tile))
		{
			ob->velX = 0;
			ob->velY = 0;
			ob->clipped = CLIP_normal;
			jumptime = 0;
			ob->currentAction = CK_ACTION(KD_ACT_keenjump3);
			ob->yDirection = 1;
			SD_PlaySound (KD_PLUMMETSND);
			return;
		}
	}

	KD_PoleActions (ob);
}


/*
=======================
=
= KeenClimbThink
=
=======================
*/

void	KD_KeenClimbThink		(CK_object *ob)
{
	uint16_t  *map;

	map = CA_mapPlanes[1]+((ob->clipRects.tileY1)*CA_GetMapWidth())+ob->user4;

	if ((KD_INTILE(*map)&0x7f) != 1)
	{
		ck_nextY=0;
		ob->currentAction = CK_ACTION(KD_ACT_keenpole);		// ran out of pole
		return;
	}

	switch (kd_c.yaxis)
	{
	case 0:
		ob->currentAction = CK_ACTION(KD_ACT_keenpole);
		ob->yDirection = 0;
		break;

	case 1:
		ob->currentAction = CK_ACTION(KD_ACT_keenslide1);
		ob->yDirection = 1;
		KD_KeenDropThink (ob);
		break;
	}

	KD_PoleActions (ob);
}


/*
=======================
=
= KeenDropThink
=
=======================
*/

void	KD_KeenDropThink		(CK_object *ob)
{
	uint16_t  *map;

	map = CA_mapPlanes[1]+((ob->clipRects.tileY2)*CA_GetMapWidth())+ob->user4;

	if ((KD_INTILE(*map)&0x7f) != 1)
	{
		SD_PlaySound (KD_PLUMMETSND);
		ob->currentAction = CK_ACTION(KD_ACT_keenjump3);		// ran out of pole
		jumptime = 0;
		ob->user2 = 1;
		ob->velX = polexspeed[kd_c.xaxis+1];
		ob->velY = 0;
		ob->clipped = CLIP_normal;
		ob->clipRects.tileY2--;
		return;
	}

	switch (kd_c.yaxis)
	{
	case -1:
		ob->currentAction = CK_ACTION(KD_ACT_keenclimb1);
		ob->yDirection = -1;
		break;

	case 0:
		ob->currentAction = CK_ACTION(KD_ACT_keenpole);
		ob->yDirection = 0;
		break;
	}

	KD_PoleActions (ob);
}

//===========================================================================

/*
=======================
=
= KeenThrow
=
=======================
*/

void	KD_KeenThrow	(CK_object *ob)
{
// can't use &<var> in a switch statement...

	if (ob->currentAction == CK_ACTION(KD_ACT_keenthrow3))
	{
		if (ob->xDirection > 0)
			KD_ThrowPower (ob->clipRects.unitXmid-4*16,ob->posY+8*16,IN_dir_East);
		else
			KD_ThrowPower (ob->clipRects.unitXmid-4*16,ob->posY+8*16,IN_dir_West);
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenpolethrow2))
	{
		if (ob->xDirection > 0)
			KD_ThrowPower (ob->posX+24*16,ob->posY,IN_dir_East);
		else
			KD_ThrowPower (ob->posX-8*16,ob->posY,IN_dir_West);
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenthrowup2))
	{
		KD_ThrowPower (ob->posX+4*16,ob->posY-8*16,IN_dir_North);
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenpolethrowup2))
	{
		KD_ThrowPower (ob->posX+8*16,ob->posY-8*16,IN_dir_North);
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenpolethrowdown2))
	{
		KD_ThrowPower (ob->posX+8*16,ob->posY+8*16,IN_dir_South);
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenairthrow2))
	{
		if (ob->xDirection > 0)
			KD_ThrowPower (ob->clipRects.unitXmid-4*16,ob->posY+8*16,IN_dir_East);
		else
			KD_ThrowPower (ob->clipRects.unitXmid-4*16,ob->posY+8*16,IN_dir_West);
#if 0
		if (ob->xDirection > 0)
			KD_ThrowPower (ob->posX+32*16,ob->posY+8*16,IN_dir_East);
		else
			KD_ThrowPower (ob->posX-16*16,ob->posY+8*16,IN_dir_West);
#endif

		if (kd_thrown)
		{
			kd_thrown->velX += ob->velX/2;
			kd_thrown->velY += ob->velY/2;
		}
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenairthrowup2))
	{
		if (ob->xDirection > 0)
			KD_ThrowPower (ob->posX+16*16,ob->posY,IN_dir_North);
		else
			KD_ThrowPower (ob->posX+4*16,ob->posY,IN_dir_North);
		if (kd_thrown)
		{
			kd_thrown->velX += ob->velX/2;
			kd_thrown->velY += ob->velY/2;
		}
		return;
	}

	if (ob->currentAction == CK_ACTION(KD_ACT_keenairthrowdown2))
	{
		if (ob->xDirection > 0)
			KD_ThrowPower (ob->posX+3*16,ob->posY+16*16,IN_dir_South);
		else
			KD_ThrowPower (ob->posX+12*16,ob->posY+16*16,IN_dir_South);
		if (kd_thrown)
		{
			kd_thrown->velX += ob->velX/2;
			kd_thrown->velY += ob->velY/2;
		}
		return;
	}

	Quit ("KeenThrow: Bad state!");
}


/*
=============================================================================

						CONTACT ROUTINES

=============================================================================
*/

/*
============================
=
= KillKeen
=
============================
*/

void KD_KillKeen (void)
{
	if (!ck_godMode)
	{
		SD_PlaySound (KD_WAKEUPSND);
		ck_keenObj->clipped = CLIP_not;
		CK_SetAction2 (ck_keenObj,CK_ACTION(KD_ACT_keendie1));
	}
}



/*
============================
=
= KeenContact
=
============================
*/

static uint16_t bonuspoints[6] = {100,200,500,1000,2000,5000};

void	KD_KeenContact (CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case	KD_bonusobj:
		hit->type = KD_inertobj;
		switch (hit->user1)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			SD_PlaySound (KD_GETPOINTSSND);
			hit->gfxChunk = KD_BONUS100SPR+hit->user1;
			CK_IncreaseScore (bonuspoints[hit->user1]);
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		case 6:
			SD_PlaySound (KD_EXTRAKEENSND);
			hit->gfxChunk = KD_BONUS1UPSPR;
			ck_gameState.numLives++;
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		case 7:
			SD_PlaySound (KD_EXTRAKEENSND);
			hit->gfxChunk = KD_BONUSSUPERSPR;
			ck_gameState.numLives+=3;
			ck_gameState.numShots+=8;
			CK_IncreaseScore (10000);
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		case 8:
			SD_PlaySound (KD_GETPOWERSND);
			hit->gfxChunk = KD_BONUSFLOWERSPR;
			ck_gameState.numShots++;
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		case 9:
			SD_PlaySound (KD_GETPOWERSND);
			hit->gfxChunk = KD_BONUSFLOWERUPSPR;
			ck_gameState.numShots+=5;
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		case 10:
			SD_PlaySound (KD_GETBOMBSND);
			hit->gfxChunk = KD_BONUSBOMBSPR;
			kd_state.boobusbombs++;
			kd_state.bombsthislevel++;
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		case 11:
			SD_PlaySound (KD_GETKEYSND);
			hit->gfxChunk = KD_BONUSKEYSPR;
			kd_state.keys++;
			CK_SetAction2 (hit,CK_ACTION(KD_ACT_bonusrise));
			break;
		}
		break;

	case	KD_doorobj:
		if (kd_state.keys)
		{
			if (hit->currentAction != CK_ACTION(KD_ACT_doorraise))
			{
				SD_PlaySound (KD_OPENDOORSND);
				kd_state.keys--;
				CK_SetAction2 (hit,CK_ACTION(KD_ACT_doorraise));
			}
		}
		else
		{
			SD_PlaySound (KD_NOWAYSND);
			CK_PhysPushX (ob,hit);
		}
		break;
	case	KD_taterobj:
		if (hit->currentAction == CK_ACTION(KD_ACT_taterattack2))
			KD_KillKeen ();
		break;
	case	KD_carrotobj:
		CK_PhysPushX (ob,hit);
		if (!ob->clipped)		// got pushed off a pole
		{
			SD_PlaySound (KD_PLUMMETSND);
			ob->clipped = CLIP_normal;
			ob->velX = ob->velY = 0;
			CK_SetAction2(ob,CK_ACTION(KD_ACT_keenjump3));
			ob->user2 = 1;
			jumptime = 0;
		}
		break;
	case	KD_cartobj:
		CK_PhysPushXY (ob,hit,true);
		break;
	case	KD_broccoobj:
		if (hit->currentAction == CK_ACTION(KD_ACT_broccosmash3) || hit->currentAction == CK_ACTION(KD_ACT_broccosmash4))
			KD_KillKeen ();
		break;
	case	KD_squashobj:
		if (hit->currentAction == CK_ACTION(KD_ACT_squasherjump2))
			KD_KillKeen ();
		else
		{
			CK_PhysPushX (ob,hit);
			if (!ob->clipped)		// got pushed off a pole
			{
				SD_PlaySound (KD_PLUMMETSND);
				ob->clipped = CLIP_normal;
				ob->velX = ob->velY = 0;
				CK_SetAction2(ob,CK_ACTION(KD_ACT_keenjump3));
				ob->user2 = 1;
				jumptime = 0;
			}
		}
		break;
	case	KD_grapeobj:
		if (hit->currentAction == CK_ACTION(KD_ACT_grapefall))
			KD_KillKeen ();
		break;
	case	KD_tomatobj:
	case	KD_celeryobj:
	case	KD_asparobj:
	case	KD_turnipobj:
	case	KD_cauliobj:
	case	KD_brusselobj:
	case	KD_mushroomobj:
	case	KD_apelobj:
	case	KD_peabrainobj:
	case	KD_boobusobj:
	case	KD_shotobj:
			KD_KillKeen ();
		break;

	}
}


/*
=============================================================================

						 REACTION ROUTINES

=============================================================================
*/


/*
============================
=
= KeenSimpleReact
=
============================
*/

void	KD_KeenSimpleReact (CK_object *ob)
{
	PLACESPRITE;
}


/*
============================
=
= KeenStandReact
=
============================
*/

void	KD_KeenStandReact (CK_object *ob)
{
	if (!ob->topTI)
	{
	//
	// walked off an edge
	//
		SD_PlaySound (KD_PLUMMETSND);
		ob->velX = ob->xDirection*WALKAIRSPEED;
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_keenjump3));
		ob->user2 = 1;
		jumptime = 0;
	}
	else if ( (ob->topTI & ~7) == 8)	// deadly floor!
	{
		KD_KillKeen ();
	}

	PLACESPRITE;
}

/*
============================
=
= KeenWalkReact
=
============================
*/

void	KD_KeenWalkReact (CK_object *ob)
{
	if (!ob->topTI)
	{
	//
	// walked off an edge
	//
		ob->velX = ob->xDirection*WALKAIRSPEED;
		ob->velY = 0;
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_keenjump3));
		ob->user2 = 1;
		jumptime = 0;
	}
	else if ( (ob->topTI & ~7) == 8)	// deadly floor!
	{
		KD_KillKeen ();
		goto placeit;
	}

	if (ob->rightTI == 2)			// doors
	{

	}
	else if (ob->leftTI == 2)		// doors
	{

	}
	else if (ob->rightTI || ob->leftTI)
	{
	//
	// ran into a wall
	//
		ob->actionTimer = 0;
		ob->currentAction = CK_ACTION(KD_ACT_keenstand);
		ob->gfxChunk = ob->xDirection == 1 ? CK_ACTION(KD_ACT_keenstand)->chunkRight :
			CK_ACTION(KD_ACT_keenstand)->chunkLeft;
	}
placeit:

	PLACESPRITE;
}


/*
============================
=
= KeenAirReact
=
============================
*/

void	KD_KeenAirReact (CK_object *ob)
{
	int16_t x,y;
	uint16_t  *map,mapextra;

	if (ob->rightTI || ob->leftTI)
		ob->velX = 0;

	map = CA_mapPlanes[1] + (((ob->clipRects.tileY1)*CA_GetMapWidth())) + ob->clipRects.tileX1;
	mapextra = CA_GetMapWidth() - (ob->clipRects.tileX2 - ob->clipRects.tileX1+1);
	for (y=ob->clipRects.tileY1;y<=ob->clipRects.tileY2;y++,map+=mapextra)
		for (x=ob->clipRects.tileX1;x<=ob->clipRects.tileX2;x++,map++)
			if (KD_SOUTHWALL(*map) == 17)	// jumping up through a pole hole
			{
				ob->velX = 0;
				ob->posX = ob->clipRects.tileXmid*256 - 2*16;
				goto checknorth;
			}

	if (ob->bottomTI)
	{
		if (ob->bottomTI == 17)	// jumping up through a pole hole
		{
			ob->posY -= 32;
			ob->clipRects.unitY1 -= 32;
			ob->velX = 0;
			ob->posX = ob->clipRects.tileXmid*256 - 2*16;
		}
		else
		{
			SD_PlaySound (KD_HITHEADSND);

			if (ob->bottomTI > 1)
			{
				ob->velY += 16;
				if (ob->velY<0)	// push away from slopes
					ob->velY = 0;
			}
			else
				ob->velY = 0;
			jumptime = 0;
		}
	}

checknorth:
	if (ob->topTI)
	{
		if (!(ob->topTI == 25 && jumptime))	// KLUDGE to allow jumping off
		{										// sprites
			ob->user1 = ob->user2 = 0;
			CK_SetAction2 (ob,CK_ACTION(KD_ACT_keenstand));
			SD_PlaySound (KD_LANDSND);
		}
	}

	PLACESPRITE;
}

/*
============================
=
= KeenSlideReact
=
============================
*/

void	KD_KeenSlideReact (CK_object *ob)
{
	uint16_t  *map;

	if (ob->topTI)			// friction slow down
	{
		map = CA_mapPlanes[2] + (((ob->clipRects.tileY1)*CA_GetMapWidth()) + ob->clipRects.tileX1);
		if (!KD_SOUTHWALL(*map) && !KD_SOUTHWALL(*(map+1)))
			KD_FrictionX(ob);
	}


	if (ob->leftTI || ob->rightTI || !ob->velX)
		CK_SetAction2 (ob,CK_ACTION(KD_ACT_keengetup));

	PLACESPRITE;
}


void KD_Keen_SetupFunctions(void)
{
	CK_ACT_AddFunction("KD_CheckEnterLevel", &KD_CheckEnterLevel);
	CK_ACT_AddFunction("KD_KeenAirReact", &KD_KeenAirReact);
	CK_ACT_AddFunction("KD_KeenAirThink", &KD_KeenAirThink);
	CK_ACT_AddFunction("KD_KeenClimbThink", &KD_KeenClimbThink);
	CK_ACT_AddFunction("KD_KeenDieThink", &KD_KeenDieThink);
	CK_ACT_AddFunction("KD_KeenDropDownThink", &KD_KeenDropDownThink);
	CK_ACT_AddFunction("KD_KeenDropThink", &KD_KeenDropThink);
	CK_ACT_AddFunction("KD_KeenDuckThink", &KD_KeenDuckThink);
	CK_ACT_AddFunction("KD_KeenGoSleepThink", &KD_KeenGoSleepThink);
	CK_ACT_AddFunction("KD_KeenPauseThink", &KD_KeenPauseThink);
	CK_ACT_AddFunction("KD_KeenPoleThink", &KD_KeenPoleThink);
	CK_ACT_AddFunction("KD_KeenSimpleReact", &KD_KeenSimpleReact);
	CK_ACT_AddFunction("KD_KeenSleepThink", &KD_KeenSleepThink);
	CK_ACT_AddFunction("KD_KeenSlideReact", &KD_KeenSlideReact);
	CK_ACT_AddFunction("KD_KeenStandReact", &KD_KeenStandReact);
	CK_ACT_AddFunction("KD_KeenStandThink", &KD_KeenStandThink);
	CK_ACT_AddFunction("KD_KeenThrow", &KD_KeenThrow);
	CK_ACT_AddFunction("KD_KeenWalkReact", &KD_KeenWalkReact);
	CK_ACT_AddFunction("KD_KeenWalkThink", &KD_KeenWalkThink);
	CK_ACT_AddFunction("KD_KeenWorldThink", &KD_KeenWorldThink);
	CK_ACT_AddFunction("KD_KeenWorldWalk", &KD_KeenWorldWalk);
	CK_ACT_AddFunction("KD_PoleActions", &KD_PoleActions);
	CK_ACT_AddFunction("KD_PowerCount", &KD_PowerCount);
	CK_ACT_AddFunction("KD_PowerReact", &KD_PowerReact);
	CK_ACT_AddColFunction("KD_KeenContact", &KD_KeenContact);
	CK_ACT_AddColFunction("KD_PowerContact", &KD_PowerContact);
}
