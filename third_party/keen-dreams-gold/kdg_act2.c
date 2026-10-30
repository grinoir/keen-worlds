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
#define CANTALOUPEPLACESPRITE RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum, \
	spritedraw,0);

void	ProjectileReact (objtype *ob);


/*
=============================================================================

				TATER TROOPER / TATER TROOPERINA
						  
temp1 = chasing player
temp2 = nothink time

=============================================================================
*/

#pragma warn -sus

statetype near s_taterwalk1	= {TATERTROOPWALKL1SPR,TATERTROOPWALKR1SPR,step,false,
	true,10, 128,0, TaterThink, ShotContact, TaterWalkReact, &s_taterwalk2};
statetype near s_taterwalk2	= {TATERTROOPWALKL2SPR,TATERTROOPWALKR2SPR,step,false,
	true,10, 128,0, TaterThink, ShotContact, TaterWalkReact, &s_taterwalk3};
statetype near s_taterwalk3	= {TATERTROOPWALKL3SPR,TATERTROOPWALKR3SPR,step,false,
	true,10, 128,0, TaterThink, ShotContact, TaterWalkReact, &s_taterwalk4};
statetype near s_taterwalk4	= {TATERTROOPWALKL4SPR,TATERTROOPWALKR4SPR,step,false,
	true,10, 128,0, TaterThink, ShotContact, TaterWalkReact, &s_taterwalk1};

//statetype s_taterwalk5	= {TATERTROOPERINAWALKL1SPR,TATERTROOPERINAWALKR1SPR,step,false,
//	true,10, 128,0, TaterThink, NULL, TaterWalkReact, &s_taterwalk6};
//statetype s_taterwalk6	= {TATERTROOPERINAWALKL2SPR,TATERTROOPERINAWALKR2SPR,step,false,
//	true,10, 128,0, TaterThink, NULL, TaterWalkReact, &s_taterwalk7};
//statetype s_taterwalk7	= {TATERTROOPERINAWALKL3SPR,TATERTROOPERINAWALKR3SPR,step,false,
//	true,10, 128,0, TaterThink, NULL, TaterWalkReact, &s_taterwalk8};
//statetype s_taterwalk8	= {TATERTROOPERINAWALKL4SPR,TATERTROOPERINAWALKR4SPR,step,false,
//	true,10, 128,0, TaterThink, NULL, TaterWalkReact, &s_taterwalk5};

statetype near s_taterattack1= {TATERTROOPLUNGEL1SPR,TATERTROOPLUNGER1SPR,step,false,
	false,12, 0,0, NULL, ShotContact, BackupReact, &s_taterattack2};
statetype near s_taterattack2= {TATERTROOPLUNGEL2SPR,TATERTROOPLUNGER2SPR,step,false,
	false,20, 0,0, NULL, ShotContact, DrawTater, &s_taterattack3};
statetype near s_taterattack3= {TATERTROOPLUNGEL1SPR,TATERTROOPLUNGER1SPR,step,false,
	false,8, 0,0, NULL, ShotContact, DrawTater, &s_taterwalk1};

statetype near s_taterscream1= {TATERTROOPERINASCREAMLSPR,TATERTROOPERINASCREAMRSPR,step,false,
	false,12, 0,0, NULL, ShotContact, BackupReact, &s_taterscream2};
statetype near s_taterscream2= {TATERTROOPERINASCREAMLSPR,TATERTROOPERINASCREAMRSPR,step,false,
	false,20, 0,0, NULL, ShotContact, DrawTater, &s_taterscream3};
statetype near s_taterscream3= {TATERTROOPERINASCREAMLSPR,TATERTROOPERINASCREAMRSPR,step,false,
	false,8, 0,0, NULL, ShotContact, DrawTater, &s_taterwalk1};

