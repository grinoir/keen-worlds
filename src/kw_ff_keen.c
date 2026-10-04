/*
Keen Worlds: Foray in the Forest's Keen, converted from CK_KEEN.C of its
source (Nisaba and K1n9_Duk3; GPL v2 or later) by tools/kw_fitf_port.py, with
the hand fixes of tools/kw_fitf_keen_fixes.py.
*/

#include "kw_fitf.h"
#include "kw_ff_act.h"
#include "kw_ff_keen.h"

static void FF_UncacheDreamSprite(CK_object *ob);

/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2024 Nisaba
 * Copyright (C) 2019-2025 K1n9_Duk3
 *
 * The code in this file is based on:
 * Keen Dreams Source Code
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

// CK_KEEN.C


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define KEENPRIORITY 1

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

uint16_t zeromap = 0;

/*
*/

/*
*/
/*






*/

void FF_KeenContact(CK_object *a, CK_object *b);
void FF_GivePoints(int16_t score);
void FF_SpawnKeen(int16_t tileX, int16_t tileY, int16_t direction);
void FF_TileBonus(int16_t tileX, int16_t tileY, int16_t itemNumber);
void FF_SpawnSplash(int16_t tileX, int16_t tileY);
void FF_GiveDrop(int16_t tileX, int16_t tileY);
void FF_CheckInTiles(CK_object *ob);
void FF_KeenSwitchThink(CK_object *ob);
bool FF_CheckEnterHouse(CK_object *ob);
void FF_T_LineUp(CK_object *ob);
void FF_WalkSound1(CK_object *ob);
void FF_WalkSound2(CK_object *ob);
void FF_KeenEnterThink(CK_object *ob);
void FF_KeenKeyThink(CK_object *ob);
void FF_HandleRiding(CK_object *ob);
bool FF_CheckGrabPole(CK_object *ob);
void FF_KeenWalkThink(CK_object *ob);
void FF_KeenStandThink(CK_object *ob);
void FF_KeenPauseThink(CK_object *ob);
void FF_KeenLookUpThink(CK_object *ob);
void FF_KeenLookDownThink(CK_object *ob);
void FF_KeenStandReact(CK_object *ob);
void FF_KeenWalkReact(CK_object *ob);
//void KeenReadThink(objtype *ob);
void FF_KeenAirThink(CK_object *ob);
void FF_KeenAirReact(CK_object *ob);
void FF_KeenBounceThink(CK_object *ob);
void FF_KeenPogoThink(CK_object *ob);
void FF_KeenToggleTrapDoor(int16_t x, int16_t y);
void FF_KeenPogoReact(CK_object *ob);
void FF_KeenPosContact(CK_object *ob, CK_object *other);
void FF_KeenSimpleReact(CK_object *ob);
void FF_KeenHoldThink(CK_object *ob);
void FF_T_PullUp1(CK_object *ob);
void FF_T_PullUp2(CK_object *ob);
void FF_T_PullUp3(CK_object *ob);
void FF_T_PulledUp(CK_object *ob);
void FF_KeenDieThink(CK_object *ob);
void FF_KillKeen();
void FF_KillKeenEx(bool force);
void FF_PoleActions(CK_object *ob);
void FF_KeenPoleThink(CK_object *ob);
void FF_KeenClimbThink(CK_object *ob);
void FF_KeenDropThink(CK_object *ob);
void FF_T_KeenPoleShoot(CK_object *ob);
void FF_KeenPoleReact(CK_object *ob);
void FF_ExplodeShot(CK_object *ob);
void FF_T_Shot(CK_object *shot);
void FF_R_Shot(CK_object *ob);
void FF_KeenShootThink(CK_object *ob);
void FF_T_Projectile(CK_object *ob);
void FF_T_KeenSink(CK_object *ob);

// New stuff
#if 1
void FF_T_KeenStartDream(CK_object *ob);
void FF_T_KeenDream(CK_object *ob);
void FF_T_DreamBubble(CK_object *ob);
void FF_R_AutoCache(CK_object *ob);

//void CK_DamageKeen(unsigned x, unsigned y);
//void CK_KeenKnockbackReact(objtype *ob);
void FF_T_KeenStunnedRecover(CK_object *ob);
void FF_R_KeenStunned(CK_object *ob);
#endif


//statetype s_keenRidePlatform = {0x8B, 0x8B, stepthink, false, push_down, 4, 0, 32, KeenStandThink, KeenContact, KeenStandReact, CK_ACTION(CK_ACT_keenRidePlatform)};








/*


#if 0
#endif
*/
















// Keen Shots







// New Stuff
#if 1
//statetype s_KeenKnockback1 = {KEENKNOCKBACK1SPR, KEENKNOCKBACK1SPR, stepthink, false, push_none, 8, 0, 0, T_Projectile, NULL, CK_KeenKnockbackReact, CK_ACTION(FF_ACT_KeenKnockback2)};
//statetype s_KeenKnockback2 = {KEENKNOCKBACK2SPR, KEENKNOCKBACK2SPR, stepthink, false, push_none, 8, 0, 0, T_Projectile, NULL, CK_KeenKnockbackReact, CK_ACTION(FF_ACT_KeenKnockback1)};
//statetype s_KeenKnockbackRecover1 = {KEENKNOCKDOWN1SPR, KEENKNOCKDOWN1SPR, step, false, push_none, 70, 0, 0, NULL, NULL, R_Draw, CK_ACTION(CK_ACT_keenStanding)};

//statetype SV_ACT_KeenNull;


//statetype SV_ACT_KeenNull = {-1, -1, think, false, 0, 0, 0, 0, NULL, NULL, R_Draw, NULL};
#endif

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/



// (ck_keenState is the engine's)

void FF_R_Draw(CK_object *ob);


soundnames bonussound[]  = { FF_GOTGEMSND, FF_GOTGEMSND, FF_GOTGEMSND, FF_GOTGEMSND,
                               FF_GOTITEMSND,FF_GOTITEMSND,FF_GOTITEMSND,FF_GOTITEMSND,FF_GOTITEMSND,FF_GOTITEMSND,
                               FF_GOTEXTRALIFESND, FF_GOTSTUNNERSND,FF_GOTITEMSND 
};

uint16_t bonuspoints[]  = {  0,   0,   0,   0, 100, 200, 500, 1000, 2000, 5000,   0,   0,   10000,   0};
uint16_t bonussprite[] = {	FF_GEMSHADOWSPR, FF_GEMSHADOWSPR, FF_GEMSHADOWSPR, FF_GEMSHADOWSPR, 
								FF_POINTS100SHADOWSPR, FF_POINTS200SHADOWSPR, FF_POINTS500SHADOWSPR,  FF_POINTS1000SHADOWSPR,  FF_POINTS2000SHADOWSPR,  FF_POINTS5000SHADOWSPR, 
								FF_POINTS1UPSHADOWSPR, FF_STUNNERSHADOWSPR, FF_POINTS10000SHADOWSPR, FF_POINTS2UPSHADOWSPR
							 };


void FF_GivePoints(int16_t score);

//==========================================================================

/*
============================
=
= KeenContact
=
============================
*/

void FF_KeenContact(CK_object *a, CK_object *b)
{
	// K1n9_Duk3 mod: use switch instead of if/else if
	switch (b->type)
	{
	case CT5_Item:
		
		if (ff_map.nisasi)
			break;	
		if (b->user1 > 12)
			break;
		if (b->user1 == 11 && ck_gameState.numShots >= ff_state.maxammo)
			break;
 	
		SD_PlaySound(bonussound[b->user1]);

		b->type = CT_Friendly;
		b->zLayer = FOREGROUNDPRIORITY;

		if (b->user1 != 10)
		{
			b->gfxChunk = bonussprite[b->user1];
		}

		FF_GivePoints(bonuspoints[b->user1]);

		if (bonuspoints[b->user1])
		{
			ff_map.itemsCollected++;	// K1n9_Duk3 addition
			if (ff_map.itemsCollected == ff_map.itemsTotal)
			{
				SD_PlaySound(FF_COLLECTEDALLPOINTSSND);
				ck_gameState.numLives++;
			}
		}

		b->yDirection = IN_motion_Up;	// just in case
		
		if (b->user1 < 4)
		{
			ck_gameState.keyGems[b->user1]++;
		}
		else if (b->user1 == 10)
		{
			if (ck_gameState.difficulty == D_Normal)
			{
				ck_gameState.numLives += 2;
				b->gfxChunk = bonussprite[13];
			}
			else
			{ 
				ck_gameState.numLives++;
				b->gfxChunk = bonussprite[b->user1];
			}
			//ck_mapState.livesCollected++;	// K1n9_Duk3 addition
		}
		else if (b->user1 == 11)
		{
			ck_gameState.numShots += ff_state.shotsinclip;
			if (ck_gameState.numShots > ff_state.maxammo)
				ck_gameState.numShots = ff_state.maxammo;

			//ck_mapState.ammoCollected++;	// K1n9_Duk3 addition
		}
	/* //ONLY IN KEEN 5!
		else if (b->user1 == 12)
		{
			ff_state.securityCard = 1;
		}
	*/
		CK_SetAction2(b, CK_ACTION(FF_ACT_bonusrise));
		break;

	case FF_CT_SneakyTree:	// K1n9_Duk3 mod
		if (b->user4)
			break;	// can't land on it while it's grabbed by a Nisasi
	case CT5_Platform:
	case FF_CT_LogPlatform:	// K1n9_Duk3 mod
	case FF_CT_OccupiedLog:
	case FF_CT_Cloud:
		if (!ck_keenState.platform)
			CK_PhysPushY(a,b);
		break;

	case FF_CT_FallBlock:
		CK_PhysPushXY(a, b, true);
		break;

	case FF_CT_Collectible:
		SD_PlaySound(FF_GOODIESSND);
		ff_map.collectiblesNew |= b->user1;
		b->type = CT_Friendly;
		b->zLayer = FOREGROUNDPRIORITY;
		CK_SetAction2(b, CK_ACTION(FF_ACT_bonusrise));
		break;

	case FF_CT_Lindsey:
		FF_LindseyDialog(b->user4);
		// turn Lindsey into a smoke cloud:
		b->type = FF_CT_LindseyVanish;	// no further interaction with this object
		b->gfxChunk = FF_SMIRKYPOOF1SPR;
		b->user1 = FF_SMIRKYPOOF4SPR;
		b->posX += -8*16;
		CK_SetAction2(b, CK_ACTION(FF_ACT_Smoke));
		SD_PlaySound(FF_YETICLOUDSND);

		//
		// find the next Lindsey (if it exists) and activate it:
		//
		for (a=ck_keenObj; a; a=a->next)
		{
			if (a->type == FF_CT_LindseyHidden && a->user4 == b->user4+1)
			{
				a->type = FF_CT_Lindsey;
				a->user3 = SD_GetLastTimeCount();
				break;
			}
		}
		break;
			
	case FF_CT_CouncilMemberDone:
	case FF_CT_CouncilMember:
		// show dialog
		if (!b->user4)	// but only if janitor is Nisasi-free
		{
			if (a->gfxChunk == FF_KEENSLEEP3SPR || a->gfxChunk == FF_KEENSLEEP4SPR)
			{
				// Keen is sleeping
				if (!b->user3)
				{
					FF_JanitorSleepMonolog();	// let Janitor mumble something
				}
				b->user3 = 42;	// restart cooldown, so we don't see the message in an infinite loop
			}
			else if (a->gfxChunk >= FF_KEENKNOCKBACK1SPR && a->gfxChunk <= FF_KEENKNOCKDOWN1SPR)
			{
				// Keen is stunned
				if (!b->user3)
				{
					FF_JanitorStunnedMonolog();	// let Janitor mumble something
				}
				b->user3 = 42;	// restart cooldown, so we don't see the message in an infinite loop
			}
			else if (ff_map.nisasi)	// Nisaba patch
			{
				// Keen is grabbed by a Nisasi
				if (!b->user3)
				{
					FF_JanitorNisasiDialog(0);
				}
				b->user3 = 42;	// restart cooldown, so we don't see the message in an infinite loop
			}
			else if (b->type == FF_CT_CouncilMember)
			{
				FF_JanitorDialog(b->user2);
				b->type = FF_CT_CouncilMemberDone;	// avoid showing the dialog a second time
			}
		}
		break;
		
	case FF_CT_Efree:
		FF_EfreeGrabKeen(b);
		break;

	case FF_CT_Checkpoint:
		if (ff_map.nisasi)
			break;
		if (!b->user1)
		{
			ff_map.checkpointTouched = b;	// checkpoint saving is done in PlayLoop()
		}
		break;
	}
}

int16_t slopespeed[8] = {0, 0, 4, 4, 8, -4, -4, -8};

int16_t ff_KeenPoleOffs[3] = {-8, 0, 8};


void FF_GivePoints(int16_t score)
{
	ck_gameState.keenScore += score;
//	ck_mapState.pointsCollected += score;	// K1n9_Duk3 addition
	if (IN_DemoGetMode() != IN_Demo_Off) return;
	if (ck_gameState.keenScore >= ck_gameState.nextKeenAt)
	{
		SD_PlaySound(FF_GOTEXTRALIFESND);
		if (ck_gameState.difficulty == D_Normal)
			ck_gameState.numLives += 2;
		else 
			ck_gameState.numLives++;
	//	ck_gameState.nextKeenAt *= 2;	

		if (ck_gameState.nextKeenAt == 160000)
		{
			ck_gameState.nextKeenAt = 314000;
		}
		else if (ck_gameState.nextKeenAt == 314000)
		{
			ck_gameState.nextKeenAt = 500000;
		}
		else
		{
			ck_gameState.nextKeenAt = ck_gameState.nextKeenAt * 2;
		}
	}	
}

//==========================================================================

/*
===============
=
= SpawnKeen
=
===============
*/

void FF_SpawnKeen(int16_t tileX, int16_t tileY, int16_t direction)
{
	ck_keenObj->type = CT_Player;
	ck_keenObj->active = OBJ_ALWAYS_ACTIVE; 
	ck_keenObj->visible = true;
	ck_keenObj->zLayer = KEENPRIORITY;
	ck_keenObj->clipped = CLIP_normal;
	ck_keenObj->posX = RF_TileToUnit(tileX);
	ck_keenObj->posY = RF_TileToUnit(tileY) - 241;
	ck_keenObj->xDirection = direction;
	ck_keenObj->yDirection = 1;
	CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenStanding));
}

//==========================================================================

/*
===============
=
= CK_SpawnItemNotify		(K1n9_Duk3 addition)
=
= Split off from TileBonus to implement 3UP tiles, but they were cut from FITF.
=
===============
*/

void FF_SpawnItemNotify(int16_t tileX, int16_t tileY, int16_t spriteNum)
{
	CK_object *new_object = CK_GetNewObj(true);
	
	new_object->type = CT_Friendly;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->yDirection = -1;
	new_object->user2 = spriteNum;
	new_object->gfxChunk = new_object->user2;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_bonusrise));
	new_object->clipped = CLIP_not;
}

//==========================================================================

/*
============================
=
= TileBonus
=
============================
*/

void FF_TileBonus(int16_t tileX, int16_t tileY, int16_t itemNumber)
{
	if (itemNumber == 10)
	{
		if (ck_gameState.difficulty == D_Normal)
		{
			ck_gameState.numLives += 2;
		}
		else
		{
			ck_gameState.numLives++;
		}
		//ck_mapState.livesCollected++;	// K1n9_Duk3 addition
	}
	else if (itemNumber == 11)
	{
		if (ck_gameState.numShots >= ff_state.maxammo)
			return;

		ck_gameState.numShots += ff_state.shotsinclip;
		if (ck_gameState.numShots >= ff_state.maxammo)
			ck_gameState.numShots = ff_state.maxammo;

		//ck_mapState.ammoCollected++;	// K1n9_Duk3 addition
	}

	RF_ReplaceTiles(&zeromap, 1, tileX, tileY, 1, 1);
	SD_PlaySound(bonussound[itemNumber]);

	FF_GivePoints(bonuspoints[itemNumber]);

	if (bonuspoints[itemNumber])
	{
		ff_map.itemsCollected++;	// K1n9_Duk3 addition
		if (ff_map.itemsCollected == ff_map.itemsTotal)
		{
			SD_PlaySound(FF_COLLECTEDALLPOINTSSND);
			ck_gameState.numLives++;
		}
	}
	
	if (itemNumber == 10 && ck_gameState.difficulty == D_Normal)
		FF_SpawnItemNotify(tileX, tileY, bonussprite[13]);	// spawns the 2UP sprite
	else
		FF_SpawnItemNotify(tileX, tileY, bonussprite[itemNumber]);
}
	
