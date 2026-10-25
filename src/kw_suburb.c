/* Suburb Shenanigans, Bubbatom / Levellass / K1n9_Duk3, v1.2.
 * Native translation of the audited patches; GPL-2.0-or-later.
 * Original patches and notices remain beside the player's unchanged maps.
 */
#include "kw_suburb.h"
#include "kw_world.h"
#include "kw_habitat.h"
#include "kw_music.h"
#include "kw_skip.h"
#include "ck_def.h"
#include "ck_act.h"
#include "ck_game.h"
#include "ck_play.h"
#include "ck5_ep.h"
#include "ck_phys.h"
#include "ck_text.h"
#include "id_ca.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_in.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_ti.h"
#include "id_vl.h"
#include <stdio.h>
#include <stdlib.h>

bool SS_IsActive(void) {return KW_CurrentWorld()==KW_SUBURB;}
// A persistent mission state separate from the consumable security-door
// card. Both words already belong to Keen 5's saved/per-world item state.
int SS_BagState(void) {return (uint16_t)ck_gameState.ep.ck5.word_4729C&3;}
static void SS_SetBagState(int state)
{
 ck_gameState.ep.ck5.word_4729C=(ck_gameState.ep.ck5.word_4729C&~3)|state;
 ck_gameState.ep.ck5.securityCard=state;
}

static void SS_Popup(const char *clip,const char *text)
{
 if(getenv("KW_AUTOTALK") && !getenv("KW_AUTOWAIT")) return;
 CA_UpLevel();CA_CacheGrChunk(90);CA_CacheGrChunk(91);
 VL_FixRefreshBuffer();US_CenterWindow(30,8);
 VHB_DrawBitmap(US_GetWindowX()+US_GetWindowW()-48,US_GetWindowY(),90);
 US_SetWindowW(US_GetWindowW()-48);
 US_SetPrintY(US_GetWindowY()+12);US_CPrint(text);VH_UpdateScreen();
 KW_SayLine(KW_SUBURB,clip);KW_WaitDialogue(0);KW_StopVoice();
 CA_DownLevel();RF_ForceRefresh();
}

static CK_object *SS_Spawn(int x,int y,const char *action,int type)
{
 CK_object *ob=CK_GetNewObj(false);ob->type=type;
 ob->active=OBJ_ALWAYS_ACTIVE;ob->clipped=CLIP_not;
 ob->posX=RF_TileToUnit(x);ob->posY=RF_TileToUnit(y);
 CK_SetAction(ob,CK_GetActionByName(action));
 return ob;
}

static bool SS_CanTransition(CK_object *other)
{
 return other==ck_keenObj && ck_gameState.levelState==LS_Playing &&
  other->currentAction!=CK_ACTION(CK_ACT_keenDie0) && other->currentAction!=CK_ACTION(CK_ACT_keenDie1);
}
static const int ss_chainStarts[]={1,3,6,8,16,18,20,22};
bool SS_MapEntry(int *level)
{
 if(!SS_IsActive()) return true;
 if(*level==13 && SS_BagState()!=2 && !KW_IsWorldCompleted(KW_SUBURB)) {
  SS_Popup("SS_NEEDBAG.RAW","My school bag\nisn't here.\nI'm not getting on the\nbus until I've found it!");
  return false;
 }
 for(int i=0;i<8;++i)
  if(*level==ss_chainStarts[i] && !ck_gameState.levelsDone[*level] &&
     (ck_gameState.ep.ck5.word_4729C&(1<<(i+2)))) {++*level;break;}
 return true;
}

static void SS_CheckpointCol(CK_object *ob,CK_object *other)
{
 (void)ob;
 if(SS_CanTransition(other) && ca_mapOn>0 && ca_mapOn<23) {
  for(int i=0;i<8;++i) if(ca_mapOn==ss_chainStarts[i])
   ck_gameState.ep.ck5.word_4729C|=1<<(i+2);
  ck_gameState.currentLevel=ca_mapOn+1;
  ck_gameState.levelState=LS_SSCheckpoint;
 }
}
static void SS_FinishCol(CK_object *ob,CK_object *other)
{
 (void)ob;
 if(SS_CanTransition(other)) {
  ck_gameState.levelsDone[ca_mapOn-1]=1;
  ck_gameState.levelState=LS_LevelComplete;
 }
}
static void SS_MomCol(CK_object *ob,CK_object *other)
{
 (void)ob;
 if(other==ck_keenObj && !SS_BagState()) {
  SS_SetBagState(1);
  SS_Popup("SS_LEAVE.RAW","I'm off now!\nI'll see you later\ntonight. Bye!");
 }
}
static void SS_BagCol(CK_object *ob,CK_object *other)
{
 if(other!=ck_keenObj || SS_BagState()==2 || ck_gameState.levelState!=LS_Playing) return;
 SS_SetBagState(2);ob->currentAction=NULL;
 ck_gameState.levelsDone[3]=ck_gameState.levelsDone[4]=1;
 ck_gameState.levelState=LS_SSBag;
}
static void SS_BlockCol(CK_object *ob,CK_object *other)
{
 if(other!=ck_keenObj || SS_BagState()==2) return;
 // The invisible strip physically stops a rightward attempt, including
 // controllers. Releasing/repressing is needed before repeating the card.
 if(ck_inputFrame.xDirection!=IN_motion_Right) {ob->user1=0;return;}
 other->posX=ob->posX-0x100;CK_PhysUpdateNormalObj(other);
 if(!ob->user1) {
  ob->user1=1;
  SS_Popup("SS_NEEDBAG.RAW","My school bag\nisn't here.\nI'm not getting on the\nbus until I've found it!");
 }
}

