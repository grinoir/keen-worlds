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
//      ID Engine
//      ID_US.c - User Manager - User interface
//      v1.1d1
//      By Jason Blochowiak
//      Hacked up for Catacomb 3D
//

#include "id_heads.h"
#pragma hdrstop

#pragma warn    -pia

// Secret code for unlocking night mode
boolean	enternightmodecode;
const char	nightmodecode[] = "DARKNESSTUBERIA";

// Secret code for unlocking Mortimer
boolean	entermortimercode;
const char	mortimermodecode[] = "GRANDINTELLECT";

// ktx58:
//
// Keen Dreams Gold Edition functions
// These are actually defined in kdg_def.h. However, as the inclusion of 
// kdg_def.h the "ID Engine"  source files (id_*.c) would cause problems,
// the required functions are defined here.

void StartMusic (int num);
void StopMusic (void);
void EndGame (void);
void HelpScreens (void);

//      Special imports
extern  boolean         showscorebox;
extern  ScanCode        attackscan;
extern  boolean         oldattacking;
extern  boolean         mortimerunlocked;
extern  boolean         mortimerbedgfx;
extern  boolean         mortwindow;
extern  boolean         nightmode;
extern  boolean         greenball;

//      Global variables
		boolean         ingame,abortgame,loadedgame;
		GameDiff        restartgame = gd_Continue;

//      Internal variables
static  boolean         GameIsDirty,
					QuitToDos,
					CtlPanelDone;

//      Forward reference prototypes
 void     USL_SetupCard(void);

//      Control panel data

#define CtlPanelSX      74
#define CtlPanelSY      48
#define CtlPanelEX      234
#define CtlPanelEY      150
#define CtlPanelW       (CtlPanelEX - CtlPanelSX)
#define CtlPanelH       (CtlPanelEY - CtlPanelSY)

#define TileBase        54		// Keen
#define MTileBase       76		// Mortimer

// Hacks for Keen Dreams Gold Edition - KEEN
#ifdef EGAGR
#define BackColor               8
#define HiliteColor             3
#define NohiliteColor   11
#else // CGAGR
#define BackColor               0
#define HiliteColor             2
#define NohiliteColor   1
#endif

// Hacks for Keen Dreams Gold Edition - MORTIMER
#ifdef EGAGR
#define MBackColor               F_BLACK
#else // CGAGR
#define MBackColor               2
#endif

typedef enum
		{
			uc_None,
			uc_Return,
			uc_Abort,
			uc_Quit,
			uc_Loaded,
			uc_SEasy,
			uc_SNormal,
			uc_SHard,
			uc_SKeen,
			uc_SGrand,
		} UComm;
typedef enum
		{
			uii_Bad,
			uii_Button,uii_RadioButton,uii_Folder
		} UIType;
typedef enum
		{
			ui_Normal = 0,
			ui_Pushed = 1,
			ui_Selected = 2,
			ui_Disabled = 4,
			ui_Separated = 8
		} UIFlags;
#define UISelectFlags (ui_Pushed | ui_Selected | ui_Disabled)

typedef enum
		{
			uic_SetupCard,uic_DrawCard,uic_TouchupCard,
			uic_DrawIcon,uic_Draw,uic_Hit
		} UserCall;

typedef struct  UserItem
		{
				UIType                  type;
				UIFlags                 flags;
				ScanCode                hotkey;
				char                    *text;
				UComm                   comm;
				void                    far *child;     // Should be (UserItemGroup *)

				word                    x,y;
		} UserItem;
typedef struct  UserItemGroup
		{
				word                    x,y;
				graphicnums             title;
				graphicnums		morttitle;
				ScanCode                hotkey;
				UserItem                far *items;
				boolean                 (*custom)(UserCall,struct UserItem far *);      // Custom routine

				word                    cursor;
		struct  UserItemGroup   far *parent;
		} UserItemGroup;

static  char            *BottomS1,*BottomS2,*BottomS3,*BottomS4;
static  UComm           Communication;
static  ScanCode        *KeyMaps[] =
					{
						&KbdDefs[0].button0,
						&KbdDefs[0].button1,
						&attackscan,
						&KbdDefs[0].upleft,
						&KbdDefs[0].up,
						&KbdDefs[0].upright,
						&KbdDefs[0].right,
						&KbdDefs[0].downright,
						&KbdDefs[0].down,
						&KbdDefs[0].downleft,
						&KbdDefs[0].left
					};

// Custom routine prototypes
  boolean USL_ConfigCustom(UserCall call,struct UserItem far *item),
				USL_KeyCustom(UserCall call,struct UserItem far *item),
				USL_KeySCustom(UserCall call,struct UserItem far *item),
				USL_Joy1Custom(UserCall call,struct UserItem far *item),
				USL_Joy2Custom(UserCall call,struct UserItem far *item),
				USL_JoyGCustom(UserCall call,struct UserItem far *item),
				USL_LoadCustom(UserCall call,struct UserItem far *item),
				USL_SaveCustom(UserCall call,struct UserItem far *item),
				USL_ScoreCustom(UserCall call,struct UserItem far *item),
				USL_CompCustom(UserCall call,struct UserItem far *item),
				USL_TwoCustom(UserCall call,struct UserItem far *item),
				USL_SoundOffCustom(UserCall call,struct UserItem far *item),
				USL_SoundPCCustom(UserCall call,struct UserItem far *item),
				USL_SoundAdLibCustom(UserCall call,struct UserItem far *item),
				USL_SoundQuietAdLibCustom(UserCall call,struct UserItem far *item),
				USL_MusicOnCustom(UserCall call,struct UserItem far *item),
				USL_MusicOffCustom(UserCall call,struct UserItem far *item),
				USL_PongCustom(UserCall call,struct UserItem far *item);

