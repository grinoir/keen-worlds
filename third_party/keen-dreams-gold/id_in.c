/* Keen Dreams Gold Edition Source Code
 * Copyright (C) 2026 Szemigi
 * This project contains taken and modified
 * codes/codings from these following sources:
 *
 * 1.
 * Reconstructed BioMenace 1
 * Copyright (C) 2024 lethal_guitar
 * 
 * 2.
 * Foray in the Forest
 * Copyright (C) 2022-2024 Nisaba
 * Copyright (C) 2019-2025 K1n9_Duk3
 *
 * 3.
 * Atroxian Realm
 * Copyright (C) 2020 Gridlock
 *
 * 4.
 * Reconstructed Commander Keen 4-6
 * Copyright (C) 2021 K1n9_Duk3
 *
 * 5.
 * Keen Dreams Unofficial Tech Demo
 * Copyright (C) 2015 NY00123
 *
 * 6.
 * Wolfenstein 3-D Source Code
 * Copyright (C) 1995 Id Software
 *
 * 7.
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * 8.
 * Catacomb 3-D Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * 9.
 * Keen Dreams SDL2/Steam Port
 * Copyright (C) 2015 David Gow <david@davidgow.net>
 *
 * 10.
 * Keen Worlds native companion changes
 * Copyright (C) 2026 Anders Christensen
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

//
//	ID Engine
//	ID_IN.c - Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

//
//	This module handles dealing with the various input devices
//
//	Depends on: Memory Mgr (for demo recording), Sound Mgr (for timing stuff),
//				User Mgr (for command line parms)
//
//	Globals:
//		LastScan - The keyboard scan code of the last key pressed
//		LastASCII - The ASCII value of the last key pressed
//	DEBUG - there are more globals
//

#include "id_heads.h"
#include <stdbool.h>
#include <SDL.h>
#pragma	hdrstop

#define	KeyInt	9	// The keyboard ISR number

// 	Stuff for the mouse
#define	MReset		0
#define	MButtons	3
#define	MDelta		11

// Stuff for the joystick
#define	JoyScaleMax		32768
#define	JoyScaleShift	8
#define	MaxJoyValue		5000

// 	Global variables
		boolean		Keyboard[NumCodes],
					JoysPresent[MaxJoys],
					MousePresent;
		Demo		DemoMode = demo_Off;
		boolean		Paused;
		char		LastASCII;
		ScanCode	LastScan;
		KeyboardDef	KbdDefs[MaxKbds] = {{sc_Control, sc_Alt,
							sc_Home, sc_UpArrow, sc_PgUp,
							sc_LeftArrow, sc_RightArrow,
							sc_End, sc_DownArrow, sc_PgDn}};
		JoystickDef	JoyDefs[MaxJoys];
		SDL_Joystick	*Joysticks[MaxJoys];
		SDL_GameController *JoyController[MaxJoys];
		ControlType	Controls[MaxPlayers];
		boolean	GravisGamepad;
		word		GravisAction[4];
		word		GravisMap[4];

//	Internal variables
static	boolean		IN_Started;
static	boolean		CapsLock;
static	ScanCode	CurCode,LastCode;
// Note (ktx58): ASCIINames, ShiftNames, SpecialNames and ExtScanCodes
// are far arrays like in Keen 4-6.
static	byte        far ASCIINames[] =		// Unshifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'1','2','3','4','5','6','7','8','9','0','-','=',8  ,9  ,	// 0
	'q','w','e','r','t','y','u','i','o','p','[',']',13 ,0  ,'a','s',	// 1
	'd','f','g','h','j','k','l',';',39 ,'`',0  ,92 ,'z','x','c','v',	// 2
	'b','n','m',',','.','/',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0		// 7
					},
					far ShiftNames[] =		// Shifted ASCII for scan codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,27 ,'!','@','#','$','%','^','&','*','(',')','_','+',8  ,9  ,	// 0
	'Q','W','E','R','T','Y','U','I','O','P','{','}',13 ,0  ,'A','S',	// 1
	'D','F','G','H','J','K','L',':',34 ,'~',0  ,'|','Z','X','C','V',	// 2
	'B','N','M','<','>','?',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',	// 4
	'2','3','0',127,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},
					far SpecialNames[] =	// ASCII for 0xe0 prefixed codes
					{
//	 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 0
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,13 ,0  ,0  ,0  ,	// 1
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 2
	0  ,0  ,0  ,0  ,0  ,'/',0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 3
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 4
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 5
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,	// 6
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0   	// 7
					},

					*ScanNames[] =		// Scan code names with single chars
					{
	"?","?","1","2","3","4","5","6","7","8","9","0","-","+","?","?",
	"Q","W","E","R","T","Y","U","I","O","P","[","]","|","?","A","S",
	"D","F","G","H","J","K","L",";","\"","?","?","?","Z","X","C","V",
	"B","N","M",",",".","/","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","\xf","?","-","\x15","5","\x11","+","?",
	"\x13","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
	"?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?"
					},	// DEBUG - consolidate these
					far ExtScanCodes[] =	// Scan codes with >1 char names
					{
	1,0xe,0xf,0x1d,0x2a,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,
	0x3f,0x40,0x41,0x42,0x43,0x44,0x57,0x59,0x46,0x1c,0x36,
	0x37,0x38,0x47,0x49,0x4f,0x51,0x52,0x53,0x45,0x48,
	0x50,0x4b,0x4d,0x00
					},
					*ExtScanNames[] =	// Names corresponding to ExtScanCodes
					{
	"Esc","BkSp","Tab","Ctrl","LShft","Space","CapsLk","F1","F2","F3","F4",
	"F5","F6","F7","F8","F9","F10","F11","F12","ScrlLk","Enter","RShft",
	"PrtSc","Alt","Home","PgUp","End","PgDn","Ins","Del","NumLk","Up",
	"Down","Left","Right",""
					};
static	Direction	DirTable[] =		// Quick lookup for total direction
					{
						dir_NorthWest,	dir_North,	dir_NorthEast,
						dir_West,		dir_None,	dir_East,
						dir_SouthWest,	dir_South,	dir_SouthEast
					};

byte _seg	*DemoBuffer;
word		DemoOffset,DemoSize;

static	void			(*INL_KeyHook)(void);

static	char			*ParmStrings[] = {"nojoys","nomouse","swmouse","norightstick","nodpad",nil};

static	int		JoyDisableTo = 0;

// Should we use Right joystick or DPad in SDL_GameController mode?
boolean rightjoy = true;
boolean dpadokay = true;

//	Internal routines
static boolean INL_StartJoy(word joy);
static void INL_ShutJoy(word joy);

///////////////////////////////////////////////////////////////////////////
//
//	INL_KeyService() - Handles a keyboard interrupt (key up/down)
//
///////////////////////////////////////////////////////////////////////////
// The DOS keyboard interrupt's codes (set 1) for SDL's physical keys. Right
// Ctrl/Alt read as the left ones and the keypad Enter as Enter, as in DOS.
static const byte INL_DOSCode[SDL_NUM_SCANCODES] = {
	[SDL_SCANCODE_ESCAPE] = 0x01, [SDL_SCANCODE_1] = 0x02, [SDL_SCANCODE_2] = 0x03,
	[SDL_SCANCODE_3] = 0x04, [SDL_SCANCODE_4] = 0x05, [SDL_SCANCODE_5] = 0x06,
	[SDL_SCANCODE_6] = 0x07, [SDL_SCANCODE_7] = 0x08, [SDL_SCANCODE_8] = 0x09,
	[SDL_SCANCODE_9] = 0x0a, [SDL_SCANCODE_0] = 0x0b, [SDL_SCANCODE_MINUS] = 0x0c,
	[SDL_SCANCODE_EQUALS] = 0x0d, [SDL_SCANCODE_BACKSPACE] = 0x0e, [SDL_SCANCODE_TAB] = 0x0f,
	[SDL_SCANCODE_Q] = 0x10, [SDL_SCANCODE_W] = 0x11, [SDL_SCANCODE_E] = 0x12,
	[SDL_SCANCODE_R] = 0x13, [SDL_SCANCODE_T] = 0x14, [SDL_SCANCODE_Y] = 0x15,
	[SDL_SCANCODE_U] = 0x16, [SDL_SCANCODE_I] = 0x17, [SDL_SCANCODE_O] = 0x18,
	[SDL_SCANCODE_P] = 0x19, [SDL_SCANCODE_LEFTBRACKET] = 0x1a, [SDL_SCANCODE_RIGHTBRACKET] = 0x1b,
	[SDL_SCANCODE_RETURN] = 0x1c, [SDL_SCANCODE_KP_ENTER] = 0x1c,
	[SDL_SCANCODE_LCTRL] = 0x1d, [SDL_SCANCODE_RCTRL] = 0x1d,
	[SDL_SCANCODE_A] = 0x1e, [SDL_SCANCODE_S] = 0x1f, [SDL_SCANCODE_D] = 0x20,
	[SDL_SCANCODE_F] = 0x21, [SDL_SCANCODE_G] = 0x22, [SDL_SCANCODE_H] = 0x23,
	[SDL_SCANCODE_J] = 0x24, [SDL_SCANCODE_K] = 0x25, [SDL_SCANCODE_L] = 0x26,
	[SDL_SCANCODE_SEMICOLON] = 0x27, [SDL_SCANCODE_APOSTROPHE] = 0x28, [SDL_SCANCODE_GRAVE] = 0x29,
	[SDL_SCANCODE_LSHIFT] = 0x2a, [SDL_SCANCODE_BACKSLASH] = 0x2b,
	[SDL_SCANCODE_Z] = 0x2c, [SDL_SCANCODE_X] = 0x2d, [SDL_SCANCODE_C] = 0x2e,
	[SDL_SCANCODE_V] = 0x2f, [SDL_SCANCODE_B] = 0x30, [SDL_SCANCODE_N] = 0x31,
	[SDL_SCANCODE_M] = 0x32, [SDL_SCANCODE_COMMA] = 0x33, [SDL_SCANCODE_PERIOD] = 0x34,
	[SDL_SCANCODE_SLASH] = 0x35, [SDL_SCANCODE_RSHIFT] = 0x36, [SDL_SCANCODE_KP_MULTIPLY] = 0x37,
	[SDL_SCANCODE_LALT] = 0x38, [SDL_SCANCODE_RALT] = 0x38, [SDL_SCANCODE_SPACE] = 0x39,
	[SDL_SCANCODE_CAPSLOCK] = 0x3a,
	[SDL_SCANCODE_F1] = 0x3b, [SDL_SCANCODE_F2] = 0x3c, [SDL_SCANCODE_F3] = 0x3d,
	[SDL_SCANCODE_F4] = 0x3e, [SDL_SCANCODE_F5] = 0x3f, [SDL_SCANCODE_F6] = 0x40,
	[SDL_SCANCODE_F7] = 0x41, [SDL_SCANCODE_F8] = 0x42, [SDL_SCANCODE_F9] = 0x43,
	[SDL_SCANCODE_F10] = 0x44, [SDL_SCANCODE_NUMLOCKCLEAR] = 0x45, [SDL_SCANCODE_SCROLLLOCK] = 0x46,
	[SDL_SCANCODE_HOME] = 0x47, [SDL_SCANCODE_KP_7] = 0x47,
	[SDL_SCANCODE_UP] = 0x48, [SDL_SCANCODE_KP_8] = 0x48,
	[SDL_SCANCODE_PAGEUP] = 0x49, [SDL_SCANCODE_KP_9] = 0x49, [SDL_SCANCODE_KP_MINUS] = 0x4a,
	[SDL_SCANCODE_LEFT] = 0x4b, [SDL_SCANCODE_KP_4] = 0x4b, [SDL_SCANCODE_KP_5] = 0x4c,
	[SDL_SCANCODE_RIGHT] = 0x4d, [SDL_SCANCODE_KP_6] = 0x4d, [SDL_SCANCODE_KP_PLUS] = 0x4e,
	[SDL_SCANCODE_END] = 0x4f, [SDL_SCANCODE_KP_1] = 0x4f,
	[SDL_SCANCODE_DOWN] = 0x50, [SDL_SCANCODE_KP_2] = 0x50,
	[SDL_SCANCODE_PAGEDOWN] = 0x51, [SDL_SCANCODE_KP_3] = 0x51,
	[SDL_SCANCODE_INSERT] = 0x52, [SDL_SCANCODE_KP_0] = 0x52,
	[SDL_SCANCODE_DELETE] = 0x53, [SDL_SCANCODE_KP_PERIOD] = 0x53,
	[SDL_SCANCODE_F11] = 0x57, [SDL_SCANCODE_F12] = 0x58,
};

static void
INL_KeyService(ScanCode k, bool pressed)
{
	if (!pressed)	// Break code
		Keyboard[k] = false;
	else			// Make code
	{
		LastCode = CurCode;
		CurCode = LastScan = k;
		Keyboard[k] = true;
		// Keys that type no text still give the ASCII the DOS handler gave
		if (k == sc_Return || k == sc_Escape || k == sc_BackSpace || k == sc_Tab)
			LastASCII = ASCIINames[k];
	}

	if (INL_KeyHook)
		INL_KeyHook();
}

ScanCode INL_SDLKToScanCode(SDL_Scancode scan)
{
	if (scan < 0 || scan >= SDL_NUM_SCANCODES)
		return sc_None;
	return INL_DOSCode[scan];
}


static int wheel = 0;

int IN_ReadWheel()
{
	int w = wheel;
	wheel = 0;
	return w;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_HandleSDLEvent() - Handles an event from the system SDL2
//
///////////////////////////////////////////////////////////////////////////
static void INL_HandleSDLEvent(SDL_Event *event)
{

	ScanCode sc;
	static boolean special;
	char c;

	switch (event->type)
	{
	case SDL_QUIT:
		Quit(0);
		break;
	case SDL_KEYDOWN:
		if (event->key.keysym.scancode == SDL_SCANCODE_PAUSE)
		{
			Paused = true;
			break;
		}
		sc = INL_SDLKToScanCode(event->key.keysym.scancode);
		if (sc != sc_None)
			INL_KeyService(sc, true);
		break;
	case SDL_KEYUP:
		sc = INL_SDLKToScanCode(event->key.keysym.scancode);
		if (sc != sc_None)
			INL_KeyService(sc, false);
		break;
	case SDL_TEXTINPUT:
		// If the character is ASCII...
		if (event->text.text[0] < 0x80)
			LastASCII = event->text.text[0];
		break;
	case SDL_MOUSEWHEEL:
		wheel += event->wheel.y;
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		if (event->cbutton.button == SDL_CONTROLLER_BUTTON_START)
			INL_KeyService(sc_Escape, true);
		break;
	case SDL_CONTROLLERBUTTONUP:
		if (event->cbutton.button == SDL_CONTROLLER_BUTTON_START)
			INL_KeyService(sc_Escape, false);
		break;
	case SDL_CONTROLLERDEVICEADDED:
		if (!JoysPresent[event->cdevice.which])
			INL_StartJoy(event->cdevice.which);
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		{
		SDL_JoystickID jid = event->cdevice.which;
		INL_StartJoy(event->cdevice.which);
		if (JoysPresent[0] && SDL_JoystickInstanceID(Joysticks[0]) == jid)
			INL_ShutJoy(0);
		if (JoysPresent[1] && SDL_JoystickInstanceID(Joysticks[1]) == jid)
			INL_ShutJoy(1);
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_PumpEvents() - Handles system events, making sure input is up
//		-to-date
//
///////////////////////////////////////////////////////////////////////////
void IN_PumpEvents(void)
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
		INL_HandleSDLEvent(&evt);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseDelta() - Gets the amount that the mouse has moved from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetMouseDelta(int *x,int *y)
{
	SDL_GetRelativeMouseState(x, y);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetMouseButtons() - Gets the status of the mouse buttons from the
//		mouse driver
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetMouseButtons(void)
{
	return SDL_GetMouseState(0,0);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define DO_DEADZONE(val, zone) (((val)*(val) < (zone)*(zone))?0:val) 
void
IN_GetJoyAbs(word joy,int *xp,int *yp)
{
	if (JoyController[joy])
	{
		int DPadUsed = 0;
		// Controller D-Pad bindings.
		if (dpadokay && SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_DPAD_UP) && yp)
		{
			*yp = -127;
			DPadUsed = 1;
		}
		else if (dpadokay && SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_DPAD_DOWN) && yp)
		{
			*yp = 127;
			DPadUsed = 1;
		}
		if (dpadokay && SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_DPAD_LEFT) && xp)
		{
			*xp = -127;
			DPadUsed = 1;
		}
		else if (dpadokay && SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && xp)
		{
			*xp = 127;
			DPadUsed = 1;
		}
		
		if (DPadUsed) return;

		if (xp)
		{
			int val = SDL_GameControllerGetAxis(JoyController[joy], SDL_CONTROLLER_AXIS_LEFTX);
			*xp = DO_DEADZONE(val, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		}
		if (yp)
		{
			int val = SDL_GameControllerGetAxis(JoyController[joy], SDL_CONTROLLER_AXIS_LEFTY);
			*yp = DO_DEADZONE(val, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		}
		if (rightjoy && xp)
		{
			int val = SDL_GameControllerGetAxis(JoyController[joy], SDL_CONTROLLER_AXIS_RIGHTX);
			*xp += DO_DEADZONE(val, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		}
		if (rightjoy && yp)
		{
			int val = SDL_GameControllerGetAxis(JoyController[joy], SDL_CONTROLLER_AXIS_RIGHTY);
			*yp += DO_DEADZONE(val, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		}

	}
	else
	{
		const int AxisX = 0;
		const int AxisY = 1;
		if (xp)
			*xp = SDL_JoystickGetAxis(Joysticks[joy], AxisX);
		if (yp)
			*yp = SDL_JoystickGetAxis(Joysticks[joy], AxisY);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_IsJoyController() - Returns true if joy is an SDL_GameController 
//
///////////////////////////////////////////////////////////////////////////
int
IN_IsJoyController(word joy)
{
	if (JoyController[joy])
	{
		return 1; 
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyName() - Returns the name of the Joystick from the
//		system.
//
///////////////////////////////////////////////////////////////////////////
const char*
IN_GetJoyName(word joy)
{
	if (JoyController[joy])
	{
		return SDL_GameControllerName(JoyController[joy]);
	}
	else if (joy == 0)
		return "Joystick 1";
	else if (joy == 1)
		return "Joystick 2";
	return "Unknown Joystick";
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127, scaled adaptively)
//
///////////////////////////////////////////////////////////////////////////
static void
INL_GetJoyDelta(word joy,int *dx,int *dy,boolean adaptive)
{
	IN_GetJoyAbs(joy, dx, dy);
	// Clamp
	if (*dx > 127) *dx = 127;
	if (*dy > 127) *dy = 127;
	if (*dx < -127) *dx = -127;
	if (*dy < -127) *dy = -127;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyButtons() - Returns the button status of the specified
//		joystick
//
///////////////////////////////////////////////////////////////////////////
static word
INL_GetJoyButtons(word joy)
{
	// No buttons are pressed if we've disabled them for a period.
	if (SDL_GetTicks() < JoyDisableTo) return 0;

	// Gold's "Gravis Gamepad" option reads four buttons from the first pad.
	if (joy == 2)
	{
		word result = 0;
		if (!JoysPresent[0]) return 0;
		if (JoyController[0])
		{
			static const SDL_GameControllerButton b[4] = {SDL_CONTROLLER_BUTTON_A,
				SDL_CONTROLLER_BUTTON_B, SDL_CONTROLLER_BUTTON_X, SDL_CONTROLLER_BUTTON_Y};
			for (int i = 0; i < 4; i++)
				result |= SDL_GameControllerGetButton(JoyController[0], b[i]) << i;
		}
		else
			for (int i = 0; i < 4; i++)
				result |= SDL_JoystickGetButton(Joysticks[0], i) << i;
		return result;
	}

	if (JoyController[joy])
	{
		word result = 0;
		result |= SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_A);
		result |= (SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_B) << 1);

		// Also check X/Y for GameController configs.
		result |= SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_Y);
		result |= (SDL_GameControllerGetButton(JoyController[joy], SDL_CONTROLLER_BUTTON_X) << 1);
		return result;
	}
	else
	{
		const int buttonA = 0, buttonB = 1;
		return (SDL_JoystickGetButton(Joysticks[joy], buttonA) | SDL_JoystickGetButton(Joysticks[joy],buttonB) << 1);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetJoyDelay() - Disables joystick buttons for ms milliseconds
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetJoyDelay(int ms)
{
	JoyDisableTo = SDL_GetTicks() + ms;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyButtonsDB() - Returns the de-bounced button status of the
//		specified joystick
//
///////////////////////////////////////////////////////////////////////////
word
IN_GetJoyButtonsDB(word joy)
{
	return INL_GetJoyButtons(joy);
}

int
IN_GetAllJoyButtons(word joy)
{
	int numButtons = SDL_JoystickNumButtons(Joysticks[joy]);
	int i;
	for (i = 0; i < numButtons; ++i)
	{
		if (SDL_JoystickGetButton(Joysticks[joy], i))
			return i + 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartKbd() - Sets up my keyboard stuff for use
//
///////////////////////////////////////////////////////////////////////////
static void
INL_StartKbd(void)
{
	IN_ClearKeysDown();


	INL_KeyHook = 0;	// Clear key hook
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutKbd() - Restores keyboard control to the BIOS
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutKbd(void)
{
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartMouse() - Detects and sets up the mouse
//
///////////////////////////////////////////////////////////////////////////
static boolean
INL_StartMouse(void)
{
	return(true);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutMouse() - Cleans up after the mouse
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutMouse(void)
{
}

//
//	INL_SetJoyScale() - Sets up scaling values for the specified joystick
//
static void
INL_SetJoyScale(word joy)
{
	JoystickDef	*def;

	def = &JoyDefs[joy];
	def->joyMultXL = JoyScaleMax / (def->threshMinX - def->joyMinX);
	def->joyMultXH = JoyScaleMax / (def->joyMaxX - def->threshMaxX);
	def->joyMultYL = JoyScaleMax / (def->threshMinY - def->joyMinY);
	def->joyMultYH = JoyScaleMax / (def->joyMaxY - def->threshMaxY);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//		to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy)
{
	word		d,r;
	JoystickDef	*def;

	def = &JoyDefs[joy];

	def->joyMinX = minx;
	def->joyMaxX = maxx;
	r = maxx - minx;
	d = r / 5;
	def->threshMinX = ((r / 2) - d) + minx;
	def->threshMaxX = ((r / 2) + d) + minx;

	def->joyMinY = miny;
	def->joyMaxY = maxy;
	r = maxy - miny;
	d = r / 5;
	def->threshMinY = ((r / 2) - d) + miny;
	def->threshMaxY = ((r / 2) + d) + miny;

	INL_SetJoyScale(joy);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_StartJoy() - Detects & auto-configures the specified joystick
//					The auto-config assumes the joystick is centered
//
///////////////////////////////////////////////////////////////////////////
extern int			CursorX,CursorY;
static boolean
INL_StartJoy(word joy)
{
	int x,y;
	
	if (joy >= SDL_NumJoysticks()) return false;
	
	Joysticks[joy] = SDL_JoystickOpen(joy);
	if (SDL_IsGameController(joy))
	{
		JoyController[joy] = SDL_GameControllerOpen(joy);
	}
	else
		JoyController[joy] = 0;
	

	IN_GetJoyAbs(joy,&x,&y);

	JoysPresent[joy] = true;


	if (IN_Started)	
	{
		US_FlushOnHotplug();

		cursorhw = false;	// Gold has no mouse cursor sprite
	}

	//IN_SetupJoy(joy, 0, x*2, 0, y*2);
	return(true);
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_ShutJoy() - Cleans up the joystick stuff
//
///////////////////////////////////////////////////////////////////////////
static void
INL_ShutJoy(word joy)
{
	JoysPresent[joy] = false;
	if (JoyController[joy])
		SDL_GameControllerClose(JoyController[joy]);
	SDL_JoystickClose(Joysticks[joy]);

	// If we unplug the joystick in use, make the keyboard the default input.
	if (Controls[0] == ctrl_Joystick + joy)
		Controls[0] = ctrl_Keyboard;

	US_FlushOnHotplug();
}

//	Public routines

///////////////////////////////////////////////////////////////////////////
//
//	IN_Startup() - Starts up the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Startup(void)
{
	boolean	checkjoys,checkmouse;
	word	i;

	if (IN_Started)
		return;

	checkjoys = true;
	checkmouse = true;
	for (i = 1;i < _argc;i++)
	{
		switch (US_CheckParm(_argv[i],ParmStrings))
		{
		case 0:
			checkjoys = false;
			break;
		case 1:
			checkmouse = false;
		case 2:
			cursorhw = false;
			break;
		case 3:
			rightjoy = false;
			break;
		case 4:
			dpadokay = false;
			break;
		}
	}

	if (checkjoys)
		SDL_Init(SDL_INIT_JOYSTICK);
	INL_StartKbd();
	MousePresent = checkmouse? INL_StartMouse() : false;

	for (i = 0;i < MaxJoys;i++)
		JoysPresent[i] = checkjoys? INL_StartJoy(i) : false;

	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i]) cursorhw = false;

	IN_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Default() - Sets up default conditions for the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Default(boolean gotit,ControlType in)
{
	if
	(
		(!gotit)
	|| 	((in == ctrl_Joystick1) && !JoysPresent[0])
	|| 	((in == ctrl_Joystick2) && !JoysPresent[1])
	|| 	((in == ctrl_Mouse) && !MousePresent)
	)
		in = ctrl_Keyboard1;
	IN_SetControlType(0,in);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Shutdown() - Shuts down the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Shutdown(void)
{
	word	i;

	if (!IN_Started)
		return;

	INL_ShutMouse();
	for (i = 0;i < MaxJoys;i++)
		INL_ShutJoy(i);
	INL_ShutKbd();

	IN_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetKeyHook() - Sets the routine that gets called by INL_KeyService()
//			everytime a real make/break code gets hit
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetKeyHook(void (*hook)())
{
	INL_KeyHook = hook;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ClearKeyDown() - Clears the keyboard array
//
///////////////////////////////////////////////////////////////////////////
void
IN_ClearKeysDown(void)
{
	int	i;

	LastScan = sc_None;
	LastASCII = key_None;
	for (i = 0;i < NumCodes;i++)
		Keyboard[i] = false;

	// davidgow - This is also a good place to reset the mouse cursor.
	// (Thanks NY00123)
	SDL_GetRelativeMouseState(NULL, NULL);
}


///////////////////////////////////////////////////////////////////////////
//
//	INL_AdjustCursor() - Internal routine of common code from IN_ReadCursor()
//
///////////////////////////////////////////////////////////////////////////
static void
INL_AdjustCursor(CursorInfo *info,word buttons,int dx,int dy)
{
	if (buttons & (1 << 0))
		info->button0 = true;
	if (buttons & (1 << 1))
		info->button1 = true;

	info->x += dx;
	info->y += dy;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadCursor() - Reads the input devices and fills in the cursor info
//		struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadCursor(CursorInfo *info)
{
	word	i,
		player,
			buttons;
	int		dx = 0;
	int		dy = 0;

	info->x = info->y = 0;
	info->button0 = info->button1 = false;

	IN_PumpEvents();
	if (MousePresent)
	{
		if (cursorhw && cursorvisible)
		{
			int x, y;
			int buttons = SDL_GetMouseState(&x, &y);
			int vx, vy, vw, vh;
			VW_GL_GetViewport(&vx, &vy, &vw, &vh);
			info->x = (x-vx)*320/vw;
			info->y = (y-vy)*200/vh;
			info->button0 = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
			info->button1 = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		}
		else
		{
			buttons = INL_GetMouseButtons();
			dx /= 2;
			dy /= 2;
			INL_GetMouseDelta(&dx,&dy);
			INL_AdjustCursor(info,buttons,dx,dy);
		}
	}

	for (i = 0;i < MaxJoys;i++)
	{
		if (!JoysPresent[i])
			continue;

		for (player = 0;player < MaxPlayers; player++)
		{
			if (Controls[player] == ctrl_Joystick1+i)
				goto joyok;
		}
		continue;

joyok:
		buttons = INL_GetJoyButtons(i);
		INL_GetJoyDelta(i,&dx,&dy,true);
		dx /= 64;
		dy /= 64;
		INL_AdjustCursor(info,buttons,dx,dy);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_ReadControl() - Reads the device associated with the specified
//		player and fills in the control info struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadControl(int player,ControlInfo *info)
{
			boolean		realdelta;
			byte		dbyte;
			word		buttons;
			int			i;
			int			dx,dy;
			Motion		mx,my;
			ControlType	type;
register	KeyboardDef	*def;

	dx = dy = 0;
	mx = my = motion_None;
	buttons = 0;

	if (DemoMode == demo_Playback)
	{
		dbyte = DemoBuffer[DemoOffset + 1];

		my = (dbyte & 3) - 1;
		mx = ((dbyte >> 2) & 3) - 1;
		buttons = (dbyte >> 4) & 3;

		if (!(--DemoBuffer[DemoOffset]))

		{
			DemoOffset += 2;
			if (DemoOffset >= DemoSize)
				DemoMode = demo_PlayDone;
		}
		
		realdelta = false;
	}
	else if (DemoMode == demo_PlayDone)
		Quit("Demo playback exceeded");
	else
	{
		switch (type = Controls[player])
		{
		case ctrl_Keyboard1:
		case ctrl_Keyboard2:
			def = &KbdDefs[type - ctrl_Keyboard];

			if (Keyboard[def->upleft])
				mx = motion_Left,my = motion_Up;
			else if (Keyboard[def->upright])
				mx = motion_Right,my = motion_Up;
			else if (Keyboard[def->downleft])
				mx = motion_Left,my = motion_Down;
			else if (Keyboard[def->downright])
				mx = motion_Right,my = motion_Down;

			if (Keyboard[def->up])
				my = motion_Up;
			else if (Keyboard[def->down])
				my = motion_Down;

			if (Keyboard[def->left])
				mx = motion_Left;
			else if (Keyboard[def->right])
				mx = motion_Right;

			if (Keyboard[def->button0])
				buttons += 1 << 0;
			if (Keyboard[def->button1])
				buttons += 1 << 1;
			realdelta = false;
			break;
		case ctrl_Joystick1:
		case ctrl_Joystick2:
			INL_GetJoyDelta(type - ctrl_Joystick,&dx,&dy,false);
			//IN_GetJoyAbs(type - ctrl_Joystick, &dx, &dy);
			if (GravisGamepad)
			{
				buttons = INL_GetJoyButtons(2);
				for (i=0; i<4; i++)
				{
					GravisAction[i] = buttons & (1 << GravisMap[i]);
				}
			}
			buttons = INL_GetJoyButtons(type - ctrl_Joystick);
			realdelta = true;
			break;
		case ctrl_Mouse:
			INL_GetMouseDelta(&dx,&dy);
			buttons = INL_GetMouseButtons();
			realdelta = true;
			break;
		}
	}

	if (realdelta)
	{
		mx = (dx < 0)? motion_Left : ((dx > 0)? motion_Right : motion_None);
		my = (dy < 0)? motion_Up : ((dy > 0)? motion_Down : motion_None);
	}
	else
	{
		dx = mx * 127;
		dy = my * 127;
	}

	info->x = dx;
	info->xaxis = mx;
	info->y = dy;
	info->yaxis = my;
	info->button0 = buttons & (1 << 0);
	info->button1 = buttons & (1 << 1);
	info->dir = DirTable[((my + 1) * 3) + (mx + 1)];

	if (DemoMode == demo_Record)
	{
		// Pack the control info into a byte
		dbyte = (buttons << 4) | ((mx + 1) << 2) | (my + 1);

		if
		(
			(DemoBuffer[DemoOffset + 1] == dbyte)
		&&	(DemoBuffer[DemoOffset] < 255)
		)
			DemoBuffer[DemoOffset]++;


		else
		{
			if (DemoOffset || DemoBuffer[DemoOffset])
				DemoOffset += 2;

			if (DemoOffset >= DemoSize)
				Quit("Demo buffer overflow");

			DemoBuffer[DemoOffset] = 1;
			DemoBuffer[DemoOffset + 1] = dbyte;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_SetControlType() - Sets the control type to be used by the specified
//		player
//
///////////////////////////////////////////////////////////////////////////
void
IN_SetControlType(int player,ControlType type)
{
	// DEBUG - check that type is present?
	Controls[player] = type;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoRecord() - Starts the demo recording, using a buffer the
//		size passed. Returns if the buffer allocation was successful
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_StartDemoRecord(word bufsize)
{
	if (!bufsize)
		return(false);

	MM_GetPtr((memptr *)&DemoBuffer,bufsize);
	DemoMode = demo_Record;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
	DemoBuffer[0] = DemoBuffer[1] = 0;

	return(true);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StartDemoPlayback() - Plays back the demo pointed to of the given size
//
///////////////////////////////////////////////////////////////////////////
void
IN_StartDemoPlayback(byte _seg *buffer,word bufsize)
{
	DemoBuffer = buffer;
	DemoMode = demo_Playback;
	DemoSize = bufsize & ~1;
	DemoOffset = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_StopDemoRecord() - Turns off demo mode
//
///////////////////////////////////////////////////////////////////////////
void
IN_StopDemo(void)
{
	if ((DemoMode == demo_Record) && DemoOffset)
		DemoOffset += 2;

	DemoMode = demo_Off;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_FreeDemoBuffer() - Frees the demo buffer, if it's been allocated
//
///////////////////////////////////////////////////////////////////////////
void
IN_FreeDemoBuffer(void)
{
	if (DemoBuffer)
		MM_FreePtr((memptr *)&DemoBuffer);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetScanName() - Returns a string containing the name of the
//		specified scan code
//
///////////////////////////////////////////////////////////////////////////
char *
IN_GetScanName(ScanCode scan)
{
	byte		**p;
	ScanCode	far *s;

	for (s = ExtScanCodes,p = ExtScanNames;*s;p++,s++)
		if (*s == scan)
			return(*p);

	return(ScanNames[scan]);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForKey() - Waits for a scan code, then clears LastScan and
//		returns the scan code
//
///////////////////////////////////////////////////////////////////////////
ScanCode IN_WaitForKey(void)
{
	SDL_Event event;
	ScanCode	result;
	while (SDL_WaitEvent(&event))
	{
		INL_HandleSDLEvent(&event);
		if ((result = LastScan)) break;
		// Refresh the display to stop steam overlay hanging.
		VW_GL_Present();
	}
	LastScan = 0;
	return (result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_WaitForASCII() - Waits for an ASCII char, then clears LastASCII and
//		returns the ASCII value
//
///////////////////////////////////////////////////////////////////////////
char
IN_WaitForASCII(void)
{
	SDL_Event event;
	char		result;
	while (SDL_WaitEvent(&event))
	{
		INL_HandleSDLEvent(&event);
		if (!(result = LastASCII)) break;
		// Refresh the display to stop steam overlay hanging.
		VW_GL_Present();
	}
	LastASCII = '\0';
	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_AckBack() - Waits for either an ASCII keypress or a button press
//
///////////////////////////////////////////////////////////////////////////
void
IN_AckBack(void)
{
	word	i;

	while (!LastScan)
	{
		SDL_Event evt;
		if (SDL_PollEvent(&evt))
		{
			INL_HandleSDLEvent(&evt);
		}
		
		if (MousePresent)
		{
			if (INL_GetMouseButtons())
			{
				while (INL_GetMouseButtons())
				{
					SDL_Event evt;
					if (SDL_PollEvent(&evt))
					{
						INL_HandleSDLEvent(&evt);
					}
					VW_GL_Present();
				}
				return;
			}
		}

		for (i = 0;i < MaxJoys;i++)
		{
			if (JoysPresent[i] || GravisGamepad)
			{
				if (IN_GetJoyButtonsDB(i))
				{
					while (IN_GetJoyButtonsDB(i))
					{
						SDL_Event evt;
						if (SDL_PollEvent(&evt))
						{
							INL_HandleSDLEvent(&evt);
						}
						VW_GL_Present();
					}
					return;
				}
			}
		}
		// Make sure the screen is being refreshed so the steam overlay works.
		VW_GL_Present();
	}

	IN_ClearKey(LastScan);
	LastScan = sc_None;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_Ack() - Clears user input & then calls IN_AckBack()
//
///////////////////////////////////////////////////////////////////////////
void
IN_Ack(void)
{
	word	i;

	IN_ClearKey(LastScan);
	LastScan = sc_None;

	if (MousePresent)
		while (INL_GetMouseButtons())
		{
			SDL_Event evt;
			if (SDL_PollEvent(&evt))
			{
				INL_HandleSDLEvent(&evt);
			}
			VW_GL_Present();
		}
	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i] || GravisGamepad)
			while (IN_GetJoyButtonsDB(i))
			{
				SDL_Event evt;
				if (SDL_PollEvent(&evt))
				{
					INL_HandleSDLEvent(&evt);
				}
				VW_GL_Present();
			}

	IN_AckBack();
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_IsUserInput() - Returns true if a key has been pressed or a button
//		is down
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_IsUserInput(void)
{
	boolean	result;
	word	i;
	SDL_Event evt;
	if (SDL_PollEvent(&evt))
		INL_HandleSDLEvent(&evt);

	result = LastScan;

	if (MousePresent)
		if (INL_GetMouseButtons())
			result = true;

	for (i = 0;i < MaxJoys;i++)
		if (JoysPresent[i] || GravisGamepad)
			if (INL_GetJoyButtons(i))
				result = true;

	return(result);
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_UserInput() - Waits for the specified delay time (in ticks) or the
//		user pressing a key or a mouse button. If the clear flag is set, it
//		then either clears the key or waits for the user to let the mouse
//		button up.
//
///////////////////////////////////////////////////////////////////////////
boolean
IN_UserInput(longword delay,boolean clear)
{
	longword	lasttime;

	lasttime = SD_GetTimeCount();
	do
	{
		if (IN_IsUserInput())
		{
			if (clear)
				IN_AckBack();
			return(true);
		}
		VW_GL_Present();
	} while (SD_GetTimeCount() - lasttime < delay);
	return(false);
}
