/*
Keen Worlds: Foray in the Forest's camera, converted from CK_PLAY.C of its
source (CenterActor, WorldScrollScreen, ScrollScreen, CK_WrapScroll; Nisaba
and K1n9_Duk3; GPL v2 or later) by tools/kw_fitf_port.py, with the hand fixes
of tools/kw_fitf_cam_fixes.py. The engine's camera functions hand over to
these in this world.
*/

#include "kw_fitf.h"
#include "kw_ff_keen.h"

#define DEFAULT_ACTIVE_LIMIT 6

// Where objects are kept active around the screen (its CalcInactivate; the
// engine does this inline after it scrolls).
static void FF_CalcInactivate(void)
{
	ck_activeX0Tile = CK_Cross_max(RF_UnitToTile(rf_scrollXUnit) - CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT), 0);
	ck_activeX1Tile = RF_UnitToTile(rf_scrollXUnit) + RF_PixelToTile(320) + CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT);
	ck_activeY0Tile = CK_Cross_max(RF_UnitToTile(rf_scrollYUnit) - CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT), 0);
	ck_activeY1Tile = RF_UnitToTile(rf_scrollYUnit) + RF_PixelToTile(208) + CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT);
}


/*
==================
=
= CenterActor
=
==================
*/

void FF_CenterActor(CK_object *ob)
{
	uint16_t screenX, screenY;

	ff_map.screenYpx = 140;

	if (ob->posX < 152*16)
		screenX = 0;
	else
		screenX = ob->posX - 152*16;

	if (ck_gameState.currentLevel == 0)
	{
		// World Map
		if (ob->posY < 80*16)
			screenY = 0;
		else
			screenY = ob->posY - 80*16;
	}
	else
	{
		// In Level
		if (ob->clipRects.unitY2 < 140*16)
			screenY = 0;

		else
			screenY = ob->clipRects.unitY2 - 140*16;
	}

	if (!ck_inHighScores)
	{
		if (ff_map.warpInfo | ff_map.warpDeltaX | ff_map.warpDeltaY)	// bitwise or for slightly smaller code
			RF_Reposition(screenX, screenY);
		else
			RF_Reposition(screenX, screenY);
	}

	FF_CalcInactivate();
}

//===========================================================================

/*
==================
=
= WorldScrollScreen
=
= Scroll if Keen is nearing an edge
=
==================
*/

void FF_WorldScrollScreen( CK_object *keen )
{
	int scr_y, scr_x;

	if (ff_keenKilled)
		return;

	// Scroll Left, Right, or nowhere
	if ( keen->clipRects.unitX1 < rf_scrollXUnit + 144*16 )
		scr_x = keen->clipRects.unitX1 - (rf_scrollXUnit + 144*16);
	else if ( keen->clipRects.unitX2 > rf_scrollXUnit + 192*16 )
		scr_x = keen->clipRects.unitX2 /*+ 16*/ - (rf_scrollXUnit + 192*16);
	else
		scr_x = 0;

	// Scroll Up, Down, or nowhere
	if ( keen->clipRects.unitY1 < rf_scrollYUnit + 80*16 )
		scr_y = keen->clipRects.unitY1 - (rf_scrollYUnit + 80*16);
	else if ( keen->clipRects.unitY2 > rf_scrollYUnit + 112*16 )
		scr_y = keen->clipRects.unitY2 - (rf_scrollYUnit + 112*16);
	else
		scr_y = 0;

	// Limit scrolling to 256 map units (1 tile)
	// And update the active boundaries of the map
	if ( scr_x != 0 || scr_y != 0 )
	{
		if ( scr_x >= 256 )
			scr_x = 256-1;
		else if ( scr_x <= -256 )
			scr_x = -(256-1);

		if ( scr_y >= 256 )
			scr_y = 256-1;
		else if ( scr_y <= -256 )
			scr_y = -(256-1);

		RF_SmoothScroll( scr_x, scr_y );

		FF_CalcInactivate();
	}
}

//==========================================================================

/*
===============
=
= ScrollScreen
=
= Scroll if Keen is nearing an edge
= Set playstate to levelcomplete
=
===============
*/

