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
//	ID_IN.h - Header file for Input Manager
//	v1.0d1
//	By Jason Blochowiak
//

#ifndef	__TYPES__
#include "id_types.h"
#endif

#ifndef	__ID_IN__
#define	__ID_IN__

#ifdef	__DEBUG__
#define	__DEBUG_InputMgr__
#endif

#define	MaxPlayers	4
#define	MaxKbds		2
#define	MaxJoys		2
#define	NumCodes	0x10000

#include <SDL.h>
typedef	uint16_t		ScanCode;
#define	sc_None			0
#define	sc_Bad			0xff
#define sc_Return   	0x1c
#define sc_Enter    	sc_Return
#define	sc_Escape		0x01
#define	sc_Space		0x39
#define	sc_BackSpace	0x0e
#define	sc_Tab			0x0f
#define	sc_Alt			0x38
#define	sc_Control		0x1d
#define	sc_CapsLock		0x3a
#define	sc_LShift		0x2a
#define	sc_RShift		0x36
#define	sc_UpArrow		0x48
#define	sc_DownArrow	0x50
#define	sc_LeftArrow	0x4b
#define	sc_RightArrow	0x4d
#define	sc_Insert		0x52
#define	sc_Delete		0x53
#define	sc_Home			0x47
#define	sc_End			0x4f
#define	sc_PgUp			0x49
#define	sc_PgDn			0x51
#define	sc_F1			0x3b
#define	sc_F2			0x3c
#define	sc_F3			0x3d
#define	sc_F4			0x3e
#define	sc_F5			0x3f
#define	sc_F6			0x40
#define	sc_F7			0x41
#define	sc_F8			0x42
#define	sc_F9			0x43
#define	sc_F10			0x44
#define	sc_F11			0x57
#define	sc_F12			0x58

#define	sc_A			SDL_SCANCODE_A
#define	sc_B			SDL_SCANCODE_B
#define	sc_C			SDL_SCANCODE_C
#define	sc_D			SDL_SCANCODE_D
#define	sc_E			SDL_SCANCODE_E
#define	sc_F			SDL_SCANCODE_F
#define	sc_G			SDL_SCANCODE_G
#define	sc_H			SDL_SCANCODE_H
#define	sc_I			SDL_SCANCODE_I
#define	sc_J			SDL_SCANCODE_J
#define	sc_K			SDL_SCANCODE_K
#define	sc_L			SDL_SCANCODE_L
#define	sc_M			SDL_SCANCODE_M
#define	sc_N			SDL_SCANCODE_N
#define	sc_O			SDL_SCANCODE_O
#define	sc_P			SDL_SCANCODE_P
#define	sc_Q			SDL_SCANCODE_Q
#define	sc_R			SDL_SCANCODE_R
#define	sc_S			SDL_SCANCODE_S
#define	sc_T			SDL_SCANCODE_T
#define	sc_U			SDL_SCANCODE_U
#define	sc_V			SDL_SCANCODE_V
#define	sc_W			SDL_SCANCODE_W
#define	sc_X			SDL_SCANCODE_X
#define	sc_Y			SDL_SCANCODE_Y
#define	sc_Z			SDL_SCANCODE_Z

#define sc_1			SDL_SCANCODE_1
#define sc_2			SDL_SCANCODE_2
#define sc_3			SDL_SCANCODE_3
#define sc_4			SDL_SCANCODE_4
#define sc_5			SDL_SCANCODE_5
#define sc_6			SDL_SCANCODE_6
#define sc_7			SDL_SCANCODE_7
#define sc_8			SDL_SCANCODE_8
#define sc_9			SDL_SCANCODE_9
#define sc_0			SDL_SCANCODE_0

#define sc_Comma		SDL_SCANCODE_COMMA
#define sc_Fullstop		SDL_SCANCODE_PERIOD
#define sc_Slash		SDL_SCANCODE_SLASH
#define sc_LBracket		SDL_SCANCODE_LBRACKET
#define sc_RBracket		SDL_SCANCODE_RBRACKET

#define	sc_1			0x02
#define	sc_2			0x03
#define	sc_3			0x04
#define	sc_4			0x05
#define	sc_5			0x06
#define	sc_6			0x07
#define	sc_7			0x08
#define	sc_8			0x09
#define	sc_9			0x0a
#define	sc_0			0x0b

