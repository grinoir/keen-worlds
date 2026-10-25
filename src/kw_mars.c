/* Marooned on Mars - Galaxy, by Dr. Colossus, fixes by K1n9_Duk3.
 * Native implementation of the behavior changes in pat.pat (2020-09-21).
 * The converter preserves the mod's action tables; no DOS code is executed.
 * This integration code is GPL-2.0-or-later, like Omnispeak.
 */
#include "kw_mars.h"
#include "ck_def.h"
#include "ck_act.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "ck5_ep.h"
#include "ck_game.h"
#include "ck_text.h"
#include "kw_world.h"
#include "id_fs.h"
#include "id_ca.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_us.h"
#include "id_vl.h"
#include "id_vh.h"
#include "id_in.h"
#include <stdio.h>
#include <stdlib.h>

void CK_PhysPushX(CK_object *a, CK_object *b);
void CK5_RoboCol(CK_object *a, CK_object *b);
void CK_TurretShotDraw(CK_object *obj);
void CK_KeenColFunc(CK_object *a, CK_object *b);
void CK_MapLevelMarkAsDone(void);
void CK_EndingPurge(void);
extern int ck_pogoTimer;
bool KW_DebugWait(void);
void CK_TurretShoot(CK_object *obj);

bool MARS_IsActive(void) { return KW_CurrentWorld()==KW_MARS; }

void MARS_FilterInput(void)
{
 if (MARS_IsActive() && !ck_gameState.ep.ck5.securityCard) {
  ck_keenState.pogoIsPressed=ck_keenState.pogoWasPressed=false;
  ck_pogoTimer=0;
 }
}

void MARS_SpawnMine(int x, int y)
{
 CK_object *obj=CK_GetNewObj(false);
 obj->type=CT5_Mine; obj->active=OBJ_ACTIVE; obj->clipped=CLIP_not;
 obj->posX=RF_TileToUnit(x); obj->posY=RF_TileToUnit(y)+CK_INT(CK5_MineSpawnYOffset,-0x1F1);
 CK_SetAction(obj,CK_ACTION(CK5_ACT_Mine0));
}

static void MARS_MineSound(CK_object *obj)
{ (void)obj; SD_PlaySound(CK_SOUNDNUM(SOUND_MINEEXPLODE)); }

static void MARS_Collect(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) CK_SetAction2(obj,CK_ACTION(CK5_ACT_Master2));
}

static void MARS_RockCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) CK_PhysPushY(other,obj);
 else if (other->type==CT_Stunner) {
  CK_ShotHit(other);
  CK_SetAction2(obj,CK_ACTION(CK5_ACT_Spindred1));
 }
}

static void MARS_AmptonCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) CK_PhysPushX(other,obj);
 else if (other->type==CT_Stunner) {
  // user2 starts at zero. The fifth shot reaches -5, as in the patch.
  if (--obj->user2<=-5) {
   SD_PlaySound(22);
   CK_StunCreature(obj,other,CK_ACTION(CK5_ACT_AmptonStunned0));
  } else { obj->user3=1; CK_ShotHit(other); }
 }
}

static void MARS_RoboCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) CK_PhysPushX(other,obj);
 else CK5_RoboCol(obj,other);
}

static void MARS_YorpStunCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Stunner)
  CK_StunCreature(obj,other,CK_ACTION(CK5_ACT_KorathStun0));
}

static void MARS_YorpCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) {
  unsigned action=other->currentAction ? other->currentAction->compatDosPointer : 0;
  if (action>=0x0FEA && action<=0x11AC) {
   CK_SetAction2(obj,CK_ACTION(CK5_ACT_MineExplode0));
   obj->user1=0;
   SD_PlaySound(36);
  } else if (other->currentAction && (other->currentAction->velY || other->currentAction->velX)) {
   CK_PhysPushX(other,obj);
  }
 } else MARS_YorpStunCol(obj,other);
}