statetype near s_taterdead = {TATERTROOPSTUNSPR,TATERTROOPSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};
statetype near s_taterfemaledead = {TATERTROOPERINASTUNSPR,TATERTROOPERINASTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

//statetype s_taterattack4= {TATERTROOPERINALUNGEL2SPR,TATERTROOPERINALUNGER2SPR,step,false,
//	false,12, 0,0, NULL, NULL, BackupReact, &s_taterattack5};
//statetype s_taterattack5= {TATERTROOPERINALUNGEL1SPR,TATERTROOPERINALUNGER1SPR,step,false,
//	false,20, 0,0, NULL, NULL, DrawTater, &s_taterattack6};
//statetype s_taterattack6= {TATERTROOPERINALUNGEL2SPR,TATERTROOPERINALUNGER2SPR,step,false,
//	false,8, 0,0, NULL, NULL, DrawTater, &s_taterwalk5};

#pragma warn +sus

/*
====================
=
= SpawnTater
=
====================
*/

void SpawnTater (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = taterobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE + 15;
	new->xdir = 1;
	new->ydir = 1;
	NewState(new,&s_taterwalk1);
	new->hitnorth = 1;
}


/*
====================
=
= DrawTater
=
====================
*/

void DrawTater(objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape >= TATERTROOPWALKR1SPR && shape <= TATERTROOPLUNGEL2SPR && gamestate.difficulty == gd_Easy)
	{
		shape += TATERTROOPERINAWALKR1SPR - TATERTROOPWALKR1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}


/*
====================
=
= TaterWalkReact
=
====================
*/

void TaterWalkReact(objtype *ob)
{
	WalkReact(ob);
	DrawTater(ob);
}


/*
====================
=
= TaterThink
=
====================
*/

void TaterThink (objtype *ob)
{
	int delta;

	if (ob->top > player->bottom || ob->bottom < player->top)
		return;

	if ( ob->xdir == -1 )
	{
		delta = ob->left - player->right;
		if (delta > TILEGLOBAL)
			return;
		if (delta < -8*PIXGLOBAL)
		{
			ob->xdir = 1;
			return;
		}
		if (gamestate.difficulty == gd_Easy)
		{
			if (player->state == &s_keenwalk1 || player->state == &s_keenwalk2 || player->state == &s_keenwalk3 || player->state == &s_keenwalk4)
			{
				SD_PlaySound (TATERSWINGSND);
				ob->state = &s_taterattack1;
				return;
			}
			else
			{
				SD_PlaySound (KEENSCREAMSND);
				ob->state = &s_taterscream1;
				return;		
			}
		}
		SD_PlaySound (TATERSWINGSND);
		ob->state = &s_taterattack1;
		return;
	}
	else
	{
		delta = player->left - ob->right;
		if (delta > TILEGLOBAL)
			return;
		if (delta < -8*PIXGLOBAL)
		{
			ob->xdir = -1;
			return;
		}
		if (gamestate.difficulty == gd_Easy)
		{
			if (player->state == &s_keenwalk1 || player->state == &s_keenwalk2 || player->state == &s_keenwalk3 || player->state == &s_keenwalk4)
			{
				SD_PlaySound (TATERSWINGSND);
				ob->state = &s_taterattack1;
				return;
			}
			else
			{
				SD_PlaySound (KEENSCREAMSND);
				ob->state = &s_taterscream1;
				return;		
			}
		}
		SD_PlaySound (TATERSWINGSND);
		ob->state = &s_taterattack1;
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

void BackupReact (objtype *ob)
{
	if (!ob->hitnorth)
	{
		ob->x-=ob->xmove;
		ob->y-=ob->ymove;
	}

//	PLACESPRITE;
	DrawTater(ob);
}


/*
=============================================================================

						   CANTELOUPE CART

=============================================================================
*/

#pragma warn -sus

statetype near s_cartroll1	= {CANTCARTL1SPR,CANTCARTL1SPR,slide,true,
	false,5, 32,0, NULL, NULL, CartReact, &s_cartroll2};
statetype near s_cartroll2	= {CANTCARTL2SPR,CANTCARTL2SPR,slide,true,
	false,5, 32,0, NULL, NULL, CartReact, &s_cartroll1};

#pragma warn +sus

/*
====================
=
= SpawnCart
=
====================
*/

void SpawnCart (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = cartobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-3*PIXGLOBAL;
	new->xdir = 1;
	new->ydir = 1;
	new->active = allways;
	NewState (new,&s_cartroll1);
}



/*
====================
=
= CartReact
=
====================
*/

void CartReact (objtype *ob)
{
	uint16_t  *map;

	if (ob->xdir == 1 && ob->hitwest)
	{
		ob->xdir = -1;
	}
	else if (ob->xdir == -1 && ob->hiteast)
	{
		ob->xdir = 1;
	}

	map = mapsegs[1] + mapbwidthtable[ob->tilebottom+1]/2;
	if (ob->xdir == 1)
		map += ob->tileright;
	else
		map += ob->tileleft;

	if ( !tinf[NORTHWALL + *map] )
		ob->xdir = -ob->xdir;

	PLACESPRITE;
}

/*
=============================================================================

						   MELON CART

=============================================================================
*/

#pragma warn -sus

statetype near s_mcartroll1	= {MELONCARTL1SPR,MELONCARTL1SPR,slide,true,
	false,5, 16,0, NULL, NULL, CartReact, &s_mcartroll2};
statetype near s_mcartroll2	= {MELONCARTL2SPR,MELONCARTL2SPR,slide,true,
	false,5, 16,0, NULL, NULL, CartReact, &s_mcartroll1};

#pragma warn +sus

/*
====================
=
= SpawnMCart
=
====================
*/

void SpawnMCart (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = mcartobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-3*PIXGLOBAL;
	new->xdir = 1;
	new->ydir = 1;
	new->active = allways;
	NewState (new,&s_mcartroll1);
}


/*
=============================================================================

							FRENCHY

=============================================================================
*/

#define FRYXSPEED	40
#define FRYYSPEED	-20

#pragma warn -sus

statetype near s_frenchywalk1	= {FRENCHYRUNL1SPR,FRENCHYRUNR1SPR,step,false,
	true,10, 128,0, FrenchyThink, ShotContact, WalkReact, &s_frenchywalk2};
statetype near s_frenchywalk2	= {FRENCHYRUNL2SPR,FRENCHYRUNR2SPR,step,false,
	true,10, 128,0, FrenchyThink, ShotContact, WalkReact, &s_frenchywalk3};
statetype near s_frenchywalk3	= {FRENCHYRUNL3SPR,FRENCHYRUNR3SPR,step,false,
	true,10, 128,0, FrenchyThink, ShotContact, WalkReact, &s_frenchywalk4};
statetype near s_frenchywalk4	= {FRENCHYRUNL4SPR,FRENCHYRUNR4SPR,step,false,
	true,10, 128,0, FrenchyThink, ShotContact, WalkReact, &s_frenchywalk1};

statetype near s_frenchyrun1	= {FRENCHYRUNL1SPR,FRENCHYRUNR1SPR,step,true,
	true,5, 128,0, FrenchyRunThink, ShotContact, WalkReact, &s_frenchyrun2};
statetype near s_frenchyrun2	= {FRENCHYRUNL2SPR,FRENCHYRUNR2SPR,step,true,
	true,5, 128,0, FrenchyRunThink, ShotContact, WalkReact, &s_frenchyrun3};
statetype near s_frenchyrun3	= {FRENCHYRUNL3SPR,FRENCHYRUNR3SPR,step,true,
	true,5, 128,0, FrenchyRunThink, ShotContact, WalkReact, &s_frenchyrun4};
statetype near s_frenchyrun4	= {FRENCHYRUNL4SPR,FRENCHYRUNR4SPR,step,true,
	true,5, 128,0, FrenchyRunThink, ShotContact, WalkReact, &s_frenchyrun1};

statetype near s_frenchythrow1	= {FRENCHYTHROWL1SPR,FRENCHYTHROWR1SPR,step,false,
	false,10, 0,0, NULL, ShotContact, DrawReact, &s_frenchythrow2};
statetype near s_frenchythrow2	= {FRENCHYTHROWL2SPR,FRENCHYTHROWR2SPR,step,false,
	false,1, 0,0, FrenchyThrow, ShotContact, DrawReact, &s_frenchythrow3};
statetype near s_frenchythrow3	= {FRENCHYTHROWL2SPR,FRENCHYTHROWR2SPR,step,false,
	false,10, -128,0, NULL, ShotContact, DrawReact, &s_frenchywalk1};

statetype near s_fry1		= {FRENCHFRY1SPR,FRENCHFRY1SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact, &s_fry2};
statetype near s_fry2		= {FRENCHFRY2SPR,FRENCHFRY2SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact, &s_fry1};

statetype near s_frenchydead = {FRENCHYSTUNSPR,FRENCHYSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};


#pragma warn +sus


/*
====================
=
= SpawnFrenchy
=
====================
*/

void SpawnFrenchy (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = frenchyobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_frenchywalk1);
}


/*
====================
=
= FrenchyRunThink
=
====================
*/

void FrenchyRunThink (objtype *ob)
{
	ob->state = &s_frenchywalk1;
}


/*
====================
=
= FrenchyThrow
=
====================
*/

void FrenchyThrow (objtype *ob)
{
	GetNewObj (true);
	new->obclass = shotobj;
	if (ob->xdir == 1)
	{
		new->x = ob->x+24*16;
		new->y = ob->y+8*16;
	}
	else
	{
		new->x = ob->x;
		new->y = ob->y+8*16;
	}
	new->xdir = ob->xdir;
	new->ydir = 1;
	new->xspeed = ob->xdir * FRYXSPEED-(US_RndT()>>4);
	new->yspeed = FRYYSPEED;
	new->active = removable;
	NewState (new,&s_fry1);

	ob->nothink = 2;
}


/*
====================
=
= FrenchyThink
=
====================
*/

void FrenchyThink (objtype *ob)
{
	int delta;

	if ( abs(ob->y - player->y) > 3*TILEGLOBAL )
	{
		if (US_RndT()<8)
			ob->xdir = -ob->xdir;		// turn randomly
		return;
	}

	delta = player->x - ob->x;

	if (delta < -8*TILEGLOBAL)
	{
	// walk closer
		ob->xdir = -1;
	}
	if (delta < -4*TILEGLOBAL)
	{
	// throw
		ob->xdir = -1;
		ob->state = &s_frenchythrow1;
	}
	else if (delta < 0)
	{
	// run away
		ob->xdir = 1;
		ob->state = &s_frenchyrun1;
		ob->nothink = 8;
	}
	else if (delta < 4*TILEGLOBAL)
	{
	// run away
		ob->xdir = -1;
		ob->state = &s_frenchyrun1;
		ob->nothink = 8;
	}
	else if (delta < 8*TILEGLOBAL)
	{
	// throw and walk closer
		ob->xdir = 1;
		ob->state = &s_frenchythrow1;
	}
	else
	{
	// walk closer
		ob->xdir = 1;
	}
}


/*
=============================================================================

						  MELON LIPS / CANTALOUPE LIPS

ob->temp1 = direction : 0 = left, 1 = right, 2 = down, 3 = up (new one)

=============================================================================
*/

#define SPITXSPEED	48
#define SPITYSPEED	-20
#define SPITYSPEEDUP	40

#pragma warn -sus

statetype near s_melonside	= {MELONLIPSL1SPR,MELONLIPSR1SPR,step,false,
	false,200, 0,0, NULL, NULL, DrawMelon, &s_melonsidespit};
statetype near s_melonsidespit= {MELONLIPSL2SPR,MELONLIPSR2SPR,step,false,
	false,6, 0,0, MelonSpitThink, NULL, DrawMelon, &s_melonsidespit2};
statetype near s_melonsidespit2= {MELONLIPSL2SPR,MELONLIPSR2SPR,step,false,
	false,6, 0,0, NULL, NULL, DrawMelon, &s_melonside};

statetype near s_melondown	= {MELONLIPSD1SPR,MELONLIPSD1SPR,step,false,
	false,200, 0,0, NULL, NULL, DrawMelon, &s_melondownspit};
statetype near s_melondownspit	= {MELONLIPSD2SPR,MELONLIPSD2SPR,step,false,
	false,6, 0,0, MelonSpitThink, NULL, DrawMelon, &s_melondownspit2};
statetype near s_melondownspit2	= {MELONLIPSD2SPR,MELONLIPSD2SPR,step,false,
	false,6, 0,0, NULL, NULL, DrawMelon, &s_melondown};

statetype near s_melonup	= {MELONLIPSU1SPR,MELONLIPSU1SPR,step,false,
	false,200, 0,0, NULL, NULL, DrawMelon, &s_melonupspit};
statetype near s_melonupspit	= {MELONLIPSU2SPR,MELONLIPSU2SPR,step,false,
	false,6, 0,0, MelonSpitThink, NULL, DrawMelon, &s_melonupspit2};
statetype near s_melonupspit2	= {MELONLIPSU2SPR,MELONLIPSU2SPR,step,false,
	false,6, 0,0, NULL, NULL, DrawMelon, &s_melonup};

statetype near s_melonseed1	= {MELONSEEDL1SPR,MELONSEEDR1SPR,think,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact3, &s_melonseed2};
statetype near s_melonseed2	= {MELONSEEDL2SPR,MELONSEEDR2SPR,think,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact3, &s_melonseed1};

statetype near s_melonseedd1	= {MELONSEEDD1SPR,MELONSEEDD1SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact3, &s_melonseedd2};
statetype near s_melonseedd2	= {MELONSEEDD2SPR,MELONSEEDD2SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact3, &s_melonseedd1};

statetype near s_melonseedu1	= {MELONSEEDU1SPR,MELONSEEDU1SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact3, &s_melonseedu2};
statetype near s_melonseedu2	= {MELONSEEDU2SPR,MELONSEEDU2SPR,stepthink,false,
	false,4, 0,0, ProjectileThink, NULL, ProjectileReact3, &s_melonseedu1};

#pragma warn +sus

/*
====================
=
= SpawnMelon
=
====================
*/

void SpawnMelon (int tilex, int tiley,int dir)
{
	GetNewObj (false);

	new->obclass = melonobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;
	if (dir)
		new->xdir = 1;
	else
		new->xdir = -1;
	if (dir <2)
		NewState (new,&s_melonside);
	else if (dir >2)
		NewState (new,&s_melonup);
	else
		NewState (new,&s_melondown);

	new->ticcount = US_RndT()>>1;
	new->temp1 = dir;
}


/*
===================
=
= DrawMelon
=
===================
*/

void DrawMelon(objtype *ob)
{
	int shape = ob->shapenum;

	if (mapon >= 23 && mapon <= 46)
	{
		if (shape != 0 && shape != -1 && mapon >= 23 && mapon <= 46)
		{
			shape += CANTALOUPELIPSR1SPR - MELONLIPSR1SPR;
		}
		RF_RemoveSprite(&ob->sprite);
		RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
	}
	else
	{
		if (shape != 0 && shape != -1 && mapon >= 1 && mapon <= 22)
		{
			shape += CANTALOUPELIPSR1SPR - MELONLIPSR1SPR;
		}
		RF_RemoveSprite(&ob->sprite);
		RF_PlaceSprite(&ob->sprite,ob->x,ob->y,ob->shapenum,spritedraw,0);
	}
}


/*
===========================
=
= ProjectileReact2
=
===========================
*/

void ProjectileReact2(objtype *ob)
{
	int shape = ob->shapenum;

	if (ob->hitnorth || ob->hiteast || ob->hitsouth || ob->hitwest)
	{
		RemoveObj(ob);
	}
	else
	{
		if (shape != 0 && shape != -1 && mapon >= 23 && mapon <= 46)
		{
			shape += CANTALOUPELIPSR1SPR - MELONLIPSR1SPR;
		}
		RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
	}
}


/*
====================
=
= MelonSpitThink
=
====================
*/

void MelonSpitThink (objtype *ob)
{
	GetNewObj (false);
	if (mapon >= 23 && mapon <= 46)
		new->obclass = shot2obj;
	else
		new->obclass = shotobj;
	switch (ob->temp1)
	{
	case 0:
		new->x = ob->x+24*16;
		new->y = ob->y+8*16;
		new->xdir = ob->xdir;
		new->ydir = 1;
		new->xspeed = -SPITXSPEED-(US_RndT()>>4);
		new->yspeed = SPITYSPEED;
		NewState (new,&s_melonseed1);
		break;
	case 1:
		new->x = ob->x;
		new->y = ob->y+8*16;
		new->xdir = ob->xdir;
		new->ydir = 1;
		new->xspeed = SPITXSPEED+(US_RndT()>>4);
		new->yspeed = SPITYSPEED;
		NewState (new,&s_melonseed1);
		break;
	case 2:
		new->x = ob->x+8*16;
		new->y = ob->y+24*16;
		new->ydir = 1;
		new->yspeed = -SPITYSPEED;
		NewState (new,&s_melonseedd1);
		break;
	case 3:
		new->x = ob->x;
		new->y = ob->y;
		new->xdir = 0;
		new->ydir = -1;
		new->yspeed = -SPITYSPEEDUP;
		NewState(new, &s_melonseedu1);
		break;
	}

	new->active = removable;
}

/*
============================
=
= ProjectileReact
=
============================
*/

void	ProjectileReact (objtype *ob)
{
	unsigned wall,absx,absy,angle,newangle;
	unsigned long speed;

	PLACESPRITE;
	if (ob->hiteast || ob->hitwest)
		ob->xspeed= -ob->xspeed/2;

	if (ob->hitsouth)
		ob->yspeed= -ob->yspeed/2;

	wall = ob->hitnorth;
	if (wall)
	{
		if (ob->yspeed < 0)
			ob->yspeed = 0;

		absx = abs(ob->xspeed);
		absy = ob->yspeed;
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
		if (ob->xspeed > 0)
			angle = 7-angle;

		speed >>= 1;
		newangle = bounceangle[ob->hitnorth][angle];
		switch (newangle)
		{
		case 0:
			ob->xspeed = speed / 286;
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 1:
			ob->xspeed = speed / 362;
			ob->yspeed = -ob->xspeed;
			break;
		case 2:
			ob->yspeed = -(speed / 286);
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 3:

		case 4:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 5:
			ob->yspeed = -(speed / 286);
			ob->xspeed = ob->yspeed / 2;
			break;
		case 6:
			ob->xspeed = ob->yspeed = -(speed / 362);
			break;
		case 7:
			ob->xspeed = -(speed / 286);
			ob->yspeed = ob->xspeed / 2;
			break;

		case 8:
			ob->xspeed = -(speed / 286);
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 9:
			ob->xspeed = -(speed / 362);
			ob->yspeed = -ob->xspeed;
			break;
		case 10:
			ob->yspeed = speed / 286;
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 11:

		case 12:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 13:
			ob->yspeed = speed / 286;
			ob->xspeed = ob->yspeed / 2;
			break;
		case 14:
			ob->xspeed = speed / 362;
			ob->yspeed = speed / 362;
			break;
		case 15:
			ob->xspeed = speed / 286;
			ob->yspeed = ob->xspeed / 2;
			break;
		}

		if (speed < 256*16)
			RemoveObj (ob);
	}
}

/*
============================
=
= ProjectileReact3
=
============================
*/

void	ProjectileReact3 (objtype *ob)
{
	unsigned wall,absx,absy,angle,newangle;
	unsigned long speed;
	int shape = ob->shapenum;

	if (shape != 0 && shape != -1 && mapon >= 23 && mapon <= 46)
	{
		shape += CANTALOUPESEEDR1SPR - MELONSEEDR1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
	if (ob->hiteast || ob->hitwest)
		ob->xspeed= -ob->xspeed/2;

	if (ob->hitsouth)
		ob->yspeed= -ob->yspeed/2;

	wall = ob->hitnorth;
	if (wall)
	{
		if (ob->yspeed < 0)
			ob->yspeed = 0;

		absx = abs(ob->xspeed);
		absy = ob->yspeed;
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
		if (ob->xspeed > 0)
			angle = 7-angle;

		speed >>= 1;
		newangle = bounceangle[ob->hitnorth][angle];
		switch (newangle)
		{
		case 0:
			ob->xspeed = speed / 286;
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 1:
			ob->xspeed = speed / 362;
			ob->yspeed = -ob->xspeed;
			break;
		case 2:
			ob->yspeed = -(speed / 286);
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 3:

		case 4:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 5:
			ob->yspeed = -(speed / 286);
			ob->xspeed = ob->yspeed / 2;
			break;
		case 6:
			ob->xspeed = ob->yspeed = -(speed / 362);
			break;
		case 7:
			ob->xspeed = -(speed / 286);
			ob->yspeed = ob->xspeed / 2;
			break;

		case 8:
			ob->xspeed = -(speed / 286);
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 9:
			ob->xspeed = -(speed / 362);
			ob->yspeed = -ob->xspeed;
			break;
		case 10:
			ob->yspeed = speed / 286;
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 11:

		case 12:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 13:
			ob->yspeed = speed / 286;
			ob->xspeed = ob->yspeed / 2;
			break;
		case 14:
			ob->xspeed = speed / 362;
			ob->yspeed = speed / 362;
			break;
		case 15:
			ob->xspeed = speed / 286;
			ob->yspeed = ob->xspeed / 2;
			break;
		}

		if (speed < 256*16)
			RemoveObj (ob);
	}
}


/*
=============================================================================

						CELERY

=============================================================================
*/

#pragma warn -sus

statetype near s_celerystop1 = {CELERYSTOPLSPR,CELERYSTOPLSPR,step,true,
	false,10, 0,0, CeleryStopThink, NULL, DrawReact, &s_celerystop1};
statetype near s_celerystop2 = {CELERYSTOPRSPR,CELERYSTOPRSPR,step,true,
	false,20, 0,0, CeleryStopThink, NULL, DrawReact, &s_celerystop2};

statetype near s_celeryscare1 = {CELERYATTACKL1SPR,CELERYATTACKR1SPR,step,true,
	false,10, 0,0, CeleryThink, NULL, DrawReact, &s_celeryscare2};
statetype near s_celeryscare2 = {CELERYATTACKL2SPR,CELERYATTACKR2SPR,step,true,
	false,20, 0,0, CeleryThink, NULL, DrawReact, &s_celeryscare1};

#pragma warn +sus

/*
====================
=
= SpawnCelery
=
====================
*/

void SpawnCelery (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = celeryobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-4*BLOCKSIZE;
	new->xdir = 1;
	NewState (new,&s_celerystop1);
}

/*
====================
=
= CeleryThink
=
====================
*/

void CeleryThink (objtype *ob)
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
		ob->state = &s_celeryscare1;
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
		ob->state = &s_celeryscare2;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
}

/*
====================
=
= CeleryStopThink
=
====================
*/

void CeleryStopThink (objtype *ob)
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
		ob->state = &s_celerystop1;
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
		ob->state = &s_celerystop2;
		ob->needtoclip = false;
		ob->xmove = 0;
		return;
	}
}



