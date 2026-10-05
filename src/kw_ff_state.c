/*
Keen Worlds: Foray in the Forest's movement, wall clipping and state
machine, converted from CK_STATE.C of its source (Nisaba and K1n9_Duk3; GPL
v2 or later) by tools/kw_fitf_port.py, with the hand fixes of
tools/kw_fitf_state_fixes.py. The engine's own functions hand over to these
in this world (ck_phys.c, ck_play.c): its states push objects to floors,
ceilings and walls, its slopes and custom clip boxes differ from Keen 5's.
*/

#include "kw_fitf.h"
#include "kw_ff_keen.h"
#include "kw_ff_state.h"

static bool FF_SlopesNearby(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t walltype);

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

// CK_STATE.C


/*
=============================================================================

	LOCAL CONSTANTS

=============================================================================
*/

// maximum difference between two adjacent positions on a slope, in global units
// (see wallclip)

#define SLOPEMAX	16

#define MAXMOVE	(256-1-SLOPEMAX)
#define MAXFALLSPEED 70

// K1n9_Duk3 says:
// The maximum falling speed of 70 units per tic is a source for unintended
// behavior when the game is running at a very low frame rate. Since the
// clipping routines don't allow anything to move more than 1 tile per frame,
// this means that once the game takes 4 or more tics per frame (17.5 fps or
// less), the level objects don't fall as fast as they are supposed to.
// At 4 tics and 70 units per tic, the objects would need to move by 280 units,
// but the clipping routines only move it by MAXMOVE (or MAXMOVE+16) units.
// So since the objects don't fall as fast as they are supposed to, they can
// actually jump longer distances at low frame rates.
//
// A maximum falling speed of 48 would be the highest value you can use without
// running into issues like this, assuming the MINTICS value is set to 5 like
// in Keen 4-6. If MINTICS is 6, then the maximum speed would be 40.


/*
=============================================================================

	GLOBAL VARIABLES

=============================================================================
*/

// (ck_nextX and ck_nextY are the engine's)

/*
=============================================================================

	LOCAL VARIABLES

=============================================================================
*/



static int16_t wallclip[8][16] = {
	{ 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100 },
	{   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0 },
	{   0x0,   0x8,  0x10,  0x18,  0x20,  0x28,  0x30,  0x38,  0x40,  0x48,  0x50,  0x58,  0x60,  0x68,  0x70,  0x78 },
	{  0x80,  0x88,  0x90,  0x98,  0xa0,  0xa8,  0xb0,  0xb8,  0xc0,  0xc8,  0xd0,  0xd8,  0xe0,  0xe8,  0xf0,  0xf8 },
	{   0x0,  0x10,  0x20,  0x30,  0x40,  0x50,  0x60,  0x70,  0x80,  0x90,  0xa0,  0xb0,  0xc0,  0xd0,  0xe0,  0xf0 },
	{  0x78,  0x70,  0x68,  0x60,  0x58,  0x50,  0x48,  0x40,  0x38,  0x30,  0x28,  0x20,  0x18,  0x10,  0x08,  0x0  },
	{  0xF8,  0xF0,  0xE8,  0xE0,  0xD8,  0xD0,  0xC8,  0xC0,  0xB8,  0xB0,  0xA8,  0xA0,  0x98,  0x90,  0x88,  0x80 },
	{  0xF0,  0xE0,  0xD0,  0xC0,  0xB0,  0xA0,  0x90,  0x80,  0x70,  0x60,  0x50,  0x40,  0x30,  0x20,  0x10,  0x0  }
};

uint16_t oldtileleft, oldtiletop, oldtileright, oldtilebottom, oldtilemidx;
uint16_t oldleft, oldtop, oldright, oldbottom, oldmidx;
int16_t leftmoved, topmoved, rightmoved, bottommoved, midxmoved;

//==========================================================================

/*
====================
=
= CK_CalcTileClipRects	(K1n9_Duk3 addition)
=
= Helper routine that contains some shared code (to reduce redundancy)
=
====================
*/

void FF_CalcTileClipRects(CK_object *ob)
{
	ob->clipRects.unitXmid = (ob->clipRects.unitX2 - ob->clipRects.unitX1) / 2 + ob->clipRects.unitX1;

	ob->clipRects.tileX1 = RF_UnitToTile(ob->clipRects.unitX1);
	ob->clipRects.tileX2 = RF_UnitToTile(ob->clipRects.unitX2);
	ob->clipRects.tileY1 = RF_UnitToTile(ob->clipRects.unitY1);
	ob->clipRects.tileY2 = RF_UnitToTile(ob->clipRects.unitY2);

	ob->clipRects.tileXmid = RF_UnitToTile(ob->clipRects.unitXmid);
}

/*
====================
=
= CalcBounds
=
====================
*/

void FF_CalcBounds(CK_object *ob)
{
	// KEEN WORLDS: the engine's is the same (sprite box, then the tile box)
	CK_ResetClipRects(ob);
}

/*
====================
=
= CK_CustomClipRects		(K1n9_Duk3 addition)
=
= Sets up a custom (hard-coded) hit rect for the object. This is for use with
= the regular ClipToWalls code, not for FullClipToWalls! Note that this hit rect
= is only used for clipping -- the hit detection will still use the hit rect of
= the object's current sprite.
=
====================
*/

void FF_CustomClipRects(CK_object *ob)
{
	switch (ob->type)
	{
	case FF_CT_SneakyTree:
		// Note: I would like to make the hitbox even more wide, but
		// that would cause problems on 45 degree slopes. A 32 pixel
		// wide hitbox seems to be the maximum width that is still
		// able to walk up on all slopes without getting stuck.
		// Most sprites in Keen 4-6 appear to have a hitbox width of
		// 32 pixels or less, including RoboRed. And those that do
		// have a bigger hitbox are either placed on flat ground or
		// don't walk/slide on the ground at all.
		// Maybe some tiles/attributes would need to be modified to
		// avoid this problem.
		ob->clipRects.unitX1 = ob->posX + 6*16;
		ob->clipRects.unitX2 = ob->posX + 38*16;
		ob->clipRects.unitY1 = ob->posY;
		ob->clipRects.unitY2 = ob->posY + 39*16;
		break;
	case FF_CT_Crab:
		ob->clipRects.unitX1 = ob->posX + CRAB_X1;
		ob->clipRects.unitX2 = ob->posX + CRAB_X2;
		ob->clipRects.unitY1 = ob->posY + CRAB_Y1;
		ob->clipRects.unitY2 = ob->posY + CRAB_Y2;
		break;
	case FF_CT_FallBlock:
		ob->clipRects.unitX1 = ob->posX + 8*16;
		ob->clipRects.unitX2 = ob->posX + 53*16;
		ob->clipRects.unitY1 = ob->posY + 16*16;
		ob->clipRects.unitY2 = ob->posY + 44*16;
		break;
	default:
		//Quit("CK_CustomClipRects: Bad obclass");
		Quit("Foray in the Forest: BADOBCLASS_CUSTOM");
	}

	FF_CalcTileClipRects(ob);
}

/*
====================
=
= CK_SetOldClipRects
=
====================
*/

