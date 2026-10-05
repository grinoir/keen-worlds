#include "kw_fitf.h"
#include "kw_ff_keen.h"

typedef int turntype_fwd;
void FF_TurnX(CK_object *ob);
void FF_TurnY(CK_object *ob);
void FF_T_Projectile(CK_object *ob);
void FF_T_WeakProjectile(CK_object *ob);
void FF_T_Velocity(CK_object *ob);
void FF_C_LethalBlock(CK_object *ob, CK_object *hit);
void FF_C_Lethal(CK_object *ob, CK_object *hit);
void FF_R_Draw(CK_object *ob);
void FF_R_Walk(CK_object *ob);
void FF_R_WalkNormal(CK_object *ob);
void FF_R_Fall(CK_object *ob);
void FF_R_Stunned(CK_object *ob);
void FF_StunObj(CK_object *ob, CK_object *shot, CK_action *stunstate);
void FF_DoorOpen(CK_object *ob);
bool FF_DoorsActive(void);
CK_object *FF_SpawnBonus(int16_t tileX, int16_t tileY, int16_t itemNumber);
void FF_T_Bonus(CK_object *ob);
void FF_SpawnStaticPlat(int16_t tileX, int16_t tileY);
void FF_SpawnPlatform(int16_t tileX, int16_t tileY, int16_t direction);
void FF_T_Platform(CK_object *ob);
void FF_R_Platform(CK_object *ob);
void FF_SpawnDropPlat(int16_t tileX, int16_t tileY);
void FF_T_DropPlatSit (CK_object *ob);
void FF_T_DropPlatFall (CK_object *ob);
void FF_T_DropPlatRise (CK_object *ob);
void FF_SpawnSlug(int16_t tileX, int16_t tileY);
void FF_T_Slug(CK_object *ob);
void FF_T_SlugPiss(CK_object *ob);
void FF_C_Slug(CK_object *a, CK_object *b);
void FF_C_SlugSlime(CK_object *a, CK_object *b);
void FF_R_SlugWalk(CK_object *ob);
void FF_SmirkyCheckTiles(CK_object *ob);
void FF_R_PestRest(CK_object *ob);
void FF_SpawnSkypest(int16_t tileX, int16_t tileY);
void FF_T_PestFly(CK_object *ob);
void FF_C_PestFly(CK_object *a, CK_object *b);
void FF_C_Squashable(CK_object *a, CK_object *b);
void FF_T_PestRest(CK_object *ob);
void FF_R_Pest(CK_object *ob);
void FF_SpawnPatPat(int16_t tileX, int16_t tileY, int16_t dir);
void FF_T_PatPat(CK_object *ob);
void FF_SpawnCouncilMember(int16_t tileX, int16_t tileY, int16_t number);
void FF_T_Council(CK_object *ob);
void FF_T_CouncilMop(CK_object *ob);
void FF_C_Council(CK_object *ob, CK_object *hit);
void FF_R_Council(CK_object *ob);
void FF_R_Bucket(CK_object *ob);
void FF_SpawnLindsey(int16_t tileX, int16_t tileY, int16_t num);
void FF_T_Lindsey(CK_object *ob);
void FF_R_Lindsey(CK_object *ob);
void FF_LindseyVanish(CK_object *ob);
void FF_SpawnXkyLick(int16_t tileX, int16_t tileY);
void FF_T_XkyLickBounce(CK_object *ob);
void FF_T_XkyLickShuffle(CK_object *ob);
void FF_C_XkyLick(CK_object *a, CK_object *b);
void FF_C_XkyLickAttack(CK_object *a, CK_object *b);
void FF_R_XkyLickAir(CK_object *ob);
void FF_R_XkyLick(CK_object *ob);
void FF_SpawnYorp(int16_t tileX, int16_t tileY);
void FF_T_YorpWalk(CK_object *ob);
void FF_T_YorpSearch(CK_object *ob);
void FF_C_Yorp(CK_object *a, CK_object *b);
void FF_SpawnDungBeetle(int16_t tileX, int16_t tileY);
void FF_SpawnChemolain(int16_t tileX, int16_t tileY);
void FF_ChemCountdown(CK_object *ob);
void FF_T_ChemStand(CK_object *ob);
void FF_T_ChemCycle(CK_object *ob);
void FF_T_ChemHidden(CK_object *ob);
void FF_C_ChemRide(CK_object *a, CK_object *b);
void FF_C_Chem(CK_object *a, CK_object *b);
void FF_SpawnLogPlatform(int16_t tileX, int16_t tileY, int16_t xdir);
void FF_T_Log(CK_object *ob);
void FF_R_Log(CK_object *ob);
void FF_SpawnSalamander(int16_t tileX, int16_t tileY);
void FF_SalamanderThink(CK_object *ob);
void FF_SpawnWormouth(int16_t tileX, int16_t tileY);
void FF_WormouthLookRightThink(CK_object *ob);
void FF_WormouthLookThink(CK_object *ob);
void FF_WormouthLookLeftThink(CK_object *ob);
void FF_WormouthThink(CK_object *ob);
void FF_WormouthLookContact(CK_object *ob, CK_object *hit);
void FF_WormouthBiteContact(CK_object *ob, CK_object *hit);
void FF_WormouthWalkReact(CK_object *ob);
void FF_SpawnShrubbery(int16_t tileX, int16_t tileY);
void FF_T_Shrubbery(CK_object *ob);
void FF_ShrubberyLookThink(CK_object *ob);
void FF_T_ShrubberyWalk(CK_object *ob);
void FF_C_Shrubbery(CK_object *ob, CK_object *hit);
void FF_R_ShrubberyFall(CK_object *ob);
void FF_SpawnBounder(int16_t tileX, int16_t tileY);
void FF_T_Bounder(CK_object *ob);
void FF_C_Bounder(CK_object *ob, CK_object *hit);
void FF_R_Bounder(CK_object *ob);
void FF_SpawnTentacle(int16_t tileX, int16_t tileY);
void FF_T_Tentacle(CK_object *ob);
void FF_T_TentacleAttack(CK_object *ob);
void FF_C_Tentacle(CK_object *ob, CK_object *hit);
void FF_SpawnPoleknawel(int16_t tileX, int16_t tileY);
void FF_T_PoleknawelLand(CK_object *ob);
void FF_T_PoleknawelWalk(CK_object *ob);
void FF_T_PoleknawelSlide(CK_object *ob);
void FF_T_PoleknawelThrow(CK_object *ob);
void FF_C_Poleknawel(CK_object *ob, CK_object *hit);
void FF_R_PoleknawelWalk(CK_object *ob);
void FF_R_PoleknawelFall(CK_object *ob);
void FF_KnawlReact(CK_object *ob);
void FF_SpawnKitten(int16_t tileX, int16_t tileY);
void FF_T_KittenIdle(CK_object *ob);
void FF_T_KittenRoll(CK_object *ob);
void FF_T_KittenExplode(CK_object *ob);
void FF_C_Kitten(CK_object *ob, CK_object *hit);
void FF_SpawnGellyGhost(int16_t tileX, int16_t tileY);
void FF_T_GellyGhost(CK_object *ob);
void FF_C_GellyGhost(CK_object *ob, CK_object *hit);
void FF_R_GellyGhost(CK_object *ob);
void FF_SpawnCollectible(int16_t tileX, int16_t tileY, int16_t number);
void FF_SpawnRoots(int16_t tileX, int16_t tileY);
void FF_T_Roots(CK_object *ob);
void FF_C_Roots(CK_object *ob, CK_object *hit);
void FF_SpawnYeti(int16_t tileX, int16_t tileY);
void FF_PushKeenX(CK_object *hit, CK_object *ob);
void FF_T_Yeti(CK_object *ob);
void FF_C_Yeti(CK_object *ob, CK_object *hit);
void FF_T_YetiCloud(CK_object *ob);
void FF_C_YetiCloud(CK_object *ob, CK_object *hit);
void FF_T_TinyYetiRoll(CK_object *ob);
void FF_T_TinyYetiJump(CK_object *ob);
void FF_C_TinyYeit(CK_object *ob, CK_object *hit);
void FF_R_TinyYetiJump(CK_object *ob);
void FF_R_TinyYetiRoll(CK_object *ob);
void FF_SpawnCloud(int16_t tileX, int16_t tileY);
void FF_SpawnInvisibleCloud(uint16_t unitX, uint16_t unitY, uint16_t w, uint16_t h);
void FF_T_Cloud(CK_object *ob);
void FF_SpawnMoletDirt(uint16_t unitX, uint16_t unitY);
void FF_T_MoletDirt(CK_object *ob);
void FF_SpawnMolet(int16_t tileX, int16_t tileY);
void FF_T_Molet(CK_object *ob);
void FF_T_MoletDig(CK_object *ob);
void FF_T_MoletFall(CK_object *ob);
void FF_C_Molet(CK_object *ob, CK_object *hit);
void FF_MoletWalkReact(CK_object *ob);
void FF_R_MoletFall(CK_object *ob);
void FF_SpawnBird(int16_t tileX, int16_t tileY);
int16_t FF_CountBirds(void);
int16_t FF_CountEggs(void);
void FF_T_BirdFly(CK_object *ob);
void FF_T_BirdAttack(CK_object *ob);
void FF_T_BirdLayEgg(CK_object *ob);
void FF_C_Bird(CK_object *ob, CK_object *hit);
void FF_R_Bird(CK_object *ob);
void FF_T_BirdEgg(CK_object *ob);
void FF_C_BirdEgg(CK_object *ob, CK_object *hit);
void FF_R_BirdEgg(CK_object *ob);
void FF_R_EggPiece(CK_object *ob);
void FF_SpawnSneakyTree(int16_t tileX, int16_t tileY);
void FF_T_SneakyTree(CK_object *ob);
void FF_R_SneakyTree(CK_object *ob);
void FF_SpawnCroc(int16_t tileX, int16_t tileY);
void FF_CrocCheckTiles(CK_object *ob);
void FF_T_CrocWalk(CK_object *ob);
void FF_T_CrocJump(CK_object *ob);
void FF_T_CrocTrap(CK_object *ob);
void FF_T_CrocSnap(CK_object *ob);
void FF_C_Croc(CK_object *ob, CK_object *hit);
void FF_R_CrocWalk(CK_object *ob);
void FF_R_CrocJump(CK_object *ob);
void FF_R_CrocSnap(CK_object *ob);
void FF_SpawnTentacleNest(int16_t tileX, int16_t tileY);
void FF_T_TantacleNest(CK_object *ob);
void FF_C_TantacleNest(CK_object *ob, CK_object *hit);
void FF_T_Swallowed(CK_object *ob);
void FF_SpawnCheckpoint(int16_t tileX, int16_t tileY);
void FF_T_Checkpoint(CK_object *ob);
void FF_T_CheckpointCap(CK_object *ob);
CK_object * FF_SpawnSmoke(int16_t x, int16_t y);
void FF_T_Smoke(CK_object *ob);
void FF_CheckpointSave(CK_object *ob);
bool FF_CheckpointLoad(void);
void FF_R_Checkpoint(CK_object* ob);
void FF_RemoveLeaf(CK_object *ob);
void FF_SpawnLeaveaTrip(int16_t tileX, int16_t tileY);
void FF_T_LeaveaTree(CK_object *ob);
void FF_T_LeaveaFall(CK_object *ob);
void FF_R_LeaveaFall(CK_object *ob);
void FF_T_LeaveaWalk(CK_object *ob);
void FF_C_LeaveaWalk(CK_object *ob, CK_object *hit);
void FF_R_LeaveaWalk(CK_object *ob);
void FF_SpawnWarble(int16_t tileX, int16_t tileY);
void FF_T_Warble(CK_object *ob);
void FF_C_Warble(CK_object *ob, CK_object *hit);
CK_object *FF_SpawnStaticSprite(uint16_t tileX, uint16_t tileY, int16_t chunk, int16_t priority);
void FF_SpawnEfree(uint16_t x, uint16_t y);
void FF_T_Efree(CK_object *ob);
void FF_EfreeGrabKeen(CK_object *ob);
void FF_SpawnNisasi(int16_t tileX, int16_t tileY);
void FF_NisasiLetGo(CK_object *ob);
void FF_T_Nisasi(CK_object *ob);
void FF_C_Nisasi(CK_object *ob, CK_object *hit);
void FF_R_Nisasi(CK_object *ob);
void FF_DropNisasi(void);
void FF_ObjDropNisasi(CK_object *ob);
void FF_SpawnCouncilPage(int16_t tileX, int16_t tileY, int16_t type);
void FF_T_Page(CK_object *ob);
void FF_R_Page(CK_object *ob);
CK_object *FF_FindPageObj(uint16_t infoval);
void FF_ControlPage(CK_object *ob);
void FF_T_KeenEnterMachine(CK_object *ob);
void FF_T_KeenMindcontrol(CK_object *ob);
CK_object *FF_SpawnSlideDoor(int16_t tileX, int16_t tileY, int16_t type);
void FF_T_SlideDoor(CK_object *ob);
void FF_SpawnCloneMachine(int16_t tileX, int16_t tileY, int16_t type, int16_t dir);
void FF_AnimateCloneObj(CK_object *ob);
void FF_T_CloneMachine(CK_object *ob);
void FF_C_CloneMachine(CK_object *ob, CK_object *hit);
void FF_T_CloneAnimation(CK_object *ob);
void FF_T_CloneObj(CK_object *ob);
void FF_R_CloneObj(CK_object *ob);
void FF_ToggleCloneMachine(int16_t tileX, int16_t tileY);
void FF_SpawnCoronaPlant(int16_t tileX, int16_t tileY, int16_t dir);
void FF_T_CoronaPlant(CK_object *ob);
void FF_T_CoronaBall(CK_object *ob);
void FF_R_CoronaBall(CK_object *ob);
void FF_SpawnNipnap(int16_t tileX, int16_t tileY);
void FF_T_Nipnap(CK_object *ob);
void FF_C_Nipnap(CK_object *ob, CK_object *hit);
void FF_SpawnCrab(int16_t tileX, int16_t tileY, bool onCeiling);
void FF_R_CrabTurn(CK_object *ob);
void FF_T_CrabFloor(CK_object *ob);
void FF_T_CrabTurn(CK_object *ob);
void FF_T_CrabWall(CK_object *ob);
void FF_T_CrabCeiling(CK_object *ob);
void FF_CrabDraw(CK_object *ob);
void FF_CrabDrawFlipped(CK_object *ob);
void FF_R_CrabFloor(CK_object *ob);
void FF_R_CrabWall(CK_object *ob);
void FF_R_CrabCeiling(CK_object *ob);
void FF_CrabFallReact(CK_object *ob);
void FF_C_Crab(CK_object *ob, CK_object *hit);
void FF_SpawnBrainFucl(int16_t tileX, int16_t tileY);
void FF_T_BrainFucl(CK_object *ob);
void FF_C_BrainFucl(CK_object *ob, CK_object *hit);
void FF_SpawnDonut(int16_t tileX, int16_t tileY);
void FF_T_Donut(CK_object *ob);
void FF_SpawnBalloonSmirky(int16_t tileX, int16_t tileY);
void FF_T_BalloonSmirky(CK_object *ob);
void FF_SpawnFungusAmongus(int16_t tileX, int16_t tileY);
void FF_T_Fungus(CK_object *ob);
void FF_C_Fungus(CK_object *ob, CK_object *hit);
void FF_R_Fungus(CK_object *ob);
void FF_R_FungusFall(CK_object *ob);
void FF_SpawnWorldNessie(int16_t tileX, int16_t tileY);
void FF_T_WorldNessie(CK_object *ob);
void FF_R_WorldNessie(CK_object *ob);
void FF_SpawnFrog(int16_t tileX, int16_t tileY);
void FF_T_Frog(CK_object *ob);
void FF_R_Frog(CK_object *ob);
void FF_PageWalk(CK_object *ob);
void FF_PageContact(CK_object *ob, CK_object *hit);
void FF_PageWalkReact(CK_object *ob);
void FF_KeenLook(CK_object *ob);
void FF_SpawnQuicksand(uint16_t x, uint16_t y);
void FF_T_Quicksand(CK_object *ob);
void FF_SpawnFallBlock(int16_t tileX, int16_t tileY);
void FF_T_FallBlock(CK_object *ob);
void FF_C_FallBlock(CK_object *ob, CK_object *hit);
void FF_R_FallBlock(CK_object *ob);
void FF_SpawnObelisk(int16_t tileX, int16_t tileY);
void FF_T_Obelisk(CK_object *ob);
void FF_C_Obelisk(CK_object *ob, CK_object *hit);
void FF_R_Obelisk(CK_object *ob);
void FF_SpawnMummySpawner(int16_t tileX, int16_t tileY);
void FF_T_MummySpawner(CK_object *ob);
void FF_MummyCheckForKeen(CK_object *ob);
void FF_T_Mummy(CK_object *ob);
void FF_C_Mummy(CK_object *ob, CK_object *hit);
void FF_R_Mummy(CK_object *ob);
void FF_T_KeenMummy(CK_object *ob);
void FF_C_KeenMummy(CK_object *ob, CK_object *hit);
void FF_T_Bandage(CK_object *ob);
void FF_SpawnLemonShark(int16_t tileX, int16_t tileY);
void FF_T_LemonShark(CK_object *ob);
void FF_SpawnDonutCloud(int16_t tileX, int16_t tileY);
void FF_T_DonutCloud(CK_object *ob);
void FF_SpawnJediBoot(int16_t tileX, int16_t tileY);
void FF_T_JediBoot(CK_object *ob);
void FF_C_JediBoot(CK_object *ob, CK_object *hit);
void FF_T_KeenSmoke(CK_object *ob);
void FF_SpawnSlugCans(int16_t tileX, int16_t tileY);
void FF_SpawnBigShroom(int16_t tileX, int16_t tileY);

/* "Foray in the Forest" Source Code
 * Copyright (C) 2022-2024 Nisaba
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

// CK_ACT.C


// This file contains some object functions (think, etc) which are common to
// several episodes.

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

//extern statetype s_bonus2;	// K1n9_Duk3 mod: we don't actually need this



void FF_T_Projectile(CK_object *ob);
void FF_T_WeakProjectile(CK_object *ob);
void FF_T_Velocity(CK_object *ob);
void FF_C_LethalBlock(CK_object *o1, CK_object *o2);
void FF_C_Lethal(CK_object *o1, CK_object *o2);
void FF_R_Draw(CK_object *ob);
void FF_R_Walk(CK_object *ob);
void FF_R_WalkNormal(CK_object *ob);
void FF_R_Stunned(CK_object *ob);
void FF_StunObj(CK_object *ob, CK_object *shot, CK_action *stunstate);
void FF_DoorOpen(CK_object *ob);
void CK_SecurityDoorOpen(CK_object *ob);
void FF_T_Bonus(CK_object *ob);
void FF_T_Platform(CK_object *ob);
void FF_T_DropPlatSit (CK_object *ob);
void FF_T_DropPlatFall (CK_object *ob);
void FF_T_DropPlatRise (CK_object *ob);
/*
=============================================================================

LOCAL VARIABLES

=============================================================================
*/


//===========================================================================
//
// GENERIC ACTOR BEHAVIOURS
// 
//===========================================================================

void  FF_TurnX(CK_object *ob)
{
	ob->xDirection = -ob->xDirection;
	ob->posX -= ob->deltaPosX;
	ob->posY -= ob->deltaPosY;
	CK_SetAction2(ob, ob->currentAction);
}

void  FF_TurnY(CK_object *ob)
{
	ob->yDirection = -ob->yDirection;
	ob->posX -= ob->deltaPosX;
	ob->posY -= ob->deltaPosY;
	CK_SetAction2(ob, ob->currentAction);
}

// Think function for adding gravity

void FF_T_Projectile(CK_object *ob)
{
	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	FF_DoGravity(ob);
	ck_nextX = ob->velX * SD_GetSpriteSync();
}

// Think function for adding a slightly lower amount of gravity

void FF_T_WeakProjectile(CK_object *ob)
{
	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	FF_DoWeakGravity(ob);
	ck_nextX = ob->velX * SD_GetSpriteSync();
}

void FF_T_Velocity(CK_object *ob)
{
	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	ck_nextX = ob->velX * SD_GetSpriteSync();
	ck_nextY = ob->velY * SD_GetSpriteSync();
}

/*
void CK_ObjBadState (void)
{
	//Quit("Object with bad state!");
	Quit("Foray in the Forest: BADSTATE");
}

*/

void FF_C_LethalBlock(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Stunner)
	{
		FF_ExplodeShot(hit);
	}
	else if (hit->type == CT_Player)
	{
		FF_KillKeen();
	}
}

void FF_C_Lethal(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player)
	{
		FF_KillKeen();
	}
}


void FF_R_Draw(CK_object *ob)
{
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//
// For walking and turning around at edges 
//
void FF_R_Walk(CK_object *ob)
{
	// K1n9_Duk3 mod: merged all checks into one if-statement
	if ( (ob->xDirection == IN_motion_Right && ob->leftTI != 0)
	  || (ob->xDirection == IN_motion_Left && ob->rightTI != 0)
	  || (ob->topTI == 0) )
	{
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		ob->xDirection = -ob->xDirection;
		ob->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(ob, ob->currentAction);
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

//
// For slugs  (and maybe others???)
//
void FF_R_WalkNormal(CK_object *ob)
{
	// K1n9_Duk3 mod: merged all checks into one if-statement
	if ( (ob->xDirection == IN_motion_Right && ob->leftTI != 0)
	  || (ob->xDirection == IN_motion_Left && ob->rightTI != 0)
	  || (ob->topTI == 0 || (ob->topTI & ~SLOPEMASK)) )
	{
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		ob->xDirection = -ob->xDirection;
		ob->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(ob, ob->currentAction);
	}

	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

void FF_R_Fall(CK_object *ob)
{
	// Handle physics
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = 0;
	}

	if (ob->bottomTI)
	{
		ob->velY = 0;
	}

	if (ob->topTI)
	{
		ob->velX = ob->velY = 0;
		if (ob->currentAction->next)
		{
			CK_SetAction2(ob, ob->currentAction->next);
		}
	}
	FF_R_Draw(ob);
}

//
// Think function for stunned creatures
//
void FF_R_Stunned(CK_object *ob)
{
	int16_t starsX, starsY;

	FF_R_Fall(ob);
/*
	// Handle physics
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = 0;
	}

	if (ob->bottomTI)
	{
		ob->velY = 0;
	}

	if (ob->topTI)
	{
		ob->velX = ob->velY = 0;
		if (ob->currentAction->next)
		{
			ChangeState(ob, ob->currentAction->next);
		}
	}

	// Draw the primary chunk
	RF_PlaceSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
*/


	// Draw the stunner stars, offset based on the initial type of the stunned
	// critter

	starsX = starsY = 0;

	switch (ob->user4)
	{
		// Enemy specific star-offsets
		case FF_CT_Slug:
			starsX -= 3*16;
			starsY -= 8*16;
			break;
		case FF_CT_Wormouth:
			starsX = 4*16;
			starsY = -350;	// a bit weird, but this is the value used in KEEN 4
			break;
		case FF_CT_XkyLick:
			starsY = -16*16;
			break;
		case FF_CT_Bird:
			starsX = 12*16;
			starsY = -8*16;
			break;
	}

	// Tick the star 3-frame animation forward
	if ((ob->user1 += SD_GetSpriteSync()) > 10)
	{
		ob->user1 -= 9;
		if (++ob->user2 >= 3)
			ob->user2 = 0;
	}

	// FIXME: Will cause problems on 64-bit systems
	RF_AddSpriteDrawUsing16BitOffset(&ob->user3, ob->posX + starsX, ob->posY + starsY, ob->user2 + FF_STUNNEDSTARS1SPR, false, FOREGROUNDPRIORITY);
}

void FF_StunObj(CK_object *ob, CK_object *shot, CK_action *stunstate)
{
	// Kill the stunner shot	(K1n9_Duk3 fix: only when it's not a NULL pointer)
	if (shot)
		FF_ExplodeShot(shot);

	FF_ObjDropNisasi(ob);

	// Set stunned creature action
	ob->user1 = ob->user2 = ob->user3 = 0;
	ob->user4 = ob->type;
	CK_SetAction2(ob, stunstate);
	ob->type = CT5_StunnedCreature;

	// Make the creature jump up a bit
	if ((ob->velY -= 0x18) < -0x30)
		ob->velY = -0x30;

	// K1n9_Duk3 fix:
	ob->timeUntillThink = 0;	// allow it to jump without delay
}

//===========================================================================
//
// DOORS
// 
//===========================================================================


void FF_DoorOpen(CK_object *ob)
{
	int16_t i;
	uint16_t tilesToReplace[0x30];

	if (ob->user1 > 0x30)
	{
		//Quit("Door too tall!");
		Quit("Foray in the Forest: DOORTOOTALL");
	}

	for (i = 0; i < ob->user1; i++)
	{
		tilesToReplace[i] = FORESPOT(ob->posX, ob->posY + i) + ob->user2;	// K1n9_Duk3 mod: + temp2 instead of + 1 (to support closing doors)
	}

	RF_ReplaceTiles(tilesToReplace, 1, ob->posX, ob->posY, 1, ob->user1);
}

// patch: wait for doors to completly open/close before releasing the door switch
bool FF_DoorsActive(void)
{
	CK_object *ob;
	
	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		if (ob->currentAction && ob->currentAction->think == FF_DoorOpen)
			return true;
	}
	return false;
}

/*
void CardDoorOpen(objtype *ob)
{
	int x, y;
	unsigned tilesToReplace[0x30];
	for (y = 0; y < 4; ++y)
	{
		for (x = 0; x < 4; ++x)
		{
			tilesToReplace[y*4+x] = FORESPOT(ob->posX+x, ob->posY+y) - 4;
		}
	}

	RF_ReplaceTiles(tilesToReplace, 1, ob->posX, ob->posY, 4, 4);
	ob->user1++;
	if (ob->user1 == 3)
	{
		ob->currentAction = NULL;
	}
}
*/

//===========================================================================
//
// ITEMS 
// 
//===========================================================================

//statetype s_bonus2 = {0, 0, step, false, push_none, 20, 0, 0, T_Bonus, NULL, R_Draw, CK_ACTION(FF_ACT_bonus1)};	// K1n9_Duk3 mod: we don't actually need this


//StartSprites + 
uint16_t bonusshape[] ={
	FF_REDGEM1SPR, FF_YELLOWGEM1SPR, FF_BLUEGEM1SPR, FF_GREENGEM1SPR,
	FF_POINTS100ITEM1SPR, FF_POINTS200ITEM1SPR, FF_POINTS500ITEM1SPR, FF_POINTS1000ITEM1SPR, FF_POINTS2000ITEM1SPR, FF_POINTS5000ITEM1SPR,
	FF_ITEM3UP1SPR, FF_ITEMSTUNNER1SPR
};

CK_object *FF_SpawnBonus(int16_t tileX, int16_t tileY, int16_t itemNumber)
{

	CK_object *new_object = CK_GetNewObj(false);

	new_object->clipped = CLIP_not;
	//new->active = ac_yes;
	new_object->zLayer = 2;
	new_object->type = CT5_Item;	
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->yDirection = IN_motion_Up;
	new_object->user1 = itemNumber;
	new_object->gfxChunk = bonusshape[itemNumber];
	new_object->user2 = new_object->gfxChunk;
	new_object->user3 = new_object->gfxChunk + 2;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_bonus1));

	// FITF hack: item number 6 (500 points) doesn't animate
	if (itemNumber == 6)
		new_object->user3--;

/*
	if (itemNumber == 10)
	{
		ck_mapState.livesTotal++;
	}
	else if (itemNumber == 11)
	{
		ck_mapState.ammoTotal++;
	}
	else */ if (ck_itemPoints[itemNumber])
	{
		//ck_mapState.pointsTotal += ck_itemPoints[itemNumber];
		ff_map.itemsTotal++;
	}

	// K1n9_Duk3 note: You should NOT cache graphics in the spawn routine!
	// (But marking the graphics as needed would be okay here.)
/*
	CA_CacheGrChunk(new->gfxChunk);
	CA_CacheGrChunk(new->gfxChunk + 1);
*/

	return new_object;
}

void FF_T_Bonus(CK_object *ob)
{
	//ob->visible = true;	// redundant, StateMachine will notice the new shapenum
	if (++ob->gfxChunk == ob->user3)
		ob->gfxChunk = ob->user2;
}

/*
=============================================================================

	 PLATFORMS

	temp1 - initial x position for falling platforms
	temp2 - additional sprite pointer for thruster sprites
	temp3 - additional sprite pointer for thruster sprites

=============================================================================
*/
//===========================================================================
//
// PLATFORMS
// 
//===========================================================================

#if 0	// static platforms are not used in FITF


void FF_SpawnStaticPlat(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->type = CT5_Platform;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	//new->zLayer = 0;	//redundant - GetNewObj already set it to 0
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = new_object->user1 = RF_TileToUnit(tileY);
	//new->xDirection = 0;	//redundant - GetNewObj already set it to 0
	new_object->yDirection = 1;
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_statplat));
}
#endif


void FF_SpawnPlatform(int16_t tileX, int16_t tileY, int16_t direction)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->type = CT5_Platform;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	//new->zLayer = 0;	//redundant - GetNewObj already set it to 0
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);

	switch (direction)
	{
	case arrow_North:
		//new->xDirection = 0;	//redundant - GetNewObj already set it to 0
		new_object->yDirection = -1;
		break;
	case arrow_East:
		new_object->xDirection = 1;
		//new->yDirection = 0;	//redundant - GetNewObj already set it to 0
		break;
	case arrow_South:
		//new->xDirection = 0;	//redundant - GetNewObj already set it to 0
		new_object->yDirection = 1;
		break;
	case arrow_West:
		new_object->xDirection = -1;
		//new->yDirection = 0;	//redundant - GetNewObj already set it to 0
		break;
	}

	CK_SetAction(new_object, CK_ACTION(FF_ACT_platform));
}



void FF_T_Platform(CK_object *ob)
{
	uint16_t nextPosUnit, nextPosTile;

	if (ck_nextX || ck_nextY)
	{
		return;
	}

	ck_nextX = ob->xDirection * 12 * SD_GetSpriteSync();
	ck_nextY = ob->yDirection * 12 * SD_GetSpriteSync();

	if (ob->xDirection == 1)
	{
		nextPosUnit = ob->clipRects.unitX2 + ck_nextX;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (ob->clipRects.tileX2 != nextPosTile && INFOSPOT(nextPosTile, ob->clipRects.tileY1) == PLATFORMBLOCK)
		{
			ob->xDirection = -1;
			ck_nextX -= (nextPosUnit & 255);
		}
	}
	else if (ob->xDirection == -1)
	{
		nextPosUnit = ob->clipRects.unitX1 + ck_nextX;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (ob->clipRects.tileX1 != nextPosTile && INFOSPOT(nextPosTile, ob->clipRects.tileY1) == PLATFORMBLOCK)
		{
			ob->xDirection = 1;
			ck_nextX += (256 - nextPosUnit) & 255;
		}
	}
	else if (ob->yDirection == 1)
	{
		nextPosUnit = ob->clipRects.unitY2 + ck_nextY;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (ob->clipRects.tileY2 != nextPosTile && INFOSPOT(ob->clipRects.tileX1, nextPosTile) == PLATFORMBLOCK)
		{
			if (INFOSPOT(ob->clipRects.tileX1, nextPosTile - 2) == PLATFORMBLOCK)
			{
				// Got a block icon directly above and directly below, so stop the platform.
				ob->visible = true;
				ck_nextY = 0;
			}
			else
			{
				ob->yDirection = -1;
				ck_nextY -= ( nextPosUnit & 255);
			}
		}
	}
	else if (ob->yDirection == -1)
	{
		nextPosUnit = ob->clipRects.unitY1 + ck_nextY;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (ob->clipRects.tileY1 != nextPosTile && INFOSPOT(ob->clipRects.tileX1, nextPosTile) == PLATFORMBLOCK)
		{
			if (INFOSPOT(ob->clipRects.tileX1, nextPosTile + 2) == PLATFORMBLOCK)
			{
				// Got a block icon directly above and directly below, so stop the platform.
				ob->visible = true;
				ck_nextY = 0;
			}
			else
			{
				ob->yDirection = 1;
				ck_nextY += 256 - (nextPosUnit & 255);
			}
		}
	}
}

void FF_R_Platform(CK_object *ob)
{
	uint16_t frame;

	// place platform sprite:
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
	
	// place (or remove) thruster sprites:
	frame = (((uint16_t)SD_GetLastTimeCount()) >> 2) & 1;
	if (ob->xDirection == 1)
	{
		RF_AddSpriteDrawUsing16BitOffset(&ob->user2, ob->posX-1*16, ob->posY+3*16, frame+FF_PLATSIDETHRUST1SPR, false, 0);
		if (ob->user3)
			RF_RemoveSpriteDrawUsing16BitOffset(&ob->user3);
	}
	else if (ob->xDirection == -1)
	{
		if (ob->user2)
			RF_RemoveSpriteDrawUsing16BitOffset(&ob->user2);
		RF_AddSpriteDrawUsing16BitOffset(&ob->user3, ob->posX+48*16, ob->posY+5*16, frame+FF_PLATSIDETHRUST1SPR, false, 1);
	}
	else if (ob->yDirection == -1)
	{
		RF_AddSpriteDrawUsing16BitOffset(&ob->user2, ob->posX+2*16, ob->posY+9*16, frame+FF_PLATLTHRUST1SPR, false, 0);
		RF_AddSpriteDrawUsing16BitOffset(&ob->user3, ob->posX+46*16, ob->posY+8*16, frame+FF_PLATRTHRUST1SPR, false, 0);
	}
	else if (ob->yDirection == 1)
	{
		if (frame)
		{
			RF_AddSpriteDrawUsing16BitOffset(&ob->user2, ob->posX+2*16, ob->posY+9*16, frame+FF_PLATLTHRUST1SPR, false, 0);
			RF_AddSpriteDrawUsing16BitOffset(&ob->user3, ob->posX+46*16, ob->posY+8*16, frame+FF_PLATRTHRUST1SPR, false, 0);
		}
		else
		{
			if (ob->user2)
				RF_RemoveSpriteDrawUsing16BitOffset(&ob->user2);
			if (ob->user3)
				RF_RemoveSpriteDrawUsing16BitOffset(&ob->user3);
		}
	}
}



void FF_SpawnDropPlat(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = CT5_Platform;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	//new->zLayer = 0;	//redundant - GetNewObj already set it to 0
	new_object->posX = RF_TileToUnit(tileX);
	new_object->user1 = new_object->posY = RF_TileToUnit(tileY);
	//new->xDirection = motion_None;	//redundant
	new_object->yDirection = IN_motion_Down;
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_dropplatsit));
}

void FF_T_DropPlatSit (CK_object *ob)
{

	if (ob == ck_keenState.platform)
	{
		ck_nextY = SD_GetSpriteSync() * 16;
		ob->velY = 0;
		if ((uint16_t)(ob->posY + ck_nextY - ob->user1) >= 8*16)
			ob->currentAction = CK_ACTION(FF_ACT_dropplatfall);
	}
}

void FF_T_DropPlatFall (CK_object *ob)
{
	uint16_t newY, newYT;

	FF_DoGravity(ob);

	// K1n9_Duk3 fix: never move more than 15 pixels in one step (avoid moving through blocking tiles)
	if (ck_nextY >= 15*16)
		ck_nextY = 15*16;

	newY = ob->clipRects.unitY2 + ck_nextY;
	newYT = RF_UnitToTile(newY);

	// Stop falling if platform hits a block
	if ((ob->clipRects.tileY2 != newYT) && (INFOSPOT(ob->clipRects.tileX1, newYT) == PLATFORMBLOCK))
	{
		ck_nextY = 0xFF - (ob->clipRects.unitY2 & 0xFF);
		if (ck_keenState.platform != ob)
			ob->currentAction = CK_ACTION(FF_ACT_dropplatrise);
	}
}

void FF_T_DropPlatRise (CK_object *ob)
{
	if (ck_keenState.platform == ob)
	{
		ob->velY = 0;
		ob->currentAction = CK_ACTION(FF_ACT_dropplatfall);
	}
	else if ((uint16_t) ob->posY <= (uint16_t) ob->user1)
	{
		ck_nextY = ob->user1 - ob->posY;
		ob->currentAction = CK_ACTION(FF_ACT_dropplatsit);
	}
}

//===========================================================================

/*
##############################################################################

	FITF enemies

##############################################################################
*/

/*
==============================================================================
	SLUGS

	temp1 - 0 for regular slugs, non-zero for donut-carrying slug
	temp2 - 
	temp3 - sprite pointer for donut sprite
	temp4 - Nisasi pointer
==============================================================================
*/


void FF_T_Slug(CK_object *ob);
void FF_T_SlugPiss(CK_object *ob);
void FF_C_Slug(CK_object *a, CK_object *b);
void FF_C_SlugSlime(CK_object *a, CK_object *b);
void FF_R_SlugWalk(CK_object *ob);



void FF_SpawnSlug(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Slug;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 0x71;
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	new_object->yDirection = IN_motion_Down;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_slugwalk1));
}

void FF_T_Slug(CK_object *ob)
{
	if (!ob->user4)	// K1n9_Duk3 mod: don't slime with Nisasi
	{
		 int16_t rnd;	// cannot use unsigned here!

		rnd = US_RndT();
		if (ck_gameState.difficulty >= D_Hard)
			rnd -= 0x0D;
		
		if (rnd < 0x10)
		{
			// start sliming:
			ck_nextX = 0;	// K1n9_Duk3 mod: don't move!
			ob->xDirection = ob->posX < ck_keenObj->posX ? IN_motion_Right : IN_motion_Left;
			ob->currentAction = CK_ACTION(FF_ACT_slugpiss1);
			SD_PlaySound(FF_SLUGSLIMESND);
		}
	}
}

void FF_T_SlugPiss(CK_object *ob)
{
	CK_object *new_object = CK_GetNewObj(true);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	//new->zLayer = 0;	//redundant - GetNewObj already set it to 0
	new_object->posX = ob->posX;
	new_object->posY = ob->clipRects.unitY2 - 0x80;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_slugslime));
}

void FF_C_Slug(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		FF_KillKeen();
	}
	else if (b->type == CT_Stunner)
	{
		// Nisaba patch: Donut-Slug: Slug carries donut item like a snail shell. 
		// If shot, slug gets stunned and the donut can be collected as a 
		// regular point item.
		if (a->user1)	// if slug is carrying donut
		{
			RF_RemoveSpriteDrawUsing16BitOffset(&a->user3);
			
			if (true)	// because SpawnBonus() will cause a crash when this is NULL
			{
				CK_object *donut;
			
				donut = FF_SpawnBonus(a->clipRects.tileXmid, a->clipRects.tileY1, 9);
				ff_map.itemsTotal--;	// because SpawnBonus increased it
				donut->posX = a->posX + 4*16;	// adjust relative position
				donut->posY = a->posY + 4*16;	// here as well
				CK_SetAction(donut, donut->currentAction);	// set action again because position changed
			}
		}
		FF_StunObj(a, b, US_RndT() < 0x80 ? CK_ACTION(FF_ACT_slugstun) : CK_ACTION(FF_ACT_slugstunalt));
		a->velY = -24;
		a->velX = a->xDirection * 8;
	}
}

void FF_C_SlugSlime(CK_object *a, CK_object *b)
{
	
	switch (b->type)
	{
	case CT_Player:
		if (a->currentAction->next)
			FF_KillKeen();
		break;
#ifdef EXTRA_INTERACTIONS
	case FF_CT_CouncilMember:
	case FF_CT_CouncilMemberDone:
		CK_RemoveObj(a);
		break;
#endif
	}
	
}

void FF_R_SlugWalk(CK_object *ob)
{
	FF_R_WalkNormal(ob);
	// Nisaba patch: Donut-Slug: Slug carries donut item like a snail shell. 
	// If shot, slug gets stunned and the donut can be collected as a point item.
	if (ob->user1)
	{
		int16_t xoff, yoff;
		
		switch (ob->gfxChunk)
		{
		case FF_SLUGWALKL1SPR:
			xoff = 4*16;
			yoff = 4*16;
			break;
		case FF_SLUGWALKL2SPR:
			xoff = 7*16;
			yoff = 5*16;
			break;
		case FF_SLUGSLIMELSPR:
			xoff = 7*16;
			yoff = 4*16;
			break;
		case FF_SLUGWALKR1SPR:
			xoff = -4*16;
			yoff = 4*16;
			break;
		case FF_SLUGWALKR2SPR:
			xoff = -7*16;
			yoff = 5*16;
			break;
		default:	// just to make sure xoff and yoff are initialized...
			// no break here!
		case FF_SLUGSLIMERSPR:
			xoff = -7*16;
			yoff = 4*16;
			break;
		}
#if 1
		// draw donut behind slug sprite:
		RF_AddSpriteDrawUsing16BitOffset(&ob->user3, ob->posX+xoff, ob->posY+yoff, FF_POINTS5000ITEM1SPR, false, ob->zLayer);
		RF_PlaceSpriteAfter(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, &ob->user3);
#else
		// draw donut on top of slug sprite:
		RF_PlaceSpriteAfter(&ob->user3, ob->posX+xoff, ob->posY+yoff, FF_POINTS5000ITEM1SPR, false, ob->zLayer, &ob->sde);
#endif
	}
	if (!ob->topTI)
	{
		ob->timeUntillThink = 0;
		ob->xDirection = -ob->xDirection;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_slugfall));
	}
}



/*
==============================================================================
	SMIRKY / TREASURE EATER
==============================================================================
*/

/*
*/

#ifdef EXTRA_INTERACTIONS
#endif

/*
void CK_SmirkySearch(objtype *ob);
void CK_SmirkyTeleport(objtype *ob);
void CK_SmirkyCol(objtype *a, objtype *b);
void CK_SmirkyCheckTiles(objtype *ob);
void CK_SmirkyDraw(objtype *ob);
*/

/*
*/

#ifdef EXTRA_INTERACTIONS
#endif

/*
void CK_SpawnSmirky(int tileX, int tileY)
{
	objtype *new = GetNewObj(false);
	new->type = CT_Smirky;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new->zLayer = FOREGROUNDPRIORITY;
	new->posX = RF_TileToGlobal(tileX);
	new->posY = RF_TileToGlobal(tileY) - 0x180;
	new->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	new->yDirection = motion_Down;
	NewState(new, CK_ACTION(FF_ACT_Smirky0));
}

void CK_SmirkySearch(objtype *ob)
{
	int x, y, i;
	objtype *o;
	ob->currentAction = CK_ACTION(FF_ACT_SmirkyJump0);

	// Jump for item goodies
	for (o = player; o; o = o->next)
	{
		if (o->type == CT_Item)
		{
			if (o->active == ac_yes &&
					o->clipRects.unitX2 > ob->clipRects.unitX1 &&
					o->clipRects.unitX1 < ob->clipRects.unitX2 &&
					o->clipRects.unitY2 < ob->clipRects.unitY1 &&
					o->clipRects.unitY2 > ob->clipRects.unitY1 + 0x300)
			{
				ob->velX = 0;
				ob->velY = -48;
				return;
			}
		}
	}

	// Jump for tile goodies
	// DOS Keen increments a far pointer to the foreground plane, but it is
	// semantically equivalent to calculate the offset on every iteration
	for (y = ob->clipRects.tileY1 - 3; y < ob->clipRects.tileY1; y++)
	{
		for (x = ob->clipRects.tileX1; x < ob->clipRects.tileX2 + 1; x++)
		{
			int mf = TI_ForeMisc(FORESPOT(x, y)) & INTILE_TYPEMASK;
			if (mf == MF_Centilife || mf >= MF_Points100 && mf <= MF_Stunner)
			{
				ob->velX = 0;
				ob->velY = -48;
				return;
			}
		}
	}

	// Decide to teleport for some reason
	if (ob->user1 >= 2)
	{
		ob->currentAction = CK_ACTION(FF_ACT_SmirkyTele0);
		return;
	}

	// Decide to hop to the side if there's something to land on
	for (i = 0; i < 4; i++)
	{
		int y = ob->clipRects.tileY2 - 2 + i;
		int x = ob->clipRects.tileXmid + ob->xDirection * 4;
		int tf = TI_ForeTop(FORESPOT(x, y));
		if (tf)
		{
			ob->velX = ob->xDirection * 20;
			ob->velY = -24;
			return;
		}
	}

	// Decide to teleport if counter hits threshold
	if (++ob->user1 == 2)
	{
		SD_PlaySound(SMIRKYTELESND);
		ob->currentAction = CK_ACTION(FF_ACT_SmirkyTele0);
		return;
	}

	// Finally, just turn around and hop back where we came from
	ob->velX = -ob->xDirection * 20;
	ob->velY = -24;
}

void CK_SmirkyTeleport(objtype *ob)
{
	objtype *o;
	ob->user1 = 0;	// reset tleport timer
	for (o = player; o; o = o->next)
	{
		if (o->type == CT_Item && o->user1 >= 4)	// K1n9_Duk3 fix: skip key gems!
		{
			ob->posX = o->posX - 0x80;
			ob->posY = o->posY;
			NewState(ob, CK_ACTION(FF_ACT_SmirkyTele0));
		}
	}

	// Remove smirky if all the treasure items are gone
	if (!o)
	{
		RemoveObj(ob);
		return;
	}
}

void CK_SmirkyCol(objtype *a, objtype *b)
{
	if (b->type == CT_Item && b->user1 >= 4)	// K1n9_Duk3 fix: skip key gems!
	{
		b->type = CT_Friendly;
		b->zLayer = FOREGROUNDPRIORITY;
		ChangeState(b, CK_ACTION(FF_ACT_StolenItem0));
		SD_PlaySound(SMIRKYSTEALSND);
	}
	else if (b->type == CT_Stunner)
	{
		a->user1 = a->user2 = a->user3 = 0;
		a->user4 = a->type;
		a->type = CT_StunnedCreature;
		ExplodeShot(b);
		ChangeState(a, CK_ACTION(FF_ACT_SmirkyStunned0));
		a->velY -= 16;
	}
}
*/

#ifdef EXTRA_INTERACTIONS
void FF_SmirkyCheckTiles(CK_object *ob)
{
	int16_t x, y;
	for (y = ob->clipRects.tileY1; y <= ob->clipRects.tileY2; y++)
	{
		for (x = ob->clipRects.tileX1; x <= ob->clipRects.tileX2; x++)
		{
			int16_t mf = TI_ForeMisc(FORESPOT(x, y)) & INTILE_TYPEMASK;
			if (mf == MF_Centilife || mf >= MF_Points100 && mf <= MF_Stunner)
			{
				CK_object *new_object = CK_GetNewObj(true);
				new_object->type = CT_Friendly;
				new_object->zLayer = FOREGROUNDPRIORITY;
				new_object->clipped = CLIP_not;
				new_object->posX = RF_TileToUnit(x);
				new_object->posY = RF_TileToUnit(y);
				new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
				CK_SetAction2(new_object, CK_ACTION(FF_ACT_StolenItem0));
				
				RF_ReplaceTiles(&zeromap, 1, x, y, 1, 1);
				SD_PlaySound(FF_SMIRKYSTEALSND);	// K1n9_Duk3 addition (for consistency)
			}
		}
	}
}
#endif

/*
void CK_SmirkyDraw(objtype *ob)
{
	CK_SmirkyCheckTiles(ob);

	if (ob->topTI)
		ChangeState(ob, CK_ACTION(FF_ACT_Smirky0));

	if (ob->leftTI || ob->rightTI)
	{
		ob->user1++;
		ob->xDirection = -ob->xDirection;
		ob->velX = 0;
	}

	if (ob->topTI)
		ob->velY = 0;

	RF_PlaceSprite(&(ob->sde), ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
}
*/


/*
==============================================================================
	SKYPEST

	temp1 - upside-down flag for landing on ceilings
	temp2 - flying time (in tics)
==============================================================================
*/

#define SKYPEST_GROUNDDELAY		(3*70)
#define SKYPEST_CEILINGDELAY	(ck_gameState.difficulty == D_Easy? 2*70 : 4*70)
#define SKYPEST_MAXDELAY		(4*70)
//#define SKYPEST_MAXDELAY	(max(SKYPEST_GROUNDDELAY, SKYPEST_CEILINGDELAY))


void FF_T_PestFly(CK_object *ob);
void FF_C_PestFly(CK_object *a, CK_object *b);
void FF_C_Squashable(CK_object *a, CK_object *b);
void FF_T_PestRest(CK_object *ob);
void FF_R_Pest(CK_object *ob);

void FF_R_PestRest(CK_object *ob)
{
	FF_DrawSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, ob->user1, ob->zLayer);
}


void FF_SpawnSkypest(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Skypest;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	//new->zLayer = 0;	//redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	new_object->yDirection = US_RndT() < 0x80 ? IN_motion_Down : IN_motion_Up;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_pestfly1));
}

void FF_T_PestFly(CK_object *ob)
{
	if (ck_nextX || ck_nextY)	// if we have movement, this think function has already been executed for the current frame
		return;

	if (ob->user2 < SKYPEST_MAXDELAY)	// safety measure to avoid overflow issues
		ob->user2 += SD_GetSpriteSync();

	if (US_RndT() < SD_GetSpriteSync())
		ob->xDirection = -ob->xDirection;

	if (ob->yDirection == IN_motion_Up && US_RndT() < SD_GetSpriteSync())
		ob->yDirection = IN_motion_Down;

	if (ob->yDirection == IN_motion_Down && US_RndT() < SD_GetSpriteSync() * 2)
		ob->yDirection = -ob->yDirection;

	FF_AccelerateX(ob, ob->xDirection, 20);
	FF_AccelerateY(ob, ob->yDirection, 20);
}

void FF_C_PestFly(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		FF_KillKeen();
	}
	// There's no "else" in the original Keen 4 EXE so, so let's omit it for
	// the sake of consistency, even if there's no change to behaviors
	if (b->type == CT_Stunner)
	{
		if (b->xDirection == IN_motion_Right)
			a->velX = 20;
		else if (b->xDirection == IN_motion_Left)
			a->velX = -20;
		else if (b->yDirection == IN_motion_Down)
			a->velY = 20;
		else if (b->yDirection == IN_motion_Up)
			a->velY = -20;

		FF_ExplodeShot(b);
	}
}

void FF_C_Squashable(CK_object *a, CK_object *b)
{
	if (b->currentAction == CK_ACTION(CK_ACT_keenPogo1) ||
			b->currentAction == CK_ACTION(CK_ACT_keenPogo2) ||
			b->currentAction == CK_ACTION(CK_ACT_keenPogo3))
	{
		CK_SetAction2(a, CK_ACTION(FF_ACT_squashedpest));
		SD_PlaySound(FF_SQUISHSND);
		a->type = CT_Friendly;
	}
}

void FF_T_PestRest(CK_object *ob)
{
	if (ob->user1)
	{
		ob->yDirection = IN_motion_Down;
		ob->velY = 16;
		ck_nextY = 144;
	}
	else
	{
		ob->yDirection = IN_motion_Up;
		ob->velY = -16;
		ck_nextY = -144;
	}
	ob->user1 = ob->user2 = 0;	// reset flying time and upside-down flag
}

void FF_R_Pest(CK_object *ob)
{
	// K1n9_Duk3 addition (2024-07-03): never fly into water:
	if ((TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2)) & INTILE_TYPEMASK) == MF_Water
		|| INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2) == SKYPESTBLOCK)
	{
		// bounce back:
		ob->xDirection = -ob->xDirection;
		ob->yDirection = -ob->yDirection;
		ob->velX = -ob->velX;
		ob->velY = -ob->velY;
		// undo last movement (move out of water/blocker -- hopefully):
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		CK_SetAction(ob, ob->currentAction);
		goto draw;	// don't let the other code change the object's x/y direction
	}
	// end of addition
	
	if (ob->bottomTI && !ob->rightTI && !ob->leftTI)
	{
		if ((ob->bottomTI & SLOPEMASK) == 1 && ob->user2 >= SKYPEST_CEILINGDELAY	// only land on flat ceilings
			&& ck_gameState.currentLevel != MAP_DOPEFISHLEVEL)	// but never land on ceilings in level 11
		{
			ob->user1 = drawflipped;	// landed on ceiling
			CK_SetAction2(ob, CK_ACTION(FF_ACT_pestrest1));
			FF_R_PestRest(ob);
			return;
		}
		ob->velY = 8;
		ob->yDirection = IN_motion_Down;
	}

	if (ob->topTI && !ob->rightTI && !ob->leftTI)
	{
		if (ob->user2 >= SKYPEST_GROUNDDELAY && (ob->topTI != TI_TOP_BREAKABLE))	// K1n9_Duk3 mod: check here, so Skypest can bounce off the bad floor
		{
			ob->posY += 0x80;
			ob->user1 = 0;	//landed on floor
			CK_SetAction2(ob, CK_ACTION(FF_ACT_pestrest1));
		}
		else
		{
			ob->velY = -8;
			ob->yDirection = IN_motion_Up;
		}
	}

	if (ob->leftTI)
	{
		ob->velX = 0;
		ob->xDirection = IN_motion_Left;
	}

	if (ob->rightTI)
	{
		ob->velX = 0;
		ob->xDirection = IN_motion_Right;
	}

draw:
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer);
}

#undef SKYPEST_GROUNDDELAY
#undef SKYPEST_CEILINGDELAY
#undef SKYPEST_MAXDELAY

/*
==============================================================================
	MIMROCK
==============================================================================
*/

/*

void CK_MimrockWait(objtype *ob);
void CK_MimrockCheckJump(objtype *ob);
void CK_MimrockCol(objtype *a, objtype *b);
void CK_MimrockAirCol(objtype *a, objtype *b);
void CK_MimrockJumpDraw(objtype *ob);
void CK_MimrockBounceDraw(objtype *ob);


void CK_SpawnMimrock(int tileX, int tileY)
{
	objtype *new = GetNewObj(false);
	new->type = CT_Mimrock;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new->zLayer = FOREGROUNDPRIORITY;
	new->posX = RF_TileToGlobal(tileX);
	new->posY = RF_TileToGlobal(tileY) - 0xD0;
	new->xDirection = motion_Right;
	new->yDirection = motion_Down;
	NewState(new, CK_ACTION(FF_ACT_Mimrock0));
}

void CK_MimrockWait(objtype *ob)
{
	if (ABS((unsigned)(ob->clipRects.unitY2 - player->clipRects.unitY2)) <= 0x500)
	{
		if (ABS((unsigned)(ob->posX - player->posX)) > 0x300)
		{
			if (player->posX < ob->posX)
			{
				if (player->xDirection == motion_Left)
				{
					ob->xDirection = motion_Left;
					ob->currentAction = CK_ACTION(FF_ACT_MimrockWalk0);
				}
			}
			else
			{
				if (player->xDirection == motion_Right)
				{
					ob->xDirection = motion_Right;
					ob->currentAction = CK_ACTION(FF_ACT_MimrockWalk0);
				}
			}
		}
	}
}

void CK_MimrockCheckJump(objtype *ob)
{
	if ((ABS((unsigned)(ob->clipRects.unitY2 - player->clipRects.unitY2)) <= 0x500) &&
			(ob->xDirection == player->xDirection))
	{
		if (ABS((unsigned)(ob->posX - player->posX)) < 0x400)
		{
			ob->velX = ob->xDirection * 20;
			ob->velY = -40;
			ytry = ob->velY * tics;
			ob->currentAction = CK_ACTION(FF_ACT_MimrockJump0);
		}
	}
	else
	{
		ob->currentAction = CK_ACTION(FF_ACT_Mimrock0);
	}
}

void CK_MimrockCol(objtype *a, objtype *b)
{
	if (b->type == CT_Stunner)
	{
		a->user1 = a->user2 = a->user3 = 0;
		a->user4 = a->type;
		a->type = CT_StunnedCreature;
		ExplodeShot(b);
		ChangeState(a, CK_ACTION(FF_ACT_MimrockStunned0));
		a->velY -= 16;
	}
}

void CK_MimrockAirCol(objtype *a, objtype *b)
{
	if (b->type == CT_Player)
	{
		KillKeen();
	}
	else
	{
		CK_MimrockCol(a,b);
	}
}

void CK_MimrockJumpDraw(objtype *ob)
{
	if (ob->topTI)
	{
		SD_PlaySound(SOUND_MIMROCKJUMP);
		ob->velY = -20;
		ChangeState(ob, CK_ACTION(FF_ACT_MimrockBounce0));
	}

	if (ob->leftTI || ob->topTI)
		ob->velX = 0;

	if (ob->topTI || ob->bottomTI)
		ob->velY = 0;

	RF_PlaceSprite(&(ob->sde), ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
}

void CK_MimrockBounceDraw(objtype *ob)
{
	if (ob->topTI)
	{
		SD_PlaySound(SOUND_MIMROCKJUMP);
		ChangeState(ob, CK_ACTION(FF_ACT_Mimrock0));
	}

	if (ob->leftTI || ob->topTI)
		ob->velX = 0;

	if (ob->topTI)
		ob->velY = 0;

	RF_PlaceSprite(&(ob->sde), ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
}
*/

/*
==============================================================================
	PatPats
==============================================================================
*/


void FF_T_PatPat(CK_object *ob); 


void FF_SpawnPatPat(int16_t tileX, int16_t tileY, int16_t dir)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_PatPat;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->clipped = CLIP_not;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->xDirection = IN_motion_Right;
	new_object->yDirection = IN_motion_Down;
	if (dir == 0) 
	{
		CK_SetAction(new_object, CK_ACTION(FF_ACT_PatPatD0));
		new_object->posY -= 0xC0;
	} else {
		CK_SetAction(new_object, CK_ACTION(FF_ACT_PatPatU0));
		new_object->posY -= 0x40;
	}

	new_object->user1 = new_object->currentAction->timer;
	new_object->user2 = dir;
}

void FF_T_PatPat(CK_object *ob) 
{
	int16_t delta;
	int16_t xdist;
	uint16_t ydist;
	bool gofast = false;

	if (abs(ob->posX - ck_keenObj->posX) < 3*256)
	{
		if (ob->user2 == 0)
		{
			ydist = ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY1;
			if (ydist < 8*256) 
				gofast = true;
		}
		else
		{
			ydist = ck_keenObj->clipRects.unitY2 - ob->clipRects.unitY1;
			if (ydist < 8*256) 
				gofast = true;
		}
	}

	if (gofast)
	{
		delta = 4 * SD_GetSpriteSync();
	}
	else
	{
		delta = SD_GetSpriteSync();
	}

	if ((ob->user1 -= delta) <= 0)
	{
		int16_t extra = -ob->user1;
		CK_SetAction2(ob, ob->currentAction->next);
		ob->user1 = ob->currentAction->timer - extra;
	}
}

/*
==============================================================================
	COUNCIL JANITOR

	The bucket is spawned directly after spawning the janitor, so the janitor
	can always access the bucket object via its 'next' pointer. That means
	we don't need to store a pointer in the user variables, so there are no
	pointer issues when loading a saved game.

	temp1 - used for steps countdown and animation count
	temp2 - janitor number
	temp3 - cooldown after touching a sleeping Keen
	temp4 - Nisasi pointer

	BUCKET

	temp1 - address of janitor's sprite pointer (sprite)
	temp2
	temp3
	temp4
==============================================================================
*/

//extern statetype s_councilstand;


void FF_SpawnCouncilMember(int16_t tileX, int16_t tileY, int16_t number)
{	
	int16_t addr;
	
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_CouncilMember;
	//new->zLayer = 0;	//redundant - GetNewObj already set it to 0
	new_object->user2 = number;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 369;
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	new_object->yDirection = IN_motion_Down;
	addr = 0; // KEEN WORLDS: its bucket is drawn after the janitor's sprite (RF_PlaceSpriteAfter); not ordered here yet
	CK_SetAction(new_object, CK_ACTION(FF_ACT_councilwalk1));

	// also spawn the bucket along with the council janitor:
	new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Bucket;
	//new->zLayer = 0;	// must have the same priority as the janitor for RF_PlaceSpriteAfter
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->gfxChunk = FF_COUNCILBUCKETSPR;
	new_object->user1 = addr;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_bucket));
}

void FF_T_Council(CK_object *ob)
{
	// handle steps countdown:
	if (ob->user1)
		ob->user1--;

	// handle cooldown:
	if (ob->user3)
		ob->user3--;

	// random pause
/*
	if ((tics << 3) > US_RndT())
		ob->currentAction = CK_ACTION(FF_ACT_councilstand);
*/
}

void FF_T_CouncilMop(CK_object *ob)
{
	if (--ob->user1 <= 0)
	{
		ob->user1 = 12;	// this is how many steps the janitor must move before he can use the bucket again
		CK_SetAction2(ob, CK_ACTION(FF_ACT_councilwalk1));

		// make the bucket visible again:
		ob = ob->next;	// bucket is always spawned directly after the janitor, so ob->next is the bucket object
		ob->gfxChunk = FF_COUNCILBUCKETSPR;
		ob->visible = true;	// force redraw of the bucket!
	}
}

void FF_C_Council(CK_object *ob, CK_object *hit)
{
	if (hit->type == FF_CT_Bucket && hit == ob->next && !ob->user1)
	{
		if ((ob->xDirection == IN_motion_Left && ob->posX == hit->posX)
			|| (ob->xDirection == IN_motion_Right && ob->posX + 8*16 == hit->posX))
		{
			hit->gfxChunk = 0;	// make bucket invisible
			hit->visible = true;	// force redraw

			ob->user1 = 3;	// this is how many times the the bucket animation plays
			CK_SetAction2(ob, CK_ACTION(FF_ACT_councilmop1));
		}
	}
	// Note: touching Keen is handled in KeenContact() in CK_KEEN.C
}

void FF_R_Council(CK_object *ob)
{
	int16_t xdir = ob->xDirection;

	FF_R_Walk(ob);

	if (xdir != ob->xDirection)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_councilturn1));
		ob->timeUntillThink = 0;
		FF_R_Draw(ob);	// to replace the sprite placed by R_Walk()
	}
}

void FF_R_Bucket(CK_object *ob)
{
	RF_PlaceSpriteAfter(&(ob->sde), ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, ob->user1);
}

//statetype s_councilstand = {COUNCILSTANDLSPR, COUNCILSTANDRSPR, step, false, push_down, 120, 0, 0, NULL, NULL, R_Draw, CK_ACTION(FF_ACT_councilwalk1)};


// HACK:
// Left and right sprites are swapped here, since these actions are used AFTER
// the janitor turns around, but we want to pretend he's still facing the other
// direction.

/*
==============================================================================
	LINDSEY

	temp1 - initial y position
	temp4 - associated message number
==============================================================================
*/


void FF_T_Lindsey(CK_object *ob);
void FF_R_Lindsey(CK_object *ob);


void FF_SpawnLindsey(int16_t tileX, int16_t tileY, int16_t num)
{
	CK_object *new_object = CK_GetNewObj(false);
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	//new->zLayer = 0;	//redundant
	new_object->clipped = CLIP_not;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->user1 = new_object->posY = RF_TileToUnit(tileY) - 0x100;
	new_object->user4 = num;
	new_object->yDirection = IN_motion_Down;
	if (num == 0)
	{
		new_object->type = FF_CT_Lindsey;
	}
	else if (num == -1)
	{
		new_object->type = FF_CT_LindseySpecial;
	}
	else
	{
		new_object->type = FF_CT_LindseyHidden;
	}
	CK_SetAction(new_object, CK_ACTION(FF_ACT_lindsey1));
}

void FF_T_Lindsey(CK_object *ob)
{
	// special Lindsey shows a message and ends the level after the smoke clears
	if (ob->type == FF_CT_LindseySpecial)
	{
		if ((ob->user4 += SD_GetSpriteSync()) > 33)	// need at least 32 tics for the smoke to clear
		{
			ck_gameState.levelState = ex_bonuscomplete;
		}
	}

	FF_AccelerateY(ob, ob->yDirection, 8);
	if ((int16_t)(ob->user1 - ob->posY) > 0x20)
		ob->yDirection = IN_motion_Down;

	if ((int16_t)(ob->posY - ob->user1) > 0x20)
		ob->yDirection = IN_motion_Up;
}

void FF_R_Lindsey(CK_object *ob)
{
	if (ob->type == FF_CT_LindseyHidden)
		RF_RemoveSpriteDraw(&ob->sde);
	else
		FF_R_Draw(ob);
}

void FF_LindseyVanish(CK_object *ob)
{
	ob->type = FF_CT_LindseyVanish;	// no further interaction with this object
	ob->gfxChunk = FF_SMIRKYPOOF1SPR;
	ob->user1 = FF_SMIRKYPOOF4SPR;
	ob->posX += -8*16;
	CK_SetAction2(ob, CK_ACTION(FF_ACT_Smoke));
}

/*
==============================================================================
	XkyLicks / OPTIQUAT

	temp1 = shuffletimer
	temp2 = chase mode flag
	temp3 = mushroom tics
	temp4 = Nisasi pointer
==============================================================================
*/





void FF_SpawnXkyLick(int16_t tileX, int16_t tileY);
void FF_T_XkyLickBounce(CK_object *ob);
void FF_T_XkyLickShuffle(CK_object *ob);
void FF_C_XkyLick(CK_object *a, CK_object *b);
void FF_C_XkyLickAttack(CK_object *a, CK_object *b);
void FF_R_XkyLickAir(CK_object *ob);
void FF_R_XkyLick(CK_object *ob);





#define MUSHROOMDELAY (-42)	// counted in bounces (must be negative!)


void FF_SpawnXkyLick(int16_t tileX, int16_t tileY)
{

	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_XkyLick;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->user1 = new_object->posY = RF_TileToUnit(tileY);
	new_object->user3 = MUSHROOMDELAY;
	new_object->xDirection = US_RndT() < 0x80? IN_motion_Right : IN_motion_Left;
	new_object->yDirection = IN_motion_Down;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_XkyLickHop2));	// K1n9_Duk3 mod: start in falling state
}

void FF_T_XkyLickBounce(CK_object *ob) 
{
	CK_object *target = ck_keenObj;
	int16_t dx, dy=0;

	if (ob->user3 > 0)
	{
		if (--ob->user3 <= 0)
		{
			ob->user3 = MUSHROOMDELAY;
		}
		else
		{
			if (ff_map.lastCheckpoint) // KEEN WORLDS: null until a checkpoint is touched
			{
			target = ff_map.lastCheckpoint;
			dy = 30*16;	// because of different sprite offsets
			}
		}
	}
	else if (ob->user3 < 0)
	{
		ob->user3++;
	}

	dx = target->posX - ob->posX;
	dy += target->posY - ob->posY;

	if (((ob->user2 && abs(dx) < 0xA00) || 
			(!ob->user2 && abs(dx) < 0x700)) && 
			dy < 0x300 && dy > -0x700 &&
			!ob->user4)
	{
		// Chase
		ob->user2 = 1;
		ob->xDirection = ob->posX > target->posX ? IN_motion_Left : IN_motion_Right;

		// Start attack animation when TARGET is really close:

		if (dy >= -0x100 && dy <= 0x100 && !ob->user4)
		{
			if (ob->xDirection == IN_motion_Right && dx > -0x20 && dx < 0x180 ||
					ob->xDirection == IN_motion_Left && dx < 0x20 && dx > -0x200)
			{
				SD_PlaySound(FF_OPTIQUATLICKSND);
				CK_SetAction2(ob, CK_ACTION(FF_ACT_XkyLickFlame0));
				return;
			}
		}

		if (abs(dx) < 0x300) 
		{
			ob->velX = ob->xDirection * 16;
			ob->velY = -16;
		} 
		else 
		{
			ob->velX = ob->xDirection * 32;
			ob->velY = -24;
		}
	}
	else	// TARGET is too far away
	{
		ob->user2 = 0;

		// Jump around 
		if (US_RndT() < 0x30) 
		{
			ob->xDirection = -ob->xDirection;
			ob->velX = ob->xDirection * 4;
			ob->velY = -32 - (US_RndT() >> 4);
		} 
		else if (US_RndT() < 0x40) 
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_XkyLickShuffle0));
			ob->user1 = 8 + (US_RndT() >> 4);
		} 
		else 
		{
			ob->velX = ob->xDirection * 16;
			ob->velY = -16 - (US_RndT() >> 5);
		}

		// Randomly start chasing a checkpoint mushroom
		if (!ob->user3)
		{
			//if (US_RndT() == 255)	// extremely rare, but not impossible
			if (US_RndT() < 0x80)	// pretty common
			{
				ob->user3 = 4;	// four bounces should be enough, otherwise the checkpoint would be too far away anyway
				ob->user2 = 1;	// start in chase mode
			}
		}
		else if (ob->user3 > 0)
		{
			ob->user3 = MUSHROOMDELAY;
		}
	}
}

void FF_T_XkyLickShuffle(CK_object *ob)
{
	if (ob->user3 < 0)
	{
		ob->user3++;
	}

	if (--ob->user1 == 0)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_XkyLickSearch0));
	}
}

void FF_C_XkyLick(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
		//StunObj(a, b, US_RndT() < 0x80 ? CK_ACTION(FF_ACT_XkyLickStunned0) : CK_ACTION(FF_ACT_XkyLickStunned1));
		FF_StunObj(a, b, CK_ACTION(FF_ACT_XkyLickStunned0));
		a->velY = -24;
		a->velX = a->xDirection * 8;
		a->zLayer = 2;
	}
}

void FF_C_XkyLickAttack(CK_object *a, CK_object *b)
{
	if (b->gfxChunk == FF_CPBIGSHROOMSPR
		//&& a->user3 > 0		// add the temp3 check if accidently licking a mushroom shouldn't count
		&& a->currentAction == CK_ACTION(FF_ACT_XkyLickFlame7))
	{
		if (  (a->xDirection < 0 && a->clipRects.unitX1 > b->clipRects.unitX1)
			|| (a->xDirection > 0 && a->clipRects.unitX2 < b->clipRects.unitX2))
		{
			CK_SetAction2(a, CK_ACTION(FF_ACT_XkyLickShroomed));
			a->velY = -24;
			a->velX = -a->xDirection * 8;
		}
		a->user3 = MUSHROOMDELAY;
	}
	else if (b->type == CT_Player)
	{
		FF_KillKeen();
	}
	else
	{
		FF_C_XkyLick(a, b);
	}
}

void FF_R_XkyLickAir(CK_object *ob) 
{
#if 0
	if (ob->leftTI || ob->rightTI)
	{

	}
#endif

	if (ob->topTI)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_XkyLickHop3));
	}

	FF_R_Draw(ob);
}

void FF_R_XkyLick(CK_object *ob)
{
	// turn around when hitting a wall or walking off a ledge:
	if ( (ob->xDirection == IN_motion_Right && ob->leftTI != 0)
	  || (ob->xDirection == IN_motion_Left && ob->rightTI != 0)
	  || (ob->topTI == 0) )
	{
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		ob->xDirection = -ob->xDirection;
		ob->timeUntillThink = US_RndT() / 32;
		CK_SetAction2(ob, ob->currentAction);
		
		// just start falling if still not touching the ground:
		if (!ob->topTI)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_XkyLickHop2));
			ob->timeUntillThink = ob->velX = ob->velY = 0;
		}
	}

	FF_R_Draw(ob);
}


#if 0
/*
==============================================================================
	Yorp
==============================================================================
*/

void FF_SpawnYorp(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Yorp;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 0x100;
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	new_object->yDirection = IN_motion_Down;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_YorpWalk0));
}

void FF_T_YorpWalk(CK_object *ob)
{
	int16_t r = US_RndT();
	if (r < 0x20) 
	{
		ob->user1 = 5 + (r >> 3);
		CK_SetAction(ob, CK_ACTION(FF_ACT_YorpSearch0));
		return;
	}

	r = US_RndT();
	if (r < 0x20) 
	{
		CK_SetAction(ob, CK_ACTION(FF_ACT_YorpJump0));
		ob->velX = 10;
		ob->velY = -10 - (r >> 2);
		return;
	}
}

void FF_T_YorpSearch(CK_object *ob)
{
	if (--ob->user1 == 0) 
	{
		ob->xDirection = ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid?
			IN_motion_Right : IN_motion_Left;

		CK_SetAction(ob, CK_ACTION(FF_ACT_YorpWalk0));
	}
}

void FF_C_Yorp(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
		FF_StunObj(a, b, US_RndT() < 0x80 ? CK_ACTION(FF_ACT_XkyLickStunned0) : CK_ACTION(FF_ACT_XkyLickStunned1));
		a->velY = -24;
		a->velX = a->xDirection * 8;
	} 
	else if (b->type == CT_Player)
	{
		if (b->velY > 0 && 
				(b->clipRects.unitX2 > a->clipRects.unitX1 && b->clipRects.unitX1 < a->clipRects.unitX1) || 
				(b->clipRects.unitX1 < a->clipRects.unitX2 && b->clipRects.unitX2 > a->clipRects.unitX2))
		{
			CK_SetAction(a, CK_ACTION(FF_ACT_YorpFazed0));
		}
		else
		{
			CK_PhysPushX(b, a);
		}
	}
}
#endif

/*
==============================================================================
	EARL FISH
==============================================================================


void CK_SpawnEarlFish(int tileX, int tileY);
void CK_EarlFishRecover(objtype *ob);
void CK_EarlFishGlide(objtype *ob);
void CK_EarlFishDraw(objtype *ob);


// temp1: vertical movement timer
// temp2: glide decay timer

void CK_SpawnEarlFish(int tileX, int tileY)
{
	objtype *new = GetNewObj(false);
	new->type = CT_EarlFish;
	//new->active = ac_yes;	// redundant - GetNewObj always sets active to ac_yes
	//new->clipped = cl_midclip;	// also redundant
	//new->zLayer = 0;	// also redundant
	new->posX = RF_TileToGlobal(tileX);
	new->posY = RF_TileToGlobal(tileY);
	new->xDirection = US_RndT() < 0x80 ? motion_Right : motion_Left;
	new->yDirection = motion_None;
	NewState(new, CK_ACTION(FF_ACT_EarlFishSwim0));
}

#define EARLFISH_MIN_GLIDE 9
#define EARLFISH_MIN_GLIDE_Y 2
void CK_EarlFishRecover(objtype *ob) {
			ob->user1 = 0;
			ob->velX = ob->xDirection * 24;
			ob->velY = ((int)US_RndT() - 128) >> 4;
			ob->yDirection = ob->velY > 0? motion_Down : motion_Up;
			if (ABS(ob->velY) < EARLFISH_MIN_GLIDE_Y) {
				ob->velY = ob->yDirection * EARLFISH_MIN_GLIDE_Y;
			}
}

void CK_EarlFishGlide(objtype *ob) {

	FrictionY(ob);
	// slow down a bit
	if (ABS(ob->velY) < EARLFISH_MIN_GLIDE_Y) {
		ob->velY = ob->yDirection * (EARLFISH_MIN_GLIDE_Y);
	}

	FrictionX(ob);
	if (ABS(ob->velX) < EARLFISH_MIN_GLIDE) {
		ob->velX = ob->xDirection * EARLFISH_MIN_GLIDE;
		if (++ob->user1 > TickBase) {
			ob->velX >>= 1; 
			if (US_RndT() < 0x08) {
				ob->currentAction = CK_ACTION(FF_ACT_EarlFishRecover0);
			}
		}
	}
}


void CK_EarlFishDraw(objtype *ob)
{
	if (ob->rightTI || ob->leftTI)
	{
		ob->velX = 0;
		ob->xDirection = -ob->xDirection;
	}

	if (ob->topTI || ob->bottomTI) {
		ob->velY = -ob->velY;
		ob->velY >>= 1;
	}

	RF_PlaceSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, spritedraw, ob->zLayer);
}
*/

/*
==============================================================================
	DUNG BEETLE

	walks back & forth; just eye candy
==============================================================================
*/



void FF_SpawnDungBeetle(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_DungBeetle;
	//new->active = ac_yes;	// redundant - GetNewObj always sets active to ac_yes
	//new->clipped = cl_midclip;	// also redundant
	//new->zLayer = 0;	// also redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) + 5*16;
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Right : IN_motion_Left;
	//new->yDirection = motion_None;	// also redundant
	CK_SetAction(new_object, CK_ACTION(FF_ACT_DungBeetle1));
}

/*
==============================================================================
	CHEMOLAIN

	teleports from log to log, stuns Keen on contact and can't be stunned

	temp1 - countdown until next teleport
	temp2 - color cycle counter
	temp3 - log pointer
	temp4 - Nisasi pointer
==============================================================================
*/






void FF_SpawnChemolain(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Chemolain;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->clipped = CLIP_not;
	new_object->zLayer = 1;	// logs need to use a lower priority value than this one!
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	//new->xDirection = motion_None;	// redundant
	//new->yDirection = motion_None;	// redundant
	CK_SetAction(new_object, CK_ACTION(FF_ACT_ChemStand));
	new_object->user1 = 100;
}

void FF_ChemCountdown(CK_object *ob)
{
	int16_t temp = 1;

	// count down at 4x the speed if Keen is near:
	if (abs(ck_keenObj->posX - ob->posX) < 10*256)
		temp <<= 2;

	if (ob->user1 > temp)
	{
		ob->user1 -= temp;
	}
	else if (!ob->user4)	// don't teleport with Nisasi
	{
		ob->user1 = 100;	// reset countdown
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ChemWarpOut1));
		if (ob->user3)
		{
			 CK_object *log = FF_ObjFromUser(ob->user3);
			// old log is no longer occupied:
			log->type = FF_CT_LogPlatform;
		}
	}
}

void FF_T_ChemStand(CK_object *ob)
{
	int16_t temp = US_RndT();

	if (temp < 2)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ChemLick1));
	}
	else if (temp < 16)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ChemLook));
	}
	else if (CK_ObjectVisible(ob))	// only count down when visible (for ALWAYS ACTIVE)
	{
		FF_ChemCountdown(ob);
	}
}

void FF_T_ChemCycle(CK_object *ob)
{
	if (ob->user2-- > 0)
	{
		if (ob->gfxChunk >= FF_CHEMCYCLE6SPR || ob->gfxChunk < FF_CHEMCYCLE1SPR)
		{
			ob->gfxChunk = FF_CHEMCYCLE1SPR;
		}
		else
		{
			ob->gfxChunk++;
		}
		ob->visible = true;	// force redraw
	}
	else
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ChemStand));
	}
	// allow Chemolain to warp out during the color cycle animation
	// (to make sure Keen and the Chemolain won't get stun-locked)
	FF_ChemCountdown(ob);
}

void FF_T_ChemHidden(CK_object *ob)
{
	CK_object *o, *best=NULL;
//	boolean visible = false;

	// find a (random) log platform:
	for (o=ck_keenObj; o; o=o->next)
	{
#if 1	// (new version)
		// only log platforms and not the one it's currently sitting on!
		if (o != FF_ObjFromUser(ob->user3) && o->type == FF_CT_LogPlatform
			// log platform must not be farther than 16 tiles away from ob
			&& abs(o->clipRects.tileXmid - ob->clipRects.tileXmid) <= 16
			&& abs(o->clipRects.tileY1 - ob->clipRects.tileY2) <= 16)
		{
			// make sure to remember the first log platform we find,
			// in case we don't find any others.
			if (!best)
			{
				best = o;
			}
			else
			{
				// use a 50% chance to prefer the new one over the old.
				if (US_RndT() < 128)
					best = o;
			}
		}
#else	// (old version)
		// only log platforms and not the one it's currently sitting on!
		if (o != FF_ObjFromUser(ob->user3) && o->type == FF_CT_LogPlatform)
		{
			// make sure to remember the first log platform we find,
			// in case we don't find any others.
			if (!best)
			{
				best = o;
				visible = CK_ObjectVisible(o);
			}
			else if (CK_ObjectVisible(o))
			{
				if (!visible)
				{
					// "best" log is not visible, but current log is, so of course we
					// want to use the current one as the "best". But let's say there
					// is a very small chance that we keep the old one.
					if (US_RndT() < 250)
					{
						best = o;
						visible = true;
					}
				}
				else
				{
					// both logs are visible, so we'll use a 50% chance to prefer the
					// new one over the old.
					if (US_RndT() < 128)
						best = o;
				}
			}
			else if (!visible)
			{
				// neither the "best" nor the current log are visible, so we'll use
				// a 50% chance to prefer the new one over the old.
				if (US_RndT() < 128)
					best = o;
			}
		}
#endif
	}

	// scan complete, now let's see if we have found a new log
	if (best)
	{
		// okay, move the chemolain to the new log and make it appear:
		ob->user3 = FF_ObjToUser(best);
		ob->posX = best->posX;	// Note: contact function sets the correct position
		ob->posY = best->posY;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ChemWarpIn1));
		best->type = FF_CT_OccupiedLog;
	}
	// if no log is found, the actor waits and keeps searching until it finds one
}

void FF_C_ChemRide(CK_object *a, CK_object *b)
{
	// HACK:
	// We abuse this contact function to let the chemolain
	// "ride" the log it's currently occupying. For this to
	// work, the chemolain needs to have its hitbox extended
	// down a bit, so that it always touches the log even if
	// the log moves down.

	if (!a->user3)
	{
		switch (b->type)
		{
		case FF_CT_LogPlatform:
			b->type = FF_CT_OccupiedLog;
			a->user3 = FF_ObjToUser(b);
			break;
		default:
			return;
		}
	}

	if (a->user3 == FF_ObjToUser(b))
	{
		// adjust x & y position correctly and make sure hitboxes still overlap
		ck_nextX = b->posX+4*16 - a->posX;
		ck_nextY = b->clipRects.unitY1 - 34*16 - a->posY;
		CK_PhysUpdateNormalObj(a);
	}
}

void FF_C_Chem(CK_object *a, CK_object *b)
{
	switch (b->type)
	{
	case CT_Player:
		FF_StunKeen(a->clipRects.unitXmid);
		CK_SetAction2(a, CK_ACTION(FF_ACT_ChemCycle));
		a->user2 = 12;	// cycle through all 6 frames twice
		break;
	case CT_Stunner:
		FF_ExplodeShot(b);
		CK_SetAction2(a, CK_ACTION(FF_ACT_ChemCycle));
		a->user2 = 12;	// cycle through all 6 frames twice
		break;
	default:
		FF_C_ChemRide(a, b);
		break;
	}
}






/*
==============================================================================
	LOG

	- a (swimming) wooden platform that Keen can land on
	- sinks when Keen is standing on it (but not as fast as falling platforms)
	- can also move left/right
	- is blocked by tiles, but can also be blocked by block icons
	- stops moving left/right when hitting the ground (while sinking)
	- continues moving in the old direction when it rises back up again

	temp1 - initial y position (for sinking and rising back up)
	temp2 - previous x direction
==============================================================================
*/


void FF_SpawnLogPlatform(int16_t tileX, int16_t tileY, int16_t xdir)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_LogPlatform;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	//new->clipped = cl_fullclip;	// Tile-Hitbox differs from Object-Hitbox!
	// Note: when using full clipping, you need to add a hard-coded hitbox
	// definition in FullClipToWalls in CK_PHYS.C
	//new->zLayer = 0;	// redundant // logs are drawn behind any other actors
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = new_object->user1 = RF_TileToUnit(tileY) + 3*16;
	new_object->xDirection = xdir;
	//new->yDirection = motion_None;	// redundant
	CK_SetAction(new_object, CK_ACTION(FF_ACT_log));
}

void FF_T_Log(CK_object *ob)
{
	if (ob == ck_keenState.platform)
	{
		// Keen is standing on this log, so make it sink slowly:
		ck_nextY = SD_GetSpriteSync();

		if ((int16_t)(ob->posY - ob->user1) >= 16*16)
		{
			// make Keen fall off:
			ob->type = FF_CT_SlipperyLog;
			ck_keenState.platform = NULL;
		}
	}
	else if (ob->posY > ob->user1)
	{
		// Keen is no longer standing on this log and it hasn't reached its
		// initial y position again, so make it rise (faster than it sinks):
		 int16_t speed = -(SD_GetSpriteSync()*4);
		if ((ck_nextY = ob->user1 - ob->posY) < speed)
			ck_nextY = speed;
	}
	else
	{
		if (ob->type == FF_CT_SlipperyLog)
			ob->type = FF_CT_LogPlatform;
		ck_nextY = ob->user1 - ob->posY;	// move back to initial position
	}

	// Note: engine handles left/right movement if we have an x direction

	// if log is moving, check for [B] icons 
	// (2 tiles below the log's initial y position!):
	if (ob->xDirection == IN_motion_Left)
	{
		if (INFOSPOT(ob->clipRects.tileX1, RF_UnitToTile(ob->user1) + 2) == PLATFORMBLOCK)
		{
			ob->xDirection = IN_motion_Right;
		}
	}
	else if (ob->xDirection == IN_motion_Right)
	{
		if (INFOSPOT(ob->clipRects.tileX2, RF_UnitToTile(ob->user1) + 2) == PLATFORMBLOCK)
		{
			ob->xDirection = IN_motion_Left;
		}
	}
}

void FF_R_Log(CK_object *ob)
{
	if (ob->leftTI || ob->rightTI)
	{
		// hit a left/right wall, so turn around:
		ob->xDirection = -ob->xDirection;
	}
	if (ob->topTI)
	{
		// log has hit the ground
		if (ob == ck_keenState.platform)
		{
			// log is sinking, so stop moving left/right:
			if (ob->xDirection != IN_motion_None)
			{
				ob->user2 = ob->xDirection;
				ob->xDirection = IN_motion_None;
			}
		}
		else
		{
			// log is not sinking, so it must have hit a slope.
			// just turn around when that happens
			ob->xDirection = -ob->xDirection;
		}
	}
	else
	{
		// not touching the ground anymore, so start moving left/right again:
		if (ob->xDirection == IN_motion_None)
			ob->xDirection = ob->user2;
	}
	FF_R_Draw(ob);
}


/*
==============================================================================
	SALAMANDER

	stays in place, vanishes forever when Keen comes near, just eye candy
==============================================================================
*/



void FF_SpawnSalamander(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Salamander;
	//new->active = ac_yes;	// redundant - GetNewObj always sets active to ac_yes
	//new->clipped = cl_midclip;	// also redundant
	//new->zLayer = 0;	// also redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->xDirection = IN_motion_Left;
	//new->yDirection = motion_None;	// also redundant
	CK_SetAction(new_object, CK_ACTION(FF_ACT_SalamanderStand));
}

void FF_SalamanderThink(CK_object *ob)
{
	if (abs(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid) < 6*256
		&& (int16_t)(ob->clipRects.unitY1 - ck_keenObj->clipRects.unitY2) < 3*256
		&& (int16_t)(ck_keenObj->clipRects.unitY1 - ob->clipRects.unitY2) < 2*256)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_SalamanderVanish1));
	}
	else if (ob->currentAction == CK_ACTION(FF_ACT_SalamanderStand))
	{
		 int16_t rnd = US_RndT();
		if (rnd < 16)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_SalamanderLick1));
		}
	}
}



/*
==============================================================================
	WORMOUTH

	implementation taken straight from KEEN 4
==============================================================================
*/


void FF_SpawnWormouth(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Wormouth;
	//new->active = ac_yes;	// redundant - GetNewObj always sets active to ac_yes
	//new->zLayer = 0;	// redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) + 0x8F;
	if (US_RndT() < 0x80)
	{
		new_object->xDirection = IN_motion_Right;
	}
	else
	{
		new_object->xDirection = IN_motion_Left;
	}
	new_object->yDirection = IN_motion_Down;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Wormouth));
	
	// HACK: modify speed setting based on difficulty:
	if (ck_gameState.difficulty >= D_Hard)
		CK_ACTION(FF_ACT_Wormouth)->velX = 21;
	else
		CK_ACTION(FF_ACT_Wormouth)->velX = 16;
}

void FF_WormouthLookRightThink(CK_object *ob)
{
	if (ck_keenObj->posX > ob->posX)
	{
		ob->xDirection = IN_motion_Right;
		ob->currentAction = CK_ACTION(FF_ACT_Wormouth);
	}
}

void FF_WormouthLookThink(CK_object *ob)
{
	if (ck_keenObj->posX > ob->posX)
	{
		ob->xDirection = IN_motion_Right;
	}
	else
	{
		ob->xDirection = IN_motion_Left;
	}
}

void FF_WormouthLookLeftThink(CK_object *ob)
{
	if (ck_keenObj->posX < ob->posX)
	{
		ob->xDirection = IN_motion_Left;
		ob->currentAction = CK_ACTION(FF_ACT_Wormouth);
	}
}

void FF_WormouthThink(CK_object *ob)
{
	int16_t xdist, ydist;

	xdist = ck_keenObj->posX - ob->posX;
	ydist = ck_keenObj->clipRects.unitY2 - ob->clipRects.unitY2;
	if ((xdist < -3*256 || xdist > 3*256) && US_RndT() < 6)
	{
		ck_nextX = 0;	// no movement on state transition!
		ob->currentAction = CK_ACTION(FF_ACT_WormouthLook1);
	}
	else if (ydist >= -256 && ydist <= 256)
	{
		if (ob->xDirection == IN_motion_Right && xdist > 8*16 && xdist < 24*16
			|| ob->xDirection == IN_motion_Left && xdist < -8*16 && xdist > -32*16)
		{
			SD_PlaySound(FF_WORMMOUTHBITESND);
			ck_nextX = 0;	// no movement on state transition!
			ob->currentAction = CK_ACTION(FF_ACT_WormouthBite1);
		}
	}
}

void FF_WormouthLookContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Stunner)
	{
		FF_StunObj(ob, hit, CK_ACTION(FF_ACT_WormouthStunned));
		// Note: our StunObj() always makes the creature jump, which wasn't the case in KEEN 4!
		ob->velY = 0;	//no jump for you!
	}
}

void FF_WormouthBiteContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player)
	{
		if (ob->xDirection == IN_motion_Right && ob->posX > hit->posX
			|| ob->xDirection == IN_motion_Left && ob->posX < hit->posX)
			return;
		
		FF_KillKeen();
	}
	else
	{
		FF_WormouthLookContact(ob, hit);
	}
}

void FF_WormouthWalkReact(CK_object *ob)
{
	FF_R_Walk(ob);
	if (!ob->topTI)
	{
		ob->timeUntillThink = 0;
		ob->xDirection = -ob->xDirection;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_WormouthFall));
	}
}


/*
==============================================================================
	SHRUBBERY / LEGBERY

	Randomly takes a peek while standing still. Stands up if Keen is nearby
	and walks towards Keen. Temporarily stuns Keen when touching him.

	temp1 - number of repeats for the long looking animation
==============================================================================
*/

//extern statetype s_ShrubberyLook3;
//extern statetype s_ShrubberyLook4;

void FF_SpawnShrubbery(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Shrubbery;
	//new->active = ac_yes;	// redundant - GetNewObj always sets active to ac_yes
	//new->zLayer = 0;	// redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 11*16;
	new_object->xDirection = IN_motion_Left;
	new_object->yDirection = IN_motion_Down;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Shrubbery));
}

void FF_T_Shrubbery(CK_object *ob)
{
	// stand up if Keen is near:
	if (abs((uint16_t)(ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2)) <= 3*256 
		&& abs((uint16_t)(ob->posX - ck_keenObj->posX)) <= 5*256)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberyStandUp));
		if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid)
			ob->xDirection = IN_motion_Right;
		else
			ob->xDirection = IN_motion_Left;
	}
	else if (ob->gfxChunk == FF_SHRUBBERYSPR)	// only when not already looking around
	{
		// randomly start looking around:
		int16_t rnd = US_RndT();

		if (rnd < 0x08)
		{
			ob->user1 = 4;	// play the looking animation twice
			ob->gfxChunk = FF_SHRUBBERYLOOK1SPR;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberyLook2));
		}
		else if (rnd < 0x10)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberyLook1));
		}
	}
}

void FF_ShrubberyLookThink(CK_object *ob)
{
	// Note: this is used in an think action, so it's safe to re-use ticcount in here
	if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
	{
		ob->actionTimer -= 10;
		ob->gfxChunk++;
		if (ob->gfxChunk > FF_SHRUBBERYLOOK4SPR)
		{
			if (--ob->user1 <= 0)
			{
				CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberyLook5));
			}
			else
			{
				ob->gfxChunk = FF_SHRUBBERYLOOK2SPR;
			}
		}
	}
	FF_T_Shrubbery(ob);
}

void FF_T_ShrubberyWalk(CK_object *ob)
{
	// turn around to follow Keen
	if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid)
		ob->xDirection = IN_motion_Right;
	else
		ob->xDirection = IN_motion_Left;

	// sit down when Keen is too far away
	if (!CK_ObjectVisible(ob)
		|| abs((uint16_t)(ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2)) > 3*256 
		|| abs((uint16_t)(ob->posX - ck_keenObj->posX)) > 5*256)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberySitDown));
	}
}

void FF_C_Shrubbery(CK_object *ob, CK_object *hit)
{
	int16_t ymove;

	switch (hit->type)
	{
	case CT_Player:
		//if (ob->gfxChunk != SHRUBBERYSPR)
		{
			FF_StunKeen(ob->clipRects.unitXmid);
			// change shrubbery state to avoid stun-locking Keen:
			if (ob->currentAction != CK_ACTION(FF_ACT_ShrubberyFall))
			{
				CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberySitDown));
				ob->timeUntillThink = (2*70)/10;	//also wait 2 seconds before looking or standing up again
			}
		}
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		if (ob->gfxChunk < FF_SHRUBBERYGETUPSPR)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_Shrubbery));
			ob->timeUntillThink = (4*70)/10;	// don't look or stand up for 4 seconds (s_Shrubbery thinks every 10 tics)
			break;
		}
		ymove = ob->clipRects.unitY1;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_ShrubberyFall));

		// make sure the top of the bush/hitbox stays in place:
		if ((ymove -= ob->clipRects.unitY1) < 0)	// only move up, don't move down!
			FF_MoveObjVert(ob, ymove);
		// Note: we can't move the bush down because then it might be pushed partially
		// into the ground, causing the engine to move it to the left or to the right
		// in an attempt to fix that. And we don't want that to happen.

		// jump up a bit:
		ob->timeUntillThink = 0;
		if ((ob->velY -= 0x18) < -0x30)
			ob->velY = -0x30;
		break;
	}
}

void FF_R_ShrubberyFall(CK_object *ob)
{
	if (ob->topTI)
	{
		// object has hit the ground, back to waiting state
		CK_SetAction2(ob, CK_ACTION(FF_ACT_Shrubbery));
		ob->velY = 0;
		ob->velX = 0;
		ob->timeUntillThink = (4*70)/10;	// don't look or stand up for 4 seconds (s_Shrubbery thinks every 10 tics)
	}
	FF_R_Draw(ob);
}

//statetype s_ShrubberyLook3   = {SHRUBBERYLOOK3SPR,   SHRUBBERYLOOK3SPR,   step, false, push_none, 10,  0, 0, T_Shrubbery,     C_Shrubbery, R_Draw, CK_ACTION(FF_ACT_ShrubberyLook4)};
//statetype s_ShrubberyLook4   = {SHRUBBERYLOOK4SPR,   SHRUBBERYLOOK4SPR,   step, false, push_none, 10,  0, 0, T_Shrubbery,     C_Shrubbery, R_Draw, CK_ACTION(FF_ACT_ShrubberyLook5)};

/*
==============================================================================
	BOUNDER / PUFFPUFF

	Hovers in place, randomly floats left or right, follows Keen after getting
	shot.

	temp1 - initial y position (for hovering)
	temp2 - remaining chase ticks
	temp3 - amplitude
	temp4 - slow acceleration counter
==============================================================================
*/

#define BOUNDER_FLOAT_VELX 4
#define BOUNDER_CHASE_VELX 24
#define BOUNDER_FLOAT_VELY 16
#define BOUNDER_BOUNCE_VELY 14
#define BOUNDER_ACCELERATES true
#define BOUNDER_MIN_AMPLITUDE 24
#define BOUNDER_ACCELERATION_DELAY 20


void FF_SpawnBounder(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Bounder;
	//new->active = ac_yes;	// redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 9*16;
	new_object->user1 = new_object->posY - 2*256;
	new_object->user3 = BOUNDER_MIN_AMPLITUDE;
	//new->xDirection = motion_None;	// redundant
	new_object->yDirection = IN_motion_Up;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_BounderFloat1c));
}

void FF_T_Bounder(CK_object *ob)
{
	 int16_t temp;

	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	if (ob->gfxChunk != FF_BOUNDERFLOATSHOTSPR)
	{
		if (ob->user2 > SD_GetSpriteSync())
		{
			ob->user2 -= SD_GetSpriteSync();
		}
		else
		{
			ob->user2 = 0;
		}
	}

#if BOUNDER_ACCELERATES
	temp = ob->user2? BOUNDER_CHASE_VELX : BOUNDER_FLOAT_VELX;
	if (ob->xDirection && ob->gfxChunk != FF_BOUNDERFLOATSHOTSPR && abs(ob->velX) <= temp)
	{
		if (temp == BOUNDER_FLOAT_VELX)
		{
			if ((ob->user4 += SD_GetSpriteSync()) > 0)
			{
				ob->user4 -= BOUNDER_ACCELERATION_DELAY;
				ob->velX += ob->xDirection;
			}
			ck_nextX = ob->velX * SD_GetSpriteSync();
		}
		else
			FF_AccelerateX(ob, ob->xDirection, temp);
	}
#else
	if (ob->xDirection && ob->gfxChunk != FF_BOUNDERFLOATSHOTSPR)
	{
		FF_AccelerateX(ob, ob->xDirection, ob->user2? BOUNDER_CHASE_VELX : BOUNDER_FLOAT_VELX);
	}
#endif
	else
	{
		FF_FrictionX(ob);
	}

	if (ob->yDirection == IN_motion_Up && ob->posY < ob->user1-ob->user3)
	{
		ob->yDirection = IN_motion_Down;
	}
	else if (ob->yDirection == IN_motion_Down && ob->posY > ob->user1+ob->user3)
	{
		ob->yDirection = IN_motion_Up;
	}

	temp = ob->velY;
	FF_AccelerateY(ob, ob->yDirection, BOUNDER_FLOAT_VELY);

	//if (((temp ^ ob->velY) & 0x8000) && temp >= 0 && ob->gfxChunk != BOUNDERFLOATSHOTSPR)
	if ((((temp ^ ob->velY) & 0x8000) && temp >= 0 || ob->topTI) && ob->gfxChunk != FF_BOUNDERFLOATSHOTSPR)
	{
		int16_t xdist;
		xdist = ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;
		if (ob->user2)
		{
			// chasing Keen
			temp = xdist;

			if (abs(xdist) > 15*256)
				ob->user2 = false;	// stop chasing Keen

			// we don't have a SIGN macro that leaves 0 as 0, so only use the macro on non-0 values
			if (temp)
				temp = SIGN(temp);
			ob->xDirection = temp;
		}
		else
		{
			// random movement
			temp = US_RndT();
			if (temp < 100)
			{
				temp = IN_motion_Left;
			}
			else if (temp < 200)
			{
				temp = IN_motion_Right;
			}
			else
			{
				temp = IN_motion_None;
			}
			ob->xDirection = temp;
		}

#if (!BOUNDER_ACCELERATES)
		// I don't remember why I set the the xspeed value here. It prevents the
		// acceleration code above from working correctly and it uses different
		// speed values than the acceleration code.
		if (ob->user2)
			ob->velX = temp * 40;
		else
			ob->velX = temp * 20;
#endif

		// adjust amplitude based on xdist
		ob->user3 = 1*256 - (abs(xdist) >> 2);
		if (ob->user3 < BOUNDER_MIN_AMPLITUDE)
			ob->user3 = BOUNDER_MIN_AMPLITUDE;
	}

	if (ob->gfxChunk != FF_BOUNDERFLOATSHOTSPR)
	{
		// change state according to new direction
		if (ob->xDirection != IN_motion_None)
		{
			if (ob->velY < 0)
			{
				ob->currentAction = CK_ACTION(FF_ACT_BounderFloat1);
			}
			else
			{
				ob->currentAction = CK_ACTION(FF_ACT_BounderFloat2);
			}
		}
		else
		{
			if (ob->velY < 0)
			{
				ob->currentAction = CK_ACTION(FF_ACT_BounderFloat1c);
			}
			else
			{
				ob->currentAction = CK_ACTION(FF_ACT_BounderFloat2c);
			}
		}
	}
}

void FF_C_Bounder(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		CK_SetAction2(ob, CK_ACTION(FF_ACT_BounderStunned));
	/*
		for (ob=player; ob; ob=ob->next)
		{
			if (ob->type == CT_Bounder)
			{
	*/
		ob->user2 = 9*70;	// start chasing Keen for 9 seconds
		ob->user3 = BOUNDER_MIN_AMPLITUDE;
	/*
			}
		}
	*/
		break;
	}
}

void FF_R_Bounder(CK_object *ob)
{
#if BOUNDER_ACCELERATES
	if (ob->topTI && ob->velY > 0 || ob->bottomTI && ob->velY < 0)
	{
		if ((ob->velY = -ob->velY) < 0)
			ob->yDirection = IN_motion_Up;
		else
			ob->yDirection = IN_motion_Down;
		
		// bounce off the floor/ceiling:
		if (((ob->user1 - ob->posY) ^ (ob->yDirection)) & 0x8000)
			ob->velY = ob->yDirection * BOUNDER_BOUNCE_VELY;
	}
	if (ob->leftTI && ob->velX > 0 || ob->rightTI && ob->velX < 0)
	{
		ob->velX = -ob->velX;
		if (ob->xDirection != IN_motion_None)
		{
			if (ob->velX < 0)
				ob->xDirection = IN_motion_Left;
			else
				ob->xDirection = IN_motion_Right;
		}
	}
#else
	if (ob->topTI || ob->bottomTI)
	{
		ob->yDirection = -ob->yDirection;
		ob->velY = -ob->velY;
	}
	else if (ob->leftTI || ob->rightTI)
	{
		ob->xDirection = -ob->xDirection;
		ob->velX = -ob->velX;
	}
#endif
	FF_R_Draw(ob);
}



/*
==============================================================================
	TENTACLE / PURPLE TENTACLE

	Stands in place until Keen comes near, then it moves out of its hole and
	slams the ground in an attempt to hurt Keen. Has a 3 second delay after
	each attack.
==============================================================================
*/


void FF_SpawnTentacle(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Tentacle;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_TentacleWait));
}

void FF_T_Tentacle(CK_object *ob)
{
	int16_t xdist =  ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;
	if (abs(ob->posY - ck_keenObj->posY) < 6*256)
	{
		if (xdist < 0 && xdist >= (int16_t)(-3.5*256))
		{
			ob->xDirection = IN_motion_Left;
			ob->currentAction = CK_ACTION(FF_ACT_TentacleAttack1);
		}
		else if (xdist >= 0 && xdist <= (int16_t)(3.5*256))
		{
			ob->xDirection = IN_motion_Right;
			ob->currentAction = CK_ACTION(FF_ACT_TentacleAttack1);
		}
	}
}

void FF_T_TentacleAttack(CK_object *ob)
{
	SD_PlaySound(FF_TENTACLESWOOSHSND);
}

void FF_C_Tentacle(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);	// shot explodes but doesn't do anything else
		break;
	}
}


/*
==============================================================================
	POLEKNAWEL / LEVAR

	Slides down to the bottom of a pole and randomly throws projectiles. Falls
	to the ground when shot. Walks on the ground and randomly throws
	projectiles. Can also get on poles again.
	
	temp4 - Nisasi pointer
==============================================================================
*/



void FF_SpawnPoleknawel(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Poleknawel;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->xDirection = US_RndT() < 0x80? IN_motion_Left : IN_motion_Right;
	if ((TI_ForeMisc(FORESPOT(tileX, tileY)) & INTILE_TYPEMASK) == MF_Pole)
	{
		new_object->posX -= 0x78;
		new_object->clipped = CLIP_not;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_PoleknawelPole1));
	}
	else
	{
		//new->clipped = cl_midclip;	//redundant - GetNewObj always sets needtoclip to cl_midclip
		CK_SetAction(new_object, CK_ACTION(FF_ACT_PoleknawelLook));
	}
}

void FF_T_PoleknawelLand(CK_object *ob)
{
#ifdef EXTRA_INTERACTIONS 
	// only start looking when NOT grabbed by a Nisasi:
	if (!ob->user4)
#endif
	{
		ob->currentAction = CK_ACTION(FF_ACT_PoleknawelLook);
	}
}

void FF_T_PoleknawelWalk(CK_object *ob)
{
	// Note: the distance from the middle of the tile in the following check should match the walking speed of the Poleknawel.
	if (abs((ob->clipRects.unitXmid & 0xFF) - 256/2) <= 0x40	//middle of the hitbox is near the middle of the tile
		&& (TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2)) & INTILE_TYPEMASK) == MF_Pole)
	{
		ob->currentAction = CK_ACTION(FF_ACT_PoleknawelPole1);
		ck_nextX = 256/2 - (ob->clipRects.unitXmid & 0xFF);	//move to the middle of the tile
		ob->clipped = CLIP_not;
	}
	else
	{
		 int16_t rnd;	// cannot use unsigned here!
		
		rnd = US_RndT();
		if (!(ck_gameState.difficulty == D_Easy))
			rnd -= 0x10;
		
		if (rnd < 0x06)
		{
		ck_nextX = 0;	// no movement on state transition!
		ob->currentAction = CK_ACTION(FF_ACT_PoleknawelPoop);
		ob->xDirection = ob->posX < ck_keenObj->posX ? IN_motion_Right : IN_motion_Left;
		SD_PlaySound(FF_EGGCRACKSND);
		}
	}
}

void FF_T_PoleknawelSlide(CK_object *ob)
{
	if ((TI_ForeMisc(FORESPOT(ob->clipRects.tileXmid, RF_UnitToTile(ob->clipRects.unitY2+0x20))) & INTILE_TYPEMASK) == MF_Pole)
	{
		ck_nextY = 0x20;
	}
	else
	{
		 int16_t rnd;	// cannot use unsigned here!
		
		rnd = US_RndT();
		if (!(ck_gameState.difficulty == D_Easy))
			rnd -= 0x10;
		
		if (rnd < 0x06)
		// throw a projectile
		ob->currentAction = CK_ACTION(FF_ACT_PoleknawelThrow);
		// TODO: play a sound here?
	}
}

void FF_T_PoleknawelThrow(CK_object *ob)
{
	CK_object *new_object = CK_GetNewObj(true);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	//new->zLayer = 0;	// redundant
	new_object->posX = ob->clipRects.unitXmid - 7*16;
	new_object->yDirection = IN_motion_Down;

	if (ob->currentAction == CK_ACTION(FF_ACT_PoleknawelPoop))
	{
		new_object->posY = ob->clipRects.unitY1;

		new_object->velY = -32;
		new_object->velX = 32*ob->xDirection;
	}
	else
	{
		new_object->posY = ob->clipRects.unitY2;

		// shoot in a random direction (and with random speed):
		new_object->velX = (US_RndT()-0x80)/4;	// random speed between -32 and 31
	}

/*
	// shoot in Keen's direction:
	if (player->clipRects.unitXmid < ob->clipRects.unitXmid - TILEGLOBAL)
	{
		new->xDirection = motion_Left;
		new->velX = -0x18;
	}
	else if (player->clipRects.unitXmid > ob->clipRects.unitXmid + TILEGLOBAL)
	{
		new->xDirection = motion_Right;
		new->velX = 0x18;
	}
	// otherwise the shot flies straight down (values for this are already set by GetNewObj)
*/
	CK_SetAction(new_object, CK_ACTION(FF_ACT_PoleknawelShot1));
	SD_PlaySound(FF_EGGCRACKSND);
}
/*
void PoleknawelPoop(objtype *ob)
{
	objtype *new = GetNewObj(true);
	new->type = CT_Friendly;
	new->active = ac_removable;
	//new->zLayer = 0;	// redundant
	new->posX = ob->clipRects.unitXmid - 8*PIXGLOBAL;
	new->posY = ob->clipRects.unitY2 - 7*PIXGLOBAL;
	NewState(new, CK_ACTION(FF_ACT_PoleknawelPool1));
}
*/
void FF_C_Poleknawel(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		FF_ObjDropNisasi(ob);
		ob->clipped = CLIP_normal;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_PoleknawelFall));

		// Make the creature jump up a bit
		if ((ob->velY -= 0x18) < -0x30)
			ob->velY = -0x30;
		ob->timeUntillThink = 0;	// allow it to jump without delay
		break;
	}
}

void FF_R_PoleknawelWalk(CK_object *ob)
{
	FF_R_Walk(ob);
	if (!ob->topTI)
	{
		ob->timeUntillThink = 0;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_PoleknawelFall));
	}
}

void FF_R_PoleknawelFall(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_PoleknawelLand));
	}
	else if (ob->bottomTI)
	{
		ob->velY = 0;
	}
	FF_R_Draw(ob);
}

void FF_KnawlReact(CK_object *ob)
{
	if (ob->topTI)	// hit the ground ?
	{
		// turn into pool when hitting the ground:
		CK_SetAction2(ob, CK_ACTION(FF_ACT_PoleknawelPool1));

	/*
		// bounce off the ground and remove after x bounces:
		ob->velY = -ob->velY / 2;	// bounce back at half the speed
		if (++ob->user1 > 2)	//remove when it hits the ground for the 2nd time
		{
			RemoveObj(ob);
			return;
		}
	*/
	}
	if (ob->bottomTI)	// hit the ceiling?
	{
		ob->velY = -ob->velY / 2;	// bounce back at half the speed
	}
	if (ob->leftTI || ob->rightTI)	// hit a wall?
	{
		ob->velX = -ob->velX / 2;	// bounce back at half the speed
	}
	FF_R_Draw(ob);
}


/*
==============================================================================
	KITTEN / FAT RED CAT

	Sits on its fat butt and meows at random. Starts rolling towards Keen
	when getting shot or when Keen is close. Explodes when touching Keen.

	temp4 - Nisasi pointer
==============================================================================
*/


void FF_SpawnKitten(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Kitten;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 10*16;
	new_object->xDirection = US_RndT() < 0x80? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_Down;
	//new->clipped = cl_midclip;	//redundant - GetNewObj always sets needtoclip to cl_midclip
	CK_SetAction(new_object, CK_ACTION(FF_ACT_KittenIdle1));
}

void FF_T_KittenIdle(CK_object *ob)
{
	// start rolling when Keen is close:
	if (abs(ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid) < 8*256
		&& abs(ck_keenObj->posY - ob->posY) < 3*256)
	{
		FF_ObjDropNisasi(ob);
		CK_SetAction2(ob, CK_ACTION(FF_ACT_KittenRoll1));
	}
	else if (ob->currentAction == CK_ACTION(FF_ACT_KittenIdle1) && CK_ObjectVisible(ob))
	{
		if (US_RndT() == 4)
		{
			SD_PlaySound(FF_KITTENMEOWSND);
			ob->currentAction = CK_ACTION(FF_ACT_KittenMeow1);
		}
	}
}

void FF_T_KittenRoll(CK_object *ob)
{
	// stop rolling when too far away from Keen:
	if (abs(ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid) > 9*256
		|| abs(ck_keenObj->posY - ob->posY) > 5*256)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_KittenIdle1));
	}
	else
	{
		if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
			ob->xDirection = IN_motion_Left;
		else
			ob->xDirection = IN_motion_Right;
	}
}

void FF_T_KittenExplode(CK_object *ob)
{
	SD_PlaySound(FF_KITTENEXPLODESND);
}

void FF_C_Kitten(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		CK_SetAction2(ob, CK_ACTION(FF_ACT_KittenBlow1));
		ob->clipped = CLIP_not;	// no clipping for the smoke cloud
		ob->timeUntillThink = 0;	// allow Kitten to think, otherwise the explosion sound might not play
		FF_ObjDropNisasi(ob);
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		CK_SetAction2(ob, CK_ACTION(FF_ACT_KittenRoll1));
		ob->timeUntillThink = 20;
		FF_ObjDropNisasi(ob);
		break;
	}
}


/*
==============================================================================
	GELLYGHOST / GELANTIN SPOOP

	Floats up and down. Stuns Keen on touch. Can be pushed by shooting it.

	temp1 - bottom y
	temp2 - visibility state (see enum)
==============================================================================
*/

#define GG_UP_SPEED 18

enum {GG_NORMAL, GG_VANISH, GG_APPEAR, GG_HIDDEN};


void FF_SpawnGellyGhost(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_GellyGhost;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = new_object->user1 = RF_TileToUnit(tileY);
	new_object->xDirection = US_RndT() < 0x80? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_Down;
	new_object->velY = 4;
	//new->clipped = cl_midclip;	//redundant - GetNewObj always sets needtoclip to cl_midclip
	new_object->gfxChunk = FF_GELLYGHOSTFADE4SPR;
	new_object->user2 = GG_HIDDEN;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_GGhostIdle));
}

void FF_T_GellyGhost(CK_object *ob)
{
	int16_t dx, dy;

	if (ob->velY == 0)
	{
		ob->velY = 6;
		ob->yDirection = IN_motion_Down;
		if (ob->velX == 0)
		{
			ob->xDirection = -ob->xDirection;
		}
	}
	else if (ob->posY >= ob->user1)
	{
		ob->velY = -GG_UP_SPEED;
		ob->yDirection = IN_motion_Up;
	}

	if (ob->velY < 0)
	{
		FF_FrictionY(ob);
		FF_FrictionX(ob);
	}
	else
	{
		ck_nextY = ob->velY * SD_GetSpriteSync();
		if (ob->velX < -4 || ob->velX > 4)
			FF_FrictionX(ob);
		else
			FF_AccelerateX(ob, ob->xDirection, 4);
	}

	//
	// change frame to match current state
	//

	dx = ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid;
	dy = ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2;
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;

	if (dx > 5*256 || dy > 5*256)
	{
		if (ob->user2 != GG_VANISH && ob->user2 != GG_HIDDEN)
		{
			ob->user2 = GG_VANISH;
			if (ob->gfxChunk < FF_GELLYGHOSTFADE1SPR)
				ob->gfxChunk = FF_GELLYGHOSTFADE1SPR;
			ob->actionTimer = 0;
		}
	}
	else if (dx < 4*256 && dy < 4*256)
	{
		if (ob->user2 == GG_VANISH || ob->user2 == GG_HIDDEN)
		{
			ob->user2 = GG_APPEAR;
			if (ob->gfxChunk < FF_GELLYGHOSTFADE1SPR)
				ob->gfxChunk = FF_GELLYGHOSTFADE4SPR;
			ob->actionTimer = 0;
		}
	}

	if (ob->user2 == GG_VANISH)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			if (ob->gfxChunk == FF_GELLYGHOSTFADE4SPR)
			{
				ob->user2 = GG_HIDDEN;
			}
			else
			{
				ob->gfxChunk++;
			}
		}
	}
	else if (ob->user2 == GG_APPEAR)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			if (ob->gfxChunk-- == FF_GELLYGHOSTFADE1SPR)
				ob->user2 = GG_NORMAL;
		}
	}

	if (ob->user2 != GG_NORMAL)
		return;

	if (ob->velX <= -16)
		ob->gfxChunk = FF_GELLYGHOSTSHOTL1SPR;
	else if (ob->velX >= 16)
		ob->gfxChunk = FF_GELLYGHOSTSHOTR1SPR;
	else if (ob->velX < -8)
		ob->gfxChunk = FF_GELLYGHOSTSHOTL2SPR;
	else if (ob->velX > 8)
		ob->gfxChunk = FF_GELLYGHOSTSHOTR2SPR;
	else
	{
		if (ob->xDirection == IN_motion_Left)
		{
			if (ob->posY >= ob->user1-2*16)
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATL2SPR;
			}
			else if (ob->velY < -8)
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATL3SPR;
			}
			else if (ob->velY < 0)
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATL4SPR;
			}
			else
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATL1SPR;
			}
		}
		else
		{
			if (ob->posY >= ob->user1-2*16)
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATR2SPR;
			}
			else if (ob->velY < -8)
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATR3SPR;
			}
			else if (ob->velY < 0)
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATR4SPR;
			}
			else
			{
				ob->gfxChunk = FF_GELLYGHOSTFLOATR1SPR;
			}
		}
	}
}

void FF_C_GellyGhost(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_StunKeen(ob->clipRects.unitXmid);
		break;
	case CT_Stunner:
		// push ghost in shot direction and adjust temp1 (bottom y)
		if (hit->xDirection == IN_motion_Left)
		{
			ob->xDirection = IN_motion_Right;
			ob->velX -= 24;
			if (ob->velX < -32)
				ob->velX = -32;
		}
		else if (hit->xDirection == IN_motion_Right)
		{
			ob->xDirection = IN_motion_Left;
			ob->velX += 24;
			if (ob->velX > 32)
				ob->velX = 32;
		}
		else if (hit->yDirection == IN_motion_Up)
		{
			ob->yDirection = IN_motion_Up;
			ob->velY -= 24;
			ob->user1 -= 3*256;
		}
		else if (hit->yDirection == IN_motion_Down)
		{
			ob->yDirection = IN_motion_Down;
			ob->velY += 24;
			ob->user1 += 3*256;
		}
		FF_ExplodeShot(hit);
		break;
	}
}

void FF_R_GellyGhost(CK_object *ob)
{	
	if (ob->topTI)
	{
		ob->user1 = ob->posY - 4*16;
	}
	if (ob->bottomTI)
	{
		ob->velY = 0;
		ob->user1 = ob->posY + GG_UP_SPEED*GG_UP_SPEED;
	}
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = 0;
	}

	if (ob->user2 == GG_HIDDEN)
		RF_RemoveSpriteDraw(&ob->sde);
	else
		FF_R_Draw(ob);
}


#undef GG_UP_SPEED

/*
==============================================================================
	COLLECTIBLES / PLUSHIES

	Special GOODIES that Keen can collect. They can be spawned as regular items
	in the level or as non-interactive items in the BWB level (if collected).

	temp1 - unique bit for the item (1 << itemnumber)
	temp2 - first sprite (same as for regular bonus items)
	temp3 - last sprite + 1 (same as for regular bonus items)
==============================================================================
*/

// Note: Keen touching the item is handled in KeenContact() in CK_KEEN.C !

void FF_SpawnCollectible(int16_t tileX, int16_t tileY, int16_t number)
{
	int16_t bit;

	bit = 1 << number;

	if (ck_gameState.currentLevel == MAP_BWBMEGAROCKET)
	{
		// we're in the BWB level here

		// offsets to allow pixel-perfect positions in BWB cockpit:
		// (these are in pixel units, relative to the tile position)
		// Yorp, Bear, Keen, Vort, Garg, Mort, 
		static  int16_t xoffs[6] = {-3, +3, -6, +1, +0, +3};
		static  int16_t yoffs[6] = {-2, -3, +6, +0, -5, +1};

		// only spawn the collectible if the player has collected it:
		if (ff_state.collectiblesTotal & bit)
		{
			CK_object *new_object = CK_GetNewObj(false);
			new_object->clipped = CLIP_not;
			new_object->type = CT_Friendly;
			new_object->posX = RF_TileToUnit(tileX) + RF_PixelToUnit(xoffs[number]);
			new_object->posY = RF_TileToUnit(tileY) + RF_PixelToUnit(yoffs[number]);
			new_object->gfxChunk = FF_COLLECTIBLES_LUMP_START + number*2;
			new_object->user2 = new_object->gfxChunk;
			new_object->user3 = new_object->gfxChunk + 1;
			CK_SetAction(new_object, CK_ACTION(FF_ACT_bonus1));

			CA_MarkGrChunk(new_object->gfxChunk);
		}
	}
	else	// not in BWB level
	{
		// don't spawn the collectible if the player already collected it:
		if ((ff_state.collectiblesTotal & bit))
			return;

		// check if there already is an identical collectible in the level
		{
			CK_object *ob;
			
			for (ob=ck_keenObj; ob; ob=ob->next)
			{
				if (ob->type == FF_CT_Collectible && ob->user1 == bit)
				{
					// randomly decide to keep the old one or remove it and spawn a new one
					if (US_RndT() < 0x80)
					{
						return;	//keep the old one
					}
					CK_RemoveObj(ob);	// remove the old one and spawn a new one
					break;
				}
			}
		}
		
		// if we get here, we either didn't find an identical collectible or we
		// removed it from the level, so we definitely need to spawn a new one
		{
			CK_object *new_object = CK_GetNewObj(false);
			new_object->type = FF_CT_Collectible;
			new_object->posX = RF_TileToUnit(tileX);
			new_object->posY = RF_TileToUnit(tileY);
			if (number == 0)	// if it's the Yorp
			{
				new_object->posX += 0*16;
				new_object->posY += -10*16;
			}
			new_object->yDirection = -1;
			new_object->gfxChunk = FF_COLLECTIBLES_LUMP_START + number*2;
			new_object->user2 = new_object->gfxChunk;
			new_object->user3 = new_object->gfxChunk + 2;
			new_object->user1 = bit;
			CK_SetAction(new_object, CK_ACTION(FF_ACT_bonus1));

			CA_MarkGrChunk(new_object->gfxChunk);
			CA_MarkGrChunk(new_object->gfxChunk+1);
		}
	}
}

/*
==============================================================================
	ROOTS / THORNBUSH

	Intended to be placed at the base of a pole. Kill Keen on touch and need
	to be shot 2 times to remove them, each shot changing the frame.
	Regenerates after 4 seconds if not fully destroyed.

	temp1 - regeneration timer
==============================================================================
*/


void FF_SpawnRoots(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->clipped = CLIP_not;
	new_object->type = FF_CT_Roots;
	new_object->posX = RF_TileToUnit(tileX) - 4*16;
	new_object->posY = RF_TileToUnit(tileY) - 21*16;
	new_object->gfxChunk = FF_ROOTS1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Roots));
}

void FF_T_Roots(CK_object *ob)
{
	if (ob->gfxChunk > FF_ROOTS2SPR)
	{
		if ((ob->user1 -= SD_GetSpriteSync()) < 0)
		{
			if (ob->gfxChunk++ >= FF_ROOTS5SPR)
			{
				CK_RemoveObj(ob);
				return;
			}
			ob->user1 += 10;	// delay for vanishing animation
		}
	}
	else if (ob->gfxChunk > FF_ROOTS1SPR)
	{
		if ((ob->user1 -= SD_GetSpriteSync()) < 0)
		{
			ob->gfxChunk--;
			ob->user1 += 10;	// delay for growing animation
		}
	}
}

void FF_C_Roots(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		if (ob->gfxChunk > FF_ROOTS2SPR)
			break;
		FF_ExplodeShot(hit);
		if (ob->gfxChunk == FF_ROOTS2SPR)
		{
			ob->user1 = 10;	// delay for vanishing animation
			ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
			SD_PlaySound(FF_ROOTSSND);
		}
		else
		{
			ob->user1 = 4*70;
		}
		ob->gfxChunk++;
		CK_SetAction2(ob, ob->currentAction);	// force game to update the sprite
		break;
	}
}


/*
==============================================================================
	YETI / WADDLE

	Stands at tunnel entrances and blows a cloud at Keen when Keen comes near.
	The cloud pushes Keen back. Shooting the Yeti 5 times causes it to explode
	into several small yetis that roll and jump around, pushing Keen on touch.

	temp1 - health
	temp2 - cooldown/delay between clouds
	temp3 - animation counter
	temp4 - cooldown for hurt animation
------------------------------------------------------------------------------
	YETI CLOUD

	temp1 - animation counter
------------------------------------------------------------------------------
	SMALL YETI

	temp1 - cooldown (can't merge into big yeti when this is non-zero)
	temp2 - touch counter
	temp4 - big yeti's x position
==============================================================================
*/

#define TINY_YETI_COUNT 4	// remember: count is 1 smaller than the total because the object never touches itself
#define TINY_YETI_COUNT_EASY 3	// Also remember that this code needs TINY_YETI_COUNT-1 growing sprites!


void FF_SpawnYeti(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Yeti;
	//new->active = ac_yes;	//redundant - GetNewObj always sets active to ac_yes
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 27*16;
	new_object->xDirection = IN_motion_Left;
	new_object->yDirection = IN_motion_Down;
	//new->clipped = cl_midclip;	//redundant - GetNewObj always sets needtoclip to cl_midclip
	new_object->gfxChunk = FF_YETIL1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_YetiStand));
}

void  FF_PushKeenX(CK_object *hit, CK_object *ob)
{
	// push Keen, but only if Keen is in regular collision mode:
	if (hit->currentAction->collide == &FF_KeenContact)
	{
		// remember hitnorth, so Keen can still land while being pushed
		int16_t oldHitNorth = hit->topTI;
		int16_t oldHitSouth = hit->bottomTI;
		int16_t oldY = hit->posY;
		int16_t oldPush = hit->currentAction->stickToGround;
		
		if (ck_keenState.platform)
			hit->currentAction->stickToGround = 0;	// don't push Keen down when he's on a platform object
		
		ff_map.playerkludgeclipcancel = true;
		CK_PhysPushX(hit, ob);
		ff_map.playerkludgeclipcancel = false;
		
		hit->currentAction->stickToGround = oldPush;	// restore old value
		
		if (hit->leftTI || hit->rightTI)
			hit->user1 = hit->user2 = 0;	// disable Keen's idle animations while being pushed
		
		if (hit->posY == oldY)
		{
			if (!hit->topTI)
				hit->topTI = oldHitNorth;
			if (!hit->bottomTI)
				hit->bottomTI = oldHitSouth;
		}
	}
}

void FF_T_Yeti(CK_object *ob)
{
	if (ob->clipRects.unitXmid > ck_keenObj->clipRects.unitXmid)
	{
		ob->xDirection = IN_motion_Left;
	}
	else
	{
		ob->xDirection = IN_motion_Right;
	}

	if (ob->user2 > SD_GetSpriteSync())
	{
		ob->user2 -= SD_GetSpriteSync();
	}
	else if (ob->user2)
	{
		ob->user2 = 0;
	}
	else if (abs(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid) <= 4*256
		&& abs(ob->posY - ck_keenObj->posY) <= 2*256)
	{
		// spawn a cloud that pushes the player back
		CK_object *new_object = CK_GetNewObj(true);
		new_object->zLayer = 2;
		new_object->posX = ob->posX + 9*16;
		new_object->posY = ob->posY + 10*16;
		new_object->type = FF_CT_YetiCloud;
		new_object->gfxChunk = FF_YETICLOUD1SPR;
		new_object->xDirection = ob->xDirection;
		new_object->velX = 29*ob->xDirection;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_YetiCloud));
		SD_PlaySound(FF_YETICLOUDSND);

		ob->user2 = 35;	// don't spawn another cloud in the next 35 tics
	}

	if ((ob->user4 -= SD_GetSpriteSync()) < 0)
	{
		ob->user4 = 0;
		ck_nextX = (ob->posX & 16);
	}
	else
	{
		ck_nextX = (ob->posX & 16)? 16 : -16;	// shake it, yeti!
		ob->user3 += SD_GetSpriteSync() << 1;	// make animation go at triple speed
	}

	// update frame:
	ob->user3 += SD_GetSpriteSync();
	if (ob->user3 >= 40)
	{
		ob->user3 -= 40;
	}
	if (ob->user3 < 25)
	{
		ob->gfxChunk = FF_YETIL1SPR+1 + ob->xDirection;
	}
	else
	{
		ob->gfxChunk = FF_YETIL2SPR+1 + ob->xDirection;
	}
}

void FF_C_Yeti(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_PushKeenX(hit, ob);
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		ob->user4 = 35;	// half a second of shaking

		//if (++ob->user1 >= 5)
		if (++ob->user1 >= (ck_gameState.difficulty == D_Easy? 4 : 5))
		{
			 int16_t i;

			// turn big yeti into tiny yeti:
			ob->user1 = 10*70;
			ob->user2 = ob->user3 = 0;
			ob->user4 = ob->posX + 256;
			ob->type = FF_CT_TinyYetiMaster;
			ob->gfxChunk = FF_TINYYETIROLL1SPR;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_TinyYetiRoll));

			// spawn some more tiny yetis:
			//for (i=0; i < TINY_YETI_COUNT; i++)
			//for (i=0; i < (ck_gameState.difficulty == gd_Easy? TINY_YETI_COUNT_EASY : TINY_YETI_COUNT); i++)
			for (i=(ck_gameState.difficulty == D_Easy? TINY_YETI_COUNT_EASY : TINY_YETI_COUNT); i-- > 0;)

			{
				CK_object *new_object = CK_GetNewObj(true);

				// Note: if GetNewObj() returns the dummy object (because there
				// were no free spots in the objarray), the tiny yeti object will
				// not exists in the level and the tiny yetis won't be able to merge
				// into a big one again.

				new_object->type = FF_CT_TinyYeti;
				new_object->zLayer = 1;
				new_object->gfxChunk = FF_TINYYETIROLL1SPR;
				new_object->posX = ob->posX + (i & 1)*256;
				new_object->posY = ob->posY + (i & 2)*256/2;
				new_object->xDirection = (US_RndT() < 0x80)? IN_motion_Left : IN_motion_Right;
				new_object->user1 = 12*70;
				new_object->user4 = ob->posX + 256;
				CK_SetAction(new_object, CK_ACTION(FF_ACT_TinyYetiRoll));
			}
		}
		break;
	}
}

void FF_T_YetiCloud(CK_object *ob)
{
	if ((ob->user1 += SD_GetSpriteSync()) >= 8)
	{
		ob->user1 -= 8;
		if (++ob->gfxChunk > FF_YETICLOUD7SPR)
		{
			CK_RemoveObj(ob);
			return;
		}
	}
	ck_nextX = ob->velX * SD_GetSpriteSync();
}

void FF_C_YetiCloud(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player && ob->type == FF_CT_YetiCloud)
	{
		FF_PushKeenX(hit, ob);
	}
}

void FF_T_TinyYetiRoll(CK_object *ob)
{
	int16_t xmove = SD_GetSpriteSync() * 28;
	int16_t xdist = ob->posX - ob->user4;

	if ((ob->user1 -= SD_GetSpriteSync()) < 0)
		ob->user1 = 0;

	if (ob->user1 && !ob->user2)
	{
		// don't move too far away from the big yeti's position:
		if (xdist > 2*256)
		{
			ob->xDirection = IN_motion_Left;
		}
		else if (xdist < -2*256)
		{
			ob->xDirection = IN_motion_Right;
		}

		// jump at random:
		if (US_RndT() < 10)
		{
			if (ob->xDirection == IN_motion_Left)
			{
				ob->velX = -28;
				ob->gfxChunk = FF_TINYYETILSPR;
			}
			else
			{
				ob->velX = 28;
				ob->gfxChunk = FF_TINYYETIRSPR;
			}
			ob->velY = -48;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_TinyYetiJump));
			return;
		}
	}
	else
	{
		// return to big yeti's position:
		if (ob->posX > ob->user4)
		{
			ob->xDirection = IN_motion_Left;
		}
		else
		{
			ob->xDirection = IN_motion_Right;
		}

		// don't let any tiny yeti move past the target position:
		if (xmove > abs(xdist))
		{
			xmove = abs(xdist);
		}

		// stop moving when close to desired position:
		if (abs(xdist) < 2*16)	// distance must be smaller than half the hitbox width of the tiny yeti!
		{
			if (ob->user2)
			{
				// tiny yeti has already grown a bit (merged with others):
				ob->gfxChunk = ob->user2+FF_YETIGROW1SPR-1;
			}
			else
			{
				// tiny yeti is just standing:
				ob->gfxChunk = FF_TINYYETIROLL1SPR;

				// allow the (non-growing) yeti to jump a little while waiting to get merged:
				if (!ob->user2)
				{
					ob->velX = 0;
					ob->velY = -24;
					CK_SetAction2(ob, CK_ACTION(FF_ACT_TinyYetiJump));
				}
			}
			// master can only absorb the others when it is at
			// the target position, so move it there
			if (ob->type == FF_CT_TinyYetiMaster)
				ck_nextX = ob->user4-ob->posX;
			return;
		}
	}

	ck_nextX = ob->xDirection * xmove;

	// rolling animation:
	if ((ob->actionTimer += SD_GetSpriteSync()) >= 4)
	{
		ob->actionTimer -= 4;
		ob->gfxChunk += ob->xDirection;
	}
	// Note: ticcount is ignored for actions using think, so it's safe to use it here.

	if (ob->gfxChunk < FF_TINYYETIROLL1SPR)
	{
		ob->gfxChunk = FF_TINYYETIROLL4SPR;
	}
	else if (ob->gfxChunk > FF_TINYYETIROLL4SPR)
	{
		ob->gfxChunk = FF_TINYYETIROLL1SPR;
	}
}

void FF_T_TinyYetiJump(CK_object *ob)
{
	int16_t xdist = ob->posX - ob->user4;

	if ((ob->user1 -= SD_GetSpriteSync()) < 0)
		ob->user1 = 0;

	// don't move too far away from the big yeti's position:
	if ((ob->xDirection == IN_motion_Right && xdist > + 2*256)
		|| (ob->xDirection == IN_motion_Left && xdist < - 2*256))
	{
		FF_FrictionX(ob);
	}
	else
	{
		ck_nextX = ob->velX * SD_GetSpriteSync();
	}
	FF_DoGravity(ob);
}

void FF_C_TinyYeit(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_PushKeenX(hit, ob);
		break;
	case FF_CT_TinyYeti:
		// only tiny yeti master can grow, and only when touching tiny yetis that have the same target position!
		if (ob->user1 || ob->type != FF_CT_TinyYetiMaster || hit->user4 != ob->user4 || ob->posX != ob->user4)
			break;

		// absorb the other yeti:
#if 1
		CK_RemoveObj(hit);
#else
		// turn hit object into smoke cloud instead of just removing it
		hit->velX = 0;
		hit->gfxChunk = FF_YETICLOUD1SPR;
		hit->type = FF_CT_YetiCloud;
		hit->user1 = 0;
		hit->clipped = CLIP_not;
		hit->zLayer = FOREGROUNDPRIORITY;
		hit->posX -= 10*16;
		hit->posY -= 10*16;
		CK_SetAction2(hit, CK_ACTION(FF_ACT_YetiCloud));
#endif

		//if (++ob->user2 == TINY_YETI_COUNT)
		if (++ob->user2 == (ck_gameState.difficulty == D_Easy? TINY_YETI_COUNT_EASY : TINY_YETI_COUNT))
		{
			// turn ob into big yeti:
			ob->type = FF_CT_Yeti;
			ob->gfxChunk = FF_YETIL1SPR;
			ob->posY -= 29*16;
			ob->posX = ob->user4 - 256;
			ob->user1 = ob->user2 = ob->user3 = ob->user4 = ob->actionTimer = 0;
			CK_SetAction(ob, CK_ACTION(FF_ACT_YetiStand));
		}
		else
		{
			if (ob->velY < 0)
				ob->velY = 0;
			ob->user1 = 7;	// force a pause of 0.1 seconds after merging
			// assign correct sprite to tiny yeti master:
			ob->gfxChunk = ob->user2 + FF_YETIGROW1SPR-1;
			CK_SetAction(ob, ob->currentAction);	// update hitbox (DON'T use ChangeState here - that's not safe after manipulating x)
		}
		break;
	}
}

void FF_R_TinyYetiJump(CK_object *ob)
{
	if (ob->leftTI || ob->rightTI)
	{
		ob->velX = -ob->velX/2;
		ob->xDirection = -ob->xDirection;
	}
	if (ob->bottomTI || ob->topTI)
	{
		ob->velY = 0;
		if (ob->topTI)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_TinyYetiRoll));
		}
	}
	FF_R_Draw(ob);
}

void FF_R_TinyYetiRoll(CK_object *ob)
{
	if (!ob->topTI)
	{
		if (US_RndT() < 0x80)
		{
			if (ob->xDirection == IN_motion_Left)
			{
				ob->velX = -28;
				ob->gfxChunk = FF_TINYYETILSPR;
			}
			else
			{
				ob->velX = 28;
				ob->gfxChunk = FF_TINYYETIRSPR;
			}
			ob->velY = -48;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_TinyYetiJump));
		}
		else
		{
			ob->velX = -ob->velX;
			ob->xDirection = -ob->xDirection;
			ob->posX -= ob->deltaPosX;
		}
	}
	else if (ob->leftTI || ob->rightTI)
	{
		ob->velX = -ob->velX;
		ob->xDirection = -ob->xDirection;
	}
	FF_R_Draw(ob);
}


#undef TINY_YETI_COUNT
#undef TINY_YETI_COUNT_EASY

/*
==============================================================================
	CLOUD

	A more or less static platform that Keen can stand on. Keen slowly sinks
	into it until he eventually falls out at the bottom end.

	temp1 - initial y position
	temp2 - initial hitbox height
==============================================================================
*/


void FF_SpawnCloud(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Cloud;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY /*= new->user1*/ = RF_TileToUnit(tileY);
	new_object->clipped = CLIP_not;
	new_object->gfxChunk = FF_BIGCLOUDSPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Cloud));

	new_object->user2 = new_object->clipRects.unitY2-new_object->clipRects.unitY1;
}

void FF_SpawnInvisibleCloud(uint16_t unitX, uint16_t unitY, uint16_t w, uint16_t h)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Cloud;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->posX=new_object->clipRects.unitX1 = unitX;
	new_object->posY=new_object->clipRects.unitY1 /*= new->user1*/ = unitY;
	new_object->clipped = CLIP_not;
	new_object->currentAction = CK_ACTION(FF_ACT_Cloud);
	new_object->clipRects.unitX2 = unitX + w;
	new_object->clipRects.unitY2 = unitY + h;
	new_object->user2 = h;
	FF_CalcTileClipRects(new_object);
}

void FF_T_Cloud(CK_object *ob)
{
#if 0
	//
	// version 1: make the cloud sink when Keen stands on it
	//
	if (ob == ck_keenState.platform)
	{
		// Keen is standing on the cloud, so make it move down:
		ck_nextY = SD_GetSpriteSync();
		if (ob->posY - ob->user1 >= 3*70)
		{
			// make Keen fall off
			ck_keenState.platform = NULL;
			ob->type = CT_Friendly;
		}
	}
	else
	{
		// Keen is NOT standing on the cloud, so let it rise back up:
		ck_nextY = -SD_GetSpriteSync()*2;
		if (ck_nextY+ob->posY < ob->user1)
			ck_nextY = ob->user1 - ob->posY;
		ob->type = FF_CT_Cloud;
	}
#else
	//
	// version 2: make Keen sink into the cloud
	//
	if (ob == ck_keenState.platform)
	{
		// Keen is standing on the cloud, so make Keen sink into it by hacking the cloud's hitbox:
		ob->clipRects.unitY1 += ob->gfxChunk? SD_GetSpriteSync()*2 : SD_GetSpriteSync();
		if (ob->clipRects.unitY1 >= ob->clipRects.unitY2)
		{
			// make Keen fall off
			ck_keenState.platform = NULL;
			ob->type = CT_Friendly;
		}
	}
	else
	{
		// Keen is NOT standing on the cloud, so restore the hitbox:
		ob->clipRects.unitY1 = ob->clipRects.unitY2 - ob->user2;
		ob->type = FF_CT_Cloud;
	}
#endif
}


/*
==============================================================================
	MOLET / DICKIE DIG DIGGENS

	Walks back and forth on normal ground until it hits a wall that has the
	left or right side set to 2 instead of 1. This causes it to change into
	noclip mode, digging through the wall. Touching a block icon causes it
	to change back into normal movement mode.

	temp1 - animation counter
	temp2 - animation frame

	WARNING: Some older versions of Abiathar don't seem to like tiles that have
	the left/right side set to 2 and change them to 1 instead when saving the
	levels. Be sure to upgrade to the most recent version before editing any
	FITF levels.
==============================================================================
*/


void FF_SpawnMoletDirt(uint16_t unitX, uint16_t unitY)
{
	CK_object *new_object = CK_GetNewObj(true);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	new_object->clipped = CLIP_not;
	new_object->zLayer = 1;	// must use a greater value than Molet!
	new_object->posX = unitX;
	new_object->posY = unitY;
	new_object->gfxChunk = FF_MOLETDIRT1SPR;
	new_object->user1 = 50;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_MoletDirt));
}

void FF_T_MoletDirt(CK_object *ob)
{
	if (++ob->gfxChunk > FF_MOLETDIRT6SPR)
		ob->gfxChunk = FF_MOLETDIRT1SPR;

	if (--ob->user1 <= 0)
		CK_RemoveObj(ob);
}



void FF_SpawnMolet(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = CT_Friendly;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	//new->zLayer = 0;	// redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 4*16;
	new_object->xDirection = US_RndT() < 0x80? IN_motion_Left : IN_motion_Right;
	if (new_object->xDirection == IN_motion_Left)
		new_object->gfxChunk = FF_MOLETWALKL1SPR;
	else
		new_object->gfxChunk = FF_MOLETWALKR1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_MoletWalk));
}

void FF_T_Molet(CK_object *ob)
{
	// a 4-frame animation that changes the frame every 6 tics:
	if ((ob->user1 += SD_GetSpriteSync()) >= 6)
	{
		ob->user1 -= 6;
		if (++ob->user2 == 4)
			ob->user2 = 0;
	}
	if (ob->xDirection == IN_motion_Left)
	{
		ob->gfxChunk = FF_MOLETWALKL1SPR + ob->user2;
	}
	else
	{
		ob->gfxChunk = FF_MOLETWALKR1SPR + ob->user2;
	}
#ifdef EXTRA_INTERACTIONS
	FF_SmirkyCheckTiles(ob);
#endif
}

void FF_T_MoletDig(CK_object *ob)
{
	int16_t oldchunk = ob->gfxChunk;
	FF_T_Molet(ob);
	if (ob->gfxChunk != oldchunk)
	{
		if (ob->xDirection == IN_motion_Left)
		{
			if (INFOSPOT(ob->clipRects.tileX1, ob->clipRects.tileY2) == PLATFORMBLOCK)
			{
				ob->currentAction = CK_ACTION(FF_ACT_MoletWalk);
				ob->clipped = CLIP_normal;
			}
			oldchunk = ob->clipRects.unitX1-3*16;
			if ((oldchunk & 0xFF) < 8)
			{
				oldchunk = RF_UnitToTile(oldchunk)-1;
				if (!TI_ForeRight(FORESPOT(oldchunk, ob->clipRects.tileY2))
					&& TI_ForeRight(FORESPOT(oldchunk+1, ob->clipRects.tileY2)))
				{
					// Molet is coming out of the wall, so add dirt
					FF_SpawnMoletDirt(RF_TileToUnit(oldchunk)-2*16, ob->posY-8*16);
				}
			}
			ck_nextX = -8;
		}
		else
		{
			if (INFOSPOT(ob->clipRects.tileX2, ob->clipRects.tileY2) == PLATFORMBLOCK)
			{
				ob->currentAction = CK_ACTION(FF_ACT_MoletWalk);
				ob->clipped = CLIP_normal;
			}
			oldchunk = ob->clipRects.unitX2+3*16;
			if ((oldchunk & 0xFF) > 256-8)
			{
				oldchunk = RF_UnitToTile(oldchunk)+1;
				if (!TI_ForeLeft(FORESPOT(oldchunk, ob->clipRects.tileY2))
					&& TI_ForeLeft(FORESPOT(oldchunk-1, ob->clipRects.tileY2)))
				{
					// Molet is coming out of the wall, so add dirt
					FF_SpawnMoletDirt(RF_TileToUnit(oldchunk)-16*16, ob->posY-8*16);
				}
			}
			ck_nextX = 8;
		}
	}
}

void FF_T_MoletFall(CK_object *ob)
{
	FF_T_Projectile(ob);
#ifdef EXTRA_INTERACTIONS
	FF_SmirkyCheckTiles(ob);
#endif
}

void FF_C_Molet(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ob->xDirection < 0)
		{
			// molet is facing left
			// only the tip of the drill (leftmost side of the molet's hitbox) is deadly
			if (hit->clipRects.unitX1 <= ob->clipRects.unitX1 && hit->clipRects.unitX2 >= ob->clipRects.unitX1)
				FF_KillKeen();
		}
		else
		{
			// molet is facing right
			// only the tip of the drill (rightmost side of the molet's hitbox) is deadly
			if (hit->clipRects.unitX1 <= ob->clipRects.unitX2 && hit->clipRects.unitX2 >= ob->clipRects.unitX2)
				FF_KillKeen();
		}
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		break;
		
#ifdef EXTRA_INTERACTIONS
	case CT5_Item:
		if (hit->user1 >= 4)	// don't steal key gems!
		{
			hit->type = CT_Friendly;
			hit->zLayer = FOREGROUNDPRIORITY;
			hit->yDirection = IN_motion_None;
			CK_SetAction2(hit, CK_ACTION(FF_ACT_StolenItem0));
			SD_PlaySound(FF_SMIRKYSTEALSND);
		}
		break;
#endif
	}
}

void FF_MoletWalkReact(CK_object *ob)
{
	if (ob->leftTI && ob->xDirection == IN_motion_Right)
	{
		if (ob->leftTI == 2)
		{
			ob->clipped = CLIP_not;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_MoletDig));
			FF_SpawnMoletDirt(RF_TileToUnit(ob->clipRects.tileX2)+0*16, ob->posY-8*16);
		}
		else
			goto turn;
	}
	else if (ob->rightTI && ob->xDirection == IN_motion_Left)
	{
		if (ob->rightTI == 2)
		{
			ob->clipped = CLIP_not;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_MoletDig));
			FF_SpawnMoletDirt(RF_TileToUnit(ob->clipRects.tileX1)-16*16, ob->posY-8*16);
		}
		else
		{
turn:
			ob->xDirection = -ob->xDirection;
			ob->posX -= ob->deltaPosX;
			ob->gfxChunk -= 4*ob->xDirection;
		}
	}
	else if (!ob->topTI)
	{
		CK_SetAction2(ob, CK_ACTION(FF_ACT_MoletFall));
	}
	FF_R_Draw(ob);
}

void FF_R_MoletFall(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_MoletWalk));
	}
	FF_R_Draw(ob);
}


/*
==============================================================================
	BLU BIRD

	Flies around above Keen's head. Starts a dive attack when Keen gets too
	close and/or drops an egg when shot. Also has a chance to drop an egg at
	random.

	temp1 - counter for the shaking animation when laying an egg
	      - movement duration when flying normally
	      - countdown before attack sequence
	temp2 - defines if the bird can reproduce (0 = sterile, 1 = fertile)
	temp3 - hits taken so far
------------------------------------------------------------------------------
	EGG

	Falls to the ground and either lands safely or cracks open and spills its
	contents. If it landed safely, it will break when touched by Keen or hit by
	a shot. If the egg breaks more than 4 seconds after it was spawned, it
	releases a new bird.

	temp1 - counts for how long the egg has existed
	temp4 - Nisasi pointer
==============================================================================
*/

#define BIRD_SPEED 10
#define BIRD_SHAKETIME (70/2)



void FF_SpawnBird(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Bird;
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->user2 = true;	// bird is fertile
	CK_SetAction(new_object, CK_ACTION(FF_ACT_BirdFly2));
}

int16_t  FF_CountBirds(void)
{
	int16_t count = 0;
	CK_object *ob;

	for (ob = ck_keenObj->next; ob; ob = ob->next)
	{
		if (ob->type == FF_CT_Bird || ob->type == FF_CT_BirdEgg
			|| (ob->type == CT5_StunnedCreature && ob->user4 == FF_CT_Bird))
		{
			count++;
		}
	}
	return count;
}

int16_t  FF_CountEggs(void)
{
	int16_t count = 0;
	CK_object *ob;

	for (ob = ck_keenObj->next; ob; ob = ob->next)
	{
		if (ob->type == FF_CT_Egg || ob->type == FF_CT_BirdEgg)
		{
			count++;
		}
	}
	return count;
}

void FF_T_BirdFly(CK_object *ob)
{
	int16_t ydist = ob->clipRects.unitY2 + 55*16 - ck_keenObj->clipRects.unitY2;
	int16_t xdist = ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid;

	// If we have a movement speed, the think function was already executed
	// for this frame and the animation (state change) caused the game to
	// execute it again. In that case, don't do anything.
	if (ck_nextX || ck_nextY)
		return;

	// move in a random direction while Keen is not a player type (dying or grabbed by Efree)
	if (ck_keenObj->type != CT_Player)
	{
		if ((ob->user1 -= SD_GetSpriteSync()) < 0)
		{
			ob->user1 = 70/2;
			ob->xDirection = (US_RndT() % 3)-1;
			ob->yDirection = (US_RndT() % 3)-1;
		}
		FF_AccelerateY(ob, ob->yDirection, BIRD_SPEED);
		FF_AccelerateX(ob, ob->xDirection, BIRD_SPEED);
		return;
	}

	if (ydist > 0)
	{
		FF_AccelerateY(ob, IN_motion_Up, BIRD_SPEED);
	}
	else
	{
		FF_AccelerateY(ob, IN_motion_Down, BIRD_SPEED);
	}

	if (xdist > 6*256)
	{
		ob->xDirection = IN_motion_Right;
		FF_AccelerateX(ob, IN_motion_Right, BIRD_SPEED);
		return;
	}
	else if (xdist < -6*256)
	{
		ob->xDirection = IN_motion_Left;
		FF_AccelerateX(ob, IN_motion_Left, BIRD_SPEED);
		return;
	}
	else
	{
		if ((ob->user1 -= SD_GetSpriteSync()) < 0)
			ob->user1 = 0;

		if (xdist > 5*256 || xdist < -5*256)
		{
			// Bird is more than 5 tiles, but less than 6 tiles away from Keen.
			// This would be a good place to (randomly) start the dive attack.
			if (ydist < 16 && ydist > -16 && US_RndT() < 128
				&& ck_keenObj->type == CT_Player)
			{
				ob->currentAction = CK_ACTION(FF_ACT_BirdAttack);
				return;
			}
		}

		if (ob->user1)
		{
			FF_AccelerateX(ob, ob->xDirection, BIRD_SPEED);
		}
		else
		{
			FF_FrictionX(ob);

			// start laying an egg when Keen is directly below the bird and looking up:
			if (ck_keenObj->gfxChunk == FF_KEENLOOKUPSPR
				&& ob->velY == 0
				&& ck_keenObj->clipRects.unitY1 > ob->clipRects.unitY2
				&& abs(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid) < 256)
			{
				SD_PlaySound(FF_BIRDLAYSEGGSND);
				ob->currentAction = CK_ACTION(FF_ACT_BirdLayEgg);
			}
			else if (US_RndT() < 0x10)
			{
				ob->user1 = 2*70;
			}
		}
	}
}

void FF_T_BirdAttack(CK_object *ob)
{
	if (ob->gfxChunk < FF_BIRDATTACKR1SPR || ob->gfxChunk > FF_BIRDATTACKL3SPR)
	{
		if (ob->posX < ck_keenObj->posX)
		{
			ob->xDirection = IN_motion_Right;
			ob->gfxChunk = FF_BIRDATTACKR1SPR;
		}
		else
		{
			ob->xDirection = IN_motion_Left;
			ob->gfxChunk = FF_BIRDATTACKL1SPR;
		}
		ob->velX = 0;
		ob->velY = 0;
		ob->user1 = 0;
		SD_PlaySound(FF_BIRDSND);
	}
	else if (ob->user1 < 2)
	{
		ob->user1 += SD_GetSpriteSync();
	}
	else if (ob->xDirection < 0)
	{
		if (ob->clipRects.unitX2 < ck_keenObj->clipRects.unitX1)
		{
			// bird has moved past Keen, so make it move up again:
			FF_FrictionX2(ob);
			if (ob->clipRects.unitY2 + 2*256 > ck_keenObj->clipRects.unitY1)
			{
				FF_AccelerateY(ob, -2, BIRD_SPEED);
			}
			else
			{
				FF_FrictionY(ob);
			}
			if (ob->velX > -BIRD_SPEED)
			{
				ob->currentAction = CK_ACTION(FF_ACT_BirdFly4);
			}
			return;
		}
		// bird is still moving towards Keen here
		FF_AccelerateX(ob, -4, 48);
		if (ob->velX > -48)
		{
			// bird is still starting the attack (hasn't reached max xspeed yet)
			FF_AccelerateY(ob, 2, 48);
			if (ob->gfxChunk == FF_BIRDATTACKL1SPR)
			{
				if ((ob->actionTimer += SD_GetSpriteSync()) > 10)
					ob->gfxChunk++;
			}
		}
		else
		{
			FF_AccelerateY(ob, -2, 0);
			ob->gfxChunk = FF_BIRDATTACKL3SPR;
		}
	}
	else
	{
		if (ob->clipRects.unitX1 > ck_keenObj->clipRects.unitX2)
		{
			// bird has moved past Keen, so make it move up again:
			FF_FrictionX2(ob);
			if (ob->clipRects.unitY2 + 2*256 > ck_keenObj->clipRects.unitY1)
			{
				FF_AccelerateY(ob, -2, BIRD_SPEED);
			}
			else
			{
				FF_FrictionY(ob);
			}
			if (ob->velX < BIRD_SPEED)
			{
				ob->currentAction = CK_ACTION(FF_ACT_BirdFly4);
			}
			return;
		}
		// bird is still moving towards Keen here
		FF_AccelerateX(ob, 4, 48);
		if (ob->velX < 48)
		{
			// bird is still starting the attack (hasn't reached max xspeed yet)
			FF_AccelerateY(ob, 2, 48);
			if (ob->gfxChunk == FF_BIRDATTACKR1SPR)
			{
				if ((ob->actionTimer += SD_GetSpriteSync()) > 10)
					ob->gfxChunk++;
			}
		}
		else
		{
			// bird has reached full xspeed, now reduce yspeed
			FF_AccelerateY(ob, -2, 0);
			ob->gfxChunk = FF_BIRDATTACKR3SPR;
		}
	}

	if (ck_nextY < 0)
		ob->yDirection = IN_motion_Up;
	else
		ob->yDirection = IN_motion_Down;
}

void FF_T_BirdLayEgg(CK_object *ob)
{
	// shake before laying the egg
	if ((ob->actionTimer += SD_GetSpriteSync()) < BIRD_SHAKETIME)
	{
		if (++ob->user1 & 1)
		{
			ck_nextX = 16;
		}
		else
		{
			ck_nextX = -16;
		}
	}
	else
	{
		if (FF_CountEggs() < 5)	// each egg needs 4 sprites (egg + 3 small pieces when breaking), so 5 eggs = 20 sprites!
		{
			// spawn egg:
			CK_object *new_object = CK_GetNewObj(true);
			new_object->posX = ob->posX;
			new_object->posY = ob->posY + 256;
			new_object->gfxChunk = FF_EGG3SPR;

			// check if bird is fertile and can lay a fertilized egg:
			if (ob->user2 && US_RndT() < 85 && CK_ObjectVisible(ob) && FF_CountBirds() < 7)
			{
				new_object->type = FF_CT_BirdEgg;
				ob->user2 = false;
			}
			else
			{
				new_object->type = FF_CT_Egg;
				new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
			}
			CK_SetAction(new_object, CK_ACTION(FF_ACT_BirdEgg));
		}

		CK_SetAction2(ob, CK_ACTION(FF_ACT_BirdFly1));
		ob->velY = -BIRD_SPEED;	// bird flies up
	}
}

void FF_C_Bird(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		if (++ob->user3 == 2)
		{
			FF_StunObj(ob, hit, CK_ACTION(FF_ACT_BirdFall));
			ob->zLayer = 0;
		}
		else
		{
			FF_ExplodeShot(hit);
			if (ob->currentAction != CK_ACTION(FF_ACT_BirdLayEgg))	// don't restart the shaking animation if it's playing already
			{
				SD_PlaySound(FF_BIRDLAYSEGGSND);
				CK_SetAction2(ob, CK_ACTION(FF_ACT_BirdLayEgg));
				ob->velX = 0;
				ob->velY = 0;
			}

		}

		// make the other birds start the attack animation:
		for (hit = ck_keenObj->next; hit; hit = hit->next)
		{
			if (hit != ob && hit->type == FF_CT_Bird && hit->active && hit->currentAction != CK_ACTION(FF_ACT_BirdLayEgg))
			{
				CK_SetAction2(hit, CK_ACTION(FF_ACT_BirdAttack));
			}
		}
		break;
	}
}

void FF_R_Bird(CK_object *ob)
{
	if ((ob->leftTI && ob->xDirection > 0) || (ob->rightTI && ob->xDirection < 0))
	{
		ob->velX = -ob->velX;
		ob->xDirection = -ob->xDirection;
		if (ob->currentAction == CK_ACTION(FF_ACT_BirdAttack))
			CK_SetAction2(ob, CK_ACTION(FF_ACT_BirdFly1));
	}
	if ((ob->topTI && ob->yDirection > 0) || (ob->bottomTI && ob->yDirection < 0))
	{
		ob->velY = -ob->velY;
		ob->xDirection = -ob->xDirection;
		if (ob->currentAction == CK_ACTION(FF_ACT_BirdAttack))
			CK_SetAction2(ob, CK_ACTION(FF_ACT_BirdFly1));
	}
	FF_R_Draw(ob);
}

void FF_T_BirdEgg(CK_object *ob)
{
	FF_T_Projectile(ob);
	if (ob->gfxChunk == FF_EGG3SPR)
	{
		if (ob->user1 < 4*70)
		{
			ob->user1 += SD_GetSpriteSync();
		}
		// if grabbed by Nisasi AND landed on ground, then start breaking the egg:
		if (ob->user4 && ob->topTI)
		{
			ob->gfxChunk--;
		}
	}
	else if (ob->gfxChunk < FF_EGG3SPR)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			if (ob->gfxChunk == FF_EGG1SPR)
			{
				FF_ObjDropNisasi(ob);

				// if there's a bird in the egg, let it hatch, otherwise just break the egg
				if (ob->type == FF_CT_BirdEgg && ob->user1 >= 4*70)
				{
					SD_PlaySound(FF_BIRDLAYSEGGSND);
					ob->gfxChunk = FF_EGGBIRD1SPR;
				}
				else
				{
					
					SD_PlaySound(FF_EGGCRACKSND);
					ob->gfxChunk = FF_EGGCRACK1SPR;
					ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
				}

				// spawn some flying shell pieces
				{
					int16_t i;

					for (i=2; i>0; i--)			// Nisaba patch: we only need two eggshell splitter
					{
						CK_object *new_object = CK_GetNewObj(true);
						new_object->posX = ob->posX + 256;
						new_object->posY = ob->posY + 256;
						if (i == 1)
						{
							new_object->velX = 48 - (US_RndT() & 15);
						}
						else if (i == 2)
						{
							new_object->velX = -48 + (US_RndT() & 15);
						}
						else
						{
							new_object->velX = 8 - (US_RndT() & 15);
						}
						new_object->velY = -48 + (US_RndT() & 15);
						new_object->gfxChunk = i + FF_EGGSHELL1SPR - 1;
						new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
						new_object->zLayer = 1;
						CK_SetAction(new_object, CK_ACTION(FF_ACT_EggPiece));
					}
				}
			}
			else
			{
				ob->gfxChunk--;
			}
		}
	}
	else if (ob->gfxChunk >= FF_EGGBIRD1SPR && ob->gfxChunk <= FF_EGGBIRD5SPR)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk++;

			if (ob->gfxChunk == FF_EGGBIRD2SPR)
			{
				// spawn a new bird
				CK_object *new_object = CK_GetNewObj(true);
				new_object->zLayer = 1;
				new_object->type = FF_CT_Bird;
				new_object->posX = ob->posX;
				new_object->posY = ob->posY - 256;
				new_object->user2 = true;	// new bird is fertile, too
				new_object->xDirection = (ck_keenObj->clipRects.unitXmid < new_object->clipRects.unitXmid) ? IN_motion_Left : IN_motion_Right;
				CK_SetAction(new_object, CK_ACTION(FF_ACT_BirdFly1));

				ob->active = OBJ_EXISTS_ONLY_ONSCREEN;
			}

			if (ob->gfxChunk > FF_EGGBIRD5SPR)
				CK_RemoveObj(ob);
		}
	}
	else if (ob->gfxChunk >= FF_EGGCRACK1SPR && ob->gfxChunk <= FF_EGGCRACK4SPR)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk++;
		}
	}
	else if (ob->gfxChunk == FF_EGGCRACK5SPR)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 5*70)
		{
			ob->actionTimer -= 5*70;
			ob->gfxChunk++;
		}
	}
	else if (ob->gfxChunk == FF_EGGCRACK6SPR)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 14)
			CK_RemoveObj(ob);
	}
	else
	{
		// this should be unreachable, but just in case...
		CK_RemoveObj(ob);
	}
}

void FF_C_BirdEgg(CK_object *ob, CK_object *hit)
{
	if (ob->gfxChunk == FF_EGG3SPR)
	{
		switch (hit->type)
		{
		case CT_Stunner:
			FF_ExplodeShot(hit);
			// no break here!
		case CT_Player:
			// break the egg:
			SD_PlaySound(FF_EGGCRACKSND);
			ob->gfxChunk--;
			break;
		}
	}
/*	
	else if (ob->gfxChunk >= EGGCRACK1SPR && ob->gfxChunk <= EGGCRACK6SPR)
	{
		if (hit->type == CT_Player)
			KillKeen();
	}
*/
}

void FF_R_BirdEgg(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		// make the egg break if there's no bird in it:
		if (ob->gfxChunk == FF_EGG3SPR && ob->type != FF_CT_BirdEgg)
			ob->gfxChunk--;
	}
	FF_R_Draw(ob);
}

void FF_R_EggPiece(CK_object *ob)
{
	if (ob->leftTI || ob->rightTI || ob->topTI || ob->bottomTI)
	{
		CK_RemoveObj(ob);
	}
	else
	{
		FF_R_Draw(ob);
	}
}




/*
==============================================================================
	SNEAKY TREE

	A tree stump that Keen can stand on. Looks around at random while Keen is
	NOT standing on it. Moves away from Keen when Keen is not looking at it.

	temp1 - state (see enum)
	temp2 - look counter
	temp3 - blocked direction (1 = can't go right; -1 = can't go left)
	temp4 - Nisasi ptr
==============================================================================
*/

enum {ST_LOOK, ST_EXTEND, ST_RETRACT, ST_SLIDE};

//#define SNEAKYTREE_LOOKDELAY (ck_gameState.difficulty == gd_Easy? (TickBase/3) : (TickBase/2)) // half a second
#define SNEAKYTREE_LOOKDELAY (70/2) // half a second
#define SNEAKYTREE_EXTENDDELAY (8) 
#define SNEAKYTREE_RETRACTDELAY (3) 
#define SNEAKYTREE_SLIDERETRACTDELAY (5)
#define SNEAKYTREE_SLIDESPEED 48
#define SNEAKYTREE_SLIDESPEED2 32


void FF_SpawnSneakyTree(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 24*16;
	//new->zLayer = 0;	// redundant
	new_object->type = FF_CT_SneakyTree;
	new_object->clipped = FF_CLIP_custommid;
	new_object->gfxChunk = FF_SNEAKYHIDESPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_SneakyTree));
}

void FF_T_SneakyTree(CK_object *ob)
{
	if (ob->velX || ob->user1 == ST_SLIDE)
	{
		FF_FrictionX2(ob);	// FrictionX2 means higher friction

		if (ob->user1 == ST_SLIDE)
		{
			if ((ob->actionTimer += SD_GetSpriteSync()) >= SNEAKYTREE_SLIDERETRACTDELAY)
			{
				ob->actionTimer -= SNEAKYTREE_SLIDERETRACTDELAY;

				if (ob->gfxChunk == FF_SNEAKYSLIDEL1SPR)
				{
					ob->gfxChunk = FF_SNEAKYLOOKLSPR;
					ob->user1 = ST_LOOK;
				}
				else if (ob->gfxChunk == FF_SNEAKYSLIDER1SPR)
				{
					ob->gfxChunk = FF_SNEAKYLOOKRSPR;
					ob->user1 = ST_LOOK;
				}
				else
				{
					ob->gfxChunk--;
				}
			}
		}
		else
		{
			ob->gfxChunk = ob->xDirection < 0 ? FF_SNEAKYLOOKLSPR : FF_SNEAKYLOOKRSPR;
		}
	}
	else if (ob->user1 == ST_EXTEND)
	{
		if (ob->xDirection > 0)
		{
			if (ck_keenObj->xDirection < 0 && ck_keenObj->clipRects.unitX2 < ob->clipRects.unitX1)
			{
				if ((ob->actionTimer += SD_GetSpriteSync()) >= SNEAKYTREE_EXTENDDELAY)
				{
					ob->actionTimer -= SNEAKYTREE_EXTENDDELAY;

					if (ob->gfxChunk == FF_SNEAKYSLIDER4SPR)
					{
						ob->gfxChunk--;// = SNEAKYLOOKRSPR;
						ob->velX = ob->user3 ? SNEAKYTREE_SLIDESPEED2 : SNEAKYTREE_SLIDESPEED;
						ob->user1 = ST_SLIDE;
						ob->user3 = 0;
						ob->actionTimer = 0;
					}
					else
					{
						ob->gfxChunk++;
					}
				}
			}
			else
			{
				ob->user1 = ST_RETRACT;
				ob->actionTimer = 0;
			}
		}
		else
		{
			if (ck_keenObj->xDirection > 0 && (ck_keenObj->clipRects.unitX1 > ob->clipRects.unitX2
				|| (ob->user3 == IN_motion_Right && ck_keenObj->clipRects.unitX1 >= ob->posX + 22*16)))
			{
				if ((ob->actionTimer += SD_GetSpriteSync()) >= SNEAKYTREE_EXTENDDELAY)
				{
					ob->actionTimer -= SNEAKYTREE_EXTENDDELAY;

					if (ob->gfxChunk == FF_SNEAKYSLIDEL4SPR)
					{
						ob->gfxChunk--;// = SNEAKYLOOKLSPR;
						ob->velX = ob->user3 ? -SNEAKYTREE_SLIDESPEED2 : -SNEAKYTREE_SLIDESPEED;
						ob->user1 = ST_SLIDE;
						ob->user3 = 0;
						ob->actionTimer = 0;
					}
					else
					{
						ob->gfxChunk++;
					}
				}
			}
			else
			{
				ob->user1 = ST_RETRACT;
				ob->actionTimer = 0;
			}
		}
	}
	else if (ob->user1 == ST_RETRACT)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= SNEAKYTREE_RETRACTDELAY)
		{
			ob->actionTimer -= SNEAKYTREE_RETRACTDELAY;

			if (ob->gfxChunk == FF_SNEAKYSLIDEL1SPR || ob->gfxChunk == FF_SNEAKYSLIDER1SPR)
			{
				ob->gfxChunk = FF_SNEAKYLOOKUSPR;
				ob->user1 = ST_LOOK;
			}
			else
			{
				ob->gfxChunk--;
			}
		}
	}
	else if (ob == ck_keenState.platform || ob->user4)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) >= SNEAKYTREE_LOOKDELAY)
		{
			ob->actionTimer -= SNEAKYTREE_LOOKDELAY;

			ob->gfxChunk = US_RndT() < 0x10 ? FF_SNEAKYHIDESPR : FF_SNEAKYLOOKUSPR;
		}
		ob->user1 = ST_LOOK;
		ob->user2 = 0;
	}
	else //if (ob->user1 == ST_LOOK)
	{
		int16_t xdist = abs(ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid);

		if (ck_keenObj->xDirection < 0 && ck_keenObj->clipRects.unitX2 < ob->clipRects.unitX1)
		{
			ob->xDirection = IN_motion_Right;
		}
		else if (ck_keenObj->xDirection > 0 && (ck_keenObj->clipRects.unitX1 > ob->clipRects.unitX2
			|| (ob->user3 == IN_motion_Right && ck_keenObj->clipRects.unitX1 >= ob->posX + 22*16)))
		{
			ob->xDirection = IN_motion_Left;
		}
		else
		{
			ob->xDirection = IN_motion_None;
		}

		if (ob->xDirection == IN_motion_None || xdist > 6*256)
		{
			ob->gfxChunk = FF_SNEAKYHIDESPR;
			ob->actionTimer = 0;
			ob->user2 = 0;
		}
		else if ((ob->actionTimer += SD_GetSpriteSync()) >= SNEAKYTREE_LOOKDELAY)
		{
			ob->actionTimer -= SNEAKYTREE_LOOKDELAY;

			ob->gfxChunk = US_RndT() < 0x80 ? FF_SNEAKYLOOKLSPR : FF_SNEAKYLOOKRSPR;
			ob->user2++;

			if (ob->user2 > (ck_gameState.difficulty == D_Easy? 6 : 8))
			{
				if (ob->user3 != ob->xDirection)
				{
					ob->gfxChunk = ob->xDirection > 0 ? FF_SNEAKYSLIDER1SPR : FF_SNEAKYSLIDEL1SPR;
					ob->user1 = ST_EXTEND;
				}
				ob->user2 = 0;
			}
		}
	}
}

void FF_R_SneakyTree(CK_object *ob)
{
	if ((ob->xDirection > 0 && ob->leftTI) || (ob->xDirection < 0 && ob->rightTI) || !ob->topTI)
	{
		ob->user3 = ob->xDirection;
		ob->velX = 0;
		if (!ob->topTI)
		{
			ob->posX -= ob->deltaPosX;
			ob->posY -= ob->deltaPosY;
		}
	}
	FF_R_Draw(ob);
}


#undef SNEAKYTREE_LOOKDELAY
#undef SNEAKYTREE_EXTENDDELAY
#undef SNEAKYTREE_RETRACTDELAY
#undef SNEAKYTREE_SLIDERETRACTDELAY
#undef SNEAKYTREE_SLIDESPEED
#undef SNEAKYTREE_SLIDESPEED2

/*
==============================================================================
	CROCODENT

	Walks around and has a random chance of jumping. Turns into a trap when
	Keen is near, trap is activated when Keen touches it or tries to jump over
	it. Trap also activates automatically after 8 seconds.

	temp1 - counter for leaving the trap state
	temp2 - random jump allowed (boolean)
	temp3 - old tileXmid position
	temp4 - Nisasi pointer
==============================================================================
*/

#define CROC_HIGHJUMP -52
#define CROC_LOWJUMP  -32
#define CROC_XSPEED    26


void FF_SpawnCroc(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->type = FF_CT_Crocodent;
	new_object->zLayer = 1;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) + 2*16;
	if (US_RndT() < 0x80)
	{
		new_object->xDirection = IN_motion_Left;
		new_object->gfxChunk = FF_CROCOWALKL1SPR;
	}
	else
	{
		new_object->xDirection = IN_motion_Right;
		new_object->gfxChunk = FF_CROCOWALKR1SPR;
	}
	CK_SetAction(new_object, CK_ACTION(FF_ACT_CrocWalk));
}

void FF_CrocCheckTiles(CK_object *ob)
{
	int16_t w,h,i,skip,off;

	h = ob->clipRects.tileY2 - ob->clipRects.tileY1 + 1;
	w = ob->clipRects.tileX2 - ob->clipRects.tileX1 + 1;
	skip = CA_GetMapWidth() - w;
	off = ob->clipRects.tileX1 + ((ob->clipRects.tileY1) * CA_GetMapWidth());
	while (h--)
	{
		i = w;
		while (i--)
		{
			 int16_t info = FF_MapAt(2, off++);

			switch (info-DIRARROWSTART)
			{
			case arrow_East:
				if (ob->xDirection > 0)
					ob->user2 = true;
				else
					ob->user2 = false;
				break;

			case arrow_West:
				if (ob->xDirection < 0)
					ob->user2 = true;
				else
					ob->user2 = false;
				break;
			}
		}
		off += skip;
	}
}

void FF_T_CrocWalk(CK_object *ob)
{
	int16_t xdist, ydist;

	FF_CrocCheckTiles(ob);

	xdist = ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid;
	ydist = ob->clipRects.unitY2 - ck_keenObj->clipRects.unitY2;

	if (!ob->user4
		&& abs(xdist) < 4*256
		//&& player->currentAction->stickToGround
		&& abs(ydist) < 3*256)
	{
		ck_nextX = 0;	// no movement!
		ob->gfxChunk = ob->xDirection < 0 ? FF_CROCOTRAPOPENL1SPR : FF_CROCOTRAPOPENR1SPR;
		ob->currentAction = CK_ACTION(FF_ACT_CrocTrap);
	}
	else if (ob->xDirection < 0)
	{
		if (++ob->gfxChunk > FF_CROCOWALKL4SPR)
			ob->gfxChunk = FF_CROCOWALKL1SPR;

		if (!ob->user4)	// not grabbed by Nisasi?
		{
			// start jumping when standing in a jump arrow spot:
			if (ob->user3 != ob->clipRects.tileXmid // only do this check once per tile!
				&& INFOSPOT(ob->clipRects.tileXmid,ob->clipRects.tileY2) == DIRARROWSTART+arrow_NorthWest
				&& US_RndT() < 100)
			{
				ob->velY = CROC_HIGHJUMP;
				ob->velX = -CROC_XSPEED;
				ob->gfxChunk = FF_CROCOJUMPL1SPR;
				ob->currentAction = CK_ACTION(FF_ACT_CrocJump);
			}
			// otherwise try to do a random jump, if allowed:
			else if (ob->user2 && US_RndT() < 8)
			{
				ob->velY = CROC_LOWJUMP;
				ob->velX = -CROC_XSPEED;
				ob->gfxChunk = FF_CROCOJUMPL1SPR;
				ob->currentAction = CK_ACTION(FF_ACT_CrocJump);
			}
		}
	}
	else
	{
		if (++ob->gfxChunk > FF_CROCOWALKR4SPR)
			ob->gfxChunk = FF_CROCOWALKR1SPR;

		if (!ob->user4)	// not grabbed by Nisasi?
		{
			// start jumping when standing in a jump arrow spot:
			if (ob->user3 != ob->clipRects.tileXmid // only do this check once per tile!
				&& INFOSPOT(ob->clipRects.tileXmid,ob->clipRects.tileY2) == DIRARROWSTART+arrow_NorthEast
				&& US_RndT() < 100)
			{
				ob->velY = CROC_HIGHJUMP;
				ob->velX = CROC_XSPEED;
				ob->gfxChunk = FF_CROCOJUMPR1SPR;
				ob->currentAction = CK_ACTION(FF_ACT_CrocJump);
			}
			// otherwise try to do a random jump, if allowed:
			else if (ob->user2 && US_RndT() < 8)
			{
				ob->velY = CROC_LOWJUMP;
				ob->velX = CROC_XSPEED;
				ob->gfxChunk = FF_CROCOJUMPR1SPR;
				ob->currentAction = CK_ACTION(FF_ACT_CrocJump);
			}
		}
	}

	ob->user3 = ob->clipRects.tileXmid;	// remember old tile position for the jump checks
}

void FF_T_CrocJump(CK_object *ob)
{
	FF_T_Projectile(ob);	// medium gravity
	FF_CrocCheckTiles(ob);

	if (ck_nextY > 0)
		ob->gfxChunk = ob->xDirection < 0 ? FF_CROCOJUMPL2SPR : FF_CROCOJUMPR2SPR;
}

void FF_T_CrocTrap(CK_object *ob)
{
	switch (ob->gfxChunk)
	{
	case FF_CROCOTRAPOPENR1SPR:
	case FF_CROCOTRAPOPENL1SPR:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk++;
		}
		break;
	case FF_CROCOTRAPOPENR2SPR:
	case FF_CROCOTRAPOPENL2SPR:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 10)
		{
			ob->actionTimer -= 10;
			ob->gfxChunk = FF_CROCOTRAPOPEN3SPR;
		}
		break;
	case FF_CROCOTRAPOPEN3SPR:
		if ((ob->actionTimer += SD_GetSpriteSync()) < 10)
			break;
		ob->actionTimer = 0;
		ob->user1 = 0;
	case FF_CROCOTRAP1SPR:
	case FF_CROCOTRAP2SPR:
		if (ob->actionTimer == 0)
		{
			if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
			{
				ob->gfxChunk = FF_CROCOTRAP1SPR;
			}
			else
			{
				ob->gfxChunk = FF_CROCOTRAP2SPR;
			}
			// randomly look in the other direction:
			if (US_RndT() < 50)
			{
				ob->gfxChunk ^= (FF_CROCOTRAP1SPR ^ FF_CROCOTRAP2SPR);
			}
			ob->actionTimer = 0.5*70;
		}
		else
		{
			if ((ob->actionTimer -= SD_GetSpriteSync()) < 0)
				ob->actionTimer = 0;
		}

		if (ck_keenObj->clipRects.unitY2 <= ob->clipRects.unitY2
			&& ck_keenObj->clipRects.unitX2 > ob->clipRects.unitX1
			&& ck_keenObj->clipRects.unitX1 < ob->clipRects.unitX2)
		{
			SD_PlaySound(FF_TENTACLESWOOSHSND);
			ob->gfxChunk = FF_CROCOTRAPSNAP1SPR;
			ob->velX = 0;
			ob->velY = -48;
			ob->currentAction = CK_ACTION(FF_ACT_CrocSnap);
		}

		// cancel trap mode after 5 seconds
		if ((ob->user1 += SD_GetSpriteSync()) >= 5*70)
		{
			ob->user1 = 0;
			ob->gfxChunk = FF_CROCOTRAPSNAP1SPR;
			ob->velX = 0;
			ob->velY = -24;
			ob->currentAction = CK_ACTION(FF_ACT_CrocSnap);
		}

		break;
	}
}

void FF_T_CrocSnap(CK_object *ob)
{
	FF_T_Projectile(ob);
/*	
	if (ytry >= 0 && ob->topTI)
	{
		if ((ob->actionTimer += tics) >= 40)
		{
			ob->gfxChunk = ob->xDirection < 0 ? CROCOWALKL1SPR : CROCOWALKR1SPR;
			ob->currentAction = CK_ACTION(FF_ACT_CrocWalk);
		}
	}
	else
*/
	{
		if (ck_nextY > 0)
			ob->gfxChunk = FF_CROCOTRAPSNAP2SPR;
	}
}

void FF_C_Croc(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player
		&& ob->gfxChunk >= FF_CROCOTRAPOPENR1SPR && ob->gfxChunk <= FF_CROCOTRAPSNAP1SPR)
	{
		FF_KillKeen();
		if (ob->currentAction == CK_ACTION(FF_ACT_CrocTrap))
		{
			ob->gfxChunk = FF_CROCOTRAPSNAP1SPR;
			ob->velX = 0;
			ob->velY = -48;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrocSnap));
		}
	}
	
#ifdef EXTRA_INTERACTIONS
	else if (hit->type == FF_CT_Skypest && ob->gfxChunk == FF_CROCOTRAPSNAP1SPR)
	{
		// TODO: make sure the skypest is in the crocodent's mouth
		CK_RemoveObj(hit);
		ob->gfxChunk++;
	}
#endif
}

void FF_R_CrocWalk(CK_object *ob)
{
	if (ob->xDirection > 0 && ob->leftTI)
	{
		ob->xDirection = IN_motion_Left;
		ob->gfxChunk += FF_CROCOWALKL1SPR - FF_CROCOWALKR1SPR;
	}
	else if (ob->xDirection < 0 && ob->rightTI)
	{
		ob->xDirection = IN_motion_Right;
		ob->gfxChunk += FF_CROCOWALKR1SPR - FF_CROCOWALKL1SPR;
	}
	if (!ob->topTI)	// not on solid ground anymore?
	{
#if 0
		// old code, not needed anymore:

		if (ob->user4)
		{
			ob->xDirection = -ob->xDirection;
			ob->posX -= ob->deltaPosX;
			ob->posY -= ob->deltaPosY;
			ob->gfxChunk += (ob->xDirection < 0)? (FF_CROCOWALKL1SPR - FF_CROCOWALKR1SPR) : (FF_CROCOWALKR1SPR - FF_CROCOWALKL1SPR);
		}
		else
		{
			if (ob->xDirection < 0)
			{
				ob->gfxChunk = FF_CROCOJUMPL1SPR;
				ob->velX = -CROC_XSPEED;
			}
			else
			{
				ob->gfxChunk = FF_CROCOJUMPR1SPR;
				ob->velX = CROC_XSPEED;
			}
			ob->velY = CROC_LOWJUMP;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrocJump));
		}
#else
		// new code: always turn around (jump would have been initiated by CrocWalk)

		// update sprite number, so TurnX uses the correct hitbox:
		ob->gfxChunk += (ob->xDirection > 0)? (FF_CROCOWALKL1SPR - FF_CROCOWALKR1SPR) : (FF_CROCOWALKR1SPR - FF_CROCOWALKL1SPR);
		FF_TurnX(ob);
		if (!ob->topTI)	// still not on solid ground?
		{
			// just start falling:
			ob->xDirection = -ob->xDirection;	// back to old direction
			ob->gfxChunk = (ob->xDirection < 0)? FF_CROCOJUMPL1SPR : FF_CROCOJUMPR1SPR;
			ob->velX = ob->velY = 0;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrocJump));
		}
#endif
	}
	FF_R_Draw(ob);
}

void FF_R_CrocJump(CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;
	if (ob->leftTI && ob->velX > 0 || ob->rightTI && ob->velX < 0)
		ob->velX = 0;
	if (ob->topTI)
	{
		ob->velY = 0;
		ob->gfxChunk = ob->xDirection < 0 ? FF_CROCOWALKL1SPR : FF_CROCOWALKR1SPR;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_CrocWalk));
	}
	FF_R_Draw(ob);
}

void FF_R_CrocSnap(CK_object *ob)
{
	if (ob->bottomTI)
		ob->velY = 0;
	if (ob->topTI && ob->velY >= 0)
	{
		ob->velY = 0;
		// turn to face Keen:
		ob->xDirection = (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)? IN_motion_Left : IN_motion_Right;
		if (US_RndT() < 77)	// croc has a 30% chance to walk away from Keen after landing
			ob->xDirection = -ob->xDirection;
		ob->gfxChunk = ob->xDirection < 0 ? FF_CROCOWALKL1SPR : FF_CROCOWALKR1SPR;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_CrocWalk));
		//ob->gfxChunk = CROCOTRAPSNAP3SPR;
	}
	FF_R_Draw(ob);
}


#undef CROC_HIGHJUMP
#undef CROC_LOWJUMP
#undef CROC_XSPEED

/*
==============================================================================
	TENTACLE NEST / GRETENTA

	Stationary object that pulls Keen or Skypests into its mouth to kill them.

------------------------------------------------------------------------------
	SWALLOWED OBJECT

	temp1 - pointer to the tentacle nest object
	temp2 - animation delay
	temp3 - first animation sprite
	temp4 - last animation sprite
==============================================================================
*/

#define NEST_ANIMDELAY 10


void FF_SpawnTentacleNest(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_TentacleNest;
	new_object->clipped = CLIP_not;
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->gfxChunk = FF_GRETENTA1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_TentacleNest));
}

void FF_T_TantacleNest(CK_object *ob)
{
	if ((ob->actionTimer += SD_GetSpriteSync()) >= NEST_ANIMDELAY)
	{
		ob->actionTimer -= NEST_ANIMDELAY;
		ob->gfxChunk++;
		if (ob->gfxChunk > FF_GRETENTA8SPR)
			ob->gfxChunk = FF_GRETENTA1SPR;
	}
}

void FF_C_TantacleNest(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ck_godMode || ff_map.invincibilityTimer || FF_KeenIsDead())
			return;
		ff_keenKilled = true;
		SD_PlaySound(FF_DEATHTENTACLENESTSND);
		hit->user3 = hit->gfxChunk = FF_EFREEKEEN1SPR;
		hit->user4 = FF_EFREEKEEN2SPR;
		hit->user2 = 30;
		FF_DropNisasi();
		break;
	case FF_CT_Skypest:
		hit->user3 = hit->gfxChunk = FF_SKYPESTFLYL1SPR;
		hit->user4 = FF_SKYPESTFLYL2SPR;
		hit->user2 = 5;
		break;
	default:
		return;
	}
	CK_SetAction(hit, CK_ACTION(FF_ACT_Swallowed));
	hit->clipped = CLIP_not;
	hit->type = CT_Friendly;
	hit->user1 = FF_ObjToUser(ob);
	hit->zLayer = ob->zLayer-1;
	if (hit->zLayer < 0)
		hit->zLayer = 0;
	hit->yDirection = (hit->clipRects.unitY1 < ob->posY+24*16)? IN_motion_Down : IN_motion_Up;
}

void FF_T_Swallowed(CK_object *ob)
{
#define XSPEED 8
#define YSPEED 4

	CK_object *nest = FF_ObjFromUser(ob->user1);

	ck_nextX = nest->posX+28*16 - ob->clipRects.unitXmid;
	if (ob->yDirection > 0)
	{
		ck_nextY = 16*SD_GetSpriteSync();
		if (ob->clipRects.unitY1 >= nest->posY+24*16)
			ob->yDirection = IN_motion_Up;
	}
	else// if (ob->yDirection <= 0)
	{
		ck_nextY = -YSPEED*SD_GetSpriteSync();
	}
	if (ck_nextX < 0 && ck_nextX < -XSPEED*SD_GetSpriteSync())
	{
		ck_nextX = -XSPEED*SD_GetSpriteSync();
	}
	else if (ck_nextX > 0 && ck_nextX > XSPEED*SD_GetSpriteSync())
	{
		ck_nextX = XSPEED*SD_GetSpriteSync();
	}

	if ((ob->actionTimer += SD_GetSpriteSync()) > ob->user2)
	{
		ob->actionTimer -= ob->user2;
		if (++ob->gfxChunk > ob->user4)
			ob->gfxChunk = ob->user3;
	}

	if (ob->yDirection <= 0 && ob->clipRects.unitY2 < nest->posY+24*16)
	{
		if (ob == ck_keenObj)
		{
			ob->gfxChunk = 0;
			ck_gameState.levelState = ex_died;
		}
		else
		{
			CK_RemoveObj(ob);
		}
	}

#undef XSPEED
#undef YSPEED
}



/*
==============================================================================
	CHECKPOINT MUSHROOMS

	temp1 - 0 if not touched, 1 if touched, 2 after first retry etc.
	temp2 - 
	temp3 - additional sprite pointer
	temp4 - 
==============================================================================
*/



void FF_SpawnCheckpoint(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Checkpoint;
	new_object->clipped = CLIP_not;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 44*16;
	new_object->gfxChunk = FF_CHECKPOINT1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Checkpoint));
}

void FF_T_Checkpoint(CK_object *ob)
{
	if (ob->user1)
	{
		ob->gfxChunk = FF_CPBIGSHROOMSPR;
	}
	else
	{
		if ((ob->actionTimer+=SD_GetSpriteSync()) >= 12)
		{
			ob->actionTimer -= 12;
			if (ob->gfxChunk++ == FF_CHECKPOINT3SPR)
				ob->gfxChunk = FF_CHECKPOINT1SPR;
		}
	}
}

void FF_T_CheckpointCap(CK_object *ob)
{
	// remove the cap when Keen lands on something solid or has fallen out of the cap:
	if (ck_keenObj->topTI || ob->clipRects.unitY2 < ck_keenObj->clipRects.unitY1)
		CK_RemoveObj(ob);
}

CK_object * FF_SpawnSmoke(int16_t x, int16_t y)
{
	CK_object *new_object = CK_GetNewObj(true);
	SD_PlaySound(FF_CHECKPOINTSND);
	new_object->posX = x;
	new_object->posY = y;
	new_object->clipped = CLIP_not;
	new_object->zLayer = FOREGROUNDPRIORITY-1;
	new_object->gfxChunk = FF_SMIRKYPOOF1SPR;
	new_object->user1 = FF_SMIRKYPOOF4SPR;
	new_object->actionTimer = US_RndT() & 7;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Smoke));
	return new_object;
}

void FF_T_Smoke(CK_object *ob)
{
	 int16_t maxtics = ob->type == FF_CT_LindseyVanish? 8 : 20;

	if ((ob->actionTimer += SD_GetSpriteSync()) >= maxtics)
	{
		ob->actionTimer -= maxtics;
		if (ob->gfxChunk++ == ob->user1)
			CK_RemoveObj(ob);
	}
}

// Note:
// Keen touching the checkpoint is handled in CK_KEEN.C
// PlayLoop() calls CheckpointSave() 

void FF_CheckpointSave(CK_object *ob)
{
	CK_object *o;

	// turn the old "used" checkpoint mushrooms into smoke:
#if 0
	for (o=ck_keenObj; o; o=o->next)
	{
		if (o->type == FF_CT_Checkpoint && o->user1)
		{
			RF_RemoveSpriteDrawUsing16BitOffset(&o->user3);	// remove flask overlay (if present)
			o->posX -= 8*16;
			o->posY += 256;
			o->type = CT_Friendly;
			o->gfxChunk = FF_SMIRKYPOOF1SPR;
			o->user1 = FF_SMIRKYPOOF4SPR;
			o->active = OBJ_EXISTS_ONLY_ONSCREEN;
			CK_SetAction(o, CK_ACTION(FF_ACT_Smoke));
		}
	}
#else
	if (ff_map.lastCheckpoint)
	{
		o = ff_map.lastCheckpoint;
		RF_RemoveSpriteDrawUsing16BitOffset(&o->user3);	// remove flask overlay (if present)
		o->posX -= 8*16;
		o->posY += 256;
		o->type = CT_Friendly;
		o->gfxChunk = FF_SMIRKYPOOF1SPR;
		o->user1 = FF_SMIRKYPOOF4SPR;
		o->active = OBJ_EXISTS_ONLY_ONSCREEN;
		CK_SetAction(o, CK_ACTION(FF_ACT_Smoke));
	}
	ff_map.lastCheckpoint = ob;
#endif
	ff_map.checkpointTouched = NULL;

	// save checkpoint and change object states:
	ob->user1 = 1;	// checkpoint has been used now
	//ob->gfxChunk = CPBIGSHROOMSPR;
	//NewState(ob, ob->currentAction);

	if (FF_SaveCheckpoint())
	{
		// spawn "CHECKPOINT" sprite:
		CK_object *new_object = CK_GetNewObj(true);
		new_object->posX = ob->posX-20*16;
		new_object->posY = ob->posY+42*16;
		new_object->zLayer = FOREGROUNDPRIORITY;
		new_object->clipped = CLIP_not;
		new_object->type = CT_Friendly;
		new_object->gfxChunk = FF_CHECKPOINTRISESPR;
		new_object->yDirection = IN_motion_Up;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_bonusrise));
		new_object->actionTimer = -50;	// stay visible a little longer than the score numbers
		
		// spawn "1UP" sprite: (this doesn't assign a state, so it will cause problems)
		/*
		new = GetNewObj(true);
		new->posX = ob->posX-3*PIXGLOBAL;
		new->posY = ob->posY+22*PIXGLOBAL;
		new->zLayer = FOREGROUNDPRIORITY;
		new->clipped = cl_noclip;
		new->type = CT_Friendly;
		*/

		// spawn some smoke clouds:
		o = FF_SpawnSmoke(ob->posX-4*16, ob->posY-6*16);
		o = FF_SpawnSmoke(ob->posX-20*16, ob->posY-8*16);
		o->gfxChunk++;
		o = FF_SpawnSmoke(ob->posX-4*16, ob->posY+20*16);
		o->gfxChunk++;
		o = FF_SpawnSmoke(ob->posX-20*16, ob->posY+20*16);

		// play checkpoint sound effect:
		// (down here to avoid conflicts with SpawnSmoke and its sound effect)
		if (ck_gameState.difficulty != FF_D_Expert)
			SD_PlayTwoSounds(FF_CHECKPOINTSND, FF_CHECKPOINT2SND); 
		else
			SD_PlaySound(FF_CHECKPOINTSND); 

	}
	else
	{
		ff_map.lastCheckpoint = NULL;
		CK_RemoveObj(ob);
	}
}

bool FF_CheckpointLoad(void)
{
	CK_object *o;

	if (!FF_LoadCheckpoint())
		return false;

	// find the checkpoint object:
#if 0
	for (o=ck_keenObj; o; o=o->next)
	{
		if (o->type == FF_CT_Checkpoint && o->user1)
		{
			// move Keen into the cap of the checkpoint mushroom:
			ck_keenObj->posX = o->posX;
			ck_keenObj->posY = o->posY;

			// spawn a second cap on top of Keen (so Keen falls out of the cap):
			o = CK_GetNewObj(true);
			o->clipped = CLIP_not;
			o->posX = ck_keenObj->posX;
			o->posY = ck_keenObj->posY;
			o->zLayer = FOREGROUNDPRIORITY-1;
			o->gfxChunk = FF_CPSHROOMCAPSPR;
			CK_SetAction(o, CK_ACTION(FF_ACT_CheckpointCap));

			// make Keen fall:
			ck_keenObj->clipped = CLIP_normal;	// just in case Keen was in noclip mode
			ck_keenObj->velX = 0;
			ck_keenObj->velY = 0;
			ck_keenObj->yDirection = IN_motion_Down;
			CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenFall1));
			break;
		}
	}
#else
	if (ff_map.lastCheckpoint)
	{
		o = ff_map.lastCheckpoint;
		// move Keen into the cap of the checkpoint mushroom:
		ck_keenObj->posX = o->posX;
		ck_keenObj->posY = o->posY;

		// spawn a second cap on top of Keen (so Keen falls out of the cap):
		o = CK_GetNewObj(true);
		o->clipped = CLIP_not;
		o->posX = ck_keenObj->posX;
		o->posY = ck_keenObj->posY;
		o->zLayer = FOREGROUNDPRIORITY-1;
		o->gfxChunk = FF_CPSHROOMCAPSPR;
		CK_SetAction(o, CK_ACTION(FF_ACT_CheckpointCap));

		// make Keen fall:
		ck_keenObj->clipped = CLIP_normal;	// just in case Keen was in noclip mode
		ck_keenObj->velX = 0;
		ck_keenObj->velY = 0;
		ck_keenObj->yDirection = IN_motion_Down;
		CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenFall1));

	}
#endif
	FF_RemoveDreamBubbles();

	// Note: If no checkpoint object was found, Keen's position is left unchanged
	// and loading the checkpoint works more like loading a saved game.

	if (ff_map.invincibilityTimer < CHECKPOINT_INVINCIBILITY)
		ff_map.invincibilityTimer = CHECKPOINT_INVINCIBILITY;

	// remove IF-Stunner objects from the level:
	for (o=ck_keenObj; o; o=o->next)
	{
		if (o->type == CT5_Item && o->user1 == 11 && o->user4 == -1)
			CK_RemoveObj(o);
	}

	return true;
}

void FF_R_Checkpoint(CK_object* ob)
{
#define FLASK_HEIGHT 13
#define FLASK_XOFF (2*16)
#define FLASK_YOFF (39*16)

	// draw the main checkpoint sprite:
	FF_R_Draw(ob);
	
	// draw the flask symbol overlay, if necessary:
	if (ob->user1)
	{
		// Note: The pixels that get cut off at the top need to be subtracted from
		// y to make the flask symbol appear at the same position for every case
		switch (ff_state.freeRestartsPerCheckpoint - ob->user1)
		{
		case 0:	// 1 free restart left
			RF_PlaceSpriteAfterClipped(&ob->user3, ob->posX+FLASK_XOFF, ob->posY+(FLASK_YOFF-2*FLASK_HEIGHT*16), FF_CHECKPOINTOVERLAYSPR, false, ob->zLayer, &ob->sde, 2*FLASK_HEIGHT, 0);
			break;
		case 1:	// 2 free restarts left
			RF_PlaceSpriteAfterClipped(&ob->user3, ob->posX+FLASK_XOFF, ob->posY+(FLASK_YOFF-FLASK_HEIGHT*16), FF_CHECKPOINTOVERLAYSPR, false, ob->zLayer, &ob->sde, FLASK_HEIGHT, FLASK_HEIGHT);
			break;
		case 2:	// 3 free restarts left
			RF_PlaceSpriteAfterClipped(&ob->user3, ob->posX+FLASK_XOFF, ob->posY+FLASK_YOFF, FF_CHECKPOINTOVERLAYSPR, false, ob->zLayer, &ob->sde, 0, 2*FLASK_HEIGHT);
			break;
		}
	}
	
#undef FLASK_HEIGHT
#undef FLASK_XOFF
#undef FLASK_YOFF
}



/*
==============================================================================
	LEAVEA TRIP

	Tree:
	temp1: state (see enum)

	Leaf:
	temp1 - frame for walking animation ( & 3) or vanish counter (squished leaf)
	temp2 - pointer to parent tree object
==============================================================================
*/


enum {LT_IDLE, LT_BLANK, LT_GROW};

#define LEAVEA_VELX 16

void  FF_RemoveLeaf(CK_object *ob)
{
	(FF_ObjFromUser(ob->user2))->user1 = LT_GROW;	// allow "mother" to grow another leaf
	CK_RemoveObj(ob);
}

void FF_SpawnLeaveaTrip(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->gfxChunk = FF_LEAVEATREE1SPR;
	new_object->type = CT_Friendly;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_LeaveaTree));
}

void FF_T_LeaveaTree(CK_object *ob)
{
	int16_t xdist;

	switch (ob->user1)
	{
	case LT_IDLE:
#if 0 	// Nisaba patch: we don't need LeaveaTree animations (sprites have been removed!)
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 12)		
		{
			ob->actionTimer -= 12;
			if (++ob->gfxChunk > LEAVEATREE3SPR)
				ob->gfxChunk = FF_LEAVEATREE1SPR;
		}
#endif

		xdist = ob->clipRects.unitXmid-ck_keenObj->clipRects.unitXmid;
		if (xdist > -6*256 && xdist < 6*256)
		{
			CK_object *new_object = CK_GetNewObj(true);
			new_object->posX = ob->posX + 6*16;
			new_object->posY = ob->clipRects.unitY2;
			new_object->gfxChunk = FF_LEAVEAFALL2SPR;
			new_object->user2 = FF_ObjToUser(ob);
			new_object->xDirection = IN_motion_Left;
			new_object->velX = LEAVEA_VELX;
			CK_SetAction(new_object, CK_ACTION(FF_ACT_LeaveaFall));
			ob->user1 = LT_BLANK;
			ob->gfxChunk = FF_LEAVEATREE1SPR;
		}
		break;
	case LT_BLANK:
		ob->gfxChunk = FF_LEAVEATREE1SPR;
		break;
	case LT_GROW:
		ob->actionTimer += SD_GetSpriteSync();

#if 0	// Nisaba patch: we don't need LeaveaTree animations (sprites have been removed!)
		if (ob->gfxChunk != LEAVEATREE5SPR)
		{											
			if (ob->actionTimer >= 70)
			{
				ob->gfxChunk = LEAVEATREE5SPR;
				ob->actionTimer = 0;
			}
		}
		else if (ob->actionTimer >= 12)
#endif

		{
			ob->actionTimer = 0;
			ob->user1 = LT_IDLE;
			ob->gfxChunk = FF_LEAVEATREE1SPR;
		}
		break;
	}
}

void FF_T_LeaveaFall(CK_object *ob)
{
	if (!CK_ObjectVisible(ob))
	{
		FF_RemoveLeaf(ob);
		return;
	}
#if 0
	if ((ob->actionTimer += SD_GetSpriteSync()) >= 8)
	{
		ob->actionTimer -= 8;
		if (++ob->gfxChunk > FF_LEAVEAFALL3SPR)
			ob->gfxChunk = FF_LEAVEAFALL1SPR;
	}
#else
	if (ob->xDirection > 0 && ob->velX >= LEAVEA_VELX
		|| ob->xDirection < 0 && ob->velX <= -LEAVEA_VELX)
	{
		ob->xDirection = -ob->xDirection;
	}
	if (ob->velX >= 5)
	{
		ob->gfxChunk = FF_LEAVEAFALL3SPR;
	}
	else if (ob->velX <= -5)
	{
		ob->gfxChunk = FF_LEAVEAFALL1SPR;
	}
	else
	{
		ob->gfxChunk = FF_LEAVEAFALL2SPR;
	}
	FF_AccelerateY(ob, IN_motion_Down, 10);
	FF_AccelerateX(ob, ob->xDirection, LEAVEA_VELX);
#endif
}

void FF_R_LeaveaFall(CK_object *ob)
{
	if (ob->topTI)
	{
		ck_nextY = 3*16;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_LeaveaWalk));
		if (ck_keenObj->clipRects.unitXmid > ob->clipRects.unitXmid)
		{
			ob->gfxChunk = FF_LEAVEAWALKR1SPR;
			ob->xDirection = IN_motion_Right;
		}
		else
		{
			ob->gfxChunk = FF_LEAVEAWALKL1SPR;
			ob->xDirection = IN_motion_Left;
		}
	}
	FF_R_Draw(ob);
}

void FF_T_LeaveaWalk(CK_object *ob)
{
	int16_t xdist;
	ob->zLayer = 2;

	if (!CK_ObjectVisible(ob))
	{
		FF_RemoveLeaf(ob);
		return;
	}

	ob->user1++;
	if (ob->gfxChunk == FF_LEAVEASQUISHEDSPR)
	{
		ck_nextX = 0;	// squished leaves shouldn't move

		// squished leaves disappear after a while, even when they're still visible
		if (ob->user1 > 30)
			FF_RemoveLeaf(ob);
	}
	else
	{
		ob->gfxChunk = FF_LEAVEAWALKR1SPR + (ob->user1 & 3);
		if (ob->xDirection < 0)
			ob->gfxChunk += FF_LEAVEAWALKL1SPR-FF_LEAVEAWALKR1SPR;
	}
}

void FF_C_LeaveaWalk(CK_object *ob, CK_object *hit)
{
	if (hit != ck_keenObj || ob->gfxChunk == FF_LEAVEASQUISHEDSPR)
		return;

	if (hit->currentAction == CK_ACTION(CK_ACT_keenPogo1) ||
		hit->currentAction == CK_ACTION(CK_ACT_keenPogo2) ||
		hit->currentAction == CK_ACTION(CK_ACT_keenPogo3))
	{
		ob->gfxChunk = FF_LEAVEASQUISHEDSPR;
		CK_SetAction2(ob, ob->currentAction);
		SD_PlaySound(FF_SQUISHSND);
		ob->user1 = 0;	
	}
	else
	{
		FF_PushKeenX(hit, ob);
	}
}

void FF_R_LeaveaWalk(CK_object *ob)
{
	if (!ob->topTI)
	{
		ob->velY = 0;
		ob->velX = ob->xDirection*8;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_LeaveaFall));
		FF_R_Draw(ob);
	}
	else
	{
		FF_R_Walk(ob);
	}
	ob->timeUntillThink = 0;
}


/*
==============================================================================
	WARBLE / ZIBBLINX

	temp1 - current state of the object
	temp2 - hide countdown after getting shot twice/blink count after first hit
	temp3 - hits taken so far
	temp4 - Nisasi pointer
==============================================================================
*/

enum {WARBLE_RISE, WARBLE_IDLE, WARBLE_SINK, WARBLE_HIDE};

#define WARBLE_RISE_DELAY 8
#define WARBLE_SINK_DELAY 8
#define WARBLE_BLINK_DELAY 10
#define WARBLE_ANIM_DELAY 18


void FF_SpawnWarble(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->type = FF_CT_Warble;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 37*16;
	new_object->clipped = CLIP_not;
	new_object->gfxChunk = FF_WARBLEGROW1SPR;
	new_object->user1 = WARBLE_HIDE;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Warble));
}

void FF_T_Warble(CK_object *ob)
{
	int16_t xdist = ob->clipRects.unitXmid - ck_keenObj->clipRects.unitXmid;
	xdist = abs(xdist);
	switch (ob->user1)
	{
	case WARBLE_HIDE:
		ob->gfxChunk = FF_WARBLEGROW1SPR;
		if (ob->user2)
		{
			if (ob->user2 > SD_GetSpriteSync())
				ob->user2 -= SD_GetSpriteSync();
			else
				ob->user2 = 0;
		}
		else if (xdist < 3*256 && ob->clipRects.unitY2 >= ck_keenObj->clipRects.unitY1)
		{
			ob->user1 = WARBLE_RISE;
		}
		break;
	case WARBLE_RISE:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= WARBLE_RISE_DELAY)
		{
			ob->actionTimer -= WARBLE_RISE_DELAY;
			ob->gfxChunk++;
			if (ob->gfxChunk == FF_WARBLEGROW6SPR)
				ob->user1 = WARBLE_IDLE;
		}
		break;
	case WARBLE_SINK:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= WARBLE_SINK_DELAY)
		{
			ob->actionTimer -= WARBLE_SINK_DELAY;
			ob->gfxChunk--;
			if (ob->gfxChunk == FF_WARBLEGROW1SPR)
			{
				ob->user1 = WARBLE_HIDE;
				ob->user3 = 0;	// hiding "heals" the warble
			}
		}
		break;
	case WARBLE_IDLE:
		if (xdist > 5*256)
		{
			FF_ObjDropNisasi(ob);
			ob->user1 = WARBLE_SINK;
			ob->gfxChunk = FF_WARBLEGROW5SPR;
		}
		else
		{
			if (ob->user2)
			{
				if ((ob->actionTimer += SD_GetSpriteSync()) >= WARBLE_BLINK_DELAY)
				{
					ob->actionTimer -= WARBLE_BLINK_DELAY;
					ob->gfxChunk = (ob->user2 & 1)? FF_WARBLEGROW13SPR : FF_WARBLEGROW10SPR;
					ob->user2--;
				}
			}
			else
			{
				if ((ob->actionTimer += SD_GetSpriteSync()) >= WARBLE_ANIM_DELAY)
				{
					ob->actionTimer -= WARBLE_ANIM_DELAY - (US_RndT() & 7);	// animation delay is sligtly random
					xdist = (ob->gfxChunk - FF_WARBLEGROW6SPR) & 3;	// we're just re-using the variable here
					ob->gfxChunk = (US_RndT() >> 3) & 3;
					if (ob->gfxChunk == xdist)	// same frame as before?
						ob->gfxChunk ^= 3;	// can't allow that!
					ob->gfxChunk += ob->user3? FF_WARBLEGROW10SPR : FF_WARBLEGROW6SPR;
				}
			}
		}
		break;
	}
}

void FF_C_Warble(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		if (ob->user3++)
		{
			ob->user2 = 3*70;	// stay hidden for 3 seconds
			switch (ob->user1)
			{
			case WARBLE_IDLE:
				ob->gfxChunk = FF_WARBLEGROW5SPR;
				// no break here!
			case WARBLE_RISE:
				ob->actionTimer = 0;
				ob->user1 = WARBLE_SINK;
				FF_ObjDropNisasi(ob);
				break;
			}
		}
		else
		{
			ob->user2 = 8;
		}
		break;
	}
}


/*
==============================================================================
	STATIC SPRITES

	Just set decoration, these objects don't actually do anything.
==============================================================================
*/

CK_object *FF_SpawnStaticSprite(uint16_t tileX, uint16_t tileY, int16_t chunk, int16_t priority)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->clipped = CLIP_not;
	new_object->zLayer = priority;
	new_object->gfxChunk = chunk;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_bucket));
	CA_MarkGrChunk(chunk);
	return new_object;
}

/*
==============================================================================
	EFREE

	Picks Keen up and transports him to otherwise inaccessible locations.

	temp1 - state
	temp2 - base sprite + animation offset (highest bit is animation offset)
	temp3 - blink duration left (relative to animation speed!)
==============================================================================
*/

enum {EFREE_SEARCH, EFREE_DOCK, EFREE_TRANSPORT, EFREE_VANISH};


void FF_SpawnEfree(uint16_t x, uint16_t y)
{
	// only spawn it if the sprites are cached:
	if (ca_graphChunkNeeded[FF_EFREE1SPR] & ca_levelbit)
	{
		CK_object *new_object = CK_GetNewObj(true);
		new_object->posX = x-4*16;
		new_object->posY = y;
		new_object->clipped = CLIP_not;
		new_object->user2=new_object->gfxChunk = FF_EFREE1SPR;
		new_object->type = FF_CT_Efree;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_Efree));
	}
}

void FF_T_Efree(CK_object *ob)
{
	switch (ob->user1)
	{
	case EFREE_SEARCH:
		if (ob->clipRects.unitY2 >= ck_keenObj->clipRects.unitY1+4*16
			|| ob->posX+4*16 != ck_keenObj->posX
			|| ff_map.nisasi
			|| FF_KeenIsDead())
		{
			ob->user1 = EFREE_VANISH;
		}
		else
		{
			ck_nextY = 8*SD_GetSpriteSync();
		}
		goto animate;
	case EFREE_DOCK:
		ck_nextY = ck_keenObj->posY + 5*16 - ob->clipRects.unitY2;
		ob->user1++;
		goto animate;
	/*
		if ((ob->actionTimer += tics) >= 6)
		{
			ob->actionTimer -= 6;
			if (ob->user2 < 5)
			{
				static unsigned anim[] = {EFREE3SPR, EFREE1SPR, EFREE4SPR, EFREE1SPR, EFREE5SPR};
				ob->gfxChunk = anim[ob->user2++];
			}
			else
			{
				ob->gfxChunk = EFREE1SPR;
				ob->user1++;
			}
		}
		break;
	*/
	case EFREE_TRANSPORT:
		// drop Keen when passing through a bock icon:
		{
			uint16_t off, w;
			off = ((ob->clipRects.tileY2) * CA_GetMapWidth()) + ob->clipRects.tileX1;
			w = 1 + ob->clipRects.tileX2 - ob->clipRects.tileX1;
			while (w--)
			{
				if (FF_MapAt(2, off++) == PLATFORMBLOCK)
					goto dropKeen;
			}
		}

		if	(ob->clipRects.tileY1 == 0)	// always drop Keen at the top of the level!
		{
dropKeen:
			// make Keen fall:
			CK_SetAction2(ck_keenObj, CK_ACTION(CK_ACT_keenFall1));
			ck_keenObj->clipped = CLIP_normal;
			ck_keenObj->type = CT_Player;
			ck_keenObj->velX=ck_keenObj->velY=0;
			// Efree vanishes:
			ob->user1++;
		}
		else
		{
			// move Keen up:
			FF_MoveObjVert(ck_keenObj, -8*SD_GetSpriteSync());
			ck_keenObj->visible = true;
		}
		// no break here!
	case EFREE_VANISH:
		ck_nextY = -8*SD_GetSpriteSync();
animate:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 6)
		{
			ob->actionTimer -= 6;
			if (ob->user3)
			{
				if ((ob->user3 -= SD_GetSpriteSync()) <= 0)
				{
					if ((ob->user2 & 0x7FFF) == FF_EFREE1SPR)
					{
						ob->user3 = 0;
					}
					else
					{
						//ob->user3 = TickBase/2 + US_RndT()/8;	//wait 0.5 to 1 second before next blink
						ob->user3 = 3;	//don't start the next blink immediately
					}
					ob->user2 &= 0x8000;
					ob->user2 |= FF_EFREE1SPR;
				}
			}
			else
			{
				//random blink animation:
				int16_t rnd = US_RndT();
				if (rnd < 24)
				{
					ob->user2 &= 0x8000;
					ob->user2 |= FF_EFREE3SPR + ((rnd >> 2) & 6);
					ob->user3 = 3;	//blink for 0.3888 seconds
				}
			}
			ob->user2 ^= 0x8000;
			ob->gfxChunk = (ob->user2 & 0x7FFF) + ((ob->user2 & 0x8000)? 1 : 0);
		}
		break;
	}
	if (ob->user1 == EFREE_VANISH && !CK_ObjectVisible(ob))
		CK_RemoveObj(ob);
}

void FF_EfreeGrabKeen(CK_object *ob)
{
	if (ob->user1 == EFREE_SEARCH)
	{
		ck_keenObj->type = CT_Friendly;
		ck_keenObj->clipped = CLIP_not;
		ck_keenObj->gfxChunk = ck_keenObj->user2 = FF_EFREEKEEN1SPR;
		ck_keenObj->user3 = FF_EFREEKEEN2SPR+1;
		CK_SetAction2(ck_keenObj, CK_ACTION(FF_ACT_bonus1));
		ob->user1 = EFREE_DOCK;
	}
}


/*
==============================================================================
	NISASI / HUGGUG

	Jumps around and slides down on walls. Grabs onto Keen's head when touching
	it, and has to be shaken off. Keen can only walk left/right while Nisasi
	hangs on his head (handled elsewhere).

	temp1 - pointer to grabbed object
	temp2 - old player direction (for shaking off) / last grabbed object
	temp3 - player shake counter / cooldown
	temp4 - shake cooldown
==============================================================================
*/


void FF_SpawnNisasi(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 11*16;
	new_object->type = FF_CT_Nisasi;
	new_object->xDirection = (US_RndT() < 0x80)? -1 : 1;
	new_object->velY = 16;
	new_object->gfxChunk = FF_NISASIJUMPL1SPR + 2 + new_object->xDirection*2;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Nisasi));
}

void FF_NisasiLetGo(CK_object *ob)
{
	ob->velX = 48*ob->xDirection;
	ob->velY = -24;
	(FF_ObjFromUser(ob->user1))->user4 = 0;	// remove Nisasi ptr from grabbed object
	ob->user2 = ob->user1;	// last grabbed object = currently grabbed object
	ob->user3 = 20*70;	// don't grab it again for 20 seconds
	ob->actionTimer = ob->user1 = ob->user4 = 0;	// reset other variables, too
	ob->type = FF_CT_NisasiCooldown;	// don't grab anything until Nisasi lands
	//ob->clipped = cl_midclip;	// we don't use noclip anymore
}

void FF_T_Nisasi(CK_object *ob)
{
	if (ob->user1)
	{
		CK_object *grabbed = FF_ObjFromUser(ob->user1);
		ob->visible = true;	// always redraw the Nisasi when it grabs onto something
		if (grabbed == ck_keenObj)
		{
			// handle shaking off:
			if (ob->user2 != grabbed->xDirection)
			{
				ob->user2 = grabbed->xDirection;
				if (++ob->user3 > 3)
				{
					// Nisasi is shaken off
					//ob->clipped = cl_midclip;	// we don't use noclip anymore
					ob->xDirection = ob->user2;
					ob->velX = 48*ob->xDirection;
					ob->velY = -24;
					ob->actionTimer=ob->user1=ob->user2=ob->user3=ob->user4 = 0;
					ob->type = FF_CT_NisasiCooldown;
					ff_map.nisasi = NULL;	// for input stuff
					SD_SetMusicRate(DEFAULT_MUSIC_RATE);
					goto normal;
				}
				else
				{
					ob->user4 = 10;	// restart cooldown
				}
			}
			else if (ob->user3)
			{
				if (ob->user4 < SD_GetSpriteSync())
				{
					ob->user3 = ob->user4 = 0;	// reset shake count and cooldown
				}
				else
				{
					ob->user4 -= SD_GetSpriteSync();
				}
			}
		}
		else
		{
			if ((ob->user3 -= SD_GetSpriteSync()) <= 0)
			{
				FF_NisasiLetGo(ob);
				goto normal;
			}
		}
		
		ck_nextX = grabbed->posX - ob->posX;
		ck_nextY = grabbed->posY - ob->posY;
		
		// K1n9_Duk3 addition 2024-01-02: perform unclipped movement when more than 4 tiles away
		if (abs(ck_nextX) > 4*256 || abs(ck_nextY) > 4*256)
		{
			FF_MoveObjHoriz(ob, ck_nextX);
			FF_MoveObjVert(ob, ck_nextY);
			ck_nextX = ck_nextY = 0; // no extra movement
		}
		// end of addition
		
		return;
	}
	
	else if (ob->user3)
	{
		if ((ob->user3 -= SD_GetSpriteSync()) <= 0)
		{
			ob->user2 = 0;	// stop ignoring the last grabbed object
			ob->user3 = 0;
		}
	}
normal:
	ob->actionTimer += SD_GetSpriteSync();
	if (ob->leftTI || ob->rightTI || ob->topTI)
	{
		ob->gfxChunk = FF_NISASIHANGLSPR + 2 + ob->xDirection*2;
		if (!ob->topTI)
		{
			int16_t wall;
			if (ob->leftTI)
				wall = TI_ForeLeft(FORESPOT(ob->clipRects.tileX2+1, ob->clipRects.tileY2));
			else
				wall = TI_ForeRight(FORESPOT(ob->clipRects.tileX1-1, ob->clipRects.tileY2));

			if (wall)
				wall = !(TI_ForeTop(FORESPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2+1)));

			if (wall && ob->actionTimer < 4*70)
			{
				ck_nextY = SD_GetSpriteSync();	// slide down on the wall
				ck_nextX = -ob->xDirection;	// push against wall, so hitwest/hiteast will be updated correctly
				return;
			}
		}
		// start jumping
		ob->velX = ob->xDirection * (US_RndT()/8 +16);	// random speed of 16 to 48
		ob->velY = -48;
	}

	FF_T_Projectile(ob);
	ck_nextX = ob->velX * SD_GetSpriteSync();

	ob->gfxChunk = FF_NISASIJUMPL1SPR + 2 + ob->xDirection*2;
	if (ob->actionTimer & 0x8)
		ob->gfxChunk++;
}

void FF_C_Nisasi(CK_object *ob, CK_object *hit)
{
	if (ob->user1 || (ob->user2 == FF_ObjToUser(hit)) || ob->type != FF_CT_Nisasi || (hit->user4 && hit->type != CT_Player))
		return;

	switch (hit->type)
	{
	case CT_Player:
		ob->visible = true;	// force redraw
		
		// K1n9_Duk3 note: I think we don't need to check if the Nisasi hit Keen's
		// head. Situations where a Nisasi hits a sleeping Keen are rare enough.
		
		if (!ff_map.nisasi && !ff_map.invincibilityTimer && !FF_KeenIsDead())
		{
			if (hit->gfxChunk == FF_KEENSLEEP3SPR || hit->gfxChunk == FF_KEENSLEEP4SPR)
			{
				//RemoveObj(ob);
				ob->gfxChunk = FF_YETICLOUD3SPR;
				ob->clipped = CLIP_not;
				ob->type = CT_Friendly;
				ob->user1=ob->velX=0;
				CK_SetAction2(ob, CK_ACTION(FF_ACT_YetiCloud));

				// find the current dream bubble
				for (ob=ck_keenObj->next; ob; ob=ob->next)
				{
					if (ob->type == FF_CT_DreamBubble)
					{
						ob->user4 = -1;	// force bubble to show the Nisasi sprite next
						ob->user1=ob->user2=0;	// force bubble to update the sprite
						break;
					}
				}
			}
			else
			{
				ob->user1 = FF_ObjToUser(hit);
				ob->user2 = hit->xDirection;
				ob->user3 = ob->user4 = 0;	// reset shake count and cooldown
				//ob->clipped = cl_noclip;	// we don't actually need to noclip
				ob->timeUntillThink = 0;
				ff_map.nisasi = ob;	// for input stuff
				hit->clipped = CLIP_normal;	// just in case
				
				// Keen might have been climbing or pogoing or whatever, so change the state
				if (hit->topTI)
				{
					CK_SetAction2(hit, CK_ACTION(CK_ACT_keenStanding));
				}
				else
				{
					CK_SetAction2(hit, CK_ACTION(CK_ACT_keenFall1));
				}
			}
		}
		return;
	case FF_CT_CouncilMember:
	case FF_CT_CouncilMemberDone:
		break;
		
#ifdef EXTRA_INTERACTIONS
	case FF_CT_Crocodent:
		if (hit->currentAction != CK_ACTION(FF_ACT_CrocWalk))
			return;
		break;
	case FF_CT_Chemolain:
		if (hit->gfxChunk > FF_CHEMCYCLE6SPR || hit->gfxChunk < FF_CHEMLOOK1SPR)
			return;
		hit->user2 = 12;
		CK_SetAction2(hit, CK_ACTION(FF_ACT_ChemCycle));
		break;
	case FF_CT_Egg:
	case FF_CT_BirdEgg:
		if (hit->gfxChunk < FF_EGG1SPR || hit->gfxChunk > FF_EGG3SPR)
			return;
		break;
	case FF_CT_SneakyTree:
		if (hit->user1 != ST_LOOK)
		{
			hit->velX = hit->actionTimer = 0;	// stop moving
			hit->user1 = ST_RETRACT;
		}
		if (hit == ck_keenState.platform)
			ck_keenState.platform = NULL;
		break;
	case FF_CT_Kitten:
		if (hit->currentAction->think != &FF_T_KittenIdle)
			return;
		break;
	case FF_CT_Poleknawel:
		if (hit->gfxChunk < FF_POLEKNAWELFALLSPR || hit->gfxChunk > FF_POLEKNAWELPOOPSPR)
		{
#if 1
			// Nisasi CAN grab Poleknawels while they're on a pole, making them fall
			hit->clipped = CLIP_normal;
			hit->timeUntillThink = 0;
			CK_SetAction2(hit, CK_ACTION(FF_ACT_PoleknawelFall));
#else
			// Nisasi can NOT grab Poleknawels while they're on a pole
			return;
#endif
		}
		if (hit->gfxChunk != FF_POLEKNAWELFALLSPR)
			CK_SetAction2(hit, CK_ACTION(FF_ACT_PoleknawelLand));	// make polwknawel stop moving/throwing
		break;
#endif 

	case FF_CT_Slug:
		if (hit->currentAction == CK_ACTION(FF_ACT_slugpiss1))
			CK_SetAction2(hit, CK_ACTION(FF_ACT_slugwalk1));	// abort sliming when hit by Nisasi
		break;
	case FF_CT_XkyLick:
		if ((hit->gfxChunk >= FF_XKYLICKATTACKR1SPR && hit->gfxChunk <= FF_XKYLICKATTACKL3SPR))
			CK_SetAction2(hit, CK_ACTION(FF_ACT_XkyLickHop2));	// abort licking when hit by Nisasi
		break;
	case FF_CT_Warble:
		if (hit->user1 != WARBLE_IDLE)
			return;
		hit->user2 += 8;	// make it blink 8 times
		hit->user3 = 1;		// second phase (next stunner hit makes it hide)
		break;
	case FF_CT_CoronaPlant:
		hit->actionTimer = 0;
		if (hit->user1)
		{
			// close plant's mouth:
			hit->user1 = 0;
			hit->gfxChunk--;
			hit->visible = true;
		}
		break;
	default:
		return;
	}
	// common code to make Nisasi grab onto the hit object:
	hit->user4 = FF_ObjToUser(ob);
	ob->visible = true;
	ob->user1 = FF_ObjToUser(hit);
	ob->user3 = 4*70;	// let go after 4 seconds
	//ob->clipped = cl_noclip;	// we don't actually need to noclip
	ob->timeUntillThink = 0;
}

void FF_R_Nisasi(CK_object *ob)
{
	if (ob->user1)
	{
		uint16_t x, y, g;
		void *spriteptr = &ob->sde;
		
		// K1n9_Duk3 addition 2024-01-02: make Nisasi always active while grabbing something
		ob->active = OBJ_ALWAYS_ACTIVE;
		// end of addition
	
		ob = FF_ObjFromUser(ob->user1);
		x = ob->posX;
		y = ob->posY;
		g = ob->xDirection < 0? FF_NISASIGRABLSPR : FF_NISASIGRABRSPR;

		switch (ob->type)
		{
#ifdef EXTRA_INTERACTIONS
		case FF_CT_Chemolain:
			x += 12*16;
			y +=  1*16;
			g = FF_NISASIGRABRSPR;
			break;
		case FF_CT_Crocodent:
			x += ob->xDirection < 0? -11*16 : 11*16;
			y += -15*16;
			break;
		case FF_CT_SneakyTree:
			x +=  14*16;
			y += -15*16;
			g = FF_NISASIGRABRSPR;
			break;
		case FF_CT_Egg:
		case FF_CT_BirdEgg:
			x += 8*16;
			y += (-6-(ob->gfxChunk - FF_EGG1SPR))*16;
			g = FF_NISASIGRABRSPR;
			break;
		case FF_CT_Poleknawel:
			x += 2*16;
			y += -4*16;
			g = FF_NISASIGRABRSPR;
			break;
		case FF_CT_Kitten:
			switch (ob->gfxChunk)
			{
			case FF_KITTENSTANDSPR:
				x += -27*16;
				y += -14*16;
				g = FF_NISASIGRABRSPR;
				break;
			case FF_KITTENMEOW1SPR:
				x += -34*16;
				y +=  -7*16;
				g = FF_NISASIGRABRSPR;
				break;
			case FF_KITTENMEOW2SPR:
				x += -42*16;
				y += -18*16;
				g = FF_NISASIGRABLSPR;
				break;
			case FF_KITTENMEOW3SPR:
				x += -36*16;
				y += -16*16;
				g = FF_NISASIGRABLSPR;
				break;
			}
			break;
#endif
		case FF_CT_CouncilMember:
		case FF_CT_CouncilMemberDone:
			switch (ob->gfxChunk)
			{
			case FF_COUNCILWALKR1SPR:
				y += -12*16;
				x +=  -1*16;
				break;
			case FF_COUNCILWALKR2SPR:
				y += -12*16;
				x +=   1*16;
				break;
			case FF_COUNCILSTANDRSPR:
				y += -11*16;
				//x += 0*PIXGLOBAL;
				g = FF_NISASIGRABRSPR;	// because xdir is swapped while turning around
				break;
			case FF_COUNCILMOPR1SPR:
				y += -11*16;
				x +=   1*16;
				break;
			case FF_COUNCILMOPR2SPR:
				y += -12*16;
				x +=   1*16;
				break;
			case FF_COUNCILWALKL1SPR:
				y += -12*16;
				x +=  -8*16;
				break;
			case FF_COUNCILWALKL2SPR:
				y += -12*16;
				x +=  -9*16;
				break;
			case FF_COUNCILSTANDLSPR:
				y += -11*16;
				x +=  -8*16;
				g = FF_NISASIGRABLSPR;	// because xdir is swapped while turning around
				break;
			case FF_COUNCILMOPL1SPR:
				y += -11*16;
				x +=  -7*16;
				break;
			case FF_COUNCILMOPL2SPR:
				y += -12*16;
				x +=  -8*16;
				break;
			}
			break;
		case FF_CT_Slug:
			switch (ob->gfxChunk)
			{
			case FF_SLUGWALKR1SPR:
				x += -4*16;
				y += -9*16;
				break;
			case FF_SLUGWALKR2SPR:
				x += -6*16;
				y += -8*16;
				break;
			case FF_SLUGWALKL1SPR:
				x += -12*16;
				y +=  -9*16;
				break;
			case FF_SLUGWALKL2SPR:
				x += -10*16;
				y +=  -8*16;
				break;
			}
			break;
		case FF_CT_XkyLick:
			g = ob->xDirection < 0? FF_NISASIGRABRSPR : FF_NISASIGRABLSPR;	// opposite of regular mapping!
			switch (ob->gfxChunk)
			{
			case FF_XKYLICKR1SPR:
				x +=  -9*16;
				y += -24*16;
				break;
			case FF_XKYLICKR2SPR:
				x +=  -9*16;
				y += -26*16;
				g = FF_NISASIGRABLSPR;	// because xdir is ignored in search animation
				break;
			case FF_XKYLICKR3SPR:
				x +=  -6*16;
				y += -25*16;
				break;
			case FF_XKYLICKR4SPR:
				x +=  -6*16;
				y += -22*16;
				break;
			case FF_XKYLICKL1SPR:
				//x += 0*PIXGLOBAL;
				y += -23*16;
				g = FF_NISASIGRABRSPR;	// because xdir is ignored in search animation
				break;
			case FF_XKYLICKL2SPR:
				//x += 0*PIXGLOBAL;
				y += -25*16;
				break;
			case FF_XKYLICKL3SPR:
				x +=  -1*16;
				y += -24*16;
				break;
			case FF_XKYLICKL4SPR:
				x +=  -1*16;
				y += -21*16;
				break;
			}
			break;
//
//	NOTE: The ";" lines below are used to add some padding and thus ensure
// that the compiled code for the "classic" version has the same size as the
// compiled code for the "Xky-Keen" version. The only reason to do this was to
// keep the differences between the two executables minimal, so that we could
// squeeze both versions onto a single floppy disk.
//	
		case CT_Player:
			if (CK_INT(ff_classicKeen, 0))
			{
			// compatible with the Vanilla-Keen sprites
			switch (ob->gfxChunk)
			{
			case FF_KEENSTANDRSPR:
				y -= 15*16;
				x -=  1*16;
				;
				break;
			case FF_KEENRUNR1SPR:
				y -= 14*16;
				x +=  2*16;
				;
				break;
			case FF_KEENRUNR2SPR:
				y -= 13*16;
				x +=  2*16;
				;
				break;
			case FF_KEENRUNR3SPR:
				y -= 14*16;
				x +=  0*16;
				;
				break;
			case FF_KEENRUNR4SPR:
				y -= 13*16;
				x +=  1*16;
				;
				break;
			case FF_KEENJUMPR2SPR:
				y -= 15*16;
				x -=  3*16;
				;
				break;
			case FF_KEENJUMPR3SPR:
				y -= 14*16;
				x +=  0*16;
				;
				break;
			case FF_KEENSTANDLSPR:
				y -= 15*16;
				x -=  7*16;
				;
				break;
			case FF_KEENRUNL1SPR:
				y -= 14*16;
				x -= 10*16;
				;
				break;
			case FF_KEENRUNL2SPR:
				y -= 13*16;
				x -=  8*16;
				;
				break;
			case FF_KEENRUNL3SPR:
				y -= 14*16;
				x -=  7*16;
				;
				break;
			case FF_KEENRUNL4SPR:
				y -= 13*16;
				x -=  8*16;
				;
				break;
			case FF_KEENJUMPL2SPR:
				y -= 15*16;
				x -=  4*16;
				;
				break;
			case FF_KEENJUMPL3SPR:
				y -= 15*16;
				x -=  6*16;
				;
				break;
			}
			}
			else
			{
			// compatible with the XkyKeen sprites
			switch (ob->gfxChunk)
			{
			case FF_KEENSTANDRSPR:
				y -= 15*16;
				x +=  0*16;
				;
				break;
			case FF_KEENRUNR1SPR:
				y -= 14*16;
				x -=  1*16;
				;
				break;
			case FF_KEENRUNR2SPR:
				y -= 12*16;
				x +=  0*16;
				;
				break;
			case FF_KEENRUNR3SPR:
				y -= 14*16;
				x -=  1*16;
				;
				break;
			case FF_KEENRUNR4SPR:
				y -= 13*16;
				x +=  0*16;
				;
				break;
			case FF_KEENJUMPR2SPR:
				y -= 12*16;
				x -=  1*16;
				;
				break;
			case FF_KEENJUMPR3SPR:
				y -= 12*16;
				x +=  0*16;
				;
				break;
			case FF_KEENSTANDLSPR:
				y -= 15*16;
				x -=  7*16;
				;
				break;
			case FF_KEENRUNL1SPR:
				y -= 14*16;
				x -= 8*16;
				;
				break;
			case FF_KEENRUNL2SPR:
				y -= 12*16;
				x -=  8*16;
				;
				break;
			case FF_KEENRUNL3SPR:
				y -= 14*16;
				x -=  7*16;
				;
				break;
			case FF_KEENRUNL4SPR:
				y -= 13*16;
				x -=  8*16;
				;
				break;
			case FF_KEENJUMPL2SPR:
				y -= 12*16;
				x -=  8*16;
				;
				break;
			case FF_KEENJUMPL3SPR:
				y -= 12*16;
				x -=  9*16;
				;
				;
				;
				;
				;
				break;
			}
			}
			break;
		case FF_CT_Warble:
			g = FF_NISASIGRABLSPR;
			switch (ob->gfxChunk)
			{
			case FF_WARBLEGROW6SPR:
			case FF_WARBLEGROW7SPR:
			case FF_WARBLEGROW8SPR:
			case FF_WARBLEGROW9SPR:
				x +=  -8*16;
				y += -11*16;
				break;
			case FF_WARBLEGROW10SPR:
			case FF_WARBLEGROW11SPR:
			case FF_WARBLEGROW12SPR:
			case FF_WARBLEGROW13SPR:
				x +=  -9*16;
				y += -13*16;
				break;
			}
			break;
		case FF_CT_CoronaPlant:
			switch (ob->gfxChunk)
			{
			case FF_CORONAL1SPR:
				x += -11*16;
				y += -12*16;
				g = FF_NISASIGRABLSPR;
				break;
			case FF_CORONAU1SPR:
				x +=  -2*16;
				y += -16*16;
				g = FF_NISASIGRABRSPR;
				break;
			case FF_CORONAR1SPR:
				x +=  1*16;
				y += -8*16;
				g = FF_NISASIGRABRSPR;
				break;
			}
		}
		
		RF_PlaceSpriteAfter(spriteptr, x, y, g, false, ob->zLayer, &ob->sde);
		return;
	}
	
	// K1n9_Duk3 addition 2024-01-02: Nisasi is not grabbing anything, so reset it to active (NOT always active)
	ob->active = OBJ_ACTIVE;
	// end of addition
	
	if (ob->bottomTI)
		ob->velY = 0;
	if (ob->velX && (ob->leftTI || ob->rightTI || ob->topTI))
	{
		ob->type = FF_CT_Nisasi;
		ob->velX = 0;
		ob->actionTimer = US_RndT();	// sliding down on the walls is also random
		ob->timeUntillThink = 21;
		if (ob->topTI)
		{
			ob->xDirection = (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)? -1 : 1;
			ob->gfxChunk = FF_NISASIFLOORSPR;
		}
		else
		{
			ob->xDirection = -ob->xDirection;
			ob->gfxChunk = FF_NISASIHANGLSPR + 2 + ob->xDirection*2;
		}
	}
	FF_R_Draw(ob);
}

void FF_DropNisasi(void)	// for Keen
{
	 CK_object *ob;
	if (ff_map.nisasi)
	{
		ob = ff_map.nisasi;
		ff_map.nisasi = NULL;
		//ob->clipped = cl_midclip;	// we don't use noclip anymore
		ob->xDirection = ob->user2;
		ob->velX = ob->velY = 1;
		ob->actionTimer=ob->user1=ob->user2=ob->user3=ob->user4 = 0;
		ob->type = FF_CT_NisasiCooldown;
		SD_SetMusicRate(DEFAULT_MUSIC_RATE);
	}
}


void FF_ObjDropNisasi(CK_object *ob)	// for other objects
{
	// Make Nisasi let go (if creature is grabbed by a Nisasi
	
	// check if temp4 value is a pointer to an entry in the object array
	{
		CK_object *n = FF_ObjFromUser(ob->user4);
		if (n && n->type == FF_CT_Nisasi && n->user1 == FF_ObjToUser(ob))
		{
			FF_NisasiLetGo(n);
		}
	}
}


/*
==============================================================================
	COUNCIL PAGE

	Stands in place, waiting to get mind-controlled by the player. Can only
	walk left/right and use switches. No jumping/falling or shooting.

	temp1 - idle counter (when controlled by player), destX for sliding
	temp2 - internal state (see enum)
	temp3 - tileX for switches
	temp4 - tileY for switches
==============================================================================
*/

enum {
	PG_INACTIVE, PG_NORMAL, PG_DREAM, PG_STOPDREAM,
	PG_USE, PG_AFTERUSE, PG_ACTIVATE, PG_INACTIVATE,
	PG_ENTERMACHINE, PG_LEAVEMACHINE, PG_LEFTMACHINE
};

#define PAGE_DREAMDELAY (2*70)
#define PAGE_WALKSPEED	(12)	// half of Keen's walking speed
#define PAGE_SLIDESPEED	(6)



void FF_SpawnCouncilPage(int16_t tileX, int16_t tileY, int16_t type)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX) + 8*16;
	new_object->posY = RF_TileToUnit(tileY) - 15*16;
	new_object->type = FF_CT_Page;
	new_object->xDirection = IN_motion_Right;
	new_object->gfxChunk = FF_PAGESLEEPRSPR;
	//new->user2 = PG_INACTIVE;	// redundant (PG_INACTIVE is 0 and temp2 is already set to 0)
	CK_SetAction(new_object, type? CK_ACTION(FF_ACT_PageWalk) : CK_ACTION(FF_ACT_Page));
}

void FF_T_Page(CK_object *ob)
{
	switch (ob->user2)
	{
	case PG_ACTIVATE:
		if (ob->next->gfxChunk == 0)	// is door open?
			ob->user2 = PG_LEAVEMACHINE;
		// no break here!
	case PG_INACTIVE:
inactive:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 8)
		{
			ob->actionTimer -= 8;
			ob->gfxChunk++;
		}
		if (ob->gfxChunk > FF_PAGESTAND3SPR || ob->gfxChunk < FF_PAGESTAND1SPR)
			ob->gfxChunk = FF_PAGESTAND1SPR;
		break;
	case PG_INACTIVATE:
		if (ob->next->gfxChunk == FF_SLIDEDOORRED8SPR)	// door is closed?
		{
			// wait 1/4 second before switching to Keen
			if ((ob->user1 += SD_GetSpriteSync()) >= 70/4)
			{
				// make Keen controllable again:
				ck_keenObj->user1 = 1;	// make Keen wait until door is open
				ck_keenObj->gfxChunk = FF_KEENSTANDSPR;
				/*
				if (!OnScreen(player))
					CenterActor(player);
				*/
				
				// make Page inactive:
				ob->user2 = PG_INACTIVE;
				ff_map.mindcontrolled = NULL;

				// open Keen's door:
				for (ob=ck_keenObj; ob; ob=ob->next)
				{
					if (ob->type == FF_CT_SlideDoor && ob->user2 == FF_SLIDEDOORBLUE1SPR)
					{
						ob->user1 = 0;
						ob->actionTimer = -70/4;	// wait 1/4 second before opening the door
					}
				}
				return;	// because ob is no longer valid
			}
		}
		goto inactive;
	case PG_ENTERMACHINE:
		ck_nextY = -(SD_GetSpriteSync()*4);
		if ((ob->actionTimer += SD_GetSpriteSync()*4) >= (256/4))
		{
			ob->actionTimer -= (256/4);
			if (++ob->user1 == 4)
			{
				// change Page state:
				ck_nextY += ob->actionTimer;
				ob->user1 = ob->user3 = 0;
				ob->user2 = PG_INACTIVATE;

				// close Page's door:
				ob = ob->next;
				ob->user1 = 1;
				return;	// because ob is no longer valid
			}
		}
		ob->gfxChunk = (ob->user1 & 1)? FF_PAGESWITCH1SPR : FF_PAGESWITCH2SPR;
		break;
	case PG_LEAVEMACHINE:
		FF_DoWeakGravity(ob);
		ob->gfxChunk = FF_PAGEDREAM1SPR;
		if (ob->topTI)
		{
			// change Page state:
			ob->velY = 0;
			ob->user2 = PG_LEFTMACHINE;
			ob->gfxChunk = FF_PAGESTAND1SPR;
		}
		break;
	case PG_LEFTMACHINE:
		ob->user1 += SD_GetSpriteSync();
		if (ck_inputFrame.dir == IN_dir_None && ob->user1 < 70)
		{
			ck_nextY = 16;	// push down
			goto inactive;
		}
		goto benormal;
	case PG_USE:
		ob->gfxChunk = FF_PAGESWITCH1SPR;
		// Is the door still active?
		if (ob->clipRects.unitX1 == ob->user1 && !FF_DoorsActive())
		{
			if ((TI_ForeMisc(FORESPOT(ob->user3, ob->user4)) & INTILE_TYPEMASK) == MF_MindMachine)
			{
				// change Page state:
				ob->user2 = PG_ENTERMACHINE;
				ob->user1 = ob->user3 = ob->user4 = ob->actionTimer = 0;	// reset counters
			}
			else if ((ob->actionTimer += SD_GetSpriteSync()) >= 8)	// short delay before actually using the switch
			{
				FF_ToggleSwitch(ob->user3, ob->user4);
				ob->actionTimer=ob->user1=ob->user3=ob->user4 = 0;
				ob->user2 = PG_AFTERUSE;
			}
		}
		else
		{
			// slide to the middle of the switch tile:
			 int16_t maxspeed = SD_GetSpriteSync()*PAGE_SLIDESPEED;
			ck_nextX = ob->user1-ob->clipRects.unitX1;
			if (ck_nextX < -maxspeed)
			{
				ck_nextX = -maxspeed;
			}
			else if (ck_nextX > maxspeed)
			{
				ck_nextX = maxspeed;
			}
			ck_nextY = abs(ck_nextX) + 16;	// manually push to floor
		}
		break;
	case PG_AFTERUSE:
		ob->gfxChunk = FF_PAGESWITCH2SPR;
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 8)
		{
			ob->actionTimer = 0;
			ob->user2 = PG_NORMAL;
		}
		break;
	case PG_DREAM:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 15)
		{
			ob->actionTimer -= 15;
			if (ob->gfxChunk == FF_PAGEDREAM1SPR)
				ob->gfxChunk++;
			else
				ob->gfxChunk = FF_PAGEDREAM1SPR;
		}
		ob->user1 += SD_GetSpriteSync();
		if (ck_inputFrame.dir != IN_dir_None || ob->user1 > 10*70)
		{
			ob->actionTimer = 0;
			ob->user2 = PG_STOPDREAM;
			ob->gfxChunk = FF_PAGESTAND1SPR;
			FF_RemoveDreamBubbles();
		}
		break;
	case PG_STOPDREAM:
		if ((ob->actionTimer += SD_GetSpriteSync()) < 15)
			break;
benormal:
		ob->actionTimer=ob->user1 = 0;
		ob->user2 = PG_NORMAL;
		// no break here!
	default:
		if (ck_inputFrame.yDirection == -1 && !ff_keenUpHeld)
		{
			uint16_t h, x, y, i;
			x = ob->clipRects.tileXmid;
			y = ob->clipRects.tileY1;
			i = x + ((y) * CA_GetMapWidth());
			h = ob->clipRects.tileY2 - y+1;
			while (h--)
			{
				switch (TI_ForeMisc(FF_MapAt(1, i)) & INTILE_TYPEMASK)
				{
				case MF_SwitchOn:
				case MF_SwitchOff:
				case MF_BridgeSwitch:
					// set target x position:
					ob->user1 = RF_TileToUnit(x);
startuse:
					// save x/y coordinates of the switch tile:
					ob->user3 = x;
					ob->user4 = y;
					// start using the switch/button/whatever:
					ob->user2 = PG_USE;
					ob->actionTimer = 0;
					ob->gfxChunk = FF_PAGESWITCH1SPR;
					ff_keenUpHeld = true;
					return;
				case MF_MindMachine:
					ob->user1 = RF_TileToUnit(x) - 8*16;
					if ((TI_ForeMisc(FF_MapAt(1, i+1)) & INTILE_TYPEMASK) == MF_MindMachine)
						ob->user1 += 256;
					goto startuse;
				}
				i += CA_GetMapWidth();
				y++;
			}
		}
		if (ck_inputFrame.xDirection)
		{
			ob->xDirection = ck_inputFrame.xDirection;
			ck_nextX = ob->xDirection*SD_GetSpriteSync()*PAGE_WALKSPEED;
			ob->gfxChunk = ob->xDirection +1+FF_PAGEWALKLSPR;
			ob->user1 = 0;	// reset idle counter
		}
		else
		{
			ob->gfxChunk = ob->xDirection +1+ FF_PAGESLEEPLSPR;
			if ((ob->user1 += SD_GetSpriteSync()) >= PAGE_DREAMDELAY)
			{
				// start idle/dream animation
				ob->user2 = PG_DREAM;	
				ob->gfxChunk = FF_PAGESTAND1SPR;
				FF_SpawnDreamBubble(ob->posX+4*16, ob->posY-30*16, 1);
				ob->user1 = 0;
			}
		}
		ck_nextY = abs(ck_nextX) + 16;	// manually push to floor
	}
}

void FF_R_Page(CK_object *ob)
{
	uint16_t y;

	// don't fall:
	if (!ob->topTI && (ob->user2 == PG_NORMAL || ob->user2 == PG_USE || ob->user2 == PG_LEFTMACHINE))
	{
		FF_MoveObjHoriz(ob, -ob->deltaPosX);
		FF_MoveObjVert(ob, -ob->deltaPosY);
	}

	y = ob->posY;
	if (ob->user2 == PG_NORMAL)
	{
		y -= ((ob->posX-1) & (16 << 3)) >> 3;	// adds up & down movement while walking
	}
	RF_AddSpriteDraw(&ob->sde, ob->posX, y, ob->gfxChunk, false, ob->zLayer);
}


CK_object *FF_FindPageObj(uint16_t infoval)
{
	CK_object *ob;
	uint16_t x, y;
	
	y = infoval & 0xFF;
	x = infoval >> 8;

	for (ob=ck_keenObj; ob; ob=ob->next)
	{
		if (ob->type == FF_CT_Page && ob->clipRects.tileX1 == x && ob->clipRects.tileY2 == y)
		{
			return ob;
		}
	}
	return NULL;
}

void FF_ControlPage(CK_object *ob)
{
	// enable Page:
	ob->user2 = PG_ACTIVATE;
	ob->user1 = 0;
	ff_map.mindcontrolled = ob;
	/*
	if (!OnScreen(ob))
		CenterActor(ob);
	*/

	// open Page's door:
	ob = ob->next;	// door object ALWAYS follows directly after Page object
	ob->user1 = 0;
	ob->actionTimer = -70/4;	// wait 1/4 second before opening
}

void FF_T_KeenEnterMachine(CK_object *ob)
{
	if (++ob->user1 == 5)
	{
		ob->user2 = FF_ObjToUser(FF_SpawnSlideDoor(ob->clipRects.tileX1, ob->clipRects.tileY2, 1));
		ob->user1 = 0;
		ob->gfxChunk = FF_KEENSTANDLSPR;
		ob->timeUntillThink = 8;
		ob->currentAction = CK_ACTION(FF_ACT_keenMindcontrol);
		ob->type = CT_Friendly;	// enemies won't interact with Keen
	}
	else
	{
		ck_nextY = -4*16;
		ob->gfxChunk = (ob->user1 & 1)? FF_KEENWALKAWAY1SPR : FF_KEENWALKAWAY2SPR;
		SD_PlaySound(FF_KEENWALK1SND + (ob->user1 & 1));
	}
}


void FF_T_KeenMindcontrol(CK_object *ob)
{
	if (ob->user1)
	{
		ob->gfxChunk = FF_KEENSTANDSPR;
		if ((FF_ObjFromUser(ob->user2))->gfxChunk == 0)	// is door open?
		{
			CK_RemoveObj(FF_ObjFromUser(ob->user2));	// remove Keen's door object
			ob->user1=ob->user2=ob->user3=ob->user4 = 0;
			ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
			ob->type = CT_Player;	// enemies will interact with Keen again
		}
	}
	else if (ob->user3)
	{
		ob->gfxChunk = FF_KEENSTANDSPR;
		if ( (FF_ObjFromUser(ob->user2))->gfxChunk == FF_SLIDEDOORBLUE8SPR )
		{
			// Keen's door is closed, control the Page after 1/4 second!
			if ((ob->actionTimer += SD_GetSpriteSync()) >= 70/4)
			{
				FF_ControlPage(FF_ObjFromUser(ob->user3));
				ob->user3 = 0;
			}
		}
		else
		{
			ob->actionTimer = 0;
		}
	}
	else
	{
		switch (ck_inputFrame.xDirection)
		{
		case IN_motion_Left:
			ob->gfxChunk = FF_KEENSTANDLSPR;
			break;
		case IN_motion_Right:
			ob->gfxChunk = FF_KEENSTANDRSPR;
			break;
		default:
			ob->gfxChunk = FF_KEENSTANDSPR;
		}
	}
}


/*
==============================================================================
	SLIDING DOOR

	Just eye candy. Opens or closes when a mindcontrol machine is used.

	temp1 - direction (0 = opening, 1 = closing)
	temp2 - base sprite number
==============================================================================
*/

#define DOOR_ANIM_DELAY 4


CK_object *FF_SpawnSlideDoor(int16_t tileX, int16_t tileY, int16_t type)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_SlideDoor;
	new_object->clipped = CLIP_not;
	new_object->zLayer = FOREGROUNDPRIORITY;
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->posX = RF_TileToUnit(tileX) + 8*16;;
	new_object->posY = RF_TileToUnit(tileY) - 18*16;
	switch (type)
	{
	case 0:
		new_object->user2 = FF_SLIDEDOORRED1SPR;
		new_object->gfxChunk = FF_SLIDEDOORRED8SPR;
		break;
	case 1:
		new_object->user2 = FF_SLIDEDOORBLUE1SPR;
		//new->gfxChunk = 0;	// redundant
		break;
	}
	new_object->user1 = 1;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_SlideDoor));
	return new_object;
}

void FF_T_SlideDoor(CK_object *ob)
{
	if (ob->user1)
	{
		// door is closing
		if (ob->gfxChunk != ob->user2+7)
		{
			// closing animation is not done yet
			if ((ob->actionTimer += SD_GetSpriteSync()) >= DOOR_ANIM_DELAY)
			{
				ob->actionTimer -= DOOR_ANIM_DELAY;
				if (!ob->gfxChunk)
				{
					ob->gfxChunk = ob->user2;
				}
				else
				{
					ob->gfxChunk++;
				}
			}
		}
		else
		{
			ob->actionTimer = 0;
		}
	}
	else
	{
		// door is opening
		if (ob->gfxChunk && CK_ObjectVisible(ob))
		{
			// opening animation is not done yet
			if ((ob->actionTimer += SD_GetSpriteSync()) >= DOOR_ANIM_DELAY)
			{
				ob->actionTimer -= DOOR_ANIM_DELAY;
				if (ob->gfxChunk == ob->user2)
				{
					ob->gfxChunk = 0;
				}
				else
				{
					ob->gfxChunk--;
				}
			}
		}
		else
		{
			ob->actionTimer = 0;
		}
	}
}


/*
==============================================================================
	CLONE MACHINE

	Creates dummy clone objects

	temp1 - machine type (janitor, poleknawel ...)
	temp2 - janitor spawn counter
	temp3 - spawn countdown
	temp4 - combined x/y position
------------------------------------------------------------------------------
	CLONE OBJECT

	temp1 - clone type; no animation or movement while high bit is set
	temp2 - clipping value
------------------------------------------------------------------------------
	SCANNING/CLONING ANIMATION

	temp1 - pointer to scanned/cloned object
	temp2 - movement counter
	temp3 - animation counter
==============================================================================
*/

enum {CLONE_POLEKNAWEL, CLONE_MOLET, CLONE_CHEMOLAIN, CLONE_NISASI, CLONE_JANITOR, NUMCLONETYPES};


#define SCAN_HEIGHT 42	// how many pixels the scan animation moves up or down
#define SCAN_WIDTH 32	// width of the scan animation sprite, in pixels
#define SCAN_YOFF -7
#define SCAN_DIR IN_motion_Down	// should be either motion_Up or motion_Down
#define SCAN_ANIM_DELAY 4	// delay for sprite change in tics (lower numbers -> faster animation)
#define SCAN_MOVE_DELAY 2	// delay for movement (up/down)
#define SCAN_INTERVAL (SCAN_HEIGHT*SCAN_MOVE_DELAY + 2*SCAN_WIDTH)	// time between the start of a scan and the start of the next scan

// please don't modify this:
#if (SCAN_DIR == IN_motion_Up)
#define SCAN_START_Y (SCAN_YOFF*16)
#elif (SCAN_DIR == IN_motion_Down)
#define SCAN_START_Y ((SCAN_YOFF-SCAN_HEIGHT)*16)
#else
#error bad SCAN_DIR
#endif

static int16_t cloneBaseSprite[NUMCLONETYPES] = {FF_POLEKNAWELWALK1SPR, FF_MOLETWALKR1SPR, FF_CHEMLOOK1SPR, FF_NISASIFLOORSPR, FF_COUNCILSTANDRSPR};

void FF_SpawnCloneMachine(int16_t tileX, int16_t tileY, int16_t type, int16_t dir)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_CloneMachineOff;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY+1);
	new_object->user1 = type;
	new_object->user4 = tileX | (tileY << 8);
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->clipped = CLIP_not;
	new_object->gfxChunk = cloneBaseSprite[type];
	new_object->posY -= VH_GetSpriteTableEntry(new_object->gfxChunk - FF_STARTSPRITES).yh + 1;	// + 1 so it won't clip into the floor
	new_object->xDirection = dir;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_CloneMachine));

	if (type == CLONE_JANITOR)
		new_object->gfxChunk = 0;
}

void FF_AnimateCloneObj(CK_object *ob)
{
	ob->actionTimer += SD_GetSpriteSync();
	switch (ob->user1)
	{
	case CLONE_POLEKNAWEL:
		if (ob->type == FF_CT_CloneRising)
		{
			ob->gfxChunk = FF_POLEKNAWELPOOPSPR;
			break;
		}
		while (ob->actionTimer >= 10)
		{
			ob->actionTimer -= 10;
			if (++ob->gfxChunk > FF_POLEKNAWELWALK2SPR)
				ob->gfxChunk = FF_POLEKNAWELWALK1SPR;
		}
		break;
	case CLONE_MOLET:
		while (ob->actionTimer >= 10)
		{
			ob->actionTimer -= 10;
			if (++ob->gfxChunk > FF_MOLETWALKR4SPR)
				ob->gfxChunk = FF_MOLETWALKR1SPR;
		}
		break;
	case CLONE_CHEMOLAIN:
		if (ob->gfxChunk == FF_CHEMLOOK1SPR)
		{
			if (ob->actionTimer > 180)
			{
				ob->actionTimer = 0;
				ob->gfxChunk = FF_CHEMLICK1SPR;
			}
		}
		else
		{
			while (ob->actionTimer >= 10)
			{
				ob->actionTimer -= 10;
				if (++ob->gfxChunk > FF_CHEMLICK2SPR)
				{
					ob->actionTimer = (US_RndT() >> 2);
					ob->gfxChunk = FF_CHEMLOOK1SPR;
					break;
				}
			}
		}
		break;
	case CLONE_NISASI:
		while (ob->actionTimer >= 30)
		{
			ob->actionTimer -= 54;
			ob->gfxChunk ^= (FF_NISASIFLOORSPR^FF_NISASIFLOOR2SPR);
		}
		break;
	case CLONE_JANITOR:
		if (!ob->gfxChunk)
		{
			ob->actionTimer = 0;
			break;
		}
		if (ob->actionTimer >= 4*70)
		{
			ob->actionTimer = US_RndT();
			if (ob->gfxChunk == FF_COUNCILSTANDRSPR)
			{
				ob->gfxChunk = FF_COUNCILWALKR1SPR;
			}
			else
			{
				ob->gfxChunk = FF_COUNCILSTANDRSPR;
			}
		}
		break;
	}
}

void FF_T_CloneMachine(CK_object *ob)
{
	CK_object *scan, *clone;

	if (!(ob->user1 & 0x8000))
	{
		FF_AnimateCloneObj(ob);
	}

	if (ob->type == FF_CT_CloneMachineOn)
	{
		if ((ob->user3 += SD_GetSpriteSync()) >= SCAN_INTERVAL && !(ob->user1 & 0x8000))
		{
			ob->user3 = 0;	// restart countdown

			//
			// start the scanning animation
			//

			scan = CK_GetNewObj(true);
			scan->zLayer = FOREGROUNDPRIORITY-1;
			scan->type = CT_Friendly;	// don't clip the sprite of the scanned object
			scan->active = OBJ_ALWAYS_ACTIVE;
			scan->user1 = FF_ObjToUser(ob);	// scanner animation gets a pointer to the scanned object
			scan->posX = ob->posX;
			scan->posY = ob->clipRects.unitY2 + SCAN_START_Y;
			scan->clipped = CLIP_not;
			scan->gfxChunk = FF_SCANNER1SPR;
			CK_SetAction(scan, CK_ACTION(FF_ACT_CloneAnimation));

			//
			// spawn the clone (based on clone machine type)
			//
			clone = CK_GetNewObj(true);
			clone->active = OBJ_ALWAYS_ACTIVE;
			clone->posY = ob->posY;
			clone->posX = ob->posX + (4*256)*ob->xDirection;
			clone->xDirection = ob->xDirection;
			clone->yDirection = IN_motion_Down;
			clone->user1 = ob->user1;
			if (ob->user1 == CLONE_POLEKNAWEL)		// Nisaba: spawning position patch
			{
				clone->posX = ob->posX + (-10*256)*ob->xDirection;
			}
			if (ob->user1 == CLONE_MOLET)
			{
				clone->posX = ob->posX + (0*256)*ob->xDirection;
				clone->posY = ob->posY + (-4*256)*ob->xDirection;
			}
			if (ob->user1 == CLONE_NISASI)
			{
				clone->posX = ob->posX + (-4*256)*ob->xDirection;
				clone->posY = ob->posY + (4*256)*ob->xDirection;
			}
			if (ob->user1 == CLONE_JANITOR)
			{
				if ((++ob->user2) == 8)
					ob->type = CT_Friendly;	// disable the clone machine
			}
			clone->type = FF_CT_Clone;
			clone->gfxChunk = ob->gfxChunk;
			clone->actionTimer = ob->actionTimer - SD_GetSpriteSync();
			CK_SetAction(clone, CK_ACTION(FF_ACT_CloneObj));
#if (SCAN_DIR == IN_motion_Up)
			clone->user2 = (VH_GetSpriteTableEntry(clone->gfxChunk - FF_STARTSPRITES).yh - VH_GetSpriteTableEntry(clone->gfxChunk - FF_STARTSPRITES).originY) >> G_P_SHIFT;
#elif (SCAN_DIR == IN_motion_Down)
			clone->user2 = SCAN_HEIGHT - (((VH_GetSpriteTableEntry(clone->gfxChunk - FF_STARTSPRITES).yh - VH_GetSpriteTableEntry(clone->gfxChunk - FF_STARTSPRITES).originY) >> G_P_SHIFT) - spritetable[clone->gfxChunk - FF_STARTSPRITES].height);
#endif
			clone->user1 |= 0x8000;	// hack to prevent clone object from animating
			ob->user1 |= 0x8000;	// also prevent master object from animating

			//
			// we're re-using the scan variable here to spawn the clone animation
			//
			scan = CK_GetNewObj(true);
			scan->zLayer = FOREGROUNDPRIORITY-1;
			scan->active = OBJ_ALWAYS_ACTIVE;
			scan->type = FF_CT_CloneAnimation;
			scan->user1 = FF_ObjToUser(clone);	// animation gets a pointer to the clone object
			scan->posX = clone->posX;
			scan->posY = clone->clipRects.unitY2 + SCAN_START_Y;
			scan->clipped = CLIP_not;
			scan->gfxChunk = FF_SCANNER1SPR;
			CK_SetAction(scan, CK_ACTION(FF_ACT_CloneAnimation));
		}
	}
	else if (ob->type == CT_Friendly)
	{
		if ((ob->user3 += SD_GetSpriteSync()) >= 2*70)
		{
			ck_gameState.levelState = ex_clonedjanitor;
		}
	}
	else
	{
		ob->user3 = 0;
	}
}

//Old:
/* 
void C_CloneMachine(objtype *ob, objtype *hit)
{
	int xdist;

	if (ob->user1 == CLONE_JANITOR)
	{
		switch (hit->type)
		{
		case CT_CouncilMember:
		case CT_CouncilMemberDone:
			xdist = hit->posX - ob->posX;
			if (xdist > -2*PIXGLOBAL && xdist < 2*PIXGLOBAL)
			{
				// remove the janitor object
				RemoveObj(hit);
				// make the clone machine display the janitor sprite
				ob->gfxChunk = COUNCILSTANDRSPR;
				ob->visible = true;	// force redraw
				// and deactivate the clone machine by default
				ob->type = CT_CloneMachineOff;
			}
		}
	}
}
*/ 
//New:
void FF_C_CloneMachine(CK_object *ob, CK_object *hit)
{
	int16_t xdist;
	uint16_t tile;

	if (ob->user1 == CLONE_JANITOR)
	{
		switch (hit->type)
		{
		case FF_CT_CouncilMember:
		case FF_CT_CouncilMemberDone:
			xdist = hit->posX - ob->posX;
			if (xdist > -2*16 && xdist < 2*16)
			{
				// remove the janitor object
				CK_RemoveObj(hit);
				// make the clone machine display the janitor sprite
				ob->gfxChunk = FF_COUNCILSTANDRSPR;
				ob->visible = true;	// force redraw
				
				// enable first button of activation sequence (at x=4, y=92):
				tile = 3195;	// tile number to place
				RF_ReplaceTiles(&tile, 1, 4, 92, 1, 1);
			}
			break;
			
		case CT_Player:
			// show janitor message 10 if not done already:
			if (ob->gfxChunk && !(ff_state.janitorMessageShown & (1 << 10)))
			{
				FF_JanitorDialog(10);
			}
			break;
		}
	}
}

void FF_T_CloneAnimation(CK_object *ob)
{
	ob->actionTimer += SD_GetSpriteSync();
	while (ob->actionTimer >= SCAN_ANIM_DELAY)
	{
		ob->actionTimer -= SCAN_ANIM_DELAY;

		if (++ob->gfxChunk > FF_SCANNER3SPR)
			ob->gfxChunk = FF_SCANNER1SPR;
	}

	ob->user2 += SD_GetSpriteSync();
	while (ob->user2 >= SCAN_MOVE_DELAY)
	{
		ob->user2 -= SCAN_MOVE_DELAY;

		if (++ob->user3 > SCAN_HEIGHT)
		{
			//
			// allow clone/base object to animate again
			//
			(FF_ObjFromUser(ob->user1))->user1 &= 0x7FFF;
			if (ob->type == FF_CT_CloneAnimation)
			{
				(FF_ObjFromUser(ob->user1))->user2 = 0;	// nothing clipped off anymore
				(FF_ObjFromUser(ob->user1))->visible = true;	// force redraw
			}

			//
			// kill the animation object
			//
			CK_RemoveObj(ob);
			return;
		}

		ck_nextY += SCAN_DIR*16;
		if (ob->type == FF_CT_CloneAnimation)
		{
			CK_object *clone = FF_ObjFromUser(ob->user1);

			if (clone->user2)
			{
				clone->user2--;	// make one more line of the sprite visible
				if (!clone->user2)
					clone->user1 &= 0x7FFF;	// allow object to move and animate
				clone->visible = true;	// force redraw
			}
		}

	}
}

void FF_T_CloneObj(CK_object *ob)
{
	//
	// do nothing while cloning animation is still active
	//
	if (ob->user1 & 0x8000)
		return;

	//
	// animate the clone
	//
	FF_AnimateCloneObj(ob);

	if (ob->type == FF_CT_CloneRising)
	{
		if (ob->bottomTI)	// hit the ceiling?
		{
			CK_RemoveObj(ob);
		}
		else
		{
			FF_AccelerateY(ob, IN_motion_Up, 70);
		}
	}
	else
	{
		//
		// clone either falls down or slides to the right
		//
		if (ob->topTI)
		{
			ck_nextX = (16/2) * SD_GetSpriteSync() * ob->xDirection;
			ck_nextY = abs(ck_nextX)+1;	// also push down (stick to ground)
			ob->velY = 0;
		}
		else
		{
			FF_DoWeakGravity(ob);
		}

		//
		// change the clone's state when it hits a platform blocker icon
		//
		if (INFOSPOT(ob->clipRects.tileXmid, ob->clipRects.tileY2) == PLATFORMBLOCK)
		{
			if (ob->user1 == CLONE_JANITOR)
			{
				// janitor clones turn into real janitors:
				ob->currentAction = CK_ACTION(FF_ACT_councilwalk1);
				ob->type = FF_CT_CouncilMemberDone;
			}
			else
			{
				// other clones start floating upwards:
				ob->type = FF_CT_CloneRising;
				ob->velY = -16;
			} 
		}
	}
}

void FF_R_CloneObj(CK_object *ob)
{
	if (ob->user2 < 0)
	{
		FF_R_Draw(ob);
	}
	else
	{
#if (SCAN_DIR == IN_motion_Up)
		RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, ob->user2);
#elif (SCAN_DIR == IN_motion_Down)
		RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, -ob->user2);
#endif
	}
}


void FF_ToggleCloneMachine(int16_t tileX, int16_t tileY)
{
	CK_object *o;
	uint16_t position = tileX | (tileY << 8);

	for (o=ck_keenObj->next; o; o = o->next)
	{
		if (o->user4 == position
			&& (o->type == FF_CT_CloneMachineOn || o->type == FF_CT_CloneMachineOff))
		{
			o->type ^= (FF_CT_CloneMachineOn ^ FF_CT_CloneMachineOff);
			if (o->type == FF_CT_CloneMachineOn)
				ff_map.machinesActive++;
			else
				ff_map.machinesActive--;
			break;
		}
	}
}

/*
==============================================================================
	CORONA PLANT

	Basically a turret, except with its own sprite. Spits sticky balls.

	temp1 - frame offset (for spitting frame)
	temp2
	temp3
	temp4 - Nisasi pointer
------------------------------------------------------------------------------
	CORONA BALL

	A projectile that flies in a straight line and sticks to walls.

	temp1 - state (0 = flying, 1 = sticking to wall)
==============================================================================
*/


void FF_SpawnCoronaPlant(int16_t tileX, int16_t tileY, int16_t dir)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_CoronaPlant;
	new_object->clipped = CLIP_not;
	//new->zLayer = 0;	// redundant
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->xDirection = dir;
	new_object->yDirection = (dir == 0)? IN_motion_Up : IN_motion_None;
	new_object->velX = new_object->xDirection * 64;
	new_object->velY = new_object->yDirection * 64;
	new_object->gfxChunk = FF_CORONAU1SPR + (dir << 1);
	CK_SetAction(new_object, CK_ACTION(FF_ACT_CoronaPlant));
}

void FF_T_CoronaPlant(CK_object *ob)
{
	if (ob->user4)	// do nothing while grabbed by Nisasi
		return;

	ob->actionTimer += SD_GetSpriteSync();
	if (ob->user1)
	{
		//
		// plant's mouth is open 
		//
		if (ob->actionTimer >= 34)
		{
			ob->user1 = 0;	// close mouth
			ob->gfxChunk--;
			ob->actionTimer = 0;
		}
	}
	else
	{
		//
		// plant's mouth is closed
		//
		if (ob->actionTimer >= (ck_gameState.difficulty == D_Easy? 3*70 : 2*70))
		{
			// spawn the ball
			CK_object *new_object = CK_GetNewObj(true);
			new_object->type = FF_CT_CoronaBall;
			new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
			new_object->zLayer = 2;
			new_object->posX = ob->posX;
			new_object->posY = ob->posY;
			switch (ob->xDirection)
			{
			case IN_motion_Left:
				new_object->posX -= 1*16;
				new_object->posY += 4*16;
				break;
			case IN_motion_None:
				new_object->posX += 4*16;
				new_object->posY -= 4*16;
				break;
			case IN_motion_Right:
				new_object->posX += 6*16;
				new_object->posY += 4*16;
				break;
			}
			new_object->velX = ob->velX;
			new_object->velY = ob->velY;
			new_object->gfxChunk = FF_CORONAVIRUS1SPR;
			CK_SetAction2(new_object, CK_ACTION(FF_ACT_CoronaBall));
			SD_PlaySound(FF_BERKELOIDTHROWSND);

			// open the plant's mouth
			ob->user1++;	// open mouth
			ob->gfxChunk++;
			ob->actionTimer = 0;
		}
	}
}

void FF_T_CoronaBall(CK_object *ob)
{
	ob->actionTimer += SD_GetSpriteSync();

	if (ob->user1)
	{
		//
		// ball is sticking to a wall or ceiling
		//

		// remove after 1.4 / 1.0 seconds
		if (ob->actionTimer >= (ck_gameState.difficulty >= D_Hard? (int16_t)(1.4*70) : (int16_t)(1.0*70)))
		//if (ob->actionTimer >= (int)(1.0*TickBase))
			CK_RemoveObj(ob);
	}
	else
	{
		//
		// ball is flying
		//

	/*
		// animate the ball
		while (ob->actionTimer >= 6)
		{
			ob->actionTimer -= 6;
			if (++ob->gfxChunk > CORONAVIRUS3SPR)h
				ob->gfxChunk = CORONAVIRUS1SPR;
		}
	*/

		ck_nextX = ob->velX * SD_GetSpriteSync();
		ck_nextY = ob->velY * SD_GetSpriteSync();
	}
}

void FF_R_CoronaBall(CK_object *ob)
{
	if (ob->topTI || ob->leftTI || ob->rightTI || ob->bottomTI)
	{
		ob->user1 = 1;	// is now sticking to something
		ob->actionTimer = 0;	// reset value (this is now used for the vanish countdown)
	}
	FF_R_Draw(ob);
}


/*
==============================================================================
	NIPNAP

	Hanging from ceiling. Reaches down when Keen is near, then pauses.

	temp1 - internal state (see enum)
==============================================================================
*/

enum {NIPNAP_IDLE, NIPNAP_DOWN, NIPNAP_UP, NIPNAP_PAUSE};


void FF_SpawnNipnap(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Nipnap;
	new_object->clipped = CLIP_not;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->gfxChunk = FF_NIPNAPHIDESPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Nipnap));
}

void FF_T_Nipnap(CK_object *ob)
{
	ob->actionTimer += SD_GetSpriteSync();
	switch (ob->user1)
	{
	case NIPNAP_IDLE:
		if (!(ck_keenObj->posY - ob->posY <= 65*16
			&& ck_keenObj->clipRects.unitX1 < ob->clipRects.unitX2+24*SD_GetSpriteSync()		// Keen's walking speed is 24 units per tic
			&& ck_keenObj->clipRects.unitX2 > ob->clipRects.unitX1-24*SD_GetSpriteSync()))	// Keen's walking speed is 24 units per tic
		{
			break;
		}
		SD_PlaySound(FF_GRABBINGSND);
		ob->user1++;	// start going down
		ob->actionTimer = SD_GetSpriteSync();
		// no break here!
	case NIPNAP_DOWN:
		while (ob->actionTimer >= 4)	// need a while loop for constant timing here because MAXTICS is 5
		{
			ob->actionTimer -= 4;
			if (++ob->gfxChunk == FF_NIPNAPGRAB4SPR)
			{
				ob->user1++;	// start going up
				ob->actionTimer = 0;
				break;	// break the while-loop
			}
		}
		break;
	case NIPNAP_UP:
		if (ob->actionTimer >= 16)
		{
			ob->actionTimer -= 16;
			if (--ob->gfxChunk == FF_NIPNAPSLEEPSPR)
			{
				ob->user1++;	// start pause
				ob->actionTimer = 0;
			}
		}
		break;
	case NIPNAP_PAUSE:
		if (ob->actionTimer >= 1*70)
		{
			ob->gfxChunk++;	// open the eye again
			ob->actionTimer = 0;
			ob->user1 = NIPNAP_IDLE;
		}
		break;
	}
}

void FF_C_Nipnap(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ob->user1 == NIPNAP_IDLE)
		{
			// idle NipNap starts reaching down when touched by Keen
			// (next contact will stun Keen)
			ob->user1++;
			ob->gfxChunk++;
			ob->visible = true;
			ob->actionTimer = 0;
			SD_PlaySound(FF_GRABBINGSND);
		}
		else if (ob->user1 != NIPNAP_PAUSE)	// Keen can only get stunned when NipNap is not "sleeping"
		{
			FF_StunKeen(ob->clipRects.unitXmid);
		}
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		break;
	}
}



/*
==============================================================================
	CRAB

	Can walk on floors, walls and ceilings

	temp1 - sprite frame offset (added to ob->gfxChunk for drawing)
	temp2 - turn type
	temp3 - next state after turning
	temp4 - combined x/y tile position for climbing around outer edges
==============================================================================
*/


typedef enum {
	inner_FloorToLeftWall,
	inner_FloorToRightWall,
	inner_LeftWallToFloor,
	inner_RightWallToFloor,
	inner_CeilingToLeftWall,
	inner_CeilingToRightWall,
	inner_LeftWallToCeiling,
	inner_RightWallToCeiling,

	outer_start,

	outer_FloorToLeftWall=outer_start,
	outer_FloorToRightWall,
	outer_LeftWallToFloor,
	outer_RightWallToFloor,
	outer_CeilingToLeftWall,
	outer_CeilingToRightWall,
	outer_LeftWallToCeiling,
	outer_RightWallToCeiling,

	NUMTURNTYPES
} turntype;

// Note: The left and right walls refer to the left or right side of the tile
// the crab is currently climbing on. It does NOT mean that the wall is on the
// left or right side of the crab. If the crab is climbing the left side of a
// wall, then the wall will be on the right side of the crab (and vice versa).

void FF_SpawnCrab(int16_t tileX, int16_t tileY, bool onCeiling)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_Crab;
	new_object->clipped = FF_CLIP_custommid;
	new_object->actionTimer = US_RndT() & 6;
	new_object->user1 = US_RndT() % 3;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->xDirection = (US_RndT() < 0x80)? IN_motion_Left : IN_motion_Right;
	if (onCeiling)
	{
		new_object->posY = RF_TileToUnit(tileY) - CRAB_Y1;
		new_object->yDirection = IN_motion_Up;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_CrabCeiling));
	}
	else
	{
		new_object->posY = RF_TileToUnit(tileY) - CRAB_Y2 + 256-1;
		new_object->yDirection = IN_motion_Down;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_CrabFloor));
	}
}

void FF_R_CrabTurn(CK_object *ob)
{
	FF_DrawSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, (ob->user2 & 4) ? drawflipped : 0, ob->zLayer);
}

void FF_CrabStartTurn(CK_object *ob, uint16_t tx, uint16_t ty, turntype turn)
{
	//xtry = ytry = 0;	// redundant: we always call ChangeState(), which sets these to 0

	ob->user2 = turn;
	ob->clipped = CLIP_not;
	switch ((turn >> 1) & 3)
	{
	case 0:
	case 2:
		ob->user3 = 0;
		break;
	case 1:
		ob->user3 = 1;
		break;
	case 3:
		ob->user3 = 2;
	}
	if (turn >= outer_start)
	{
		// These tables define the Crab's position while it is turning, relative
		// to the Crab's position before it started turning. They have no effect
		// on the position of the Crab after it has turned, so they are purely
		// cosmetical. Feel free to adjust these as you please.
#define XOFF (CRAB_XMID/2) 
#define YOFF (CRAB_YMID/2) 
#if 1
		static int16_t xoff[8] = {-XOFF, XOFF,  XOFF, -XOFF, -XOFF,  XOFF, XOFF, -XOFF};
		static int16_t yoff[8] = { YOFF, YOFF, -YOFF, -YOFF, -YOFF, -YOFF, YOFF,  YOFF};
#else
		static int16_t xoff[8] = {-XOFF, XOFF,  XOFF, -XOFF, -XOFF,  XOFF, XOFF, -XOFF-4*16};
		static int16_t yoff[8] = { YOFF, YOFF, -YOFF, -YOFF, -YOFF, -YOFF-4*16, YOFF,  YOFF-4*16};
#endif
#undef XOFF
#undef YOFF
		
		ob->user4 = tx | (ty << 8);

		turn = (turntype)(turn & 7);
		ob->posX += xoff[turn];
		ob->posY += yoff[turn];

		ob->gfxChunk = ((turn & 3)^2) + FF_CRABCORNER1SPR;
	}
	else
	{
		ob->gfxChunk = (turn & 3) + FF_CRABCORNER1SPR;
	}
	CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabTurn));
	FF_R_CrabTurn(ob);
}

void FF_T_CrabFloor(CK_object *ob)
{
	//
	// update animation:
	//
	if (++ob->user1 >= 3)
		ob->user1 = 0;

	//
	// turn around when hitting a block icon:
	//
	if (ob->xDirection > 0)
	{
		if (INFOSPOT(RF_UnitToTile(ob->posX+CRAB_X2), RF_UnitToTile(ob->posY+CRAB_Y2)) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
	else
	{
		if (INFOSPOT(RF_UnitToTile(ob->posX+CRAB_X1), RF_UnitToTile(ob->posY+CRAB_Y2)) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
}

void FF_T_CrabTurn(CK_object *ob)
{
	 int16_t turn = ob->user2;
	
	if (turn >= outer_start)
	{
		// these tables now define the EXACT position of the Crab after turning,
		// in relation to the tile it climbed around (in global units):
		static int16_t xoff[8] =
		{
			// outer_FloorToLeftWall, outer_FloorToRightWall:
			-(CRAB_X2+1), 256-CRAB_X1,
			// outer_LeftWallToFloor, outer_RightWallToFloor:
			-CRAB_XMID+16, 256-CRAB_XMID-16,
			// outer_CeilingToLeftWall, outer_CeilingToRightWall:
			-(CRAB_X2+1), 256-CRAB_X1,
			// outer_LeftWallToCeiling, outer_RightWallToCeiling:
			-CRAB_XMID+16, 256-CRAB_XMID-16,
		};
		static int16_t yoff[8] =
		{
			// outer_FloorToLeftWall, outer_FloorToRightWall:
			-CRAB_YMID+16, -CRAB_YMID+16,
			// outer_LeftWallToFloor, outer_RightWallToFloor:
			-(CRAB_Y2+1), -(CRAB_Y2+1),
			// outer_CeilingToLeftWall, outer_CeilingToRightWall:
			256-CRAB_YMID-16, 256-CRAB_YMID-16,
			// outer_LeftWallToCeiling, outer_RightWallToCeiling:
			256-CRAB_Y1, 256-CRAB_Y1,
		};
		// You can adjust the offsets that use PIXGLOBAL (change PIXGLOBAL to
		// 2*PIXGLOBAL or 3*PIXGLOBAL etc.), but that may cause problems with
		// slope tiles near the corner the crab climbed around.

		turn = (turntype)(turn & 7);
#if 1
		// optimized version:
		ob->posX = RF_TileToUnit(ob->user4) + xoff[turn];	// don't need to mask off the bits of the y position, they get shifted out anyways
		ob->posY = (ob->user4 & 0xFF00) + yoff[turn];	// don't need to shift here, the bits of the y position are already in the right place
#else
		// safe, but slightly slower version:
		ob->posX = RF_TileToUnit(ob->user4 & 0xFF) + xoff[turn];
		ob->posY = RF_TileToUnit((ob->user4 >> 8) & 0xFF) + yoff[turn];
#endif
	}
	
	ob->clipped = FF_CLIP_custommid;
	CK_SetAction2(ob, ob->user3 == 0 ? CK_ACTION(FF_ACT_CrabWall) : ob->user3 == 1 ? CK_ACTION(FF_ACT_CrabFloor) : CK_ACTION(FF_ACT_CrabCeiling));
}

void FF_T_CrabWall(CK_object *ob)
{
	int16_t tx, ty;

	//
	// update animation:
	//
	if (++ob->user1 >= 3)
		ob->user1 = 0;

	//
	// turn around when hitting a block icon:
	//
	if (ob->yDirection > 0)
	{
		if (INFOSPOT(RF_UnitToTile(ob->posX+CRAB_XMID), RF_UnitToTile(ob->posY+CRAB_Y2)) == PLATFORMBLOCK)
		{
			ob->yDirection = -ob->yDirection;
			ck_nextY = -ck_nextY;
		}
	}
	else
	{
		if (INFOSPOT(RF_UnitToTile(ob->posX+CRAB_XMID), RF_UnitToTile(ob->posY+CRAB_Y1)) == PLATFORMBLOCK)
		{
			ob->yDirection = -ob->yDirection;
			ck_nextY = -ck_nextY;
		}
	}

	//
	// check walls
	// Note: this uses custom clipping, so we can't just use the hitbox values here
	//
	ty = RF_UnitToTile(ob->posY + ck_nextY + CRAB_YMID);

	if (ob->xDirection < 0)
	{
		tx = RF_UnitToTile(ob->posX+CRAB_X1 - 16);

		if (!TI_ForeRight(FORESPOT(tx, ty)))
		{
			if (ob->yDirection < 0 && TI_ForeTop(FORESPOT(tx, ty+1)) )
			{
				ob->yDirection = IN_motion_Down;
				FF_CrabStartTurn(ob, tx, ty+1, outer_RightWallToFloor);
				return;
			}
			else if (ob->yDirection > 0 && TI_ForeBottom(FORESPOT(tx, ty-1)) )
			{
				ob->yDirection = IN_motion_Up;
				FF_CrabStartTurn(ob, tx, ty-1, outer_RightWallToCeiling);
				return;
			}
			else
			{
				ck_nextY = -ck_nextY;
				ob->yDirection = -ob->yDirection;
			}
		}
	}
	else
	{
		tx = RF_UnitToTile(ob->posX+CRAB_X2 + 16);

		if (!TI_ForeLeft(FORESPOT(tx, ty)))
		{
			if (ob->yDirection < 0 && TI_ForeTop(FORESPOT(tx, ty+1)) )
			{
				ob->yDirection = IN_motion_Down;
				FF_CrabStartTurn(ob, tx, ty+1, outer_LeftWallToFloor);
				return;
			}
			else if (ob->yDirection > 0 && TI_ForeBottom(FORESPOT(tx, ty-1)) )
			{
				ob->yDirection = IN_motion_Up;
				FF_CrabStartTurn(ob, tx, ty-1, outer_LeftWallToCeiling);
				return;
			}
			else
			{
				ck_nextY = -ck_nextY;
				ob->yDirection = -ob->yDirection;
			}
		}
	}
}

void FF_T_CrabCeiling(CK_object *ob)
{
	//
	// update animation:
	//
	if (++ob->user1 >= 3)
		ob->user1 = 0;

	//
	// turn around when hitting a block icon:
	//
	if (ob->xDirection > 0)
	{
		if (INFOSPOT(RF_UnitToTile(ob->posX+CRAB_X2), RF_UnitToTile(ob->posY+CRAB_Y1)) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
	else
	{
		if (INFOSPOT(RF_UnitToTile(ob->posX+CRAB_X1), RF_UnitToTile(ob->posY+CRAB_Y1)) == PLATFORMBLOCK)
		{
			ob->xDirection = -ob->xDirection;
			ck_nextX = -ck_nextX;
		}
	}
	
	// K1n9_Duk3 note: We had considered making the Crab fall down when it is
	// directly above Keen, but I thought it would have led to unfair deaths.
}

void  FF_CrabDraw(CK_object *ob)
{
	RF_AddSpriteDraw(&ob->sde, ob->posX, ob->posY, ob->gfxChunk+ob->user1, false, ob->zLayer);
}

void  FF_CrabDrawFlipped(CK_object *ob)
{
	FF_DrawSprite(&ob->sde, ob->posX, ob->posY, ob->gfxChunk+ob->user1, drawflipped, ob->zLayer);
}

void FF_R_CrabFloor(CK_object *ob)
{
	if (ob->xDirection > 0 && ob->leftTI)
	{
		ob->yDirection = IN_motion_Up;
		FF_CrabStartTurn(ob, 0, 0, inner_FloorToLeftWall);
		return;
	}
	else if (ob->xDirection < 0 && ob->rightTI)
	{
		ob->yDirection = IN_motion_Up;
		FF_CrabStartTurn(ob, 0, 0, inner_FloorToRightWall);
		return;
	}
	else if (!ob->topTI)
	{
		int16_t tx = RF_UnitToTile(ob->posX - ob->deltaPosX + CRAB_XMID);
		int16_t ty = RF_UnitToTile(ob->posY + CRAB_Y2 + 16);
		
		if (ob->xDirection > 0)
		{
			if (TI_ForeRight(FORESPOT(tx, ty)))
			{
				ob->xDirection = IN_motion_Left;
				FF_CrabStartTurn(ob, tx, ty, outer_FloorToRightWall);
				return;
			}
		}
		else
		{
			if (TI_ForeLeft(FORESPOT(tx, ty)))
			{
				ob->xDirection = IN_motion_Right;
				FF_CrabStartTurn(ob, tx, ty, outer_FloorToLeftWall);
				return;
			}
		}

		//
		// cannot climb around corner, so just turn around:
		//
		FF_TurnX(ob);
		// start falling if still not touching a tile
		if (!ob->topTI)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabFall));
		}
	}
	FF_CrabDraw(ob);
}

void FF_R_CrabWall(CK_object *ob)
{
	if (ob->yDirection < 0 && ob->bottomTI)
	{
		ob->xDirection = -ob->xDirection;
		if (ob->xDirection < 0)
			FF_CrabStartTurn(ob, 0, 0, inner_LeftWallToCeiling);
		else
			FF_CrabStartTurn(ob, 0, 0, inner_RightWallToCeiling);
		return;
	}
	else if (ob->yDirection > 0 && ob->topTI)
	{
		ob->xDirection = -ob->xDirection;
		if (ob->xDirection < 0)
			FF_CrabStartTurn(ob, 0, 0, inner_LeftWallToFloor);
		else
			FF_CrabStartTurn(ob, 0, 0, inner_RightWallToFloor);
		return;
	}
	else if (ob->xDirection < 0 && !ob->rightTI)
	{
		//
		// outer edges are handled by the think function, so just turn around here:
		//
		FF_TurnY(ob);
		// start falling if still not touching a tile
		if (!ob->rightTI)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabFall));
			FF_R_Draw(ob);
			return;
		}
	}
	else if (ob->xDirection > 0 && !ob->leftTI)
	{
		//
		// outer edges are handled by the think function, so just turn around here:
		//
		FF_TurnY(ob);
		// start falling if still not touching a tile
		if (!ob->leftTI)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabFall));
			FF_R_Draw(ob);
			return;
		}
	}

	if (ob->yDirection < 0)
		FF_CrabDraw(ob);
	else
		FF_CrabDrawFlipped(ob);
}

void FF_R_CrabCeiling(CK_object *ob)
{
	if (ob->xDirection > 0 && ob->leftTI)
	{
		ob->yDirection = IN_motion_Down;
		FF_CrabStartTurn(ob, 0, 0, inner_CeilingToLeftWall);
		return;
	}
	else if (ob->xDirection < 0 && ob->rightTI)
	{
		ob->yDirection = IN_motion_Down;
		FF_CrabStartTurn(ob, 0, 0, inner_CeilingToRightWall);
		return;
	}
	else if (!ob->bottomTI)
	{
		int16_t tx = RF_UnitToTile(ob->posX - ob->deltaPosX + CRAB_XMID);
		int16_t ty = RF_UnitToTile(ob->posY + CRAB_Y1 - 16);

		if (ob->xDirection > 0)
		{
			if (TI_ForeRight(FORESPOT(tx, ty)))
			{
				ob->xDirection = IN_motion_Left;
				FF_CrabStartTurn(ob, tx, ty, outer_CeilingToRightWall);
				return;
			}
		}
		else
		{
			if (TI_ForeLeft(FORESPOT(tx, ty)))
			{
				ob->xDirection = IN_motion_Right;
				FF_CrabStartTurn(ob, tx, ty, outer_CeilingToLeftWall);
				return;
			}
		}

		//
		// cannot climb around corner, so just turn around:
		//
		FF_TurnX(ob);
		// start falling if still not touching a tile
		if (!ob->bottomTI)
		{
			CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabFall));
		}
	}
	FF_CrabDrawFlipped(ob);
}

void FF_CrabFallReact(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		ob->yDirection = IN_motion_Down;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabFloor));
	}
	FF_CrabDraw(ob);
}

void FF_C_Crab(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		FF_KillKeen();
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		ob->clipped = FF_CLIP_custommid;
		if (ob->currentAction == CK_ACTION(FF_ACT_CrabWall))
			ob->xDirection = -ob->xDirection;	// walk away from the wall after landing
		CK_SetAction2(ob, CK_ACTION(FF_ACT_CrabFall));
	}
}



/*
==============================================================================
	BRAIN FUCL

	Grabs onto Keen on touch and triggers the bad ending.

	temp1 - state
	temp2 - animation counter
	temp3 - 
	temp4 - 
==============================================================================
*/


void FF_SpawnBrainFucl(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_BrainFucl;
	new_object->clipped = CLIP_not;
	new_object->zLayer = 2;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->gfxChunk = FF_BRAINFUCL1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_BrainFucl));
	ff_map.brainfucl = -1;	// level has BrainFucl, but it's not yet active
}

void FF_T_BrainFucl(CK_object *ob)
{
	if (ob->user1)
	{
		 int16_t maxspeed = SD_GetSpriteSync()*2;

		ck_nextX = ck_keenObj->posX + 5*16 - ob->posX;

		if (ck_nextX < -maxspeed)
			ck_nextX = -maxspeed;
		else if (ck_nextX > maxspeed)
			ck_nextX = maxspeed;
	}

	switch (ob->user1)
	{
	case 1:
		if (ob->posY < ck_keenObj->posY - 44*16)
		{
			ck_nextY = SD_GetSpriteSync()*(16/2);
		}
		else
		{
			ob->user1++;
			ck_keenObj->gfxChunk = FF_KEENLOOKUPSPR;
			ck_keenObj->visible = true;
		}
		break;
	case 2:
		if (ob->posY < ck_keenObj->posY - 24*16)
		{
			ck_nextY = SD_GetSpriteSync()*(16/3);
		}
		else
		{
			ob->user1++;
			ob->gfxChunk++;
			ck_keenObj->gfxChunk = FF_KEENSTANDLSPR;
			ck_keenObj->visible = true;
		}
		break;
	case 3:
		if (ob->posY < ck_keenObj->posY - 17*16)
		{
			ck_nextY = SD_GetSpriteSync()*(16/4);
		}
		else
		{
			ob->user1++;
			ob->gfxChunk++;
		}
		break;
	case 4:
		ob->actionTimer += SD_GetSpriteSync();
		SD_SetMusicRate(350);

		if (ob->actionTimer >= 8)
		{
			ob->actionTimer -= 9;
			ob->gfxChunk ^= FF_BRAINFUCL2SPR ^ FF_BRAINFUCL3SPR;
			ck_gameState.keenScore >>= 1;
			ob->user2++;
			if (ob->user2 == 6)
			{
				ck_keenObj->gfxChunk = FF_VOODOOKEENSPR;
				ck_keenObj->visible = true;
			}
			else if (ob->user2 == 42)
			{
				ck_gameState.levelState = ex_brainfucled;
			}
		}
	}
}

void FF_C_BrainFucl(CK_object *ob, CK_object *hit)
{
	if (hit->type != CT_Player)
		return;

	if (ff_map.brainfucl < 0)
	{
		ff_map.brainfucl = 1;	// brainfucl is now active and blocks player input
	ff_brainfucled++;
		if (hit->gfxChunk >= FF_KEENPOGORSPR && hit->gfxChunk <= FF_KEENPOGOBOUNCELSPR)
		{
			// make Keen stop pogoing
			CK_SetAction2(hit, CK_ACTION(CK_ACT_keenFall1));
		}
	}
	else if (!ob->user1 && hit->currentAction->stickToGround == 1 && hit->posX + 4*16 <= ob->posX)
	{
		// Keen is on ground and has moved past the BrainFucl thing
		// so activate the BrainFucl thing:
		ob->user1++;
		// and make Keen stop walking
		hit->gfxChunk = FF_KEENSTANDLSPR;
		CK_SetAction2(hit, CK_ACTION(FF_ACT_KeenBrainFucled));
	}
}


/*
==============================================================================
	FLYING DONUT

	Bonus item that flies around a bit.

	temp1 - item number (for bonus handling)
	temp2 - initial x position
	temp3 - initial y position
	temp4 - 
==============================================================================
*/

#define DONUT_ANIM_DELAY 6


void FF_SpawnDonut(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->clipped = CLIP_not;
	new_object->type = CT5_Item;
	new_object->user1 = 12;	// 10000 points
	new_object->user2=new_object->posX = RF_TileToUnit(tileX);
	new_object->user3=new_object->posY = RF_TileToUnit(tileY);
	new_object->xDirection = US_RndT() < 0x80 ? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = US_RndT() < 0x80 ? IN_motion_Up : IN_motion_Down;
	new_object->gfxChunk = FF_FLYDONUT1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Donut));
	ff_map.itemsTotal++;
}

void FF_T_Donut(CK_object *ob)
{
	ob->actionTimer += SD_GetSpriteSync();
	if (ob->actionTimer >= 2*DONUT_ANIM_DELAY)
	{
		ob->actionTimer -= 2*DONUT_ANIM_DELAY;

		if (US_RndT() < 4)
			ob->xDirection = -ob->xDirection;
		if (US_RndT() < 4)
			ob->yDirection = -ob->yDirection;
	}

	ob->gfxChunk = ob->actionTimer < DONUT_ANIM_DELAY ? FF_FLYDONUT1SPR : FF_FLYDONUT2SPR; 

	if ((int16_t)(ob->user2 - ob->posX) > 74*16)
		ob->xDirection = IN_motion_Right;
	if ((int16_t)(ob->posX - ob->user2) > 74*16)
		ob->xDirection = IN_motion_Left;

	if ((int16_t)(ob->user3 - ob->posY) > 24*16)
		ob->yDirection = IN_motion_Down;
	if ((int16_t)(ob->posY - ob->user3) > 24*16)
		ob->yDirection = IN_motion_Up;

	FF_AccelerateX(ob, ob->xDirection, 9);
	FF_AccelerateY(ob, ob->yDirection, 8);
}


/*
==============================================================================
	BALLOON SMIKRY

	Floats up and disappears as soon as it's off-camera. Just eye candy.

	temp1 -
	temp2 -
	temp3 -
	temp4 - 
==============================================================================
*/


void FF_SpawnBalloonSmirky(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->clipped = CLIP_not;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->gfxChunk = FF_SMIRKYBALLOON1SPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_BalloonSmirky));
}

void FF_T_BalloonSmirky(CK_object *ob)
{
	FF_AccelerateY(ob, IN_motion_Up, 12);

	if (!CK_ObjectVisible(ob))
		CK_RemoveObj(ob);

	if ((ob->actionTimer += SD_GetSpriteSync()) >= 20)
	{
		ob->actionTimer -= 44;
		ob->gfxChunk ^= FF_SMIRKYBALLOON1SPR ^ FF_SMIRKYBALLOON2SPR;
	}
}


/*
==============================================================================
	FUNGUSAMONGUS

	Kind of a combination of Bluebird (idle until touched, like the egg,
	stunnable like the bird) and Berkeloid (chase behavior).

	temp1 - internal state (see enum)
	temp2 - cycle counter for wakeup animation
	temp3 -
	temp4 - 
==============================================================================
*/


#define FU_WALK_SPEED (ck_gameState.difficulty >= D_Hard? 25 : 24)
#define FU_ANIM_DELAY 8
#define FU_STUN_DELAY (ck_gameState.difficulty >= D_Hard? 2*70 : 4*70)
#define FU_WAKEUP_DELAY 16
#define FU_WAKEUP_CYCLES 4

enum {FU_IDLE, FU_WALK, FU_STUNNED, FU_WAKEUP};

void FF_SpawnFungusAmongus(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX-1);
	new_object->posY = RF_TileToUnit(tileY) - 25*16;
	new_object->gfxChunk = FF_FUNGUSAMONGUSSTUNNEDSPR;
	new_object->xDirection = (US_RndT() < 0x90)? IN_motion_Left : IN_motion_Right;
	new_object->yDirection = IN_motion_Down;
	//new->user1 = FU_IDLE;	// redundant
	new_object->type = FF_CT_Fungus;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Fungus));
}

void FF_T_Fungus(CK_object *ob)
{
	switch (ob->user1)
	{
	case FU_WALK:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= FU_ANIM_DELAY)
		{
			ob->actionTimer -= FU_ANIM_DELAY;
			if (++ob->gfxChunk > FF_FUNGUSAMONGUS5SPR)
				ob->gfxChunk = FF_FUNGUSAMONGUS1SPR;

			// berkeloid-like behavior: 1/8 chance of turning to chase Keen
			if (US_RndT() <= 44)
			{
				ob->xDirection = ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid? IN_motion_Left : IN_motion_Right;
			}
		}
		ck_nextX = ob->xDirection * SD_GetSpriteSync() * FU_WALK_SPEED;
		break;

	case FU_STUNNED:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= FU_STUN_DELAY)
		{
			ob->user2 = ob->actionTimer = 0;
			SD_PlaySound(FF_FUNGUSSND);
			ob->user1 = FU_WAKEUP;
		}
		break;

	case FU_WAKEUP:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= FU_WAKEUP_DELAY)
		{
			ob->actionTimer -= FU_WAKEUP_DELAY;

			if (++ob->user2 >= FU_WAKEUP_CYCLES)
			{
				ob->user2 = 0;
				ob->user1 = FU_WALK;
				ob->gfxChunk = FF_FUNGUSAMONGUS1SPR;
			}
			else
			{
				ob->gfxChunk = (ob->user2 & 1)? FF_FUNGUSAMONGUS1SPR : FF_FUNGUSAMONGUSSTUNNEDSPR;
			}
		}
		break;
	}
}

void FF_C_Fungus(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		switch (ob->user1)
		{
		case FU_WALK:
			FF_KillKeen();
			break;

		case FU_IDLE:
			ob->user1 = FU_WAKEUP;
			SD_PlaySound(FF_FUNGUSSND);
			break;
		}
		break;

	case CT_Stunner:
		switch (ob->user1)
		{
		case FU_WALK:
		case FU_WAKEUP:
			FF_ExplodeShot(hit);
			if (ob->gfxChunk != FF_FUNGUSAMONGUSSTUNNEDSPR)
			{
				ob->posY -= 9*16;
				ob->gfxChunk = FF_FUNGUSAMONGUSSTUNNEDSPR;
			}
			ob->velY = -32;
			ob->user1 = FU_STUNNED;
			ob->actionTimer = 0;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_FungusFall));
			break;
		}
		break;
	}
}

void FF_R_Fungus(CK_object *ob)
{
	if (!ob->topTI || (ob->xDirection > 0 && ob->leftTI) || (ob->xDirection < 0 && ob->rightTI))
	{
		ob->xDirection = -ob->xDirection;
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		CK_SetAction2(ob, ob->currentAction);
		if (!ob->topTI)
			CK_SetAction2(ob, CK_ACTION(FF_ACT_FungusFall));
	}
	FF_R_Draw(ob);
}

void FF_R_FungusFall(CK_object *ob)
{
	if (ob->topTI)
	{
		ob->velY = 0;
		CK_SetAction2(ob, CK_ACTION(FF_ACT_Fungus));
	}
	FF_R_Draw(ob);
}


/*
==============================================================================
	WORLDMAP NESSIE

	Sticks its head up when Keen is near, then vanishes forever.

	temp1 - internal state (0 = waiting, 1 = rising, 2 = looking, 3 = lowering)
	temp2 - how far it has risen, in pixels
	temp3 -
	temp4 - 
==============================================================================
*/

#define WORLDNESSIE_SPRITEHEIGHT 20
#define WORDLNESSIE_DELAY 1


void FF_SpawnWorldNessie(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->clipped = CLIP_not;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->type = FF_CT_WorldNessie;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_WorldNessie));
}

void FF_T_WorldNessie(CK_object *ob)
{
	int16_t xdist, ydist;

	switch (ob->user1)
	{
	case 0:
		xdist = ob->clipRects.tileXmid - ck_keenObj->clipRects.tileXmid;
		ydist = ob->clipRects.tileY1 - ck_keenObj->clipRects.tileY1;
		if (xdist >= -3 && xdist <= 3 && ydist >= -3 && ydist <= 3)
		{
			// start rising:
			ob->user1++;
			SD_PlaySound(FF_STATUSUPSND);
			break;	// for testing
		/*
			// remove all other worldmap nessies from the level:
			for (ob = player; ob; ob = ob->next)
			{
			 	if (ob->type == CT_WorldNessie && ob->user1 == 0)
			 		RemoveObj(ob);
			}
		*/
		}
		break;
		
	case 1:
		ob->actionTimer += SD_GetSpriteSync();
		while (ob->actionTimer >= WORDLNESSIE_DELAY)	// need the loop for low frame rates!
		{
			ob->actionTimer -= WORDLNESSIE_DELAY;
			ob->visible = true;
			if (++ob->user2 >= WORLDNESSIE_SPRITEHEIGHT)
			{
				ob->user1++;
				break;	// transitioned into the next internal state
			}
		}
		break;
		
	case 2:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 100)
		{
			ob->actionTimer = 0;
			ob->user1++;
			SD_PlaySound(FF_STATUSDOWNSND);
		}
		break;
		
	case 3:
		ob->actionTimer += SD_GetSpriteSync();
		while (ob->actionTimer >= WORDLNESSIE_DELAY)	// need the loop for low frame rates!
		{
			ob->actionTimer -= WORDLNESSIE_DELAY;
			ob->visible = true;
			if (--ob->user2 <= 0)
			{
				ob->user1++;
				break;	// transitioned into the next internal state
			}
		}
		break;
		
	default:
		CK_RemoveObj(ob);
		//	for testing only:  
		//ob->actionTimer = ob->user1 = ob->user2 = 0;
	}
}

void FF_R_WorldNessie(CK_object *ob)
{
	if (ob->user2)
	{
		// draw with clipping
		int16_t skip = WORLDNESSIE_SPRITEHEIGHT - ob->user2;
		RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY+RF_PixelToUnit(skip), ob->gfxChunk, false, ob->zLayer, -skip);
		//R_Draw(ob);
	}
	else
	{
		RF_RemoveSpriteDraw(&ob->sde);
	}
}


#undef WORLDNESSIE_SPRITEHEIGHT


/*
==============================================================================
	FROG / FROGGO

	Randomly jumps up. Just eye candy.

	temp1 - jump countdown
	temp2 - 
	temp3 -
	temp4 - 
==============================================================================
*/

#define FROG_MINJUMPDELAY (3*70)
#define FROG_MAXJUMPDELAY (18*70)


void FF_SpawnFrog(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Frog));
}

void FF_T_Frog(CK_object *ob)
{
	if (ob->user1)
	{
		if ((uint16_t)ob->user1 > SD_GetSpriteSync())
		{
			ob->user1 -= SD_GetSpriteSync();
		}
		else
		{
			// start jumping:
			ob->user1 = 0;
			ob->velY = -48;
		}
	}
	else
	{
		FF_T_WeakProjectile(ob);
	}
}

void FF_R_Frog(CK_object *ob)
{
	if (ob->bottomTI)	// hit ceiling?
		ob->velY = 0;	// start falling

	if (ob->topTI)		// hit floor?
		ob->user1 = US_RndT() * ((FROG_MAXJUMPDELAY-FROG_MINJUMPDELAY)/255) + FROG_MINJUMPDELAY;

	FF_R_Draw(ob);
}



/*
==============================================================================
	COUNCIL PAGE II

	Stands or walks around. Triggers a dreaming interaction when touched by
	Keen.

	temp1 - internal state
	temp2 - touch cooldown
	temp3 - dream number
	temp4 - 
==============================================================================
*/


enum {PAGE_AMBUSH, PAGE_WALK, PAGE_STAND, PAGE_DREAM};

void FF_PageWalk(CK_object *ob)
{
	uint16_t tx;

	// update touch cooldown:
	if ((ob->user2 -= SD_GetSpriteSync()) < 0)
		ob->user2 = 0;

	switch (ob->user1)
	{
	case PAGE_WALK:
		// start standing still when countdown ends:
		if ((ob->actionTimer -= SD_GetSpriteSync()) < 0)
		{
			ob->user1 = PAGE_STAND;	// start standing still
			ob->actionTimer = 100;
			break;
		}
		ck_nextX = ob->xDirection * SD_GetSpriteSync() * (PAGE_WALKSPEED/2);
		ob->gfxChunk = FF_PAGEWALKLSPR+1 + ob->xDirection;

		// turn around when about to hit a block icon:
		tx = RF_UnitToTile(ob->clipRects.unitXmid + ck_nextX);
		if (tx != ob->clipRects.tileXmid)
		{
			if (INFOSPOT(tx, ob->clipRects.tileY2) == PLATFORMBLOCK)
			{
				ob->user1 = PAGE_STAND;
				ck_nextX = 0;
				ob->gfxChunk = FF_PAGESLEEPLSPR+1 + ob->xDirection;
				ob->timeUntillThink = ob->actionTimer = 50;	// wait 50 ticks before turning, stand for another 50
				ob->xDirection = -ob->xDirection;
			}
		}
		break;

	case PAGE_STAND:
		ob->gfxChunk = FF_PAGESLEEPLSPR+1 + ob->xDirection;
		if ((ob->actionTimer -= SD_GetSpriteSync()) < 0)
		{
			ob->actionTimer = US_RndT()*4 + 100;	// pick a random delay for standing still
			ob->user1 = PAGE_WALK;	// start walking again
		}
		break;

	case PAGE_DREAM:
		ob->actionTimer += SD_GetSpriteSync();
		ob->gfxChunk = ((ob->actionTimer >> 4) & 1) + FF_PAGEDREAM1SPR;
		if (ob->actionTimer >= 30 && ob->actionTimer - SD_GetSpriteSync() < 30)
		{
			FF_RemoveDreamBubbles();	// just in case Keen is dreaming or something...
			FF_SpawnDreamBubble(ob->posX+4*16, ob->posY-30*16, 0x8001 + ob->user3);
			ob->user3 ^= 1;	// show other dream bubble next time
		}
		if (ob->actionTimer > 5*70 || ck_keenObj->currentAction != CK_ACTION(FF_ACT_KeenLook))	// Remember: dream bubble lasts over 5 seconds
		{
			ob->active = OBJ_ACTIVE;	// no longer always active
			ob->user2 = 3*70;	// start touch cooldown (3 seconds)
			ob->user1 = PAGE_STAND;	// start standing
			ob->actionTimer = 100;
			FF_RemoveDreamBubbles();	// in case the dream bubble is still active

			if (ck_keenObj->currentAction == CK_ACTION(FF_ACT_KeenLook))
				CK_SetAction2(ck_keenObj, CK_ACTION(FF_ACT_KeenShrug));
		}
		break;
	}
}

void FF_PageContact(CK_object *ob, CK_object *hit)
{
	if (hit->type == CT_Player)
	{
		if (ob->user1 == PAGE_AMBUSH)	// page is waiting for first contact
		{
			ob->user1 = PAGE_STAND;	// change to standing
			ob->actionTimer = 0.5*70;	// start walking after 0.5 seconds
			ob->user2 = 1.5*70;	// page starts interacting with Keen 1 second after he starts walking
		}
		else if (ob->user2 == 0 && ob->user1 != PAGE_DREAM && hit->topTI
			&& (hit->currentAction->think == FF_KeenPauseThink
			 || hit->currentAction->think == FF_KeenWalkThink))
		{
			ob->active = OBJ_ALWAYS_ACTIVE;	// just to make sure the dream animation ends
			ob->user1 = PAGE_DREAM;	// start the animation
			ob->actionTimer = 0;

			// change Keen into the "cutscene" state:
			hit->gfxChunk = FF_KEENANGRY1SPR;
			CK_SetAction2(hit, CK_ACTION(FF_ACT_KeenLook));
		}
	}
}

void FF_PageWalkReact(CK_object *ob)
{
	uint16_t y;

	y = ob->posY;

	if ((ob->xDirection > 0 && ob->leftTI) || (ob->xDirection < 0 && ob->rightTI) || !ob->topTI)
	{
		ob->posX -= ob->deltaPosX;
		ob->posY -= ob->deltaPosY;
		ob->gfxChunk = FF_PAGESLEEPLSPR+1 + ob->xDirection;
		ob->xDirection = -ob->xDirection;
		ob->timeUntillThink = 10 + US_RndT() / 16;
	}

	switch (ob->gfxChunk)
	{
	case FF_PAGEWALKLSPR:
	case FF_PAGEWALKRSPR:
		y -= (((ob->posX-1) << 1) & (16 << 3)) >> 3;	// adds up & down movement while walking
	}
	RF_AddSpriteDraw(&ob->sde, ob->posX, y, ob->gfxChunk, false, ob->zLayer);
}

void FF_KeenLook(CK_object *ob)
{
	ob->actionTimer += SD_GetSpriteSync();
	if (ob->actionTimer > 100)
		ob->actionTimer -= 100;
	ob->gfxChunk = ob->actionTimer > 90? FF_KEENANGRY1SPR : FF_KEENANGRY2SPR;
}


/*
==============================================================================
	QUICKSAND PLATFORM

	Allows Keen to stand and walk inside the quicksand tiles. Slowly moves down
	to make Keen sink into the quicksand.

	temp1 - 
	temp2 - 
	temp3 - 
	temp4 - 
==============================================================================
*/


void FF_SpawnQuicksand(uint16_t x, uint16_t y)
{
	CK_object *new_object = CK_GetNewObj(true);
	if (FF_ObjToUser(new_object) == FF_ObjToUser(NULL) /* no free object */)
		return;

	ff_map.quicksand = new_object;
	new_object->type = CT5_Platform;
	new_object->clipped = CLIP_not;
	new_object->clipRects.unitX2 = (new_object->posX = new_object->clipRects.unitX1 = x) + 256;
	new_object->clipRects.unitY2 = (new_object->posY = new_object->clipRects.unitY1 = y) + 256;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Quicksand));

	new_object->deltaPosY = -256;	// allow Keen to land
	CK_PhysPushY(ck_keenObj, new_object);
	new_object->deltaPosY = 0;
}

void FF_T_Quicksand(CK_object *ob)
{
	ck_nextY = SD_GetSpriteSync()*2;	// platform moves down and therefore allows Keen to sink deeper into the quicksand
}

// Note: The sprites are only used for their hitbox, they are never drawn!


/*
==============================================================================
	FALLING BLOCK

	description.

	temp1 - internal state and falling counter (0 = waiting, 1..4 = falling)
	temp2 - 
	temp3 - foreground sprite pointer
	temp4 - 
==============================================================================
*/


void FF_SpawnFallBlock(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(true);

	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->type = FF_CT_FallBlock;
	new_object->clipped = FF_CLIP_custommid;
	new_object->posX = RF_TileToUnit(tileX) - 8*16;
	new_object->posY = RF_TileToUnit(tileY);
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Fallblock));
}

void FF_T_FallBlock(CK_object *ob)
{
	if (ob->user1 == 0)
	{
		// start falling when Keen is below the block:
		if (ck_keenObj->clipRects.unitXmid >= ob->clipRects.unitX1
			&& ck_keenObj->clipRects.unitXmid <= ob->clipRects.unitX2
			&& ck_keenObj->clipRects.unitY1 > ob->clipRects.unitY2)
		{
			// abort if any blocking tiles are in the way:
			 int16_t y;
			 uint16_t mapoff = ((ob->clipRects.tileY2) * CA_GetMapWidth()) + ob->clipRects.tileXmid;
			for (y = ob->clipRects.tileY2; y <= ck_keenObj->clipRects.tileY1; y++, mapoff+=CA_GetMapWidth())
			{
				if (TI_ForeTop(FF_MapAt(1, mapoff)))
					return;
			}

			// start falling:
			ob->user1++;
			ff_map.shaketics = 70/4;	// shake the screen
			SD_PlaySound(FF_YETICLOUDSND);	
		}
	}
	else
	{
		if (ob->user1 < 4)
			ob->user1++;
		FF_T_Projectile(ob);
	}
}

void FF_C_FallBlock(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		if (ob->user1 == 0 && hit->velY > 30)
		{
			ob->user1++;	// start falling
			ff_map.shaketics = 70/4;	// shake the screen
			SD_PlaySound(FF_YETICLOUDSND);	
		}
		break;
	case CT_Stunner:
		FF_ExplodeShot(hit);
		break;
	}
}

void FF_R_FallBlock(CK_object *ob)
{
	if (ob->topTI)
	{
		if (ob->user1 >= 4)
		{
			ff_map.shaketics = ob->velY;	// shake the screen
			SD_PlaySound(FF_KITTENEXPLODESND);	
		}
		ob->user1 = ob->velY = 1;
	}
	FF_R_Draw(ob);
	RF_AddSpriteDrawUsing16BitOffset(&ob->user3, ob->posX, ob->posY, FF_SANDSTONEFORESPR, false, FOREGROUNDPRIORITY-1);
}



/*
==============================================================================
	OBELISK

	Stationary obstacle that blocks Keen and Keen's shots. Sinks into the ground
	when shot.

	temp1 - clipping value (must be negative)
	temp2 - shake & sink counter
	temp3 - 
	temp4 - 
==============================================================================
*/


#define OBELISK_SINKDELAY	8	// takes this many tics to sink by one pixel
#define OBELISK_MAXSINK		80	// how many pixels the Obelisk can sink in total
#define OBELISK_PERSHOT		8	// sink this many pixels after each shot

void FF_SpawnObelisk(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->clipped = CLIP_not;
	new_object->type = FF_CT_Obelisk;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Obelisk));
}

void FF_T_Obelisk(CK_object *ob)
{
	if (ob->user2)
	{
		if ((ob->actionTimer += SD_GetSpriteSync()) > OBELISK_SINKDELAY)
		{
			SD_PlaySound(FF_YETICLOUDSND);	
			ob->actionTimer -= OBELISK_SINKDELAY;
			ob->user2--;
			if (ob->user1 > -OBELISK_MAXSINK)	// don't sink more than this
			{
				// sink by one pixel:
				ob->user1--;	// adjust clipping (negative value -> cut off pixels at the bottom)
				ob->posY += 16;
				ob->clipRects.unitY1 += 16;	// move the bottom part of the hitbox down
				
			}
			else
			{
				// cannot sink any more:
				ob->user2 = 0;	// stop shaking
				ob->posX |= 16;
			}
		}
		ob->posX ^= 16;	// make it shake
		ob->visible = true;	// force redraw
	}
}

void FF_C_Obelisk(CK_object *ob, CK_object *hit)
{
	switch (hit->type)
	{
	case CT_Player:
		// quick hack to make sure Keen always gets pushed out of the Obelisk's hitbox:
		ob->deltaPosX = hit->clipRects.unitXmid < ob->clipRects.unitXmid? -3*256 : 3*256;
		FF_PushKeenX(hit, ob);
		break;

	case CT_Stunner:
		// make it shake and sink:
		ob->user2 = OBELISK_PERSHOT;
		FF_ExplodeShot(hit);
		break;
	}
}

void FF_R_Obelisk(CK_object *ob)
{
	RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY, ob->gfxChunk, false, ob->zLayer, ob->user1);
}


/*
==============================================================================
	MUMMY

	Type (either CT_Mummy1 or CT_Mummy2) defines teleport destination for Keen.

	temp1 - internal state (mummy) / visibility value (spawner)
	temp2 - height clipping (in pixels)
	temp3 - look counter
	temp4 - 


	KEEN MUMMY

	Stores mummy's type as its own type (for teleport destination)

	temp1 - internal state
	temp2 - old originxglobal \_  for detecting when the screen stops
	temp3 - old originyglobal /   scrolling to teleport destination
	temp4 - 
==============================================================================
*/


#define MUMMY_HEIGHT	36		// height of the mummy sprite(s) in pixels
#define MUMMY_RISEDELAY 5
#define MUMMY_SINKDELAY 5
#define MUMMY_OFFSCREENDELAY	(5*70)	// hide mummy if not visible during the last 5 seconds
#define MUMMY_RESPAWNDELAY	(ck_gameState.difficulty >= D_Hard? 2*70 : 4*70)	// hidden mummy waits 4 seconds before allowing a respawn
#define BANDAGE_ANIMDELAY	10

enum {MUMMY_STAND, MUMMY_RISE, MUMMY_SINK, MUMMY_HIDDEN, MUMMY_RUN, MUMMY_SPIN, MUMMY_SPINDONE, MUMMY_LOOK, MUMMY_AFTERLOOK};
enum {KMUMMY_FALL, KMUMMY_PREWRAP, KMUMMY_WRAP, KMUMMY_POSTWRAP, KMUMMY_VANISH, KMUMMY_WAITSCROLL, KMUMMY_UNWRAP, KMUMMY_DONE};

void FF_SpawnMummySpawner(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);

	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY) - 20*16;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_MummySpawner));
}

void FF_T_MummySpawner(CK_object *ob)
{
	bool wasVisible;

	// update visibility variable (and remember old value):
	wasVisible = ob->user1;
	ob->user1 = CK_ObjectVisible(ob);

	// if no mummy exists and spawner is visible then randomly spawn a mummy:
	if (!ff_map.mummy && ob->user1 && US_RndT() < 0x10)
	{
		CK_object *new_object = CK_GetNewObj(true);
		new_object->posX = ob->posX;
		new_object->posY = ob->posY;
		new_object->type = ob->posX >= 60*256 ? FF_CT_Mummy1 : FF_CT_Mummy2;
		new_object->active = OBJ_ALWAYS_ACTIVE;
		new_object->xDirection = IN_motion_Left;
		if (wasVisible)
		{
			// make mummy rise out of the floor:
			new_object->user1 = MUMMY_RISE;
			SD_PlaySound(FF_MUMMYSND);
			new_object->user2 = MUMMY_HEIGHT;
		}
		CK_SetAction(new_object, CK_ACTION(FF_ACT_Mummy));
		ff_map.mummy = new_object;
	}
}

void FF_MummyCheckForKeen(CK_object *ob)
{
	uint16_t xdist = abs(ck_keenObj->clipRects.unitXmid - ob->clipRects.unitXmid);
	uint16_t ydist = abs(ck_keenObj->posY - ob->posY);

	if (xdist < 5*256 && ydist < 2*256)
	{
		// start running, but only if Keen isn't in a mummy state
		if (ck_keenObj->currentAction != CK_ACTION(FF_ACT_KeenMummy))
		{
			// TODO? maybe play a sound here?
			ob->xDirection = ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid? -1 : 1;
			ob->user1 = MUMMY_RUN;
			CK_SetAction2(ob, CK_ACTION(FF_ACT_Mummy));
		}
	}
}

void FF_T_Mummy(CK_object *ob)
{
	switch (ob->user1)
	{
	case MUMMY_STAND:
#if 0
		//if (US_RndT() <= tics)
		{
set_direction:
			if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
				ob->xDirection = IN_motion_Left;
			else
				ob->xDirection = IN_motion_Right;
		}

		//xtry = 8*ob->xDirection;	// allow mummy to move

		// hide the mummy when it hasn't been visible for a while:
		if (CK_ObjectVisible(ob))
		{
			ob->actionTimer = 0;	// reset counter
		}
		else if ((ob->actionTimer += SD_GetSpriteSync()) >= MUMMY_OFFSCREENDELAY)
		{
			ob->user1 = MUMMY_HIDDEN;
			ob->user2 = MUMMY_HEIGHT;
		}
#else
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 28)
		{
			//ob->actionTimer = 0;	// Note: no need to reset ticcount, action change resets it
			ob->currentAction = CK_ACTION(FF_ACT_MummyLook);
			ob->user1 = MUMMY_LOOK;
		}
#endif
		FF_MummyCheckForKeen(ob);
		break;

	case MUMMY_LOOK:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 48)
		{
			// stop looking:
			ob->currentAction = CK_ACTION(FF_ACT_Mummy);	// Note: no need to reset ticcount, action change resets it
			ob->user1 = MUMMY_AFTERLOOK;
		}
		FF_MummyCheckForKeen(ob);
		break;

	case MUMMY_AFTERLOOK:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 48)
		{
			ob->actionTimer = 0;
			if (++ob->user3 >= 3)
			{
				// looked both directions, so start sinking:
				ob->user1 = MUMMY_SINK;
				SD_PlaySound(FF_MUMMYSND);
				return;
			}

			ob->xDirection = -ob->xDirection;
			ob->user1 = MUMMY_STAND;
			ob->visible = true;
		}
		FF_MummyCheckForKeen(ob);
		break;

	case MUMMY_RISE:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= MUMMY_RISEDELAY)
		{
			ob->actionTimer -= MUMMY_RISEDELAY;
			ob->xDirection = -ob->xDirection;
			ob->visible = true;
			
			if ((ob->user2 -= 4) <= 0)
			{
				ob->user2 = 0;
				ob->user1 = MUMMY_STAND;
				if (ck_keenObj->clipRects.unitXmid < ob->clipRects.unitXmid)
					ob->xDirection = IN_motion_Left;
				else
					ob->xDirection = IN_motion_Right;
			}
		}
		break;

	case MUMMY_SINK:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= MUMMY_SINKDELAY)
		{
			ob->actionTimer -= MUMMY_SINKDELAY;
			ob->xDirection = -ob->xDirection;
			ob->visible = true;
			if ((ob->user2 += 4) >= MUMMY_HEIGHT)
			{
				ob->user2 = MUMMY_HEIGHT;
				ob->user1 = MUMMY_HIDDEN;
			}
		}
		break;

	case MUMMY_RUN:
		//ob->xDirection = (ob->clipRects.unitXmid > player->clipRects.unitXmid)? motion_Left : motion_Right;
		ck_nextX = ob->xDirection*SD_GetSpriteSync()*35;
		// K1n9_Duk3 note: Since the mummy can't change direction in this substate
		// anymore, we don't need extra code to make it stop running if it can't
		// reach Keen. It will will eventually hit a wall or step off a ledge,
		// which will cause it to sink into the ground and disappear.
		break;

	case MUMMY_SPIN:
		ck_nextX = ob->xDirection*SD_GetSpriteSync()*38;
		
		// We don't want the mummy to move (too far) past the destination, but
		// we also need to make sure the mummy's xdir matches the movement
		// for the current frame so that it doesn't get stuck when moving off an
		// egde. That's why we need two checks for each direction: one to make
		// sure the mummy doesnt move past the destination and another to make
		// it turn once it has reached the destination.
		
		if (ob->xDirection > 0)
		{
			 uint16_t destx = ck_keenObj->clipRects.unitXmid+256;
			if (ob->clipRects.unitXmid >= destx)
			{
				ob->xDirection = IN_motion_Left;
				ob->zLayer = ck_keenObj->zLayer-1;
				ck_nextX = -ck_nextX;
			}
			else if (ob->clipRects.unitXmid+ck_nextX > destx)
			{
				ck_nextX = destx - ob->clipRects.unitXmid;
			}
		}
		else
		{
			 uint16_t destx = ck_keenObj->clipRects.unitXmid-256;
			if (ob->clipRects.unitXmid <= destx)
			{
				ob->xDirection = IN_motion_Right;
				ob->zLayer = ck_keenObj->zLayer+1;
				ck_nextX = -ck_nextX;
			}
			else if (ob->clipRects.unitXmid+ck_nextX < destx)
			{
				ck_nextX = destx - ob->clipRects.unitXmid;
			}
		}
		break;

	case MUMMY_SPINDONE:
		// TODO: move away from Keen?
		ob->user1 = MUMMY_SINK;
		SD_PlaySound(FF_MUMMYSND);
		break;

	case MUMMY_HIDDEN:
		// remove the hidden mummy after a while:
		// (this allows the mummy spawner to spawn a new mummy)
		if ((ob->actionTimer += SD_GetSpriteSync()) >= MUMMY_RESPAWNDELAY)
		{
			if (ff_map.mummy == ob)
				ff_map.mummy = NULL;
			CK_RemoveObj(ob);
		}
		break;

	}
}
/*
void T_MummyLook(objtype *ob)
{
	int xdist = player->clipRects.unitXmid - ob->clipRects.unitXmid;
	int ydist = abs(player->posY - ob->posY);

	ob->actionTimer += tics;
	if (ob->xDirection > 0)
	{
		if (xdist >= 0 && xdist < 4*TILEGLOBAL && ydist < 2*TILEGLOBAL)
		{
start_running:
			// start running, but only if Keen isn't in a mummy state
			if (ob->user1 != MUMMY_RUN && player->currentAction != CK_ACTION(FF_ACT_KeenMummy))
			{
				ob->user1 = MUMMY_RUN;
			}
		}
	}
	else
	{
		if (xdist <= 0 && xdist > -4*TILEGLOBAL && ydist < 2*TILEGLOBAL)
		{
			goto start_running;
		}
	}

	if (ob->actionTimer >= 48)
	{
		// stop looking:
		ob->currentAction = CK_ACTION(FF_ACT_Mummy);
	}
}
*/

void FF_C_Mummy(CK_object *ob, CK_object *hit)
{
	// fake a smaller hitbox when mummy is sinking/rising:
	if (ob->user1 == MUMMY_HIDDEN || hit->clipRects.unitY2 < ob->clipRects.unitY1 + RF_PixelToUnit(ob->user2))
		return;

	switch (hit->type)
	{
	case CT_Player:
		if (FF_KeenIsDead())
			break;

		switch (ob->user1)
		{
		case MUMMY_RUN:
		case MUMMY_STAND:
		case MUMMY_LOOK:
			ob->user1 = MUMMY_SPIN;
			SD_PlaySound(FF_MUMMYENWRAPPINGSND);
			CK_SetAction2(ob, CK_ACTION(FF_ACT_Mummy));

			hit->type = ob->type;
			hit->clipped = CLIP_normal;
			hit->zLayer = 1;
			hit->gfxChunk = FF_KEENSTANDSPR;
			hit->user1=hit->user2=hit->user3=hit->user4 = 0;
			CK_SetAction2(hit, CK_ACTION(FF_ACT_KeenMummy));
		}
		break;

	case CT_Stunner:
		FF_ExplodeShot(hit);
		CK_SetAction2(ob, CK_ACTION(FF_ACT_Mummy));
		ob->user1 = MUMMY_SINK;
		break;
	}
}

void FF_R_Mummy(CK_object *ob)
{
	if (!ob->topTI || ob->leftTI && ob->xDirection > 0 || ob->rightTI && ob->xDirection < 0)
	{
		 int16_t oldtimer = ob->actionTimer;
		ob->posX -= ob->deltaPosX;
		ob->xDirection = -ob->xDirection;
		CK_SetAction2(ob, ob->currentAction);
		ob->actionTimer = oldtimer;	// don't interrupt the offscreen counter
		switch (ob->user1)
		{
		case MUMMY_SPIN:
			ob->zLayer = ck_keenObj->zLayer+ob->xDirection;
			break;

		case MUMMY_RUN:
			ob->user1 = MUMMY_SPINDONE;
			break;
		}
	}
	RF_PlaceSpriteClipped(&ob->sde, ob->posX, ob->posY+RF_PixelToUnit(ob->user2), ob->gfxChunk, false, ob->zLayer, -ob->user2);
}

void FF_T_KeenMummy(CK_object *ob)
{
	switch (ob->user1)
	{
	case KMUMMY_FALL:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 2*70)
		{
			// Mummy couldn't reach Keen, so reset Keen:
			ob->currentAction = CK_ACTION(CK_ACT_keenFall1);
			ob->type = CT_Player;

			// reset mummy:
			if (ff_map.mummy)
				ff_map.mummy->user1 = MUMMY_SPINDONE;
		}
		// no break here - both substates need gravity

	case KMUMMY_PREWRAP:
		FF_DoGravity(ob);
		break;

	case KMUMMY_WRAP:
		// wait until mummy is gone:
		if (!ff_map.mummy || ff_map.mummy->user1 == MUMMY_HIDDEN)
		{
			ob->actionTimer = 0;
			ob->user1++;
		}
		break;

	case KMUMMY_POSTWRAP:
		// wait two seconds before making the bandages fall:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 2*70)
		{
			ob->actionTimer -= BANDAGE_ANIMDELAY;
			if (++ob->gfxChunk == FF_BANDAGEFALL3SPR)
			{
				ob->actionTimer = 0;
				ob->user1++;
			}
		}
		break;

	case KMUMMY_VANISH:
		// wait a while, then start moving the camera to the destination:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 2*70)
		{
			// spawn a removable bandage where Keen was standing:
			CK_object *new_object = CK_GetNewObj(true);
			new_object->posX = ob->posX;
			new_object->posY = ob->posY;
			new_object->zLayer = 1;
			new_object->gfxChunk = FF_BANDAGEFALL3SPR;
			CK_SetAction(new_object, CK_ACTION(FF_ACT_Bandage));

			// move Keen to the destination:
			ob->velX=ob->velY=ob->user2=ob->user3=ob->user4 = 0;
			if (ob->type == FF_CT_Mummy1)
			{
				FF_SpawnKeen(92, 79, 0);
			}
			else
			{
				FF_SpawnKeen(71, 68, 0);
			}
			ob->currentAction = CK_ACTION(FF_ACT_KeenMummy);
			ob->gfxChunk = FF_KEENMUMMY4SPR;
			ob->user1 = KMUMMY_WAITSCROLL;
		}
		break;

	case KMUMMY_WAITSCROLL:
		// waiting for the camera to stop scrolling:
		if (ob->user2 == rf_scrollXUnit && ob->user3 == rf_scrollYUnit)
		{
			ob->user1++;
		}
		else
		{
			ob->user2 = rf_scrollXUnit;
			ob->user3 = rf_scrollYUnit;
		}
		break;

	case KMUMMY_UNWRAP:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 3*70)
		{
			// spawn falling bandages:
			CK_object *new_object = CK_GetNewObj(true);
			new_object->posX = ob->posX;
			new_object->posY = ob->posY;
			new_object->zLayer = 2;	// must be in front of Keen
			new_object->gfxChunk = FF_BANDAGEFALL1SPR;
			CK_SetAction(new_object, CK_ACTION(FF_ACT_Bandage));

			ob->gfxChunk = FF_KEENSTANDSPR;
			ob->user1++;
			ob->actionTimer = 0;
		}
		break;

	case KMUMMY_DONE:
		if ((ob->actionTimer += SD_GetSpriteSync()) >= 5*BANDAGE_ANIMDELAY)
		{
			// allow player to control Keen again:
			ob->currentAction = CK_ACTION(CK_ACT_keenStanding);
			ob->user1=ob->user2=ob->user3=ob->user4 = 0;
		}
	}
}

void FF_C_KeenMummy(CK_object *ob, CK_object *hit)
{
	// always run Keen's default contact routine first to allow Keen to land on platform objects:
	FF_KeenContact(ob, hit);
	
	if (ob->user1 > KMUMMY_PREWRAP)
		return;

	switch (hit->type)
	{
	case FF_CT_Mummy1:
	case FF_CT_Mummy2:
		// only update the wrapping state if Keen is standing on something solid
		//	and the mummy has moved past Keen during the current frame:
		if (ob->topTI && hit->clipRects.unitXmid >= ob->clipRects.unitXmid
			&& hit->clipRects.unitXmid-hit->deltaPosX < ob->clipRects.unitXmid)
		{
			ob->user1 = KMUMMY_PREWRAP;
			ob->visible = true;
			FF_RemoveDreamBubbles();	// Nisaba patch: just in case Keen is dreaming

			// only switch to the next wrap sprite if Mummy is drawn in front of Keen:
			if (hit->zLayer <= ob->zLayer)
				break;

			if (ob->gfxChunk == FF_KEENSTANDSPR)
			{
				ob->gfxChunk = FF_KEENMUMMY1SPR;
			}
			else
			{
				ob->gfxChunk++;
				if (ob->gfxChunk == FF_KEENMUMMY6SPR)
				{
					ob->actionTimer = 0;
					ob->user1++;
					hit->user1 = MUMMY_SPINDONE;
				}
			}
		}
	}
}

void FF_T_Bandage(CK_object *ob)
{
	if (ob->gfxChunk == FF_BANDAGEFALL3SPR)
	{
		if (!CK_ObjectVisible(ob) || ++ob->user1 >= 20)
			CK_RemoveObj(ob);
	}
	else
	{
		ob->gfxChunk++;
	}
}


/*
==============================================================================
	LEMON SHARK

	Floats left and disappears as soon as it's off-camera. Just eye candy.

	temp1 -
	temp2 -
	temp3 -
	temp4 - 
==============================================================================
*/


void FF_SpawnLemonShark(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->clipped = CLIP_not;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	//new->gfxChunk = LEMONSHARKSPR;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_LemonShark));
}

void FF_T_LemonShark(CK_object *ob)
{
	FF_AccelerateX(ob, IN_motion_Left, 8);
	FF_AccelerateY(ob, IN_motion_Down, 2);

	if (!CK_ObjectVisible(ob))
		CK_RemoveObj(ob);
}


/*
==============================================================================
	DONUT CLOUD

	Appears on the worldmap, hovers in place when 500000 points are reached
	and disappears again after the bonus level has been completed.

	temp1 - initial y position (for hovering)
	temp2 - 
	temp3 - amplitude
	temp4 - 
==============================================================================
*/


void FF_SpawnDonutCloud(int16_t tileX, int16_t tileY)
{
	if (ck_gameState.keenScore >= BONUSLEVEL_SCORELIMIT && !ff_state.cheated && !ck_gameState.levelsDone[MAP_BONUSLEVEL])
	{
		CK_object *new_object = CK_GetNewObj(false);
		new_object->type = FF_CT_DonutCloud;
	
		new_object->clipped = CLIP_not;
		new_object->zLayer = FOREGROUNDPRIORITY;
		new_object->posX = RF_TileToUnit(tileX) - 8*16;
		new_object->posY = RF_TileToUnit(tileY) - 6*16;
		new_object->user1 = new_object->posY;
		new_object->user3 = 24;
		//new->xDirection = motion_None;	// redundant
		new_object->yDirection = IN_motion_Up;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_DonutCloud));
		
		FF_SpawnStaticSprite(tileX, tileY+2, FF_DONUTCLOUDSHADOWSPR, 0);
	}
}

void FF_T_DonutCloud(CK_object *ob)
{
	if (ob->yDirection == IN_motion_Up && ob->posY < ob->user1-ob->user3)
	{
		ob->yDirection = IN_motion_Down;
	}
	else if (ob->yDirection == IN_motion_Down && ob->posY > ob->user1+ob->user3)
	{
		ob->yDirection = IN_motion_Up;
	}

	FF_AccelerateY(ob, ob->yDirection, 4);
}



/*
==============================================================================
	JEDI BOOT

	Object to enter the secret TARDIS level or the MUSHROOM2 level when touched.
	
	temp1 - non-zero after explode sound has been played
	temp2 - 
	temp3 - 
	temp4 - 
==============================================================================
*/


void FF_SpawnJediBoot(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->type = FF_CT_JediBoot;
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	CK_SetAction(new_object, CK_ACTION(FF_ACT_JediBoot));	
	//new->user4 = 0;
}

void FF_T_JediBoot(CK_object *ob)
{
	//ob->gfxChunk = JEDIBOOTSPR;
	
	if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1 && !ob->user1)
	{
		ob->user1 = 1;	// don't play it again, Sam!
		SD_PlaySound(FF_KITTENEXPLODESND);
	}
}

void FF_C_JediBoot(CK_object *ob, CK_object *hit) 
{
	if (hit->type == CT_Player)
	{
		if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL1)
		{
			//playstate = ex_mushroomtrigger;
			
			// turn Keen into smoke cloud:
			hit->gfxChunk = FF_SMIRKYPOOF1SPR;
			hit->type = CT_Friendly;	// prevent other objects from interacting with Keen
			hit->clipped = CLIP_not;
			hit->posX -= 8*16;
			hit->user1 = 0;
			CK_SetAction2(hit, CK_ACTION(FF_ACT_KeenSmoke));
			hit->actionTimer = 10;
			SD_PlaySound(FF_KITTENEXPLODESND);
		}
		else if (ck_gameState.currentLevel == MAP_WORLDMAP)
		{
			FF_MiscDialog(-1);
			CK_RemoveObj(ob);	// remove the JediBoot object
		}
		else if (ck_gameState.currentLevel == MAP_MUSHROOMLEVEL2)
		{
			if (ff_state.cheated)
			{
				FF_MiscDialog(-2);
				CK_RemoveObj(ob);	// remove the JediBoot object
			}
			else
			{
				ck_gameState.levelState = ex_secrettrigger;
			}
		}
	}
}

void FF_T_KeenSmoke(CK_object *ob)
{
	if ((ob->actionTimer -= SD_GetSpriteSync()) <= 0)
	{
		if (ob->gfxChunk)
		{
			ob->actionTimer += 10;
			if (++ob->gfxChunk > FF_SMIRKYPOOF5SPR)
			{
				ob->gfxChunk = 0;
				ob->actionTimer += 1*70;	// level ends 1 second after smoke is gone
				ck_gameState.numShots = 0;	// just in case...
			}
		}
		else
		{
			ck_gameState.levelState = ex_mushroomtrigger;
		}
	}
	
	// make Keen explode like an ammo pinata:
	if (ck_gameState.numShots > 0 && (ob->user1 -= SD_GetSpriteSync()) <= 0)
	{
		CK_object *new_object = CK_GetNewObj(true);
		new_object->posX = ob->posX+8*16;
		new_object->posY = ob->posY;
		new_object->gfxChunk = FF_ITEMSTUNNER1SPR;
		new_object->velX = 16 + (US_RndT() >> 3);	// random value between 16 and 47
		new_object->velY = -40 - (US_RndT() >> 4);	// random value between -40 and -55
		new_object->zLayer = FOREGROUNDPRIORITY;
		new_object->clipped = CLIP_not;
		CK_SetAction(new_object, CK_ACTION(FF_ACT_EggPiece));
		ob->user1 += 8;
		
		if ((ck_gameState.numShots -= 5) < 0)
			ck_gameState.numShots = 0;
		
	}
}



/*
==============================================================================
	SLUG CANS

	Mainly eyecandy. Cans are displayed in the BWB level,	depending on how many
	cans have already been collected. At 7, 11 or all of them
==============================================================================
*/

void FF_SpawnSlugCans(int16_t tileX, int16_t tileY)
{
	// Note: The static sprites use MAXPRIORITY to make sure they are drawn on
	// on top of the BWB sprite. This could cause conflicts with the score box,
	// but there is no scrolling in the BWB level, so these sprites and the score
	// box will never overlap.
	
	CK_object *ob;
	int16_t toplace = ff_state.slugcans;
	
	// Note: Cheaters can collect an infinite amount of slug cans, which will
	// lead to extra cans being spawned at a weird position and will eventually
	// lead to a GetNewObj crash. Probably nothing to be too worried about, but
	// the following code could be used to avoid that problem.
#if 0
	if (toplace > SLUGCANS_TOTAL)
	{
		toplace = SLUGCANS_TOTAL;
	}
#endif
	
#if 1
	// Nisaba's original logic for the slug can sprites:
	if (ff_state.slugcans >= 7 && ff_state.slugcans < SLUGCANS_TOTAL)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_SLUGCANS1SPR, MAXPRIORITY);
		ob->posX += -5*16;
		ob->posY += 37*16;
		toplace -= 7;	// K1n9_Duk3 addition
	}
	if (ff_state.slugcans >= 11)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_SLUGCANS3SPR, MAXPRIORITY);
		ob->posX += 126*16;
		ob->posY += -13*16;
		toplace -= 4;	// K1n9_Duk3 addition
	}
	if (ff_state.slugcans >= SLUGCANS_TOTAL)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_SLUGCANS2SPR, MAXPRIORITY);
		ob->posX += -5*16;
		ob->posY += 7*16;
		toplace -= 13;	// K1n9_Duk3 addition
	}
#else
	// K1n9_Duk3's hacked up version (to have as few extra cans as possible).
	// With this version, we would never need to spawn more than 3 extra cans.
	// I tested this, but having the cans get moved around all the time seemed
	// weird -- expecially when the cans that replace the whiskey bottle and the
	// battery are repeatedly taken out and put back in again. So I went back to
	// using Nisaba's original logic.
	if (toplace >= 13)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_SLUGCANS2SPR, MAXPRIORITY);
		ob->posX += -5*16;
		ob->posY += 7*16;
		toplace -= 13;
	}
	else if (toplace >= 7)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_SLUGCANS1SPR, MAXPRIORITY);
		ob->posX += -5*16;
		ob->posY += 37*16;
		toplace -= 7;
	}
	if (toplace >= 4)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_SLUGCANS3SPR, MAXPRIORITY);
		ob->posX += 126*16;
		ob->posY += -13*16;
		toplace -= 4;
	}
#endif

	// K1n9_Duk3 addition:
	// Spawn extra sprites for the collected cans that aren't shown in Nisaba's
	// big hand-crafted sprites, so that the players can see EXACTLY how many
	// cans they have collected so far when they visit the BWB level.
	while (toplace-- > 0)
	{
		ob = FF_SpawnStaticSprite(tileX, tileY, FF_BALLSPR, 0);
		if (toplace >= 4)
		{
			ob->posX += (toplace-4)*8*16 - 5*256 + 4*16;
			ob->posY += 1*256-0*16;
		}
		else
		{
			ob->posX += toplace*8*16 - 5*256;
			ob->posY += 2*256-0*16;
		}
	}
	// Note that these extra cans get spawned on the far left, where Keen enters
	// the level when he returns to the BWB level. I didn't want to place them
	// on top of the BWB sprite since that would make the performance even worse
	// when that huge BWB sprite needs to be redrawn. Having the two hand-crafted
	// slug can sprites on top of the BWB sprite is bad enough, no need to add
	// another 5-6 small sprites on top of that. We really should have just
	// implemented the BWB with foreground tiles instead of using a sprite, but
	// it seems a bit too late to do that now.
}

/*
==============================================================================
	BIG SHROOM
	
	just eyecandy (spawning smoke clouds)
==============================================================================
*/

void FF_SpawnBigShroom(int16_t tileX, int16_t tileY)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->posX = RF_TileToUnit(tileX);
	new_object->posY = RF_TileToUnit(tileY);
	new_object->zLayer = FOREGROUNDPRIORITY-1;
	new_object->gfxChunk = FF_SMIRKYPOOF1SPR;
	new_object->user1 = FF_SMIRKYPOOF4SPR;
	new_object->actionTimer = US_RndT() & 7;
	CK_SetAction(new_object, CK_ACTION(FF_ACT_Smoke));
}

void kw_ff_act_SetupFunctions(void)
{
	CK_ACT_AddFunction("FF_AnimateCloneObj", &FF_AnimateCloneObj);
	CK_ACT_AddFunction("FF_CheckpointSave", &FF_CheckpointSave);
	CK_ACT_AddFunction("FF_ChemCountdown", &FF_ChemCountdown);
	CK_ACT_AddFunction("FF_ControlPage", &FF_ControlPage);
	CK_ACT_AddFunction("FF_CrabDraw", &FF_CrabDraw);
	CK_ACT_AddFunction("FF_CrabDrawFlipped", &FF_CrabDrawFlipped);
	CK_ACT_AddFunction("FF_CrabFallReact", &FF_CrabFallReact);
	CK_ACT_AddFunction("FF_CrocCheckTiles", &FF_CrocCheckTiles);
	CK_ACT_AddFunction("FF_DoorOpen", &FF_DoorOpen);
	CK_ACT_AddFunction("FF_EfreeGrabKeen", &FF_EfreeGrabKeen);
	CK_ACT_AddFunction("FF_KeenLook", &FF_KeenLook);
	CK_ACT_AddFunction("FF_KnawlReact", &FF_KnawlReact);
	CK_ACT_AddFunction("FF_LindseyVanish", &FF_LindseyVanish);
	CK_ACT_AddFunction("FF_MoletWalkReact", &FF_MoletWalkReact);
	CK_ACT_AddFunction("FF_MummyCheckForKeen", &FF_MummyCheckForKeen);
	CK_ACT_AddFunction("FF_NisasiLetGo", &FF_NisasiLetGo);
	CK_ACT_AddFunction("FF_ObjDropNisasi", &FF_ObjDropNisasi);
	CK_ACT_AddFunction("FF_PageWalk", &FF_PageWalk);
	CK_ACT_AddFunction("FF_PageWalkReact", &FF_PageWalkReact);
	CK_ACT_AddFunction("FF_R_Bird", &FF_R_Bird);
	CK_ACT_AddFunction("FF_R_BirdEgg", &FF_R_BirdEgg);
	CK_ACT_AddFunction("FF_R_Bounder", &FF_R_Bounder);
	CK_ACT_AddFunction("FF_R_Bucket", &FF_R_Bucket);
	CK_ACT_AddFunction("FF_R_Checkpoint", &FF_R_Checkpoint);
	CK_ACT_AddFunction("FF_R_CloneObj", &FF_R_CloneObj);
	CK_ACT_AddFunction("FF_R_CoronaBall", &FF_R_CoronaBall);
	CK_ACT_AddFunction("FF_R_Council", &FF_R_Council);
	CK_ACT_AddFunction("FF_R_CrabCeiling", &FF_R_CrabCeiling);
	CK_ACT_AddFunction("FF_R_CrabFloor", &FF_R_CrabFloor);
	CK_ACT_AddFunction("FF_R_CrabTurn", &FF_R_CrabTurn);
	CK_ACT_AddFunction("FF_R_CrabWall", &FF_R_CrabWall);
	CK_ACT_AddFunction("FF_R_CrocJump", &FF_R_CrocJump);
	CK_ACT_AddFunction("FF_R_CrocSnap", &FF_R_CrocSnap);
	CK_ACT_AddFunction("FF_R_CrocWalk", &FF_R_CrocWalk);
	CK_ACT_AddFunction("FF_R_Draw", &FF_R_Draw);
	CK_ACT_AddFunction("FF_R_EggPiece", &FF_R_EggPiece);
	CK_ACT_AddFunction("FF_R_Fall", &FF_R_Fall);
	CK_ACT_AddFunction("FF_R_FallBlock", &FF_R_FallBlock);
	CK_ACT_AddFunction("FF_R_Frog", &FF_R_Frog);
	CK_ACT_AddFunction("FF_R_Fungus", &FF_R_Fungus);
	CK_ACT_AddFunction("FF_R_FungusFall", &FF_R_FungusFall);
	CK_ACT_AddFunction("FF_R_GellyGhost", &FF_R_GellyGhost);
	CK_ACT_AddFunction("FF_R_LeaveaFall", &FF_R_LeaveaFall);
	CK_ACT_AddFunction("FF_R_LeaveaWalk", &FF_R_LeaveaWalk);
	CK_ACT_AddFunction("FF_R_Lindsey", &FF_R_Lindsey);
	CK_ACT_AddFunction("FF_R_Log", &FF_R_Log);
	CK_ACT_AddFunction("FF_R_MoletFall", &FF_R_MoletFall);
	CK_ACT_AddFunction("FF_R_Mummy", &FF_R_Mummy);
	CK_ACT_AddFunction("FF_R_Nisasi", &FF_R_Nisasi);
	CK_ACT_AddFunction("FF_R_Obelisk", &FF_R_Obelisk);
	CK_ACT_AddFunction("FF_R_Page", &FF_R_Page);
	CK_ACT_AddFunction("FF_R_Pest", &FF_R_Pest);
	CK_ACT_AddFunction("FF_R_PestRest", &FF_R_PestRest);
	CK_ACT_AddFunction("FF_R_Platform", &FF_R_Platform);
	CK_ACT_AddFunction("FF_R_PoleknawelFall", &FF_R_PoleknawelFall);
	CK_ACT_AddFunction("FF_R_PoleknawelWalk", &FF_R_PoleknawelWalk);
	CK_ACT_AddFunction("FF_R_ShrubberyFall", &FF_R_ShrubberyFall);
	CK_ACT_AddFunction("FF_R_SlugWalk", &FF_R_SlugWalk);
	CK_ACT_AddFunction("FF_R_SneakyTree", &FF_R_SneakyTree);
	CK_ACT_AddFunction("FF_R_Stunned", &FF_R_Stunned);
	CK_ACT_AddFunction("FF_R_TinyYetiJump", &FF_R_TinyYetiJump);
	CK_ACT_AddFunction("FF_R_TinyYetiRoll", &FF_R_TinyYetiRoll);
	CK_ACT_AddFunction("FF_R_Walk", &FF_R_Walk);
	CK_ACT_AddFunction("FF_R_WalkNormal", &FF_R_WalkNormal);
	CK_ACT_AddFunction("FF_R_WorldNessie", &FF_R_WorldNessie);
	CK_ACT_AddFunction("FF_R_XkyLick", &FF_R_XkyLick);
	CK_ACT_AddFunction("FF_R_XkyLickAir", &FF_R_XkyLickAir);
	CK_ACT_AddFunction("FF_RemoveLeaf", &FF_RemoveLeaf);
	CK_ACT_AddFunction("FF_SalamanderThink", &FF_SalamanderThink);
	CK_ACT_AddFunction("FF_ShrubberyLookThink", &FF_ShrubberyLookThink);
	CK_ACT_AddFunction("FF_T_BalloonSmirky", &FF_T_BalloonSmirky);
	CK_ACT_AddFunction("FF_T_Bandage", &FF_T_Bandage);
	CK_ACT_AddFunction("FF_T_BirdAttack", &FF_T_BirdAttack);
	CK_ACT_AddFunction("FF_T_BirdEgg", &FF_T_BirdEgg);
	CK_ACT_AddFunction("FF_T_BirdFly", &FF_T_BirdFly);
	CK_ACT_AddFunction("FF_T_BirdLayEgg", &FF_T_BirdLayEgg);
	CK_ACT_AddFunction("FF_T_Bonus", &FF_T_Bonus);
	CK_ACT_AddFunction("FF_T_Bounder", &FF_T_Bounder);
	CK_ACT_AddFunction("FF_T_BrainFucl", &FF_T_BrainFucl);
	CK_ACT_AddFunction("FF_T_Checkpoint", &FF_T_Checkpoint);
	CK_ACT_AddFunction("FF_T_CheckpointCap", &FF_T_CheckpointCap);
	CK_ACT_AddFunction("FF_T_ChemCycle", &FF_T_ChemCycle);
	CK_ACT_AddFunction("FF_T_ChemHidden", &FF_T_ChemHidden);
	CK_ACT_AddFunction("FF_T_ChemStand", &FF_T_ChemStand);
	CK_ACT_AddFunction("FF_T_CloneAnimation", &FF_T_CloneAnimation);
	CK_ACT_AddFunction("FF_T_CloneMachine", &FF_T_CloneMachine);
	CK_ACT_AddFunction("FF_T_CloneObj", &FF_T_CloneObj);
	CK_ACT_AddFunction("FF_T_Cloud", &FF_T_Cloud);
	CK_ACT_AddFunction("FF_T_CoronaBall", &FF_T_CoronaBall);
	CK_ACT_AddFunction("FF_T_CoronaPlant", &FF_T_CoronaPlant);
	CK_ACT_AddFunction("FF_T_Council", &FF_T_Council);
	CK_ACT_AddFunction("FF_T_CouncilMop", &FF_T_CouncilMop);
	CK_ACT_AddFunction("FF_T_CrabCeiling", &FF_T_CrabCeiling);
	CK_ACT_AddFunction("FF_T_CrabFloor", &FF_T_CrabFloor);
	CK_ACT_AddFunction("FF_T_CrabTurn", &FF_T_CrabTurn);
	CK_ACT_AddFunction("FF_T_CrabWall", &FF_T_CrabWall);
	CK_ACT_AddFunction("FF_T_CrocJump", &FF_T_CrocJump);
	CK_ACT_AddFunction("FF_T_CrocSnap", &FF_T_CrocSnap);
	CK_ACT_AddFunction("FF_T_CrocTrap", &FF_T_CrocTrap);
	CK_ACT_AddFunction("FF_T_CrocWalk", &FF_T_CrocWalk);
	CK_ACT_AddFunction("FF_T_Donut", &FF_T_Donut);
	CK_ACT_AddFunction("FF_T_DonutCloud", &FF_T_DonutCloud);
	CK_ACT_AddFunction("FF_T_DropPlatFall", &FF_T_DropPlatFall);
	CK_ACT_AddFunction("FF_T_DropPlatRise", &FF_T_DropPlatRise);
	CK_ACT_AddFunction("FF_T_DropPlatSit", &FF_T_DropPlatSit);
	CK_ACT_AddFunction("FF_T_Efree", &FF_T_Efree);
	CK_ACT_AddFunction("FF_T_FallBlock", &FF_T_FallBlock);
	CK_ACT_AddFunction("FF_T_Frog", &FF_T_Frog);
	CK_ACT_AddFunction("FF_T_Fungus", &FF_T_Fungus);
	CK_ACT_AddFunction("FF_T_GellyGhost", &FF_T_GellyGhost);
	CK_ACT_AddFunction("FF_T_JediBoot", &FF_T_JediBoot);
	CK_ACT_AddFunction("FF_T_KeenEnterMachine", &FF_T_KeenEnterMachine);
	CK_ACT_AddFunction("FF_T_KeenMindcontrol", &FF_T_KeenMindcontrol);
	CK_ACT_AddFunction("FF_T_KeenMummy", &FF_T_KeenMummy);
	CK_ACT_AddFunction("FF_T_KeenSmoke", &FF_T_KeenSmoke);
	CK_ACT_AddFunction("FF_T_KittenExplode", &FF_T_KittenExplode);
	CK_ACT_AddFunction("FF_T_KittenIdle", &FF_T_KittenIdle);
	CK_ACT_AddFunction("FF_T_KittenRoll", &FF_T_KittenRoll);
	CK_ACT_AddFunction("FF_T_LeaveaFall", &FF_T_LeaveaFall);
	CK_ACT_AddFunction("FF_T_LeaveaTree", &FF_T_LeaveaTree);
	CK_ACT_AddFunction("FF_T_LeaveaWalk", &FF_T_LeaveaWalk);
	CK_ACT_AddFunction("FF_T_LemonShark", &FF_T_LemonShark);
	CK_ACT_AddFunction("FF_T_Lindsey", &FF_T_Lindsey);
	CK_ACT_AddFunction("FF_T_Log", &FF_T_Log);
	CK_ACT_AddFunction("FF_T_Molet", &FF_T_Molet);
	CK_ACT_AddFunction("FF_T_MoletDig", &FF_T_MoletDig);
	CK_ACT_AddFunction("FF_T_MoletDirt", &FF_T_MoletDirt);
	CK_ACT_AddFunction("FF_T_MoletFall", &FF_T_MoletFall);
	CK_ACT_AddFunction("FF_T_Mummy", &FF_T_Mummy);
	CK_ACT_AddFunction("FF_T_MummySpawner", &FF_T_MummySpawner);
	CK_ACT_AddFunction("FF_T_Nipnap", &FF_T_Nipnap);
	CK_ACT_AddFunction("FF_T_Nisasi", &FF_T_Nisasi);
	CK_ACT_AddFunction("FF_T_Obelisk", &FF_T_Obelisk);
	CK_ACT_AddFunction("FF_T_Page", &FF_T_Page);
	CK_ACT_AddFunction("FF_T_PatPat", &FF_T_PatPat);
	CK_ACT_AddFunction("FF_T_PestFly", &FF_T_PestFly);
	CK_ACT_AddFunction("FF_T_PestRest", &FF_T_PestRest);
	CK_ACT_AddFunction("FF_T_Platform", &FF_T_Platform);
	CK_ACT_AddFunction("FF_T_PoleknawelLand", &FF_T_PoleknawelLand);
	CK_ACT_AddFunction("FF_T_PoleknawelSlide", &FF_T_PoleknawelSlide);
	CK_ACT_AddFunction("FF_T_PoleknawelThrow", &FF_T_PoleknawelThrow);
	CK_ACT_AddFunction("FF_T_PoleknawelWalk", &FF_T_PoleknawelWalk);
	CK_ACT_AddFunction("FF_T_Projectile", &FF_T_Projectile);
	CK_ACT_AddFunction("FF_T_Quicksand", &FF_T_Quicksand);
	CK_ACT_AddFunction("FF_T_Roots", &FF_T_Roots);
	CK_ACT_AddFunction("FF_T_Shrubbery", &FF_T_Shrubbery);
	CK_ACT_AddFunction("FF_T_ShrubberyWalk", &FF_T_ShrubberyWalk);
	CK_ACT_AddFunction("FF_T_SlideDoor", &FF_T_SlideDoor);
	CK_ACT_AddFunction("FF_T_Slug", &FF_T_Slug);
	CK_ACT_AddFunction("FF_T_SlugPiss", &FF_T_SlugPiss);
	CK_ACT_AddFunction("FF_T_Smoke", &FF_T_Smoke);
	CK_ACT_AddFunction("FF_T_SneakyTree", &FF_T_SneakyTree);
	CK_ACT_AddFunction("FF_T_Swallowed", &FF_T_Swallowed);
	CK_ACT_AddFunction("FF_T_TantacleNest", &FF_T_TantacleNest);
	CK_ACT_AddFunction("FF_T_Tentacle", &FF_T_Tentacle);
	CK_ACT_AddFunction("FF_T_TentacleAttack", &FF_T_TentacleAttack);
	CK_ACT_AddFunction("FF_T_TinyYetiJump", &FF_T_TinyYetiJump);
	CK_ACT_AddFunction("FF_T_TinyYetiRoll", &FF_T_TinyYetiRoll);
	CK_ACT_AddFunction("FF_T_Velocity", &FF_T_Velocity);
	CK_ACT_AddFunction("FF_T_Warble", &FF_T_Warble);
	CK_ACT_AddFunction("FF_T_WeakProjectile", &FF_T_WeakProjectile);
	CK_ACT_AddFunction("FF_T_WorldNessie", &FF_T_WorldNessie);
	CK_ACT_AddFunction("FF_T_XkyLickBounce", &FF_T_XkyLickBounce);
	CK_ACT_AddFunction("FF_T_XkyLickShuffle", &FF_T_XkyLickShuffle);
	CK_ACT_AddFunction("FF_T_Yeti", &FF_T_Yeti);
	CK_ACT_AddFunction("FF_T_YetiCloud", &FF_T_YetiCloud);
	CK_ACT_AddFunction("FF_TurnX", &FF_TurnX);
	CK_ACT_AddFunction("FF_TurnY", &FF_TurnY);
	CK_ACT_AddFunction("FF_WormouthLookLeftThink", &FF_WormouthLookLeftThink);
	CK_ACT_AddFunction("FF_WormouthLookRightThink", &FF_WormouthLookRightThink);
	CK_ACT_AddFunction("FF_WormouthLookThink", &FF_WormouthLookThink);
	CK_ACT_AddFunction("FF_WormouthThink", &FF_WormouthThink);
	CK_ACT_AddFunction("FF_WormouthWalkReact", &FF_WormouthWalkReact);
	CK_ACT_AddColFunction("FF_C_Bird", &FF_C_Bird);
	CK_ACT_AddColFunction("FF_C_BirdEgg", &FF_C_BirdEgg);
	CK_ACT_AddColFunction("FF_C_Bounder", &FF_C_Bounder);
	CK_ACT_AddColFunction("FF_C_BrainFucl", &FF_C_BrainFucl);
	CK_ACT_AddColFunction("FF_C_Chem", &FF_C_Chem);
	CK_ACT_AddColFunction("FF_C_ChemRide", &FF_C_ChemRide);
	CK_ACT_AddColFunction("FF_C_CloneMachine", &FF_C_CloneMachine);
	CK_ACT_AddColFunction("FF_C_Council", &FF_C_Council);
	CK_ACT_AddColFunction("FF_C_Crab", &FF_C_Crab);
	CK_ACT_AddColFunction("FF_C_Croc", &FF_C_Croc);
	CK_ACT_AddColFunction("FF_C_FallBlock", &FF_C_FallBlock);
	CK_ACT_AddColFunction("FF_C_Fungus", &FF_C_Fungus);
	CK_ACT_AddColFunction("FF_C_GellyGhost", &FF_C_GellyGhost);
	CK_ACT_AddColFunction("FF_C_JediBoot", &FF_C_JediBoot);
	CK_ACT_AddColFunction("FF_C_KeenMummy", &FF_C_KeenMummy);
	CK_ACT_AddColFunction("FF_C_Kitten", &FF_C_Kitten);
	CK_ACT_AddColFunction("FF_C_LeaveaWalk", &FF_C_LeaveaWalk);
	CK_ACT_AddColFunction("FF_C_Lethal", &FF_C_Lethal);
	CK_ACT_AddColFunction("FF_C_LethalBlock", &FF_C_LethalBlock);
	CK_ACT_AddColFunction("FF_C_Molet", &FF_C_Molet);
	CK_ACT_AddColFunction("FF_C_Mummy", &FF_C_Mummy);
	CK_ACT_AddColFunction("FF_C_Nipnap", &FF_C_Nipnap);
	CK_ACT_AddColFunction("FF_C_Nisasi", &FF_C_Nisasi);
	CK_ACT_AddColFunction("FF_C_Obelisk", &FF_C_Obelisk);
	CK_ACT_AddColFunction("FF_C_PestFly", &FF_C_PestFly);
	CK_ACT_AddColFunction("FF_C_Poleknawel", &FF_C_Poleknawel);
	CK_ACT_AddColFunction("FF_C_Roots", &FF_C_Roots);
	CK_ACT_AddColFunction("FF_C_Shrubbery", &FF_C_Shrubbery);
	CK_ACT_AddColFunction("FF_C_Slug", &FF_C_Slug);
	CK_ACT_AddColFunction("FF_C_SlugSlime", &FF_C_SlugSlime);
	CK_ACT_AddColFunction("FF_C_Squashable", &FF_C_Squashable);
	CK_ACT_AddColFunction("FF_C_TantacleNest", &FF_C_TantacleNest);
	CK_ACT_AddColFunction("FF_C_Tentacle", &FF_C_Tentacle);
	CK_ACT_AddColFunction("FF_C_TinyYeit", &FF_C_TinyYeit);
	CK_ACT_AddColFunction("FF_C_Warble", &FF_C_Warble);
	CK_ACT_AddColFunction("FF_C_XkyLick", &FF_C_XkyLick);
	CK_ACT_AddColFunction("FF_C_XkyLickAttack", &FF_C_XkyLickAttack);
	CK_ACT_AddColFunction("FF_C_Yeti", &FF_C_Yeti);
	CK_ACT_AddColFunction("FF_C_YetiCloud", &FF_C_YetiCloud);
	CK_ACT_AddColFunction("FF_PageContact", &FF_PageContact);
	CK_ACT_AddColFunction("FF_PushKeenX", &FF_PushKeenX);
	CK_ACT_AddColFunction("FF_WormouthBiteContact", &FF_WormouthBiteContact);
	CK_ACT_AddColFunction("FF_WormouthLookContact", &FF_WormouthLookContact);
}