static void MARS_YorpStunThink(CK_object *obj)
{
 if (++obj->user1>=20) obj->currentAction=CK_ACTION(CK5_ACT_SpirogripSpin7);
}

static void MARS_YorpLand(CK_object *obj)
{
 if (obj->bottomTI || obj->topTI) obj->velY=0;
 if (obj->leftTI || obj->rightTI) obj->velX=0;
 if (obj->topTI) {
  if (obj->currentAction->next) CK_SetAction2(obj,obj->currentAction->next);
  else { CK_RemoveObj(obj); return; }
 }
 RF_AddSpriteDraw(&obj->sde,obj->posX,obj->posY,obj->gfxChunk,false,obj->zLayer);
}

static void MARS_CommanderCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) CK_KillKeen();
 else if (other->type==CT_Stunner) {
  if (--obj->user2) { obj->user3=2; obj->visible=true; CK_ShotHit(other); }
  else { obj->velX=obj->velY=0; CK_StunCreature(obj,other,CK_ACTION(CK5_ACT_ShocksundStun0)); }
 } else if (other->currentAction==CK_ACTION(CK5_ACT_Spindred1)) {
  obj->velX=obj->velY=0;
  obj->currentAction=CK_ACTION(CK5_ACT_ShocksundStun0);
 }
}

static void MARS_IceCol(CK_object *obj, CK_object *other)
{
 if (other->type!=CT_Player || ck_invincibilityTimer || ck_godMode) return;
 CK_SetAction2(other,CK_ACTION(KW_ACT_New014D));
 SD_PlaySound(27);
 other->velY=-48; other->velX=16;
 other->clipped=CLIP_normal; other->zLayer=1;
 CK_SetAction2(obj,CK_ACTION(CK_ACT_turretShotHit1));
}

static void MARS_ShellyCol(CK_object *obj, CK_object *other)
{
 if (other->type==CT_Player) CK_PhysPushX(other,obj);
 else if (other->type==CT_Stunner) {
  CK_ShotHit(other);
  CK_SetAction2(obj,obj->topTI ? CK_ACTION(CK5_ACT_ShellyDieGround) : CK_ACTION(CK5_ACT_ShellyDieAir));
 }
 // Enemy shots leave the butler in its current state (K1n9_Duk3's fix).
}

static void MARS_ShellyWait(CK_object *obj)
{
 if (ck_keenObj->clipRects.unitY1<obj->clipRects.unitY2) return;
 int dx=(int)ck_keenObj->clipRects.unitXmid-obj->clipRects.unitXmid;
 if ((obj->xDirection==1 && dx>=0x600 && dx<=0x900) ||
     (obj->xDirection==-1 && dx<=-0x600 && dx>=-0xB00)) {
  obj->velX=obj->xDirection==1 ? 32 : -33;
  obj->velY=-64;
  obj->currentAction=CK_ACTION(CK5_ACT_ShellyDive0);
  ck_nextX=ck_nextY=0;
 }
}

static void MARS_StatueCol(CK_object *obj, CK_object *other)
{
 if (other->type!=CT_Player || ca_mapOn==15) return;
 const char *text=ca_mapOn==6 ? "The teleporter in\nthe ice will send\nyou to the dark\nside of Mars." :
  ca_mapOn==9 ? "There's a hidden city.\n\nLook in the south!" :
  ca_mapOn==2 ? "It is too bad that\nyou cannot read the\nStandard Galactic\nAlphabet, human." :
  "You will need a\nRaygun in the end,\nbut not to shoot\nthe Vorticon...";
 int picture=ca_mapOn==6 ? 46 : ca_mapOn==9 ? 44 : ca_mapOn==2 ? 47 : 45;
 CA_CacheGrChunk(picture); CA_CacheGrChunk(91);
 SD_MusicOff(); SD_PlaySound(3);
 VL_FixRefreshBuffer(); US_CenterWindow(26,8);
 int x=US_GetWindowX(), y=US_GetWindowY(), width=US_GetWindowW();
 VH_DrawBitmap(x,y,picture);
 US_SetWindowX(x+48);US_SetWindowW(width-48);
 US_SetPrintX(x+48); US_SetPrintY(y+12); US_Print(text);
 US_SetWindowX(x);US_SetWindowW(width);
 VH_UpdateScreen(); IN_ClearKeysDown();
 if (!KW_DebugWait()) IN_WaitButton();
 RF_ForceRefresh(); RF_Refresh();
 StartMusic(ca_mapOn); SD_SetLastTimeCount(SD_GetTimeCount());
 CK_RemoveObj(obj);
}

