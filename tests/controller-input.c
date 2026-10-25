/* Real SDL controller mapping, including a D-pad represented only as buttons. */
#include "id_in.h"
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// SDL's global backend table remains reachable with ASan. These unused
// keyboard/hotplug entry points are outside this joystick-only fixture.
bool INL_StartJoy(int joystick) {(void)joystick;return false;}
void INL_StopJoy(int joystick) {(void)joystick;}
void IN_HandleKeyDown(IN_ScanCode sc,bool special) {(void)sc;(void)special;}
void IN_HandleKeyUp(IN_ScanCode sc,bool special) {(void)sc;(void)special;}
void IN_HandleTextEvent(const char *text) {(void)text;}
void IN_ClearKeysDown(void) {}
void KW_ClearPresentationKeys(void) {}
void Quit(const char *message) {fprintf(stderr,"Unexpected SDL quit: %s\n",message?message:"");abort();}

bool IN_SDL_StartJoy(int joystick);
void IN_SDL_StopJoy(int joystick);
void IN_SDL_JoyGetAbs(int joystick, int *x, int *y);
uint16_t IN_SDL_JoyGetButtons(int joystick);
const char *IN_SDL_JoyGetButtonName(int joystick, int index);

int main(void)
{
 assert(SDL_Init(SDL_INIT_GAMECONTROLLER) == 0);
 int device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 2, 15, 0);
 assert(device >= 0);
 char guid[33],mapping[512];
 SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(device),guid,sizeof(guid));
 snprintf(mapping,sizeof(mapping),"%s,Keen Xbox test,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,leftx:a0,lefty:a1,",guid);
 assert(SDL_GameControllerAddMapping(mapping)>=0 && SDL_IsGameController(device));
 SDL_Joystick *virtual=SDL_JoystickOpen(device);assert(virtual);
 assert(SDL_JoystickNumHats(virtual)==0 && IN_SDL_StartJoy(device));
 int x,y;
 for(int button=11;button<=14;++button) {
  assert(SDL_JoystickSetVirtualButton(virtual,button,1)==0);SDL_JoystickUpdate();
  IN_SDL_JoyGetAbs(0,&x,&y);
  assert(x==(button==13?-32000:button==14?32000:0));
  assert(y==(button==11?-32000:button==12?32000:0));
  assert(SDL_JoystickSetVirtualButton(virtual,button,0)==0);SDL_JoystickUpdate();
 }
 assert(SDL_JoystickSetVirtualButton(virtual,11,1)==0);
 assert(SDL_JoystickSetVirtualButton(virtual,14,1)==0);SDL_JoystickUpdate();
 IN_SDL_JoyGetAbs(0,&x,&y);assert(x==32000 && y==-32000);
 assert(SDL_JoystickSetVirtualButton(virtual,11,0)==0);
 assert(SDL_JoystickSetVirtualButton(virtual,14,0)==0);
 assert(SDL_JoystickSetVirtualAxis(virtual,0,-16000)==0);SDL_JoystickUpdate();
 IN_SDL_JoyGetAbs(0,&x,&y);assert(x==-16000 && y==0);
 assert(SDL_JoystickSetVirtualButton(virtual,3,1)==0);SDL_JoystickUpdate();
 assert(IN_SDL_JoyGetButtons(0)&(1<<3));
 assert(!strcmp(IN_SDL_JoyGetButtonName(0,3),"Y"));
 assert(!strcmp(IN_SDL_JoyGetButtonName(0,6),"Start"));
 IN_SDL_StopJoy(0);SDL_JoystickClose(virtual);
 assert(SDL_JoystickDetachVirtual(device)==0);
 // Hotplug a plain joystick into the same slot: retain its raw hat path
 // and do not reuse the old controller handle.
 device=SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_UNKNOWN,2,4,1);assert(device>=0);
 assert(!SDL_IsGameController(device));
 virtual=SDL_JoystickOpen(device);assert(virtual && IN_SDL_StartJoy(device));
 assert(SDL_JoystickSetVirtualHat(virtual,0,SDL_HAT_LEFTUP)==0);SDL_JoystickUpdate();
 IN_SDL_JoyGetAbs(0,&x,&y);assert(x==-32000 && y==-32000);
 assert(IN_SDL_JoyGetButtonName(0,3)==NULL);
 IN_SDL_StopJoy(0);SDL_JoystickClose(virtual);
 assert(SDL_JoystickDetachVirtual(device)==0);SDL_Quit();
 puts("PASS: real SDL mapping reads Xbox Y/Start, button-only D-pad in all directions/diagonal, and analog movement");
 puts("PASS: hotplug from a mapped controller to a plain joystick retains raw hat input safely");
 return 0;
}
