/* Keen Dreams Gold Edition Source Code
 * Copyright (C) 2026 Szemigi
 * This project contains taken and modified
 * codes/codings from these following sources:
 *
 * 1.
 * Reconstructed BioMenace 1
 * Copyright (C) 2024 lethal_guitar
 * 
 * 2.
 * Foray in the Forest
 * Copyright (C) 2022-2024 Nisaba
 * Copyright (C) 2019-2025 K1n9_Duk3
 *
 * 3.
 * Atroxian Realm
 * Copyright (C) 2020 Gridlock
 *
 * 4.
 * Reconstructed Commander Keen 4-6
 * Copyright (C) 2021 K1n9_Duk3
 *
 * 5.
 * Keen Dreams Unofficial Tech Demo
 * Copyright (C) 2015 NY00123
 *
 * 6.
 * Wolfenstein 3-D Source Code
 * Copyright (C) 1995 Id Software
 *
 * 7.
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * 8.
 * Catacomb 3-D Source Code
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

// KDG_ACT1.C
#include "kdg_def.h"
#pragma hdrstop


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/


#define PLACESPRITE RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum, \
	spritedraw,0);

// For Mortimer
#define SPDPOWERUP		-64
#define SPDPOWER		64
#define SPDPOWERY		-20
#define SPDPOLESIDEJUMP	8
#define	JUMPTIME		16
#define	POLEJUMPTIME	10
#define	SPDJUMP			40
#define	SPDPOLEJUMP		20
#define	POLEJUMPTIME	10

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

int	flowertime[6] = {700,700,350,175,100,100};

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


void DoGravity (objtype *ob)
{
	long	i;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -ACCGRAVITY)
			{
			// stop at apex of jump
				ob->ymove += ob->yspeed;
				ob->yspeed = 0;
				return;
			}
			ob->yspeed+=ACCGRAVITY;
			if (ob->yspeed>SPDMAXY)
			  ob->yspeed=SPDMAXY;
		}
		ob->ymove+=ob->yspeed;
	}
}


/*
====================
=
= DoWeakGravity
=
====================
*/

void DoWeakGravity(objtype *ob)
{
	int tickCount;
	int lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount != lastTimeCount; tickCount++)
	{
		if (tickCount & 1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -3)
			{
				ob->ymove += ob->yspeed;
				ob->yspeed = 0;
				return;
			}

			ob->yspeed += 3;
			if (tickCount & 2)
				ob->yspeed++;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ob->ymove+=ob->yspeed;
	}
}


/*
====================
=
= DoHighGravity
=
====================
*/

void DoHighGravity(objtype *ob)
{
	int tickCount;
	int lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount != lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -4)
			{
				ob->ymove += ob->yspeed;
				ob->yspeed = 0;
				return;
			}
			ob->yspeed += 4;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ob->ymove+=ob->yspeed;
	}
}


/*
====================
=
= DoMidGravity
=
====================
*/

void DoMidGravity(objtype *ob)
{
	int tickCount;
	int lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount != lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			if (ob->yspeed < 0 && ob->yspeed >= -3)
			{
				ob->ymove += ob->yspeed;
				ob->yspeed = 0;
				return;
			}
			ob->yspeed += 3;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ob->ymove+=ob->yspeed;
	}
}


/*
====================
=
= DoLowGravity
=
====================
*/

void DoLowGravity(objtype *ob)
{
	long tickCount;
	long lastTimeCount = lasttimecount;
	for (tickCount = lastTimeCount - tics; tickCount < lastTimeCount; tickCount++)
	{
		if ((tickCount?0:1) & 3)	// BUG: this effectively means "(if (tickCount == 0))"
		{
			ob->yspeed += 1;
			if (ob->yspeed > 70)
			{
				ob->yspeed = 70;
			}
		}
		ob->ymove+=ob->yspeed;
	}
}



/*
===============
=
= AccelerateX
=
===============
*/

void AccelerateX (objtype *ob,int dir,int max)
{
	long	i;
	unsigned	olddir;

	olddir = ob->xspeed & 0x8000;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			ob->xspeed+=dir;
			if ( (ob->xspeed & 0x8000) != olddir)
			{
				olddir = ob->xspeed & 0x8000;
				ob->xdir = olddir ? -1 : 1;
			}
			if (ob->xspeed>max)
			  ob->xspeed=max;
			else if (ob->xspeed<-max)
			  ob->xspeed=-max;
		}
		ob->xmove+=ob->xspeed;
	}
}


/*
===============
=
= AccelerateXv
=
= Doesn't change object's xdir
=
===============
*/

void AccelerateXv(objtype *ob, int dir, int maxspeed)
{
	int i;

//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->xspeed += dir;
			if (ob->xspeed > maxspeed)
			{
				ob->xspeed = maxspeed;
			}
			else if (ob->xspeed < -maxspeed)
			{
				ob->xspeed = -maxspeed;
			}
		}
		ob->xmove+= ob->xspeed;
	}
}


/*
===============
=
= AccelerateY
=
===============
*/

void AccelerateY(objtype *ob, int dir, int maxspeed)
{
	int i;

//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics; i<lasttimecount; i++)
	{
		if (i & 1)
		{
			ob->yspeed += dir;
			if (ob->yspeed > maxspeed)
			{
				ob->yspeed = maxspeed;
			}
			else if (ob->yspeed < -maxspeed)
			{
				ob->yspeed = -maxspeed;
			}
		}
		ob->xmove+= ob->yspeed;
	}
}


/*
===============
=
= FrictionX
=
===============
*/

void FrictionX (objtype *ob)
{
	long	i;
	int		dir;
	unsigned	olddir;

	olddir = ob->xspeed & 0x8000;

	if (ob->xspeed > 0)
		dir = -1;
	else if (ob->xspeed < 0)
		dir = 1;
	else
		dir = 0;
//
// only accelerate on odd tics, because of limited precision
//
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			ob->xspeed+=dir;
			if ( (ob->xspeed & 0x8000) != olddir)
				ob->xspeed = 0;
		}
		ob->xmove+=ob->xspeed;
	}
}


/*
===============
=
= ProjectileThink
=
===============
*/

void ProjectileThink (objtype *ob)
{
	DoGravity (ob);
	ob->xmove = tics*ob->xspeed;
}

/*
===============
=
= WeakProjectileThink
=
===============
*/

void WeakProjectileThink(objtype *ob)
{
	DoWeakGravity(ob);
	ob->xmove = ob->xspeed*tics;
}

/*
===============
=
= VelocityThink
=
===============
*/

void VelocityThink (objtype *ob)
{
	ob->xmove = tics*ob->xspeed;
	ob->ymove = tics*ob->yspeed;
}

/*
===============
=
= DrawReact
=
===============
*/

void DrawReact (objtype *ob)
{
	RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,1);
}

void DrawReact2 (objtype *ob)
{
	RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,2);
}

void DrawReact3 (objtype *ob)
{
	RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,3);
}

void DrawReact4 (objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1)
	{
		shape += POOF5SPR - POOF1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,2);
}

void DrawReact5 (objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand && mapon == 26)
	{
		shape += MORTEXIT1SPR - KEENEXIT1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,1);
}

void DrawReact6 (objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += BOOBUSBOOM4SPR - BOOBUSBOOM1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,1);
}

void DrawReact7 (objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += POOF5SPR - POOF1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,3);
}

void DrawReact8 (objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += BOOBUSBOOM4SPR - BOOBUSBOOM1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,3);
}

void DrawReact9 (objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += MORTLOWSHOOTRSPR - KEENLOWSHOOTRSPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,1);
}

void DrawReact10 (objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
==================
=
= ChangeState
=
==================
*/

void ChangeState (objtype *ob, statetype near *state)
{
	ob->state = state;
	ob->ticcount = 0;
	if (state->rightshapenum)
	{
		if (ob->xdir>0)
			ob->shapenum = state->rightshapenum;
		else
			ob->shapenum = state->leftshapenum;
	}

	ob->xmove = 0;
	ob->ymove = 0;

	ob->needtoreact = true;			// it will need to be redrawn this frame
	ClipToWalls (ob);
}


/*
====================
=
= WalkReact
=
====================
*/

void WalkReact (objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}
	else if (!ob->hitnorth)
	{
		ob->x -= 2*ob->xmove;
		ob->y -= ob->ymove;

		ob->xdir = -ob->xdir;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}

	PLACESPRITE;
}



/*
=============================================================================

				DOOR / METAL DOOR

=============================================================================
*/

statetype near s_door  	 = {DOORSPR,DOORSPR,think,false,
	false,0, 0,0, NULL, DoorContact, DrawDoor, &s_door};
statetype near s_doorraise = {DOORSPR,DOORSPR,slide,false,
	false,24, 0,32, NULL, DoorContact, DrawDoor, NULL};

statetype near s_metaldoor  	 = {KMETALDOORSPR,KMETALDOORSPR,think,false,
	false,0, 0,0, NULL, DoorContact, DrawMetalDoor, &s_metaldoor};
statetype near s_metaldoorraise = {KMETALDOORSPR,KMETALDOORSPR,slide,false,
	false,24, 0,32, NULL, DoorContact, DrawMetalDoor, NULL};

/*
======================
=
= SpawnDoor
=
======================
*/