/*
=============================================================================

						   DREAM MACHINE

=============================================================================
*/

#pragma warn -sus

statetype near s_machine	= {DREAMMACHINESPR,DREAMMACHINESPR,step,false,
	true,10, 0,0, NULL, NULL, DrawReact, &s_machine};

#pragma warn +sus

/*
====================
=
= SpawnMachine
=
====================
*/

void SpawnMachine (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = machineobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-6*BLOCKSIZE;
	new->xdir = 1;
	NewState (new,&s_machine);
}


/*
=============================================================================

						   MUSHROOM

=============================================================================
*/

void MushroomRiseReact (objtype *ob);
void MushroomFallReact (objtype *ob);

#pragma warn -sus

statetype near s_mushroomfall	= {MUSHROOMFALLSPR,MUSHROOMFALLSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, MushroomFallReact, NULL};

statetype near s_mushroomrise	= {MUSHROOMRISESPR,MUSHROOMRISESPR,slide,false,
	false,0, 0,-16, NULL, NULL, MushroomRiseReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnMushroom
=
====================
*/

void SpawnMushroom (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = mushroomobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_mushroomfall);
}


/*
====================
=
= MushroomRiseReact
=
====================
*/

void MushroomRiseReact (objtype *ob)
{
	if (ob->hitsouth)
		ChangeState(ob,&s_mushroomfall);
	PLACESPRITE;
}