#define DefButton(key,text)                             uii_Button,ui_Normal,key,text
#define DefRButton(key,text)                    uii_RadioButton,ui_Normal,key,text
#define DefFolder(key,text,child)               uii_Folder,ui_Normal,key,text,uc_None,child
#define CustomGroup(title,morttitle,key,custom)   0,0,title,morttitle,key,0,custom
	UserItem far holder[] =
	{
		{DefButton(sc_None,"DEBUG")},
		{uii_Bad}
	};
	UserItemGroup   far holdergroup = {0,0,CP_MAINMENUPIC,CP_MMAINMENUPIC,sc_None,holder};

	// Sound menu
	UserItemGroup   far nosoundgroup = {0,0,0,0,sc_None,0,USL_SoundOffCustom};
	UserItemGroup   far pcsoundgroup = {0,0,0,0,sc_None,0,USL_SoundPCCustom};
	UserItemGroup   far adlibsoundgroup = {0,0,0,0,sc_None,0,USL_SoundAdLibCustom};
	UserItemGroup   far quietadlibsoundgroup = {0,0,0,0,sc_None,0,USL_SoundQuietAdLibCustom};

	UserItem far soundi[] =
	{
		{DefFolder(sc_None,"NO SOUND EFFECTS",&nosoundgroup)},
		{DefFolder(sc_None,"PC SPEAKER",&pcsoundgroup)},
		{DefFolder(sc_None,"ADLIB/SOUND BLASTER",&adlibsoundgroup)},
		{DefFolder(sc_None,"QUIET ADLIB/SOUND BLASTER",&quietadlibsoundgroup)},
		{uii_Bad}
	};
	UserItemGroup   far soundgroup = {8,0,CP_SOUNDMENUPIC,CP_MSOUNDMENUPIC,sc_None,soundi};

	// Music menu
	UserItemGroup   far nomusicgroup = {0,0,0,0,sc_None,0,USL_MusicOffCustom};
	UserItemGroup   far yesmusicgroup = {0,0,0,0,sc_None,0,USL_MusicOnCustom};

	UserItem far musici[] =
	{
		{DefFolder(sc_None,"NO MUSIC",&nomusicgroup)},
		{DefFolder(sc_None,"ADLIB/SOUND BLASTER",&yesmusicgroup)},
		{uii_Bad}
	};
	UserItemGroup   far musicgroup = {8,0,CP_MUSICMENUPIC,CP_MMUSICMENUPIC,sc_None,musici};

	// New game menu
	UserItem far newgamei[] =
	{
		{DefButton(sc_None,"BEGIN EASY GAME"),uc_SEasy},
		{DefButton(sc_None,"BEGIN NORMAL GAME"),uc_SNormal},
		{DefButton(sc_None,"BEGIN HARD GAME"),uc_SHard},
		{DefButton(sc_None,"BEGIN KEEN GAME"),uc_SKeen},
		{DefButton(sc_None,nil),uc_SGrand},
		{uii_Bad}
	};
	UserItemGroup   far newgamegroup = {8,0,CP_NEWGAMEMENUPIC,CP_MNEWGAMEMENUPIC,sc_None,newgamei,0,0};

	// Load/Save game menu
	UserItem far loadsavegamei[] =
	{
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Button,ui_Normal,sc_None},
		{uii_Bad}
	};
	UserItemGroup   far loadgamegroup = {4,3,CP_LOADMENUPIC,CP_MLOADMENUPIC,sc_None,loadsavegamei,USL_LoadCustom};
	UserItemGroup   far savegamegroup = {4,3,CP_SAVEMENUPIC,CP_MSAVEMENUPIC,sc_None,loadsavegamei,USL_SaveCustom};

	// Options menu
	UserItemGroup   far scoregroup = {0,0,0,0,sc_None,0,USL_ScoreCustom};
	UserItemGroup   far compgroup = {0,0,0,0,sc_None,0,USL_CompCustom};
	UserItemGroup   far twogroup = {0,0,0,0,sc_None,0,USL_TwoCustom};

	UserItem far optionsi[] =
	{
		{DefFolder(sc_None,"",&scoregroup)},
		{DefFolder(sc_None,"",&compgroup)},
		{DefFolder(sc_None,"",&twogroup)},
		{uii_Bad}
	};
	UserItemGroup   far optionsgroup = {8,0,CP_OPTIONSMENUPIC,CP_MOPTIONSMENUPIC,sc_None,optionsi};

	// Keyboard menu
	UserItem far keyi[] =
	{
		{DefButton(sc_None,"UP & LEFT")},
		{DefButton(sc_None,"UP")},
		{DefButton(sc_None,"UP & RIGHT")},
		{DefButton(sc_None,"RIGHT")},
		{DefButton(sc_None,"DOWN & RIGHT")},
		{DefButton(sc_None,"DOWN")},
		{DefButton(sc_None,"DOWN & LEFT")},
		{DefButton(sc_None,"LEFT")},
		{uii_Bad}
	};
	UserItemGroup   far keygroup = {0,0,CP_KEYMOVEMENTPIC,CP_MKEYMOVEMENTPIC,sc_None,keyi,USL_KeyCustom};
	UserItem far keybi[] =
	{
		{DefButton(sc_None,"JUMP")},
		{DefButton(sc_None,"POGO")},
		{DefButton(sc_None,"ATTACK")},
		{uii_Bad}
	};
	UserItemGroup   far keybgroup = {0,0,CP_KEYBUTTONPIC,CP_MKEYBUTTONPIC,sc_None,keybi,USL_KeyCustom};
	UserItem far keysi[] =
	{
		{DefFolder(sc_None,"MOVEMENT",&keygroup)},
		{DefFolder(sc_None,"BUTTON",&keybgroup)},
		{uii_Bad}
	};
	UserItemGroup   far keysgroup = {8,0,CP_KEYBOARDMENUPIC,CP_MKEYBOARDMENUPIC,sc_None,keysi,USL_KeySCustom};

	// Joystick #1 & #2 and Gravis Gamepad
	UserItemGroup   far joy1group = {CustomGroup(CP_JOYSTICK1PIC,CP_MJOYSTICK1PIC,sc_None,USL_Joy1Custom)};
	UserItemGroup   far joy2group = {CustomGroup(CP_JOYSTICK2PIC,CP_MJOYSTICK2PIC,sc_None,USL_Joy2Custom)};
	UserItemGroup   far gravisgroup = {CustomGroup(CP_GRAVISPIC,CP_MGRAVISPIC,sc_None,USL_JoyGCustom)};

	// Config menu
	UserItem far configi[] =
	{
		{DefFolder(sc_None,"SOUND",&soundgroup)},
		{DefFolder(sc_None,"MUSIC",&musicgroup)},
		{DefFolder(sc_None,"OPTIONS",&optionsgroup)}, // Bring this back for Keen Dreams Gold
		{uii_Folder,ui_Separated,sc_None,"USE KEYBOARD",uc_None,&keysgroup},
		{DefFolder(sc_None,"USE JOYSTICK #1",&joy1group)},
		{DefFolder(sc_None,"USE JOYSTICK #2",&joy2group)},
		{DefFolder(sc_None,"",&gravisgroup)},
		{uii_Bad}
	};
	UserItemGroup   far configgroup = {8,0,CP_CONFIGMENUPIC,CP_MCONFIGMENUPIC,sc_None,configi,USL_ConfigCustom};

	// Main menu
	UserItemGroup   far ponggroup = {0,0,0,0,sc_None,0,USL_PongCustom};
	UserItem far rooti[] =
	{
		{DefFolder(sc_None,"NEW GAME",&newgamegroup)},
		{DefFolder(sc_None,"LOAD GAME",&loadgamegroup)},
		{DefFolder(sc_None,"SAVE GAME",&savegamegroup)},
		{DefFolder(sc_None,"CONFIGURE",&configgroup)},
		{DefButton(sc_None,nil),uc_Return},        // Return to Game/Demo
		{DefButton(sc_None,"END GAME"),uc_Abort},
		{DefFolder(sc_None,"FLOWER PONG",&ponggroup)},
		{DefButton(sc_None,"QUIT"),uc_Quit},
		{uii_Bad}
	};
	UserItemGroup   far rootgroup = {32,4,CP_MAINMENUPIC,CP_MMAINMENUPIC,sc_None,rooti};
#undef  DefButton
#undef  DefFolder

#define MaxCards        7
	word                    cstackptr;
	UserItemGroup   far *cardstack[MaxCards],
					far *topcard;

//      Card stack code
 void
USL_SetupStack(void)
{
	cstackptr = 0;
	cardstack[0] = topcard = &rootgroup;
}

 void
USL_PopCard(void)
{
	if (!cstackptr)
		return;

	topcard = cardstack[--cstackptr];
}

 void
USL_PushCard(UserItemGroup far *card)
{
	if (cstackptr == MaxCards - 1)
		return;

	topcard = cardstack[++cstackptr] = card;
}

 void
USL_DrawItemIcon(UserItem far *item)
{
	word    flags,tile;

	if (topcard->custom && topcard->custom(uic_DrawIcon,item))
		return;

	flags = item->flags;
	if (flags & ui_Disabled)
		if (mortimerbedgfx)
		{
			tile = MTileBase + ((flags & ui_Selected)? 3 : 2);
		}
		else
		{
			tile = TileBase + ((flags & ui_Selected)? 5 : 4);
		}
	else
		if (mortimerbedgfx)
		{
			tile = MTileBase + ((flags & ui_Selected)? 1 : 0);
		}
		else
		{
			tile = TileBase + ((flags & ui_Selected)? 1 : 0);
		}
	VWB_DrawTile8(item->x,item->y,tile);
}

 void
