// Keen Dreams Gold at the bunk: installation checks and the native launch,
// through the real bridge (src/kw_arcade.c) with isolated fixtures.
#include "kw_arcade.h"
#include "id_fs.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
const char *fs_keenPath, *fs_userPath;
bool vl_isFullScreen;
static int fadedOut, fadedIn;
void VL_FadeToBlack(void) { ++fadedOut; }
void VL_FadeFromBlack(void) { assert(fadedOut==fadedIn+1); ++fadedIn; }
void RF_ForceRefresh(void) {}
void RF_Refresh(void) {}
uint32_t SD_GetTimeCount(void) { return 100; }
void SD_SetLastTimeCount(uint32_t time) { assert(time==100); }
SDL_Window *SDL_GetKeyboardFocus(void) { return NULL; }
FS_File FSL_OpenFileInDirCaseInsensitive(const char *dir, const char *file, bool write)
{ char path[2048]; (void)write; snprintf(path,sizeof(path),"%s/%s",dir,file); return fopen(path,"rb"); }
void KW_LinesOver(void) {}
void SD_MusicOff(void) {}
void SD_StopSound(void) {}
void KW_ClearPresentationKeys(void) {}
void IN_ClearKeysDown(void) {}
void IN_PumpEvents(void) {}
static void put(const char *dir, const char *name, long size)
{
 char path[2048]; snprintf(path,sizeof(path),"%s/%s",dir,name);
 FILE *f=fopen(path,"wb"); assert(f);
 for (long i=0;i<size;++i) fputc(0,f);
 fclose(f);
}
int main(int argc, char **argv)
{
 assert(argc==3); fs_keenPath=argv[1]; fs_userPath=argv[2];
 char data[2048], sub[2048], gold[2048];
 snprintf(data,sizeof(data),"%s/mods",fs_keenPath); mkdir(data,0755);
 snprintf(data,sizeof(data),"%s/mods/dreams-gold",fs_keenPath);
 assert(KW_DreamsGoldCheck()==KW_ARCADE_NO_DATA);	// companion beside us, no files
 mkdir(data,0755);
 put(data,"KDGOLD.EXE",1000);
 assert(KW_DreamsGoldCheck()==KW_ARCADE_WRONG_VERSION);
 put(data,"KDGOLD.EXE",108427);
 assert(KW_DreamsGoldCheck()==KW_ARCADE_NO_DATA);
 put(data,"KDGOLD.EGA",1); put(data,"KDGOLD.MAP",1); put(data,"KDGOLD.AUD",1);
 assert(KW_DreamsGoldCheck()==KW_ARCADE_NO_DATA);	// headers missing
 snprintf(sub,sizeof(sub),"%s/KDGSRC",data); mkdir(sub,0755);
 snprintf(sub,sizeof(sub),"%s/KDGSRC/static",data); mkdir(sub,0755);
 put(sub,"KDGOLD.EHD",1); put(sub,"KDGOLD.MHD",1); put(sub,"KDGOLD.AHD",1);
 assert(KW_DreamsGoldCheck()==KW_ARCADE_READY);
 for (int fullscreen=0;fullscreen<=1;++fullscreen)
 {
  vl_isFullScreen=fullscreen;
  assert(!SDL_setenv("GOLD_TEST_FULLSCREEN", fullscreen ? "-fullscreen=true" : "-fullscreen=false", 1));
  assert(KW_PlayDreamsGold()==KW_ARCADE_READY);
  snprintf(gold,sizeof(gold),"%s/dreams-gold/TEST.SAV",fs_userPath);
  FILE *f=fopen(gold,"rb"); assert(f); char line[2048]; assert(fgets(line,sizeof(line),f)); fclose(f);
  assert(!strcmp(line,data));
  assert(!remove(gold));
 }
 assert(fadedOut==2 && fadedIn==2);
 // A failing companion is reported, and the journey goes on.
 assert(!SDL_setenv("GOLD_TEST_FAIL","1",1));
 assert(KW_PlayDreamsGold()==KW_ARCADE_FAILED);
 puts("PASS: Keen Dreams Gold install checks, launch arguments, separate saves and failure report");
 return 0;
}