void FF_SetOldClipRects(CK_object *ob)
{
	oldleft = ob->clipRects.unitX1;
	oldright = ob->clipRects.unitX2;
	oldtop = ob->clipRects.unitY1;
	oldbottom = ob->clipRects.unitY2;

	oldmidx = ob->clipRects.unitXmid;

	oldtileleft = ob->clipRects.tileX1;
	oldtileright = ob->clipRects.tileX2;
	oldtiletop = ob->clipRects.tileY1;
	oldtilebottom = ob->clipRects.tileY2;

	oldtilemidx = ob->clipRects.tileXmid;
}

/*
====================
=
= CK_SetDeltaClipRects
=
====================
*/

void FF_SetDeltaClipRects(CK_object *ob)
{
	leftmoved = ob->clipRects.unitX1 - oldleft;
	topmoved = ob->clipRects.unitY1 - oldtop;
	rightmoved = ob->clipRects.unitX2 - oldright;
	bottommoved = ob->clipRects.unitY2 - oldbottom;

	midxmoved = ob->clipRects.unitXmid - oldmidx;

	// We don't calculate tile deltas.
}


/*
====================
=
= MoveObjVert
=
====================
*/

void FF_MoveObjVert(CK_object *ob, int16_t ymove)
{
	ob->posY += ymove;
	ob->clipRects.unitY1 += ymove;
	ob->clipRects.unitY2 += ymove;

	ob->clipRects.tileY1 = RF_UnitToTile(ob->clipRects.unitY1);
	ob->clipRects.tileY2 = RF_UnitToTile(ob->clipRects.unitY2);
}

/*
====================
=
= MoveObjHoriz
=
====================
*/

void FF_MoveObjHoriz(CK_object *ob, int16_t xmove)
{
	ob->posX += xmove;
	ob->clipRects.unitX1 += xmove;
	ob->clipRects.unitX2 += xmove;
	ob->clipRects.unitXmid += xmove;	// GARG!

	// Now update the tile rect
	// WARNING: Keen _didn't_ update tilemidx!

	ob->clipRects.tileX1 = RF_UnitToTile(ob->clipRects.unitX1);
	ob->clipRects.tileX2 = RF_UnitToTile(ob->clipRects.unitX2);
	ob->clipRects.tileXmid = RF_UnitToTile(ob->clipRects.unitXmid);	// GARG!
}

/*
====================
=
= PlayerBottomKludge
=
====================
*/

void FF_PlayerBottomKludge(CK_object *ob)
{
	uint16_t  *map;
	uint16_t wall, clip, xpix;
	int16_t xmove, ymove;

	map = CA_TilePtrAtPos(0, ob->clipRects.tileY2-1, 1);
	if (ob->xDirection == 1)
	{
		xpix = 0;
		map += ob->clipRects.tileX2;
		xmove = ob->clipRects.unitX2 - ob->clipRects.unitXmid;
		if (TI_ForeLeft(*(map-CA_GetMapWidth())) || TI_ForeLeft(*map))
		{
			return;
		}
	}
	else
	{
		xpix = 15;
		map += ob->clipRects.tileX1;
		xmove = ob->clipRects.unitX1 - ob->clipRects.unitXmid;
		if (TI_ForeRight(*(map-CA_GetMapWidth())) || TI_ForeRight(*map))
		{
			return;
		}
	}
	
	if (!TI_ForeTop(*map))
	{
		map += CA_GetMapWidth();
		if ((wall = TI_ForeTop(*map)) == 1)
		{
			clip = wallclip[wall&SLOPEMASK][xpix];
			ymove = RF_TileToUnit(ob->clipRects.tileY2) + clip - 1 -ob->clipRects.unitY2;
			if (ymove <= 0 && ymove >= -bottommoved)
			{
				ob->topTI = wall;
				FF_MoveObjVert(ob, ymove);
				FF_MoveObjHoriz(ob, xmove);
			}
		}
	}
}

/*
====================
=
= PlayerTopKludge
=
====================
*/

void FF_PlayerTopKludge(CK_object *ob)
{
	uint16_t  *map;
	uint16_t xpix, wall, clip;
	int16_t move;

	map = CA_TilePtrAtPos(0, ob->clipRects.tileY1+1, 1);
	if (ob->xDirection == 1)
	{
		xpix = 0;
		map += ob->clipRects.tileX2;
		if (TI_ForeLeft(*(map+CA_GetMapWidth())) || TI_ForeLeft(*(map+2*CA_GetMapWidth())))
		{
			return;
		}
	}
	else
	{
		xpix = 15;
		map += ob->clipRects.tileX1;
		if (TI_ForeRight(*(map+CA_GetMapWidth())) || TI_ForeRight(*(map+2*CA_GetMapWidth())))
		{
			return;
		}
	}
	
	if (!TI_ForeBottom(*map))
	{
		map -= CA_GetMapWidth();
		if ((wall = TI_ForeBottom(*map)) != 0)
		{
			clip = wallclip[wall&SLOPEMASK][xpix];
			move = RF_TileToUnit(ob->clipRects.tileY1+1) - clip - ob->clipRects.unitY1;
			if (move >= 0 && move <= -topmoved)
			{
				ob->bottomTI = wall;
				FF_MoveObjVert(ob, move);
			}
		}
	}
}

/*
====================
=
= CheckPosition
=
= Checks if the position is okay, i.e., the object's tile hit rectangle doesn't
= contain any tiles that have blocking walls. Returns false when blocking tiles
= are found (position not okay), otherwise returns true (no blocking tiles,
= position okay)
=
====================
*/

bool FF_CheckPosition(CK_object *ob)
{
	uint16_t tile, x, y;
	uint16_t  *map;
	uint16_t rowdiff;
	
	map = CA_TilePtrAtPos(ob->clipRects.tileX1, ob->clipRects.tileY1, 1);
	rowdiff = CA_GetMapWidth()-(ob->clipRects.tileX2-ob->clipRects.tileX1+1);
	for (y=ob->clipRects.tileY1; y<=ob->clipRects.tileY2; y++,map+=rowdiff)
	{
		for (x=ob->clipRects.tileX1; x<=ob->clipRects.tileX2; x++)
		{
			tile = *(map++);
			if (TI_ForeTop(tile) || TI_ForeRight(tile) || TI_ForeBottom(tile) || TI_ForeLeft(tile))
			{
				return false;
			}
		}
	}
	return true;
}

#if 0
/*
====================
=
= SlopesNearby		(K1n9_Duk3 addition)
=
====================
*/

static bool   FF_SlopesNearby(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t walltype)
{
	uint16_t  *map;
	uint16_t rowdiff, x;
	
	// swap coordinates if necessary to make sure x1, y1 is the top left corner
	// and x2, y2 ist the bottom right corner of the area we want to check
	if (x1 > x2)
	{
		x = x2;
		x2 = x1;
		x1 = x;
	}
	if (y1 > y2)
	{
		x = y2;
		y2 = y1;
		y1 = x;
	}
	
	// check that area for tiles with sloped edges:
	map = CA_TilePtrAtPos(x1, y1, 1);
	rowdiff = CA_GetMapWidth() - (x2 - x1 + 1);
	
	for (; y1 <= y2; y1++, map += rowdiff)
	{
		for (x = x1; x <= x2; x++, map++)
		{
			if ((tinf[walltype + *map] & SLOPEMASK) > 1)
				return true;
		}
	}
	
	return false;
}
#endif