static void SS_DoorOpen(CK_object *ob)
{
 uint16_t tiles[16];
 if(ob->posX+4>CA_GetMapWidth() || ob->posY+4>CA_GetMapHeight()) return;
 for(int y=0;y<4;++y) for(int x=0;x<4;++x)
  tiles[y*4+x]=CA_TileAtPos(ob->posX+x,ob->posY+y,1)-4;
 RF_ReplaceTiles(tiles,1,ob->posX,ob->posY,4,4);
}

static void SS_Car(CK_object *ob,bool vertical)
{
 if(ck_nextX || ck_nextY) return;
 int dx=ob->xDirection*12*SD_GetSpriteSync(),dy=ob->yDirection*12*SD_GetSpriteSync();
 int old=vertical?ob->clipRects.tileY2:ob->clipRects.tileX2;
 int edge=(vertical?ob->clipRects.unitY2+dy:ob->clipRects.unitX2+dx);
 int tile=edge>>8;
 int x=vertical?ob->clipRects.tileX1:tile,y=vertical?tile:ob->clipRects.tileY1;
 if((vertical || tile!=old) && x>=0 && y>=0 && x<CA_GetMapWidth() && y<CA_GetMapHeight() &&
    CA_TileAtPos(x,y,2)==(vertical?116:115)) {
  if(vertical) {ob->xDirection=ob->yDirection==-1?1:-1;ob->yDirection=0;
   ob->currentAction=CK_ACTION(CK5_ACT_Slice0);dy-=edge&255;}
  else {ob->yDirection=ob->xDirection==1?1:-1;ob->xDirection=0;
   ob->currentAction=CK_ACTION(SS_ACT_CarVertical);dx-=edge&255;}
 }
 ck_nextX=dx;ck_nextY=dy;
}
static void SS_CarHorizontal(CK_object *ob) {SS_Car(ob,false);}
static void SS_CarVertical(CK_object *ob) {SS_Car(ob,true);}
static void SS_ChildMove(CK_object *ob)
{
 if(!ck_nextX && !ck_nextY) {ck_nextX=ob->xDirection*32*SD_GetSpriteSync();ck_nextY=ob->yDirection*32*SD_GetSpriteSync();}
}
static void SS_ChildCol(CK_object *ob,CK_object *other)
{
 if(other==ck_keenObj) CK_KillKeen();
 else if(other->type==CT_Stunner) CK_StunCreature(ob,other,CK_ACTION(CK5_ACT_VolteStunned));
}

static void SS_HiveCol(CK_object *ob,CK_object *other)
{
 if(other!=ck_keenObj && other->type!=CT_Stunner && other->type!=CT5_EnemyShot) return;
 if(other->type==CT_Stunner) CK_ShotHit(other);
 else if(other->type==CT5_EnemyShot) CK_RemoveObj(other);
 SD_PlaySound(CK_SOUNDNUM(SOUND_SHELLYEXPLODE));
 CK_SetAction2(ob,CK_ACTION(CK5_ACT_ShellyDieGround));
 ob->clipped=CLIP_not;ob->topTI=ob->bottomTI=0;
 CK_object *smoke=CK_GetNewObj(true);
 if(smoke) {smoke->posX=ob->posX;smoke->posY=ob->posY;smoke->type=CT_Friendly;
  smoke->clipped=CLIP_not;CK_SetAction(smoke,CK_ACTION(CK5_ACT_ShellyExplode0));}
}
static void SS_SpawnBees(CK_object *ob)
{
 CK_object *bee=CK_GetNewObj(true);
 if(bee) {bee->type=CT5_Sphereful;bee->active=OBJ_ACTIVE;bee->clipped=CLIP_normal;
  bee->posX=ob->posX;bee->posY=ob->posY;bee->velX=7;bee->velY=0;
  CK_SetAction(bee,CK_ACTION(CK5_ACT_Sphereful0));}
 CK_object *bits=CK_GetNewObj(true);
 if(bits) {bits->type=CT_Friendly;bits->posX=ob->posX;bits->posY=ob->posY;bits->velX=-32;bits->velY=-24;
  CK_SetAction(bits,CK_ACTION(CK5_ACT_ShellyBits1));}
}
static void SS_BeeDraw(CK_object *ob)
{
 if(ob->leftTI || ob->rightTI) ob->velX=-ob->velX;
 if(ob->bottomTI) ob->velY=-ob->velY;
 if(ob->topTI) {
  ob->velY=-ob->velY+(ck_keenObj->posY<ob->posY?-1:1);
  if(ob->velY>-4) ob->velY=-4;else if(ob->velY<-12) ob->velY=-12;
 }
 RF_AddSpriteDraw(&ob->sde,ob->posX,ob->posY,ob->gfxChunk,0,ob->zLayer);
 static const uint16_t pos[]={384,369,328,265,192,119,58,15,0,15,58,119,192,265,328,369};
 int time=(SD_GetLastTimeCount()>>3)&15,frame=433+(time>>2),layer=time>=8?2:0;
 RF_AddSpriteDrawUsing16BitOffset(&ob->user1,ob->posX+pos[time],ob->posY+pos[time],frame,0,layer);
 RF_AddSpriteDrawUsing16BitOffset(&ob->user2,ob->posX+384-pos[time],ob->posY+pos[time],frame,0,layer);
 time=(time+8)&15;layer=time>=8?2:0;
 RF_AddSpriteDrawUsing16BitOffset(&ob->user3,ob->posX+pos[time],ob->posY+pos[time],frame,0,layer);
 RF_AddSpriteDrawUsing16BitOffset(&ob->user4,ob->posX+384-pos[time],ob->posY+pos[time],frame,0,layer);
}