/*
===========================
=
= SpawnSplash
=
===========================
*/

void FF_SpawnSplash(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(true);
	new_object->type = CT_Friendly;
	new_object->clipped = CLIP_not;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	
	CK_SetAction(new_object, CK_ACTION(FF_ACT_splash1));
}

/*
============================
=
= GiveDrop
=
============================
*/

void FF_GiveDrop(int16_t tileX, int16_t tileY)
{
	RF_ReplaceTiles(&zeromap, 1, tileX, tileY, 1, 1);
	SD_PlaySound(FF_GOTVITALINSND);
	FF_SpawnSplash(tileX, tileY);

//	ck_mapState.dropsCollected++;	// K1n9_Duk3 addition

	// Award extra life at 100 vitalins
	if (++ck_gameState.numCentilife == 100)
	{
		CK_object *new_object = CK_GetNewObj(true);
		
		new_object->type = CT_Friendly;
		new_object->zLayer = FOREGROUNDPRIORITY;
		new_object->posX = RF_TileToUnit(tileX);
		new_object->posY = RF_TileToUnit(tileY);
		new_object->yDirection = IN_motion_Up;
		new_object->user2 = new_object->gfxChunk = FF_VITALIN1UPSHADOWSPR;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_bonusrise));
		new_object->clipped = CLIP_not;

		ck_gameState.numCentilife = 0;
		SD_PlaySound(FF_GOTEXTRALIFEK6SND);
		ck_gameState.numLives++;
	}
}

/*
============================
=
= CheckInTiles
=
============================
*/

void FF_CheckInTiles(CK_object *ob)
{
	int16_t x,y, specialTileInfo, targetXUnit;
	uint16_t warpinfo, warp, quicksand, quicksandy;

	ff_map.inwater = warpinfo = warp = quicksand = 0;

	// don't collect tiles during special death anmations:
	if (ob->currentAction == CK_ACTION(FF_ACT_keenDrown)
		|| ob->currentAction == CK_ACTION(FF_ACT_keenBurn)
		|| ob->currentAction == CK_ACTION(FF_ACT_Swallowed)
		|| ob->currentAction == CK_ACTION(FF_ACT_keenSink))
	{
		goto check_quicksand;
	}

	if (ff_map.moontile == 1)
		ff_map.moontile = 0;

	for (y = ob->clipRects.tileY1; y <= ob->clipRects.tileY2; ++y)
	{
		for (x = ob->clipRects.tileX1; x <= ob->clipRects.tileX2; ++x)
		{
			int16_t tilenum = FORESPOT(x,y);
			specialTileInfo =  (TI_ForeMisc(tilenum) & INTILE_TYPEMASK);
			switch (specialTileInfo)
			{
			case MF_FireHazard:
				if (!FF_KeenIsDead())
					FF_BurnKeen(true);
				else
					FF_KillKeenEx(true);
				break;

			case MF_DeadlyWater:
				if (!FF_KeenIsDead())
				{
					FF_DrownKeen(y);
					return;
				}
				break;
		/*
			case MF_Deadly:
				KillKeenEx(true);
				break;
		*/
			// Nisaba patch: Make deadly tiles slightly less deadly by giving
			// a tolerance margin of 4 pixels to each side.
			case MF_Deadly:
				if (x >= RF_UnitToTile(ob->clipRects.unitX1 + 4*16) && x <= RF_UnitToTile(ob->clipRects.unitX2 - 4*16))
					FF_KillKeenEx(true);
				break;
		
			case MF_Centilife:
				if (ff_map.nisasi)
					break;
				FF_GiveDrop(x,y);
				break;

			case MF_KeyholderA:
			case MF_KeyholderB:
			case MF_KeyholderC:
			case MF_KeyholderD:
				if (y == ob->clipRects.tileY2 && ob->topTI
					&& ob->currentAction != CK_ACTION(CK_ACT_keenPlaceGem)
					&& !ff_map.nisasi
					&& ck_gameState.keyGems[specialTileInfo - MF_KeyholderA])
				{
					targetXUnit = RF_TileToUnit(x) - 4*16;
					if (ob->posX == targetXUnit)
					{
						ck_gameState.keyGems[specialTileInfo - MF_KeyholderA]--;
						CK_SetAction2(ob, CK_ACTION(CK_ACT_keenPlaceGem));
					}
					else
					{
						ob->user1 = targetXUnit;
						ob->currentAction = CK_ACTION(CK_ACT_keenSlide);
					}
				}
				return;

			case MF_Moon:
				if (!ff_map.moontile)
					ff_map.moontile = 1;
				break;

			case MF_Points100:
			case MF_Points200:
			case MF_Points500:
			case MF_Points1000:
			case MF_Points2000:
			case MF_Points5000:
			case MF_1UP:
			case MF_Stunner:
				// check for Slug Slime cans:
				if (ff_map.nisasi)
					break;	
				if (tilenum == 0x5C || tilenum == 0x6E)
					ff_map.slugcans++;

				FF_TileBonus(x,y,specialTileInfo - (MF_Points100-4));
				break;
/*
			case MF_3UP:
				ck_gameState.numLives += 3;
				RF_ReplaceTiles(&zeromap, 1, x, y, 1, 1);
				SD_PlaySound(GOTEXTRALIFESND);
				CK_SpawnItemNotify(x, y, WORLDNESSIE);
				break;
*/
			case MF_Warp:
				if (warpinfo)
				{
					uint16_t info = INFOSPOT(x, y);
					if (info)
					{
						if (info != warpinfo)
						{
							// Keen is touching two warp tiles with differing destinations
							// so don't warp at all.
							warp = 0;
						}
						warpinfo = info;
					}
				}
				else
				{
					warpinfo = INFOSPOT(x, y);
					warp = warpinfo;
				}
				break;
				
#ifdef BETTER_WARP
			case MF_BetterWarp:
				if (x == ob->clipRects.tileXmid)
				{
					int16_t distX, distY;
					uint16_t info;
					
					info = INFOSPOT(x, y);
					if (info)
					{
						distY = (info & 0xFF) - y;
						distX = (info >> 8) - x;
						info = (distY & 0xFF) | (distX << 8);
						
						if (warpinfo)
						{
							if (info != warpinfo)
							{
								// Keen is touching two warp tiles with differing destinations
								// so don't warp at all.
								ff_map.warpDeltaX = ff_map.warpDeltaY = warp = 0;
							}
							warpinfo = info;
						}
						else
						{
							ff_map.warpDeltaX = distX;
							ff_map.warpDeltaY = distY;
							warpinfo = info;
						}
					}
				}
				break;
#endif

			case MF_MusicTrigger:
				if (!FF_KeenIsDead())
				{
					uint16_t info = INFOSPOT(x, y);
					if (info >= INFONUMBER_0 && info <= INFONUMBER_MAX)
					{
						info -= INFONUMBER_0;
						if (ff_map.music != info && info < FF_LASTMUSIC)
						{
							ff_map.music = info;
							FF_StartMusic(info, true);
						}
					}
				}
				break;

			case MF_Quicksand:
				if (!quicksand)
					quicksandy = y;
				quicksand = 1;
				ff_map.inwater = 1;	// quicksand also counts as water
				break;

			case MF_Water:
				// Keen must be at least 8 pixels into the water tile 
				// for him to be treated as being in the water:
				if (ob->clipRects.unitY2 >= RF_TileToUnit(y) + 8*16)
					ff_map.inwater = 1;
				break;
			}
		}
	}

	if (FF_KeenIsDead())
		quicksand = 0;

	// handle warping down here, so Keen can still collect tile items
	// near the warp spot.
	if (warp)
	{
		ff_map.warpInfo = warpinfo;
		// The actual warping must be handled between the refresh and
		// updating the objects, otherwise the objects around the new
		// position might be invisible for one frame.
	}

check_quicksand:
	// spawn or remove quicksand platform:
	if (quicksand && !ck_godMode)
	{
		if (quicksandy < ob->clipRects.tileY2)
		{
			// Keen's feet have sunken below the topmost quicksand tile
			// so Keen Must Die!

			FF_RemoveDreamBubbles();	// just in case...
			FF_DropNisasi();

			;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_keenSink));
			// Nisaba patch:
			//ob->user1 = 0;
			ob->user2 = 0;
			ob->clipped = CLIP_not;
			ff_keenKilled = true;
		}
		else if (ff_map.quicksand)
		{
			// move quicksand object along with Keen:
			FF_MoveObjHoriz(ff_map.quicksand, ob->posX - ff_map.quicksand->posX);
			FF_MoveObjVert(ff_map.quicksand, ob->clipRects.unitY2+16 - ff_map.quicksand->clipRects.unitY1);
		}
		else
		{
			uint16_t sandtop = RF_TileToUnit(quicksandy)+16;
			// turn off pogo:
			if (ob->currentAction->draw == FF_KeenPogoReact)
			{
				sandtop += 8*16;	// pogo landing makes Keen sink 8 pixels into the quicksand
				ob->posY += sandtop - ob->clipRects.unitY2;
				CK_SetAction2(ob, CK_ACTION(CK_ACT_keenFall2));
			}

			// spawn quicksand:
			FF_SpawnQuicksand(ob->posX, sandtop);
		}
	}
	else if (ff_map.quicksand)
	{
		// remove the quicksand object:
		if (ck_keenState.platform == ff_map.quicksand)
			ck_keenState.platform = NULL;
		CK_RemoveObj(ff_map.quicksand);
		ff_map.quicksand = NULL;
	}
}

/*
============================
=
= CK_OpenGemDoor		(K1n9_Duk3 addtion)
=
= Helper routine, split off from KeenKeyThink, to allow switches to open/close
= gem doors.
=
============================
*/

void FF_OpenGemDoor(int16_t targetX, int16_t targetY)
{
	int16_t doorHeight, doorY;
	uint16_t doorTile;

	CK_object *new_object = CK_GetNewObj(false);

	new_object->posX = targetX;
	new_object->posY = targetY;

	if (targetX > CA_GetMapWidth() || targetX < 2 || targetY > CA_GetMapHeight() || targetY < 2)
	{
	//	Quit("Keyholder points to a bad spot!");
		Quit("Foray in the Forest: BADKEYHOLDERSPOT");
	}

	// Calculate the height of the door.
	doorHeight = 0;
	doorY = targetY;
	doorTile = FORESPOT(targetX, doorY);
	do
	{
		doorHeight++;
		doorY++;
	} while ((TI_ForeMisc(FORESPOT(targetX, doorY)) & INTILE_TYPEMASK) == MF_GemDoor);

	new_object->user1 = doorHeight;
	new_object->user2 = TI_ForeAnimTile(doorTile)? 1 : -1;	// 1 = open door, -1 = close door
	new_object->clipped = CLIP_not;
	new_object->type = CT_Friendly;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	CK_SetAction(new_object, CK_ACTION(CK_ACT_DoorOpen1));
}

/*
============================
=
= CK_OpenDoubleDoor		(K1n9_Duk3 addtion)
=
= Alternative to CK_OpenGemDoor, except for two door-like things. Used for the
= gate in Temple of Doom.
=
============================
*/

void FF_OpenDoubleDoor(int16_t targetX, int16_t targetY)
{
	int16_t doorHeight, doorY;
	uint16_t doorType;

	if (targetX > CA_GetMapWidth() || targetX < 2 || targetY > CA_GetMapHeight() || targetY < 2)
	{
	//	Quit("Keyholder points to a bad spot!");
		Quit("Foray in the Forest: BADKEYHOLDERSPOT");
	}

	// Calculate the height of the door.
	doorHeight = 0;
	doorY = targetY;
	doorType = TI_ForeMisc(FORESPOT(targetX, doorY)) & INTILE_TYPEMASK;
	do
	{
		doorHeight++;
		doorY++;
	} while ((TI_ForeMisc(FORESPOT(targetX, doorY)) & INTILE_TYPEMASK) == doorType);
	
	// Spawn door opener for each side of the door:
	for (doorY = 0; doorY < 2; doorY++)
	{
		CK_object *new_object = CK_GetNewObj(false);
		new_object->posX = targetX;
		new_object->posY = targetY;
		new_object->user1 = doorHeight;
		new_object->user2 = 1;	// 1 = open door, -1 = close door
		new_object->clipped = CLIP_not;
		new_object->type = CT_Friendly;
		new_object->active = OBJ_ALWAYS_ACTIVE;
		CK_SetAction(new_object, (doorType == MF_DoubleDoor1)? CK_ACTION(CK_ACT_DoorOpen3) : CK_ACTION(CK_ACT_DoorOpen2));

		targetX++;
	}
}

/*
============================
=
= ParseToggleStack		(K1n9_Duk3 addtion)
=
= Processes a stack of infoplane links and toggles whatever they are linked to.
= This is KEENGINE-like behavior, meaning the links go down. But it can easily
= be changed to go up like in Atroxian Realm. Unlike Atroxian Realm, this code
= doesn't require tiles with a special INTILE value to be placed where the link
= info is; it decides what to do with each link info based on what it links to.
=
============================
*/

void FF_ParseToggleStack(int16_t tileX, int16_t tileY)
{
	static int16_t oppositearrow[8] = {2,3,0,1,6,7,4,5};

	uint16_t targetX, targetY, x, y;
	uint16_t info, offset, targetOff, tile;

//	unsigned top, mid, bot, far *map;	// zapper variables

	offset = ((tileY) * CA_GetMapWidth()) + tileX;

	while ((info = FF_MapAt(2, offset)) != 0 && (uint16_t)tileY < CA_GetMapHeight())
	{
		offset += CA_GetMapWidth();	// += means move down, -= means move up
		tileY++;	// ++ means move down, -- means move up

		targetX = (info >> 8);
		targetY = (info & 0xFF);

		// safety check (could be omitted):
		if (targetX < 2 || targetX > CA_GetMapWidth() || targetY < 2 || targetY > CA_GetMapHeight())
			break;

		targetOff = ((targetY) * CA_GetMapWidth()) + targetX;

		switch (TI_ForeMisc(FF_MapAt(1, targetOff)) & INTILE_TYPEMASK)
		{
		case MF_GemDoor:
			FF_OpenGemDoor(targetX, targetY);
			break;

		case MF_DoubleDoor1:
		case MF_DoubleDoor2:
			FF_OpenDoubleDoor(targetX, targetY);
			break;

		case MF_CloneMachine:
			FF_ToggleCloneMachine(targetX, targetY);
			break;
/*
		case MF_ZapperOn:
			map = mapsegs[1];
			top = *map;
			mid = *++map;
			bot = *++map;
			map = mapsegs[1] + targetOff + mapwidth;

			RF_ReplaceTiles(&top, 1, targetX, targetY++, 1, 1);
			while (TI_ForeMisc(*map) == MF_Deadly)
			{
				RF_ReplaceTiles(&mid, 1, targetX, targetY++, 1, 1);
				map += mapwidth;
			}
			RF_ReplaceTiles(&bot, 1, targetX, targetY, 1, 1);
			break;

		case MF_ZapperOff:
			map = mapsegs[1] + 3;
			top = *map;
			mid = *++map;
			bot = *++map;
			map = mapsegs[1] + targetOff + mapwidth;

			RF_ReplaceTiles(&top, 1, targetX, targetY++, 1, 1);
			while (TI_ForeMisc(*map) != MF_ZapperOff)
			{
				RF_ReplaceTiles(&mid, 1, targetX, targetY++, 1, 1);
				map += mapwidth;
			}
			RF_ReplaceTiles(&bot, 1, targetX, targetY, 1, 1);
			break;
*/

		default:
			if (!TI_ForeAnimTile(FF_MapAt(1, targetOff)))
			{
				info = FF_MapAt(2, targetOff);
				if (!info)
				{
					CA_SetTileAtPos((targetOff) % CA_GetMapWidth(), (targetOff) / CA_GetMapWidth(), 2, PLATFORMBLOCK);
				}
				else if (info >= DIRARROWSTART && info < DIRARROWEND)
				{
					CA_SetTileAtPos((targetOff) % CA_GetMapWidth(), (targetOff) / CA_GetMapWidth(), 2, oppositearrow[info-DIRARROWSTART] + DIRARROWSTART);
				}
				else
				{
					CA_SetTileAtPos((targetOff) % CA_GetMapWidth(), (targetOff) / CA_GetMapWidth(), 2, 0);
				}
				break;
			}
			// no break here! dropping through to bridge behavior is intended
		case MF_Bridge:
			for (y = 0; y < 2; y++,targetY++)
			{
				for (x = targetX - y,targetOff = ((targetY) * CA_GetMapWidth()) + x; x < CA_GetMapWidth(); x++, targetOff++)
				{
					{
						 int16_t anim;

						tile = FF_MapAt(1, targetOff);
						anim = TI_ForeAnimTile(tile);
						if (!anim)
							break;
						tile += anim;
					}
					// K1n9_Duk3 note: I intentionally split the anim check into a
					// separate code block that doesn't contain the RF_ReplaceTiles call.
					// This allowed the compiler to generate slightly more efficient
					// code, because it knows that the anim variable definitely does
					// not exist anymore when the function is called and therefore
					// doesn't need to be put in a register that is still valid after
					// the function call.
					
					RF_ReplaceTiles(&tile, 1, x, targetY, 1, 1);
				}
			}
		}
	}
}