USL_DrawItem(UserItem far *item)
{
	if (topcard->custom && topcard->custom(uic_Draw,item))
		return;

	if (mortimerbedgfx)
	{
		VWB_Bar(CtlPanelSX + 1,item->y,
				CtlPanelEX - CtlPanelSX - 1,8,MBackColor);       // Clear out background
		USL_DrawItemIcon(item);
		if ((item->flags & ui_Selected) && !(item->flags & ui_Disabled))
			fontcolor = 3;
		else
			fontcolor = 11;
	}
	else
	{
		VWB_Bar(CtlPanelSX + 1,item->y,
				CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
		USL_DrawItemIcon(item);
		if ((item->flags & ui_Selected) && !(item->flags & ui_Disabled))
			fontcolor = HiliteColor;
		else
			fontcolor = NohiliteColor;
	}
	px = item->x + 8;
	py = item->y + 1;
	USL_DrawString(item->text);
	fontcolor = F_BLACK;
}

// Color hack for Keen Dreams Gold is here too
#define MyLine(y)       VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,y,NohiliteColor);
#define MMyLine(y)      VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,y,0);

 void
USL_DrawBottom(void)
{
	word    w,h;

	if (mortimerbedgfx)
	{
		fontcolor = 11;
	}
	else
	{
		fontcolor = NohiliteColor;
	}

	px = CtlPanelSX + 4;
	py = CtlPanelEY - 15;
	USL_DrawString(BottomS1);

	USL_MeasureString(BottomS2,&w,&h);
	px = CtlPanelEX - 4 - w;
	USL_DrawString(BottomS2);

	USL_MeasureString(BottomS3,&w,&h);
	px = CtlPanelSX + ((CtlPanelEX - CtlPanelSX - w) / 2);
	py += h + 1;
	USL_DrawString(BottomS3);

	fontcolor = F_WHITE;
	if (mortimerbedgfx)
	{
		MMyLine(CtlPanelEY - 17);
	}
	else
	{
		MyLine(CtlPanelEY - 17);
	}
}

 void
USL_DrawCtlPanelContents(void)
{
	int                             x,y;
	UserItem                far *item;

	if (topcard->custom && topcard->custom(uic_DrawCard,nil))
		return;

	if (topcard->title)
	{
		// Draw the title
		MyLine(CtlPanelSY + 7);
		CA_CacheGrChunk(topcard->title);
		VWB_DrawPic(CtlPanelSX + 6,CtlPanelSY,topcard->title);
	}

	if (mortimerbedgfx)
	{
		if (topcard->morttitle)
		{
			// Draw the title for Mortimer
			MMyLine(CtlPanelSY + 7);
			CA_CacheGrChunk(topcard->morttitle);
			VWB_DrawPic(CtlPanelSX + 6,CtlPanelSY,topcard->morttitle);
		}
	}

	USL_DrawBottom();

	if (!topcard->items)
		return;

	x = topcard->x + CtlPanelSX;
	if (x % 8)
		x += 8 - (x % 8);
	y = topcard->y + CtlPanelSY + 12;
	for (item = topcard->items;item->type != uii_Bad;item++)
	{
		if (item->flags & ui_Separated)
			y += 8;

		item->x = x;
		item->y = y;
		USL_DrawItem(item);
		y += 8;
	}
	if (topcard->custom)
		topcard->custom(uic_TouchupCard,nil);
}

 void
USL_DrawCtlPanel(void)
{
	if (topcard->items || topcard->title)
	{
		// Draw the backdrop
		CA_CacheGrChunk(CP_MENUSCREENPIC);
		VWB_DrawPic(0,0,CP_MENUSCREENPIC);

		// Draw the contents
		USL_DrawCtlPanelContents();
	}

	if (mortimerbedgfx)
	{
		if (topcard->items || topcard->morttitle)
		{
			// Draw the backdrop
			CA_CacheGrChunk(CP_MMENUSCREENPIC);
			VWB_DrawPic(0,0,CP_MMENUSCREENPIC);

			// Draw the contents
			USL_DrawCtlPanelContents();
		}
	}

	// Refresh the screen
	VW_UpdateScreen();
}

 void
USL_DialogSetup(word w,word h,word *x,word *y)
{
	if (Keyboard[sc_P])
	{
		CA_CacheGrChunk(PW_MENUMASKPICM);
		VWB_DrawMPic(75,60,PW_MENUMASKPICM);
	}
	else
	{
		CA_CacheGrChunk(CP_MENUMASKPICM);
		VWB_DrawMPic(CtlPanelSX,CtlPanelSY,CP_MENUMASKPICM);
	}

	*x = CtlPanelSX + ((CtlPanelW - w) / 2);
	*y = CtlPanelSY + ((CtlPanelH - h) / 2);
	if (mortimerbedgfx)
	{
		VWB_Bar(*x,*y,w + 1,h + 1,MBackColor);
		VWB_Hlin(*x - 1,*x + w + 1,*y - 1,0);
		VWB_Hlin(*x - 1,*x + w + 1,*y + h + 1,0);
		VWB_Vlin(*y - 1,*y + h + 1,*x - 1,0);
		VWB_Vlin(*y - 1,*y + h + 1,*x + w + 1,0);
	}
	else
	{
		VWB_Bar(*x,*y,w + 1,h + 1,BackColor);
		VWB_Hlin(*x - 1,*x + w + 1,*y - 1,NohiliteColor);
		VWB_Hlin(*x - 1,*x + w + 1,*y + h + 1,NohiliteColor);
		VWB_Vlin(*y - 1,*y + h + 1,*x - 1,NohiliteColor);
		VWB_Vlin(*y - 1,*y + h + 1,*x + w + 1,NohiliteColor);
	}
}

 void
USL_ShowLoadSave(char *s,char *name)
{
	word    x,y,
			w,h,
			tw,sw;
	char    msg[MaxGameName + 4];

	strcpy(msg,"'");
	strcat(msg,name);
	strcat(msg,"'");
	USL_MeasureString(s,&sw,&h);
	USL_MeasureString(msg,&w,&h);
	tw = ((sw > w)? sw : w) + 6;
	USL_DialogSetup(tw,(h * 2) + 2,&x,&y);
	py = y + 2;
	px = x + ((tw - sw) / 2);
	USL_DrawString(s);
	py += h;
	px = x + ((tw - w) / 2);
	USL_DrawString(msg);

	VW_UpdateScreen();

	IN_UserInput(100, true);
}

 boolean
USL_CtlDialog(char *s1,char *s2,char *s3)
{
	word            w,h,sh,
				w1,w2,w3,
				x,y;
	ScanCode        c;
	CursorInfo      cursorinfo;

	USL_MeasureString(s1,&w1,&h);
	USL_MeasureString(s2,&w2,&h);
	if (s3)
		USL_MeasureString(s3,&w3,&h);
	else
		w3 = 0;
	w = (w1 > w2)? ((w1 > w3)? w1 : w3) : ((w2 > w3)? w2 : w3);
	w += 7;
	sh = h;
	h *= s3? 5 : 4;

	USL_DialogSetup(w,h,&x,&y);

	if (mortimerbedgfx)
	{
		fontcolor = 3;
	}
	else
	{
		fontcolor = HiliteColor;
	}
	px = x + ((w - w1) / 2);
	py = y + sh + 1;
	USL_DrawString(s1);
	py += (sh * 2) - 1;

	if (mortimerbedgfx)
	{
		VWB_Hlin(x + 3,x + w - 3,py,0);
	}
	else
	{
		VWB_Hlin(x + 3,x + w - 3,py,NohiliteColor);
	}
	py += 2;

	if (mortimerbedgfx)
	{
		fontcolor = 11;
	}
	else
	{
		fontcolor = NohiliteColor;
	}
	px = x + ((w - w2) / 2);
	USL_DrawString(s2);
	py += sh;

	if (s3)
	{
		px = x + ((w - w3) / 2);
		USL_DrawString(s3);
	}

	VW_UpdateScreen();

	IN_ClearKeysDown();
	do
	{
		IN_ReadCursor(&cursorinfo);
		if (cursorinfo.button0)
			c = sc_Y;
		else if (cursorinfo.button1)
			c = sc_Escape;
		else
			c = LastScan;
	} while (c == sc_None);
	do
	{
		IN_ReadCursor(&cursorinfo);
	} while (cursorinfo.button0 || cursorinfo.button1);

	IN_ClearKeysDown();
	USL_DrawCtlPanel();
	return(c == sc_Y);
}

 boolean
USL_ConfirmComm(UComm comm)
{
	boolean confirm,dialog;
	char    *s1,*s2,*s3;

	if (!comm)
		Quit("USL_ConfirmComm() - empty comm");

	confirm = true;
	dialog = false;
	s3 = "ESC TO BACK OUT";
	switch (comm)
	{
	case uc_Abort:
		s1 = "REALLY END CURRENT GAME?";
		s2 = "PRESS Y TO END IT";
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_Quit:
		s1 = "REALLY QUIT?";
		s2 = "PRESS Y TO QUIT";
		dialog = true;
		break;
	case uc_Loaded:
		s1 = "YOU'RE IN A GAME";
		s2 = "PRESS Y TO LOAD GAME";
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	case uc_SEasy:
	case uc_SNormal:
	case uc_SHard:
	case uc_SKeen:
	case uc_SGrand:
		s1 = "YOU'RE IN A GAME";
		s2 = "PRESS Y FOR NEW GAME";
		if (ingame && GameIsDirty)
			dialog = true;
		break;
	}

	confirm = dialog? USL_CtlDialog(s1,s2,s3) : true;
	if (confirm)
	{
		Communication = comm;
		CtlPanelDone = true;
	}
	return(confirm);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_HandleError() - Handles telling the user that there's been an error
//
///////////////////////////////////////////////////////////////////////////
 void
USL_HandleError(int num)
{
	char    buf[64];

	strcpy(buf,"Error: ");
	if (num < 0)
		strcat(buf,"Unknown");
	else if (num == ENOMEM)
		strcat(buf,"Disk is Full");
	else if (num == EINVFMT)
		strcat(buf,"File is Incomplete");
	else
		strcat(buf,strerror(num));

	VW_HideCursor();

	USL_CtlDialog(buf,"PRESS ANY KEY",nil);
	VW_UpdateScreen();

	IN_ClearKeysDown();
	IN_Ack();

	VW_ShowCursor();
	VW_UpdateScreen();
}

//      Custom routines
#if 0
 boolean
USL_GenericCustom(UserCall call,UserItem far *item)
{
	boolean result;

	result = false;
	switch (call)
	{
	}
	return(result);
}
#endif

 void
USL_SetOptionsText(void)
{
	optionsi[0].text = showscorebox? "SCORE BOX (ON)" : "SCORE BOX (OFF)";
	optionsi[1].text = compatibility? "SVGA COMPATIBILITY (ON)" : "SVGA COMPATIBILITY (OFF)";
	optionsi[2].text = oldattacking? "TWO-BUTTON ATTACK (ON)" : "TWO-BUTTON ATTACK (OFF)";

	keybi[2].flags &= ~ui_Disabled;
	if (oldattacking)
		keybi[2].flags |= ui_Disabled;

	// gravis option is only enabled when a joystick is selected
	configi[6].flags |= ui_Disabled;
	if (Controls[0] == ctrl_Joystick1 || Controls[0] == ctrl_Joystick2)
		configi[6].flags &= ~ui_Disabled;

	configi[6].text = GravisGamepad? "USE GRAVIS GAMEPAD (ON)" : "USE GRAVIS GAMEPAD (OFF)";
}

#pragma argsused
 boolean
USL_ScoreCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	showscorebox ^= true;
	USL_CtlDialog(showscorebox? "Score box now on" : "Score box now off",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_CompCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	compatibility ^= true;
	USL_CtlDialog(compatibility? "SVGA compatibility now on" : "SVGA compatibility now off",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}


#pragma argsused
 boolean
USL_TwoCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	oldattacking ^= true;
	USL_CtlDialog(oldattacking? "Two-button attack now on" : "Two-button attack now off",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_MusicOffCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	if (MusicMode!=smm_Off)
	{
		SD_SetMusicMode(smm_Off);
	}
	USL_CtlDialog("Music turned off",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_MusicOnCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	if (MusicMode!=smm_AdLib)
	{
		SD_SetMusicMode(smm_AdLib);
		StartMusic(53);
	}
	USL_CtlDialog("Music turned on",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_SoundOffCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	if (SoundMode!=sdm_Off)
	{
		SD_WaitSoundDone();
		SD_SetSoundMode(sdm_Off);
	}

	USL_CtlDialog("No sound effects",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_SoundPCCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	if (SoundMode!=sdm_PC)
	{
		SD_WaitSoundDone();
		SD_SetSoundMode(sdm_PC);
		CA_LoadAllSounds();
	}

	USL_CtlDialog("PC Speaker sound effects",
					"Press any key",nil);
	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_SoundAdLibCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	QuietFX ^= true;
	if (SoundMode!=sdm_AdLib)
	{
		SD_WaitSoundDone();
		QuietFX = false;
		SD_SetSoundMode(sdm_AdLib);
		CA_LoadAllSounds();
	}
	else
	{
		QuietFX = false;
	}

	USL_CtlDialog("AdLib sound effects",
					"Press any key",nil);

	USL_SetOptionsText();
	return(true);
}

#pragma argsused
 boolean
USL_SoundQuietAdLibCustom(UserCall call,UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	QuietFX ^= false;
	if (SoundMode!=sdm_AdLib)
	{
		SD_WaitSoundDone();
		SD_SetSoundMode(sdm_AdLib);
		CA_LoadAllSounds();
	}
	else
	{
		QuietFX = true;
	}

	USL_CtlDialog("Quiet AdLib sound effects",
					"Press any key",nil);

	USL_SetOptionsText();
	return(true);
}

 boolean
USL_ConfigCustom(UserCall call,UserItem far *item)
{
static  char    *CtlNames[] = {"KEYBOARD","KEYBOARD","JOYSTICK #1","JOYSTICK #2","MOUSE"};
		char    *s;
		word    w,h,
				tw;

	if (call == uic_TouchupCard)
	{
		s = "CONTROL: ";
		USL_MeasureString(s,&w,&h);
		tw = w;
		USL_MeasureString(CtlNames[Controls[0]],&w,&h);
		tw += w;
		py = CtlPanelEY - 18 - h;
		px = CtlPanelSX + ((CtlPanelW - tw) / 2);
		if (mortimerbedgfx)
		{
			fontcolor = 11;
		}
		else
		{
			fontcolor = NohiliteColor;
		}
		USL_DrawString(s);
		USL_DrawString(CtlNames[Controls[0]]);
	}
	item++; // Shut the compiler up
	return(false);
}

 void
USL_CKSetKey(UserItem far *item,word i)
{
	boolean         on;
	word            j;
	ScanCode        scan;
	longword        time;
	CursorInfo      cursorinfo;

	on = false;
	time = 0;
	LastScan = sc_None;
	if (mortimerbedgfx)
	{
		fontcolor = 3;
	}
	else
	{
		fontcolor = HiliteColor;
	}
	do
	{
		if (TimeCount >= time)
		{
			on ^= true;
			if (mortimerbedgfx)
			{
				VWB_Bar(item->x + 90,item->y,40,8,fontcolor ^ MBackColor);
				VWB_Bar(item->x + 90 + 1,item->y + 1,40 - 2,8 - 2,MBackColor);
			}
			else
			{
				VWB_Bar(item->x + 90,item->y,40,8,fontcolor ^ BackColor);
				VWB_Bar(item->x + 90 + 1,item->y + 1,40 - 2,8 - 2,BackColor);
			}
			if (on)
				if (mortimerbedgfx)
				{
					VWB_DrawTile8(item->x + 90 + 16,item->y,MTileBase + 6);
				}
				else
				{
					VWB_DrawTile8(item->x + 90 + 16,item->y,TileBase + 8);
				}
			VW_UpdateScreen();

			time = TimeCount + (TickBase / 2);
		}

		IN_ReadCursor(&cursorinfo);
		while (cursorinfo.button0 || cursorinfo.button1)
		{
			IN_ReadCursor(&cursorinfo);
			LastScan = sc_Escape;
		}

#if 0	// this hack is no longer needed, keyboard handler now skips LShift prefixes	--K1n9_Duk3
	asm     pushf
	asm     cli
		if (LastScan == sc_LShift)
			LastScan = sc_None;
	asm     popf
#endif
	} while (!(scan = LastScan));

	if (scan != sc_Escape)
	{
		for (j = 0,on = false;j < 11;j++)
		{
			if (j == i)
				continue;
			if (*(KeyMaps[j]) == scan)
			{
				on = true;
				break;
			}
		}
		if (on)
			USL_CtlDialog("Key already used","Press a key",nil);
		else
			*(KeyMaps[i]) = scan;
	}
	IN_ClearKeysDown();
}

#pragma argsused
 boolean
USL_KeySCustom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
	{
		Controls[0] = ctrl_Keyboard;
		GravisGamepad = false;
		USL_SetOptionsText();
	}
	return(false);
}

#pragma argsused
 boolean
USL_KeyCustom(UserCall call,UserItem far *item)
{
	boolean result;
	word    i;

	result = false;
	i = (topcard == &keygroup)? (3 + (item - keyi)) : (item - keybi);

	switch (call)
	{
	case uic_SetupCard:
		Controls[0] = ctrl_Keyboard;
		break;
	case uic_Draw:
		if (mortimerbedgfx)
		{
			VWB_Bar(CtlPanelSX + 1,item->y,
					CtlPanelEX - CtlPanelSX - 1,8,MBackColor);       // Clear out background
		}
		else
		{
			VWB_Bar(CtlPanelSX + 1,item->y,
					CtlPanelEX - CtlPanelSX - 1,8,BackColor);       // Clear out background
		}
		USL_DrawItemIcon(item);
		if (mortimerbedgfx)
		{
			fontcolor = (item->flags & ui_Selected)? 3 : 11;
		}
		else
		{
			fontcolor = (item->flags & ui_Selected)? HiliteColor : NohiliteColor;
		}
		px = item->x + 8;
		py = item->y + 1;
		USL_DrawString(item->text);
		if (mortimerbedgfx)
		{
			VWB_Bar(item->x + 90,item->y,40,8,fontcolor ^ MBackColor);
			VWB_Bar(item->x + 90 + 1,item->y + 1,40 - 2,8 - 2,MBackColor);
		}
		else
		{
			VWB_Bar(item->x + 90,item->y,40,8,fontcolor ^ BackColor);
			VWB_Bar(item->x + 90 + 1,item->y + 1,40 - 2,8 - 2,BackColor);
		}
		px = item->x + 90 + 6;
		py = item->y + 1;
		USL_DrawString(IN_GetScanName(*KeyMaps[i]));
		result = true;
		break;
	case uic_Hit:
		USL_KeyCustom(uic_Draw,item);
		USL_CKSetKey(item,i);
		USL_DrawCtlPanel();
		result = true;
		break;
	}
	return(result);
}

 void
USL_CJDraw(char *s1,char *s2)
{
	word    w,h;

	USL_MeasureString(s1,&w,&h);
	px = CtlPanelSX + ((CtlPanelW - w) / 2);
	py = CtlPanelEY - 34;
	if (mortimerbedgfx)
	{
		VWB_Bar(CtlPanelSX + 1,py,CtlPanelW - 2,h * 2,MBackColor);
		fontcolor = 3;
	}
	else
	{
		VWB_Bar(CtlPanelSX + 1,py,CtlPanelW - 2,h * 2,BackColor);
		fontcolor = HiliteColor;
	}
	USL_DrawString(s1);
	py += h;
	USL_MeasureString(s2,&w,&h);
	px = CtlPanelSX + ((CtlPanelW - w) / 2);
	USL_DrawString(s2);
}

 boolean
USL_CJGet(word joy,word button,word x,word y,word *xaxis,word *yaxis)
{
	boolean         on;
	longword        time;

	while (IN_GetJoyButtonsDB(joy))
		if (LastScan == sc_Escape)
			return(false);

	on = false;
	time = 0;
	while (!(IN_GetJoyButtonsDB(joy) & (1 << button)))
	{
		if (TimeCount >= time)
		{
			on ^= true;
			time = TimeCount + (TickBase / 2);
			if (mortimerbedgfx)
			{
				VWB_DrawTile8(x,y,MTileBase + on);
			}
			else
			{
				VWB_DrawTile8(x,y,TileBase + on);
			}
			VW_UpdateScreen();
		}

		if (LastScan == sc_Escape)
			return(false);
	}
	IN_GetJoyAbs(joy,xaxis,yaxis);
	return(true);
}

 boolean
USL_ConfigJoystick(word joy)
{
	word    x,y,
			minx,miny,
			maxx,maxy;

	BottomS1 = BottomS2 = "";
	BottomS3 = "Esc to back out";
	USL_DrawCtlPanel();
	x = CtlPanelSX + 60;
	y = CtlPanelSY + 19;
	if (mortimerbedgfx)
	{
		CA_CacheGrChunk(CP_MJOYSTICKPIC);
		VWB_DrawPic(x,y,CP_MJOYSTICKPIC);
	}
	else
	{
		CA_CacheGrChunk(CP_JOYSTICKPIC);
		VWB_DrawPic(x,y,CP_JOYSTICKPIC);
	}

	USL_CJDraw("Move joystick to upper left","and press button #1");
	if (mortimerbedgfx)
	{
		VWB_DrawTile8(x + 24,y + 8,MTileBase + 4);
		VWB_DrawTile8(x + 8,y + 8,MTileBase + 1);
		VWB_DrawTile8(x + 8,y + 24,MTileBase + 0);
	}
	else
	{
		VWB_DrawTile8(x + 24,y + 8,TileBase + 6);
		VWB_DrawTile8(x + 8,y + 8,TileBase + 1);
		VWB_DrawTile8(x + 8,y + 24,TileBase + 0);
	}
	VW_UpdateScreen();
	if (!USL_CJGet(joy,0,x + 8,y + 8,&minx,&miny))
		return(false);

	USL_CJDraw("Move joystick to lower right","and press button #2");
	if (mortimerbedgfx)
	{
		VWB_DrawTile8(x + 24,y + 8,MTileBase + 10);
		VWB_DrawTile8(x + 40,y + 24,MTileBase + 5);
		VWB_DrawTile8(x + 8,y + 8,MTileBase + 0);
		VWB_DrawTile8(x + 8,y + 24,MTileBase + 1);
	}
	else
	{
		VWB_DrawTile8(x + 24,y + 8,TileBase + 16);
		VWB_DrawTile8(x + 40,y + 24,TileBase + 7);
		VWB_DrawTile8(x + 8,y + 8,TileBase + 0);
		VWB_DrawTile8(x + 8,y + 24,TileBase + 1);
	}
	VW_UpdateScreen();
	if (!USL_CJGet(joy,1,x + 8,y + 24,&maxx,&maxy))
		return(false);

	while (IN_GetJoyButtonsDB(joy))
		;

	IN_SetupJoy(joy,minx,maxx,miny,maxy);
	return(true);
}

#pragma argsused
 boolean
USL_Joy1Custom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
	{
		if (USL_ConfigJoystick(0))
		{
			Controls[0] = ctrl_Joystick1;
			USL_CtlDialog("USING JOYSTICK #1","PRESS ANY KEY",nil);
		}
		return(true);
	}
	else
		return(false);
}

#pragma argsused
 boolean
USL_Joy2Custom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
	{
		if (USL_ConfigJoystick(1))
		{
			Controls[0] = ctrl_Joystick2;
			USL_CtlDialog("USING JOYSTICK #2","PRESS ANY KEY",nil);
		}
		return(true);
	}
	else
		return(false);
}

 void
USL_CGDraw(char *s1, char *s2, int buttonsDone)
{
	static char *GButtonNames[4] = {"Red","Blue","Yellow","Green"};
	static char *GActionNames[4] = {"Jump","Pogo","Attack","Status"};

	int	i, n;
	char	*actionstr;
	word	w, h;

	VWB_Bar(CtlPanelSX+1, CtlPanelSY+16, CtlPanelW-2, 68, BackColor);
	px = CtlPanelSX+8;
	py = CtlPanelSY+16;
	USL_DrawString("Make sure that the button");
	px = CtlPanelSX+8;
	py = CtlPanelSY+24;
	USL_DrawString("switch is set for 4 buttons");

	for (i=0; i<4; i++)
	{
		px = CtlPanelSX+8;
		py = i*8 + CtlPanelSY+40;
		USL_DrawString(GButtonNames[i]);
		USL_DrawString(":");
		actionstr = "?";
		for (n=0;n<buttonsDone;n++)
		{
			if (GravisMap[n] == i)
				actionstr = GActionNames[n];
		}
		px = CtlPanelSX+56;
		USL_DrawString(actionstr);
	}

	USL_MeasureString(s1, &w, &h);
	px = w;
	USL_MeasureString(s2, &w, &h);
	px = (CtlPanelW-px-w)/2 + CtlPanelSX;
	py = CtlPanelSY+76;
	USL_DrawString(s1);
	USL_DrawString(s2);
	VW_UpdateScreen();
}

 boolean
USL_CGGet(int buttonsDone, char *action)
{
	word	buttons, i, n;

	USL_CGDraw("PRESS BUTTON FOR ", action, buttonsDone);

redo:
	do
	{
		if (LastScan == sc_Escape)
		{
			Keyboard[sc_Escape] = false;
			if (LastScan == sc_Escape)
				LastScan = sc_None;
			return false;
		}

		buttons = IN_GetJoyButtonsDB(2);
		if (!buttons)
			continue;

		for(i=n=0; i<4; i++)
		{
			if (buttons & (1 << i))
				n += i+1;
		}

		if (!n || n >= 5)
			continue;

		n--;
		for (i=0; i<buttonsDone; i++)
		{
			if (GravisMap[i] == n)
				goto redo;
		}

		GravisMap[buttonsDone] = n;
		return true;
	} while (true);
}

#pragma argsused
 boolean
USL_JoyGCustom(UserCall call,UserItem far *item)
{
	if (call == uic_SetupCard)
	{
		if (GravisGamepad)
		{
			GravisGamepad = false;
		}
		else
		{
			BottomS1 = BottomS2 = "";
			BottomS3 = "ESC to back out";
			USL_DrawCtlPanel();
			fontcolor = HiliteColor;
			px = CtlPanelSX + 8;
			py = CtlPanelSX + 8;
			fontcolor = HiliteColor;	// redundant...
			IN_ClearKeysDown();
			if (  USL_CGGet(0, "JUMP")
				&& USL_CGGet(1, "POGO")
				&& USL_CGGet(2, "ATTACK")
				&& USL_CGGet(3, "STATUS") )
			{
				GravisGamepad = true;
				USL_CGDraw("PRESS ANY KEY", "", 4);
				IN_Ack();
			}
			else
			{
				GravisGamepad = false;
			}
		}
		USL_SetOptionsText();
		return true;
	}
	return false;
}

 void
USL_DrawFileIcon(UserItem far *item)
{
	word    color,mortcolor;

	item->y = topcard->y + CtlPanelSY + 12;
	item->y += (item - loadsavegamei) * 11;

	if (mortimerbedgfx)
	{
		fontcolor = (item->flags & ui_Selected)? 3 : 11;
		mortcolor = fontcolor ^ MBackColor;  // Blech!
		VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y,mortcolor);
		VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y + 9,mortcolor);
		VWB_Vlin(item->y,item->y + 9,item->x,mortcolor);
		VWB_Vlin(item->y,item->y + 9,item->x + (CtlPanelW - 12),mortcolor);
	}
	else
	{
		fontcolor = (item->flags & ui_Selected)? HiliteColor : NohiliteColor;
		color = fontcolor ^ BackColor;  // Blech!
		VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y,color);
		VWB_Hlin(item->x,item->x + (CtlPanelW - 12),item->y + 9,color);
		VWB_Vlin(item->y,item->y + 9,item->x,color);
		VWB_Vlin(item->y,item->y + 9,item->x + (CtlPanelW - 12),color);
	}
}

 void