bool MARS_IceTile(CK_object *keen, int misc)
{
 if (!MARS_IsActive() || misc!=27) return false;
 if (keen->currentAction==CK_ACTION(CK_ACT_keenStanding))
  keen->currentAction=CK_ACTION(CK_ACT_keenRun1);
 return true;
}

void MARS_AfterScan(void)
{
 if (!MARS_IsActive()) return;
 for (CK_object *obj=ck_keenObj->next; obj; obj=obj->next) {
  if (obj->type==CT5_Spindred) { obj->type=CT5_Platform; obj->zLayer=1; }
  if (obj->type==CT5_Volte) obj->zLayer=1;
 }
}

void MARS_MapFrame(void)
{
 // The original patch checks these four completed levels (DOS levelsDone
 // starts at dseg:6F1C), only on the world map. The player still has to
 // collect the parts and reach each level's exit.
 if (MARS_IsActive() && ca_mapOn==0 && ck_gameState.levelsDone[3] &&
     ck_gameState.levelsDone[4] && ck_gameState.levelsDone[8] && ck_gameState.levelsDone[17])
  ck_gameState.levelState=LS_DestroyedQED;
}

void MARS_SetupFunctions(void)
{
 CK_ACT_AddFunction("MARS_MineSound",MARS_MineSound);
 CK_ACT_AddFunction("MARS_YorpStunThink",MARS_YorpStunThink);
 CK_ACT_AddFunction("MARS_YorpLand",MARS_YorpLand);
 CK_ACT_AddFunction("MARS_ShellyWait",MARS_ShellyWait);
 CK_ACT_AddColFunction("MARS_Collect",MARS_Collect);
 CK_ACT_AddColFunction("MARS_RockCol",MARS_RockCol);
 CK_ACT_AddColFunction("MARS_AmptonCol",MARS_AmptonCol);
 CK_ACT_AddColFunction("MARS_RoboCol",MARS_RoboCol);
 CK_ACT_AddColFunction("MARS_YorpCol",MARS_YorpCol);
 CK_ACT_AddColFunction("MARS_YorpStunCol",MARS_YorpStunCol);
 CK_ACT_AddColFunction("MARS_CommanderCol",MARS_CommanderCol);
 CK_ACT_AddColFunction("MARS_IceCol",MARS_IceCol);
 CK_ACT_AddColFunction("MARS_ShellyCol",MARS_ShellyCol);
 CK_ACT_AddColFunction("MARS_StatueCol",MARS_StatueCol);
}