/*
=======================
=
= ToggleSwitch		(K1n9_Duk3 addition)
=
= Helper routine, split off from KeenSwitchThink to allow the code to be re-used
= by the mind-controlled Council Page.
=
=======================
*/

void FF_ToggleSwitch(int16_t switchTileX, int16_t switchTileY)
{
/*
	int tileY, tileX;
	unsigned currentTile, newTile;
	static int infoPlaneInverses[8] = {2,3,0,1,6,7,4,5};	// K1n9_Duk3 mod: added 'static'
	unsigned infoPlaneValue;
	unsigned switchTarget = INFOSPOT(switchTileX, switchTileY);
	int switchTargetX = (switchTarget >> 8);
	int switchTargetY = (switchTarget & 0xFF);
*/
	uint16_t switchTile = FORESPOT(switchTileX, switchTileY);
//	char switchMisc = TI_ForeMisc(switchTile);

	// Toggle the switch.
	uint16_t switchNextTile = switchTile + TI_ForeAnimTile(switchTile);
	RF_ReplaceTiles(&switchNextTile, 1, switchTileX, switchTileY, 1, 1);
	SD_PlaySound(FF_CLICKSND);

	FF_ParseToggleStack(switchTileX, switchTileY);

/*
	switch (switchMisc)
	{
	case MF_BridgeSwitch:
		for (tileY = switchTargetY; tileY < switchTargetY + 2; ++tileY)
		{
			for (tileX = switchTargetX - ((tileY == switchTargetY)? 0 : 1); tileX < mapwidth; ++tileX)
			{
				currentTile = FORESPOT(tileX, tileY);
				if (!TI_ForeAnimTile(currentTile)) break;
				newTile = currentTile + TI_ForeAnimTile(currentTile);
				RF_ReplaceTiles(&newTile, 1, tileX, tileY, 1, 1);
			} 
		}
		break;
	case MF_SwitchOff:
	case MF_SwitchOn:
		infoPlaneValue = INFOSPOT(switchTargetX, switchTargetY);
		if (infoPlaneValue >= DIRARROWSTART && infoPlaneValue < DIRARROWEND)
		{
			// Invert the direction of the goplat arrow.
			infoPlaneValue = infoPlaneInverses[infoPlaneValue - DIRARROWSTART] + DIRARROWSTART;
		}
		else
		{
			// Insert or remove a [B] block.
			infoPlaneValue ^= PLATFORMBLOCK;
		}
		
		*(mapsegs[2] + (switchTargetY*mapwidth + switchTargetX)) = infoPlaneValue;
		break;
	}
*/
}

//===========================================================================

/*
=======================
=
= KeenSwitchThink
=
=======================
*/

void FF_KeenSwitchThink(CK_object *ob)
{
	FF_ToggleSwitch(ob->clipRects.tileXmid, ob->clipRects.tileY1);
}

//===========================================================================

/*
=======================
=
= T_KeenUseAmmoStation		(K1n9_Duk3 addition)
=
=======================
*/

void FF_T_KeenUseAmmoStation(CK_object *ob)
{
	uint16_t limit = INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1);

	if (limit >= INFONUMBER_0 && limit <= INFONUMBER_MAX)
	{
		limit -= INFONUMBER_0;
		if (limit > ff_state.maxammo)
			limit = ff_state.maxammo;
	}
	else
	{
		limit = DEFAULT_SHOTS;
	}
	// give ammo:
	if (ck_gameState.numShots < limit)
	{
		ck_gameState.numShots++;

		if (ck_gameState.numShots == limit)
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenPressSwitch2);
			SD_PlaySound(FF_GOTSTUNNERSND);
		}
		else
		{
			SD_PlaySound(FF_BALLBOUNCESND);
		}
	}
	else
	{
		SD_PlaySound(FF_REFILLUNITSND);
		ob->currentAction = CK_ACTION(CK_ACT_keenPressSwitch2);
	}
}


//==========================================================================

/*
======================
=
= CheckEnterHouse
=
= Checks for tiles that Keen can interact with by pressing up
=
======================
*/

bool FF_CheckEnterHouse(CK_object *ob)
{
	uint16_t destx;
	uint8_t intile, intile2;


	intile = TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1)) & INTILE_TYPEMASK;

	// Are we pressing a switch? And is the door still active?
	if (intile == MF_SwitchOff || intile == MF_SwitchOn || intile == MF_BridgeSwitch)
	{
		destx = RF_TileToUnit(ob->clipRects.tileXmid) - 4*16;
		if (ob->posX == destx && !FF_DoorsActive())
		{
			// Flip that switch!
			ob->currentAction = CK_ACTION(CK_ACT_keenPressSwitch1);
		}
		else
		{
			ob->user1 = destx;
			ob->currentAction = CK_ACTION(CK_ACT_keenSlide);
		}
		ff_keenUpHeld = true;
		return true;
	}

	// K1n9_Duk3 addition:
	if (intile == MF_MindMachine)
	{
		CK_object *page;

		page = FF_FindPageObj(INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1));
		if (page)
		{
			destx = RF_TileToUnit(ob->clipRects.tileXmid) - 12*16;
			if (intile != (TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid - 1, ob->clipRects.tileY1)) & INTILE_TYPEMASK))
			{
				destx += 256;
			}
			if (ob->posX == destx)
			{
				if (intile == (TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2)) & INTILE_TYPEMASK))
				{
					// Keen's feet are inside the machine, don't walk up
					ob->currentAction = CK_ACTION(FF_ACT_keenMindcontrol);
					ob->user2 = FF_ObjToUser(FF_SpawnSlideDoor(ob->clipRects.tileX1, ob->clipRects.tileY2, 1));
				}
				else
				{
					// Keen's feet are NOT in the machine, so walk up
					ob->currentAction = CK_ACTION(FF_ACT_keenEnterMachine);
				}
				ob->user1 = 0;
				ob->user3 = FF_ObjToUser(page);
			}
			else
			{
				ob->user1 = destx;
				ob->currentAction = CK_ACTION(CK_ACT_keenSlide);
			}
			ff_keenUpHeld = true;
			return true;
		}
	}

	if (intile == MF_AmmoStation)
	{
		destx = RF_TileToUnit(ob->clipRects.tileXmid) - 4*16;
		if (ob->posX == destx)
		{
			ob->currentAction = CK_ACTION(FF_ACT_keenUseAmmoStation);
		}
		else
		{
			ob->user1 = destx;
			ob->currentAction = CK_ACTION(CK_ACT_keenSlide);
		}
		ff_keenUpHeld = true;
		return true;
	}
	// end of addition
	
	if (intile == MF_Dialog)		// Nisaba patch
	{
		destx = RF_TileToUnit(ob->clipRects.tileXmid) - 4*16;
		
		if (ob->posX == destx && ob->gfxChunk == FF_KEENWALKAWAY1SPR)
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenPressSwitch2);
#if 0
			if (intile == MF_Dialog)
			{
				CK_HALDialog();
			}
			else
			{
				FF_RunSnailas();
			}
#else
			FF_MiscDialog(INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1)-INFONUMBER_0);
#endif
		}
		else
		{
			ob->user1 = destx;
			ob->currentAction = CK_ACTION(CK_ACT_keenSlide);
		}
		ff_keenUpHeld = true;
		return true;
	}
	
	// Are we enterting a door?
	if (intile == MF_Door || intile == MF_KeycardDoor || intile == MF_QuickDoor)
	{
		destx = RF_TileToUnit(ob->clipRects.tileXmid) + 4*16;

		// If the door is two tiles wide, we want to be in the centre.
		intile2 = TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid - 1, ob->clipRects.tileY1)) & INTILE_TYPEMASK;
	//	if (intile2 == MF_Door || intile2 == MF_KeycardDoor)
		if (intile2 == intile)	// K1n9_Duk3 mod (smaller and faster)
		{
			destx -= 256;
		}

		if (ob->posX == destx && ob->gfxChunk == FF_KEENWALKAWAY1SPR)
		{
			//We're at the door.
			
			// Is it a special door?
			if (intile == MF_QuickDoor)
			{
				// instant teleport to destination:
				FF_DelayedFade();	// fade out (and automatically fade in later)
				ob->currentAction = CK_ACTION(FF_ACT_keenEnterDoor6);
				FF_TeleportKeen(ob, INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1));
			}
			else if (intile == MF_KeycardDoor)
			{
			/*
				if (ff_state.securityCard)
				{
					objtype *new = GetNewObj(false);
					new->posX = ob->clipRects.tileXmid - 2;
					new->posY = ob->clipRects.tileY2 - 4;
					new->active = ac_allways;
					new->type = CT_Friendly;
					NewState(new, CK_ACTION(CK_ACT_SecurityDoorOpen));
					
					ff_state.securityCard = 0;
					SD_PlaySound(OPENSECURITYDOORSND);
					ob->currentAction = CK_ACTION(CK_ACT_keenEnterDoor1);
					ob->yDirection = motion_Down;	// K1n9_Duk3 fix: avoid door glitch
					ob->zLayer = 0;
					ff_keenUpHeld = true;
					return true;
				}
				else
			*/
				{
					SD_PlaySound(FF_OLDNOWAYSND);
					ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
					ff_keenUpHeld = true;
					return false;
				}
			}
			else
			{
				ff_map.invincibilityTimer = 110;
				ob->yDirection = IN_motion_Down;	// K1n9_Duk3 fix: avoid door glitch
				ob->currentAction = CK_ACTION(CK_ACT_keenEnterDoor2);
				ob->zLayer = 0;

#if 0
				if (!INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1))
				{
					CK5_SpawnLightning();
				}
#endif
			}
		}
		else
		{
			ob->user1 = destx;
			ob->currentAction = CK_ACTION(CK_ACT_keenSlide);
		}

		ff_keenUpHeld = true;

		return true;
	}


	// No? Return to our caller, who will handle poles/looking up.
	return false;
}
	
//===========================================================================

/*
=======================
=
= T_LineUp
=
= Lines up Keen's position for interacting with tiles (temp1 is desired x)
=
=======================
*/

void FF_T_LineUp(CK_object *ob)
{
	int16_t deltaX = ob->user1 - ob->posX;
	if (deltaX < 0)
	{
		// Move left one px per tick.
		ck_nextX -= SD_GetSpriteSync() * 16;
		// If we're not at our target yet, return.
		if (ck_nextX > deltaX) return;
	}
	else if (deltaX > 0)
	{
		// Move right one px per tick.
		ck_nextX += SD_GetSpriteSync() * 16;
		// If we're not at our target yet, return.
		if (ck_nextX < deltaX) return;
	}

	// We're at our target.
	ck_nextX = deltaX;
	ob->user1 = 0;
	if (!FF_CheckEnterHouse(ob))
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
	}
}

//==========================================================================

/*
============================
=
= WalkSound1
=
============================
*/

void FF_WalkSound1(CK_object *ob)
{
	SD_PlaySound(FF_KEENWALK1SND);
}

/*
============================
=
= WalkSound2
=
============================
*/

void FF_WalkSound2(CK_object *ob)
{
	SD_PlaySound(FF_KEENWALK2SND);
}

//==========================================================================

/*
============================
=
= TeleportKeen		(K1n9_Duk3 addition)
=
============================
*/

void FF_TeleportKeen(CK_object *ob, uint16_t destination)
{
	ob->posY = RF_TileToUnit((destination&0xFF)) - 256 + 15;
	ob->posX = RF_TileToUnit((destination >> 8));
	ob->zLayer = KEENPRIORITY;
	ob->clipped = CLIP_not;
	CK_SetAction2(ob, ob->currentAction->next);
	ob->clipped = CLIP_normal;
	CK_CentreCamera(ob);
}

/*
=======================
=
= KeenEnterThink
=
=======================
*/

// Think function for entering a door.
void FF_KeenEnterThink(CK_object *ob)
{
	uint16_t destination = INFOSPOT(ob->clipRects.tileX1, ob->clipRects.tileY2);

	if (destination == 0x0000)
	{
		ck_gameState.levelState = ex_completed;	// Keen 5 used ex_portout here (secret level teleporter)
		ob->currentAction = CK_ACTION(CK_ACT_keenEnteredDoor);
		return;
	}

	if (destination == 0xB1B1)
	{
		ck_gameState.levelState = ex_completed;	// K1n9_Duk3 fix: use enum name instead of number
		ob->currentAction = CK_ACTION(CK_ACT_keenEnteredDoor);
		return;
	}

	FF_TeleportKeen(ob, destination);
}

//===========================================================================

/*
=======================
=
= KeenKeyThink
=
=======================
*/

void FF_KeenKeyThink(CK_object *ob)
{
	uint16_t oldFGTile = FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2);
	uint16_t newFGTile = oldFGTile + 18;
	uint16_t target = INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2);

	uint16_t targetX = target >> 8;
	uint16_t targetY = target & 0xFF;

	RF_ReplaceTiles(&newFGTile, 1, ob->clipRects.tileXmid, ob->clipRects.tileY2, 1, 1);

	SD_PlaySound(FF_OPENGEMDOORSND);
	
	switch (TI_ForeMisc(FORESPOT(targetX, targetY)) & INTILE_TYPEMASK)
	{
	case MF_DoubleDoor1:
	case MF_DoubleDoor2:
		FF_OpenDoubleDoor(targetX, targetY);
		break;

	default:
		FF_OpenGemDoor(targetX, targetY);
		break;
	}
}

//===========================================================================

/*
============================
=
= HandleRiding
=
============================
*/