/*
====================
=
= ClipToEnds
=
====================
*/

void FF_ClipToEnds(CK_object *ob)
{


#if 0
	// K1n9_Duk3 note: This appears to be the Keen Dreams ClipToEnds() code,
	// modified to work with this code's naming scheme. I have not tested if
	// this works correctly.
	
	uint16_t	 *map,tile,facetile,info,wall;
	int16_t	leftpix,rightpix,midtiles,toppix,bottompix;
	int16_t	x,y,clip,move,totalmove,maxmove,midxpix;

	midxpix = (ob->clipRects.unitXmid&0xf0) >> G_P_SHIFT;

	maxmove = -abs(midxmoved) - bottommoved - SLOPEMAX;
	map = (uint16_t  *)mapsegs[1] +
		mapbwidthtable[oldtilebottom-1]/2 + ob->clipRects.tileXmid;
	for (y=oldtilebottom-1 ; y<=ob->clipRects.tileY2 ; y++,map+=CA_GetMapWidth())
	{
		if (wall = tinf[NORTHWALL+*map])
		{
			clip = wallclip[wall & SLOPEMASK][midxpix];
			move = ( (y<<G_T_SHIFT)+clip - 1) - ob->clipRects.unitY2;
			if (move<0 && move>=maxmove)
			{
				ob->topTI = wall;
				FF_MoveObjVert (ob,move);
				return;
			}
		}
	}

	maxmove = abs(midxmoved) - topmoved + SLOPEMAX;
	map = (uint16_t  *)mapsegs[1] +
		mapbwidthtable[oldtilebottom+1]/2 + ob->clipRects.tileXmid;
	for (y=oldtiletop+1 ; y>=ob->clipRects.tileY1 ; y--,map-=CA_GetMapWidth())
	{
		if (wall = tinf[SOUTHWALL+*map])
		{
			clip = wallclip[wall & SLOPEMASK][midxpix];
			move = ( ((y+1)<<G_T_SHIFT)-clip ) - ob->clipRects.unitY1;
			if (move > 0 && move<=maxmove)
			{
				totalmove = ob->deltaPosY + move;
				if (totalmove < 256 && totalmove > -256)
				{
					ob->bottomTI = wall;
					FF_MoveObjVert (ob,move);
				}
			}
		}
	}
#endif

	// K1n9_Duk3 note: This appears to be based on Omnispeak/NetKeen or some
	// other decompilation effort. I have modified the code below to make it
	// a little faster and/or smaller. Instead of storing the tile number in
	// a variable and looking up its top/bottom edge in the tile attributes
	// every time, I made the code look up the edge setting right away and
	// store it in a variable. I also made the ceiling check store the
	// result of 'clip - objYOffset' in variable dy and use then use dy
	// in the if-checks instead of re-calculating 'clip - objYOffset'
	// every single time.
	
	int16_t y, x, wall, clip, objYOffset, dy;

	int16_t midTileXOffset = (ob->clipRects.unitXmid >> G_P_SHIFT) & 0x0F;

	// Checking collisions with the floor
	// The maximum amount we can move upwards due to tile clipping (i.e., being pushed up by a 45 degree slope)
	// is equal to the horizontal displacement, so add this to the vertical displacement (+ 1 pixel)
#if 1
	// old code:
	int16_t maxmove = -abs(midxmoved) - bottommoved - SLOPEMAX;	// Move above slope first.
#else
	// FITF version (experimental):
	int16_t maxmove = -bottommoved;
	
	// K1n9_Duk3 note:
	// The additional adjustment (+/- SLOPEMAX) for the vertical displacement is
	// necessary for 45 degree slopes. If an object moves by 1 global unit along
	// the x axis, this can already move the center position to a different pixel
	// position and result in a floor/ceiling that is one pixel higher/lower than
	// the one at the old position, therefore the code must be allowed to move
	// the object by SLOPEMAX units more than the x-movement to prevent the
	// object from moving through the sloped floor/ceiling.
	//
	// The problem with the old code is that the additional vertical displacement
	// is ALWAYS allowed, even when there are no slope tiles nearby. This can
	// cause some jumps to be cut short prematurely (the object hits the floor
	// while it is still moving up). This problem was present in the original
	// Keens as well, but the limited frame rate made it less likely to pop up.
	//
	// In the end, this didn't really fix the jump problem, which is why I went
	// back to the old code again. The jumping problem was fixed in Keen's react
	// routines instead, by making sure Keen can't land while still moving up.
	
	if (FF_SlopesNearby(ob->clipRects.tileXmid, ob->clipRects.tileY2, oldtilemidx, oldtilebottom, NORTHWALL))
	{
		maxmove -= abs(midxmoved) + SLOPEMAX;
	}
#endif

	// Now check all tiles in a vertical strip over the new xMid tile, within the
	// sprite's Y boundaries for top blocking-ness, starting from top to bottom

	// For a sprite to collide with a sloped floor, it has to be moved underneath the floor first,
	for (y = oldtilebottom - 1; y <= ob->clipRects.tileY2; ++y)
	{
		wall = TI_ForeTop(FORESPOT(ob->clipRects.tileXmid, y));
		if (wall)
		{
			objYOffset = ob->clipRects.unitY2 - RF_TileToUnit(y);
			clip = wallclip[wall & SLOPEMASK][midTileXOffset];

			// dy = the amount to move the sprite vertically to set it one global unit
			// above the slope
			dy = (clip - objYOffset) - 1;
			if ((dy < 0) && (dy >= maxmove))
			{
				ob->topTI = wall;
				FF_MoveObjVert(ob, dy);
				return;
			}
		}
	}


	// Checking collisions with ceiling
#if 1
	// old code:
	maxmove = abs(midxmoved) - topmoved + SLOPEMAX;
#else
	// FITF version (experimental):
	maxmove = -topmoved;

	if (FF_SlopesNearby(ob->clipRects.tileXmid, ob->clipRects.tileY1, oldtilemidx, oldtiletop, SOUTHWALL))
	{
		maxmove += abs(midxmoved) + SLOPEMAX;
	}
#endif

	for (y = oldtiletop + 1; y >= (int16_t)ob->clipRects.tileY1; --y)	// K1n9_Duk3 fix: cast to signed int to avoid infinite loops when tiletop is 0
	{
		wall = TI_ForeBottom(FORESPOT(ob->clipRects.tileXmid, y));
		if (wall)
		{
			objYOffset = ob->clipRects.unitY1 - RF_TileToUnit(y + 1);
			clip = -wallclip[wall & SLOPEMASK][midTileXOffset];
			
			dy = clip - objYOffset;
			// Only move the object if movement doesn't exceed the maximum displacement
			// and doesn't cause the object to move by more than one tile in total
			if ((dy > 0) && (dy <= maxmove) && (ck_nextY + dy < 256) && (ck_nextY + dy > -256))
			{
				ob->bottomTI = wall;
				FF_MoveObjVert(ob, dy);
			}
		}
	}
}