void	SpawnDoor (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = doorobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = -1;
	new->needtoclip = false;
	NewState (new,&s_door);
}

/*
======================
=
= SpawnMetalDoor
=
======================
*/

void	SpawnMetalDoor (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = metaldoorobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = -1;
	new->needtoclip = false;
	NewState (new,&s_metaldoor);
}

/*
======================
=
= DoorContact
=
======================
*/

void	DoorContact (objtype *ob, objtype *hit)
{
	ClipToSpriteSide (hit,ob);
}

/*
============
=
= DrawDoor
=
============
*/

void DrawDoor(objtype *ob)
{
	int shape = ob->shapenum;

	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += MDOORSPR - DOORSPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}

/*
================
=
= DrawMetalDoor
=
================
*/

void DrawMetalDoor(objtype *ob)
{
	int shape = ob->shapenum;

	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += MMETALDOORSPR - KMETALDOORSPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}

/*
=============================================================================

							FLOWER

temp1 = original class
temp2 = original state
temp3 = flower count

=============================================================================
*/

void ChangeToFlower (objtype *ob);
void FlowerThink (objtype *ob);
void ChangeFromFlower (objtype *ob);


#pragma warn -sus

statetype near s_flower1  	 = {FLOWER6SPR,FLOWER1SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower2};
statetype near s_flower2  	 = {FLOWER7SPR,FLOWER2SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower3};
statetype near s_flower3  	 = {FLOWER8SPR,FLOWER3SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower4};
statetype near s_flower4  	 = {FLOWER9SPR,FLOWER4SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower5};
statetype near s_flower5  	 = {FLOWER8SPR,FLOWER3SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower6};
statetype near s_flower6  	 = {FLOWER7SPR,FLOWER2SPR,stepthink,false,
	false,20, 0,0, FlowerThink, NULL, DrawReact, &s_flower1};

statetype near s_flower7  	 = {FLOWER10SPR,FLOWER5SPR,stepthink,false,
	false,20, 0,0, ProjectileThink, NULL, DrawReact, &s_flower7};

statetype near s_poofto1	 = {POOF5SPR,POOF1SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_poofto2};
statetype near s_poofto2	 = {POOF6SPR,POOF2SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_poofto3};
statetype near s_poofto3	 = {POOF7SPR,POOF3SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_poofto4};
statetype near s_poofto4	 = {POOF8SPR,POOF4SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, NULL};

statetype near s_poof	 = {-1,-1,step,false,
	false,10, 0,0, NULL, NULL, DrawReact, NULL};

statetype near s_pooffrom1  	 = {POOF8SPR,POOF4SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_pooffrom2};
statetype near s_pooffrom2  	 = {POOF7SPR,POOF3SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_pooffrom3};
statetype near s_pooffrom3  	 = {POOF6SPR,POOF2SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_pooffrom4};
statetype near s_pooffrom4  	 = {POOF5SPR,POOF1SPR,step,false,
	false,20, 0,0, ChangeFromFlower, NULL, DrawReact4, &s_pooffrom5};
statetype near s_pooffrom5  	 = {POOF6SPR,POOF2SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_pooffrom6};
statetype near s_pooffrom6  	 = {POOF7SPR,POOF3SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, &s_pooffrom7};
statetype near s_pooffrom7  	 = {POOF8SPR,POOF4SPR,step,false,
	false,10, 0,0, NULL, NULL, DrawReact4, NULL};

#pragma warn +sus

/*
======================
=
= ChangeToFlower
=
======================
*/

void ChangeToFlower (objtype *ob)
{
	SD_PlaySound (FLOWERPOWERSND);
	ob->y = ob->bottom-TILEGLOBAL*2;
	ob->temp1 = (int)ob->obclass;
	// Workaround: Depending  on the DOS version/emulator used, DOS drivers 
	// loaded,  etc. the program may be loaded at different addresses in DOS
	// real mode memory. To avoid saved game incompatibilities due to this,
	// segments relative to _SS will be saved. The _far statetypes should 
	// have segments below  the main stack (and data) segment when the program 
	// is compiled with Borland C++, so there should be no underflow.   
	
	// Keen Worlds companion: the state is saved as its number in the state
	// table (kdg_states.c), which is stable across runs and saved games.
	ob->temp2 = (int)StateToInt(ob->state);
	ob->temp3 = 0;
	ob->temp4 = 0;
	ob->needtoclip = true;
	ob->obclass = inertobj;
	ob->xspeed = 0;
	if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Grand && mapon >= 1 && mapon <= 10)
	{
		ChangeState (ob,&s_flower7);
	}
	else
	{
		ChangeState (ob,&s_flower1);
		ob->active = allways;			// flower never deactivated
	}
	GetNewObj (true);
	new->x = ob->x;
	new->y = ob->y;
	new->xdir = ob->xdir;
	NewState (new,&s_poofto1);
	new->active = removable;
}


/*
======================
=
= FlowerThink
=
======================
*/

void FlowerThink (objtype *ob)
{
	ProjectileThink (ob);
	if ( (ob->temp3+=tics) >= flowertime[gamestate.difficulty])
	{
		GetNewObj (true);
		new->active = allways;
		new->temp1 = ObjToInt(ob);
		new->x = ob->x;
		new->y = ob->y;
		new->xdir = ob->xdir;
		new->obclass = pooffromobj;
		NewState (new,&s_pooffrom1);
		ob->temp3 = 0;
	}
}


/*
======================
=
= ChangeFromFlower
=
======================
*/

void ChangeFromFlower (objtype *ob)
{
	objtype *flower;
	// statetype near *state;
	unsigned	oldbottom;

	new->obclass = nothing;
	SD_PlaySound (UNFLOWERPOWERSND);
	flower = IntToObj(ob->temp1);

	oldbottom = flower->bottom;
	// Segments relative to _SS are saved, see comment in ChangeToFlower
	ChangeState (flower,IntToState(flower->temp2));
	flower->y += oldbottom - flower->bottom;
	flower->temp4 = 0;
	flower->obclass = flower->temp1;
	flower->active = yes;			// allow it to unspawn now if off screen
}


/*
=============================================================================

							BONUS

temp1 = bonus type
temp2 = base shape number
temp3 = last animated shape number +1

=============================================================================
*/

void BonusThink (objtype *ob);

#pragma warn -sus

statetype near s_bonus  	 = {NULL,NULL,step,false,
	false,20, 0,0, BonusThink, NULL, DrawReact2, &s_bonus};

statetype near s_bonusrise  	 = {NULL,NULL,slide,false,
	false,40, 0,8, NULL, NULL, DrawReact3, NULL};

#pragma warn +sus

/*
====================
=
= SpawnBonus
=
====================
*/