/*
====================
=
= MushroomFallReact
=
====================
*/

void MushroomFallReact (objtype *ob)
{
	if (ob->hitnorth)
	{
		ob->yspeed = -(ob->yspeed<<1)/3;
			ChangeState(ob,&s_mushroomrise);
	}
	PLACESPRITE;
}


/*
=============================================================================

						   BRUSSEL SPROUT

=============================================================================
*/

#pragma warn -sus

statetype near s_brusselroll1 = {BRUSSELROLL1SPR,BRUSSELROLL1SPR,step,true,
	true,10, 128,0, NULL, NULL, WalkReact, &s_brusselroll2};
statetype near s_brusselroll2 = {BRUSSELROLL2SPR,BRUSSELROLL2SPR,step,false,
	true,10, 128,0, NULL, NULL, WalkReact, &s_brusselroll3};
statetype near s_brusselroll3 = {BRUSSELROLL3SPR,BRUSSELROLL3SPR,step,true,
	true,10, 128,0, NULL, NULL, WalkReact, &s_brusselroll4};
statetype near s_brusselroll4 = {BRUSSELROLL4SPR,BRUSSELROLL4SPR,step,false,
	true,10, 128,0, NULL, NULL, WalkReact, &s_brusselroll1};

#pragma warn +sus

/*
====================
=
= SpawnBrussel
=
====================
*/

void SpawnBrussel (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = brusselobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-1*BLOCKSIZE;
	new->xdir = -1;
	NewState (new,&s_brusselroll1);
}



/*
=============================================================================

							SQUASHER

=============================================================================
*/

#define SPDSQUASHLEAPY  50


void SquasherThink (objtype *ob);
void SquasherJumpReact (objtype *ob);
void DrawSquasher (objtype *ob);
void SquasherWalkReact (objtype *ob);

#pragma warn -sus

statetype near s_squasherwalk1	= {SQUASHERWALKL1SPR,SQUASHERWALKR1SPR,step,false,
	true,10, 128,0, SquasherThink, ShotContact, SquasherWalkReact, &s_squasherwalk2};
statetype near s_squasherwalk2	= {SQUASHERWALKL2SPR,SQUASHERWALKR2SPR,step,false,
	true,10, 128,0, SquasherThink, ShotContact, SquasherWalkReact, &s_squasherwalk1};

statetype near s_squasherjump1	= {SQUASHERJUMPL1SPR,SQUASHERJUMPR1SPR,stepthink,false,
	false,20, 0,0, ProjectileThink, ShotContact, SquasherJumpReact, &s_squasherjump2};
statetype near s_squasherjump2	= {SQUASHERJUMPL2SPR,SQUASHERJUMPR2SPR,think,false,
	false,0, 0,0, ProjectileThink, ShotContact, SquasherJumpReact, NULL};

statetype near s_squasherwait	= {SQUASHERJUMPL2SPR,SQUASHERJUMPR2SPR,step,false,
	false,10, 0,0, ProjectileThink, ShotContact, DrawSquasher, &s_squasherwalk1};