static void SS_GardenerThink(CK_object *ob)
{
 if(US_RndT()<16) {ob->xDirection=ob->posX<ck_keenObj->posX?1:-1;ob->currentAction=CK_ACTION(CK5_ACT_KorathWait);}
}
static void SS_GardenerCol(CK_object *ob,CK_object *other)
{
 if(other==ck_keenObj) CK_KillKeen();
 else if(other->type==CT_Stunner) CK_StunCreature(ob,other,CK_ACTION(CK5_ACT_KorathStun0));
}
static void SS_OldManThink(CK_object *ob)
{
 if(US_RndT()<16) ob->currentAction=CK_ACTION(CK5_ACT_AmptonComp0);
}
static void SS_OldManCol(CK_object *ob,CK_object *other)
{
 if(other==ck_keenObj) CK_KillKeen();
 else if(other->type==CT_Stunner) CK_StunCreature(ob,other,CK_ACTION(CK5_ACT_AmptonStunned0));
}
static void SS_OldManShoot(CK_object *ob)
{
 CK_object *shot=CK5_SpawnEnemyShot(ob->posX+(ob->xDirection==1?112:0),ob->posY+400,CK_ACTION(CK_ACT_turretShot1));
 if(shot) {shot->velX=ob->xDirection*60;shot->xDirection=ob->xDirection;SD_PlaySound(29);}
}
static void SS_VacuumCol(CK_object *ob,CK_object *other)
{
 if(other==ck_keenObj && (other->currentAction==CK_ACTION(CK_ACT_keenPogo2) || other->currentAction==CK_ACTION(CK_ACT_keenPogo3))) {
  CK_SetAction2(ob,CK_ACTION(SS_ACT_VacuumDead));SD_PlaySound(33);
 } else if(other->type==CT_Stunner) CK_ShotHit(other);
}
static void SS_VacuumThink(CK_object *ob)
{
 if(US_RndT()<16) {ob->xDirection=ob->posX>ck_keenObj->posX?-1:1;
  ob->user1=1;ob->currentAction=CK_ACTION(CK5_ACT_RoboShoot0);}
}
static void SS_VacuumShoot(CK_object *ob)
{
 if(--ob->user1==0) ob->currentAction=CK_ACTION(CK5_ACT_Robo0);
 CK_object *shot=CK5_SpawnEnemyShot(ob->posX+(ob->xDirection==1?293:0),ob->posY+CK_INT(CK5_RoboShotYOffset,512),CK_ACTION(CK5_ACT_RoboShot0));
 if(shot) {shot->velX=ob->xDirection*30;shot->velY=0;SD_PlaySound(CK_SOUNDNUM(SOUND_ENEMYSHOOT));}
}
static void SS_BullyShoot(CK_object *ob)
{
 CK_object *shot=CK5_SpawnEnemyShot(ob->posX+(ob->xDirection==1?256:0),ob->posY+64,CK_ACTION(CK5_ACT_BarkShot0));
 if(shot) {shot->velX=ob->xDirection*48;shot->velY=-16;SD_PlaySound(CK_SOUNDNUM(SOUND_ENEMYSHOOT));}
}
static void SS_MortimerShoot(CK_object *ob)
{
 CK_object *shot=CK5_SpawnEnemyShot(ob->posX+(ob->xDirection==1?112:0),ob->posY+64,CK_ACTION(CK5_ACT_MasterBall0));
 if(shot) {shot->velX=ob->xDirection*60;SD_PlaySound(CK_SOUNDNUM(SOUND_ENEMYSHOOT));}
}
static void SS_BalloonCol(CK_object *ob,CK_object *other)
{
 if(other==ck_keenObj) {
  if(ck_invincibilityTimer) return;
  other->zLayer=1;other->clipped=CLIP_normal;other->yDirection=1;other->velX=0;
  ck_keenState.platform=NULL;ck_keenState.keenSliding=false;
  CK_SetAction2(other,CK_ACTION(CK_ACT_keenLookDown5));
  CK_SetAction2(ob,CK_ACTION(CK5_ACT_BarkShotDie0));
 } else if(other->type==CT_Stunner) {CK_ShotHit(other);CK_SetAction2(ob,CK_ACTION(CK5_ACT_BarkShotDie0));}
}
static void SS_StunFallDraw(CK_object *ob)
{
 if(ob->topTI) {ob->velY=0;CK_SetAction2(ob,CK_ACTION(SS_ACT_Stunned));}
 if(ob->leftTI || ob->rightTI) ob->velX=0;
 RF_AddSpriteDraw(&ob->sde,ob->posX,ob->posY,ob->gfxChunk,0,ob->zLayer);
}
void CK_BasicDrawFunc4(CK_object *ob);
static void SS_StunStars(CK_object *ob)
{
 CK_BasicDrawFunc4(ob);
 int y=ob->user4==CT5_Ampton?-176:ob->user4==CT5_Volte?-128:ob->user4==CT5_Shikadi?-160:0;
 if(y) RF_AddSpriteDrawUsing16BitOffset(&ob->user3,ob->posX,ob->posY+y,
  ob->user2+CK_CHUNKNUM(SPR_STARS1),false,3);
}

