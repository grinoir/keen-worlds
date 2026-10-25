// Exercise the real native-process bridge, using isolated game fixtures.
#include "kw_arcade.h"
#include "id_fs.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
const char *fs_keenPath, *fs_userPath;
bool vl_isFullScreen;
static int stopped, cleared;
static int fadedOut, fadedIn, refreshed;
void VL_FadeToBlack(void) { ++fadedOut; }
void VL_FadeFromBlack(void) { assert(fadedOut==fadedIn+1); ++fadedIn; }
void RF_ForceRefresh(void) {}
void RF_Refresh(void) { ++refreshed; }
uint32_t SD_GetTimeCount(void) { return 100; }
void SD_SetLastTimeCount(uint32_t time) { assert(time==100); }
static SDL_Window *testWindow;
// Keep focus deterministic under SDL's dummy driver, while exercising
// actual SDL window flags, display bounds, hide/show and restoration.
SDL_Window *SDL_GetKeyboardFocus(void) { return testWindow; }
FS_File FSL_OpenFileInDirCaseInsensitive(const char *dir, const char *file, bool write)
{ char path[2048]; (void)write; snprintf(path,sizeof(path),"%s/%s",dir,file); return fopen(path,"rb"); }
void KW_LinesOver(void) { ++stopped; }
void SD_MusicOff(void) {}
void SD_StopSound(void) {}
void KW_ClearPresentationKeys(void) { ++cleared; }
void IN_ClearKeysDown(void) { ++cleared; }
void IN_PumpEvents(void) {}
int main(int argc, char **argv)
{
 assert(argc==3); fs_keenPath=argv[1]; fs_userPath=argv[2];
 assert(KW_ArcadeCheck(0)==KW_ARCADE_NO_ENGINE);
 assert(KW_ArcadeCheck(4)==KW_ARCADE_NO_ENGINE);
 for (int fullscreen=0;fullscreen<=1;++fullscreen) {
  vl_isFullScreen=fullscreen;
  assert(!SDL_setenv("ARCADE_TEST_FULLSCREEN", fullscreen ? "-fullscreen=true" : "-fullscreen=false", 1));
  for (int episode=1;episode<=3;++episode) {
   assert(KW_ArcadeCheck(episode)==KW_ARCADE_READY);
   assert(KW_PlayArcade(episode)==KW_ARCADE_READY);
   assert(vl_isFullScreen==fullscreen);
   char path[2048]; snprintf(path,sizeof(path),"%s/arcade/keen%d/TEST.SAV",fs_userPath,episode);
   FILE *f=fopen(path,"rb"); assert(f); char line[2048]; assert(fgets(line,sizeof(line),f)); fclose(f);
   char expected[2048];snprintf(expected,sizeof(expected),"%s/arcade/keen%d",fs_keenPath,episode);
   assert(!strcmp(line,expected));
  }

 }
 assert(!SDL_Init(SDL_INIT_VIDEO));
 testWindow=SDL_CreateWindow("arcade regression",20,20,320,200,0);assert(testWindow);
 vl_isFullScreen=false;
 assert(!SDL_setenv("ARCADE_TEST_FULLSCREEN","-fullscreen=true",1));
 for (int external=0;external<=1;++external) {
  {
   assert(!SDL_SetWindowFullscreen(testWindow,0));
   if (external) {
    SDL_Rect bounds;assert(!SDL_GetDisplayBounds(0,&bounds));
    SDL_SetWindowPosition(testWindow,bounds.x,bounds.y);
    SDL_SetWindowSize(testWindow,bounds.w,bounds.h);
    assert(!(SDL_GetWindowFlags(testWindow)&SDL_WINDOW_FULLSCREEN));
   } else assert(!SDL_SetWindowFullscreen(testWindow,SDL_WINDOW_FULLSCREEN_DESKTOP));
   assert(KW_PlayArcade(1)==KW_ARCADE_READY);
   assert(!vl_isFullScreen); // actual fullscreen may differ from the saved option
   assert(SDL_GetWindowFlags(testWindow)&SDL_WINDOW_FULLSCREEN);
  }
 }
 // The desktop can also make a game windowed while its saved option is
 // still fullscreen. The visible window determines the child mode.
 assert(!SDL_SetWindowFullscreen(testWindow,0));
 SDL_SetWindowPosition(testWindow,20,20);SDL_SetWindowSize(testWindow,320,200);
 vl_isFullScreen=true;
 assert(!SDL_setenv("ARCADE_TEST_FULLSCREEN","-fullscreen=false",1));
 assert(KW_PlayArcade(1)==KW_ARCADE_READY);
 assert(vl_isFullScreen && !(SDL_GetWindowFlags(testWindow)&SDL_WINDOW_FULLSCREEN));
 SDL_DestroyWindow(testWindow);testWindow=NULL;SDL_Quit();
 assert(stopped==9 && cleared==18);
 assert(fadedOut==9 && fadedIn==9 && refreshed==18);
 char path[2048]; snprintf(path,sizeof(path),"%s/arcade/keen1/SOUNDS.CK1",fs_keenPath);
 assert(!remove(path));
 assert(KW_ArcadeCheck(1)==KW_ARCADE_NO_DATA);
 snprintf(path,sizeof(path),"%s/arcade/keen1/KEEN1.EXE",fs_keenPath);
 FILE *f=fopen(path,"wb");assert(f);fputc(0,f);fclose(f);
 assert(KW_ArcadeCheck(1)==KW_ARCADE_WRONG_VERSION);
 assert(KW_PlayArcade(1)==KW_ARCADE_WRONG_VERSION);
 snprintf(path,sizeof(path),"%s/arcade/keen2/EGAHEAD.CK2",fs_keenPath);assert(!remove(path));
 assert(KW_ArcadeCheck(2)==KW_ARCADE_NO_DATA);
 assert(stopped==9 && cleared==18);
 puts("PASS: arcade launch/return, saved and actual fullscreen, desktop fullscreen geometry, separate saves, input cleanup and missing/version checks");
 return 0;
}