void FF_HandleRiding(CK_object *ob)
{
	// Save the platform pointer, we might be wiping it.
	CK_object *plat = ck_keenState.platform;
	int16_t deltax = ob->deltaPosX;	// K1n9_Duk3 addition


	if (ob->clipRects.unitX2 < plat->clipRects.unitX1 || ob->clipRects.unitX1 > plat->clipRects.unitX2)
	{
		// We've fallen off the platform horizontally.
		ck_keenState.platform = NULL;
		return;
	}

	if (ob->deltaPosY < 0)
	{
		// If we've jumped off the platform.
		ck_keenState.platform = NULL;
		if (plat->deltaPosY < 0)
		{
			ck_nextX = 0;
			ck_nextY = plat->deltaPosY;
			CK_PhysUpdateSimpleObj(ob);
			return;
		}
	}
	else
	{
		//Ride the platform
		ck_nextX = plat->deltaPosX;
		ck_nextY = plat->clipRects.unitY1 - ob->clipRects.unitY2 - 16;
		CK_PhysUpdateSimpleObj(ob);

		if (ob->clipRects.unitY2 != plat->clipRects.unitY1 - 16)
		{
			// blocking tiles prevent Keen from being moved to the top of
			// the platform, meaning we're no longer standing on it
			ck_keenState.platform = NULL;
			return;
		}

		// Keen has a "/NOPAN" parameter which disables use of the EGA panning
		// register. In order to make platforms less ugly when this is on
		// (though not much less ugly, IMHO), we would do some more processing here.
		// As is, we just do the line below, to keep keen at the same position while scrolling.	

#ifdef NEWMODS
		// K1n9_Duk3 fix:
		// Make sure Keen and the platform have the same "subpixel" position
		// so that the sprites don't move back and forth in relation to each
		// other when moving sideways. But only if the platform moved sideways
		// and Keen didn't (before the platform moved him).
		if (deltax == 0 && plat->deltaPosX != 0)
		{
			ob->posX &= ~0xF;
			ob->posX |= plat->posX & 0xF;
		}

#else
		// Keen's x position should move with 2px granularity when on a platform so that scrolling
		// looks nice. (Keen will stay at the same pixel position when scrolling, as we scroll 2px at a time).
		ob->posX |= plat->posX & 0x1F;
#endif

		// We've hit the ceiling?
		if (ob->bottomTI)
		{
			ck_keenState.platform = NULL;
			return;
		}

		// We're standing on something, don't fall down!
		ob->topTI = PLATFORMEDGE;
	}
}


//==========================================================================

/*
======================
=
= CheckGrabPole
=
======================
*/

bool FF_CheckGrabPole(CK_object *ob)
{
	uint16_t candidateTile;

//
// kludgy bit to not let you grab a pole the instant you jump off it
//
	if (SD_GetLastTimeCount() < ck_keenState.poleGrabTime)
		ck_keenState.poleGrabTime = 0;
	else if (SD_GetLastTimeCount() - ck_keenState.poleGrabTime < 19)
		return false;

	candidateTile = FORESPOT(ob->clipRects.tileXmid, ((ck_inputFrame.yDirection==-1)?((ob->clipRects.unitY1+96)>>G_T_SHIFT):(ob->clipRects.tileY2+1)));


	if ((TI_ForeMisc(candidateTile) & INTILE_TYPEMASK) == MF_Pole)
	{
		ob->posX = RF_TileToUnit(ob->clipRects.tileXmid - 1) + 8*16;
		ck_nextX = 0;
		ck_nextY = ck_inputFrame.yDirection * 32;
		ob->clipped = CLIP_not;		// can climb through pole holes
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleSit);
		ck_keenState.platform = NULL;	// K1n9_Duk3 addition
		return true;
	}
	return false;
}

//===========================================================================

/*
=======================
=
= KeenWalkThink
=
=======================
*/

void FF_KeenWalkThink(CK_object *ob)
{
#if 0	// K1n9_Duk3 mod: speed adjustment code has been moved into StateMachine()
	// K1n9_Duk3 addition:
	if (ff_map.nisasi || ff_map.inwater)
		ck_nextX >>= 1;	// HACK: run at half the speed
	// end of addition
#endif

	if (!ck_inputFrame.xDirection)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
		FF_KeenStandThink(ob);
		return;
	}

	ob->xDirection = ck_inputFrame.xDirection;
	
	if (ck_inputFrame.yDirection == -1)
	{
		if (FF_CheckGrabPole(ob)) return;
		if (ff_keenUpHeld || FF_CheckEnterHouse(ob))
			return;
	}
	else if (ck_inputFrame.yDirection == 1)
	{
		if (FF_CheckGrabPole(ob)) return;
	}

	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		
		if (ck_inputFrame.yDirection == -1)
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenShootUp1);
		}
		else
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenShoot1);
		}
		return;
	}

	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
	{
		ck_keenState.jumpWasPressed = true;
		SD_PlaySound(FF_KEENJUMPSND);
		ob->velX = ob->xDirection * 16;
		ob->velY = -40;
		ck_nextX = ck_nextY = 0;
		ck_keenState.jumpTimer = 18;
		ob->currentAction = CK_ACTION(CK_ACT_keenJump1);
		// Is this the mystical 'impossible pogo'?
		// k5disasm has a 'return' here, probably a mistake?
	}

	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		ob->currentAction = CK_ACTION(CK_ACT_keenPogo1);
		SD_PlaySound(FF_KEENJUMPSND);
		ob->velX = ob->xDirection * 16;
		ob->velY = -48;
		// Should this be nextY? Andy seems to think so, but lemm thinks that X is right...
		ck_nextX = 0;
		ck_keenState.jumpTimer = ff_map.inwater? 0 : 24;
		return;
	}

#if 0	// K1n9_Duk3 mod: speed adjustment code has been moved into StateMachine()

	// Andy seems to think this is Y as well. Need to do some more disasm.
	// If this is an X vel, then surely we'd want to multiply it by the direction?
	
	// While the Keen 5 Disassembly does set xtry here, it would make more sense if ytry 
	// were being set.
	
	// K1n9_Duk3 note: The slopespeed array contains additional X-movement for
	// Keen walking on slopes, to slow him down when running uphill and speed him
	// up when running downhill. It has nothing to do with Keen's Y position.
#if 0
	ck_nextX += slopespeed[ob->topTI & SLOPEMASK] * SD_GetSpriteSync();
#else
	// FITF version:
	{
		 int16_t slopeadjust;

		slopeadjust = slopespeed[ob->topTI & SLOPEMASK] * SD_GetSpriteSync();
		if (ff_map.nisasi || ff_map.inwater)
			slopeadjust >>= 1;	// HACK: move at half the speed
		ck_nextX += slopeadjust;
	}
#endif
#else
	// K1n9_Duk3 addition: let StateMachine() know that xtry has to be adjusted:
	ff_keenMustAdjustNextX = true;
#endif

	if ((ob->currentAction->chunkLeft == CK_ACTION(CK_ACT_keenRun1)->chunkLeft) && !(ob->user3))
	{
		SD_PlaySound(FF_KEENWALK1SND);
		ob->user3 = 1;
	}
	else if ((ob->currentAction->chunkLeft == CK_ACTION(CK_ACT_keenRun3)->chunkLeft) && !(ob->user3))
	{
		SD_PlaySound(FF_KEENWALK2SND);
		ob->user3 = 1;
	}
	else if ((ob->currentAction->chunkLeft == CK_ACTION(CK_ACT_keenRun2)->chunkLeft) || (ob->currentAction->chunkLeft == CK_ACTION(CK_ACT_keenRun4)->chunkLeft))
	{
		ob->user3 = 0;
	}
}

//==========================================================================

/*
============================
=
= KeenStandThink
=
============================
*/

void FF_KeenStandThink(CK_object *ob)
{
#ifdef KEEN5
	// If we're riding a platform, do it surfin' style!)
	if (ob->topTI == PLATFORMEDGE)
	{
		// But only if such an action exists in this episode. :/
		if (CK_ACTION(CK_ACT_keenRidePlatform))
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenRidePlatform);
		}
	}
#endif

	if (ck_inputFrame.xDirection)
	{
	// started walking
		ob->currentAction = CK_ACTION(CK_ACT_keenRun1);
		FF_KeenWalkThink(ob);
		
		// K1n9_Duk3 note: I *think* this was supposed to make Keen move a little
		// slower than normal when he changes from standing still to walking. But
		// it ends up doing the opposite in some (rare) cases.
		//
		// Let's assume tics is 5 and Keen was in the standing state. The standing
		// state has a tic time of 4, so depending on how many tics have already
		// accumulated while Keen was standing, the state machine code might have
		// used only 1 to 4 out of the 5 tics. The remaining tics will be spent in
		// Keen's walking state(s), which will add additional movement (24 units
		// per tic). If Keen is walking normally (on flat ground), he will move
		// up to 5*24 = 120 units in 5 tics. But if Keen just started walking and
		// there are 4 tics left after this routine is executed, Keen will move
		// (24*5)/4 + 4*24 = 30 + 96 = 126 units! 
		ck_nextX = (int16_t)(ob->xDirection * ob->currentAction->velX * SD_GetSpriteSync())/4;
		
		// K1n9_Duk3 addition: don't modify this xtry value in StateMachine():
		ff_keenMustAdjustNextX = false;
		return;
	}

	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		if (ck_inputFrame.yDirection == -1)
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenShootUp1);
		}
		else
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenShoot1);
		}
		return;
	}

	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
	{
	// jump straight up
		ck_keenState.jumpWasPressed = true;
		SD_PlaySound(FF_KEENJUMPSND);
		ob->velX = 0;
		ob->velY = -40;
		ck_nextY = 0;
		ck_keenState.jumpTimer = 18;
		ob->currentAction = CK_ACTION(CK_ACT_keenJump1);
		return;
	}

	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
	// get on pogo
		ck_keenState.pogoWasPressed = true;
		SD_PlaySound(FF_KEENJUMPSND);
		ob->currentAction = CK_ACTION(CK_ACT_keenPogo1);
		ob->velX = 0;
		ob->velY = -48;
		ck_nextY = 0;
		ck_keenState.jumpTimer = ff_map.inwater? 0 : 24;
		return;
	}

	if (ck_inputFrame.yDirection == -1)
	{
		if (FF_CheckGrabPole(ob)) return;
		if (!ff_keenUpHeld && FF_CheckEnterHouse(ob)) return;
		ob->currentAction = CK_ACTION(CK_ACT_keenLookUp1);
	}	
	else if (ck_inputFrame.yDirection == 1)
	{
		// Try poles.
		if (FF_CheckGrabPole(ob)) return;
		// Keen looks down.
		ob->currentAction = CK_ACTION(CK_ACT_keenLookDown1);
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

void FF_KeenPauseThink(CK_object *ob)
{
#ifdef KEEN5
	// If we're riding a platform, do it surfin' style!
	if (ob->topTI == PLATFORMEDGE)
	{
		// But only if such an action exists in this episode. :/
		if (CK_ACTION(CK_ACT_keenRidePlatform))
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenRidePlatform);
		}
	}
#endif

	if (ck_inputFrame.xDirection || ck_inputFrame.yDirection || ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed )
	{
		ob->user1 = ob->user2 = 0;	//Idle Time + Idle State
		ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
		FF_KeenStandThink(ob);

		return;
	}

	// K1n9_Duk3 addition:
	if (ff_map.nisasi)
		return;	// no idle animations when Keen has a Nisasi stuck to his face

	// If not on platform
	if ((ob->topTI & ~SLOPEMASK) != (PLATFORMEDGE & ~SLOPEMASK))
		ob->user1 += SD_GetSpriteSync();

	if (ob->user2 == 0 && ob->user1 > 200)
	{
		ob->user2++;
		ob->currentAction = CK_ACTION(CK_ACT_keenIdle);
		ob->user1 = 0;
	}
	else if (ob->user2 == 1 && ob->user1 > 300)
	{
		ob->user2++;
		if (ff_map.moontile == 1)
		{
			FF_SpawnEfree(ob->posX, rf_scrollYUnit-42*16);	// K1n9_Duk3 addition
		//	ck_mapState.moontile = 2;	// don't spawn it again?
		}
		ob->currentAction = CK_ACTION(CK_ACT_keenBored1);
		ob->user1 = 0;
	}
	else if (ob->user2 == 2 && ob->user1 > 600)
	{
		ob->user2++;
		ob->user1 = 0;
		if (!ff_map.inwater)
			ob->currentAction = CK_ACTION(FF_ACT_keenYawn1);
	}

}

//===========================================================================

/*
=======================
=
= KeenLookUpThink
=
=======================
*/

void FF_KeenLookUpThink(CK_object *ob)
{
	if (ck_inputFrame.yDirection != -1 ||
			ck_inputFrame.xDirection != 0 ||
			(ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed) ||
			(ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed) ||
			(ck_keenState.shootIsPressed))
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
		FF_KeenStandThink(ob);
	}
}

//===========================================================================

/*
=======================
=
= KeenLookDownThink
=
=======================
*/

void FF_KeenLookDownThink(CK_object *ob)
{
	int16_t tile1, tile2;
	uint16_t deltay;

	//Try to jump down
	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed
		&& (ob->topTI & SLOPEMASK) == 1)
	{
		ck_keenState.jumpWasPressed = true;

		// cannot drop down into falling blocks:
		if (ck_keenState.platform && ck_keenState.platform->type == FF_CT_FallBlock)
			return;

		// If the tiles below the player are blocking on any side but the top, they cannot be jumped through
		tile1 = FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2);
		tile2 = FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2+1);
		
		if (TI_ForeLeft(tile1) || TI_ForeBottom(tile1) || TI_ForeRight(tile1))
			return;

		if (TI_ForeLeft(tile2) || TI_ForeBottom(tile2) || TI_ForeRight(tile2))
			return;
	//	#define max(a,b) (((a)>(b))?(a):(b))

		deltay = max(SD_GetSpriteSync(),4) << 4;
	
		//Moving platforms
		if (ck_keenState.platform)
			deltay += ck_keenState.platform->deltaPosY;

		ck_keenState.platform = NULL;


		ob->clipRects.unitY2 += deltay;
		ob->posY += deltay;
		ck_nextX = 0;
		ck_nextY = 0;
		ob->currentAction = CK_ACTION(CK_ACT_keenFall1);
		ob->velX = ob->velY = 0;
		SD_PlaySound(FF_KEENFALLSND);
		return;
	}
	

	if (ck_inputFrame.yDirection != 1 || ck_inputFrame.xDirection != 0 || (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
		|| (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed))
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenLookDown4);
		return;
	}
}	

//===========================================================================

/*
============================
=
= KeenStandReact
=
============================
*/

void FF_KeenStandReact(CK_object *ob)
{
	if (!ob->topTI)
	{
		FF_RemoveDreamBubbles();	// just in case...
		SD_PlaySound(FF_KEENFALLSND);
		ob->velX = ob->xDirection * 8;
		ob->velY = 0;
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenFall1));
		ck_keenState.jumpTimer = 0;
	}
	else if ((ob->topTI & ~SLOPEMASK) == 8)
	{
		FF_KillKeen();
	}
#ifdef EXTRA_INTERACTIONS
	else if (ob->topTI == 0x29)
	{
		// Keen6 conveyor belt right
		ck_nextX = SD_GetSpriteSync() * 8;
		ck_nextY = 0;
		ob->user1 = 0;
		CK_PhysUpdateNormalObj(ob);
	}
	else if (ob->topTI == 0x31)
	{
		// Keen6 conveyor belt left
		ck_nextX = SD_GetSpriteSync() * -8;
		ck_nextY = 0;
		ob->user1 = 0;
		CK_PhysUpdateNormalObj(ob);
	}
#endif
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//===========================================================================

/*
============================
=
= KeenWalkReact
=
============================
*/