/*
====================
=
= ClipToSides
=
====================
*/

void FF_ClipToSides(CK_object *ob)
{
	int16_t tiletop = ob->clipRects.tileY1;
	int16_t tilebottom = ob->clipRects.tileY2;
	int16_t y;

	// K1n9_Duk3 note: If the low byte of ob->clipRects.unitY2 is 0xFF, then the bottom end
	// of the hitbox is directly above the tile the object is standing on and
	// this code should NOT skip checking the bottom-most tile. Similarly for the
	// top edge, if the low byte of ob->clipRects.unitY1 is 0, then the object is directly
	// below the ceiling and this code should NOT skip checking the top-most tile.
	// Checking the low byte is necessary for Keen at tics >= 5.
	
	// ignore blocking sides at the bottom when bottom of hitbox is touching a slope
	if ((ob->topTI & SLOPEMASK) > 1 && ((uint8_t)ob->clipRects.unitY2) != 0xFF || (ob == ck_keenObj && ff_map.quicksand))	//If we're on a slope (or Keen is walking in quicksand)
	{
		tilebottom--;	// don't check bottom-most tile
	}
	// ignore blocking sides at the top when top of hitbox is touching a slope
	if ((ob->bottomTI & SLOPEMASK) > 1 && ((uint8_t)ob->clipRects.unitY1) != 0)
	{
		tiletop++;	// don't check top-most tile
	}

#if 0
	// original code (always checks tileX1 first):
	
	// Check if our left side is intersecting with a wall.
	for (y = tiletop; y <= tilebottom; ++y)
	{
		// K1n9_Duk3 fix: only overwrite hiteast when touching a blocking tile
		if ((_AX = TI_ForeRight(FORESPOT(ob->clipRects.tileX1, y))) != 0)
		{
			ob->rightTI = _AX;
			// Clipping right
			// Push us right until we're no-longer intersecting.
			FF_MoveObjHoriz(ob, RF_TileToUnit(ob->clipRects.tileX1 + 1) - ob->clipRects.unitX1);
			return;
		}
	}

	// Similarly for the right side (left side of intersecting tile).
	for (y = tiletop; y <= tilebottom; ++y)
	{
		// K1n9_Duk3 fix: only overwrite hitwest when touching a blocking tile
		if ((_AX = TI_ForeLeft(FORESPOT(ob->clipRects.tileX2, y))) != 0)
		{
			ob->leftTI = _AX;
			// Push us left until we're no-longer intersecting.
			FF_MoveObjHoriz(ob, RF_TileToUnit(ob->clipRects.tileX2) - 1 - ob->clipRects.unitX2);
			return;

		}
	}
#else
	// FITF version:
	{
		uint8_t right, left;
		
		// Check if the left or right end of the object's hitbox is inside a tile
		// with a solid (blocking) edge:
		for (right = left = 0; tiletop <= tilebottom; ++tiletop)
		{
			// We could just bitwise-or the new edge values into the variables,
			// but a) this might cause problems with drillable walls and b) it's
			// probably faster to check the variable and skip reading the tile and
			// its attribute when we already found a blocking wall on that side.
			if (right == 0)
				right = TI_ForeRight(FORESPOT(ob->clipRects.tileX1, tiletop));
			if (left == 0)
				left = TI_ForeLeft(FORESPOT(ob->clipRects.tileX2, tiletop));
		}
		
		// We'll check the walls in different order depending on which direction
		// the object has moved. The assumption is that preventing the object from
		// moving into blocking tiles is more important than pushing the object
		// out of any blocking tiles it is trying to leave.
		
		// If the object has not moved to the right, check the left end of the
		// hitbox first, like the old code did:
		if (midxmoved <= 0 && right)
		{
hit_right:
			// The left end of the object's hitbox is inside a tile with a solid
			// right edge, so move the object out of that tile (push to the right):
			ob->rightTI = right;
			FF_MoveObjHoriz(ob, RF_TileToUnit(ob->clipRects.tileX1 + 1) - ob->clipRects.unitX1);
			return;
		}
		else if (left)
		{
			// The right end of the object's hitbox is inside a tile with a solid
			// left edge, so move the object out of that tile (push to the left):
			ob->leftTI = left;
			FF_MoveObjHoriz(ob, RF_TileToUnit(ob->clipRects.tileX2) - 1 - ob->clipRects.unitX2);
			return;
		}
		else if (right)
		{
			goto hit_right;
		}
	}
#endif
}

/*
====================
=
= ClipToWalls
=
= Moves the current object xtry/ytry units, clipping to walls
=
====================
*/

