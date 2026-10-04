/*
Keen Worlds: Foray in the Forest's map Keen and flags, converted from
CK_KEEN2.C of its source (Nisaba and K1n9_Duk3; GPL v2 or later) by
tools/kw_fitf_port.py, with the hand fixes of tools/kw_fitf_map_fixes.py.
*/

#include "kw_fitf.h"

void FF_T_KeenWorldWalk(CK_object *ob);
#define max(a, b) ((a) > (b) ? (a) : (b))

/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2023 Nisaba
 * Copyright (C) 2019-2025 K1n9_Duk3
 *
 * The code in this file is based on:
 * Keen Dreams Source Code
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

// CK_KEEN2.C


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


void FF_T_MapShip(CK_object *ob);

//===========================================================================
//
// MAP KEEN
//
//===========================================================================


static int worldshapes[] ={ FF_WORLDKEENU1SPR-1, FF_WORLDKEENUR1SPR-1, FF_WORLDKEENR1SPR-1, FF_WORLDKEENDR1SPR-1, FF_WORLDKEEND1SPR-1, FF_WORLDKEENDL1SPR-1, FF_WORLDKEENL1SPR-1, FF_WORLDKEENUL1SPR-1 };
static int worldanims[] ={ 2, 3, 1, 3}; 

void FF_SpawnWorldKeen(int tileX, int tileY)
{
	uint16_t info = INFOSPOT(tileX+1, tileY);

	ck_keenObj->active = OBJ_ALWAYS_ACTIVE;
	if (ck_gameState.mapPosX == 0 && info != 0)
	{
		int px, py;

		ck_keenObj->clipped = CLIP_not;
		ck_keenObj->type = CT_Friendly;
		ck_keenObj->posX = RF_TileToUnit(tileX);
		ck_keenObj->posY = RF_TileToUnit(tileY);
		ck_keenObj->zLayer = FOREGROUNDPRIORITY;
		// get target coordinates:
		px = (info >> 8);
		py = (info & 0xFF);
		ck_keenObj->user1 = RF_TileToUnit(px);
		ck_keenObj->user2 = RF_TileToUnit(py) - 20*16;	// -4 pixels so the ship can land
		// erase BWB tiles at target position:
		CA_SetTileAtPos(px, py, 1, 0);
		CA_SetTileAtPos(px + 1, py, 1, 0);
		CK_SetAction(ck_keenObj, CK_ACTION(FF_ACT_MapShip));
		CA_MarkGrChunk(ck_keenObj->gfxChunk);
	}
	else
	{
		ck_keenObj->type = CT_Player;
		if (ck_gameState.mapPosX == 0)
		{
			ck_keenObj->posX = RF_TileToUnit(tileX);
			ck_keenObj->posY = RF_TileToUnit(tileY);
		}
		else
		{
			ck_keenObj->posX = ck_gameState.mapPosX;
			ck_keenObj->posY = ck_gameState.mapPosY;
			ck_gameState.levelsDone[MAP_BWBMEGAROCKET] = true;	// small hack
		}
		ck_keenObj->zLayer = 1;
		ck_keenObj->xDirection= ck_keenObj->yDirection = IN_motion_None;
		ck_keenObj->user1 = IN_dir_South;	// K1n9_Duk3 fix: using enum name
		ck_keenObj->user2 = 3;
		ck_keenObj->user3 = 0;
		ck_keenObj->gfxChunk = FF_WORLDKEEND3SPR;
		CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_MapKeenStart));
	}
}