int originalbonusshape[12] = {PEPPERMINT1SPR,COOKIE1SPR,CANDYCANE1SPR,CANDYBAR1SPR,
	LOLLIPOP1SPR,COTTONCANDY1SPR,EXTRAKEEN1SPR,SUPERBONUS1SPR,FLOWERPOWER1SPR,
	FLOWERPOWERUP1SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

int secretbonusshape[12] = {SYRUP1SPR,GUM1SPR,CHOCOLATE1SPR,POPSICLE1SPR,
	BURGER1SPR,FRENCHFRIES1SPR,EXTRAKEEN3SPR,SUPERBONUS1SPR,VORTIPISTOL1SPR,
	VORTIPISTOLAMMO1SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

int easybonusshape[12] = {PEPPERMINT1SPR,COOKIE1SPR,CANDYCANE1SPR,CANDYBAR1SPR,
	LOLLIPOP1SPR,COTTONCANDY1SPR,EXTRAKEEN1SPR,SUPERBONUS5SPR,FLOWERPOWER3SPR,
	FLOWERPOWERUP3SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

int easysecretbonusshape[12] = {SYRUP1SPR,GUM1SPR,CHOCOLATE1SPR,POPSICLE1SPR,
	BURGER1SPR,FRENCHFRIES1SPR,EXTRAKEEN3SPR,SUPERBONUS5SPR,VORTIPISTOL1SPR,
	VORTIPISTOLAMMO1SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

int mortbonusshape[12] = {PEPPERMINT1SPR,COOKIE1SPR,CANDYCANE1SPR,CANDYBAR1SPR,
	LOLLIPOP1SPR,COTTONCANDY1SPR,EXTRAMORT1SPR,SUPERBONUS7SPR,FLOWERPOWER5SPR,
	FLOWERPOWERUP5SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

int mortsecretbonusshape[12] = {SYRUP1SPR,GUM1SPR,CHOCOLATE1SPR,POPSICLE1SPR,
	BURGER1SPR,FRENCHFRIES1SPR,EXTRAMORT3SPR,SUPERBONUS7SPR,PISTOL1SPR,
	PISTOLAMMO1SPR,BOOBUSBOMB1SPR,MAGICKEY1SPR};

void SpawnBonus (int tilex, int tiley, int type)
{
	GetNewObj (false);

	new->needtoclip = false;
	new->obclass = bonusobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;

	if (type == 9)
		new->y -= 8*PIXGLOBAL;	// flower power up one block

	new->ydir = -1;			// bonus stuff flies up when touched

	new->temp1 = type;
	if (gamestate.difficulty == gd_Easy)
		if (mapon >= 23 && mapon <= 46)
			new->temp2 = new->shapenum = easysecretbonusshape[type];
		else
			new->temp2 = new->shapenum = easybonusshape[type];
	else if (gamestate.difficulty == gd_Grand)
		if (mapon >= 23 && mapon <= 46)
			new->temp2 = new->shapenum = mortsecretbonusshape[type];
		else
			new->temp2 = new->shapenum = mortbonusshape[type];
	else
		if (mapon >= 23 && mapon <= 46)
			new->temp2 = new->shapenum = secretbonusshape[type];
		else
			new->temp2 = new->shapenum = originalbonusshape[type];	
	if (type != 7)
		new->temp3 = new->temp2 + 2;
	else
		if (gamestate.difficulty == gd_Easy)
			new->temp3 = new->temp2 + 2;	// red eye bonus is 2 stage animation only in easy mode
		else if (gamestate.difficulty == gd_Grand)
			new->temp3 = new->temp2 + 3;	// blue eye bonus is 3 stage animation only in grand mode
		else
			new->temp3 = new->temp2 + 4;	// super bonus is 4 stage animation

	NewState (new,&s_bonus);
}

/*
====================
=
= BonusThink
=
====================
*/

void BonusThink (objtype *ob)
{
	if (++ob->shapenum == ob->temp3)
		ob->shapenum = ob->temp2;
}



/*
=============================================================================

						  MORTIMER MCMIRE

=============================================================================
*/

#define SPDMORTLEAPY  50
#define MORTXSPEED	40
#define MORTYSPEED	-20
#define MORTJUMPXSPEEDWALKREACT	30

// Note (ktx58): These parameters should be modified for
// optimal behavior.
#define DUCKPROBABILITYAFTERJUMP 30
#define DUCKAFTERJUMPDURATION 30

#define BFPXSPEED	40
#define BFPYSPEED	-20

#pragma warn -sus

statetype near s_mortstand	= {MORTSTANDLSPR,MORTSTANDRSPR,stepthink,false,
	true,200, 0,16, MortStopThink, NULL, DrawReact, &s_mortstand};

statetype near s_mortjump1	= {MORTJUMPL1SPR,MORTJUMPR1SPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortjump1};
statetype near s_mortjump2	= {MORTJUMPL2SPR,MORTJUMPR2SPR,think,false,
	false,0, 0,0, MortAirThrow, NULL, MortJumpReact, &s_mortjump1};

statetype near s_mortjumpup1	= {MORTJUMPUL1SPR,MORTJUMPUR1SPR,stepthink,false,
	false,20, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortjumpup2};
statetype near s_mortjumpup2	= {MORTJUMPUL2SPR,MORTJUMPUR2SPR,stepthink,false,
	false,20, 0,0, MortAirThrow, NULL, MortJumpReact, &s_mortjumpup3};
statetype near s_mortjumpup3	= {MORTJUMPUL1SPR,MORTJUMPUR1SPR,stepthink,false,
	false,20, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortjumpup1};

statetype near s_mortduck= {MORTDUCKSPR,MORTDUCKSPR,stepthink,true,
	true,200, 0,0, MortDuckThink, NULL, DrawReact, &s_mortduck};

statetype near s_mortairthrowup1= {MORTJLTHROWU1SPR,MORTJRTHROWU1SPR,stepthink,false,
	false,8, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortairthrowup2};
statetype near s_mortairthrowup2= {MORTJLTHROWU2SPR,MORTJRTHROWU2SPR,step,true,
	false,1, 0,0, MortThrow, NULL, MortJumpReact, &s_mortairthrowup3};
statetype near s_mortairthrowup3= {MORTJLTHROWU2SPR,MORTJRTHROWU2SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortjump1};

statetype near s_mortairthrowdown1= {MORTJLTHROWD1SPR,MORTJRTHROWD1SPR,step,false,
	false,8, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortairthrowdown2};
statetype near s_mortairthrowdown2= {MORTJLTHROWD2SPR,MORTJRTHROWD2SPR,step,true,
	false,1, 0,0, MortThrow, NULL, MortJumpReact, &s_mortairthrowdown3};
statetype near s_mortairthrowdown3= {MORTJLTHROWD2SPR,MORTJRTHROWD2SPR,step,false,
	false,10, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortjumpup3};

statetype near s_mortwalk1	= {MORTRUNL1SPR,MORTRUNR1SPR,step,true,
	true,6, 128,0, WalkSound, NULL, MortWalkReact, &s_mortwalk2};
statetype near s_mortwalk2	= {MORTRUNL2SPR,MORTRUNR2SPR,step,true,
	true,6, 128,0, NULL, NULL, MortWalkReact, &s_mortwalk3};
statetype near s_mortwalk3	= {MORTRUNL3SPR,MORTRUNR3SPR,step,true,
	true,6, 128,0, WalkSound, NULL, MortWalkReact, &s_mortwalk4};
statetype near s_mortwalk4	= {MORTRUNL4SPR,MORTRUNR4SPR,step,true,
	true,6, 128,0, MortThink, NULL, MortWalkReact, &s_mortwalk1};

statetype near s_mortthrow1	= {MORTTHROWL1SPR,MORTTHROWR1SPR,step,false,
	false,4, 0,0, NULL, NULL, DrawReact, &s_mortthrow2};
statetype near s_mortthrow2	= {MORTTHROWL2SPR,MORTTHROWR2SPR,step,false,
	false,4, 0,0, NULL, NULL, DrawReact, &s_mortthrow3};
statetype near s_mortthrow3	= {MORTTHROWL3SPR,MORTTHROWR3SPR,step,false,
	false,10, 0,0, MortThrow, NULL, DrawReact, &s_mortthrow4};
statetype near s_mortthrow4	= {MORTTHROWL3SPR,MORTTHROWR3SPR,step,false,
	false,20, -128,0, NULL, NULL, DrawReact, &s_mortwalk1};

statetype near s_mortthrowup1	= {MORTTHROWU1SPR,MORTTHROWU3SPR,step,false,
	false,8, 0,0, NULL, NULL, DrawReact, &s_mortthrowup2};
statetype near s_mortthrowup2	= {MORTTHROWU2SPR,MORTTHROWU4SPR,step,false,
	false,1, 0,0, MortThrow, NULL, DrawReact, &s_mortthrowup3};
statetype near s_mortthrowup3	= {MORTTHROWU2SPR,MORTTHROWU4SPR,step,false,
	false,10, -128,0, NULL, NULL, DrawReact, &s_mortwalk1};

statetype near s_mortairthrow1	= {MORTJLTHROWL2SPR,MORTJRTHROWR2SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortairthrow2};
statetype near s_mortairthrow2	= {MORTJLTHROWL1SPR,MORTJRTHROWR1SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, MortJumpReact, &s_mortairthrow3};
statetype near s_mortairthrow3	= {MORTJLTHROWL1SPR,MORTJRTHROWR1SPR,step,true,
	false,10, 0,0, MortThrow, NULL, MortJumpReact, &s_mortairthrow4};
statetype near s_mortairthrow4	= {MORTJLTHROWL2SPR,MORTJRTHROWR2SPR,stepthink,false,
	false,20, -128,0, ProjectileThink, NULL, MortJumpReact, &s_mortjumpup3};

statetype near s_mortdie1		= {MORTDREAM1SPR,MORTDREAM1SPR,step,false,
	false,20, 0,0, NULL, NULL, DrawReact, &s_mortdie2};
statetype near s_mortdie2		= {MORTDREAM2SPR,MORTDREAM2SPR,step,false,
	false,20, 0,0, NULL, NULL, DrawReact, &s_mortdie3};
statetype near s_mortdie3		= {MORTDREAM3SPR,MORTDREAM3SPR,step,false,
	false,20, 0,0, NULL, NULL, DrawReact, &s_mortdie3};

statetype near s_blueflowerpower1		= {BLUEFLOWERPOWER1SPR,BLUEFLOWERPOWER1SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact, &s_blueflowerpower2};
statetype near s_blueflowerpower2		= {BLUEFLOWERPOWER2SPR,BLUEFLOWERPOWER2SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact, &s_blueflowerpower1};

#pragma warn +sus


/*
===============
=
= SpawnMort
=
===============
*/

objtype		*mortinlevel;

void SpawnMort (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = mortobj;
	new->needtoclip = true;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT) - BLOCKSIZE*2;

	new->xdir = 1;
	new->ydir = -1;
	if (gamestate.text_mort == true)
		NewState (new,&s_mortwalk1);
	else
		NewState (new,&s_mortstand);
	mortinlevel = new;
	
	switch (gamestate.difficulty)
	{
		case 1:
			new->temp4 = 10; break;
		case 2:
			new->temp4 = 20; break;
		case 3:
			new->temp4 = 30; break;
		case 4:
			new->temp4 = 40; break;
	}
}

/*
====================
=
= MortWalkReact
=
====================
*/

void MortWalkReact (objtype *ob)
{
	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->x -= ob->xmove;
		ob->xdir = -1;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->x -= ob->xmove;
		ob->xdir = 1;
		ob->nothink = US_RndT()>>5;
		ChangeState (ob,ob->state);
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ob->y -= ob->ymove;

		if (US_RndT() < 0x80 && gamestate.difficulty == gd_Keen)
			ob->yspeed = -10;
		else
			ob->yspeed = -SPDMORTLEAPY;
		ob->xspeed = MORTJUMPXSPEEDWALKREACT * ob->xdir;

		if (US_RndT() < 0x80 && gamestate.difficulty == gd_Keen)
			SD_PlaySound (THROWSND);
		else
			SD_PlaySound (JUMPSND);
		if (US_RndT() < 0x80 && gamestate.difficulty == gd_Keen)
			ChangeState (ob,&s_mortjumpup1);
		else
			ChangeState (ob,&s_mortjump1);
	}

	PLACESPRITE;
}


/*
====================
=
= MortStopThink
=
====================
*/

void MortStopThink (objtype *ob)
{
	ob->needtoreact = true;
	if (player->right < ob->left)
		ob->xdir = -1;
	if (player->left > ob->right)
		ob->xdir = 1;
}

/*
====================
=
= MortDuckThink
=
====================
*/

void MortDuckThink (objtype *ob)
{
	if (--ob->temp3)
	{
		MortStopThink(ob);
	}
	else
	{
		// Note (ktx58): Some extra code (xdir, xspeed change?) may be needed
		// here, please check
		ob->state = &s_mortwalk1;
	}
}

/*
====================
=
= MortAirThrow
=
====================
*/

void MortAirThrow (objtype *ob)
{
	GetNewObj (true);
	new->obclass = shotobj;
	SD_PlaySound (THROWSND);
	new->xdir = ob->xdir;
	new->ydir = ob->ydir;
	new->xspeed = ob->xspeed;
	new->yspeed = ob->yspeed;
	if (US_RndT() < 0x80 && gamestate.difficulty == gd_Keen)
		ChangeState (ob,&s_mortairthrowup1);
	else if (US_RndT() > 0x80 && gamestate.difficulty == gd_Keen)
		ChangeState (ob,&s_mortairthrow1);
	else if (US_RndT() > 0x80 && gamestate.difficulty == gd_Keen)
		ChangeState (ob,&s_mortairthrowdown1);
}

/*
====================
=
= MortJumpReact
=
====================
*/

void MortJumpReact (objtype *ob)
{
	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		if (US_RndT() < DUCKPROBABILITYAFTERJUMP)
		{
			ob->xspeed = 0;
			ob->yspeed = 0;
			ob->temp3 = DUCKAFTERJUMPDURATION;
			ChangeState (ob,&s_mortduck);

		}
		else
		{
			SD_PlaySound (THROWSND);
			if (US_RndT() < 0x80 && gamestate.difficulty == gd_Hard || US_RndT() < 0x80 && gamestate.difficulty == gd_Keen)
				ChangeState (ob,&s_mortthrowup1);
			else
				ChangeState (ob,&s_mortthrow3);
		}
	}

	PLACESPRITE;
}

/*
====================
=
= MortThink
=
====================
*/

void MortThink (objtype *ob)
{
	int delta;

	if ( abs(ob->y - player->y) > 6*TILEGLOBAL )
	{
		if (US_RndT()<8)
			ob->xdir = -ob->xdir;		// turn randomly
		return;
	}


	delta = player->x - ob->x;

	if ( ob->xdir == -1 )
	{
		if (delta < -6*TILEGLOBAL)
			return;
		if (delta > 8*PIXGLOBAL)
		{
//			SD_PlaySound (JUMPSND);
			ob->xdir = 1;
			return;
		}
	}
	else
	{
		if (delta > 6*TILEGLOBAL)
			return;
		if (delta < -8*PIXGLOBAL)
		{
			ob->xdir = 1;
			return;
		}
	}

	SD_PlaySound (JUMPSND);
	if (US_RndT() < 0x80 && gamestate.difficulty == gd_Keen)
	{
		ob->xspeed = 0;
		ob->yspeed = -SPDMORTLEAPY;
		ob->xmove = 0;
		ob->ymove = 0;		// ob->yspeed*tics;			// DEBUG
		ob->state = &s_mortjumpup1;
		return;
	}
	else
	{
		ob->yspeed = -SPDMORTLEAPY;
		ob->xspeed = delta/60;
		ob->state = &s_mortjump1;
	}
	return;
}


/*
======================
=
= ThrowBlueFlowerPower
=
======================
*/

void ThrowBlueFlowerPower (unsigned x, unsigned y, int dir)
{
	statetype near *startstate;

	if (gamestate.difficulty == gd_Easy)
	{
		SD_PlaySound (NOWAYSND);
		return;						// nothing to throw if easy mode
	}

	GetNewObj (true);
	new->obclass = shotobj;
	new->temp2 = dir;
	new->x = x;
	new->y = y;
	new->tileleft = new->tileright = x>>G_T_SHIFT;
	new->tiletop = new->tilebottom = y>>G_T_SHIFT;
	new->ydir = -1;

	switch (dir)
	{
	case dir_North:
		new->xspeed = 0;
		new->yspeed = SPDPOWERUP;
		break;
	case dir_South:
		new->xspeed = 0;
		new->yspeed = SPDPOWER;
		break;
	case dir_East:
		new->xspeed = SPDPOWER;
		new->yspeed = SPDPOWERY;
		break;
	case dir_West:
		new->xspeed = -SPDPOWER;
		new->yspeed = SPDPOWERY;
		break;
	}

	startstate = &s_blueflowerpower1;

	NewState (new,startstate);
}


/*
=======================
=
= MortThrow
=
=======================
*/

void	MortThrow	(objtype *ob)
{
	if (ob->state == &s_mortthrowup2)
	{
		ThrowBlueFlowerPower (ob->x+4*PIXGLOBAL,ob->y-8*PIXGLOBAL,dir_North);
		return;
	}

	if (ob->state == &s_mortairthrowup2)
	{
		if (ob->xdir > 0)
			ThrowBlueFlowerPower (ob->x+16*PIXGLOBAL,ob->y,dir_North);
		else
			ThrowBlueFlowerPower (ob->x+4*PIXGLOBAL,ob->y,dir_North);
		new->xspeed += ob->xspeed/2;
		new->yspeed += ob->yspeed/2;
		return;
	}

	if (ob->state == &s_mortairthrowdown2)
	{
		if (ob->xdir > 0)
			ThrowBlueFlowerPower (ob->x+3*PIXGLOBAL,ob->y+16*PIXGLOBAL,dir_South);
		else
			ThrowBlueFlowerPower (ob->x+12*PIXGLOBAL,ob->y+16*PIXGLOBAL,dir_South);
		new->xspeed += ob->xspeed/2;
		new->yspeed += ob->yspeed/2;
		return;
	}

	if (ob->state == &s_mortthrow3)
	{
		if (ob->xdir > 0)
			ThrowBlueFlowerPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_East);
		else
			ThrowBlueFlowerPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_West);
		return;
	}

	if (ob->state == &s_mortairthrow3)
	{
		if (ob->xdir > 0)
			ThrowBlueFlowerPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_East);
		else
			ThrowBlueFlowerPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_West);

		new->xspeed += ob->xspeed/2;
		new->yspeed += ob->yspeed/2;
		return;
	}

}