void FF_ScrollScreen(CK_object *ob)
{
	int pxToMove, cmpAmt;
	int deltaX = 0, deltaY = 0;	// in Units

	// Don't scroll when Keen got killed, so he can fall out at the bottom
	if (ff_keenKilled)
		goto shake;

	// End level if keen makes it out either side
	if (((ob->clipRects.unitX1 < rf_scrollXMinUnit || ob->clipRects.unitX2 > rf_scrollXMaxUnit + 320*16)
		&& ob->clipped)	// K1n9_Duk3 mod: don't win a level while in noclip mode!
		|| ob->clipRects.unitY1 < 256 && !FF_KeenIsDead())	// K1n9_Duk3 mod: levels can be exited at the top
	{
		ck_gameState.levelState = ex_completed;	// K1n9_Duk3 fix: use enum name instead of number
		return;
	}

	// Kill keen if he falls out the bottom
	if (ob->clipRects.unitY2 > (rf_scrollYMaxUnit + 208*16))
	{
		ob->posY -= ob->clipRects.unitY2 - (rf_scrollYMaxUnit + 208*16);
		SD_PlaySound(FF_KEENFALLSND);
		ck_godMode = false;
		FF_KillKeen();
		goto shake;
	}

	// Keep keen's x-coord between 144-192 pixels
	if (ob->posX < (rf_scrollXUnit + 144*16))
		deltaX = ob->posX - (rf_scrollXUnit + 144*16);

	if (ob->posX > (rf_scrollXUnit + 192*16))
		deltaX = ob->posX - (rf_scrollXUnit + 192*16);

	// Keen should be able to look up and down.
	if (ob->currentAction == CK_ACTION(CK_ACT_keenLookUp2))
	{
		if (ff_map.screenYpx + SD_GetSpriteSync() > 167)
		{
			// Keen should never be so low on the screen that his
			// feet are more than 167 px from the top.
			pxToMove = 167 - ff_map.screenYpx;
		}
		else
		{
			// Move 1px per tick.
			pxToMove = SD_GetSpriteSync();
		}
		ff_map.screenYpx += pxToMove;
		deltaY = RF_PixelToUnit(-pxToMove);

	}
	else if (ob->currentAction == CK_ACTION(CK_ACT_keenLookDown3))
	{
		int pxToMove;
		if (ff_map.screenYpx - SD_GetSpriteSync() < 20)
		{
			// Keen should never be so high on the screen that his
			// feet are fewer than 33 px from the top.
			pxToMove = ff_map.screenYpx - 20;
		}
		else
		{
			// Move 1px/tick.
			pxToMove = SD_GetSpriteSync();
		}
		ff_map.screenYpx -= pxToMove;
		deltaY = pxToMove << 4;
	}
	else if (ff_map.screenYpx < 48)
	{
		ff_map.screenYpx += SD_GetSpriteSync();
	}

	// If we're attached to the ground, or otherwise awesome
	// do somethink inscrutible.
	if (ob->topTI || !ob->clipped || ob->currentAction == CK_ACTION(CK_ACT_keenHang1) || ob != ck_keenObj || ob->currentAction == CK_ACTION(FF_ACT_keenMindcontrol))
	{
		if (ob->currentAction != CK_ACTION(CK_ACT_keenPull1) &&
				ob->currentAction != CK_ACTION(CK_ACT_keenPull2) &&
				ob->currentAction != CK_ACTION(CK_ACT_keenPull3) &&
				ob->currentAction != CK_ACTION(CK_ACT_keenPull4))
		{
			deltaY += ob->deltaPosY;

			cmpAmt = (ff_map.screenYpx << G_P_SHIFT) + rf_scrollYUnit + deltaY;
			if (cmpAmt != ob->clipRects.unitY2)
			{
				int oneDiff, otherDiff;
				if (ob->clipRects.unitY2 < cmpAmt)
					oneDiff = cmpAmt - ob->clipRects.unitY2;
				else
					oneDiff = ob->clipRects.unitY2 - cmpAmt;

				otherDiff = (signed)((uint16_t)oneDiff << G_P_SHIFT) >> 7;
				if (otherDiff > 48)
					otherDiff = 48;
				otherDiff *= SD_GetSpriteSync();
				if (otherDiff < 16)
				{
					if (oneDiff < 16)
						otherDiff = oneDiff;
					else
						otherDiff = 16;
				}
				if (ob->clipRects.unitY2 < cmpAmt)
					deltaY -= otherDiff;
				else
					deltaY += otherDiff;
#if 0
				int adjAmt = (((ff_map.screenYpx << G_P_SHIFT) + rf_scrollYUnit + deltaY - ob->clipRects.unitY2));
				int adjAmt2 = abs(adjAmt / 8);
				adjAmt2 = (adjAmt2 <= 48) ? adjAmt2 : 48;
				if (adjAmt > 0)
					deltaY -= adjAmt2;
				else
					deltaY += adjAmt2;
#endif
			}
		}
	}
	else
	{
		// Reset to 140px.
		ff_map.screenYpx = 140;
	}


	// Scroll the screen to keep keen between 33 and 167 px.
	cmpAmt = rf_scrollYUnit + deltaY + ((ob->deltaPosY>=0)? 20*16 : 48*16);
	//cmpAmt = originyglobal + deltaY + 48*PIXGLOBAL;
	if (ob->clipRects.unitY2 < cmpAmt)
		deltaY += ob->clipRects.unitY2 - cmpAmt;

	if (ob->clipRects.unitY2 > (rf_scrollYUnit + deltaY + (168 << 4)))
		deltaY += ob->clipRects.unitY2 - (rf_scrollYUnit + deltaY + (168 << 4));

shake:
	if (ff_map.shaketics)
	{
		deltaX -= ff_map.shakeX;
		deltaY -= ff_map.shakeY;

		ff_map.shakeY = -ff_map.shakeX;
		if (++ff_map.shakecount & 1)
		{
			ff_map.shakeX = ff_map.shaketics;
		}
		else
		{
			ff_map.shakeX = -ff_map.shaketics;
		}

		deltaX += ff_map.shakeX;
		deltaY += ff_map.shakeY;

		if ((ff_map.shaketics -= SD_GetSpriteSync()) < 0)
			ff_map.shaketics = 0;
	}

	// Don't scroll more than one tile's worth per frame.
	if (deltaX || deltaY)
	{
		if (deltaX > 255) deltaX = 255;
		else if (deltaX < -255) deltaX = -255;

		if (deltaY > 255) deltaY = 255;
		else

			if (deltaY < -255) deltaY = -255;

		// Do the scroll!
		if (ck_keenObj->currentAction == CK_ACTION(FF_ACT_keenMindcontrol))
		{
			if (abs(deltaX) > 16)
				deltaX /= 2;
			if (abs(deltaY) > 16)
				deltaY /= 2;
		}
		RF_SmoothScroll(deltaX, deltaY);

		FF_CalcInactivate();
	}
}