statetype near s_squasherdead = {SQUASHERSTUNSPR,SQUASHERSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

statetype near s_altsquasherdead = {ALTSQUASHERSTUNSPR,ALTSQUASHERSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnSquasher
=
====================
*/

void SpawnSquasher (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = squashobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_squasherwalk1);
}

/*
====================
=
= DrawSquasher
=
====================
*/

void DrawSquasher(objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape >= SQUASHERWALKR1SPR && shape <= SQUASHERSTUNSPR && mapon >= 23 && mapon <= 46)
	{
		shape += ALTSQUASHERWALKR1SPR - SQUASHERWALKR1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}

/*
====================
=
= SquasherWalkReact
=
====================
*/

void SquasherWalkReact(objtype *ob)
{
	WalkReact(ob);
	DrawSquasher(ob);
}

/*
====================
=
= SquasherThink
=
====================
*/

void SquasherThink (objtype *ob)
{
	int delta;

	if ( abs(ob->y - player->y) > 3*TILEGLOBAL )
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

	ob->yspeed = -SPDSQUASHLEAPY;
	ob->xspeed = delta/60;
	ob->state = &s_squasherjump1;
}

/*
====================
=
= SquasherJumpReact
=
====================
*/

void SquasherJumpReact (objtype *ob)
{
	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
		ChangeState (ob,&s_squasherwait);

//	PLACESPRITE;
	DrawSquasher(ob);
}


/*
=============================================================================

								APEL

temp4 = pole x coordinate

=============================================================================
*/

void ApelThink (objtype *ob);
void ApelClimbThink (objtype *ob);
void ApelSlideThink (objtype *ob);
void ApelFallThink (objtype *ob);

void ApelClimbReact (objtype *ob);
void ApelSlideReact (objtype *ob);
void ApelFallReact (objtype *ob);

#pragma warn -sus

statetype near s_apelwalk1	= {APELWALKL1SPR,APELWALKR1SPR,step,false,
	true,10, 128,0, ApelThink, ShotContact, WalkReact, &s_apelwalk2};
statetype near s_apelwalk2	= {APELWALKL2SPR,APELWALKR2SPR,step,false,
	true,10, 128,0, ApelThink, ShotContact, WalkReact, &s_apelwalk3};
statetype near s_apelwalk3	= {APELWALKL3SPR,APELWALKR3SPR,step,false,
	true,10, 128,0, ApelThink, ShotContact, WalkReact, &s_apelwalk1};

statetype near s_apelclimb1	= {APELSHINNY1SPR,APELSHINNY1SPR,slide,false,
	false,6, 0,-16, ApelClimbThink, ShotContact, ApelClimbReact, &s_apelclimb2};
statetype near s_apelclimb2	= {APELSHINNY2SPR,APELSHINNY2SPR,slide,false,
	false,6, 0,-16, ApelClimbThink, ShotContact, ApelClimbReact, &s_apelclimb1};

statetype near s_apelslide1	= {APELSLIDE1SPR,APELSLIDE1SPR,slide,false,
	false,6, 0,16, ApelSlideThink, ShotContact, ApelFallReact, &s_apelslide2};
statetype near s_apelslide2	= {APELSLIDE2SPR,APELSLIDE2SPR,slide,false,
	false,6, 0,16, ApelSlideThink, ShotContact, ApelFallReact, &s_apelslide3};
statetype near s_apelslide3	= {APELSLIDE3SPR,APELSLIDE3SPR,slide,false,
	false,6, 0,16, ApelSlideThink, ShotContact, ApelFallReact, &s_apelslide4};
statetype near s_apelslide4	= {APELSLIDE4SPR,APELSLIDE4SPR,slide,false,
	false,6, 0,16, ApelSlideThink, ShotContact, ApelFallReact, &s_apelslide1};

statetype near s_apelfall = {APELWALKL1SPR,APELWALKR1SPR,think,false,
	false,0, 0,0, ProjectileThink, ShotContact, ApelFallReact, NULL};

statetype near s_apeldead = {APELSTUNSPR,APELSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus


/*
====================
=
= SpawnApel
=
====================
*/

void SpawnApel (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = apelobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_apelwalk1);
}


/*
====================
=
= ApelThink
=
====================
*/

void ApelThink (objtype *ob)
{
	int	x,y;
	uint16_t  *map;

	if (ob->top > player->bottom || ob->bottom < player->top)
	{
	//
	// try to climb a pole to reach player
	//
		if (ob->y < player->y)
			y = ob->tilebottom;
		else
			y = ob->tiletop;

		map = (uint16_t *)mapsegs[1]+
			mapbwidthtable[y]/2 + ob->tilemidx;

		if ((tinf[INTILE+*map]&0x7f) == 1)
		{
			ob->xmove = (ob->tilemidx<<G_T_SHIFT) - ob->x;
			ob->ymove = 0;
			ob->temp4 = ob->tilemidx;	// for future reference
			ob->needtoclip = false;		// can climb through pole holes
			if (ob->y < player->y)
				ob->state = &s_apelslide1;
			else
				ob->state = &s_apelclimb1;
			return;
		}
	}

	if (US_RndT()>32)		// don't turn around all the time
		return;

	if (ob->x < player->x)
		ob->xdir = 1;
	else
		ob->xdir = -1;

}


/*
====================
=
= ApelClimbThink
=
====================
*/

void ApelClimbThink (objtype *ob)
{
	uint16_t  *map;

	map = (uint16_t *)mapsegs[1]+
		mapbwidthtable[ob->tiletop]/2 + ob->temp4;

	if ((tinf[INTILE+*map]&0x7f) != 1)
	{
		ob->needtoclip = true;
		ob->state = &s_apelfall;
	}
}


/*
====================
=
= ApelSlideThink
=
====================
*/

void ApelSlideThink (objtype *ob)
{
	uint16_t  *map;

	map = (uint16_t *)mapsegs[1]+
		mapbwidthtable[ob->tilebottom]/2 + ob->temp4;

	if ((tinf[INTILE+*map]&0x7f) != 1)
	{
		ob->needtoclip = true;
		ob->state = &s_apelfall;
	}
}


/*
====================
=
= ApelClimbReact
=
====================
*/

void ApelClimbReact (objtype *ob)
{
	if (ob->hitsouth)
		ChangeState (ob,&s_apelfall);
	PLACESPRITE;
}


/*
====================
=
= ApelFallReact
=
====================
*/

void ApelFallReact (objtype *ob)
{
	if (ob->hitnorth)
		ChangeState (ob,&s_apelwalk1);
	PLACESPRITE;
}


/*
=============================================================================

								PEA BRAIN

=============================================================================
*/

#define PEAXSPEED	48
#define PEAYSPEED	-20

void PeaBrainThink (objtype *ob);
void PeaFlyReact (objtype *ob);

#pragma warn -sus

statetype near s_peabrainfly	= {PEABRAINWALKL1SPR,PEABRAINWALKR1SPR,think,false,
	false,0, 0,0, ProjectileThink, ShotContact, PeaFlyReact, NULL};

statetype near s_peabrainwalk1	= {PEABRAINWALKL1SPR,PEABRAINWALKR1SPR,step,false,
	true,10, 128,0, PeaBrainThink, ShotContact, WalkReact, &s_peabrainwalk2};
statetype near s_peabrainwalk2	= {PEABRAINWALKL2SPR,PEABRAINWALKR2SPR,step,false,
	true,10, 128,0, PeaBrainThink, ShotContact, WalkReact, &s_peabrainwalk3};
statetype near s_peabrainwalk3	= {PEABRAINWALKL3SPR,PEABRAINWALKR3SPR,step,false,
	true,10, 128,0, PeaBrainThink, ShotContact, WalkReact, &s_peabrainwalk4};
statetype near s_peabrainwalk4	= {PEABRAINWALKL4SPR,PEABRAINWALKR4SPR,step,false,
	true,10, 128,0, PeaBrainThink, ShotContact, WalkReact, &s_peabrainwalk1};

statetype near s_peabraindead = {PEABRAINSTUNSPR,PEABRAINSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnPeaBrain
=
====================
*/

void SpawnPeaBrain (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = peabrainobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT) + 0 * 16;
	new->xdir = 1;
	new->ydir = 1;
	new->xspeed = PEAXSPEED-(US_RndT()>>4);
	new->yspeed = 0 /* PEAYSPEED */;
	NewState (new,&s_peabrainwalk1);
}

/*
====================
=
= PeaBrainThink
=
====================
*/

void PeaBrainThink (objtype *ob)
{
	(void)ob;
}


/*
====================
=
= PeaFlyReact
=
====================
*/

void PeaFlyReact (objtype *ob)
{
	if (ob->hitnorth)
		ChangeState (ob,&s_peabrainwalk1);

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

void PeaPodThink (objtype *ob);
void SpitPeaBrain (objtype *ob);

#pragma warn -sus

statetype near s_peapodwalk1	= {PEAPODRUNL1SPR,PEAPODRUNR1SPR,step,false,
	true,10, 128,0, PeaPodThink, ShotContact, WalkReact, &s_peapodwalk2};
statetype near s_peapodwalk2	= {PEAPODRUNL2SPR,PEAPODRUNR2SPR,step,false,
	true,10, 128,0, PeaPodThink, ShotContact, WalkReact, &s_peapodwalk3};
statetype near s_peapodwalk3	= {PEAPODRUNL3SPR,PEAPODRUNR3SPR,step,false,
	true,10, 128,0, PeaPodThink, ShotContact, WalkReact, &s_peapodwalk4};
statetype near s_peapodwalk4	= {PEAPODRUNL4SPR,PEAPODRUNR4SPR,step,false,
	true,10, 128,0, PeaPodThink, ShotContact, WalkReact, &s_peapodwalk1};

statetype near s_peapodspit1	= {PEAPODSPITLSPR,PEAPODSPITRSPR,step,false,
	true,30, 0,0, SpitPeaBrain, ShotContact, DrawReact, &s_peapodspit2};
statetype near s_peapodspit2	= {PEAPODSPITLSPR,PEAPODSPITRSPR,step,false,
	true,30, 0,0, NULL, ShotContact, DrawReact, &s_peapodwalk1};

statetype near s_peapoddead = {PEAPODSTUNSPR,PEAPODSTUNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, StunnedReact, NULL};

#pragma warn +sus

/*
====================
=
= SpawnPeaPod
=
====================
*/

void SpawnPeaPod (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = peapodobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_peapodwalk1);
}

/*
====================
=
= SpitPeaBrain
=
====================
*/

void SpitPeaBrain (objtype *ob)
{
	GetNewObj (true);
	new->obclass = peabrainobj;
	if (ob->xdir == 1)
	{
		new->x = ob->x+8*16;
		new->y = ob->y+8*16;
	}
	else
	{
		new->x = ob->x;
		new->y = ob->y+8*16;
	}
	new->xdir = ob->xdir;
	new->ydir = 1;
	new->xspeed = ob->xdir * PEAXSPEED-(US_RndT()>>4);
	new->yspeed = PEAYSPEED;
	NewState (new,&s_peabrainfly);
}



/*
====================
=
= PeaPodThink
=
====================
*/

void PeaPodThink (objtype *ob)
{
	int delta;

	if ( abs(ob->y - player->y) > 3*TILEGLOBAL )
		return;

	if (player->x < ob->x && ob->xdir == 1)
		return;

	if (player->x > ob->x && ob->xdir == -1)
		return;

	if (US_RndT()<8 && ob->temp1 < MAXPEASPIT)
	{
		ob->temp1 ++;
		ob->state = &s_peapodspit1;
		ob->xmove = 0;
	}
}


/*
=============================================================================

		KING BOOBUS TUBER AND QUEEN BUSTLING TUBERINA

temp4 = hit points

=============================================================================
*/

#define PREFRAGTHINK	60
#define POSTFRAGTHINK	60

#define SPDBOOBUSJUMP	60
#define SPDBOOBUSRUNJUMP 24
#define SPDBOOBUSJUMPVERYHIGH 120
#define SPDBOOBUSJUMPHIGH 90
#define SPDBOOBUSJUMPLOW 30

void BoobusThink (objtype *ob);
void FinishThink (objtype *ob, objtype *hit);
void FragThink (objtype *ob);
void BoobusGroundReact (objtype *ob);
void BoobusAirReact (objtype *ob);
void DrawBoobus (objtype *ob);

#pragma warn -sus

statetype near s_boobuswalk1	= {BOOBUSWALKL1SPR,BOOBUSWALKR1SPR,step,false,
	true,10, 128,0, BoobusThink, NULL, BoobusGroundReact, &s_boobuswalk2};
statetype near s_boobuswalk2	= {BOOBUSWALKL2SPR,BOOBUSWALKR2SPR,step,false,
	true,10, 128,0, BoobusThink, NULL, BoobusGroundReact, &s_boobuswalk3};
statetype near s_boobuswalk3	= {BOOBUSWALKL3SPR,BOOBUSWALKR3SPR,step,false,
	true,10, 128,0, BoobusThink, NULL, BoobusGroundReact, &s_boobuswalk4};
statetype near s_boobuswalk4	= {BOOBUSWALKL4SPR,BOOBUSWALKR4SPR,step,false,
	true,10, 128,0, BoobusThink, NULL, BoobusGroundReact, &s_boobuswalk1};

statetype near s_boobusjump	= {BOOBUSJUMPSPR,BOOBUSJUMPSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, BoobusAirReact, NULL};
statetype near s_boobusjumpup	= {BOOBUSJUMPUPSPR,BOOBUSJUMPUPSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, BoobusAirReact, NULL};
statetype near s_boobusjumpdown	= {BOOBUSJUMPDOWNSPR,BOOBUSJUMPDOWNSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, BoobusAirReact, NULL};
statetype near s_boobusjumpleft	= {BOOBUSJUMPLEFTSPR,BOOBUSJUMPLEFTSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, BoobusAirReact, NULL};
statetype near s_boobusjumpright	= {BOOBUSJUMPRIGHTSPR,BOOBUSJUMPRIGHTSPR,think,false,
	false,0, 0,0, ProjectileThink, NULL, BoobusAirReact, NULL};

statetype near s_boobusdie	= {BOOBUSDIESPR,BOOBUSDIESPR,step,false,
	false,4, 0,0, FragThink, NULL, DrawBoobus, &s_boobusdie};
statetype near s_boobusdie2	= {NULL,NULL,step,false,
	false,250, 0,0, FinishThink, NULL, NULL, NULL};

statetype near s_deathboom1	= {BOOBUSBOOM1SPR,BOOBUSBOOM1SPR,step,false,
	false,20, 0,0, ProjectileThink, NULL, DrawReact8, &s_deathboom2};
statetype near s_deathboom2	= {BOOBUSBOOM2SPR,BOOBUSBOOM2SPR,step,false,
	false,20, 0,0, ProjectileThink, NULL, DrawReact8, &s_deathboom3};
statetype near s_deathboom3	= {BOOBUSBOOM3SPR,BOOBUSBOOM3SPR,step,false,
	false,20, 0,0, ProjectileThink, NULL, DrawReact8, &s_deathboom4};
statetype near s_deathboom4	= {POOF1SPR,POOF1SPR,step,false,
	false,40, 0,0, ProjectileThink, NULL, DrawReact7, &s_deathboom5};
statetype near s_deathboom5	= {POOF2SPR,POOF2SPR,step,false,
	false,30, 0,0, ProjectileThink, NULL, DrawReact7, &s_deathboom6};
statetype near s_deathboom6	= {POOF3SPR,POOF3SPR,step,false,
	false,30, 0,0, ProjectileThink, NULL, DrawReact7, &s_deathboom7};
statetype near s_deathboom7	= {POOF4SPR,POOF4SPR,step,false,
	false,30, 0,0, ProjectileThink, NULL, DrawReact7, NULL};


#pragma warn +sus

/*
====================
=
= SpawnBoobus
=
====================
*/

void SpawnBoobus (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = boobusobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-11*BLOCKSIZE;
	new->xdir = -1;
	NewState (new,&s_boobuswalk1);
	switch (gamestate.difficulty)
	{
	case gd_Easy:
		if (US_RndT() < 0x80)
		{
			s_boobusjump.leftshapenum = s_boobusdie.leftshapenum = BOOBUSJUMPLEFTSPR;
			s_boobusjump.rightshapenum = s_boobusdie.rightshapenum = BOOBUSJUMPRIGHTSPR;
		}
		else
		{
			s_boobusjump.leftshapenum = s_boobusjump.rightshapenum = BOOBUSJUMPSPR;
			s_boobusdie.leftshapenum = s_boobusdie.rightshapenum = BOOBUSDIESPR;
		}
		new->temp4 = 8;	// health
		break;
	case gd_Normal:
		if (US_RndT() < 0x80)
		{
			s_boobusjump.leftshapenum = s_boobusdie.leftshapenum = BOOBUSJUMPLEFTSPR;
			s_boobusjump.rightshapenum = s_boobusdie.rightshapenum = BOOBUSJUMPRIGHTSPR;
		}
		else
		{
			s_boobusjump.leftshapenum = s_boobusjump.rightshapenum = BOOBUSJUMPSPR;
			s_boobusdie.leftshapenum = s_boobusdie.rightshapenum = BOOBUSDIESPR;
		}
		new->temp4 = 12;	// health
		break;				
	case gd_Hard:
		if (US_RndT() < 0x80)
		{
			s_boobusjump.leftshapenum = s_boobusdie.leftshapenum = BOOBUSJUMPLEFTSPR;
			s_boobusjump.rightshapenum = s_boobusdie.rightshapenum = BOOBUSJUMPRIGHTSPR;
		}
		else
		{
			s_boobusjump.leftshapenum = s_boobusjump.rightshapenum = BOOBUSJUMPSPR;
			s_boobusdie.leftshapenum = s_boobusdie.rightshapenum = BOOBUSDIESPR;
		}
		new->temp4 = 16;	// health
		break;		
	case gd_Keen:
		if (US_RndT() < 0x80)
		{
			s_boobusjump.leftshapenum = s_boobusdie.leftshapenum = BOOBUSJUMPLEFTSPR;
			s_boobusjump.rightshapenum = s_boobusdie.rightshapenum = BOOBUSJUMPRIGHTSPR;
		}
		else
		{
			s_boobusjump.leftshapenum = s_boobusjump.rightshapenum = BOOBUSJUMPSPR;
			s_boobusdie.leftshapenum = s_boobusdie.rightshapenum = BOOBUSDIESPR;
		}
		new->temp4 = 20;	// health
		break;
	case gd_Grand:
		if (US_RndT() < 0x80)
		{
			s_boobusjump.leftshapenum = s_boobusdie.leftshapenum = BOOBUSJUMPLEFTSPR;
			s_boobusjump.rightshapenum = s_boobusdie.rightshapenum = BOOBUSJUMPRIGHTSPR;
		}
		else
		{
			s_boobusjump.leftshapenum = s_boobusjump.rightshapenum = BOOBUSJUMPSPR;
			s_boobusdie.leftshapenum = s_boobusdie.rightshapenum = BOOBUSDIESPR;
		}
		new->temp4 = 24;	// health
		break;
	}
}


/*
===================
=
= DrawBoobus
=
===================
*/

void DrawBoobus(objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1)
	{
		switch (gamestate.difficulty)
		{
		case gd_Easy:
			shape += BUSTLINGWALKL1SPR - BOOBUSWALKL1SPR;
			break;		
		case gd_Hard:
			shape += SBOOBUSWALKL1SPR - BOOBUSWALKL1SPR;
			break;
		case gd_Keen:
			shape += PBOOBUSWALKL1SPR - BOOBUSWALKL1SPR;
			break;
		}
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}


/*
===================
=
= FragThink
=
===================
*/

void FragThink (objtype *ob)
{
	if (++ob->temp1 == PREFRAGTHINK)
		ob->state = &s_boobusdie;
	if (++ob->temp1 == POSTFRAGTHINK)
	{
		RF_RemoveSprite (&ob->sprite);
		ob->state = &s_boobusdie2;
	}

	SD_PlaySound (BOMBBOOMSND);
	GetNewObj (true);
	new->x = ob->x-BLOCKSIZE + 5*US_RndT();
	new->y = ob->y-BLOCKSIZE + 5*US_RndT();
	new->xspeed = 0; //US_RndT()/4-32;
	new->yspeed = 0; //US_RndT()/4-32;
	US_RndT();					// keep rnd from even wrapping
	ChangeState (new,&s_deathboom1);
}


/*
===================
=
= FinishThink
=
===================
*/

void FinishThink (objtype *ob, objtype *hit)
{
	GetNewObj(false);
	new->needtoclip = false;
	new->obclass = keyobj;
	new->x = ob->x;
	new->y = ob->y;
	new->ydir = -1;
	new->yspeed = -40;
	gamestate.leveldone[mapon] = true;		// Szemigi: Mark the boss level as done.
	new->temp2=new->shapenum = MAGICKEY1SPR;
	new->temp3 = new->temp2 + 2;
	NewState (new,&s_bonus);	
	(void)ob; (void)hit;
}

/*
====================
=
= BoobusThink
=
====================
*/

void BoobusThink (objtype *ob)
{
	unsigned	move;
	boolean	isinline = false;

	if (ob->left > player->right)
		ob->xdir = -1;
	else if (ob->right < player->left)
		ob->xdir = 1;
	else
		isinline = true;

	if (player->top < ob->bottom && player->bottom > ob->top)
	{
	// on same level as player, so charge!

	}
	else
	{
	// above or below player, so get directly in line and jump
		if (isinline)
		{
			if (ob->y < player->y)
			{
			// drop down
				move = PIXGLOBAL*8;
				ob->tilebottom++;
				ob->bottom += move;
				ob->y += move;
				ob->state = &s_boobusjumpdown;
				ob->yspeed = ob->xmove = ob->xspeed = ob->ymove = 0;
			}
			else
			{
			// jump up
				ob->state = &s_boobusjumpup;
				if (gamestate.difficulty == gd_Keen)
					ob->yspeed = -SPDBOOBUSJUMPVERYHIGH;
				else if (gamestate.difficulty == gd_Easy)
					ob->yspeed = -SPDBOOBUSJUMPLOW;
				else if (gamestate.difficulty == gd_Hard)
					ob->yspeed = -SPDBOOBUSJUMPHIGH;
				else
					ob->yspeed = -SPDBOOBUSJUMP;
				ob->xspeed = 0;
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

void BoobusGroundReact (objtype *ob)
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
		if (ob->bottom >= player->bottom)
		{
		 // jump over
			ob->x -= ob->xmove;
			ob->y -= ob->ymove;
			if (gamestate.difficulty == gd_Keen)
				ob->yspeed = -SPDBOOBUSJUMPVERYHIGH;
			else if (gamestate.difficulty == gd_Easy)
				ob->yspeed = -SPDBOOBUSJUMPLOW;
			else if (gamestate.difficulty == gd_Hard)
				ob->yspeed = -SPDBOOBUSJUMPHIGH;
			else
				ob->yspeed = -SPDBOOBUSJUMP;
			ob->xspeed = ob->xdir * SPDBOOBUSRUNJUMP;
		}
		else
		{
		// drop down
			ob->xspeed = ob->yspeed = 0;
		}
		ChangeState (ob,&s_boobusjump);	
	}

	DrawBoobus(ob);
}



/*
====================
=
= BoobusAirReact
=
====================
*/

void BoobusAirReact (objtype *ob)
{
	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
		ChangeState (ob,&s_boobuswalk1);

	DrawBoobus(ob);
}


/*
=============================================================================

				PEAR

=============================================================================
*/

#pragma warn -sus

statetype near s_pearwalk1	= {PEARWALKL1SPR,PEARWALKR1SPR,step,false,
	true,10, 128,0, PearThink, NULL, WalkReact, &s_pearwalk2};
statetype near s_pearwalk2	= {PEARWALKL2SPR,PEARWALKR2SPR,step,false,
	true,10, 128,0, PearThink, NULL, WalkReact, &s_pearwalk1};

#pragma warn +sus

void PearThink(objtype *ob);

/*
====================
=
= SpawnPear
=
====================
*/

void SpawnPear (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = keyobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	new->ydir = 1;
	NewState (new,&s_pearwalk1);
}

/*
====================
=
= PearThink
=
====================
*/

void PearThink (objtype *ob)
{
	ob->needtoreact = true;
	if (player->right < ob->left)
		ob->xdir = -1;
	if (player->left > ob->right)
		ob->xdir = 1;
}


/*
=============================================================================

				PUMPKIN

=============================================================================
*/

#pragma warn -sus

statetype near s_pumpkin   = {PUMPKINSITSPR, PUMPKINSITSPR, step, false, true, 20, 0, 0, PumpkinSitThink, NULL, DrawReact, &s_pumpkin};
statetype near s_pumpkinjump1  = {PUMPKINJUMPSPR, PUMPKINJUMPSPR, stepthink, false, false, 24, 0, 0, WeakProjectileThink, ShotContact, PumpkinAirReact, &s_pumpkinjump2};
statetype near s_pumpkinjump2  = {PUMPKINJUMPSPR, PUMPKINJUMPSPR, stepthink, false, false, 10, 0, 0, WeakProjectileThink, ShotContact, PumpkinAirReact, &s_pumpkinjump1};
statetype near s_pumpkinbounce = {PUMPKINJUMPSPR, PUMPKINJUMPSPR, think, false, false, 10, 0, 0, ProjectileThink, ShotContact, PumpkinBounceReact, NULL};
statetype near s_pumpkindead   = {PUMPKINDEADSPR, PUMPKINDEADSPR, think, false, false, 12, 0, 0, ProjectileThink, NULL, StunnedReact, &s_pumpkindead};

#pragma warn +sus

void PumpkinThink (objtype *ob);
void PumpkinSitThink (objtype *ob);
void PumpkinAirReact (objtype *ob);
void PumpkinBounceReact (objtype *ob);

/*
==================
=
= SpawnPumpkin
=
==================
*/

void SpawnPumpkin(int x, int y)
{
	GetNewObj(false);
	new->obclass = pumpkinobj;
	new->active = yes;
	new->x = CONVERT_TILE_TO_GLOBAL(x);
	new->y = CONVERT_TILE_TO_GLOBAL(y) + (15-TILEGLOBAL);
	new->ydir = new->xdir = 1;
	NewState(new, &s_pumpkin);
}

/*
===========================
=
= PumpkinSitThink
=
===========================
*/

void PumpkinSitThink(objtype *ob)
{
	if (abs(ob->bottom - player->bottom) > 5*TILEGLOBAL)
		return;

	if (abs(ob->x - player->x) > 3*TILEGLOBAL)
	{
		if (player->x < ob->x)
		{
			if (player->xdir == 1)
			{
				ob->xdir = -1;
				ob->state = &s_pumpkinjump1;
			}
		}
		else
		{
			if (player->xdir == -1)
			{
				ob->xdir = 1;
				ob->state = &s_pumpkinjump1;
			}
		}
	}
}

/*
==================
=
= PumpkinThink
=
==================
*/

void PumpkinThink(objtype *ob)
{
	int dist;

	AccelerateX(ob, ob->xdir, 8);
	if (ob->temp1)
	{
		if (ob->temp1 < tics)
		{
			ob->ymove = ob->yspeed * ob->temp1;
			ob->temp1 = 0;
		}
		else
		{
			ob->ymove = ob->yspeed * tics;
			ob->temp1 = ob->temp1 - tics;
		}
	}
	else
	{
		DoGravity(ob);
	}
	
	if (ob->top > player->bottom || ob->bottom < player->top)
	{
		return;
	}
	
	if (ob->xdir == -1)
	{
		dist = ob->left - player->right;
		if (dist > 4*TILEGLOBAL)
		{
			return;
		}
		ob->xdir = 1;
		return;
	}
	else
	{
		dist = player->left - ob->right;
		if (dist > 4*TILEGLOBAL)
		{
			return;
		}
		ob->xdir = -1;
		return;
	}
}

/*
===========================
=
= PumpkinAirReact
=
===========================
*/

void PumpkinAirReact(objtype *ob)
{
	if (ob->hitnorth)
	{
		SD_PlaySound(BOUNCESND);
		ob->yspeed = -50;
		ChangeState(ob, &s_pumpkinbounce);
	}

	if (ob->hitnorth || ob->hitsouth)
		ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}

/*
===========================
=
= PumpkinBounceReact
=
===========================
*/

void PumpkinBounceReact(objtype *ob)
{
	if (ob->hitnorth)
	{
		ChangeState(ob, &s_pumpkin);
	}

	if (ob->hitnorth)
		ob->yspeed = 0;

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
=============================================================================

						   PINK OWL

=============================================================================
*/

#pragma warn -sus

statetype near s_pinkowl	 = {PINKOWLSPR,PINKOWLSPR,stepthink,false,
	false,0, 0,0, NULL, NULL, DrawReact, &s_pinkowl};
statetype near s_pinkowl2	 = {PINKOWLSPR,PINKOWLSPR,stepthink,false,
	false,0, 0,0, NULL, NULL, DrawReact, &s_pinkowl2};
statetype near s_pinkowl3	 = {PINKOWLSPR,PINKOWLSPR,stepthink,false,
	false,0, 0,0, NULL, NULL, DrawReact, &s_pinkowl3};

#pragma warn +sus

/*
====================
=
= SpawnPinkOwl
=
====================
*/

void SpawnPinkOwl (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = pinkowlobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-18*PIXGLOBAL;
	new->xdir = 1;
	NewState (new,&s_pinkowl);
}

/*
====================
=
= SpawnPinkOwl2
=
====================
*/

void SpawnPinkOwl2 (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = pinkowl2obj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-18*PIXGLOBAL;
	new->xdir = 1;
	NewState (new,&s_pinkowl2);
}

/*
====================
=
= SpawnPinkOwl3
=
====================
*/

void SpawnPinkOwl3 (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = pinkowl3obj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-18*PIXGLOBAL;
	new->xdir = 1;
	NewState (new,&s_pinkowl3);
}

/*
=============================================================================

				ROVER (AT THE EXIT DOOR OF CANTALOUPE MINE)

=============================================================================
*/

#pragma warn -sus

statetype near s_rover3	= {ROVER3SPR,ROVER3SPR,step,true,
	true,30, 0,0, NULL, NULL, DrawReact, &s_rover4};
statetype near s_rover4	= {ROVER4SPR,ROVER4SPR,step,true,
	true,70, 0,0, NULL, NULL, DrawReact, &s_rover3};

#pragma warn +sus

/*
====================
=
= SpawnRover
=
====================
*/

void SpawnRover (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = roverobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-2*BLOCKSIZE;
	new->xdir = 1;
	NewState (new,&s_rover3);
}

/*
=============================================================================

				BEAN SEED

=============================================================================
*/

#pragma warn -sus

statetype near s_beanwalk1	 = {BEANWALKL1SPR,BEANWALKR1SPR,step,true,
	true,10, 128,0, NULL, BeanContact, WalkReact, &s_beanwalk2};
statetype near s_beanwalk2	 = {BEANWALKL2SPR,BEANWALKR2SPR,step,false,
	true,10, 128,0, NULL, BeanContact, WalkReact, &s_beanwalk3};
statetype near s_beanwalk3	 = {BEANWALKL3SPR,BEANWALKR3SPR,step,true,
	true,10, 128,0, NULL, BeanContact, WalkReact, &s_beanwalk1};

statetype near s_beanstunned = {BEANWALKSTUNSPR,BEANWALKSTUNSPR,stepthink,
	false,false,240, 0,0, ProjectileThink, ShotContact, DrawReact, &s_beanwalk1};

#pragma warn +sus

/*
====================
=
= SpawnBean
=
====================
*/

void SpawnBean (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = beanobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;
	new->xdir = 1;
	NewState (new,&s_beanwalk1);
}

/*
===========================
=
= BeanContact
=
===========================
*/

void BeanContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_beanstunned);
	}
	else if (hit->obclass == keenobj)
	{
		hit->obclass = nothing;
		KillKeen();
	}
}


/*
=============================================================================

				GUARD ROBOT

=============================================================================
*/

#pragma warn -sus

statetype near s_guardrobot1	 = {GUARDROBOT1SPR,GUARDROBOT1SPR,step,false,
	false,100, 0,0, NULL, GuardRobotContact, DrawReact10, &s_guardrobot2};
statetype near s_guardrobot2	 = {GUARDROBOT2SPR,GUARDROBOT2SPR,step,false,
	false,50, 0,0, NULL, GuardRobotContact, DrawReact10, &s_guardrobot3};
statetype near s_guardrobot3	 = {GUARDROBOT3SPR,GUARDROBOT3SPR,step,false,
	false,10, 0,0, GuardRobotThink, GuardRobotContact, DrawReact10, &s_guardrobot4};
statetype near s_guardrobot4	 = {GUARDROBOT3SPR,GUARDROBOT3SPR,step,false,
	false,10, 0,0, NULL, GuardRobotContact, DrawReact10, &s_guardrobot1};

statetype near s_guardrobot5	 = {GUARDROBOT1SPR,GUARDROBOT1SPR,step,false,
	false,100, 0,0, NULL, GuardRobotContact, DrawReact10, &s_guardrobot6};
statetype near s_guardrobot6	 = {GUARDROBOT2SPR,GUARDROBOT2SPR,step,false,
	false,50, 0,0, NULL, GuardRobotContact, DrawReact10, &s_guardrobot5};

#pragma warn +sus


/*
===========================
=
= SpawnGuardRobot
=
===========================
*/

void SpawnGuardRobot(int tilex, int tiley)
{
	GetNewObj(false);
	new->obclass = guardrobotobj;
	new->active = yes;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)+2*BLOCKSIZE;
	new->ydir = new->xdir = 1;
	NewState(new, &s_guardrobot1);
}