/*
=============================================================================

						   EGGPLANT

=============================================================================
*/

#pragma warn -sus

statetype near s_eggplantwalk1	= {EGGPLANTWALKL1SPR,EGGPLANTWALKR1SPR,step,false,
	true,10, 128,0, NULL, ShotContact, WalkReact, &s_eggplantwalk2};
statetype near s_eggplantwalk2	= {EGGPLANTWALKL2SPR,EGGPLANTWALKR2SPR,step,false,
	true,10, 128,0, NULL, ShotContact, WalkReact, &s_eggplantwalk1};
statetype near s_eggplantdead = {EGGPLANTDEADSPR,EGGPLANTDEADSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus


/*
====================
=
= SpawnEggplant
=
====================
*/

void SpawnEggplant (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = eggplantobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_eggplantwalk1);
}


/*
=============================================================================

				CAMEO SET 1: SNAKE, DAVE, ROVER

=============================================================================
*/

#pragma warn -sus

statetype near s_snake	= {SNAKESPR,SNAKESPR,step,true,
	true,0, 0,0, NULL, NULL, DrawReact, &s_snake};

statetype near s_dave	= {DAVESPR,DAVESPR,step,true,
	true,0, 0,0, NULL, NULL, DrawReact, &s_dave};

statetype near s_rover1	= {ROVER1SPR,ROVER1SPR,step,true,
	true,30, 0,0, NULL, NULL, DrawReact, &s_rover2};
statetype near s_rover2	= {ROVER2SPR,ROVER2SPR,step,true,
	true,70, 0,0, NULL, NULL, DrawReact, &s_rover1};

#pragma warn +sus

/*
====================
=
= SpawnCameo1
=
====================
*/

void SpawnCameo1 (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = cameo1obj;
	new->x = tilex<<G_T_SHIFT;
	if (mapon == 6)
		new->y = (tiley<<G_T_SHIFT)-3*BLOCKSIZE;
	else
		new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	switch (mapon)
	{
	case 6:
		NewState (new,&s_snake);
		break;
	case 8:
		NewState (new,&s_dave);
		break;
	case 26:
		NewState (new,&s_rover1);
		break;
	}
}


/*
=============================================================================

		CAMEO SET 2: VORTICON (RED, YELLOW, BLUE) AND SPOT

=============================================================================
*/

#pragma warn -sus