void FF_ClipToWalls(CK_object *ob)
{
	int16_t oldUnitX, oldUnitY;
	int16_t wasPushed = 0;
	oldUnitX = ob->posX;
	oldUnitY = ob->posY;
	
	switch (ob->currentAction->stickToGround)
	{
	case 1:
		// If the object is resting on a platform
		if (ob->topTI == PLATFORMEDGE)
		{
			//ytry = 145;	// K1n9_Duk3 mod: we don't need this
			
			// Note: This code is not really required for KEEN 4-6. If you go back
			// to the KEEN DREAMS source, you will see that it has no eqivalent to
			// the HandleRiding() routine. Instead, the ClipToSprite() code was
			// responsible for moving Keen sideways when he was standing on a
			// moving Canteloupe Cart in that game. That would make it necessary to
			// push Keen down into the other object if that object was moving up or
			// down, otherwise the objects would no longer touch each other and the
			// ClipToSprite() code would not be executed.
			//
			// Setting ytry to a relatively high value in here was probably
			// just an intermediate step in development, before the new routine
			// HandleRiding() was implemented. But it might also have been
			// intended to allow other objects besides the player character to
			// stand on top of another game object.
		}
		else
		{
			// Push the object down by a little more than the amount that it moves
			// along the x-axis to make sure it sticks to the ground on 45° slopes
			// and doesn't end up floating in the air.
			if (ck_nextX > 0)
			{
				ck_nextY = (ck_nextX) + SLOPEMAX;
			}
			else
			{
				ck_nextY = -(ck_nextX) + SLOPEMAX;
			}
			wasPushed = 1;
		}
		break;
	case push_up:
		// Push the object up by a little more than the amount that it moves along
		// the x-axis to make sure it sticks to the ceiling on 45° slopes and does
		// not end up floating in the air.
		if (ck_nextX > 0)
		{
			ck_nextY = -(ck_nextX) - SLOPEMAX;
		}
		else
		{
			ck_nextY = (ck_nextX) - SLOPEMAX;
		}
		wasPushed = push_up;
		break;
	case push_side:
		if (ob->xDirection > 0)
		{
			ck_nextX = IN_motion_Right * SLOPEMAX;
			wasPushed = push_right;
		}
		else
		{
			ck_nextX = IN_motion_Left * SLOPEMAX;
			wasPushed = push_left;
		}
		break;
	case push_left:
		ck_nextX = IN_motion_Left * SLOPEMAX;
		wasPushed = push_left;
		break;
	case push_right:
		ck_nextX = IN_motion_Right * SLOPEMAX;
		wasPushed = push_right;
		break;
	}

//
// move the shape
//
	if (ck_nextX > MAXMOVE)
	{
		ck_nextX = MAXMOVE;
	}
	else if (ck_nextX < -MAXMOVE)
	{
		ck_nextX = -MAXMOVE;
	}

	if (ck_nextY > MAXMOVE+SLOPEMAX)	// +SLOPEMAX because we also add SLOPEMAX for pushtofloor
	{
		ck_nextY = MAXMOVE+SLOPEMAX;	// +SLOPEMAX because we also add SLOPEMAX for pushtofloor
	}
	else if (ck_nextY < -(MAXMOVE+SLOPEMAX))	// +SLOPEMAX for sticking to ceiling
	{
		ck_nextY = -(MAXMOVE+SLOPEMAX);	// +SLOPEMAX for sticking to ceiling
	}

	if (ob->clipped == FF_CLIP_custommid)	// K1n9_Duk3 addition
		FF_CustomClipRects(ob);

	ob->posX += ck_nextX;
	ob->posY += ck_nextY;


	ob->visible = true;

	if (ob->gfxChunk)				// can't get a hit rect with no shape!
	{
		FF_SetOldClipRects(ob);

retry:
		if (ob->clipped == FF_CLIP_custommid)	// K1n9_Duk3 addition
			FF_CustomClipRects(ob);
		else
			FF_CalcBounds(ob);

		// Reset the tile clipping vars.
		ob->topTI = ob->bottomTI = ob->leftTI = ob->rightTI = 0;

		if (ob->clipped)
		{
			uint16_t oldXmid;	// K1n9_Duk3 addition
			
			// NOTE: Keen calculates a clip-rect delta here.

			FF_SetDeltaClipRects(ob);

	//
	// clip it
	//
			FF_ClipToEnds(ob);

			// Set Keen on top of a flat tile in case he's just about to miss
			// landing on it. But only if Keen isn't getting pushed around, so
			// Keen can actually get pushed off egdes reliably.
			if (ob == ck_keenObj && !ff_map.playerkludgeclipcancel)
			{
				if (!ob->topTI && bottommoved > 0)
				{
					FF_PlayerBottomKludge(ob);
				}
				if (!ob->bottomTI && topmoved < 0)
				{
					FF_PlayerTopKludge(ob);
				}
			}
			
			oldXmid = ob->clipRects.unitXmid;	// K1n9_Duk3 addition
			
			FF_ClipToSides(ob);
			
			// K1n9_Duk3 addition: Compare current and old Xmid position. If the
			// object was moved sideways, redo vertical clipping to make sure the
			// hitnorth and hitsouth values are set correctly and the object doesn't
			// end up thinking it hit a floor (or ceiling) when that's no longer
			// the case at the corrected x position. This is important if the tile
			// next to the wall doesn't have a flat surface.
			//
			// KEEN 6 is the only one of the original games that has tiles with
			// sloped top surfaces right next to walls in some of its levels (6, 7
			// and 8) and it had extra code that attempted to fix this problem.
			// I found the KEEN 6 solution insufficient for the FITF levels, so I
			// came up with this:
			if (oldXmid != ob->clipRects.unitXmid)
			{
				ob->topTI = ob->bottomTI = 0;
				// back to y position this object had before clipping
				FF_MoveObjVert(ob, oldUnitY + ck_nextY - ob->posY);
				// and clip to floor/ceiling
				FF_SetDeltaClipRects(ob);
				FF_ClipToEnds(ob);
			}
			// end of addition			
			
			if (ob->clipped == FF_CLIP_custommid)	// K1n9_Duk3 addition
				FF_CalcBounds(ob);

		}

		// Check if the object is no longer touching the surface it was supposed
		// to stick to:

		if ((!ob->topTI && wasPushed==1) || (!ob->bottomTI && wasPushed==push_up)
			|| (!ob->leftTI && wasPushed==push_right) || (!ob->rightTI && wasPushed==push_left))
		{
			// The object was pushed down to make it stick to the ground on 45° slopes
			// but now it's not touching the ground anymore. That means we should undo
			// the pushing down part (keep oldUnitY and DON'T add ytry)!
#if 1
			// FITF code:
			if (wasPushed <= push_up)
			{
				// object was pushed up or down, so keep x movement and undo y movement
				ob->posX = oldUnitX + ck_nextX;
				ob->posY = oldUnitY;
			}
			else
			{
				// object was pushed left or right, so keep y movement and undo x movement
				ob->posX = oldUnitX;
				ob->posY = oldUnitY + ck_nextY;
			}
			
			// K1n9_Duk3 note: I am going to make sure the movement is clipped.
			// Most actors that use pushtofloor will undo the movement in their
			// react code when they are no longer touching the floor/ceiling/wall,
			// so it wouldn't matter if the movement was clipped or not, but this
			// is not the case for Keen himself.
			
			wasPushed = 0;
			goto retry;	// make sure the movement is clipped!
			//CalcBounds(ob);
#else
			// CKSOURCEMOD code:
			ob->posX = oldUnitX + ck_nextX;
			ob->posY = oldUnitY + ck_nextY; // Note: Vanilla Keen does not have the +ytry,
							 // but I've added it, as it stops walking sprites from
							 // floating into the air when they should be
							 // moving down a slope

			// K1n9_Duk3 says: If objects end up floating in the air when they should be
			// walking down a slope, then there is an error in the way the sprite's
			// hitboxes are designed and that is what causes the object to float.
			// Use the Sprite Test debug command and look at the sprites in question.
			// The value for the bottom of the hitbox (yh) must be the same for all
			// frames of the walking animation. The middle of the hitbox (xl+(xh-xl)/2)
			// also has to have the same value for all the walking sprites. Another
			// possible cause could be that you forgot to set the state's pushtofloor
			// value to push_down (or to true in Keen 4-6 and Dreams).
			//
			// The "fix" above means that floating ground-based objects will be pushed
			// down a little bit every frame until they eventually touch a tile that
			// they can stand on or they fall out of the level. It does NOT fix the
			// problem, since the object will still be floating for at least one frame,
			// which will cause it to turn around or do whatever else it was programmed
			// to do when it no longer walks on solid ground. This also means that some
			// objects won't be able to turn around correctly when they reach an edge,
			// because the code that lets them turn around only resets the x position
			// and not the y position, causing the object to be partially stuck inside
			// a tile.
			//
			// And to top it all off, this code adds movement without clipping it.
			//
			// That's why I don't use this version of the code in Foray in the Forest.
			
			FF_CalcBounds(ob);
#endif
		}

		ob->deltaPosX += ob->posX - oldUnitX;
		ob->deltaPosY += ob->posY - oldUnitY;
	}
}