void FF_T_MapShip(CK_object *ob)	// K1n9_Duk3 addition
{
	if (ob->type == CT_Player)
	{
		//
		// ship has reached the target position, now move it down
		// to let it land and then wait 1 second before entering
		// the next level
		//
		if (ob->posY < ob->user2 + 20*16)	// (4)
		{
			ck_nextY = SD_GetSpriteSync() * 6;	// move down (2)
		}
		else if ((ob->actionTimer += SD_GetSpriteSync()) > 1*70)
		{
			ck_gameState.mapPosX = ob->posX;
			ck_gameState.mapPosY = ob->posY;
			ff_map.nextLevel = MAP_BWBMEGAROCKET;
			ck_gameState.currentLevel = MAP_BWBMEGAROCKET; // KEEN WORLDS: the engine's "next level"
			ck_gameState.levelState = LS_LevelComplete; // its ex_landed
			SD_PlaySound(FF_ENTERLEVELSND);
		}
	}
	else if (ob->posX == ob->user1 && ob->posY == ob->user2)
	{
		ob->type = CT_Player;
	}
	else
	{
		int xdist, ydist, vx, vy;
		int dist, speed;

		// calculate distance from target:
		xdist = ob->user1 - ob->posX;
		ydist = ob->user2 - ob->posY;
		vx = abs(xdist);
		vy = abs(ydist);
		dist = max(vx, vy);

		// set base speed based on distance
		if (dist < 2*256)
		{
			speed = 8 * SD_GetSpriteSync();
		}
		else if (dist < 4*256)
		{
			speed = 14 *SD_GetSpriteSync();
		}
		else if (dist < 8*256)
		{
			speed = 30 * SD_GetSpriteSync();
		}
		else
		{
			speed = 48 * SD_GetSpriteSync();
		}

		// set movement speed for the current vector
		if (vx > vy)
		{
			long tmp = (long)ydist * (long)speed;
			ck_nextY = tmp / vx;
			ck_nextX = (xdist < 0) ? -speed : speed;
		}
		else
		{
			long tmp = (long)xdist * (long)speed;
			ck_nextX = tmp / vy;
			ck_nextY = (ydist < 0) ? -speed : speed;
		}

		// don't move past the target:
		if (ck_nextX > 0)
		{
			if (ck_nextX + ob->posX > ob->user1)
				ck_nextX = ob->user1 - ob->posX;
		}
		else
		{
			if (ck_nextX + ob->posX < ob->user1)
				ck_nextX = ob->user1 - ob->posX;
		}
		if (ck_nextY > 0)
		{
			if (ck_nextY + ob->posY > ob->user2)
				ck_nextY = ob->user2 - ob->posY;
		}
		else
		{
			if (ck_nextY + ob->posY < ob->user2)
				ck_nextY = ob->user2 - ob->posY;
		}
	}
}


//look for level entry

/*
======================
=
= CheckEnterLevel
=
======================
*/

void FF_CheckEnterLevel(CK_object * ob)
{

	int tx, ty;

	for (ty = ob->clipRects.tileY1; ty <= ob->clipRects.tileY2; ty++)
	{
		for (tx = ob->clipRects.tileX1; tx <= ob->clipRects.tileX2; tx++)
		{
			uint16_t infotile = INFOSPOT(tx, ty);
			
			if ((infotile == (0xC000 + MAP_BONUSLEVEL)) && (ff_state.cheated || ck_gameState.keenScore < BONUSLEVEL_SCORELIMIT))
			{	
				// do nothing (don't enter the level)
			}
			else if (infotile > 0xC000 && infotile < 0xC000+FF_GAMELEVELS)
			{
				// Vanilla keen stores the current map loaded in the cache manager
				// and the "current_map" variable stored in the gamestate
				// would have been changed here.
				ck_gameState.mapPosX = ob->posX;
				ck_gameState.mapPosY = ob->posY;
				ff_map.nextLevel = infotile - 0xC000;
				ck_gameState.currentLevel = infotile - 0xC000; // KEEN WORLDS: the engine's "next level"
				ck_gameState.levelState = LS_LevelComplete;	// K1n9_Duk3 fix: use enum name instead of number
				SD_PlaySound(FF_ENTERLEVELSND);
				return;
			}
			
		}
	}
}


/*
======================
=
= T_KeenWorld
=
======================
*/