USL_DoLoadGame(UserItem far *item)
{
	char            *filename;
	word            n,
				err;
	int                     file;
	SaveGame        *game;

	if (!USL_ConfirmComm(uc_Loaded))
		return;

	n = item - loadsavegamei;
	game = &Games[n];

	USL_ShowLoadSave("Loading",game->name);

	err = 0;
	filename = USL_GiveSaveName(n);
	if ((file = open(filename,O_BINARY | O_RDONLY)) != -1)
	{
		if (read(file,game,sizeof(*game)) == sizeof(*game))
		{
			if (USL_LoadGame)
				if (!USL_LoadGame(file))
					USL_HandleError(err = errno);
		}
		else
			USL_HandleError(err = errno);
		close(file);
	}
	else
		USL_HandleError(err = errno);
	if (err)
	{
		abortgame = true;
		Communication = uc_None;
		CtlPanelDone = false;
	}
	else
		loadedgame = true;
	game->present = true;

	if (loadedgame)
		Paused = true;

	USL_DrawCtlPanel();
}

 boolean
USL_LoadCustom(UserCall call,UserItem far *item)
{
	boolean result;
	word    i;

	result = false;
	switch (call)
	{
	case uic_SetupCard:
		for (i = 0;i < MaxSaveGames;i++)
		{
			if (Games[i].present)
				loadsavegamei[i].flags &= ~ui_Disabled;
			else
				loadsavegamei[i].flags |= ui_Disabled;
		}
		break;
	case uic_DrawIcon:
		USL_DrawFileIcon(item);
		result = true;
		break;
	case uic_Draw:
		USL_DrawFileIcon(item);
		if (mortimerbedgfx)
		{
			VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,MBackColor);
		}
		else
		{
			VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,BackColor);
		}

		i = item - loadsavegamei;
		if (Games[i].present)
			px = item->x + 2;
		else
			px = item->x + 60;
		py = item->y + 2;
		USL_DrawString(Games[i].present? Games[i].name : "Empty");
		result = true;
		break;
	case uic_Hit:
		USL_DoLoadGame(item);
		result = true;
		break;
	}
	return(result);
}

 void
