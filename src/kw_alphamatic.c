/* The Alphamatic, by Gridlock: native behavior of the audited 3.0 patch.
 * Original patches by Levellass, Fleex and K1n9_Duk3; retained in the
 * player's Patch5.pat with their notices. This C implementation is
 * GPL-2.0-or-later, like Omnispeak. No DOS code is executed.
 */
#include "kw_alphamatic.h"
#include "kw_world.h"
#include "ck_def.h"
#include "ck_act.h"
#include "ck_game.h"
#include "ck_play.h"
#include "ck5_ep.h"
#include "id_ca.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_us.h"
#include "id_fs.h"
#include "id_vl.h"
#include "kw_music.h"
#include "ck_text.h"
#include "kw_habitat.h"
#include <stdlib.h>
#include <stdio.h>

bool AL_IsActive(void) { return KW_CurrentWorld()==KW_ALPHAMATIC; }

// DOS dseg:6F5C is the same unused CK5 gamestate word. It is already saved
// by the engine and by the per-world records, including mid-fight saves.
static int16_t *AL_Health(void) { return &ck_gameState.ep.ck5.word_4729C; }

static void AL_LightningThink(CK_object *obj)
{
 if (++obj->user1 == 4) obj->currentAction=NULL;
}

void AL_SpawnImpact(int x, int y)
{
 CK_object *obj=CK_GetNewObj(false);
 obj->type=CT5_Spirogrip; obj->active=OBJ_ACTIVE; obj->zLayer=3;
 obj->clipped=CLIP_not;
 obj->posX=RF_TileToUnit(x); obj->posY=RF_TileToUnit(y)-256;
 obj->xDirection=obj->yDirection=1;
 if (*AL_Health()<1) *AL_Health()=24;
 CK_SetAction(obj,CK_ACTION(CK5_ACT_SpirogripAttachDown));
}

static void AL_FuseThink(CK_object *obj)
{
 // No change between thresholds, matching the patched routine.
 static const int thresholds[]={23,20,17,14,11,8,5,2};
 for (int i=0;i<8;++i) if (*AL_Health()==thresholds[i]) obj->gfxChunk=397+i;
}

static void AL_FuseCol(CK_object *fuse, CK_object *shot)
{
 (void)fuse;
 if (shot->type!=CT_Stunner) return;
 shot->type=CT_Friendly;
 shot->currentAction=CK_ACTION(CK5_ACT_SpirogripReleaseDown);
 --*AL_Health();
 SD_PlaySound(5);
}

static void AL_ImpactCol(CK_object *impact, CK_object *fuse)
{
 (void)impact;
 if (*AL_Health()>=1 || fuse->type!=CT5_QED) return;
 CK5_SpawnFuseExplosion(fuse->clipRects.tileX1,fuse->clipRects.tileY1);
 CK5_SpawnFuseExplosion(fuse->clipRects.tileX1,fuse->clipRects.tileY2);
 CK5_SpawnFuseExplosion(fuse->clipRects.tileX2,fuse->clipRects.tileY1);
 CK5_SpawnFuseExplosion(fuse->clipRects.tileX2,fuse->clipRects.tileY2);
 RF_ReplaceTileBlock(0,0,65,2,4,2);
 RF_ReplaceTileBlock(4,0,65,4,4,2);
 CK5_SpawnLevelEnd();
 CK_RemoveObj(fuse);
 *AL_Health()=3;
}

void AL_AfterDeath(void) { if (AL_IsActive()) *AL_Health()=0; }

void AL_AfterScan(void)
{
 if (!AL_IsActive()) return;
 for (CK_object *obj=ck_keenObj->next;obj;obj=obj->next)
  if (obj->type==CT5_Robo) obj->zLayer=1;
 if (ca_mapOn!=0) return;
 int y=ck_keenObj->posY>>8;
 if (y<75 || y>100) {
  CK5_CloseMapTeleporter(22,55);
  CK5_OpenMapTeleporter(24,76);
 }
 if (y>39 && ck_gameState.levelsDone[4] && ck_gameState.levelsDone[6] &&
     ck_gameState.levelsDone[8] && ck_gameState.levelsDone[10])
  CK5_OpenMapTeleporter(26,55);
 if (y<39 || y>100) CK5_OpenMapTeleporter(24,30);
}

void AL_SetupFunctions(void)
{
 CK_ACT_AddFunction("AL_LightningThink",AL_LightningThink);
 CK_ACT_AddFunction("AL_FuseThink",AL_FuseThink);
 CK_ACT_AddColFunction("AL_FuseCol",AL_FuseCol);
 CK_ACT_AddColFunction("AL_ImpactCol",AL_ImpactCol);
}