// Developer check using the real engine, converted data, object pool and
// save loader. Run only with /MARSTEST and an isolated /USERPATH.
#define MARS_CHECK(test) do { if (!(test)) QuitF("Mars test failed at line %d: %s",__LINE__,#test); } while (0)
void MARS_SelfTest(void)
{
 puts("Mars test: load native data"); fflush(stdout);
 KW_SwitchWorld(KW_SHIP);
 CK_NewGame(); ck_gameState.difficulty=D_Normal;
 KW_SwitchWorld(KW_MARS);
 ck_gameState.currentLevel=1; CK_LoadLevel(true,true);
 MARS_CHECK(MARS_IsActive() && ck_gameState.numShots==0);
 MARS_CHECK(KW_CanReturnToCabin());
 puts("Mars test: items and collisions"); fflush(stdout);
 // Cache the few sprites these collision fixtures use, independent of
 // which enemies are naturally present in Border Town.
 const int chunks[]={203,294,303,306,307,402,412,424,108,142};
 for (unsigned i=0;i<sizeof(chunks)/sizeof(chunks[0]);++i) CA_CacheGrChunk(chunks[i]);
 ck_keenState.pogoIsPressed=ck_keenState.pogoWasPressed=true;
 ck_pogoTimer=9; MARS_FilterInput();
 MARS_CHECK(!ck_keenState.pogoIsPressed && !ck_pogoTimer);
 CK_object *item=CK_GetNewObj(false);
 item->posX=ck_keenObj->posX; item->posY=ck_keenObj->posY; item->clipped=CLIP_not;
 item->type=CT5_Item; item->user1=12;
 CK_KeenColFunc(ck_keenObj,item);
 MARS_CHECK(ck_gameState.ep.ck5.securityCard==1);
 ck_keenState.pogoIsPressed=true; MARS_FilterInput();
 MARS_CHECK(ck_keenState.pogoIsPressed);
 int lives=ck_gameState.numLives;
 item->type=CT5_Item;item->user1=10;
 CK_KeenColFunc(ck_keenObj,item);
 MARS_CHECK(ck_gameState.numCentilife==1 && ck_gameState.numLives==lives);
 puts("Mars test: Garg and Yorp"); fflush(stdout);
 CK_object *enemy=CK_GetNewObj(false), *shot=CK_GetNewObj(false);
 enemy->posX=ck_keenObj->posX; enemy->posY=ck_keenObj->posY;
 enemy->clipped=CLIP_not;
 shot->posX=ck_keenObj->posX; shot->posY=ck_keenObj->posY;shot->clipped=CLIP_not;
 enemy->type=CT5_Ampton; enemy->user2=0;
 for (int i=0;i<5;++i) {
  shot->type=CT_Stunner; MARS_AmptonCol(enemy,shot);
  MARS_CHECK(i==4 ? enemy->type==CT5_StunnedCreature : enemy->type==CT5_Ampton);
 }
 enemy->type=CT5_Korath;
 CK_SetAction2(ck_keenObj,CK_ACTION(CK_ACT_keenJump1));
 MARS_YorpCol(enemy,ck_keenObj);
 MARS_CHECK(enemy->currentAction==CK_ACTION(CK5_ACT_MineExplode0));
 for (int i=0;i<20;++i) MARS_YorpStunThink(enemy);
 MARS_CHECK(enemy->currentAction==CK_ACTION(CK5_ACT_SpirogripSpin7));
 shot->type=CT_Stunner; MARS_YorpStunCol(enemy,shot);
 MARS_CHECK(enemy->currentAction==CK_ACTION(CK5_ACT_KorathStun0));
 puts("Mars test: commander and ice"); fflush(stdout);
 enemy->type=CT5_Platform;shot->type=CT_Stunner;
 MARS_RockCol(enemy,shot);
 MARS_CHECK(enemy->currentAction==CK_ACTION(CK5_ACT_Spindred1));
 CK_object *commander=CK_GetNewObj(false);
 commander->posX=ck_keenObj->posX; commander->posY=ck_keenObj->posY;commander->clipped=CLIP_not;
 commander->type=CT5_Shocksund; commander->user2=99;
 MARS_CommanderCol(commander,enemy);
 MARS_CHECK(commander->currentAction==CK_ACTION(CK5_ACT_ShocksundStun0));
 ck_invincibilityTimer=0; ck_godMode=false;
 MARS_IceCol(shot,ck_keenObj);
 MARS_CHECK(ck_keenObj->currentAction==CK_ACTION(KW_ACT_New014D));
 MARS_CHECK(ck_keenObj->clipped==CLIP_normal && ck_keenObj->velY==-48);
 CK_object *turret=CK_GetNewObj(false);
 turret->posX=ck_keenObj->posX;turret->posY=ck_keenObj->posY;
 for (int dir=0;dir<4;++dir) {
  CK_object *tail=ck_keenObj;while (tail->next) tail=tail->next;
  turret->user1=dir;CK_TurretShoot(turret);
  CK_object *ice=tail->next;MARS_CHECK(ice);
  MARS_CHECK(ice->velX==(dir==1 ? 64 : dir==3 ? -64 : 0));
  MARS_CHECK(ice->velY==(dir==2 ? 64 : -64));
 }
 // Reload the real level before serializing, retaining the pogo and parts.
 puts("Mars test: save and load"); fflush(stdout);
 ck_gameState.currentLevel=1; CK_LoadLevel(true,true);
 MARS_CHECK(ck_gameState.ep.ck5.securityCard==1 && ck_gameState.numCentilife==1);
 CK_SetAction2(ck_keenObj,CK_ACTION(KW_ACT_New014D));
 FS_File fp=FS_CreateUserFile("MARS-TEST.SAV"); MARS_CHECK(FS_IsFileValid(fp));
 MARS_CHECK(CK_SaveGame(fp)); FS_CloseFile(fp);
 ck_gameState.numCentilife=0; ck_gameState.ep.ck5.securityCard=0;
 fp=FS_OpenUserFile("MARS-TEST.SAV"); MARS_CHECK(FS_IsFileValid(fp));
 MARS_CHECK(CK_LoadGame(fp,false)); FS_CloseFile(fp);
 MARS_CHECK(ck_gameState.numCentilife==1 && ck_gameState.ep.ck5.securityCard==1);
 MARS_CHECK(ck_keenObj->currentAction==CK_ACTION(KW_ACT_New014D));
 ck_gameState.currentLevel=0; CK_LoadLevel(true,true);
 puts("Mars test: map and ending"); fflush(stdout);
 ck_gameState.levelsDone[8]=1;
 CA_SetTileAtPos(10,10,2,0x0108); CA_SetTileAtPos(11,10,2,0xC008);
 CK_MapLevelMarkAsDone();
 MARS_CHECK(CA_TileAtPos(10,10,2)==0x0108 && CA_TileAtPos(11,10,2)==0);
 ck_gameState.levelsDone[3]=ck_gameState.levelsDone[4]=1;
 ck_gameState.levelsDone[17]=0;ck_gameState.levelState=LS_Playing;
 MARS_MapFrame();MARS_CHECK(ck_gameState.levelState==LS_Playing);
 ck_gameState.levelsDone[17]=1;MARS_MapFrame();
 MARS_CHECK(ck_gameState.levelState==LS_DestroyedQED);
 if (getenv("KW_AUTOWAIT")) {
  const int levels[]={2,6,9,1};
  for (unsigned i=0;i<sizeof(levels)/sizeof(levels[0]);++i) {
   ck_gameState.currentLevel=levels[i];CK_LoadLevel(true,true);
   CK_object *statue=CK_GetNewObj(false);
   MARS_StatueCol(statue,ck_keenObj);
  }
 }
 if (getenv("KW_AUTOFINALE")) {
  bool completed[KW_NUM_WORLDS];
  for (int w=0;w<KW_NUM_WORLDS;++w) completed[w]=KW_IsWorldCompleted(w);
  CK_EndingPurge();VL_FixRefreshBuffer();help_endgame();KW_AfterWorldEnding();
  MARS_CHECK(KW_CurrentWorld()==KW_SHIP && ck_gameState.levelState==LS_Playing);
  for (int w=0;w<KW_NUM_WORLDS;++w)
   if (w!=KW_MARS) MARS_CHECK(completed[w]==KW_IsWorldCompleted(w));
 }
 KW_TestArcadeSession();
 puts("PASS: Mars pogo ownership, parts, five-hit Garg, Yorp stomp/recovery/shooting, falling rock/commander, freezing, save/load and teleporter/ending rules");
}