void FF_KeenWalkReact(CK_object *ob)
{
	if (!ob->topTI)
	{
		SD_PlaySound(FF_KEENFALLSND);
		ob->velX = ob->xDirection * 8;
		ob->velY = 0;
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenFall1));
		ck_keenState.jumpTimer = 0;
	}
	else if ((ob->topTI & ~SLOPEMASK) == 8)
	{
		FF_KillKeen();
	}
	else	// K1n9_Duk3 mod: make sure the wall check is not executed when already falling or dead
	{
#ifdef EXTRA_INTERACTIONS
		if (ob->topTI == 0x29)
		{
			// Keen6 conveyor belt right
			ck_nextX = SD_GetSpriteSync() * 8;
			ck_nextY = 0;
			ob->user1 = 0;
			CK_PhysUpdateNormalObj(ob);
		}
		else if (ob->topTI == 0x31)
		{
			// Keen6 conveyor belt left
			ck_nextX = SD_GetSpriteSync() * -8;
			ck_nextY = 0;
			ob->user1 = 0;
			CK_PhysUpdateNormalObj(ob);
		}
#endif
		// stop running if Keen hit a wall:
		if ((ob->rightTI && ob->xDirection == -1) || (ob->leftTI && ob->xDirection == 1))
		{
			ob->timeUntillThink = 0;
			ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
			ob->gfxChunk = (ob->xDirection == -1) ? ob->currentAction->chunkLeft : ob->currentAction->chunkRight;
		}
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

/*
=============================================================================

                            KEEN'S DREAMS

=============================================================================
*/

/*
============================
=
= UncacheDreamSprite		(K1n9_Duk3 addition)
=
============================
*/

static void  FF_UncacheDreamSprite(CK_object *ob)
{
	if (ob->gfxChunk > 0)
		;	// chunk is no longer needed
}

/*
============================
=
= RemoveDreamBubbles		(K1n9_Duk3 addition)
=
============================
*/

void FF_RemoveDreamBubbles(void)
{
	 CK_object *ob;

	// remove all dream bubbles from the level:
	for (ob = ck_keenObj; ob; ob = ob->next)
	{
		if (ob->type == FF_CT_DreamBubble)
		{
			FF_UncacheDreamSprite(ob);
			CK_RemoveObj(ob);
		}
	}
}

/*
============================
=
= SpawnDreamBubble		(K1n9_Duk3 addition)
=
============================
*/

void FF_SpawnDreamBubble(uint16_t x, uint16_t y, uint16_t type)
{
	CK_object *new_object = CK_GetNewObj(true);
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->type = FF_CT_DreamBubble;
	new_object->posX = x;
	new_object->posY = y;
	new_object->user3 = type;
	new_object->clipped = CLIP_not;
	new_object->gfxChunk = FF_DREAM3SPR;	// just to set visibility rectangle
	CK_SetAction(new_object, CK_ACTION(FF_ACT_dreamBubble));
	new_object->gfxChunk = 0;	// actual sprite
	if (!type)
		new_object->user1 = (1.5*70);	// Note: TickBase is a constant, so the floating point multiplication is only done at compile time
}

/*
============================
=
= T_KeenStartDream		(K1n9_Duk3 addition)
=
============================
*/

void FF_T_KeenStartDream(CK_object *ob)
{
	FF_SpawnDreamBubble(ob->posX + 16*16, ob->posY - 11*16, 0);
	CK_SetAction2(ob, ob->currentAction->next);
}

/*
============================
=
= T_KeenDream		(K1n9_Duk3 addition)
=
============================
*/

void FF_T_KeenDream(CK_object *ob)
{
	// check if we got any player input:
	if (ck_inputFrame.dir != IN_dir_None || ck_keenState.jumpIsPressed 
		|| ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed)
	{
		// stop dreaming:
		ob->currentAction = CK_ACTION(FF_ACT_keenGetUp);
		ob->user1 = ob->user2 = 0;

		FF_RemoveDreamBubbles();
	}
}

/*
============================
=
= T_DreamBubble		(K1n9_Duk3 addition)
=
============================
*/

void FF_T_DreamBubble(CK_object *ob)
{
	 int16_t randnum;

	if (!ob->user3)
	{
		// move when Keen is moved:
		ck_nextX = ck_keenObj->posX + 16*16 - ob->posX;
		ck_nextY = ck_keenObj->posY - 11*16 - ob->posY;
	}

	if (ob->user1 > SD_GetSpriteSync())
	{
		ob->user1 -= SD_GetSpriteSync();

		// if Smirky can appear and there are only 3 seconds left,
		// we have a 50% chance that it will appear
		if (ob->user2 && ob->user1 <= 3*70)
		{
			if (US_RndT() <= 127)
			{
				// we're going to change the sprite, so un-cache the current sprite
				FF_UncacheDreamSprite(ob);

				ob->gfxChunk = FF_DREAMSMIRKYSPR;
			}
			ob->user2 = false;	// always clear the "Smirky can appear" flag
		}
	}
	else
	{
		// we're going to change the sprite, so un-cache the current sprite
		FF_UncacheDreamSprite(ob);

		// animation:
		switch (ob->gfxChunk)
		{
		case 0:
			ob->gfxChunk = FF_DREAM1SPR;
			ob->user1 = 7;	// 0.1 seconds
			break;
		case FF_DREAM1SPR:
			ob->gfxChunk++;
			ob->user1 = 7;	// 0.1 seconds
			break;
		case FF_DREAM2SPR:
			if (ob->user3)
			{
				// council page's dream bubble:
				if (ck_gameState.currentLevel == MAP_ACMELABLEVEL)
				{	
					ob->gfxChunk = (ob->user3 & 0x7FFF) + FF_DREAMPAGE2SPR - 1;
				}
				else if (ck_gameState.currentLevel == MAP_SKOVLEVEL)
				{	
					ob->gfxChunk = (ob->user3 & 0x7FFF) + FF_DREAMPAGE1SPR - 1;
				}
				else if (ck_gameState.currentLevel == MAP_TARDISLEVEL)
				{	
					ob->gfxChunk = FF_DREAMBANANASPR;
				}				
				ob->user3++;
			}

			else
			{
				// Keen's dream bubble:
				randnum = US_RndT();
retry:
				if (ob->user4 < 0)	// -1 means show Nisasi dream bubble
				{
					ob->gfxChunk++;	// use Nisasi sprite (follows directly after DREAM2SPR)
					ob->user4 = 0;
				}
			/*
				else if (!randnum)
					ob->gfxChunk++;
			*/
				else if (randnum <= 51)
					ob->gfxChunk = FF_DREAMBWBSPR;
				else if (randnum <= 89)
					ob->gfxChunk = FF_DREAMTEDDYSPR;
				else if (randnum <= 127)
					ob->gfxChunk = FF_DREAMYORPSPR;
				else if (randnum <= 165)
					ob->gfxChunk = FF_DREAMROBOREDSPR;
				else if (randnum <= 191)
					ob->gfxChunk = FF_DREAMICESPR, ob->user2 = true;	// Smirky can appear after 4 seconds
				else if (randnum <= 204)
					ob->gfxChunk = FF_DREAMDREAMSPR;
				else if (randnum <= 216)
					ob->gfxChunk = FF_DREAMSMIRKYSPR;
				else if (randnum <= 229)
					ob->gfxChunk = FF_DREAMBANANASPR, ob->user2 = true;	// Smirky can appear after 4 seconds
				else if (randnum <= 232)
					ob->gfxChunk = FF_DREAMMOONSPR;
				else
					ob->gfxChunk = FF_DREAMDOPEFISHSPR;
				//
				// never show the same dream twice in a row:
				//
				if (ob->gfxChunk == ob->user4)
				{
					ob->user2 = false;	// clear Smirky flag, just in case
					randnum = (randnum+3) & 0xFF;
					goto retry;
				}
				ob->user4 = ob->gfxChunk;
			
			// Nisaba patch: Keen dreams of specific things in certain levels
				if (ck_gameState.currentLevel == MAP_DOPEFISHLEVEL) 
				{
					ob->user2 = false;
					ob->gfxChunk = FF_DREAMDOPEFISHSPR;
				}
				else if (ck_gameState.currentLevel == MAP_SKOVLEVEL) 
				{
					ob->user2 = false;
					ob->gfxChunk = FF_DREAMPAGE1SPR;
				}
				else if (ck_gameState.currentLevel == MAP_ACMELABLEVEL) 
				{
					ob->user2 = false;
					ob->gfxChunk = FF_DREAMPAGE2SPR;	
				}
				else if (ck_gameState.currentLevel == MAP_VOOL) 
				{
					ob->user2 = false;
					ob->gfxChunk = FF_DREAMDREAMSPR;
				}
			}
			
			ob->user1 = 4*70;	// 4 seconds
			break;
		default:
			if (ob->user4 < 0)	// -1 means show Nisasi dream bubble
			{
				ob->gfxChunk = FF_DREAM1SPR;
				ob->user1 = 7;
			}
			else
			{
				if (ob->user3)	// council page dream
				{
					// abort after the last dream sprite (avoid caching issues)
					if (ob->gfxChunk == FF_DREAM_LUMP_END || (ob->user3 & 0x8000))
					{
						CK_RemoveObj(ob);
						break;
					}
				}
				ob->gfxChunk = 0;
				ob->user1 = 70;	// 1 second
			}
			break;
		}
	}
}

/*
============================
=
= R_AutoCache		(K1n9_Duk3 addition)
=
============================
*/

void FF_R_AutoCache(CK_object *ob)
{
	// auto-cache the current sprite if it isn't cached already:
	if (ob->gfxChunk > 0)
	{
		if (!ca_graphChunks[ob->gfxChunk] || !(ca_graphChunkNeeded[ob->gfxChunk] & ca_levelbit))
			CA_CacheGrChunk(ob->gfxChunk);
	}

	FF_R_Draw(ob);
}

//-----------------------------------------------------------------------------
// end of Keen's Dreams
//-----------------------------------------------------------------------------

//===========================================================================

/*
=======================
=
= KeenReadThink
=
=======================
*/

/*
void KeenReadThink(objtype *ob)
{
	if (ck_inputFrame.xDirection != 0 || ck_keenState.jumpIsPressed || ck_keenState.pogoIsPressed)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenStowBook1);
		ob->user1 = ob->user2 = 0;
	}
}
*/

//===========================================================================

/*
=======================
=
= KeenAirThink
=
=======================
*/

void FF_KeenAirThink(CK_object *ob)
{
	// safety check: don't apply the tics again if we already have movement speed
	if (ck_nextX || ck_nextY)
		return;

	// Handle Jump Cheat
	if (ck_gameState.jumpCheat && ck_keenState.jumpIsPressed)
	{
		ob->velY = -40;
		ck_keenState.jumpTimer = 18;
		ck_keenState.jumpWasPressed = true;
	}	 

	if (ck_keenState.jumpTimer)
	{
		if (ck_keenState.jumpTimer <= SD_GetSpriteSync())
		{
			ck_nextY = ob->velY * ck_keenState.jumpTimer;
			ck_keenState.jumpTimer = 0;
		}
		else
		{
			ck_nextY = ob->velY * SD_GetSpriteSync();
			if (!ck_gameState.jumpCheat)
			{
				ck_keenState.jumpTimer -= SD_GetSpriteSync();
			}
		}

		// Stop moving up if we've let go of control.
		if (!ck_keenState.jumpIsPressed)
		{
			ck_keenState.jumpTimer = 0;
		}

		if (!ck_keenState.jumpTimer && ob->currentAction->next)
		{
			ob->currentAction = ob->currentAction->next;
		}
	}
	else
	{
		if (ck_gameState.difficulty == D_Easy)
		{
			// Nisaba patch: increase the gravity value a little bit for the Easy 
			// difficutly mode
			
			//DoWeakGravity(ob);
			FF_DoEasyKeenGravity(ob);
		}
		else	// Normal or Hard
		{
			FF_DoGravity(ob);
		}

		if (ob->velY > 0 && ob->currentAction != CK_ACTION(CK_ACT_keenFall1) && ob->currentAction != CK_ACTION(CK_ACT_keenFall2))
		{
			ob->currentAction = ob->currentAction->next;
		}
	}


	// Move horizontally
	if (ck_inputFrame.xDirection)
	{

		ob->xDirection = ck_inputFrame.xDirection;
		FF_AccelerateX(ob, ck_inputFrame.xDirection*2, 24);

#if (MINTICS < 2)		// K1n9_Duk3 fix for 70 fps:
		if (!ck_nextX && SD_GetSpriteSync() < 2)
			ck_nextX = ck_inputFrame.xDirection;
		// This makes sure that Keen always moves by at least 1 unit while
		// jumping and holding down the left or right key, so that Keen can
		// grab edges reliably. AccelerateX only accelerates every other
		// tic and hitting a wall while jumping always sets xspeed to 0. The
		// original Keen games ran at 35 fps max (2 tics per frame), so the
		// xtry value would almost never be 0 here. Keen can only grab
		// edges if the hitwest or hiteast values are set correctly, which
		// requires the object to move by at least 1 unit along the x axis.
#endif
	}
	else FF_FrictionX(ob);

	// Pole
	if (ob->bottomTI == 17)
	{
		ck_nextX = 0;
		ob->velX = 0;
	}

	// Shooting
	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		switch (ck_inputFrame.yDirection)
		{
		case -1:
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShootUp1);
			break;
		case 0:
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShoot1);
			break;
		case 1:
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShootDown1);
			break;
		}
		return;
	}

	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		ob->currentAction = CK_ACTION(CK_ACT_keenPogo2);
		ck_keenState.jumpTimer = 0;
		return;
	}

	if (ck_inputFrame.yDirection == -1)
	{
		FF_CheckGrabPole(ob);
	}
	
}

//===========================================================================

/*
============================
=
= KeenAirReact
=
============================
*/