USL_DoSaveGame(UserItem far *item)
{
	boolean         ok;
	char            *filename;
	word            n,err;
	int         file;
	SaveGame        *game;

	BottomS1 = "Type name";
	BottomS2 = "Enter accepts";
	USL_DrawCtlPanel();

	n = item - loadsavegamei;
	game = &Games[n];
	if (mortimerbedgfx)
	{
		fontcolor = 3;
		VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,MBackColor);
	}
	else
	{
		fontcolor = HiliteColor;
		VWB_Bar(item->x + 1,item->y + 2,CtlPanelW - 12 - 2,7,BackColor);
	}
	game->oldtest = US_SAVEMAGIC;
	ok = US_LineInput(item->x + 2,item->y + 2,
						game->name,game->present? game->name : nil,
						true,MaxGameName,
						CtlPanelW - 22);
	if (!strlen(game->name))
		strcpy(game->name,"Untitled");
	if (ok)
	{
		USL_ShowLoadSave("Saving",game->name);

		filename = USL_GiveSaveName(n);
		err = 0;
		file = open(filename,O_CREAT | O_BINARY | O_WRONLY,
					S_IREAD | S_IWRITE | S_IFREG);
		if (file != -1)
		{
			if (write(file,game,sizeof(*game)) == sizeof(*game))
			{
				if (USL_SaveGame)
					ok = USL_SaveGame(file);
				if (!ok)
					USL_HandleError(err = errno);
			}
			else
				USL_HandleError(err = ((errno == ENOENT)? ENOMEM : errno));
			close(file);
		}
		else
			USL_HandleError(err = ((errno == ENOENT)? ENOMEM : errno));
		if (err)
		{
			remove(filename);
			ok = false;
		}

	}

	if (!game->present)
		game->present = ok;

	if (ok)
		GameIsDirty = false;
	USL_SetupCard();
}

 boolean
USL_SaveCustom(UserCall call,UserItem far *item)
{
	word    i;

	switch (call)
	{
	case uic_SetupCard:
		for (i = 0;i < MaxSaveGames;i++)
			loadsavegamei[i].flags &= ~ui_Disabled;
		return(false);
	case uic_Hit:
		USL_DoSaveGame(item);
		return(true);
//              break;
	}
	return(USL_LoadCustom(call,item));
}