void KW_TestLevelSpritesCached(const char *world, int level);
void KW_TestRunObjects(int frames);
void CK_MapLevelMarkAsDone(void);
void CK_AnimateMapTeleporter(int x, int y);
void CK5_AnimateMapElevator(int x, int y, int dir);
void CK5_MapKeenElevator(CK_object *keen);
void CK5_SliceCol(CK_object *a, CK_object *b);

#define AL_CHECK(test) do { if (!(test)) { fprintf(stderr,"Alphamatic test line %d: %s\n",__LINE__,#test); QuitF("Alphamatic test line %d: %s",__LINE__,#test); } } while (0)
static CK_object *AL_FindFuse(void)
{
 for (CK_object *obj=ck_keenObj->next;obj;obj=obj->next) if (obj->type==CT5_QED) return obj;
 return NULL;
}

void AL_SelfTest(void)
{
 KW_SwitchWorld(KW_ALPHAMATIC);
 CK_NewGame(); ck_gameState.difficulty=D_Normal; ck_godMode=true;
 AL_CHECK(ca_audInfoE.startMusic==173 && ca_audInfoE.numSongs==33);
 // Each actor runs with real collision/physics/drawing, screenful by screenful.
 for (int level=0;level<=25;++level) {
  ck_gameState.currentLevel=level; CK_LoadLevel(true,true);
  AL_CHECK(ck_keenObj && CA_GetMapWidth()>0);
  KW_TestLevelSpritesCached("alphamatic",level);
  KW_TestRunObjects(90);
 }
 puts("PASS: Alphamatic all 26 map slots load and run native actors");fflush(stdout);
 CK_SetupObjArray();
 for (int i=2;i<CK_INT(CK_MaxObjects,100);++i) CK_GetNewObj(false);
 CK_RemoveObj(CK_GetNewObj(true));
 int count=0;for (CK_object *obj=ck_keenObj;obj;obj=obj->next) {
  ++count;AL_CHECK(count<=CK_INT(CK_MaxObjects,100));
 }
 AL_CHECK(count==CK_INT(CK_MaxObjects,100));
 puts("PASS: exhausted actor pool safely discards an optional shot");fflush(stdout);
 AL_AfterDeath();ck_gameState.currentLevel=12;CK_LoadLevel(true,true);
 puts("Alphamatic test: machine hits");fflush(stdout);
 CK_object *fuse=AL_FindFuse();AL_CHECK(fuse && *AL_Health()==24);
 CK_object *shot=CK_GetNewObj(false);
 shot->posX=fuse->clipRects.unitX1;shot->posY=fuse->clipRects.unitY1;
 shot->clipped=CLIP_not;
 for (int hit=1;hit<=7;++hit) {
  shot->type=CT_Stunner;AL_FuseCol(fuse,shot);AL_FuseThink(fuse);
  AL_CHECK(*AL_Health()==24-hit);
 }
 AL_CHECK(fuse->gfxChunk==399 && shot->type==CT_Friendly);
 puts("Alphamatic test: mid-fight save");fflush(stdout);
 FS_File file=FS_CreateUserFile("ALPHAMATIC-TEST.SAV");AL_CHECK(FS_IsFileValid(file));
 AL_CHECK(CK_SaveGame(file));FS_CloseFile(file);
 *AL_Health()=99;
 file=FS_OpenUserFile("ALPHAMATIC-TEST.SAV");AL_CHECK(FS_IsFileValid(file));
 AL_CHECK(CK_LoadGame(file,false));FS_CloseFile(file);
 puts("Alphamatic test: mid-fight save loaded");fflush(stdout);
 AL_CHECK(*AL_Health()==17);
 fuse=AL_FindFuse();AL_CHECK(fuse && fuse->gfxChunk==399);
 puts("Alphamatic test: finish the machine");fflush(stdout);
 shot=CK_GetNewObj(false);shot->posX=fuse->clipRects.unitX1;shot->posY=fuse->clipRects.unitY1;shot->clipped=CLIP_not;
 for (int hit=8;hit<=23;++hit) { shot->type=CT_Stunner;AL_FuseCol(fuse,shot); }
 AL_CHECK(*AL_Health()==1);
 AL_ImpactCol(shot,fuse);AL_CHECK(AL_FindFuse()==fuse);
 shot->type=CT_Stunner;AL_FuseCol(fuse,shot);
 AL_CHECK(*AL_Health()==0 && AL_FindFuse()==fuse);
 AL_ImpactCol(shot,fuse);
 puts("Alphamatic test: machine exploded");fflush(stdout);
 AL_CHECK(!AL_FindFuse() && *AL_Health()==3);
 AL_AfterDeath();AL_CHECK(*AL_Health()==0);
 ck_gameState.currentLevel=12;CK_LoadLevel(true,true);AL_CHECK(*AL_Health()==24);
 puts("Alphamatic test: slicestar");fflush(stdout);
 // Invincible slicestars consume the shot but never enter their death action.
 CK_object slice={0};slice.type=CT5_SliceStar;slice.user4=1;
 slice.currentAction=CK_ACTION(CK5_ACT_Slice0);
 shot=CK_GetNewObj(false);shot->type=CT_Stunner;shot->clipped=CLIP_not;
 shot->posX=ck_keenObj->posX;shot->posY=ck_keenObj->posY;
 CK5_SliceCol(&slice,shot);AL_CHECK(slice.currentAction!=CK_ACTION(CK5_ACT_SliceDie0));
 puts("PASS: Alphamatic 24 hits, delayed impact, mid-fight save/load, death reset and invincible slicestars");fflush(stdout);
 ck_gameState.currentLevel=0;CK_LoadLevel(true,true);
 ck_gameState.levelsDone[16]=1;
 CA_SetTileAtPos(10,10,2,0x0910);CA_SetTileAtPos(11,10,2,0xC010);
 CK_MapLevelMarkAsDone();AL_CHECK(CA_TileAtPos(10,10,2)==0x0910);
 // Original entrances are retained by Keen Worlds for peaceful return visits.
 AL_CHECK(CA_TileAtPos(11,10,2)==0xC010);
 ck_gameState.levelsDone[13]=1;AL_CHECK(!CK_UseSecretEnding());
 ck_gameState.levelsDone[13]=LS_KorathFuse;AL_CHECK(CK_UseSecretEnding());
 CK_object effect={0};for (int i=0;i<4;++i) AL_LightningThink(&effect);
 AL_CHECK(effect.user1==4 && !effect.currentAction);
 // This is a real expansion route, with a destination beyond Y=127.
 SD_SetSpriteSync(2);CK_AnimateMapTeleporter(6,132);
 AL_CHECK(ck_keenObj->posX==RF_TileToUnit(68));
 // The normal return animation walks Keen down from the encoded tile.
 AL_CHECK(ck_keenObj->posY>=RF_TileToUnit(132) && ck_keenObj->posY<RF_TileToUnit(134));
 AL_CHECK(CA_TileAtPos(6,132,1)==0x0A9E);
 CA_SetTileAtPos(22,56,2,(10<<8)|132);
 CK5_AnimateMapElevator(22,56,0);
 AL_CHECK((uint16_t)ck_keenObj->user1==RF_TileToUnit(133));
 AL_CHECK(ck_keenObj->user2==RF_TileToUnit(10));
 puts("PASS: Alphamatic transport coordinates, Y=132 teleport/elevator, four-loop effects and real-ending selector");fflush(stdout);
 if (getenv("KW_AUTOCHUNK")) {
  int chunk=atoi(getenv("KW_AUTOCHUNK"));
  AL_CHECK(chunk==4916 || chunk==4918 || chunk==4919);
  ck_textPagesVoice=chunk==4916 ? "STORY" : chunk==4918 ? "END" : "ENDS";
  CK_ShowTextPages(chunk,26,false);
  ck_textPagesVoice=NULL;
  puts("PASS: Alphamatic illustrated text pages rendered");
 }
 KW_SwitchWorld(KW_SHIP);
 AL_CHECK(!AL_IsActive());
 KW_SwitchWorld(KW_ALPHAMATIC);AL_CHECK(*AL_Health()==24);
 ck_gameState.currentLevel=0;CK_LoadLevel(true,true);
 memset(&ck_inputFrame,0,sizeof(ck_inputFrame));
 ck_keenState.jumpIsPressed=ck_keenState.pogoIsPressed=ck_keenState.shootIsPressed=false;
 ck_inputFrame.dir=IN_dir_East;ck_inputFrame.xDirection=IN_motion_Right;
 CK_SetAction(ck_keenObj,CK_ACTION(CK_ACT_MapKeenWalk0));SD_SetSpriteSync(2);
 for(int frame=0;frame<400;++frame) CK_RunAction(ck_keenObj);
 memset(&ck_inputFrame,0,sizeof(ck_inputFrame));
 ck_inputFrame.yDirection=IN_motion_Up;
 KW_TryLaunch(ck_keenObj);KW_PlayLoopHook();
 ck_inputFrame.yDirection=IN_motion_None;
 AL_CHECK(kw_pendingWorld==KW_SHIP);
 KW_SwitchWorld(KW_SHIP);
 puts("PASS: Alphamatic machine state stays with its world across cabin travel");
 puts("PASS: Alphamatic boards the Megarocket beside its original docked ship");
}