#define	key_None		0
#define	key_Return		SDL_SCANCODE_KP_ENTER
#define	key_Escape		SDL_SCANCODE_ESCAPE
#define	key_Space		SDL_SCANCODE_KP_SPACE
#define	key_BackSpace		SDL_SCANCODE_BACKSPACE
#define	key_Tab			SDL_SCANCODE_KP_TAB
#define	key_Delete		SDL_SCANCODE_DELETE
#define key_Asterisk		SDL_SCANCODE_KP_MULTIPLY
#define key_Plus		SDL_SCANCODE_KP_PLUS
#define key_Minus		SDL_SCANCODE_KP_MINUS

typedef	enum		{
						demo_Off,demo_Record,demo_Playback,demo_PlayDone
					} Demo;
typedef	enum		{
						ctrl_Keyboard,
							ctrl_Keyboard1 = ctrl_Keyboard,ctrl_Keyboard2,
						ctrl_Joystick,
							ctrl_Joystick1 = ctrl_Joystick,ctrl_Joystick2,
						ctrl_Mouse
					} ControlType;
typedef	enum		{
						motion_Left = -1,motion_Up = -1,
						motion_None = 0,
						motion_Right = 1,motion_Down = 1
					} Motion;
typedef	enum		{
						dir_North,dir_NorthEast,
						dir_East,dir_SouthEast,
						dir_South,dir_SouthWest,
						dir_West,dir_NorthWest,
						dir_None
					} Direction;
typedef	enum		{
						ga_Jump,
						ga_Pogo,
						ga_Attack,
						ga_Status
					} GravisAType;
typedef	struct		{
						boolean		button0,button1;
						int			x,y;
						Motion		xaxis,yaxis;
						Direction	dir;
					} CursorInfo;
typedef	CursorInfo	ControlInfo;
typedef	struct		{
						ScanCode	button0,button1,
									upleft,		up,		upright,
									left,				right,
									downleft,	down,	downright;
					} KeyboardDef;
typedef	struct		{
						word		joyMinX,joyMinY,
									threshMinX,threshMinY,
									threshMaxX,threshMaxY,
									joyMaxX,joyMaxY,
									joyMultXL,joyMultYL,
									joyMultXH,joyMultYH;
					} JoystickDef;
// Global variables
extern	boolean		Keyboard[],
					MousePresent,
					JoysPresent[];
extern	Demo		DemoMode;
extern	boolean		Paused;
extern	char		LastASCII;
extern	ScanCode	LastScan;
extern	KeyboardDef	KbdDefs[];
extern	JoystickDef	JoyDefs[];
extern	ControlType	Controls[MaxPlayers];
extern	boolean		GravisGamepad;
extern	word		GravisAction[4];
extern	word		GravisMap[4];

extern	Demo		DemoMode;
extern	byte _seg	*DemoBuffer;
extern	word		DemoOffset,DemoSize;

// Function prototypes
#define	IN_KeyDown(code)	(Keyboard[(code)])
#define	IN_ClearKey(code)	{Keyboard[code] = false;\
							if (code == LastScan) LastScan = sc_None;}

// DEBUG - put names in prototypes
extern	void		IN_Startup(void),IN_Shutdown(void),
					IN_Default(boolean gotit,ControlType in),
					IN_SetKeyHook(void (*)()),
					IN_ClearKeysDown(void),
					IN_ReadCursor(CursorInfo *),
					IN_ReadControl(int,ControlInfo *),
					IN_SetControlType(int,ControlType),
					IN_GetJoyAbs(word joy,int *xp,int *yp),
					IN_SetupJoy(word joy,word minx,word maxx,
								word miny,word maxy),
					IN_StartDemoPlayback(byte _seg *buffer,word bufsize),
					IN_StopDemo(void),IN_FreeDemoBuffer(void),
					IN_Ack(void),IN_AckBack(void);
extern	boolean		IN_UserInput(longword delay,boolean clear),
					IN_IsUserInput(void),
					IN_StartDemoRecord(word bufsize);
extern	char		*IN_GetScanName(ScanCode);
extern	char		IN_WaitForASCII(void);
extern	ScanCode	IN_WaitForKey(void);
extern	word		IN_GetJoyButtonsDB(word joy);
extern	int		IN_GetAllJoyButtons(word joy);
extern	const char*	IN_GetJoyName(word joy);
extern	int		IN_IsJoyController(word joy);
extern	void		IN_PumpEvents(void);
extern	int		IN_ReadWheel(void);
extern	void		IN_SetJoyDelay(int ms);

#endif