void FF_KeenAirReact(CK_object *ob)
{
	// K1n9_Duk3 addition:
	if (ob->velY < 0)	// if moving up
	{
		if (ob->topTI != PLATFORMEDGE)	// if has NOT hit a platform object
			ob->topTI = 0;	// don't land
	}
	else if (ob->velY > 0)	// if moving down
	{
		ob->bottomTI = 0;	// don't hit ceiling
	}
	// end of addition
	
	if (ob->rightTI && ob->xDirection == -1)
	{
		ob->velX = 0;
	}
	else if (ob->leftTI && ob->xDirection == 1)
	{
		ob->velX = 0;
	}

	// Did we hit our head on the ceiling?
	if (ob->bottomTI)
	{
		// Allow Keen to jump into pole holes in the ceiling
		if (ob->bottomTI == 17)	// Pole
		{
#if 0
			// old code:
			ob->posY -= 32;
			ob->clipRects.unitY1 -= 32;
			ob->velX = 0;
			ob->posX = RF_TileToUnit(ob->clipRects.tileXmid) - 32;
#else
			// new code:
			FF_MoveObjVert(ob, -2*16);
			ob->velX = 0;
			FF_MoveObjHoriz(ob, RF_TileToUnit(ob->clipRects.tileXmid) - ob->clipRects.unitX1);
#endif
		}
		else if (!ck_gameState.jumpCheat)
		{
			SD_PlaySound(FF_KEENHITCEILINGSND);
			if (ob->bottomTI > 1)
			{
				ob->velY += 16;
				if (ob->velY < 0)
					ob->velY = 0;
			}
			else
			{
				ob->velY = 0;
			}
			ck_keenState.jumpTimer = 0;
		}
	}

	// Have we landed?
	if (ob->topTI)
	{
		ob->yDirection = IN_motion_Down;	// K1n9_Duk3 fix: avoid door glitch
		ob->deltaPosY = 0;
		// Check if deadly.
		if ((ob->topTI & ~SLOPEMASK) == 8)
		{
			FF_KillKeen();
		}
		else
		{
#ifdef KEEN5
			if (ob->topTI == 0x39) // Fuse
			{
				SD_PlaySound(KEENLANDONFUSESND);
			}
#endif
			if (ob->topTI != PLATFORMEDGE || !ck_keenState.jumpTimer) // Or standing on a platform.
			{
				ob->velY = ob->user1 = ob->user2 = 0;	// Being on the ground is boring. // K1n9_Duk3 mod: also set yspeed to 0
	
				// K1n9_Duk3 mod: use correct timing and also handle second shooting state
				if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShoot1))
				{
					int16_t temp = ob->actionTimer;
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenShoot1));
					ob->actionTimer = temp;
				}
				else if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShootUp1))
				{
					int16_t temp = ob->actionTimer;
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenShootUp1));
					ob->actionTimer = temp;
				}
				else if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShootUp2))
				{
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenShootUp1));
					ob->actionTimer = ob->currentAction->timer-1;
				}
				else if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShoot2))
				{
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenShoot1));
					ob->actionTimer = ob->currentAction->timer-1;
				}
				else if (ck_inputFrame.xDirection)
				{
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenRun1));
				}
				else
				{
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenStanding));
				}
				SD_PlaySound(FF_KEENLANDSND);
			}
		}
	}
	
	else if (ob->deltaPosY > 0)
	{
		// temp6 = Keen's previous upper y coord
		uint16_t temp6 = ob->clipRects.unitY1 - ob->deltaPosY;
		// temp8 = Keen's current upper y coord - 1.5 tiles, rounded to nearest tile, + 1.5 tiles
		uint16_t temp8 = ((ob->clipRects.unitY1 - 64) & 0xFF00) + 64;
		// temp10 = temp8 in tile coords, - 1
		int16_t temp10 = ((uint16_t)temp8 >> G_T_SHIFT) - 1 ;

		// If we're moving past a tile boundary.
		if (temp6 < temp8 && ob->clipRects.unitY1 >= temp8 && !ff_map.nisasi)	// K1n9_Duk3 mod: don't grab with Nisasi
		{
			// Moving left...
			if (ck_inputFrame.xDirection == -1 && (ob->clipRects.unitX1 & 0xFF) == 0)	// if left edge of hitbox is aligned on a tile boundary
			{
				int16_t tileX = ob->clipRects.tileX1 - ((ob->rightTI)?1:0);
				int16_t tileY = temp10;
				//VL_ScreenRect((tileX << 4) - (originxglobal >> 4), (tileY << 4) - (originyglobal >> 4), 16, 16, 1);
				int16_t upperTile = FORESPOT(tileX, tileY);
				int16_t lowerTile = FORESPOT(tileX, tileY+1);
				if ( (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile)) &
					TI_ForeRight(lowerTile) && TI_ForeTop(lowerTile))
				{
					ob->xDirection = -1;
					ob->clipped = CLIP_not;
					ob->posX = (ob->posX & 0xFF00) + 128;
					ob->posY = (temp8 - 64);
					ob->velY = ob->deltaPosY = 0;
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenHang1));
				}
			}
			else if (ck_inputFrame.xDirection == 1 && (ob->clipRects.unitX2 & 0xFF) == 0xFF)	// if right edge of hitbox is aligned on a tile boundary
			{
				int16_t tileX = ob->clipRects.tileX2 + ((ob->leftTI)?1:0);
				int16_t tileY = temp10;
				int16_t upperTile = FORESPOT(tileX, tileY);
				int16_t lowerTile = FORESPOT(tileX, tileY+1);

				if (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile) &
					TI_ForeLeft(lowerTile) && TI_ForeTop(lowerTile))
				{
					ob->xDirection = 1;
					ob->clipped = CLIP_not;
					ob->posX = (ob->posX & 0xFF00) + 256;
					ob->posY = (temp8 - 64);
					ob->velY = ob->deltaPosY = 0;
					CK_SetAction2(ob, CK_ACTION(CK_ACT_keenHang1));
				}
			}
		}
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//===========================================================================

/*
=======================
=
= KeenBounceThink
=
= Gives an extra bit of height on the first pogo bounce and creates
= the "impossible pogo trick" when the jump key is held down
=
=======================
*/

void FF_KeenBounceThink(CK_object *ob)
{
	ob->velY = -48;
	ck_nextY = 6 * ob->velY;
	ck_keenState.jumpTimer = ff_map.inwater? 0 : 24;
	SD_PlaySound(FF_KEENPOGOSND);
}

//===========================================================================

/*
=======================
=
= KeenPogoThink
=
=======================
*/

void FF_KeenPogoThink(CK_object *ob)
{
	// safety check: don't apply the tics again if we already have movement speed
	if (ck_nextX || ck_nextY)
		return;

	if (!ck_keenState.jumpTimer)
	{
		if (ck_gameState.difficulty == D_Easy)
		{
			// Nisaba patch: increase the gravity value a little bit for the Easy difficutly mode
		//	DoWeakGravity(ob);
			FF_DoEasyKeenGravity(ob);
		}
		else
		{
			FF_DoGravity(ob);
		}
	}
	else
	{
		if (ck_keenState.jumpIsPressed || ck_keenState.jumpTimer <= 9) FF_DoTinyGravity(ob);
	//	if (ck_keenState.jumpIsPressed || ck_keenState.jumpTimer <= 9) ytry += ob->velY * tics;
		else FF_DoGravity(ob);

		if (ck_keenState.jumpTimer <= SD_GetSpriteSync()) ck_keenState.jumpTimer = 0;
		else ck_keenState.jumpTimer -= SD_GetSpriteSync();

		if (!ck_keenState.jumpTimer && ob->currentAction->next) ob->currentAction = ob->currentAction->next;
	}
	if (ck_inputFrame.xDirection)
	{
		if (!ob->velX)
			ob->xDirection = ck_inputFrame.xDirection;
		FF_AccelerateX(ob, ck_inputFrame.xDirection, 24);
	}
	else
	{
		ck_nextX += ob->velX * SD_GetSpriteSync();
		if (ob->velX < 0) ob->xDirection = -1;
		else if (ob->velX > 0) ob->xDirection = 1;
	}

	// Stop for poles?
	if (ob->bottomTI == 17)
	{
		ck_nextX = 0;
		ob->velX = 0;
	}

	//Shooting
	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;
		switch (ck_inputFrame.yDirection)
		{
		case -1:
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShootUp1);
			return;
		case 0:
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShoot1);
			return;
		case 1:
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShootDown1);
			return;
		}
	}
	
	//Stop pogoing if Alt pressed
	if (ck_keenState.pogoIsPressed && !ck_keenState.pogoWasPressed)
	{
		ck_keenState.pogoWasPressed = true;
		ob->currentAction = CK_ACTION(CK_ACT_keenFall1);
	}
}

//===========================================================================

/*
=======================
=
= CK_KeenToggleTrapDoor		(K1n9_Duk3 addition)
=
=======================
*/

void FF_KeenToggleTrapDoor(int16_t x, int16_t y)
{
/*
	//
	// old fuse-breaking code from Keen 5
	//
	unsigned brokenFuseTiles[] = {0, 0};

	CK5_SpawnFuseExplosion(x,y);
	if (!(--ff_state.fusesRemaining))
	{
		CK5_SpawnLevelEnd();
	}

	
	brokenFuseTiles[0] = FORESPOT(0,0);
	brokenFuseTiles[1] = FORESPOT(0,1);

	RF_ReplaceTiles(brokenFuseTiles, 1, x, y, 1, 2);
*/

	uint16_t ix, iy, i;
	uint16_t trapdoorTiles[8];
	uint16_t mapindex = ((y) * CA_GetMapWidth()) + x;

	if (TI_ForeTop(FF_MapAt(1, mapindex+CA_GetMapWidth())) == TI_TOP_BREAKABLE)
	{
		mapindex--;
		x--;
	}

	i = 0;
	for (iy=0; iy<2; iy++, mapindex+=CA_GetMapWidth())
	{
		for (ix=0; ix<4; ix++)
		{
			 uint16_t tile = FF_MapAt(1, mapindex+ix);
			trapdoorTiles[i++] = tile + TI_ForeAnimTile(tile);
		}
	}

	RF_ReplaceTiles(trapdoorTiles, 1, x, y, 4, 2);
}

//===========================================================================

/*
============================
=
= KeenPogoReact
=
============================
*/

void FF_KeenPogoReact(CK_object *ob)
{
	// K1n9_Duk3 addition:
	if (ob->velY < 0)	// if moving up
	{
		if (ob->topTI != PLATFORMEDGE)	// if has NOT hit a platform object
			ob->topTI = 0;	// don't land
	}
	else if (ob->velY > 0)	// if moving down
	{
		ob->bottomTI = 0;	// don't hit ceiling
	}
	// end of addition
	
	if (ob->rightTI && ob->xDirection == -1)
	{
		ob->velX = 0;
	}
	else if (ob->leftTI && ob->xDirection == 1)
	{
		ob->velX = 0;
	}

	if (ob->bottomTI)
	{
		if (ob->bottomTI == 17)	//Pole
		{
#if 0
			// old code:
			ob->posY -= 32;
			ob->clipRects.unitY1 -= 32;
			ob->velX = 0;
			ob->posX = RF_TileToUnit(ob->clipRects.tileXmid) - 32;
#else
			// new code:
			FF_MoveObjVert(ob, -2*16);
			ob->velX = 0;
			FF_MoveObjHoriz(ob, RF_TileToUnit(ob->clipRects.tileXmid) - ob->clipRects.unitX1);
#endif
		}
		else if (!ck_gameState.jumpCheat)
		{
			SD_PlaySound(FF_KEENHITCEILINGSND);

			if (ob->bottomTI > 1)
			{
				ob->velY += 16;
				if (ob->velY < 0) ob->velY = 0;
			}
			else ob->velY = 0;
		
			ck_keenState.jumpTimer = 0;
		}
	}

	// Houston, we've landed!
	if (ob->topTI)
	{
		ob->yDirection = IN_motion_Down;	// K1n9_Duk3 fix: avoid door glitch
		ob->deltaPosY = 0;
		// Check if deadly.
		if ((ob->topTI & ~SLOPEMASK) == 8)
		{
			FF_KillKeen();
		}
		else
		{
			if (ob->topTI == TI_TOP_BREAKABLE) // Trap door in FITF
			{
				if (ck_gameState.currentLevel == MAP_DOPEFISHLEVEL)	// Nisaba patch: fuse has no effect in level 11
				{
				}
				else if (ob->velY >= 0x30)
				{
					SD_PlaySound(FF_ROOTSSND);
					FF_KeenToggleTrapDoor(ob->clipRects.tileXmid-1, ob->clipRects.tileY2);
					if (!TI_ForeTop(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2+1)))
					{
						// ground is no longer solid, so don't bounce
						RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
						return;
					}
				}
				else
				{
#ifdef KEEN5
					SD_PlaySound(KEENLANDONFUSESND);
#endif
				}
			}
			if (ob->topTI != PLATFORMEDGE || ck_keenState.jumpTimer == 0)
			{
				ob->velY = -48;
				ck_keenState.jumpTimer = ff_map.inwater? 0 : 24;
				SD_PlaySound(FF_KEENPOGOSND);
				CK_SetAction2(ob, CK_ACTION(CK_ACT_keenPogo2));
			}
		}
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//===========================================================================

/*
============================
=
= KeenPosContact
=
============================
*/

void FF_KeenPosContact(CK_object *ob, CK_object *other)
{
	// K1n9_Duk3 mod: using switch instead of if/else if
	switch (other->type)
	{
	case CT5_Item:	// K1n9_Duk3 addition: also pick up object-based items while on pole
		FF_KeenContact(ob, other);
		break;

	case FF_CT_FallBlock:
		ob->zLayer = KEENPRIORITY;
		ob->clipped = CLIP_normal;
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenFall1));
		ck_keenState.jumpTimer = ob->velX = ob->velY = 0;
		CK_PhysPushXY(ob, other, true);
		break;

#if 0	// touching platforms shouldn't cause Keen to fall off of poles or edges
	case CT5_Platform:
	case FF_CT_LogPlatform:	// K1n9_Duk3 mod: added log platform & cloud
	case FF_CT_Cloud:
		ob->clipped = CLIP_normal;
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenFall1));
		ck_keenState.jumpTimer = ob->velX = ob->velY = 0;
		CK_PhysPushY(ob, other);
		break;
#endif
#if 0
	case CT_Ampton:
	case CT_Korath:
		ob->zLayer = KEENPRIORITY;
		ob->clipped = CLIP_normal;
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenFall1));
		ck_keenState.jumpTimer = ob->velX = ob->velY = 0;
		break;
#endif
	}

}

/*
============================
=
= KeenSimpleReact
=
============================
*/

void FF_KeenSimpleReact(CK_object *ob)
{
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//===========================================================================

/*
=======================
=
= KeenHoldThink
=
=======================
*/

void FF_KeenHoldThink(CK_object *ob)
{
	if (ck_inputFrame.yDirection == -1 || ck_inputFrame.xDirection == ob->xDirection)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPull1);

		if(ob->xDirection == 1)
		{
			ck_nextY = -16*16;
		}
		else
		{
			ck_nextY = -8*16;
		}
		
		// K1n9_Duk3 note: Keen 4-6 have additional code to switch Keen's sprite
		// to foreground priority here, depending on whether the tiles around him
		// are foreground tiles or not. This was missing in the original CKSRCMOD
		// code and I haven't noticed any bugs in FITF's levels, so I guess we
		// don't really need it anyway.
		
	}
	else if (ck_inputFrame.yDirection == 1 || (ck_inputFrame.xDirection && ck_inputFrame.xDirection != ob->xDirection))
	{
		// Drop down.
		ob->currentAction = CK_ACTION(CK_ACT_keenFall1);
		ob->clipped = CLIP_normal;
	}
}

//===========================================================================

/*
=======================
=
= T_PullUp1
=
=======================
*/

void FF_T_PullUp1(CK_object *ob)
{
	if (ob->xDirection == 1)
		ck_nextX = 128;
	else
		ck_nextY = -128;
}

/*
=======================
=
= T_PullUp2
=
=======================
*/

void FF_T_PullUp2(CK_object *ob)
{
	ck_nextX = ob->xDirection * 128;
	ck_nextY = -128;
}

/*
=======================
=
= T_PullUp3
=
=======================
*/

void FF_T_PullUp3(CK_object *ob)
{
	ck_nextY = -128;
}

/*
=======================
=
= T_PulledUp
=
=======================
*/

void FF_T_PulledUp(CK_object *ob)
{
	ob->clipped = CLIP_normal;
	ob->zLayer = KEENPRIORITY;
	ck_nextY = 128;
}


//===========================================================================

/*
=======================
=
= KeenDieThink
=
=======================
*/

void FF_KeenDieThink(CK_object *ob)
{
	FF_DoWeakGravity(ob);
	ck_nextX = ob->velX * SD_GetSpriteSync();
	if (!CK_ObjectVisible(ob))
	{
		ck_gameState.levelState = ex_died;	// K1n9_Duk3 fix: use enum name instead of number
	}
}

/*
============================
=
= KillKeen
=
============================
*/

void FF_KillKeen(void)
{
	FF_KillKeenEx(false);
}

/*
============================
=
= KillKeenEx
=
============================
*/

void FF_KillKeenEx(bool force)
{
	CK_object *ob = ck_keenObj;

	// K1n9_Duk3 addition: force game to kill Keen (tiles are ALWAYS deadly)
	if (force)
		force = !FF_KeenIsDead();

	if (ff_map.invincibilityTimer && !force)
	{
		return;
	}

	if (ck_godMode)
	{
		return;
	}

	// K1n9_Duk3 addition: don't change state when Keen is dying a special death
	if ((ob->currentAction == CK_ACTION(FF_ACT_keenDrown) || ob->currentAction == CK_ACTION(FF_ACT_keenBurn))
		&& ob->type != CT_Player)
	{
		return;
	}

#if USER_REPORT
	if (!ff_keenKilled)
		FF_ReportWithScreenshot("killed", NULL);
#endif
	
	ff_map.invincibilityTimer = 30;
	ff_keenKilled = true;
	ob->clipped = CLIP_not;
	ob->zLayer = FOREGROUNDPRIORITY;
	if (US_RndT() < 0x80)
	{
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenDie0));
	}
	else
	{
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenDie1));
	}

	SD_PlaySound(FF_KEENDIESND);

	ob->velY = -40;
	ob->velX = 16;

	FF_RemoveDreamBubbles();	// K1n9_Duk3 addition
	FF_DropNisasi();
}

//===========================================================================

/*
=======================
=
= KeenIsOverPole		(K1n9_Duk3 addition)
=
=======================
*/