#if 0
/*
====================
=
= FullClipToWalls (not needed in FITF)
=
= Moves the current object xtry/ytry units, clipping to walls
=
= Full clipping means there is a hard-coded hit rectangle around the object and
= NO part of that hit rectangle is allowed to touch a tile that has ANY blocking
= walls. Slope tiles and one-way-blocking tiles will be considered blocking.
= The regular mid clipping allows the corners of the hit rectangle to be
= partially inside a wall, since only the midx point can actually touch the
= floor or the ceiling. It also means that objects will usually "slide off" the
= edge of a floor if only the leftmost or the rightmost part of the hit rect
= hits the floor. Full clipping objects will not slide off, but they might land
= above the floor if there are slope tiles in the way. Note that if a full
= clipping object already is at a position where it touches a tile with blocking
= walls, because it was spawned there or a blocking tile appeared because of a
= tile animation or when a bridge was activated, the object will most likely be
= stuck at that position (like when Eggbirds start to fly on/near slopes in
= Keen 4).
=
====================
*/

void FF_FullClipToWalls(CK_object *ob)
{
	int16_t delX, delY;

	int16_t oldUnitX = ob->posX;
	int16_t oldUnitY = ob->posY;

//
// move the shape
//
	if (ck_nextX > MAXMOVE)
		ck_nextX = MAXMOVE;
	else if (ck_nextX < -MAXMOVE)
		ck_nextX = -MAXMOVE;
	if (ck_nextY > MAXMOVE)
		ck_nextY = MAXMOVE;
	else if (ck_nextY < -MAXMOVE)
		ck_nextY = -MAXMOVE;

	ob->posX += ck_nextX;
	ob->posY += ck_nextY;
	ob->visible = true;

/*
	if ((ob->type != CT_SliceStar) && (ob->type != CT_Sphereful))
	{
		Quit("FullClipToWalls: Bad obclass");
	}
*/

	switch (ob->type)
	{
/*
	// Just in case anybody wants to use full clipping with the log objects in FITF:
	case CT_LogPlatform:
	case CT_OccupiedLog:
		delX = 38*PIXGLOBAL-1;
		delY = 19*PIXGLOBAL-1;
		break;
*/
	default:
		delX = 512;
		delY = 512;
		break;
	}

	ob->clipRects.unitX2 = ob->posX + delX;
	ob->clipRects.unitX1 = ob->posX;
	ob->clipRects.unitY1 = ob->posY;
	ob->clipRects.unitY2 = ob->posY + delY;
	
	ob->clipRects.tileX1 = RF_UnitToTile(ob->clipRects.unitX1);
	ob->clipRects.tileX2 = RF_UnitToTile(ob->clipRects.unitX2);
	ob->clipRects.tileY1 = RF_UnitToTile(ob->clipRects.unitY1);
	ob->clipRects.tileY2 = RF_UnitToTile(ob->clipRects.unitY2);
	
	//Reset the tile clipping vars.
	ob->topTI = ob->bottomTI = ob->leftTI = ob->rightTI = 0;

//
// clip it
//
	if (!FF_CheckPosition(ob))
	{
		FF_MoveObjHoriz(ob, -ck_nextX);
		if (FF_CheckPosition(ob))
		{
			if (ck_nextX > 0)
				ob->leftTI = 1;
			else
				ob->rightTI = 1;
		}
		else
		{
			if (ck_nextY > 0)
				ob->topTI = 1;
			else
				ob->bottomTI = 1;

			FF_MoveObjHoriz(ob, ck_nextX);
			FF_MoveObjVert(ob, -ck_nextY);
			if (!FF_CheckPosition(ob))
			{
				FF_MoveObjHoriz(ob, -ck_nextX);
				if (ck_nextX > 0)
					ob->leftTI = 1;
				else
					ob->rightTI = 1;
			}
		}
	}
	ob->deltaPosX += (ob->posX - oldUnitX);
	ob->deltaPosY += (ob->posY - oldUnitY);

	FF_CalcBounds(ob);
}
#endif

/*
====================
=
= PushObj
=
= Moves the current object xtry/ytry units, clipping to walls
= Used by ClipToSprite... routines and HandleRiding, doesn't limit movement and
= doesn't push to floor
=
====================
*/

void FF_PushObj(CK_object *ob)
{
	int16_t oldUnitX, oldUnitY;
	oldUnitX = ob->posX;
	oldUnitY = ob->posY;


//
// move the shape
//
	ob->posX += ck_nextX;
	ob->posY += ck_nextY;


	ob->visible = true;

	if (ob->gfxChunk)				// can't get a hit rect with no shape!
	{

		FF_SetOldClipRects(ob);

		FF_CalcBounds(ob);
	/*
		// Reset the tile clipping vars.
		ob->topTI = ob->bottomTI = ob->leftTI = ob->rightTI = 0;
	*/
		if (ob->clipped)
		{
			FF_SetDeltaClipRects(ob);
			FF_ClipToEnds(ob);
			FF_ClipToSides(ob);
		}

		ob->deltaPosX += ob->posX - oldUnitX;
		ob->deltaPosY += ob->posY - oldUnitY;
	}

}

/*
====================
=
= ClipToSpriteSide
=
====================
*/

void FF_ClipToSpriteSide(CK_object *push, CK_object *solid)
{
	int16_t xmove, leftinto, rightinto;
	
	//
	// amount the push shape can be pushed
	//
	xmove = solid->deltaPosX - push->deltaPosX;
	
	//
	// amount it is inside
	//
	leftinto = solid->clipRects.unitX2 - push->clipRects.unitX1;
	rightinto = push->clipRects.unitX2 - solid->clipRects.unitX1;

	ck_nextY = 0;	// K1n9_Duk3 fix: don't push up or down unless we need to pushtofloor!

	// Push object to the right
	if ((leftinto > 0) && (leftinto <= xmove))
	{
		ck_nextX = leftinto;
		if (push->currentAction->stickToGround)
			ck_nextY = leftinto + SLOPEMAX;	// K1n9_Duk3 fix: it's leftinto, not -leftinto
		FF_ClipToWalls(push);
		push->rightTI = 1;
		return;
	}

	// Push object to the left
	if ((rightinto > 0) && (-xmove >= rightinto))
	{
		ck_nextX = -rightinto;
		if (push->currentAction->stickToGround)
			ck_nextY = rightinto + SLOPEMAX;
		FF_ClipToWalls(push);
		push->leftTI = 1;
		return;
	}
}

/*
====================
=
= ClipToSpriteTop
=
====================
*/

void FF_ClipToSpriteTop(CK_object *push, CK_object *solid)
{
	int16_t oldpushtofloor;
	int16_t deltaDeltaY = push->deltaPosY - solid->deltaPosY;
	int16_t deltaClipY = push->clipRects.unitY2 - solid->clipRects.unitY1;

	ck_nextX = 0;	// K1n9_Duk3 fix: don't push left/right!

	if ((deltaClipY >= 0) && (deltaClipY <= deltaDeltaY))
	{
		// When pushing the player, set ck_mapState.riding to solid
		// (I'm not sure I like this)
		if (push == ck_keenObj)
			ck_keenState.platform = solid;

		ck_nextY = -deltaClipY - 16;	// K1n9_Duk3 mod: - 16 for consistency with HandleRiding()
		oldpushtofloor = push->currentAction->stickToGround;
		push->currentAction->stickToGround = 0;
		FF_ClipToWalls(push);
		push->currentAction->stickToGround = oldpushtofloor;

		if (!push->bottomTI)
			push->topTI = PLATFORMEDGE;
	}
}