bool SS_ScanObject(int info,int x,int y)
{
 if(!SS_IsActive()) return false;
 if(info==108 || info==109) {
  SS_Spawn(x,y,info==108?"SS_ACT_Checkpoint":"SS_ACT_Finish",CT_Friendly);
  CA_MarkGrChunk(272);return true;
 }
 if(info==70) {
  if(ca_mapOn==0) {if(SS_BagState()!=2) SS_Spawn(x,y,"CK5_ACT_FuseExplosion2",CT_Friendly);}
  else if(ca_mapOn==1) {if(!SS_BagState()) SS_Spawn(x,y,"CK5_ACT_QED",CT_Friendly);}
  else if(SS_BagState()!=2) {SS_Spawn(x,y,"CK5_ACT_FuseExplosion0",CT_Friendly);CA_MarkGrChunk(207);CA_MarkGrChunk(208);}
  CA_MarkGrChunk(272);return true;
 }
 return false;
}

void SS_AfterScan(void)
{
 if(!SS_IsActive()) return;
 for(CK_object *ob=ck_keenObj->next;ob;ob=ob->next) {
  if(ob->type==CT5_SliceStar) ob->clipped=CLIP_not;
  if(ob->type==CT5_Volte) {ob->xDirection=1;ob->yDirection=0;}
  if(ob->type==CT5_Shocksund) CK_SetAction2(ob,CK_ACTION(CK5_ACT_ShocksundJump0));
 }
 // Extra frames/spawn-time cross-creature dependencies in the original patch.
 for(int i=230;i<=241;++i) CA_MarkGrChunk(i);
 for(int i=429;i<=436;++i) CA_MarkGrChunk(i);
 CA_MarkGrChunk(272);
 if(ca_mapOn==13 && !KW_IsWorldCompleted(KW_SUBURB)) ck_gameState.levelState=LS_DestroyedQED;
}
void SS_AfterLoad(void)
{
 if(SS_IsActive() && ca_mapOn==13 && !KW_IsWorldCompleted(KW_SUBURB)) ck_gameState.levelState=LS_DestroyedQED;
}
bool SS_HandleLevelState(void)
{
 if(!SS_IsActive()) return false;
 if(ck_gameState.levelState==LS_SSCheckpoint) return true;
 if(ck_gameState.levelState==LS_SSBag) {
  SS_Popup("SS_GOTBAG.RAW","Hey, what did I tell you?\nI'll have to get my\nbrother back for this!\nNow I can catch the bus!");
  ck_gameState.currentLevel=0;return true;
 }
 return false;
}
bool SS_RevisitStart(void)
{
 if(!SS_IsActive()) return false;
 int oldX=ck_keenObj->posX,oldY=ck_keenObj->posY,oldDirection=ck_keenObj->xDirection;
 for(int y=0;y<CA_GetMapHeight();++y) for(int x=0;x<CA_GetMapWidth();++x)
  if(CA_TileAtPos(x,y,2)==108 || CA_TileAtPos(x,y,2)==109) {
   int facing=x<CA_GetMapWidth()/2?1:-1;
   // Check Billy's actual hitbox near the far-side marker. The marker
   // itself can be in a doorway or at a different height from the floor.
   for(int radius=0;radius<=6;++radius) for(int dy=-radius;dy<=radius;++dy)
    for(int dx=-radius;dx<=radius;++dx) {
     if(abs(dx)!=radius && abs(dy)!=radius) continue;
     int tx=x+facing*2+dx,ty=y+dy;
     if(tx<2 || ty<2 || tx>=CA_GetMapWidth()-3 || ty>=CA_GetMapHeight()-3) continue;
     ck_keenObj->posX=RF_TileToUnit(tx);ck_keenObj->posY=RF_TileToUnit(ty)-241;
     ck_keenObj->xDirection=facing;
     CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_keenStanding));CK_ResetClipRects(ck_keenObj);
     if(!CK_NotStuckInWall(ck_keenObj)) continue;
     for(int foot=ck_keenObj->clipRects.tileX1;foot<=ck_keenObj->clipRects.tileX2;++foot)
      if(TI_ForeTop(CA_TileAtPos(foot,ck_keenObj->clipRects.tileY2+1,1))) return true;
    }
   ck_keenObj->posX=oldX;ck_keenObj->posY=oldY;ck_keenObj->xDirection=oldDirection;
   CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_keenStanding));CK_ResetClipRects(ck_keenObj);
   return false;
  }
 return false;
}