#define PaddleMinX      (CtlPanelSX + 3)
#define PaddleMaxX      (CtlPanelEX - 15)
#define BallMinX        (CtlPanelSX + 2)
#define BallMinY        (CtlPanelSY + 12 + 2)
#define BallMaxX        (CtlPanelEX - 6)
#define BallMaxY        (CtlPanelEY - 13)
#define CPaddleY        (BallMinY + 4)
#define KPaddleY        (BallMaxY - 2)
void
USL_DrawPongScore(word k,word c)
{
	if (mortimerbedgfx)
	{
		fontcolor = F_BLACK;
	}
	else
	{
		fontcolor = HiliteColor;
	}
	PrintY = py = CtlPanelSY + 4;
	px = CtlPanelSX + 6;
	if (mortimerbedgfx)
	{
		VWB_Bar(px,py,42,6,MBackColor);
	}
	else
	{
		VWB_Bar(px,py,42,6,BackColor);
	}
	if (mortimerbedgfx)
	{
		USL_DrawString("MORT:");
	}
	else
	{
		USL_DrawString("KEEN:");
	}
	PrintX = px;
	US_PrintUnsigned(k);
	px = CtlPanelSX + 108;
	if (mortimerbedgfx)
	{
		VWB_Bar(px,py,50,6,MBackColor);
	}
	else
	{
		VWB_Bar(px,py,50,6,BackColor);
	}
	USL_DrawString("COMP:");
	PrintX = px;
	US_PrintUnsigned(c);
}

void
USL_PlayPong(void)
{
	boolean         ball,killball,revdir,done,lastscore;
	word            cycle,
				x,y,
				w,h,
				kx,cx,
				rx,
				bx,by,
				kscore,cscore,
				speedup;
	int                     bdx,bdy;
	longword        balltime,waittime;
	CursorInfo      cursorinfo;
	UComm 		comm;

	StopMusic();

	kx = cx = PaddleMinX + ((PaddleMaxX - PaddleMinX) / 2);
	bx = by = bdx = bdy = 0;
	kscore = cscore = 0;
	USL_DrawPongScore(0,0);
	cycle = 0;
	revdir = false;
	killball = true;
	done = false;
	lastscore = false;
	do
	{
		waittime = TimeCount;

		if (Keyboard[sc_P])
		{
			SD_PlaySound (STATUSONSND);
			VW_FixRefreshBuffer ();
			USL_CtlDialog("PAUSED",
				"PRESS ANY KEY TO CONTINUE",nil);
			VW_UpdateScreen ();
			IN_Ack();
			SD_PlaySound (STATUSOFFSND);
			Paused = false;
		}

		IN_ReadCursor(&cursorinfo);
		if (((cursorinfo.x < 0) || IN_KeyDown(sc_LeftArrow)) && (kx > PaddleMinX))
			kx -= 2;
		else if (((cursorinfo.x > 0) || IN_KeyDown(sc_RightArrow)) && (kx < PaddleMaxX))
			kx += 2;

		if (killball)
		{
			ball = false;
			balltime = TimeCount + TickBase;
			speedup = 10;
			killball = false;
		}

		if (ball && (cycle++ % 3))
		{
			x = (bx >> 2);
			if (!(x & 1))
				x += (US_RndT() & 1);

			if ((cx + 6 < x) && (cx < PaddleMaxX))
				cx += 1;
			else if ((cx + 6 > x) && (cx > PaddleMinX))
				cx -= 1;
		}

		CA_CacheGrChunk(PADDLESPR);

		if (mortimerbedgfx)
		{
			VWB_Bar(BallMinX,BallMinY - 1,
					BallMaxX - BallMinX + 5,BallMaxY - BallMinY + 7,
					MBackColor);
		}
		else
		{
			VWB_Bar(BallMinX,BallMinY - 1,
					BallMaxX - BallMinX + 5,BallMaxY - BallMinY + 7,
					BackColor);
		}
		VWB_DrawSprite(cx,CPaddleY,PADDLESPR);
		VWB_DrawSprite(kx,KPaddleY,PADDLESPR);
		if (ball)
		{
			if
			(
				(((bx + bdx) >> 2) > BallMaxX)
			||      (((bx + bdx) >> 2) < BallMinX)
			)
			{
				SD_PlaySound(BOUNCESND);
				bdx = -bdx;
			}
			bx += bdx;

			if (((by + bdy) >> 2) > BallMaxY)
			{
				killball = true;
				lastscore = false;
				cscore++;
				SD_PlaySound(PLUMMETSND);
				USL_DrawPongScore(kscore,cscore);
				if (mortimerbedgfx)
				{
					if (cscore == 31)
					{
						USL_CtlDialog("You lost!","Press any key",nil);
						done = true;
						continue;
					}
				}
				else if (greenball)
				{
					if (cscore == 11)
					{
						USL_CtlDialog("You lost!","Press any key",nil);
						done = true;
						continue;
					}				
				}
				else
				{
					if (cscore == 21)
					{
						USL_CtlDialog("You lost!","Press any key",nil);
						done = true;
						continue;
					}				
				}
			}
			else if (((by + bdy) >> 2) < BallMinY)
			{
				killball = true;
				lastscore = true;
				kscore++;
				SD_PlaySound(GETPOWERSND);
				USL_DrawPongScore(kscore,cscore);
				if (mortimerbedgfx)
				{
					if (kscore == 31)
					{
						USL_CtlDialog("You won!","Press any key",nil);
						done = true;
						continue;
					}
				}
				else if (greenball)
				{
					if (kscore == 11)
					{
						USL_CtlDialog("You won!","Press any key",nil);
						done = true;
						continue;
					}				
				}
				else
				{
					if (kscore == 21)
					{
						USL_CtlDialog("You won!","Press any key",nil);
						done = true;
						continue;
					}				
				}
			}
			by += bdy;

			x = bx >> 2;
			y = by >> 2;
			if (!killball)
			{
				if
				(
					(bdy < 0)
				&&      ((y >= CPaddleY) && (y < CPaddleY + 3))
				&&      ((x >= (cx - 5)) && (x < (cx + 11)))
				)
				{
					rx = cx;
					revdir = true;
					SD_PlaySound(LANDSND);
				}
				else if
				(
					(bdy > 0)
				&&      ((y >= (KPaddleY - 3)) && (y < KPaddleY))
				&&      ((x >= (kx - 5)) && (x < (kx + 11)))
				)
				{
					if (((bdy >> 2) < 3) && !(--speedup))
					{
						bdy++;
						speedup = 10;
					}
					rx = kx;
					revdir = true;
					SD_PlaySound(JUMPSND);
				}
				if (revdir)
				{
					bdy = -bdy;
					bdx = ((x + 5 - rx) >> 1) - (1 << 2);
					if (!bdx)
						bdx--;
					revdir = false;
				}
			}
			if (mortimerbedgfx)
			{
				CA_CacheGrChunk(BLUEBALL1PIXELTOTHERIGHTSPR);
				CA_CacheGrChunk(BLUEBALLSPR);
				VWB_DrawSprite(x,y,(x & 1)? BLUEBALL1PIXELTOTHERIGHTSPR : BLUEBALLSPR);
			}
			else if (greenball)
			{
				CA_CacheGrChunk(GREENBALL1PIXELTOTHERIGHTSPR);
				CA_CacheGrChunk(GREENBALLSPR);
				VWB_DrawSprite(x,y,(x & 1)? GREENBALL1PIXELTOTHERIGHTSPR : GREENBALLSPR);
			}
			else
			{
				CA_CacheGrChunk(BALL1PIXELTOTHERIGHTSPR);
				CA_CacheGrChunk(BALLSPR);
				VWB_DrawSprite(x,y,(x & 1)? BALL1PIXELTOTHERIGHTSPR : BALLSPR);
			}
		}
		else if (TimeCount >= balltime)
		{
			ball = true;
			bdx = 1 - (US_RndT() % 3);
			bdy = 3; // Keen Dreams Gold patch
			if (lastscore)
				bdy = -bdy;
			bx = (BallMinX + ((BallMaxX - BallMinX) / 2)) << 2;
			by = (BallMinY + ((BallMaxY - BallMinY) / 2)) << 2;
		}
		VW_UpdateScreen();
		while (waittime == TimeCount)
			;       // DEBUG - do adaptiveness
	} while ((LastScan != sc_Escape) && !done);
	IN_ClearKeysDown();
}

#pragma argsused
 boolean
USL_PongCustom(UserCall call,struct UserItem far *item)
{
	if (call != uic_SetupCard)
		return(false);

	StopMusic();

	VW_FadeOut ();
	RF_FixOfs();

	VW_UpdateScreen();
	VW_FadeIn ();

	if (mortimerbedgfx)
	{
		CA_CacheGrChunk(CP_MMENUSCREENPIC);
		VWB_DrawPic(0,0,CP_MMENUSCREENPIC);
	}
	else
	{
		CA_CacheGrChunk(CP_MENUSCREENPIC);
		VWB_DrawPic(0,0,CP_MENUSCREENPIC);
	}
	if (mortimerbedgfx)
	{
		CA_CacheGrChunk(CP_MFLOWERPONGPIC);
		VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_MFLOWERPONGPIC);
		VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,F_BLACK ^ MBackColor);
		VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,F_BLACK ^ MBackColor);
	}
	else
	{
		CA_CacheGrChunk(CP_FLOWERPONGPIC);
		VWB_DrawPic(CtlPanelSX + 56,CtlPanelSY,CP_FLOWERPONGPIC);
		VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelSY + 12,HiliteColor ^ BackColor);
		VWB_Hlin(CtlPanelSX + 3,CtlPanelEX - 3,CtlPanelEY - 7,HiliteColor ^ BackColor);
	}
	fontcolor = NohiliteColor;
	px = CtlPanelSX + 48;
	py = CtlPanelEY + 2;
	USL_DrawString(BottomS4);

	USL_PlayPong();

	VW_FadeOut ();
	RF_FixOfs();

	VW_UpdateScreen();
	VW_FadeIn ();

	StartMusic(53);

	return(true);
}