statetype near s_redvort1	= {REDVORTICONGRUNT1SPR,REDVORTICONGRUNT1SPR,step,true,
	true,30, 0,0, NULL, NULL, DrawReact, &s_redvort2};
statetype near s_redvort2	= {REDVORTICONGRUNT2SPR,REDVORTICONGRUNT2SPR,step,true,
	true,70, 0,0, NULL, NULL, DrawReact, &s_redvort1};

statetype near s_yellowvort1	= {YELLOWVORTICONGRUNT1SPR,YELLOWVORTICONGRUNT1SPR,step,true,
	true,30, 0,0, NULL, NULL, DrawReact, &s_yellowvort2};
statetype near s_yellowvort2	= {YELLOWVORTICONGRUNT2SPR,YELLOWVORTICONGRUNT2SPR,step,true,
	true,70, 0,0, NULL, NULL, DrawReact, &s_yellowvort1};

statetype near s_bluevort1	= {VORTICOMMANDER1SPR,VORTICOMMANDER1SPR,step,true,
	true,30, 0,0, NULL, NULL, DrawReact, &s_bluevort2};
statetype near s_bluevort2	= {VORTICOMMANDER2SPR,VORTICOMMANDER2SPR,step,true,
	true,70, 0,0, NULL, NULL, DrawReact, &s_bluevort1};

statetype near s_spot	= {SPOTSPR,SPOTSPR,step,true,
	true,30, 0,0, NULL, NULL, DrawReact, &s_spot};

#pragma warn +sus

/*
====================
=
= SpawnCameo2
=
====================
*/

void SpawnCameo2 (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = cameo2obj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	switch (gamestate.difficulty)
	{
	case 1:
		NewState (new,&s_redvort1);
		break;
	case 2:
		NewState (new,&s_yellowvort1);
		break;
	case 3:
		NewState (new,&s_bluevort1);
		break;
	case 4:
		NewState (new,&s_spot);
		break;
	}
}


/*
=============================================================================

						  GARGOYLE

=============================================================================
*/

statetype near s_gargoyleface      = {GARGOYLEFACESPR,      GARGOYLEFACESPR,      think, false, true,      8, 0, 0, NULL, FaceContact, DrawReact, NULL};
statetype near s_gargoylebreakout = {GARGOYLEBOSPR, GARGOYLEBOSPR, think,  false, false, 30000, 0, 0, ProjectileThink, NULL, StonePieceReaction, NULL};
statetype near s_gargoyleexplode1 = {GARGOYLEEXPLODE1SPR, GARGOYLEEXPLODE1SPR, think, false, false,     0, 0, 0, ProjectileThink, NULL, StonePieceReaction, NULL};
statetype near s_gargoyleexplode2 = {GARGOYLEEXPLODE2SPR, GARGOYLEEXPLODE2SPR, think, false, false,     0, 0, 0, ProjectileThink, NULL, StonePieceReaction, NULL};
statetype near s_gargoyleexplode3 = {GARGOYLEEXPLODE3SPR, GARGOYLEEXPLODE3SPR, think, false, false,     0, 0, 0, ProjectileThink, NULL, StonePieceReaction, NULL};

statetype near s_gargoylefly1    = {GARGOYLEFLY1SPR, GARGOYLEFLY4SPR, slidethink, false, false,  8, 24, 24, NULL, GargoyleContact, GargoyleReact, &s_gargoylefly2};
statetype near s_gargoylefly2    = {GARGOYLEFLY2SPR, GARGOYLEFLY3SPR, slidethink, false, false,  8, 24, 24, NULL, GargoyleContact, GargoyleReact, &s_gargoylefly3};
statetype near s_gargoylefly3    = {GARGOYLEFLY3SPR, GARGOYLEFLY2SPR, slidethink, false, false,  8, 24, 24, NULL, GargoyleContact, GargoyleReact, &s_gargoylefly4};
statetype near s_gargoylefly4    = {GARGOYLEFLY4SPR, GARGOYLEFLY1SPR, slidethink, false, false,  8, 24, 24, NULL, GargoyleContact, GargoyleReact, &s_gargoylefly1};
statetype near s_gargoylestun  = {GARGOYLESTUNSPR,   GARGOYLESTUNSPR,   stepthink,  false, false, 240,   0, 0, ProjectileThink, GargoyleStunContact, DrawReact, &s_gargoylefly2};

/*
===========================
=
= SpawnGargoyle
=
===========================
*/

void SpawnGargoyle(int x, int y)
{
	GetNewObj(false);
	new->obclass = gargoylefaceobj;
	new->active = yes;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) - 0x71;
	new->xdir = 1;
	new->ydir = 1;
	NewState(new, &s_gargoyleface);
}

/*
===========================
=
= FaceContact
=
===========================
*/

void FaceContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj || hit->obclass == keenobj)
	{
		if (hit->obclass == stunshotobj)
			ExplodeShot(hit);
		if (hit->obclass == keenobj)
			KillKeen();

		ob->obclass = inertobj;
		ob->active = removable;
		ChangeState(ob, &s_gargoylebreakout);

		GetNewObj(true);
		new->obclass = gargoyleobj;
		new->active = yes;
		new->x = ob->x;
		new->y = ob->y - 8*PIXGLOBAL;
		if (ob->x < player->x)
		{
			new->xdir = 1;
		}
		else
		{
			new->xdir = -1;
		}
		new->ydir = 1;
		NewState(new, &s_gargoylefly1);

		GetNewObj(true);
		new->obclass = inertobj;
		new->active = removable;
		new->x = ob->x;
		new->y = ob->y;
		new->xspeed = -28;
		new->yspeed = -40;
		NewState(new, &s_gargoyleexplode1);

		GetNewObj(true);
		new->obclass = inertobj;
		new->active = removable;
		new->x = ob->x;
		new->y = ob->y;
		new->xspeed = 28;
		new->yspeed = -40;
		NewState(new, &s_gargoyleexplode2);

		GetNewObj(true);
		new->obclass = inertobj;
		new->active = removable;
		new->x = ob->x;
		new->y = ob->y;
		new->xspeed = 0;
		new->yspeed = -56;
		NewState(new, &s_gargoyleexplode3);
	}
}

/*
===========================
=
= GargoyleStunContact
=
===========================
*/

void GargoyleStunContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		ob->xspeed = 0;
		StunObj(ob, hit, &s_gargoylestun);
	}
}

/*
===========================
=
= StonePieceReaction
=
===========================
*/

void StonePieceReaction(objtype *ob)
{
	if (ob->hitnorth)
		ob->xspeed = ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= GargoyleContact
=
===========================
*/

void GargoyleContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		if (godmode == false)
		{
			KillKeen();
		}
		else
		{
			return;
		}
	}
	else if (hit->obclass == stunshotobj)
	{
		ob->xspeed = 0;
		ob->needtoclip = cl_midclip;
		StunObj(ob, hit, &s_gargoylestun);
	}
}

/*
===========================
=
= GargoyleReact
=
===========================
*/