bool SS_IsStoryTrigger(CK_object *ob)
{
 if(!SS_IsActive() || !ob->currentAction) return false;
 CK_ACT_ColFunction col=ob->currentAction->collide;
 return col==SS_CheckpointCol || col==SS_FinishCol || col==SS_MomCol || col==SS_BagCol || col==SS_BlockCol;
}

void SS_SetupFunctions(void)
{
#define THINK(name) CK_ACT_AddFunction(#name,name)
#define COLLIDE(name) CK_ACT_AddColFunction(#name,name)
 THINK(SS_DoorOpen);COLLIDE(SS_MomCol);COLLIDE(SS_BagCol);COLLIDE(SS_BlockCol);
 COLLIDE(SS_CheckpointCol);COLLIDE(SS_FinishCol);THINK(SS_CarHorizontal);THINK(SS_CarVertical);
 THINK(SS_ChildMove);COLLIDE(SS_ChildCol);COLLIDE(SS_HiveCol);THINK(SS_SpawnBees);THINK(SS_BeeDraw);
 THINK(SS_GardenerThink);COLLIDE(SS_GardenerCol);THINK(SS_OldManThink);COLLIDE(SS_OldManCol);THINK(SS_OldManShoot);
 THINK(SS_VacuumThink);COLLIDE(SS_VacuumCol);THINK(SS_VacuumShoot);
 THINK(SS_BullyShoot);THINK(SS_MortimerShoot);COLLIDE(SS_BalloonCol);THINK(SS_StunFallDraw);THINK(SS_StunStars);
#undef THINK
#undef COLLIDE
}