//      Flag management stuff
 void
USL_ClearFlags(UserItemGroup far *node)
{
	UserItem        far *i;

	if (!node->items)
		return;

	for (i = node->items;i->type != uii_Bad;i++)
	{
		i->flags &= ~UISelectFlags;
		if (i->child)
			USL_ClearFlags((UserItemGroup far *)i->child);
	}
}

 int
USL_FindPushedItem(UserItemGroup far *group)
{
	word            i;
	UserItem        far *item;

	for (item = group->items,i = 0;item->type != uii_Bad;item++,i++)
		if (item->flags & ui_Pushed)
			return(i);
	return(-1);
}

 void
USL_SelectItem(UserItemGroup far *group,word index,boolean draw)
{
	UserItem        far *item;

	if (index != group->cursor)
	{
		item = &group->items[group->cursor];
		item->flags &= ~ui_Selected;
		if (draw)
			USL_DrawItem(item);
	}

	group->cursor = index;
	item = &group->items[group->cursor];
	group->items[group->cursor].flags |= ui_Selected;
	if (draw)
		USL_DrawItem(item);
}

 void
USL_PushItem(UserItemGroup far *group,word index,boolean draw)
{
	word            i;
	UserItem        far *item;

	USL_SelectItem(group,index,draw);
	for (item = group->items,i = 0;item->type != uii_Bad;item++,i++)
	{
		if (item->type != uii_RadioButton)
			continue;

		if (i == index)
		{
			item->flags |= ui_Pushed;
			if (draw)
				USL_DrawItem(item);
		}
		else if (item->flags & ui_Pushed)
		{
			item->flags &= ~ui_Pushed;
			if (draw)
				USL_DrawItem(item);
		}
	}
}

 void
USL_NextItem(void)
{
	if (topcard->items[topcard->cursor + 1].type == uii_Bad)
		return;
	USL_SelectItem(topcard,topcard->cursor + 1,true);
}

 void
USL_PrevItem(void)
{
	if (!topcard->cursor)
		return;
	USL_SelectItem(topcard,topcard->cursor - 1,true);
}

 void
USL_SetupCard(void)
{
	BottomS1 = "Arrows move";
	BottomS2 = "Enter selects";
	BottomS3 = cstackptr? "ESC to back out" : "ESC to quit";
	BottomS4 = "P to pause";

	USL_SelectItem(topcard,topcard->cursor,false);
	USL_DrawCtlPanel();     // Contents?
}

 void
USL_DownLevel(UserItemGroup far *group)
{
	if (!group)
		Quit("USL_DownLevel() - nil card");
	USL_PushCard(group);
	if (group->custom && group->custom(uic_SetupCard,nil))
		USL_PopCard();
	USL_SetupCard();
}

 void
USL_UpLevel(void)
{
	if (!cstackptr)
	{
		USL_ConfirmComm(uc_Quit);
		return;
	}

	if (topcard->items)
		topcard->items[topcard->cursor].flags &= ~ui_Selected;
	USL_PopCard();
	USL_SetupCard();
}

 void
USL_DoItem(void)
{
	// DEBUG - finish this routine
	UserItem                far *item;

	item = &topcard->items[topcard->cursor];
	if (item->flags & ui_Disabled)
		SD_PlaySound(NOWAYTWOSND);
	else
	{
		switch (item->type)
		{
		case uii_Button:
			if (!(topcard->custom && topcard->custom(uic_Hit,item)))
				USL_ConfirmComm(item->comm);
			break;
		case uii_RadioButton:
			USL_PushItem(topcard,topcard->cursor,true);
			break;
		case uii_Folder:
			USL_DownLevel(item->child);
			break;
		}
	}
}

 void