void FF_T_KeenWorld(CK_object * ob)
{

	if (ck_inputFrame.dir != IN_dir_None)
	{
		ob->currentAction = CK_ACTION(CK_ACT_MapKeenWalk0);
		ob->user2 = 0;
		FF_T_KeenWorldWalk(ob);
	}

	if (ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed)
	{
		FF_CheckEnterLevel(ob);
	}
}

/*
======================
=
= T_KeenWorldWalk
=
======================
*/

// temp1 = last dir;
// temp2 = walking frame ticker
// temp3 = 

void FF_T_KeenWorldWalk(CK_object * ob)
{

	if (ob->user3 == 0)
	{
		ob->xDirection = ck_inputFrame.xDirection;
		ob->yDirection = ck_inputFrame.yDirection;
		if (ck_keenState.pogoIsPressed || ck_keenState.jumpIsPressed || ck_keenState.shootIsPressed)
			FF_CheckEnterLevel(ob);

		// Go back to standing if no arrows pressed
		if (ck_inputFrame.dir == IN_dir_None)
		{
			ob->currentAction = CK_ACTION(CK_ACT_MapKeenStart);
			ob->gfxChunk = worldshapes[ob->user1] + 3;
			return;
		}
		else
		{
			ob->user1 = ck_inputFrame.dir;
		}
	}
	else
	{
		if ((ob->user3 -= 4) < 0)
			ob->user3 = 0;
	}

	// Advance Walking Frame Animation
	if (++ob->user2 == 4)
		ob->user2 = 0;
	ob->gfxChunk = worldshapes[ob->user1] + worldanims[ob->user2];

	//walk hi lo sound
	if (ob->user2 == 1)
	{
		SD_PlaySound(FF_KEENWALK1SND);
	}
	else if (ob->user2 == 3)
	{
		SD_PlaySound(FF_KEENWALK2SND);
	}
}



/*
======================
=
= CheckWorldInTiles
=
======================
*/

void FF_CheckWorldInTiles(CK_object *keen)
{

	int midTileX, midTileY;
	uint16_t tile;
	uint8_t info;

	if (keen->user3)
		return;

	midTileX = keen->clipRects.tileXmid;
	midTileY = ((keen->clipRects.unitY2 - keen->clipRects.unitY1) / 2 + keen->clipRects.unitY1) >> 8;

	tile = CA_TileAtPos(midTileX, midTileY, 1);
	info = TI_ForeMisc(tile);

	switch (info)
	{
		// Check for special tile properties here
		default:
			break;
	}

}

//===========================================================================
//
// MAP FLAGS
// 
//===========================================================================

/*
*/


void FF_SpawnFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff);
/*
void CK_SpawnFlippingFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff);
void TossThink (objtype *ob);
void PathThink (objtype *ob);
void FlagAlign(objtype *ob);
*/
void FF_R_Flag(CK_object *ob);

/*
*/

//void SpawnRisingFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff);
void FF_T_FlagRise(CK_object *ob);


void FF_SpawnFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff)
{

	CK_object *new_object = CK_GetNewObj(false);

	new_object->clipped = CLIP_not;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->type = CT5_MapFlag;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->posX = RF_TileToUnit(tileX) + 6*16;
	new_object->posY = RF_TileToUnit(tileY) - 30*16;
	new_object->actionTimer = US_RndT() / 16;

	new_object->user4 = spriteoff;

	CK_SetAction(new_object, CK_ACTION(CK_ACT_MapFlag0));
}

void FF_SpawnRisingFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->clipped = CLIP_not;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->type = CT5_MapFlag;
	new_object->posX = RF_TileToUnit(tileX) + 6*16;
	new_object->posY = RF_TileToUnit(tileY) - 30*16;
	new_object->user4 = spriteoff;
	new_object->gfxChunk = FF_FLAGFLIP1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_MapFlagRise));
}