void GargoyleReact(objtype *ob)
{
	if (ob->hitnorth)
	{
		ob->ydir = -1;
		ob->yspeed = -ob->yspeed;
//		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	else if (ob->hitsouth)
	{
		ob->ydir = 1;
		ob->yspeed = -ob->yspeed;
//		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	if (ob->hitwest)
	{
		ob->xdir = -1;
		ob->xspeed = -ob->xspeed;
//		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	else if (ob->hiteast)
	{
		ob->xdir = 1;
		ob->xspeed = -ob->xspeed;
//		SD_PlaySound(SND_SLICESTARBOUNCE);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
=============================================================================

						   HORSERADISH

=============================================================================
*/

#pragma warn -sus

statetype near s_horseradhole = {HORSERADHOLESPR,HORSERADHOLESPR,step,true,
	true,135, 0,0, NULL, NULL, DrawReact, &s_horseradgrow1};
statetype near s_horseradgrow1 = {HORSERADGROW1SPR,HORSERADGROW1SPR,step,false,
	true,5, 0,0, NULL, NULL, DrawReact, &s_horseradgrow2};
statetype near s_horseradgrow2 = {HORSERADGROW2SPR,HORSERADGROW2SPR,step,true,
	true,10, 0,0, NULL, NULL, DrawReact, &s_horseradgrow3};
statetype near s_horseradgrow3 = {HORSERADGROW3SPR,HORSERADGROW3SPR,step,false,
	true,20, 0,0, NULL, NULL, DrawReact, &s_horseradgrow4};
statetype near s_horseradgrow4 = {HORSERADGROW2SPR,HORSERADGROW2SPR,step,true,
	true,10, 0,0, NULL, NULL, DrawReact, &s_horseradgrow5};
statetype near s_horseradgrow5 = {HORSERADGROW1SPR,HORSERADGROW1SPR,step,false,
	true,5, 0,0, NULL, NULL, DrawReact, &s_horseradhole};

#pragma warn +sus

/*
====================
=
= SpawnHorseradish
=
====================
*/

void SpawnHorseradish (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = horseobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	NewState (new,&s_horseradhole);
}



/*
=============================================================================

						 RHUBARB

=============================================================================
*/

#pragma warn -sus

statetype near s_rhubarbwalk1	= {RHUBARB4SPR,RHUBARB1SPR,step,true,
	true,15, 88,0, NULL, NULL, WalkReact, &s_rhubarbwalk2};
statetype near s_rhubarbwalk2	= {RHUBARB5SPR,RHUBARB2SPR,step,false,
	true,10, 88,0, NULL, NULL, WalkReact, &s_rhubarbwalk3};
statetype near s_rhubarbwalk3	 = {RHUBARB6SPR,RHUBARB3SPR,step,true,
	true,5, 88,0, NULL, NULL, WalkReact, &s_rhubarbwalk4};
statetype near s_rhubarbwalk4	= {RHUBARB5SPR,RHUBARB2SPR,step,true,
	true,10, 88,0, NULL, NULL, WalkReact, &s_rhubarbwalk5};
statetype near s_rhubarbwalk5	= {RHUBARB4SPR,RHUBARB1SPR,step,false,
	true,15, 88,0, NULL, NULL, WalkReact, &s_rhubarbwalk1};

#pragma warn +sus

/*
====================
=
= SpawnRhubarb
=
====================
*/

void SpawnRhubarb (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = rhubobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_rhubarbwalk1);
}



/*
=============================================================================

						 PARSNIP

=============================================================================
*/

#pragma warn -sus

statetype near s_parsnipstand	= {PARSNIPJUMP2SPR,PARSNIPJUMP1SPR,step,true,
	true,15, 120,0, NULL, NULL, ParsnipReact, &s_parsnipwalk1};
statetype near s_parsnipwalk1	= {PARSNIP3SPR,PARSNIP1SPR,step,false,
	true,15, 120,0, NULL, NULL, WalkReact, &s_parsnipwalk2};
statetype near s_parsnipwalk2	 = {PARSNIP4SPR,PARSNIP2SPR,step,true,
	true,15, 120,0, NULL, NULL, WalkReact, &s_parsnipstand};

#pragma warn +sus

/*
====================
=
= SpawnParsnip
=
====================
*/

void SpawnParsnip (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = parsobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_parsnipstand);
}

/*
====================
=
= ParsnipReact
=
====================
*/

void ParsnipReact (objtype *ob)
{
	if (!ob->hitnorth)
	{
		ob->x-=ob->xmove;
		ob->y-=ob->ymove;
	}

	PLACESPRITE;
}


/*
=================================================================

 				BEET

=================================================================
*/

#pragma warn -sus

statetype near s_beet     = {BEETSPR, BEETSPR, stepthink, false, false, 6, 0, 1, NULL, ShotContact, DrawReact, &s_beet};
statetype near s_beetdead = {BEETSTUNSPR,BEETSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus


/*
==================
=
= SpawnBeet
=
==================
*/

void SpawnBeet (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = beetobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_beet);
}


/*
=============================================================================

						   TURNIP

=============================================================================
*/

#pragma warn -sus

statetype near s_turnip	= {TURNIP1SPR,TURNIP1SPR,step,true,
	true,50, 128,0, NULL, NULL, WalkReact, &s_turnip2};
statetype near s_turnip2	= {TURNIP2SPR,TURNIP2SPR,step,false,
	true,60, 128,0, NULL, NULL, WalkReact, &s_turnip};

#pragma warn +sus

/*
====================
=
= SpawnTurnip
=
====================
*/

void SpawnTurnip (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = turnipobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-18*PIXGLOBAL;
	new->xdir = 1;
	NewState (new,&s_turnip);
	new->temp4 = 4;				// hit it 4 times, so it becomes harmless
}



/*
=============================================================================

						BROCCOLASH

=============================================================================
*/

#pragma warn -sus

statetype near s_broccowalk1	 = {BROCCOLASHRUNL1SPR,BROCCOLASHRUNR1SPR,step,false,
	true,7, 128,0, BroccoThink, ShotContact, WalkReact, &s_broccowalk2};
statetype near s_broccowalk2	 = {BROCCOLASHRUNL2SPR,BROCCOLASHRUNR2SPR,step,false,
	true,7, 128,0, BroccoThink, ShotContact, WalkReact, &s_broccowalk3};
statetype near s_broccowalk3	 = {BROCCOLASHRUNL3SPR,BROCCOLASHRUNR3SPR,step,false,
	true,7, 128,0, BroccoThink, ShotContact, WalkReact, &s_broccowalk4};
statetype near s_broccowalk4	 = {BROCCOLASHRUNL4SPR,BROCCOLASHRUNR4SPR,step,false,
	true,7, 128,0, BroccoThink, ShotContact, WalkReact, &s_broccowalk1};

statetype near s_broccosmash1 = {BROCCOLASHSMASHL1SPR,BROCCOLASHSMASHR1SPR,step,true,
	false,3, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash2};
statetype near s_broccosmash2 = {BROCCOLASHSMASHL2SPR,BROCCOLASHSMASHR2SPR,step,true,
	false,3, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash3};
statetype near s_broccosmash3 = {BROCCOLASHSMASHL3SPR,BROCCOLASHSMASHR3SPR,step,true,
	false,3, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash4};
statetype near s_broccosmash4 = {BROCCOLASHSMASHL4SPR,BROCCOLASHSMASHR4SPR,step,false,
	false,7, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash5};
statetype near s_broccosmash5 = {BROCCOLASHSMASHL3SPR,BROCCOLASHSMASHR3SPR,step,true,
	false,6, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash6};
statetype near s_broccosmash6 = {BROCCOLASHSMASHL2SPR,BROCCOLASHSMASHR2SPR,step,true,
	false,6, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash7};
statetype near s_broccosmash7 = {BROCCOLASHSMASHL1SPR,BROCCOLASHSMASHR1SPR,step,true,
	false,6, 0,0, NULL, ShotContact, DrawReact, &s_broccosmash8};
statetype near s_broccosmash8 = {BROCCOLASHRUNL1SPR,BROCCOLASHRUNR1SPR,step,true,
	false,6, 0,0, BroccoGetUp, ShotContact, DrawReact, &s_broccosmash9};
statetype near s_broccosmash9 = {BROCCOLASHRUNL1SPR,BROCCOLASHRUNR1SPR,step,true,
	false,6, 128,0, NULL, ShotContact, WalkReact, &s_broccowalk1};

statetype near s_broccodead = {BROCCOLASHSTUNSPR,BROCCOLASHSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnBrocco
=
====================
*/

void SpawnBrocco (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = broccoobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	NewState (new,&s_broccowalk1);
}

/*
====================
=
= BroccoGetUp
=
====================
*/

void BroccoGetUp (objtype *ob)
{
	ob->needtoclip = true;
}


/*
====================
=
= BroccoThink
=
====================
*/

void BroccoThink (objtype *ob)
{
	int delta;

	if (ob->top > player->bottom || ob->bottom < player->top)
		return;

	delta = player->x - ob->x;

	if ( ob->xdir == -1 )
	{
		if (delta < -3*TILEGLOBAL)
			return;
		if (delta > TILEGLOBAL/2)
		{
			ob->xdir = 1;
			return;
		}
		ob->state = &s_broccosmash1;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
	else
	{
		delta = player->left - ob->right;
		if (delta > 3*TILEGLOBAL)
			return;
		if (delta < -TILEGLOBAL/2)
		{
			ob->xdir = -1;
			return;
		}
		ob->state = &s_broccosmash1;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
}


/*
=============================================================================

						 TOMATOOTH / BLUE TOMATOOTH

ob->temp1 = jumptime

=============================================================================
*/

#define	SPDTOMATBOUNCE	30
#define TICTOMATJUMP	10
#define SPDTOMAT		16

#define	SPDBTOMATBOUNCE	15
#define TICBTOMATJUMP	5
#define SPDBTOMAT		8

#pragma warn -sus

statetype near s_tomatbounce	 = {TOMATOOTHL1SPR,TOMATOOTHR1SPR,stepthink,false,
	false,20, 0,0, TomatBounceThink, ShotContact, TomatReact, &s_tomatbounce2};
statetype near s_tomatbounce2 = {TOMATOOTHL2SPR,TOMATOOTHR2SPR,stepthink,false,
	false,20, 0,0, TomatBounceThink, ShotContact, TomatReact, &s_tomatbounce};

statetype near s_btomatbounce	 = {BTOMATOOTHL1SPR,BTOMATOOTHR1SPR,stepthink,false,
	false,20, 0,0, TomatBounceThink, ShotContact, TomatReact, &s_btomatbounce2};
statetype near s_btomatbounce2 = {BTOMATOOTHL2SPR,BTOMATOOTHR2SPR,stepthink,false,
	false,20, 0,0, TomatBounceThink, ShotContact, TomatReact, &s_btomatbounce};

statetype near s_tomatdead = {TOMATOOTHSTUNSPR,TOMATOOTHSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};
statetype near s_btomatdead = {BTOMATOOTHSTUN,BTOMATOOTHSTUN,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnTomat
=
====================
*/

void SpawnTomat (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = tomatobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-1*BLOCKSIZE;
	new->xdir = 1;
	if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Hard)
		NewState (new,&s_btomatbounce);
	else
		NewState (new,&s_tomatbounce);
}

/*
====================
=
= TomatBounceThink
=
====================
*/

void TomatBounceThink (objtype *ob)
{
	if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Hard)
		AccelerateX (ob,ob->x > player->x ? -1 : 1,SPDBTOMAT);
	else
		AccelerateX (ob,ob->x > player->x ? -1 : 1,SPDTOMAT);
	if (ob->xspeed > 0)
		ob->xdir = 1;
	else
		ob->xdir = -1;

	if (ob->temp1)
	{
		if (ob->temp1<tics)
		{
			ob->ymove = ob->yspeed*ob->temp1;
			ob->temp1 = 0;
		}
		else
		{
			ob->ymove = ob->yspeed*tics;
			ob->temp1-=tics;
		}
	}
	else
		DoGravity(ob);

}


/*
====================
=
= TomatReactThink
=
====================
*/

void TomatReact (objtype *ob)
{
	if (ob->hiteast || ob->hitwest)
	{
		ob->xdir = -ob->xdir;
		ob->xspeed = -ob->xspeed;
	}

	if (ob->hitsouth)
	{
		if (ob->tileright >= originxtile
		&& ob->tileleft <= originxtilemax
		&& ob->tiletop >= originytile
		&& ob->tilebottom <= originytilemax)
			SD_PlaySound (BOUNCESND);
		ob->yspeed = -ob->yspeed;
	}

	if (ob->hitnorth)
	{
		if (ob->tileright >= originxtile
		&& ob->tileleft <= originxtilemax
		&& ob->tiletop >= originytile
		&& ob->tilebottom <= originytilemax)
			SD_PlaySound (BOUNCESND);
		if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Hard)
			ob->yspeed = -SPDBTOMATBOUNCE-(US_RndT()>>4);
		else
			ob->yspeed = -SPDTOMATBOUNCE-(US_RndT()>>4);
		if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Hard)
			ob->temp1 = TICBTOMATJUMP;
		else
			ob->temp1 = TICTOMATJUMP;
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

#pragma warn -sus

statetype near s_carrotwalk1	 = {CARROTRUNL1SPR,CARROTRUNR1SPR,step,false,
	true,5, 128,0, NULL, ShotContact, CarrotReact, &s_carrotwalk2};
statetype near s_carrotwalk2	 = {CARROTRUNL2SPR,CARROTRUNR2SPR,step,false,
	true,5, 128,0, NULL, ShotContact, CarrotReact, &s_carrotwalk3};
statetype near s_carrotwalk3	 = {CARROTRUNL3SPR,CARROTRUNR3SPR,step,false,
	true,5, 128,0, NULL, ShotContact, CarrotReact, &s_carrotwalk4};
statetype near s_carrotwalk4	 = {CARROTRUNL4SPR,CARROTRUNR4SPR,step,false,
	true,5, 128,0, NULL, ShotContact, CarrotReact, &s_carrotwalk1};

statetype near s_carrotleap	 = {CARROTLEAPL1SPR,CARROTLEAPR1SPR,think,false,
	false,0, 0,0, ProjectileThink, ShotContact, CarrotAirReact, NULL};

statetype near s_carrotdead = {CARROTSTUNSPR,CARROTSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnCarrot
=
====================
*/

void SpawnCarrot (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = carrotobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_carrotwalk1);
	new->hitnorth = 1;
}


/*
====================
=
= CarrotReact
=
====================
*/

void CarrotReact (objtype *ob)
{
	unsigned x, width, bot;
	uint16_t *map;

	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->xdir = -1;
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->xdir = 1;
	}
	else if (!ob->hitnorth)
	{
		ob->x -= ob->xmove;
		ob->y -= ob->ymove;

		ob->yspeed = -SPDCARROTLEAPY;
		ob->xspeed = SPDCARROTLEAPX*ob->xdir;
		ChangeState (ob,&s_carrotleap);
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

void CarrotAirReact (objtype *ob)
{
	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
		ChangeState (ob,&s_carrotwalk1);

	PLACESPRITE;
}


/*
=============================================================================

						   ASPARAGUSTO

=============================================================================
*/

#pragma warn -sus

statetype near s_asparwalk1	 = {ASPARAGUSRUNL1SPR,ASPARAGUSRUNR1SPR,step,true,
	true,3, 100,0, NULL, ShotContact, WalkReact, &s_asparwalk2};
statetype near s_asparwalk2	 = {ASPARAGUSRUNL2SPR,ASPARAGUSRUNR2SPR,step,false,
	true,3, 100,0, NULL, ShotContact, WalkReact, &s_asparwalk3};
statetype near s_asparwalk3	 = {ASPARAGUSRUNL3SPR,ASPARAGUSRUNR3SPR,step,true,
	true,3, 100,0, NULL, ShotContact, WalkReact, &s_asparwalk4};
statetype near s_asparwalk4	 = {ASPARAGUSRUNL4SPR,ASPARAGUSRUNR4SPR,step,false,
	true,3, 100,0, NULL, ShotContact, WalkReact, &s_asparwalk1};

statetype near s_aspardead = {ASPARAGUSSTUNSPR,ASPARAGUSSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnAspar
=
====================
*/

void SpawnAspar (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = asparobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;
	new->xdir = 1;
	NewState (new,&s_asparwalk1);
}


/*
=============================================================================

						   SOUR GRAPE

=============================================================================
*/

#pragma warn -sus

statetype near s_grapewait	= {GRAPEONVINESPR,GRAPEONVINESPR,think,false,
	false,0, 0,0, GrapeThink, NULL, DrawReact, NULL};

statetype near s_grapefall	= {GRAPEFALLINGSPR,GRAPEFALLINGSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, GrapeFallReact, NULL};

statetype near s_grapesit	= {GRAPEONVINESPR,GRAPEONVINESPR,step,false,
	false,30, 0,0, NULL, NULL, DrawReact, &s_graperise};
statetype near s_graperise	= {GRAPEONVINESPR,GRAPEONVINESPR,slide,false,
	false,0, 0,-16, NULL, NULL, GrapeRiseReact, NULL};

statetype near s_grapewait2	= {GRAPEONVINE2SPR,GRAPEONVINE2SPR,think,false,
	false,0, 0,0, GrapeThink, NULL, DrawReact, NULL};

statetype near s_grapefall2	= {GRAPEFALLING2SPR,GRAPEFALLING2SPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, GrapeFallReact, NULL};

statetype near s_grapesit2	= {GRAPEONVINE2SPR,GRAPEONVINE2SPR,step,false,
	false,30, 0,0, NULL, NULL, DrawReact, &s_graperise2};
statetype near s_graperise2	= {GRAPEONVINE2SPR,GRAPEONVINE2SPR,slide,false,
	false,0, 0,-16, NULL, NULL, GrapeRiseReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnGrape
=
====================
*/

void SpawnGrape (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = grapeobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;
	new->xdir = 1;
	new->ydir = 1;
	if (gamestate.difficulty == gd_Easy)
		NewState (new,&s_grapewait2);
	else
		NewState (new,&s_grapewait);
}

/*
====================
=
= GrapeThink
=
====================
*/

void GrapeThink (objtype *ob)
{
	unsigned y,starty,endy;
	uint16_t *map;

	if (player->left > ob->right
		|| player->right < ob->left
		|| player->y < ob->y )
		return;

//
// see if there are any walls between grape and player
//
	starty = ob->tilebottom;
	endy = player->tiletop;

	map = mapsegs[1] + mapbwidthtable[starty]/2 + ob->tilemidx;
	for (y = starty ; y<endy ; y++,map+=mapwidth)
		if (tinf[NORTHWALL+*map])
			return;
	if (gamestate.difficulty == gd_Easy)
		ob->state = &s_grapefall2;
	else
		ob->state = &s_grapefall;
	SD_PlaySound (GRAPESCREAMSND);

}

/*
====================
=
= GrapeRiseReact
=
====================
*/

void GrapeRiseReact (objtype *ob)
{
	if (ob->hitsouth)
		if (gamestate.difficulty == gd_Easy)
			ChangeState(ob,&s_grapewait2);
		else
			ChangeState(ob,&s_grapewait);
	PLACESPRITE;
}


/*
====================
=
= GrapeFallReact
=
====================
*/

void GrapeFallReact (objtype *ob)
{
	if (ob->hitnorth)
	{
		SD_PlaySound (BOUNCESND);
		ob->yspeed = -(ob->yspeed<<1)/3;
		if (ob->yspeed > -32)
			if (gamestate.difficulty == gd_Easy)
				ChangeState(ob,&s_grapesit2);
			else
				ChangeState(ob,&s_grapesit);
	}
	PLACESPRITE;
}


/*
=============================================================================

						   CAULIFLOWER

=============================================================================
*/

#pragma warn -sus

statetype near s_caulistop	= {CAULISTOPSPR,CAULISTOPSPR,step,true,
	false,200, 0,0, NULL, NULL, DrawReact, &s_cauli2};

statetype near s_cauli2	= {CAULI3SPR,CAULI1SPR,step,true,
	true,30, 128,0, NULL, NULL, WalkReact, &s_cauli1};
statetype near s_cauli1	= {CAULI4SPR,CAULI2SPR,step,true,
	true,40, 128,0, NULL, NULL, WalkReact, &s_cauli2};

#pragma warn +sus

/*
====================
=
= SpawnCauliflower
=
====================
*/

void SpawnCauliflower (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = cauliobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-26*PIXGLOBAL;
	new->xdir = 1;
	NewState (new,&s_caulistop);
}


/*
=============================================================================

						  MOSQUITO

=============================================================================
*/

statetype near s_mosquitofly1 = {MOSQUITOFLYL1SPR, MOSQUITOFLYR1SPR, stepthink, true, false, 5, 0, 0, MosquitoFlyThink, MosquitoFlyContact, MosquitoReact, &s_mosquitofly2};
statetype near s_mosquitofly2 = {MOSQUITOFLYL2SPR, MOSQUITOFLYR2SPR, stepthink, true, false, 5, 0, 0, MosquitoFlyThink, MosquitoFlyContact, MosquitoReact, &s_mosquitofly1};
statetype near s_mosquitoshot1 = {MOSQUITOSHOT1SPR, MOSQUITOSHOT1SPR, stepthink, false, false, 20, 0, 0, NULL, NULL, DrawReact, &s_mosquitoshot2};
statetype near s_mosquitoshot2 = {MOSQUITOSHOT2SPR, MOSQUITOSHOT2SPR, stepthink, false, false, 20, 0, 0, NULL, NULL, DrawReact, &s_mosquitoshot3};
statetype near s_mosquitoshot3 = {MOSQUITOSHOT3SPR, MOSQUITOSHOT3SPR, stepthink, false, false, 20, 0, 0, NULL, NULL, DrawReact, NULL};

/*
===========================
=
= SpawnMosquito
=
===========================
*/

void SpawnMosquito(int x, int y)
{
	GetNewObj(false);
	new->obclass = mosquitoobj;
	new->active = yes;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y);
	NewState(new, &s_mosquitofly1);
}

/*
===========================
=
= MosquitoFlyThink
=
===========================
*/

void MosquitoFlyThink(objtype *ob)
{
	int delta;

	if (ob->top > player->bottom || ob->bottom < player->top)
		return;

	delta = player->x - ob->x;

	if ( ob->xdir == -1 )
	{
		if (delta < -3*TILEGLOBAL)
			return;
		if (delta > TILEGLOBAL/2)
		{
			ob->xdir = 1;
			return;
		}
		ob->state = &s_mosquitofly1;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
	else
	{
		delta = player->left - ob->right;
		if (delta > 3*TILEGLOBAL)
			return;
		if (delta < -TILEGLOBAL/2)
		{
			ob->xdir = -1;
			return;
		}
		ob->state = &s_mosquitofly2;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
}

/*
===========================
=
= MosquitoFlyContact
=
===========================
*/

void MosquitoFlyContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		if (hit->xdir == 1)
		{
			ob->xspeed = 20;
		}
		else if (hit->xdir == -1)
		{
			ob->xspeed = -20;
		}
		else if (hit->ydir == 1)
		{
			ob->yspeed = 20;
		}
		else if (hit->ydir == -1)
		{
			ob->yspeed = -20;
		}
		ob->needtoclip = true;
		StunObj(ob, hit, &s_mosquitoshot1);
	}
}

/*
===========================
=
= MosquitoReact
=
===========================
*/

void MosquitoReact(objtype *ob)
{
	if (ob->hitsouth)
	{
		ob->yspeed = 8;
		ob->ydir = 1;
	}
	if (ob->hitwest)
	{
		ob->xspeed = 0;
		ob->xdir = -1;
	}
	if (ob->hiteast)
	{
		ob->xspeed = 0;
		ob->xdir = 1;
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
=============================================================================

			BABERRY (BABY BLUEBERRY)

=============================================================================
*/

statetype near s_baberrycry1 = {BABERRYCRY1SPR, BABERRYCRY1SPR, stepthink, true, false, 20, 0, 0, NULL, BaberryContact, BaberryReact, &s_baberrycry2};
statetype near s_baberrycry2 = {BABERRYCRY2SPR, BABERRYCRY2SPR, stepthink, true, false, 50, 0, 0, NULL, BaberryContact, BaberryReact, &s_baberrycry1};
statetype near s_baberrysit = {BABERRYCRY3SPR, BABERRYCRY3SPR, stepthink, false, false, 0, 0, 0, NULL, ShotContact, DrawReact, &s_baberrycry1};

/*
===========================
=
= SpawnBaberry
=
===========================
*/

void SpawnBaberry (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = baberryobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-0*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_baberrysit);
}

/*
===========================
=
= BaberryContact
=
===========================
*/

void BaberryContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == keenobj)
	{
		if (godmode == false)
		{
			hit->obclass = nothing;
			KillKeen();
		}
		else
		{
			return;
		}
	}
	else if (hit->obclass == stunshotobj)
	{
		ob->xspeed = 0;
		ob->needtoclip = cl_midclip;
		StunObj(ob, hit, &s_baberrycry1);
	}
}

/*
===========================
=
= BaberryReact
=
===========================
*/

void BaberryReact(objtype *ob)
{
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
=============================================================================

						   KUMQUAT

=============================================================================
*/

#pragma warn -sus

statetype near s_kumquat	= {KUMQUATLSPR,KUMQUATRSPR,stepthink,true,
	false,100, 0,0, KumquatThink, ShotContact, DrawReact, &s_kumquat2};
statetype near s_kumquat2	= {KUMQUATSITSPR,KUMQUATSITSPR,stepthink,true,
	false,200, 0,0, KumquatThink, ShotContact, DrawReact, &s_kumquat};

statetype near s_kumquatsit	= {KUMQUATSITSPR,KUMQUATSITSPR,stepthink,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnKumquat
=
====================
*/

void SpawnKumquat (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = kumquatobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-0*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_kumquat);
}

/*
====================
=
= KumquatThink
=
====================
*/

void KumquatThink (objtype *ob)
{
	ob->needtoreact = true;
	if (player->right < ob->left)
		ob->xdir = -1;
	if (player->left > ob->right)
		ob->xdir = 1;
}


/*
=============================================================================

						   STEEL CART

=============================================================================
*/

#pragma warn -sus

statetype near s_scart1	= {STEELCART1SPR,STEELCART1SPR,slide,true,
	false,40, 24,0, NULL, NULL, CartReact, &s_scart2};
statetype near s_scart2	= {STEELCART2SPR,STEELCART2SPR,slide,true,
	false,40, 24,0, NULL, NULL, CartReact, &s_scart1};

#pragma warn +sus

/*
====================
=
= SpawnSteelCart
=
====================
*/

void SpawnSteelCart (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = steelcartobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-32*PIXGLOBAL;
	new->xdir = 1;
	new->ydir = 1;
	new->active = allways;
	NewState (new,&s_scart1);
}


/*
=============================================================================

				COLONEL CORN

=============================================================================
*/

#pragma warn -sus

statetype near s_cornwalk1	= {CORNWALK1SPR,CORNWALK1SPR,step,false,
	true,10, 128,0, CornThink, CornContact, WalkReact, &s_cornwalk2};
statetype near s_cornwalk2	= {CORNWALK2SPR,CORNWALK2SPR,step,false,
	true,10, 128,0, CornThink, CornContact, WalkReact, &s_cornwalk3};
statetype near s_cornwalk3	= {CORNWALK3SPR,CORNWALK3SPR,step,false,
	true,10, 128,0, CornThink, CornContact, WalkReact, &s_cornwalk4};
statetype near s_cornwalk4	= {CORNWALK4SPR,CORNWALK4SPR,step,false,
	true,10, 128,0, CornThink, CornContact, WalkReact, &s_cornwalk1};

statetype near s_cornattack1	= {CORNATTACK1SPR,CORNATTACK1SPR,step,false,
	false,10, 0,0, NULL, CornContact, DrawReact, &s_cornattack2};
statetype near s_cornattack2	= {CORNATTACK2SPR,CORNATTACK2SPR,step,false,
	false,1, 0,0, NULL, CornContact, DrawReact, &s_cornattack3};
statetype near s_cornattack3	= {CORNATTACK2SPR,CORNATTACK2SPR,step,false,
	false,10, -128,0, NULL, CornContact, DrawReact, &s_cornwalk1};

statetype near s_corndead = {CORNSTUNNEDSPR,CORNSTUNNEDSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus


/*
====================
=
= SpawnCorn
=
====================
*/

void SpawnCorn (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = cornobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_cornwalk1);
	new->temp4 = 2;	// health
}


/*
====================
=
= CornThink
=
====================
*/

void CornThink (objtype *ob)
{
	int delta;

	delta = player->x - ob->x;

	if (delta < -8*TILEGLOBAL)
	{
	// walk closer
		ob->xdir = 1;
	}
	if (delta < -4*TILEGLOBAL)
	{
	// throw
		ob->xdir = 1;
		ob->state = &s_cornattack1;
	}
	else if (delta < 0)
	{
	// run away
		ob->xdir = -1;
		ob->state = &s_cornattack1;
		ob->nothink = 8;
	}
	else if (delta < 4*TILEGLOBAL)
	{
	// run away
		ob->xdir = 1;
		ob->state = &s_cornattack1;
		ob->nothink = 8;
	}
	else if (delta < 8*TILEGLOBAL)
	{
	// throw and walk closer
		ob->xdir = -1;
		ob->state = &s_cornattack1;
	}
	else
	{
	// walk closer
		ob->xdir = -1;
	}
}

/*
===========================
=
= CornContact
=
===========================
*/

void CornContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		if (--ob->temp4 == 0)	// handle health
		{
			StunObj(ob, hit, &s_corndead);
		}
		else
		{
			ob->needtoreact = true;
			ExplodeShot(hit);
		}
	}
}