void KW_TestLevelSpritesCached(const char *world,int level);
void KW_TestRunObjects(int frames);
#define SS_CHECK(test) do {if(!(test)) QuitF("Suburb test line %d: %s",__LINE__,#test);} while(0)
static CK_object *SS_Find(const char *action)
{
 CK_action *wanted=CK_GetActionByName(action);
 for(CK_object *ob=ck_keenObj->next;ob;ob=ob->next) if(ob->currentAction==wanted) return ob;
 return NULL;
}
static void SS_TestRoundTrip(void)
{
 int level=ca_mapOn,x=ck_keenObj->posX,y=ck_keenObj->posY,mission=ck_gameState.ep.ck5.word_4729C;
 int score=ck_gameState.keenScore;
 SS_CHECK(KW_TestWriteSave());
 ck_gameState.ep.ck5.word_4729C=0;ck_gameState.keenScore=-10;
 SS_CHECK(KW_TestReadSave());
 SS_CHECK(ca_mapOn==level && ck_keenObj->posX==x && ck_keenObj->posY==y);
 SS_CHECK(ck_gameState.ep.ck5.word_4729C==mission && ck_gameState.keenScore==score);
 ck_gameState.levelState=LS_Playing;
}
static int SS_CountType(int type)
{
 int count=0;for(CK_object *ob=ck_keenObj->next;ob;ob=ob->next) if(ob->type==type) ++count;
 return count;
}
static void SS_TestDeathRetry(void)
{
 int level=ca_mapOn,mission=ck_gameState.ep.ck5.word_4729C;
 ck_gameState.levelState=LS_Playing;ck_invincibilityTimer=0;ck_godMode=false;
 CK_CentreCamera(ck_keenObj);CK_KillKeen();SD_SetSpriteSync(2);
 SS_CHECK(ck_keenObj->currentAction==CK_ACTION(CK_ACT_keenDie0) || ck_keenObj->currentAction==CK_ACTION(CK_ACT_keenDie1));
 for(int frame=0;frame<300 && ck_gameState.levelState==LS_Playing;++frame) CK_RunAction(ck_keenObj);
 SS_CHECK(ck_gameState.levelState==LS_Died);
 // The shared Try Again path reloads currentLevel; the mission word must
 // survive in both halves, including the flag selecting the second half.
 CK_LoadLevel(true,true);ck_gameState.levelState=LS_Playing;ck_invincibilityTimer=0;ck_scrollDisabled=false;ck_godMode=true;
 SS_CHECK(ca_mapOn==level && ck_gameState.ep.ck5.word_4729C==mission);
}
static void SS_TestActors(void)
{
 ck_gameState.currentLevel=10;ck_gameState.levelsDone[10]=0;CK_LoadLevel(true,true);
 ck_gameState.levelState=LS_Playing;
 for(int i=ca_gfxInfoE.offSprites;i<ca_gfxInfoE.offSprites+ca_gfxInfoE.numSprites;++i) CA_CacheGrChunk(i);
 CK_object *child=SS_Spawn(25,10,"CK5_ACT_Volte0",CT5_Volte);
 CK_object *shot=SS_Spawn(25,10,"CK_ACT_keenShot1",CT_Stunner);
 child->user3=200;SS_ChildCol(child,shot);
 SS_CHECK(child->type==CT5_StunnedCreature && !child->user3 && child->user4==CT5_Volte);
 SS_StunStars(child);SS_CHECK(child->user3);SS_TestRoundTrip();
 child=SS_Find("CK5_ACT_VolteStunned");SS_CHECK(child && !child->user3);
 SS_StunStars(child);SS_CHECK(child->user3);
 CK_object *hive=SS_Spawn(25,10,"CK5_ACT_Shelly0",CT5_0x11);
 int bees=SS_CountType(CT5_Sphereful),oldX=ck_keenObj->posX;
 SS_HiveCol(hive,ck_keenObj);SS_CHECK(ck_keenObj->posX==oldX && !hive->currentAction->collide);
 SD_SetSpriteSync(2);
 for(int i=0;i<8 && SS_CountType(CT5_Sphereful)==bees;++i) {ck_nextX=ck_nextY=0;CK_RunAction(hive);}
 SS_CHECK(SS_CountType(CT5_Sphereful)==bees+1);
 CK_object *bee=NULL;for(CK_object *ob=ck_keenObj->next;ob;ob=ob->next) if(ob->type==CT5_Sphereful) bee=ob;
 SS_CHECK(bee && bee->velX==7);
 SS_BeeDraw(bee);SS_CHECK(bee->user1 && bee->user2 && bee->user3 && bee->user4);
 SS_TestRoundTrip();bee=SS_Find("CK5_ACT_Sphereful0");
 SS_CHECK(bee && !bee->user1 && !bee->user2 && !bee->user3 && !bee->user4);SS_BeeDraw(bee);
 SD_SetSpriteSync(2);
 CK_object *balloon=SS_Spawn(25,10,"CK5_ACT_BarkShot0",CT5_EnemyShot);
 ck_keenObj->clipped=CLIP_not;ck_keenObj->velX=50;ck_keenState.keenSliding=true;
 ck_invincibilityTimer=0;SS_BalloonCol(balloon,ck_keenObj);
 SS_CHECK(ck_keenObj->clipped==CLIP_normal && !ck_keenObj->velX && !ck_keenState.keenSliding);
 SS_CHECK(ck_keenObj->currentAction==CK_ACTION(CK_ACT_keenLookDown5));
 ck_keenObj->topTI=1;SS_StunFallDraw(ck_keenObj);SS_CHECK(ck_keenObj->currentAction==CK_ACTION(SS_ACT_Stunned));
 ck_keenObj->posX=RF_TileToUnit(28);ck_keenObj->posY=RF_TileToUnit(14)-241;
 for(int i=0;i<40;++i) {ck_nextX=ck_nextY=0;CK_RunAction(ck_keenObj);}
 SS_CHECK(ck_keenObj->currentAction!=CK_ACTION(SS_ACT_Stunned));
 CK_object *vacuum=SS_Spawn(25,10,"CK5_ACT_Robo0",CT5_Robo);
 vacuum->xDirection=1;vacuum->user1=1;
 int shots=SS_CountType(CT5_EnemyShot);SS_VacuumShoot(vacuum);
 SS_CHECK(SS_CountType(CT5_EnemyShot)==shots+1 && vacuum->currentAction==CK_ACTION(CK5_ACT_Robo0));
 CK_object *vacuumShot=SS_Find("CK5_ACT_RoboShot0");
 SS_CHECK(vacuumShot && vacuumShot->velX==30 && !vacuumShot->velY);
 CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_keenStanding));SS_VacuumCol(vacuum,ck_keenObj);
 SS_CHECK(vacuum->currentAction==CK_ACTION(CK5_ACT_Robo0) && ck_gameState.levelState==LS_Playing);
 CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_keenPogo2));SS_VacuumCol(vacuum,ck_keenObj);
 SS_CHECK(vacuum->currentAction==CK_ACTION(SS_ACT_VacuumDead));
 // The car uses the authored 115/116 turn markers, clockwise in all directions.
 for(int direction=0;direction<4;++direction) {
  CK_object car={0};bool vertical=direction&1;
  car.xDirection=vertical?0:(direction==0?1:-1);car.yDirection=vertical?(direction==1?1:-1):0;
  car.clipRects.tileX1=car.clipRects.tileY1=8;
  car.clipRects.unitX2=car.clipRects.unitY2=RF_TileToUnit(20)+12;
  car.clipRects.tileX2=car.clipRects.tileY2=20;
  int edge=20+(direction<2?0:-1),x=vertical?8:edge,y=vertical?edge:8;
  uint16_t old=CA_TileAtPos(x,y,2);CA_SetTileAtPos(x,y,2,vertical?116:115);
  // For a rightward crossing, put the previous edge just in the prior tile.
  if(direction==0) {car.clipRects.unitX2=RF_TileToUnit(20)-12;car.clipRects.tileX2=19;}
  ck_nextX=ck_nextY=0;SS_Car(&car,vertical);CA_SetTileAtPos(x,y,2,old);
  SS_CHECK(car.xDirection==(direction==1?-1:direction==3?1:0));
  SS_CHECK(car.yDirection==(direction==0?1:direction==2?-1:0));
 }
 puts("PASS: child stuns, hive/bee saves, balloon recovery, vacuum shots/pogo and all four car turns");fflush(stdout);
}
void SS_SelfTest(void)
{
 KW_SwitchWorld(KW_SUBURB);CK_NewGame();ck_gameState.difficulty=D_Hard;ck_godMode=true;
 for(int level=0;level<=23;++level) {
  printf("Suburb test: map %d\n",level);fflush(stdout);
  ck_gameState.currentLevel=level;ck_gameState.levelState=LS_Playing;CK_LoadLevel(true,true);
  KW_TestLevelSpritesCached("suburb",level);
  ck_gameState.levelState=LS_Playing;KW_TestRunObjects(90);
 }
 puts("PASS: Suburb all 24 slots load, cache and run native actors on hard difficulty");
 for(int i=0;i<8;++i) {
  int first=ss_chainStarts[i],second=first+1;
  ck_gameState.ep.ck5.word_4729C=1;
  ck_gameState.levelsDone[first]=ck_gameState.levelsDone[second]=0;
  ck_gameState.currentLevel=first;ck_gameState.levelState=LS_Playing;CK_LoadLevel(true,true);
  SS_TestRoundTrip();
  SS_TestDeathRetry();
  CK_object *checkpoint=SS_Find("SS_ACT_Checkpoint");SS_CHECK(checkpoint);
  SS_CheckpointCol(checkpoint,ck_keenObj);
  SS_CHECK(ck_gameState.currentLevel==second && ck_gameState.levelState==LS_SSCheckpoint);
  SS_CHECK(SS_HandleLevelState());CK_LoadLevel(true,true);ck_gameState.levelState=LS_Playing;
  SS_TestRoundTrip();
  SS_TestDeathRetry();
  int entry=first;SS_CHECK(SS_MapEntry(&entry) && entry==second && SS_BagState()==1);
  SS_CHECK(KW_TestWriteSave());
  KW_SwitchWorld(KW_SHIP);
  SS_CHECK(KW_TestReadSave());
  SS_CHECK(SS_IsActive() && ca_mapOn==second && SS_BagState()==1);
  entry=first;SS_CHECK(SS_MapEntry(&entry) && entry==second);
  ck_gameState.levelState=LS_Playing;
  if(first>=6) {
   CK_object *finish=SS_Find("SS_ACT_Finish");SS_CHECK(finish);
   SS_FinishCol(finish,ck_keenObj);
   SS_CHECK(ck_gameState.levelsDone[first] && ck_gameState.levelState==LS_LevelComplete);
   ck_gameState.levelsDone[second]=1;
  }
 }
 puts("PASS: all eight checkpoint chains save/load and death/retry in both halves, resume after cabin travel, and complete paired progress");fflush(stdout);
 ck_gameState.ep.ck5.word_4729C=0;int bus=13;SS_CHECK(!SS_MapEntry(&bus));
 ck_gameState.currentLevel=1;ck_gameState.levelState=LS_Playing;CK_LoadLevel(true,true);
 CK_object *mom=SS_Find("CK5_ACT_QED");SS_CHECK(mom);SS_MomCol(mom,ck_keenObj);SS_CHECK(SS_BagState()==1);
 SS_MomCol(mom,ck_keenObj);SS_CHECK(SS_BagState()==1);SS_TestRoundTrip();
 ck_gameState.levelsDone[3]=ck_gameState.levelsDone[4]=0;
 ck_gameState.currentLevel=4;ck_gameState.levelState=LS_Playing;CK_LoadLevel(true,true);
 CK_object *bag=SS_Find("CK5_ACT_FuseExplosion0");SS_CHECK(bag);
 SS_BagCol(bag,ck_keenObj);SS_CHECK(SS_BagState()==2 && ck_gameState.levelsDone[3] && ck_gameState.levelsDone[4]);
 SS_CHECK(SS_HandleLevelState() && ck_gameState.currentLevel==0);CK_LoadLevel(true,true);ck_gameState.levelState=LS_Playing;
 SS_CHECK(!SS_Find("CK5_ACT_FuseExplosion2"));
 // A security-door card may be spent, but Billy still has his school bag.
 ck_gameState.ep.ck5.securityCard=0;bus=13;SS_CHECK(SS_MapEntry(&bus));SS_TestRoundTrip();
 ck_gameState.currentLevel=13;CK_LoadLevel(true,true);SS_AfterLoad();SS_CHECK(ck_gameState.levelState==LS_DestroyedQED);
 SS_CHECK(!CK_UseSecretEnding());
 puts("PASS: one-time farewell, persistent bag, both Scudders halves, gated bus and authored normal ending");fflush(stdout);
 // Every 4x4 key door advances through exactly the three authored frames.
 const int doorLevels[]={3,5,6,8,9,11,12,14,17,18,19,20,23};int doors=0;
 for(unsigned i=0;i<sizeof(doorLevels)/sizeof(doorLevels[0]);++i) {
  ck_gameState.currentLevel=doorLevels[i];ck_gameState.levelsDone[doorLevels[i]]=0;CK_LoadLevel(true,true);
  for(int y=0;y<CA_GetMapHeight();++y) for(int x=0;x<CA_GetMapWidth();++x) {
   int misc=TI_ForeMisc(CA_TileAtPos(x,y,1))&127;if(misc<7 || misc>10) continue;
   int target=CA_TileAtPos(x,y,2),tx=target>>8,ty=target&255;
   SS_CHECK(tx+4<=CA_GetMapWidth() && ty+4<=CA_GetMapHeight());
   uint16_t original[16];for(int dy=0;dy<4;++dy) for(int dx=0;dx<4;++dx) original[dy*4+dx]=CA_TileAtPos(tx+dx,ty+dy,1);
   CK_object door={0};door.posX=tx;door.posY=ty;
   for(int frame=0;frame<3;++frame) SS_DoorOpen(&door);
   for(int dy=0;dy<4;++dy) for(int dx=0;dx<4;++dx) SS_CHECK(CA_TileAtPos(tx+dx,ty+dy,1)==original[dy*4+dx]-12);
   ++doors;
  }
 }
 SS_CHECK(doors==17);puts("PASS: all 17 expanded key doors open their full 4x4 area");fflush(stdout);
 SS_TestActors();
 // Cleared checkpoint maps keep neither enemies nor story transitions.
 for(int level=1;level<=23;++level) {
  if(level==10 || level==13 || level==15) continue;
  ck_gameState.levelsDone[level]=1;ck_gameState.currentLevel=level;CK_LoadLevel(true,true);
  int hostiles,residents;CK_object *resident;KW_CountLevelPeople(&hostiles,&residents,&resident);
  SS_CHECK(KW_IsLevelPeaceful() && !hostiles);
  for(CK_object *ob=ck_keenObj->next;ob;ob=ob->next) SS_CHECK(!SS_IsStoryTrigger(ob));
  bool chain=false;for(int i=0;i<8;++i) if(level==ss_chainStarts[i] || level==ss_chainStarts[i]+1) chain=true;
  bool marker=false;for(int y=0;y<CA_GetMapHeight();++y) for(int x=0;x<CA_GetMapWidth();++x)
   if(CA_TileAtPos(x,y,2)==108 || CA_TileAtPos(x,y,2)==109) marker=true;
  if(chain && marker) SS_CHECK(KW_LastRevisitPlacement()==KW_REVISIT_GOAL && CK_NotStuckInWall(ck_keenObj));
 }
 puts("PASS: cleared suburb levels remove enemies/story triggers and checkpoint revisits use safe far-side positions");fflush(stdout);
 if(getenv("KW_AUTOCHUNK")) {
  int chunk=atoi(getenv("KW_AUTOCHUNK"));SS_CHECK(chunk==4916 || chunk==4918);
  ck_textPagesVoice=chunk==4916?"STORY":"END";CK_ShowTextPages(chunk,26,false);ck_textPagesVoice=NULL;
  puts("PASS: Suburb illustrated pages rendered");
 }
 KW_SwitchWorld(KW_SHIP);KW_SwitchWorld(KW_SUBURB);CK_LoadLevel(true,true);
 // Billy stands on the clear floor between the covered ship and computer.
 SS_CHECK(ca_mapOn==10 && RF_UnitToTile(ck_keenObj->posX)==28);
 SS_CHECK(CK_NotStuckInWall(ck_keenObj));
 SS_CHECK(SS_BagState()==2);
 ck_inputFrame.yDirection=IN_motion_Up;KW_PlayLoopHook();SS_CHECK(kw_pendingWorld==KW_SHIP);
 ck_inputFrame.yDirection=IN_motion_None;
 puts("PASS: Earth arrives and boards beside the backyard Megarocket, retaining mission state");fflush(stdout);
 kw_pendingWorld=KW_NO_WORLD;ck_gameState.currentLevel=13;CK_LoadLevel(true,true);
 SS_AfterLoad();SS_CHECK(ck_gameState.levelState==LS_DestroyedQED);
 if(getenv("KW_AUTOFINALE")) help_endgame();else CA_UpLevel();
 KW_AfterWorldEnding();SS_CHECK(KW_IsWorldCompleted(KW_SUBURB) && ck_gameState.levelsDone[13]);
 CK_LoadLevel(true,true);SS_TestRoundTrip();ck_gameState.currentLevel=13;CK_LoadLevel(true,true);ck_gameState.levelState=LS_Playing;
 SS_AfterLoad();SS_CHECK(ck_gameState.levelState==LS_Playing);
 puts("PASS: bus ending records world completion, survives save/load and does not replay on a peaceful visit");fflush(stdout);
}