/*
====================
=
= ClipToSprite
=
====================
*/

void FF_ClipToSprite(CK_object *push, CK_object *solid, bool squish)
{
	//
	// slightly modified Keen 4-6 code:
	//
	int16_t xmove, ymove, leftinto, rightinto, topinto, bottominto;
	CK_object temp;	// K1n9_Duk3 addition

	xmove = solid->deltaPosX - push->deltaPosX;
	ck_nextX = ck_nextY = 0;
	leftinto = solid->clipRects.unitX2 - push->clipRects.unitX1;
	rightinto = push->clipRects.unitX2 - solid->clipRects.unitX1;
	if (leftinto > 0 && xmove+1 >= leftinto)
	{
_left:
		ck_nextX = leftinto;
		push->leftTI = push->velX = 0;	// K1n9_Duk3 mod: also clear hitwest
		temp = *push;	// K1n9_Duk3 addition: backup old values
		FF_PushObj(push);
		if (squish && push->leftTI)
		{
			// K1n9_Duk3 hack: undo Y movement and push again:
			*push = temp;
			ck_nextY = -push->deltaPosY;
			FF_PushObj(push);
			if (push->leftTI)
			{
				FF_KillKeenEx(true);	// always kill, ignore invincibility
			}
			else
			{
				// redo Y movement only
				ck_nextX = 0;
				ck_nextY = -ck_nextY;
				FF_PushObj(push);
			}
		}
		push->rightTI = 1;
		return;
	}
	else if (rightinto > 0 && -xmove+1 >= rightinto)
	{
_right:
		ck_nextX = -rightinto;
		push->rightTI = push->velX = 0;	// K1n9_Duk3 mod: also clear hiteast
		temp = *push;	// K1n9_Duk3 addition: backup old values
		FF_PushObj(push);
		if (squish && push->rightTI)
		{
			// K1n9_Duk3 hack: undo Y movement and push again:
			*push = temp;
			ck_nextY = -push->deltaPosY;
			FF_PushObj(push);
			if (push->rightTI)
			{
				FF_KillKeenEx(true);	// always kill, ignore invincibility
			}
			else
			{
				// redo Y movement only
				ck_nextX = 0;
				ck_nextY = -ck_nextY;
				FF_PushObj(push);
			}
		}
		push->leftTI = 1;
		return;
	}
	ymove = push->deltaPosY - solid->deltaPosY;
	topinto = solid->clipRects.unitY2 - push->clipRects.unitY1;
	bottominto = push->clipRects.unitY2 - solid->clipRects.unitY1;
	if (bottominto >= 0 && bottominto <= ymove)
	{
_bottom:
		if (push == ck_keenObj)
		{
			ck_keenState.platform = solid;
		}
		ck_nextY = -bottominto - 16;	// K1n9_Duk3 mod: - 16 for consistency with HandleRiding()
		FF_PushObj(push);
		if (squish && push->bottomTI)
		{
			FF_KillKeenEx(true);	// always kill, ignore invincibility
		}
		if (!push->bottomTI)
		{
			push->topTI = PLATFORMEDGE;
		}
		return;
	}
	else if (topinto >= 0 && topinto <= -ymove)	// Keen 4-6 used '<= ymove' here, which didn't work
	{
_top:
		ck_nextY = topinto;
		FF_PushObj(push);
		if (squish && push->topTI)
		{
			FF_KillKeenEx(true);	// always kill, ignore invincibility
		}
		push->bottomTI = PLATFORMEDGE;
		return;
	}

	//
	// K1n9_Duk3 addition:
	// always pushes push out of solid, taking the shortest possible path
	//
#define mininto xmove
#define dir ymove
	mininto = 0x7FFF;
	dir = 4;
	if (leftinto > 0 && leftinto < mininto)
	{
		mininto = leftinto;
		dir = 0;
	}
	if (rightinto > 0 && rightinto < mininto)
	{
		mininto = rightinto;
		dir = 1;
	}
	if (bottominto > 0 && bottominto < mininto)
	{
		mininto = bottominto;
		dir = 2;
	}
	if (topinto > 0 && topinto < mininto)
	{
		mininto = topinto;
		dir = 3;
	}

	switch (dir)
	{
	case 0:
		goto _left;
	case 1:
		goto _right;
	case 2:
		goto _bottom;
	case 3:
		goto _top;
	}
#undef mininto
#undef dir
}

//===========================================================================

/*
==================
=
= DoActor
=
= Moves an actor in its current state by a given number of tics.
= If that time takes it into the next state, it changes the state
= and returns the number of excess tics after the state change
=
==================
*/

int16_t FF_DoActor(CK_object *ob, int16_t time)
{
	int16_t newTime, remainingTime;
	CK_action *state = ob->currentAction;

	// ThinkMethod: 2
	if (state->type == AT_Frame)
	{
		if (state->think)
		{
			if (ob->timeUntillThink)
			{
				ob->timeUntillThink--;
			}
			else
			{
				state->think(ob);
			}
		}
		return 0;
	}

	newTime = time + ob->actionTimer;

	// If we won't run out of time.
	if (state->timer > newTime || !(state->timer))
	{
		ob->actionTimer = newTime;

		if (state->type == AT_ScaledOnce || state->type == AT_ScaledFrame)
		{
			if (ob->xDirection)
			{
				ck_nextX += state->velX * (time * ob->xDirection);
			}
			if (ob->yDirection)
			{
				ck_nextY += state->velY * (time * ob->yDirection);
			}
		}

		if (state->type == AT_UnscaledFrame || state->type == AT_ScaledFrame)
		{
			if (state->think)
			{
				if (ob->timeUntillThink)
					ob->timeUntillThink--;
				else
				{
					state->think(ob);
				}
			}
		}
		return 0;
	}

	remainingTime = state->timer - ob->actionTimer;
	newTime -= state->timer;
	ob->actionTimer = 0;

	if (state->type == AT_ScaledOnce || state->type == AT_ScaledFrame)
	{
		if (ob->xDirection)
		{
			ck_nextX += state->velX * (remainingTime * ob->xDirection);
		}
		if (ob->yDirection)
		{
			ck_nextY += state->velY * (remainingTime * ob->yDirection);
		}
	}
	else // state->type must be either step or stepthink here
	{
		if (ob->xDirection)
		{
			ck_nextX += state->velX * ob->xDirection;
		}
		if (ob->yDirection)
		{
			ck_nextY += state->velY * ob->yDirection;
		}
	}

	if (state->think)
	{
		if (ob->timeUntillThink)
			ob->timeUntillThink--;
		else
		{
			state->think(ob);
		}
	}

	if (state != ob->currentAction)
	{
		if (!ob->currentAction)
		{
			return 0;
		}
	}
	else
	{
		ob->currentAction = state->next;
	}
	return newTime;
}

//==========================================================================