/*
===========================
=
= GuardRobotThink
=
===========================
*/

void GuardRobotThink(void)
{
	SD_PlaySound (TATERSWINGSND);
	return;
}


/*
===========================
=
= GuardRobotContact
=
===========================
*/

void GuardRobotContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		StunObj(ob, hit, &s_guardrobot5);
	}
}

/*
=================================================================

 			UNKNOWN KID

=================================================================
*/

#pragma warn -sus

statetype near s_kid     = {KIDSPR, KIDSPR, stepthink, false, false, 0, 0, 0, NULL, NULL, DrawReact, &s_kid};

#pragma warn +sus


/*
==================
=
= SpawnKid
=
==================
*/

void SpawnKid (int tilex, int tiley)
{
	GetNewObj (false);

	new->obclass = kidobj;
	new->needtoclip = true;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT) - BLOCKSIZE*2;

	new->xdir = 1;
	new->ydir = -1;
	NewState (new,&s_kid);
}


/*
=============================================================================

				ROYAL KEYCARD

=============================================================================
*/

#pragma warn -sus

statetype near s_keycard 	 = {KEYCARDSPR,KEYCARDSPR,step,false,
	false,20, 0,0, KeycardThink, NULL, DrawReact2, &s_keycard};

#pragma warn +sus

/*
====================
=
= SpawnKeycard
=
====================
*/

void SpawnKeycard (int tilex, int tiley)
{
	GetNewObj (false);

	new->needtoclip = false;
	new->obclass = keycardobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = tiley<<G_T_SHIFT;
	new->ydir = -1;			// bonus stuff flies up when touched
	NewState (new,&s_keycard);
}

/*
====================
=
= KeycardThink
=
====================
*/

void KeycardThink (objtype *ob)
{
	if (++ob->shapenum == ob->temp3)
		ob->shapenum = ob->temp2;
}