//==========================================================================

/*
===============
=
= CK_WrapScroll	(K1n9_Duk3 addition)
=
= Makes the camera/origin and Keen (and Keen's shots) wrap around to the other
= end of the map when nearing the left/right/top/bottom end of the map, if the
= warp-around is enabled for the current map.
=
===============
*/

void FF_WrapScroll(void)
{
	if (ff_map.wrapScroll)
	{
		int dx, dy;

		if (rf_scrollXUnit < rf_scrollXMinUnit+256)
		{
			dx = rf_scrollXMaxUnit - (rf_scrollXMinUnit+2*256);
		}
		else if (rf_scrollXUnit > rf_scrollXMaxUnit-256)
		{
			dx = rf_scrollXMinUnit - (rf_scrollXMaxUnit-2*256);
		}
		else
		{
			dx = 0;
		}

		if (rf_scrollYUnit < rf_scrollYMinUnit+256)
		{
			dy = rf_scrollYMaxUnit - (rf_scrollYMinUnit+2*256);
		}
		else if (rf_scrollYUnit > rf_scrollYMaxUnit-256)
		{
			dy = rf_scrollYMinUnit - (rf_scrollYMaxUnit-2*256);
		}
		else
		{
			dy = 0;
		}

		if (dx || dy)
		{
			// move screen:
			//RF_Reposition(originxglobal+dx, originyglobal+dy);
			RF_Reposition(rf_scrollXUnit+dx, rf_scrollYUnit+dy);
			
			FF_CalcInactivate();	// K1n9_Duk3 addition 2024-01-01

			// move Keen:
			ck_keenObj->posX += dx;
			ck_keenObj->posY += dy;
			CK_ResetClipRects(ck_keenObj);	// update hitbox
			ck_keenObj->visible = true;	// force redraw	(re-enabled 2024-01-01)
			//R_Draw(player);	// redraw (disabled 2024-01-01)
			
			// move shots:
			{
				CK_object *currentObj;
				
				for (currentObj=ck_keenObj; currentObj; currentObj=currentObj->next)
				{
					if (currentObj->type == CT_Stunner)
					{
						currentObj->posX += dx;
						currentObj->posY += dy;
						currentObj->visible = true;	// force redraw	(re-enabled 2024-01-01)
						CK_ResetClipRects(currentObj);	// update hitbox
						//R_Draw(currentObj);	// redraw (disabled 2024-01-01)
					}
				}
			}
		}
	}
}

//===========================================================================


/*
============================
=
= PlayLoop
=
============================
*/