/*
====================
=
= StateMachine
=
= Change state and give directions
=
====================
*/

void FF_StateMachine(CK_object *ob)
{
	int16_t excesstics, oldshapenum;
	CK_action *state;

	ob->deltaPosX = ob->deltaPosY = ck_nextX = ck_nextY = 0;
	oldshapenum = ob->gfxChunk;
	
	// K1n9_Duk3 addition: always clear adjustment indicator
	ff_keenMustAdjustNextX = false;	// will be set by Keen's think routine

	state = ob->currentAction;

	excesstics = FF_DoActor(ob, SD_GetSpriteSync());

	if (ob->currentAction != state)
	{
		ob->actionTimer = 0;		// start the new state at 0, then use excess
		state = ob->currentAction;
	}
	
	// NOTE: Vanilla code doesn't check if state is non-NULL,
	// but the new code may crash as a consequence (try no-clip cheat).
	while (state && excesstics)
	{
	//
	// passed through to next state
	//
		if (state->protectAnimation || state->timer > excesstics)
		{
			excesstics = FF_DoActor(ob, excesstics);
		}
		else
		{
			excesstics = FF_DoActor(ob, state->timer - 1);
		}

		if (ob->currentAction != state)
		{
			ob->actionTimer = 0;;		// start the new state at 0, then use excess
			state = ob->currentAction;
		}
	}

	if (!state)			// object removed itself
	{
		CK_RemoveObj(ob);
		return;
	}
	
	//
	// if state->chunkRight == NULL, the state does not have a standard
	// shape (the think routine should have set it)
	//
	if (state->chunkRight)
	{
		ob->gfxChunk = (ob->xDirection > 0) ? state->chunkRight : state->chunkLeft;
	}
	if (ob->gfxChunk == -1)
	{
		ob->gfxChunk = 0;		// make it invisable this time
	}
	
	if (ob->gfxChunk != oldshapenum || ck_nextX || ck_nextY || ob->topTI == PLATFORMEDGE
		|| ob->currentAction->stickToGround)	// K1n9_Duk3 addition: added pushtofloor check
	{
		// K1n9_Duk3 mod:
		// We need to adjust Keen's walking speed in here instead of doing it in
		// KeenWalkThink() because KeenWalkThink() might be executed
		// twice during the same frame, and in that case the adjustment would be
		// applied twice instead of just once.
		if (ff_keenMustAdjustNextX)
		{
			extern int16_t slopespeed[];
			
			// apply slope speed adjustment:
			ck_nextX += slopespeed[ob->topTI & SLOPEMASK] * SD_GetSpriteSync();
			
			// HACK: cut speed in half if Keen is being slowed down:
			if (ff_map.nisasi || ff_map.inwater)
				ck_nextX >>= 1;
		}
		// end of mod
		
		// Note: Full clipping is never used in FITF, so let's simplify things:
		/*if (ob->clipped == cl_fullclip)
			FullClipToWalls(ob);
		else*/
			FF_ClipToWalls(ob);
	}
}

//==========================================================================


/*
====================
=
= NewState
=
====================
*/

void FF_NewState(CK_object *ob, CK_action *state)
{
	CK_ClipType oldClipping;

	ob->currentAction = state;

	if (state->chunkRight && ob->xDirection > 0)
		ob->gfxChunk = state->chunkRight;
	else if (state->chunkLeft)
		ob->gfxChunk = state->chunkLeft;

	if (ob->gfxChunk == -1)
		ob->gfxChunk = 0;

	ck_nextX = ck_nextY = 0;

#if 0
	// old code:
	
	// K1n9_Duk3 note: The original KEEN 4-6 code didn't have CalcBounds()
	// as a separate routine, so it temporarilily disabled clipping and ran the
	// normal clipping routine to initialize the object's hitbox to the correct
	// values for its current sprite.
	oldClipping = ob->clipped;
	ob->clipped = CLIP_not;
	FF_ClipToWalls(ob);
	ob->clipped = oldClipping;

	if (ob->clipped == CLIP_simple)	// K1n9_Duk3 fix: this was a fullclip check in the original code
		FF_FullClipToWalls(ob);
	else if (ob->clipped != CLIP_not)	// K1n9_Duk3 mod: add support for custom clipping
		FF_ClipToWalls(ob);
#else
	// K1n9_Duk3's new version for FITF:

	// first, initialize the hitbox values:
	if (ob->gfxChunk)
	{
		/*
		if (ob->clipped == cl_fullclip)
		{
			// Running the full clipping code is pretty useless here. Since the
			// object hasn't moved, the routine will not change the object's
			// position. Apart from setting the hitbox to the correct values for
			// the current position and sprite number at the end, all this does is
			// set hitnorth, hitsouth, hitwest and hiteast to 0. Or, if there are any
			// blocking tiles touching the full clipping hitbox, hitsouth and
			// hiteast will be set to 1.
			FullClipToWalls(ob);
		}
		else*/ if (ob->clipped == CLIP_not)
		{
			// Just initialitze the hitbox for the current sprite:
			FF_CalcBounds(ob);
		}
		else
		{
			// Get the initial hitbox values for the current sprite:
			if (ob->clipped == FF_CLIP_custommid)
				FF_CustomClipRects(ob);
			else
				FF_CalcBounds(ob);
			
			// And save it as the "old" hitbox:
			FF_SetOldClipRects(ob);
			
			// Now shrink the "old" hitbox:
			oldtop = oldbottom = (oldbottom-oldtop)/2 + oldtop;
			oldtiletop = oldtilebottom = RF_UnitToTile(oldtop);
			// The shrunken hitbox allows the clipping code to push the object out
			// of the floor or the ceiling.
			
			FF_SetDeltaClipRects(ob);
			
			FF_ClipToEnds(ob);
			FF_ClipToSides(ob);
			
			// If the objects needs to stick to a surface, run the clipping routine
			// to push the object against that surface and to set hitnorth, hitsouth,
			// hitwest, hiteast. Otherwise, if the object uses a custom hitbox, set
			// the hitbox back to the correct values for its current sprite.
			if (ob->currentAction->stickToGround)
				FF_ClipToWalls(ob);
			else if (ob->clipped == FF_CLIP_custommid)
				FF_CalcBounds(ob);		
		}
	}
	ob->visible = true;
#endif
}

/*
====================
=
= ChangeState
=
====================
*/

void FF_ChangeState(CK_object *ob, CK_action *state)
{
	ob->currentAction = state;
	ob->actionTimer = 0;

	if (state->chunkRight)
	{
		ob->gfxChunk = (ob->xDirection > 0) ? state->chunkRight : state->chunkLeft;
	}
#if 0
	if (state->chunkRight && ob->xDirection > 0) ob->gfxChunk = state->chunkRight;
	else if (state->chunkLeft) ob->gfxChunk = state->chunkLeft;
#endif
	if (ob->gfxChunk == -1) ob->gfxChunk = 0;

	ob->visible = true;			// it will need to be redrawn this frame
	ck_nextX = ck_nextY = 0;					// no movement


	if (ob->topTI != PLATFORMEDGE)
		FF_ClipToWalls(ob);

}