bool FF_KeenIsOverPole(CK_object *ob)
{
	uint16_t y = ob->clipRects.tileY1;
	uint16_t off = ob->clipRects.tileXmid + ((y) * CA_GetMapWidth());

	while (y <= ob->clipRects.tileY2)
	{
		if ((TI_ForeMisc(FF_MapAt(1, off)) & INTILE_TYPEMASK) == MF_Pole)
			return true;
		y++;
		off += CA_GetMapWidth();
	}
	return false;
}

//===========================================================================

/*
=======================
=
= PoleActions
=
=======================
*/

void FF_PoleActions(CK_object *ob)
{
	if (ck_inputFrame.xDirection)
		ob->xDirection = ck_inputFrame.xDirection;

	if (!FF_KeenIsOverPole(ob))
	{
		ob->velX = ob->velY = 0;
		ob->clipped = CLIP_normal;
		ob->currentAction = CK_ACTION(CK_ACT_keenJump1);
		ob->yDirection = 1;
		ck_keenState.poleGrabTime = SD_GetLastTimeCount();
		ck_keenState.jumpTimer = 0;
		SD_PlaySound(FF_KEENFALLSND);
		return;
	}

	//Shooting things. *ZAP!*
	if (ck_keenState.shootIsPressed && !ck_keenState.shootWasPressed)
	{
		ck_keenState.shootWasPressed = true;

		switch (ck_inputFrame.yDirection)
		{
		case -1:
			ob->currentAction = CK_ACTION(FF_ACT_keenPoleShootUp0);
			break;
		case 0:
			ob->currentAction = CK_ACTION(FF_ACT_keenPoleShoot0);
			break;
		case 1:
			ob->currentAction = CK_ACTION(FF_ACT_keenPoleShootDown0);
			break;
		}
	}

	
	if (ck_keenState.jumpIsPressed && !ck_keenState.jumpWasPressed)
	{
		ck_keenState.jumpWasPressed = true;
		SD_PlaySound(FF_KEENJUMPSND);
		ob->velX = ff_KeenPoleOffs[ck_inputFrame.xDirection+1];
		ob->velY = -20;
		ob->clipped = CLIP_normal;
		ck_keenState.jumpTimer = 10;
		ob->currentAction = CK_ACTION(CK_ACT_keenJump1);
		ob->yDirection = 1;
		ck_keenState.poleGrabTime = SD_GetLastTimeCount();
	}
	return;
}

/*
=======================
=
= KeenPoleThink
=
=======================
*/

void FF_KeenPoleThink(CK_object *ob)
{
	int16_t groundTile;

	if (ck_inputFrame.yDirection == 1)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleDown1);
		ob->yDirection = 1;
		FF_KeenDropThink(ob);
		return;
	}
	else if (ck_inputFrame.yDirection == -1)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleUp1);
		ob->yDirection = -1;
		return;
	}

	// When keen is at ground level, allow dismounting using left/right.
	if (ck_inputFrame.xDirection)
	{
		groundTile = FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2+1);
		if (TI_ForeTop(groundTile))
		{
			ck_keenState.jumpTimer = ob->velX = ob->velY = 0;
			ob->clipped = CLIP_normal;
			ob->currentAction = CK_ACTION(CK_ACT_keenFall1);
			ob->yDirection = 1;
			SD_PlaySound(FF_KEENFALLSND);
			return;
		}
	}

	FF_PoleActions(ob);
}

/*
=======================
=
= KeenClimbThink
=
=======================
*/

void FF_KeenClimbThink(CK_object *ob)
{
	int16_t topTile = FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY1);
	
	if ((TI_ForeMisc(topTile) & INTILE_TYPEMASK) != MF_Pole)
	{
		ck_nextY = 0;
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleSit);
		FF_PoleActions(ob);
		return;
	}

	if (ck_inputFrame.yDirection == 0)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleSit);
		ob->yDirection = 0;
	}
	else if (ck_inputFrame.yDirection == 1)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleDown1);
		ob->yDirection = 1;
		FF_KeenDropThink(ob);
	}

	FF_PoleActions(ob);
}

/*
=======================
=
= T_KeenPoleShoot		(K1n9_Duk3 addition)
=
= Checks if the pole has vanished and transitions Keen into the appropriate
= jump-shooting state if the pole is gone.
=
=======================
*/

void FF_T_KeenPoleShoot(CK_object *ob)
{
	if (!FF_KeenIsOverPole(ob))
	{
		ck_keenState.jumpTimer = ob->velX = ob->velY = 0;
		ob->clipped = CLIP_normal;
		ob->yDirection = 1;
		ck_keenState.poleGrabTime = SD_GetLastTimeCount();
		if (ob->currentAction == CK_ACTION(CK_ACT_keenPoleShoot1))
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShoot2);
		}
		else if (ob->currentAction == CK_ACTION(CK_ACT_keenPoleShootUp1))
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShootUp2);
		}
		else if (ob->currentAction == CK_ACTION(CK_ACT_keenPoleShootDown1))
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenJumpShootDown2);
		}
		else	// shot has already been spawned, so just make Keen fall off:
		{
			ob->currentAction = CK_ACTION(CK_ACT_keenJump1);
		}
	}
}

/*
=======================
=
= KeenDropThink
=
=======================
*/

void FF_KeenDropThink(CK_object *ob)
{

	int16_t tileUnderneath = FORESPOT(ob->clipRects.tileXmid, RF_UnitToTile(ob->clipRects.unitY2-4*16));

	if ((TI_ForeMisc(tileUnderneath) & INTILE_TYPEMASK) != MF_Pole)
	{
		// We're no longer on a pole.
		SD_PlaySound(FF_KEENFALLSND);
		ob->currentAction = CK_ACTION(CK_ACT_keenFall1);
		ck_keenState.jumpTimer = 0;
		ob->velX = ff_KeenPoleOffs[ck_inputFrame.xDirection + 1];
		ob->velY = 0;
		ob->clipped = CLIP_normal;
		ob->clipRects.tileY2 -= 1;	// allows ClipToWalls to push Keen out of the floor
		return;
	}

	if (ck_inputFrame.yDirection == 0)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleSit);
		ob->yDirection = 0;
	}
	else if (ck_inputFrame.yDirection == -1)
	{
		ob->currentAction = CK_ACTION(CK_ACT_keenPoleUp1);
		ob->yDirection = -1;
	}
	
	FF_PoleActions(ob);
}

/*
============================
=
= KeenPoleReact
=
============================
*/

void FF_KeenPoleReact(CK_object *ob)
{
	// Check if the bottom end of Keen's hit rectangle is inside a tile with a
	// flat top end (which means it is NOT a pole hole). If it is, it means
	// Keen has hit the floor below the pole and should let go.

	if (TI_ForeTop(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2)) == 1)
	{
		int16_t ymove = (ob->clipRects.unitY2 & 255) + 1;
		// move Keen out of the floor:
		ob->posY -= ymove;
		ob->clipRects.unitY2 -= ymove;	// adjust bottom end of hit rectangle
		ob->clipRects.tileY2--;   	// for the tile hit rect as well
		
		// and change into first look down state:
		ob->clipped = CLIP_normal;
		CK_SetAction2(ob, CK_ACTION(CK_ACT_keenLookDown1));
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

// Shooting

/*
======================
=
= SpawnShot
=
======================
*/

void FF_SpawnShot(int16_t x, int16_t y, IN_Direction dir)
{
	CK_object *shot, *hit;

	if (!ck_gameState.numShots)
	{
		SD_PlaySound(FF_CLICKSND);
		;
		return;
	}

	ck_gameState.numShots--;

	shot = CK_GetNewObj(true);
	shot->posX = x;
	shot->posY = y;
	shot->zLayer = 1;
	shot->type = CT_Stunner;
	shot->active = OBJ_ALWAYS_ACTIVE;

	SD_PlaySound(FF_KEENSHOOTSND);
	
	switch(dir)
	{
	case IN_dir_North:
		shot->xDirection = 0;
		shot->yDirection = -1;
		break;
	case IN_dir_East:
		shot->xDirection = 1;
		shot->yDirection = 0;
		break;
	case IN_dir_South:
		shot->xDirection = 0;
		shot->yDirection = 1;
		break;
	case IN_dir_West:
		shot->xDirection = -1;
		shot->yDirection = 0;
		break;
	default:
		//Quit("SpawnShot: Bad dir!");
		Quit("Foray in the Forest: BADSHOTDIR");
	}

	CK_SetAction(shot, CK_ACTION(CK_ACT_keenShot1));

#if 0
	//
	// KEEN 6 -- check if the shot hits an enemy:
	// (prevents enemies from killing Keen before they get stunned)
	//
	
	// K1n9_Duk3 note:
	// This doesn't help much. The shot and the enemies haven't moved yet at this
	// point, and collisions will be checked after they have moved. But the real
	// problem is that Keen is the first object in the object list and any
	// collisions with Keen are ALWAYS handled before handling collisions with
	// the shots in the original Keen 4-6 code. That means enemies that touch
	// Keen and a stunner shot at the same time will always get to kill Keen
	// before the shot can stun them. I fixed this problem in the collision code
	// in PlayLoop now (see CK_PLAY.C).
	
	for (hit=ck_keenObj->next; hit; hit = hit->next)
	{
		if (hit->currentAction->collide && ObjectsCollide(shot, hit))
		{
			hit->currentAction->collide(hit, shot);

			if (shot->type != CT_Stunner)
				break;	// shot has exploded
		}
	}
#endif
}

/*
======================
=
= ExplodeShot
=
======================
*/

void FF_ExplodeShot(CK_object *ob)
{
	ob->type = CT_Friendly;
	CK_SetAction2(ob, CK_ACTION(CK_ACT_keenShotHit1));
	SD_PlaySound(FF_KEENSHOTHITSND);
}

/*
======================
=
= T_Shot
=
======================
*/

void FF_T_Shot(CK_object *shot)
{
	CK_object *currentObj;

	// Stun things which are offscreen.
	if ((shot->clipRects.tileX2 < originxtile) ||
	    (shot->clipRects.tileY2 < originytile) ||
	    (shot->clipRects.tileX1 > originxtilemax) ||
	    (shot->clipRects.tileY1 > originytilemax))
	{
		if ((shot->clipRects.tileX2 + 10 < originxtile) ||
		    (shot->clipRects.tileX1 - 10 > originxtilemax) ||
		    (shot->clipRects.tileY2 + 6 < originytile) ||
		    (shot->clipRects.tileY1 - 6 > originytilemax))
		{
			// shot is off-screen by more than half a screen, so remove it
			// (shots are spawned as always active!)
			CK_RemoveObj(shot);
			return;
		}
		for (currentObj = ck_keenObj->next; currentObj; currentObj = currentObj->next)
		{
			if (!currentObj->active && ObjectsCollide(currentObj, shot))
			{
				if (currentObj->currentAction->collide)
				{
					currentObj->currentAction->collide(currentObj, shot);
					currentObj->visible = true;
					currentObj->active = OBJ_ACTIVE;
				}
				if (shot->type == CT_Nothing)
					return;
			}
		}
	}
}

/*
======================
=
= R_Shot
=
======================
*/

void FF_R_Shot(CK_object *ob)
{
	uint16_t t;

	// shoot down through a pole hole
	if (ob->topTI == 1 && ob->clipRects.tileX1 != ob->clipRects.tileX2) 
	{
		t = FORESPOT(ob->clipRects.tileX2, ob->clipRects.tileY1-1);
		if (TI_ForeTop(t) == 0x11) 
		{
			ob->topTI = 0x11;
			ob->posX += 0x100 - (ob->posX & 0xFF);
		}
	} 
	// move into pole hole before making contact
	else if (ob->topTI == 0x11 && ob->clipRects.tileX1 != ob->clipRects.tileX2) 
	{
		ob->posX &= 0xFF00;
	}

	// shoot through pole hole upwards
	if (ob->bottomTI == 1 && ob->clipRects.tileX1 != ob->clipRects.tileX2) 
	{
		t = FORESPOT(ob->clipRects.tileX2, ob->clipRects.tileY2+1);
		if (TI_ForeBottom(t) == 0x11) 
		{
			ob->bottomTI = 0x11;
			ob->posX += 0x100 - (ob->posX & 0xFF);
		}
	} 
	// move into pole hole whilst travelling upwards
	else if (ob->bottomTI == 0x11 && ob->clipRects.tileX1 != ob->clipRects.tileX2) 
	{
		ob->posX &= 0xFF00;
	}

	// if hit any other type of object, die
	if (ob->topTI != 0x11 && ob->bottomTI != 0x11) 
	{
		if (ob->topTI || ob->bottomTI || ob->rightTI || ob->leftTI) 
		{
			FF_ExplodeShot(ob);
		}
	} 
	else 
	// correct for pole hole passage
	{
		ck_nextY = ob->currentAction->velY * SD_GetSpriteSync() * ob->yDirection;
		ob->posY += ck_nextY;
		ob->clipRects.unitY1 += ck_nextY;
		ob->clipRects.unitY2 += ck_nextY;
		ob->clipRects.tileY1 = ob->clipRects.unitY1 >> G_T_SHIFT;
		ob->clipRects.tileY2 = ob->clipRects.unitY2 >> G_T_SHIFT;
	}
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//===========================================================================

/*
=======================
=
= KeenShootThink
=
=======================
*/

void FF_KeenShootThink(CK_object *ob)
{
	if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1)
	{
		SD_PlaySound(FF_KITTENEXPLODESND);
		VL_FadeFromBlack();
		ob->gfxChunk = FF_KEENBURNED1SPR;
		FF_R_AutoCache(ob);
		RF_Refresh();
		VL_FadeToBlack();
		ck_godMode = false;
		FF_BurnKeen(true);
		return;
	}
	
	if (ob->currentAction == CK_ACTION(CK_ACT_keenShoot1))
	{
		if (ob->xDirection == 1)
		{
			FF_SpawnShot(ob->posX + 256, ob->posY + 64, IN_dir_East);
		}
		else
		{
			FF_SpawnShot(ob->posX - 128, ob->posY + 64, IN_dir_West);
		}
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShoot2))
	{
		if (ob->xDirection == 1)
		{
			FF_SpawnShot(ob->posX + 256, ob->posY + 32, IN_dir_East);
		}
		else
		{
			FF_SpawnShot(ob->posX, ob->posY + 32, IN_dir_West);
		}
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShootDown2))
	{
		FF_SpawnShot(ob->posX + 128, ob->posY + 288, IN_dir_South);
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenJumpShootUp2))
	{
		FF_SpawnShot(ob->posX + 80, ob->posY - 160, IN_dir_North);
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenShootUp1))
	{
		FF_SpawnShot(ob->posX + 80, ob->posY - 160, IN_dir_North);
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenPoleShoot1))
	{
		if (ob->xDirection == 1)
		{
			FF_SpawnShot(ob->posX + 256, ob->posY + 64, IN_dir_East);
		}
		else
		{
			FF_SpawnShot(ob->posX - 128, ob->posY + 64, IN_dir_West);
		}
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenPoleShootUp1))
	{
		if (ob->xDirection == 1)
		{
			FF_SpawnShot(ob->posX + 96, ob->posY + 64, IN_dir_North);
		}
		else
		{
			FF_SpawnShot(ob->posX + 192, ob->posY + 64, IN_dir_North);
		}
	}
	if (ob->currentAction == CK_ACTION(CK_ACT_keenPoleShootDown1))
	{
		if (ob->xDirection == 1)
		{
			FF_SpawnShot(ob->posX + 96, ob->posY + 384, IN_dir_South);
		}
		else
		{
			FF_SpawnShot(ob->posX + 192, ob->posY + 384, IN_dir_South);
		}
	}
}

//===========================================================================

#if 1
//
// The following code was taken from an early version of SIGN OF THE VOOL and
// was used for reference when implementing the StunKeen code.
//