void FF_T_FlagRise(CK_object *ob)
{
	if (!vl_screenFaded)
	{
		if(!ob->user1)
		{
			ob->user1++;
			SD_PlaySound(FF_FLAGSPINSND);
		}
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 6)
		{
			ob->actionTimer -= 6;
			if (++ob->gfxChunk == FF_FLAGWAVE1SPR)
			{
				ob->currentAction = CK_ACTION(CK_ACT_MapFlag0);
				SD_PlaySound(FF_FLAGLANDSND);
			}
		}
	}
}

/*
typedef struct {
	uint16_t x, y;
} flagpointtype;

flagpointtype flagpoints[30];

void CK_SpawnFlippingFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff)
{
	long deltaX, deltaY;
	int i;

	objtype *new = GetNewObj(false);
	
	new->clipped = cl_noclip;
	new->zLayer = FOREGROUNDPRIORITY;
	new->type = CT_MapFlag;
	new->active = ac_allways;
	new->posX = ck_gameState.mapPosX - 0x100;
	new->posY = ck_gameState.mapPosY - 0x100;

	new->user4 = spriteoff;

	// Destination coordinates
	new->user1 = (tileX<<8) + 0x60;
	new->user2 = (tileY<<8) - 0x260;
	
	deltaX = (int)new->user1 - (int)new->posX;
	deltaY = (int)new->user2 - (int)new->posY;

	// Make a table of coordinates for the flag's path
	for (i = 0; i < 30; i++)
	{

		// Draw points in a straight line between keen and the holster
		flagpoints[i].posX = new->posX + deltaX*(i<24?i:24)/24;
		flagpoints[i].posY = new->posY + deltaY*i/30;

		// Offset the Y points to mimic a parabolic trajectory
		if (i < 10)
			flagpoints[i].posY -= i * 0x30;
		else if (i < 15) // starts to level
			flagpoints[i].posY -= i * 16 + 0x140;
		else if (i < 20) // flag starts falling
			flagpoints[i].posY -= (20 - i) * 16 + 0x1E0;
		else 		// starts falling faster
			flagpoints[i].posY -= (29 - i) * 0x30;
	}

	NewState(new, CK_ACTION(FF_ACT_MapFlagFlips0));
}


void TossThink (objtype *ob)
{
	// Start the throw when screen becomes light
	if (!vl_screenFaded)
	{
		SD_StopSound();
		SD_PlaySound(FLAGSPINSND);
		ob->currentAction = ob->currentAction->next;
	}
}


void PathThink (objtype *ob)
{
	ob->user3 += tics;

	if (ob->user3 > 58)
		ob->user3 = 58;

	// Move the flag along the path
	ob->posX = flagpoints[ob->user3/2].posX;
	ob->posY = flagpoints[ob->user3/2].posY;

	ob->visible = true;
	if (!ob->user1)
		SD_PlaySound(FLAGSPINSND);
}

void FlagAlign(objtype *ob)
{

	// Put the flag in its holster
	ob->posX = ob->user1;
	ob->posY = ob->user2 + 0x80;

	SD_PlaySound(FLAGLANDSND);
}
*/

void FF_R_Flag(CK_object *ob)
{
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk+ob->user4, false, ob->zLayer);
}

void kw_ff_map_SetupFunctions(void)
{
	CK_ACT_AddFunction("FF_CheckEnterLevel", &FF_CheckEnterLevel);
	CK_ACT_AddFunction("FF_CheckWorldInTiles", &FF_CheckWorldInTiles);
	CK_ACT_AddFunction("FF_R_Flag", &FF_R_Flag);
	CK_ACT_AddFunction("FF_T_FlagRise", &FF_T_FlagRise);
	CK_ACT_AddFunction("FF_T_KeenWorld", &FF_T_KeenWorld);
	CK_ACT_AddFunction("FF_T_KeenWorldWalk", &FF_T_KeenWorldWalk);
	CK_ACT_AddFunction("FF_T_MapShip", &FF_T_MapShip);
}