USL_SetControlValues(void)
{
	int sndindex;

	newgamei[4].text = mortimerunlocked? "BEGIN GRAND GAME" : "?";
	if (mortimerunlocked == false)
	{
		newgamei[4].flags |= ui_Disabled; // Secret difficulty isn't unlocked yet.
	}

	sndindex = SoundMode;
	if (sndindex == sdm_AdLib && QuietFX)
		sndindex++;

	USL_PushItem(&soundgroup,sndindex,false);
	USL_PushItem(&musicgroup,MusicMode,false);
	if (!AdLibPresent)
	{
		soundi[2].flags |= ui_Disabled; // AdLib sound effects
		soundi[3].flags |= ui_Disabled; // Quiet AdLib sound effects
		musici[1].flags |= ui_Disabled; // AdLib music
	}

	if (!JoysPresent[0])
		configi[4/*3*/].flags |= ui_Disabled; // OPTIONS is back for Keen Dreams Gold
	if (!JoysPresent[1])
		configi[5/*4*/].flags |= ui_Disabled; // OPTIONS is back for Keen Dreams Gold

	rooti[4].text = ingame? "RETURN TO GAME" : "RETURN TO DEMO";
	if (!ingame)
	{
		rooti[2].flags |= ui_Disabled;  // Save Game
		rooti[5].flags |= ui_Disabled;  // End Game
	}
	if (DemoMode)
	{
		rooti[0].flags |= ui_Disabled;  // Disable New Game option if demo-recording is used
		rooti[1].flags |= ui_Disabled;  // Disable Load Game option if demo-recording is used
		rooti[2].flags |= ui_Disabled;  // Disable Save Game option if demo-recording is used
	}
	rootgroup.cursor = ingame? 4 : 0;
	USL_SetOptionsText();
	// DEBUG - write the rest of this
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_SetUpCtlPanel() - Sets the states of the UserItems to reflect the
//              values of all the appropriate variables
//
///////////////////////////////////////////////////////////////////////////
 void
USL_SetUpCtlPanel(void)
{
	int     i;

	// Cache in all of the stuff for the control panel
	CA_UpLevel();
	CA_ClearMarks();	// K1n9_Duk3 addition
	for (i = MAINMENU_LUMP_START;i <= MAINMENU_LUMP_END;i++)
		CA_MarkGrChunk(i);
	for (i = MMAINMENU_LUMP_START;i <= MMAINMENU_LUMP_END;i++)
		CA_MarkGrChunk(i);
	for (i = PONG_LUMP_START;i <= PONG_LUMP_END;i++)
		CA_MarkGrChunk(i);
	/* CA_MarkGrChunk(STARTFONT+1);            // Little font */
	CA_MarkGrChunk(CP_MENUMASKPICM);        // Mask for dialogs
//	CA_CacheMarks("Control Panel");
	CA_LoadAllSounds();

	// Do some other setup
	fontnumber = 1;
	US_SetPrintRoutines(VW_MeasurePropString,VWB_DrawPropString);
	fontcolor = F_BLACK;
	//VW_Bar (0,0,320,200,3); // CAT3D patch
	RF_FixOfs();
	VW_InitDoubleBuffer();

	Communication = uc_None;
	USL_ClearFlags(&rootgroup);
	USL_SetControlValues();
	USL_SetupStack();
	USL_SetupCard();

	if (ingame)
		GameIsDirty = true;

	IN_ClearKeysDown();
}

 void
USL_HandleComm(UComm comm)
{
	switch (comm)
	{
	case uc_Loaded:
	case uc_Return:
		break;
	case uc_Abort:
		EndGame();
		break;
	case uc_Quit:
		QuitToDos = true;
		break;
	case uc_SEasy:
		greenball = true;
		mortimerbedgfx = false;
		mortwindow = false;
		restartgame = gd_Easy;
		break;
	case uc_SNormal:
		greenball = false;
		mortimerbedgfx = false;
		mortwindow = false;
		restartgame = gd_Normal;
		break;
	case uc_SHard:
		greenball = false;
		mortimerbedgfx = false;
		mortwindow = false;
		restartgame = gd_Hard;
		break;
	case uc_SKeen:
		greenball = false;
		mortimerbedgfx = false;
		mortwindow = false;
		restartgame = gd_Keen;
		break;
	case uc_SGrand:
		if (mortimerunlocked == true)
		{
			mortimerbedgfx = true;
			mortwindow = true;
			restartgame = gd_Grand;
		}
		break;

	default:
		Quit("USL_HandleComm() - unknown");
		break;
	}		
}

 void
USL_GetControlValues(void)
{
	int     i;

	// DEBUG - write the rest of this
	i = USL_FindPushedItem(&soundgroup);
	if (i == 2)
	{
		QuietFX = true;
		i--;
	}
	else
	{
		QuietFX = false;
	}
	if (i != SoundMode)
		SD_SetSoundMode(i);

	i = USL_FindPushedItem(&musicgroup);
	if (i != MusicMode)
		SD_SetMusicMode(i);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_TearDownCtlPanel() - Given the state of the control panel, sets the
//              modes and values as appropriate
//
///////////////////////////////////////////////////////////////////////////
 void
USL_TearDownCtlPanel(void)
{
	USL_GetControlValues();
	if (Communication)
		USL_HandleComm(Communication);

	fontnumber = 0; // Normal font
	fontcolor = F_BLACK;
	if (restartgame && USL_ResetGame)
	{
		USL_ResetGame();
	}
	else if (QuitToDos)
	{
		US_CenterWindow(20,3);
		if (DemoMode)
		{
			fontcolor = 13;
		}
		else if (mapon >= 0 && mapon <= 22)
		{
			if (mapon >= 0 && mapon <= 22)
				fontcolor = F_SECONDCOLOR;
			if (mapon >= 23 && mapon <= 47)
			{
				fontcolor = F_SECONDCOLOR;
			}
		}
		else
		{
			if (mapon >= 23 && mapon <= 46)
				fontcolor = 6;
			else if (mapon == 47)
				if (nightmode == true)
					fontcolor = 6;
				else
					fontcolor = F_SECONDCOLOR;
			if (mapon >= 0 && mapon <= 22)
			{
				fontcolor = 6;
			}
		}
		US_PrintCentered("Quitting...");
		fontcolor = F_BLACK;
		VW_UpdateScreen();
		Quit(nil);
	}

	IN_ClearKeysDown();
	SD_WaitSoundDone();
	//VW_Bar (0,0,320,200,3); // CAT3D patch
	CA_DownLevel();
	CA_LoadAllSounds();
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ControlPanel() - This is the main routine for the control panel
//
///////////////////////////////////////////////////////////////////////////
#define MoveMin 40
void
US_ControlPanel(void)
{
extern void HelpScreens(void);

	boolean         resetitem,on;
	word            i;
	int                     ydelta;
	longword        flashtime;
	UserItem        far *item;
	CursorInfo      cursorinfo;

#if 0
	// DEBUG!!!
	{
		USL_SetUpCtlPanel();
		Communication = uc_Loaded;
		CtlPanelDone = true;
		loadedgame = true;
		USL_TearDownCtlPanel();
		return;
	}
#endif

//	VW_UpdateScreen();
//	VW_FadeOut ();
//	RF_FixOfs();
	VW_ClearVideo(BLACK);

//	VW_UpdateScreen();
//	VW_FadeIn ();

	if ((LastScan < sc_F1) || (LastScan > sc_F10))
		IN_ClearKeysDown();

	USL_SetUpCtlPanel();
	USL_DrawCtlPanel();

	StartMusic(53);

	ydelta = 0;
	for (CtlPanelDone = false,resetitem = on = true;!CtlPanelDone;)
	{
		item = &(topcard->items[topcard->cursor]);

		if (resetitem)
		{
			flashtime = TimeCount + (TickBase / 2);
			resetitem = false;
		}

		if (TimeCount >= flashtime)
		{
			on ^= true;
			resetitem = true;
			if (!on)
				item->flags &= ~ui_Selected;
			USL_DrawItemIcon(item);
			item->flags |= ui_Selected;
		}

		VW_UpdateScreen();

		if (LastScan)
		{
			switch (LastScan)
			{
			case sc_UpArrow:
				USL_PrevItem();
				resetitem = true;
				break;
			case sc_DownArrow:
				USL_NextItem();
				resetitem = true;
				break;
			case sc_Return:
				USL_DoItem();
				resetitem = true;
				break;
			case sc_Escape:
				USL_UpLevel();
				resetitem = true;
				break;
			case sc_F1:
				HelpScreens();
				USL_DrawCtlPanel();
				resetitem = true;
				break;
			}

			if
			(
				(!resetitem)
			&&      (
					(LastScan == KbdDefs[0].button0)
				||      (LastScan == KbdDefs[0].button1)
				)
			)
			{
				USL_DoItem();
				resetitem = true;
			}

			if (!resetitem)
			{
				for (item = topcard->items,i = 0;item->type != uii_Bad;item++,i++)
				{
					if (item->hotkey == LastScan)
					{
						USL_SelectItem(topcard,i,true);
						resetitem = true;
						break;
					}
				}
			}

			IN_ClearKeysDown();
		}
		else
		{
			IN_ReadCursor(&cursorinfo);
			ydelta += cursorinfo.y;
			if (cursorinfo.button0)
			{
				do
				{
					IN_ReadCursor(&cursorinfo);
				} while (cursorinfo.button0);
				USL_DoItem();
				resetitem = true;
			}
			else if (cursorinfo.button1)
			{
				do
				{
					IN_ReadCursor(&cursorinfo);
				} while (cursorinfo.button1);
				USL_UpLevel();
				resetitem = true;
			}
			else if (ydelta < -MoveMin)
			{
				ydelta += MoveMin;
				USL_PrevItem();
				resetitem = true;
			}
			else if (ydelta > MoveMin)
			{
				ydelta -= MoveMin;
				USL_NextItem();
				resetitem = true;
			}
		}
	}

	USL_TearDownCtlPanel();
}

///////////////////////////////////////////////////////////////////////////
//
//	US_DisplayHighScores() - Assumes that double buffering has been started.
//		If passed a -1 will just display the high scores, but if passed
//		a non-negative number will display that entry in red and let the
//		user type in a name
//
///////////////////////////////////////////////////////////////////////////
void
US_DisplayHighScores(int which)
{
	char		buffer[16],*str;
	word		i,
				w,h,
				x,y;
	HighScore	*s;

	US_CenterWindow(30,MaxScores + (MaxScores / 2));

	x = WindowX + (WindowW / 2);
	US_Print(" Name");
	PrintX = x + 20;
	US_Print("Score");
	PrintX = x + 60;
	US_Print("Done\n\n");
	PrintY -= 3;

	if (mortimerunlocked == true && mortwindow == true)
	{
		for (i = WindowX;i < WindowX + WindowW;i += 8)
			VWB_DrawTile8M(i,WindowY + 8,21);
		VWB_DrawTile8M(WindowX - 8,WindowY + 8,20);
		VWB_DrawTile8M(WindowX + WindowW,WindowY + 8,22);
	}
	else
	{
		for (i = WindowX;i < WindowX + WindowW;i += 8)
			VWB_DrawTile8M(i,WindowY + 8,10);
		VWB_DrawTile8M(WindowX - 8,WindowY + 8,9);
		VWB_DrawTile8M(WindowX + WindowW,WindowY + 8,11);
	}

	for (i = 0,s = Scores;i < MaxScores;i++,s++)
	{
		if (DemoMode)
			fontcolor = (i == which)? 13 : F_BLACK;
		else if (mapon >= 23 && mapon <= 46)
			fontcolor = (i == which)? 6 : F_BLACK;
		else
			fontcolor = (i == which)? F_SECONDCOLOR : F_BLACK;

		if (i != which)
		{
			US_Print(" ");
			if (strlen(s->name))
				US_Print(s->name);
			else
				US_Print("-");
		}
		else
			y = PrintY;

		PrintX = x + (7 * 8);
		ultoa(s->score,buffer,10);
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');	// Used fixed-width numbers (129...)
		USL_MeasureString(buffer,&w,&h);
		PrintX -= w;
		US_Print(buffer);

		PrintX = x + 60;
		if (s->completed)
			US_PrintUnsigned(s->completed);
		else
			US_Print("-");

		US_Print("\n");
	}

	VW_FadeOut ();
	RF_FixOfs();

	VW_UpdateScreen();
	VW_FadeIn ();

	if (which != -1)
	{
		if (DemoMode)
			fontcolor = 13;
		else if (mapon >= 23 && mapon <= 46)
			fontcolor = 6;
		else
			fontcolor = F_SECONDCOLOR;
		PrintY = y;
		PrintX = WindowX;
		US_Print(" ");
		strcpy(Scores[which].name,"");
		US_LineInput(PrintX,PrintY,Scores[which].name,nil,true,MaxHighName,
						(WindowW / 2) - 8);

		if (
			strcmp(Scores[which].name, nightmodecode) == 0)
				enternightmodecode = true;

		if (
			strcmp(Scores[which].name, mortimermodecode) == 0)
				entermortimercode = true;
	}
	fontcolor = F_BLACK;

	VW_UpdateScreen();
}

///////////////////////////////////////////////////////////////////////////
//
//	US_PrintFar() - For demo stuff
//
///////////////////////////////////////////////////////////////////////////

void US_CPrintFar(char far *s)
{
	// Note: One line must be at most 79 characters long
extern  char    str[80];
	char	c, far *se;
	int	copysize;

	do
	{
		se = s;
		while ((c = *se) && (c != '\n'))
			se++;
		copysize = se - s;
#if 0
	// May be re-enabled for debugging
	if (copysize > 79)
		Quit("Line too long in US_CPrintFar");
#endif
		_fmemcpy(str, s, copysize);
		str[copysize] = 0;

		US_CPrintLine(str);
		s = se + 1;
	} while (c);
}

///////////////////////////////////////////////////////////////////////////
//
//	US_CheckHighScore() - Checks gamestate to see if the just-ended game
//		should be entered in the high score list. If so, lets the user
//		enter their name
//
///////////////////////////////////////////////////////////////////////////
void
US_CheckHighScore(unsigned long score,word other)
{
	word		i,j,
				n;
	HighScore	myscore;

	strcpy(myscore.name,"");
	myscore.score = score;
	myscore.completed = other;

	n = -1;
	if (other != (word)(-1)) 
    {
		for (i = 0,n = -1;i < MaxScores;i++)
		{
			if
			(
				(myscore.score > Scores[i].score)
			||	(
					(myscore.score == Scores[i].score)
				&& 	(myscore.completed > Scores[i].completed)
				)
			)
			{
				for (j = MaxScores;--j > i;)
					Scores[j] = Scores[j - 1];
				Scores[i] = myscore;

				StartMusic(50);
    	
				n = i;
				HighScoresDirty = true;
				break;
			}
		}
	}

	RF_FixOfs();
	VW_InitDoubleBuffer();
	if (mortwindow)
		VWB_Bar(0,0,MaxX,MaxY,4);
	else
		VWB_Bar(0,0,MaxX,MaxY,FIRSTCOLOR);

	US_DisplayHighScores(n);
	IN_UserInput(5 * TickBase,false);
	StopMusic();

	VW_FadeOut ();
	RF_FixOfs();
	VW_ClearVideo(BLACK);

	VW_UpdateScreen();

	RF_FixOfs();

	VW_UpdateScreen();
	VW_FadeIn ();
}