/*
// Knocks keen away from the coordinate passed as an argument
void CK_DamageKeen(unsigned x, unsigned y)
{
	if (ck_gameState.difficulty == D_Classic)
	{
		KillKeen();
	}
	else
	{
		int dx;

		if (ck_godMode || ck_mapState.invincibilityTimer > 0)
			return;

		if (--ff_state.health == 0)
		{
			KillKeen();
			return;
		}

		dx = x - (player->clipRects.unitX1/2 + player->clipRects.unitX2/2);

		ChangeState(player, CK_ACTION(FF_ACT_KeenKnockback1));
		SD_PlaySound(10); // TODO: need a sound!
		player->velX = dx > 0 ? -20 : 20;
		player->velY = -40;
		ck_mapState.invincibilityTimer =  INT_MAX - 1;  // Set again when keen hits the ground
	}
	
}

void CK_KeenKnockbackReact(objtype *ob)
{
	// Change stance when contacted the ground
	if (ob->topTI)
	{
		ck_mapState.invincibilityTimer = 70;
		ChangeState(ob, CK_ACTION(FF_ACT_KeenKnockbackRecover1));
	}

	if (ob->leftTI || ob->rightTI)
		ob->velX = 0;


	RF_PlaceSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
	
}
*/

//===========================================================================

/*
=======================
=
= StunKeen		(K1n9_Duk3 addition)
=
=======================
*/

#define STUN_COOLDOWN	(2*70)
#define RESTUN_DELAY	(70/5)

void FF_StunKeen(int16_t midx)
{
	// make sure Keen isn't dead:
	// (stunning a dead Keen would revive him in a glitched state!)
	if (!FF_KeenIsDead() && !ff_map.invincibilityTimer && !ck_godMode)
	{
		// play a sound here (if Keen isn't already stunned)
		if ( (ck_keenObj->currentAction != CK_ACTION(FF_ACT_keenStunned1)
			&& ck_keenObj->currentAction != CK_ACTION(FF_ACT_keenStunned2)
			&& ck_keenObj->currentAction != CK_ACTION(FF_ACT_keenStunned3))
		    || 	ff_map.stunCooldown <= STUN_COOLDOWN-RESTUN_DELAY )
		{
			;
			SD_PlaySound(FF_KEENSTUNNEDSND);
			ff_map.stunCooldown = STUN_COOLDOWN;

			if (++ff_map.stunCounter > 3)
			{
				FF_KillKeen();
				return;
			}
		}

		if (US_RndT() < 128)
			CK_SetAction2(ck_keenObj, CK_ACTION(FF_ACT_keenStunned1));
		else
			CK_SetAction2(ck_keenObj, CK_ACTION(FF_ACT_keenStunned2));

		// make Keen hop to the side:
		ck_keenObj->velX = midx > ck_keenObj->clipRects.unitXmid? -24 : 24;
		ck_keenObj->velY = -40;
		// avoid getting stuck on a platform:
		ck_keenState.platform = NULL;
		ck_keenObj->topTI = 0;	// so R_KeenStunned won't make Keen land immediately after this, effectively stopping any movement
		ck_keenObj->clipped = CLIP_normal;	// Keen might be in noclip-mode (climbing poles, grabbing edges), so we have to disable noclip here

		FF_RemoveDreamBubbles();	// because Keen might have been dreaming
		FF_DropNisasi();
	}
}

//===========================================================================

/*
=======================
=
= T_KeenStunnedRecover		(K1n9_Duk3 addition)
=
=======================
*/

void FF_T_KeenStunnedRecover(CK_object *ob)
{
	ob->user1 = ob->user2 = 0;	//Idle Time + Idle State
}

//===========================================================================

/*
=======================
=
= R_KeenStunned		(K1n9_Duk3 addition)
=
=======================
*/

void FF_R_KeenStunned(CK_object *ob)
{
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = 0;
	}
	if (ob->bottomTI)
			ob->velY = 0;

	if (ob->topTI)
	{
		ob->yDirection = IN_motion_Down;	// K1n9_Duk3 fix: avoid door glitch
		ob->velX = ob->velY = 0;
		CK_SetAction2(ck_keenObj, ck_keenObj->currentAction->next);
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//===========================================================================

/*
=======================
=
= KeenIsDead		(K1n9_Duk3 addition)
=
=======================
*/

bool FF_KeenIsDead(void)
{
	 CK_action *state = ck_keenObj->currentAction;

	return (state == CK_ACTION(CK_ACT_keenDie0)
		|| state == CK_ACTION(CK_ACT_keenDie1)
		|| state == CK_ACTION(FF_ACT_keenDrown)
		|| state == CK_ACTION(FF_ACT_keenBurn)
		|| state == CK_ACTION(FF_ACT_keenSink));
	// Note: If you implement other deaths, add any don't forget to add the new
	// death states in here! You could also check the keenkilled variable, but
	// that's unreliable because it might be reset if the player saves and loads
	// during the death animation.
}


//===========================================================================

/*
=======================
=
= DrownKeen		(K1n9_Duk3 addition)
=
=======================
*/

void FF_DrownKeen(int16_t tileY)
{
	 int16_t i;
	 CK_object *ob = ck_keenObj;
	uint16_t oldx = ob->posX, oldy = ob->posY;

	if (ck_godMode/* || ck_mapState.invincibilityTimer*/)
		return;

	;
	FF_RemoveDreamBubbles();	// just in case...
	FF_DropNisasi();

	FF_MoveObjVert(ob, RF_TileToUnit(tileY)-ob->clipRects.unitY2);
	FF_MoveObjHoriz(ob, -256);
	ob->clipped = CLIP_not;
	ob->gfxChunk = FF_NESSI1SPR;
	CK_SetAction2(ob, CK_ACTION(FF_ACT_keenDrown));
	if (!CK_NotStuckInWall(ob))
	{
		FF_MoveObjHoriz(ob, -(ob->clipRects.unitX1 & 0xFF));	// align left edge on a tile boundary
		for (i=0; i<=2*256; i+=256)
		{
			FF_MoveObjHoriz(ob, -i);
			if (CK_NotStuckInWall(ob))
				goto okay;
			FF_MoveObjHoriz(ob, 2*i);
			if (CK_NotStuckInWall(ob))
				goto okay;
			FF_MoveObjHoriz(ob, -i);
		}
die:
		// didn't find a good position for the Nessi sprite, so just use a normal death
		ob->posX = oldx;
		ob->posY = oldy;
		CK_ResetClipRects(ob);
		FF_KillKeen();
		return;
	}
okay:
#if 1
	// check if there's enough headroom for Nessi to rise
	FF_MoveObjVert(ob, -3*256);
	if (!CK_NotStuckInWall(ob))
		goto die;
	FF_MoveObjVert(ob, 3*256);
#endif
	SD_PlaySound(FF_DEADTHNESSISND);
	ff_keenKilled = true;
	ob->type = CT_Friendly;	// to make sure other objects don't interact with Keen
	ob->zLayer = ob->user1 = ob->user2 = 0;
	ob->user4 = RF_TileToUnit(tileY-2);
	ob->user3 = 70;
}

//===========================================================================

/*
=======================
=
= T_KeenDrown		(K1n9_Duk3 addition)
=
=======================
*/

void FF_T_KeenDrown(CK_object *ob)
{
#define NESSI_ANIM_DELAY 18
	if (ob->user3 > 0)
	{
		// delay before Nessi starts to rise
		ob->user3 -= SD_GetSpriteSync();
	}
	else if (!ob->user1)
	{
		// Nessi is rising
		ck_nextY = -10*SD_GetSpriteSync();
		if (ob->clipRects.unitY2 + ck_nextY <= ob->user4)
		{
			ob->user1++;
			ck_nextY = ob->user4 - ob->clipRects.unitY2;
		}
	}
	if (ob->user1 || ob->clipRects.unitY2 < ob->user4 + (uint16_t)(1.5*256))
	{
		// animate Nessi
		if ((ob->actionTimer += SD_GetSpriteSync()) >= NESSI_ANIM_DELAY)
		{
			if (ob->gfxChunk == FF_NESSI4SPR)
			{
				if (ob->actionTimer >= (3*NESSI_ANIM_DELAY)/2 && ob->user1)
					ck_nextY = 8*SD_GetSpriteSync();
				if (ob->actionTimer >= NESSI_ANIM_DELAY+70/2)
					ck_gameState.levelState = ex_died;
			}
			else
			{
				ob->actionTimer -= NESSI_ANIM_DELAY;
				FF_UncacheDreamSprite(ob);
				ob->gfxChunk++;
			}
		}
	}
#undef NESSI_ANIM_DELAY
}


//===========================================================================

/*
=======================
=
= BurnKeen		(K1n9_Duk3 addition)
=
=======================
*/

void FF_BurnKeen(bool force)
{
	CK_object *ob = ck_keenObj;

	if (ck_godMode || (ff_map.invincibilityTimer && !force)
		|| ob->currentAction == CK_ACTION(FF_ACT_keenBurn))
		return;

	if (ob->currentAction == CK_ACTION(CK_ACT_keenDie0) || ob->currentAction == CK_ACTION(CK_ACT_keenDie1))
	{
		FF_KillKeen();
		return;
	}

	;
	FF_RemoveDreamBubbles();	// just in case...
	FF_DropNisasi();

	SD_PlaySound(FF_KEENDIESND);
	ob->type = CT_Friendly;	// to make sure other objects don't interact with Keen
	ff_keenKilled = true;
	ob->gfxChunk = FF_KEENBURNED1SPR;
	ob->velX=ob->velY=0;
	CK_SetAction2(ob, CK_ACTION(FF_ACT_keenBurn));
//	ob->timeUntillThink = 40;
}

//===========================================================================

/*
=======================
=
= T_KeenBurn		(K1n9_Duk3 addition)
=
=======================
*/

void FF_T_KeenBurn(CK_object *ob)
{
	if ((ob->actionTimer += SD_GetSpriteSync()) >= 6)
	{
		if (ob->gfxChunk == FF_KEENDIE2SPR)
		{
			ob->gfxChunk = FF_KEENBURNED1SPR;
		}
		if (ob->gfxChunk == FF_KEENBURNED7SPR)
		{
			FF_T_Projectile(ob);
			if (ob->actionTimer >= 100)
				ck_gameState.levelState = ex_died;
		}
		else
		{
			ob->actionTimer -= 6;
			FF_UncacheDreamSprite(ob);
			ob->gfxChunk++;
		}
	}
}

//===========================================================================

/*
=======================
=
= R_KeenBurn		(K1n9_Duk3 addition)
=
=======================
*/

void FF_R_KeenBurn(CK_object *ob)
{
	if (ob->topTI && ob->velY > 0)
	{
		ob->velY = -(ob->velY/2);
	}
	FF_R_AutoCache(ob);
}


//===========================================================================

/*
=======================
=
= T_KeenSink		(K1n9_Duk3 addition)
=
=======================
*/

void FF_T_KeenSink(CK_object *ob)
{
	 int16_t speed = SD_GetSpriteSync()*3;

	ck_nextY = speed;
	if ((ob->user2 += speed) >= 18*16)
	{
		ck_gameState.levelState = ex_died;
	}
	if (ob->user2 >= 10*16 && ob->user2 - speed < 10*16)
	{
		SD_PlaySound(FF_KEENDIESND);
	}
}

#endif

void kw_ff_keen_SetupFunctions(void)
{
	CK_ACT_AddFunction("FF_CheckInTiles", &FF_CheckInTiles);
	CK_ACT_AddFunction("FF_ExplodeShot", &FF_ExplodeShot);
	CK_ACT_AddFunction("FF_HandleRiding", &FF_HandleRiding);
	CK_ACT_AddFunction("FF_KeenAirReact", &FF_KeenAirReact);
	CK_ACT_AddFunction("FF_KeenAirThink", &FF_KeenAirThink);
	CK_ACT_AddFunction("FF_KeenBounceThink", &FF_KeenBounceThink);
	CK_ACT_AddFunction("FF_KeenClimbThink", &FF_KeenClimbThink);
	CK_ACT_AddFunction("FF_KeenDieThink", &FF_KeenDieThink);
	CK_ACT_AddFunction("FF_KeenDropThink", &FF_KeenDropThink);
	CK_ACT_AddFunction("FF_KeenEnterThink", &FF_KeenEnterThink);
	CK_ACT_AddFunction("FF_KeenHoldThink", &FF_KeenHoldThink);
	CK_ACT_AddFunction("FF_KeenKeyThink", &FF_KeenKeyThink);
	CK_ACT_AddFunction("FF_KeenLookDownThink", &FF_KeenLookDownThink);
	CK_ACT_AddFunction("FF_KeenLookUpThink", &FF_KeenLookUpThink);
	CK_ACT_AddFunction("FF_KeenPauseThink", &FF_KeenPauseThink);
	CK_ACT_AddFunction("FF_KeenPogoReact", &FF_KeenPogoReact);
	CK_ACT_AddFunction("FF_KeenPogoThink", &FF_KeenPogoThink);
	CK_ACT_AddFunction("FF_KeenPoleReact", &FF_KeenPoleReact);
	CK_ACT_AddFunction("FF_KeenPoleThink", &FF_KeenPoleThink);
	CK_ACT_AddFunction("FF_KeenShootThink", &FF_KeenShootThink);
	CK_ACT_AddFunction("FF_KeenSimpleReact", &FF_KeenSimpleReact);
	CK_ACT_AddFunction("FF_KeenStandReact", &FF_KeenStandReact);
	CK_ACT_AddFunction("FF_KeenStandThink", &FF_KeenStandThink);
	CK_ACT_AddFunction("FF_KeenSwitchThink", &FF_KeenSwitchThink);
	CK_ACT_AddFunction("FF_KeenWalkReact", &FF_KeenWalkReact);
	CK_ACT_AddFunction("FF_KeenWalkThink", &FF_KeenWalkThink);
	CK_ACT_AddFunction("FF_PoleActions", &FF_PoleActions);
	CK_ACT_AddFunction("FF_R_AutoCache", &FF_R_AutoCache);
	CK_ACT_AddFunction("FF_R_KeenBurn", &FF_R_KeenBurn);
	CK_ACT_AddFunction("FF_R_KeenStunned", &FF_R_KeenStunned);
	CK_ACT_AddFunction("FF_R_Shot", &FF_R_Shot);
	CK_ACT_AddFunction("FF_T_DreamBubble", &FF_T_DreamBubble);
	CK_ACT_AddFunction("FF_T_KeenBurn", &FF_T_KeenBurn);
	CK_ACT_AddFunction("FF_T_KeenDream", &FF_T_KeenDream);
	CK_ACT_AddFunction("FF_T_KeenDrown", &FF_T_KeenDrown);
	CK_ACT_AddFunction("FF_T_KeenPoleShoot", &FF_T_KeenPoleShoot);
	CK_ACT_AddFunction("FF_T_KeenSink", &FF_T_KeenSink);
	CK_ACT_AddFunction("FF_T_KeenStartDream", &FF_T_KeenStartDream);
	CK_ACT_AddFunction("FF_T_KeenStunnedRecover", &FF_T_KeenStunnedRecover);
	CK_ACT_AddFunction("FF_T_KeenUseAmmoStation", &FF_T_KeenUseAmmoStation);
	CK_ACT_AddFunction("FF_T_LineUp", &FF_T_LineUp);
	CK_ACT_AddFunction("FF_T_PullUp1", &FF_T_PullUp1);
	CK_ACT_AddFunction("FF_T_PullUp2", &FF_T_PullUp2);
	CK_ACT_AddFunction("FF_T_PullUp3", &FF_T_PullUp3);
	CK_ACT_AddFunction("FF_T_PulledUp", &FF_T_PulledUp);
	CK_ACT_AddFunction("FF_T_Shot", &FF_T_Shot);
	CK_ACT_AddFunction("FF_WalkSound1", &FF_WalkSound1);
	CK_ACT_AddFunction("FF_WalkSound2", &FF_WalkSound2);
	CK_ACT_AddColFunction("FF_KeenContact", &FF_KeenContact);
	CK_ACT_AddColFunction("FF_KeenPosContact", &FF_KeenPosContact);
}
