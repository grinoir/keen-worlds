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
//      ID_US_1.c - User Manager - General routines
//      v1.1d1
//      By Jason Blochowiak
//      Hacked up for Catacomb 3D
//

//
//	This module handles dealing with user input & feedback
//
//	Depends on: Input Mgr, View Mgr, some variables from the Sound, Caching,
//		and Refresh Mgrs, Memory Mgr for background save/restore
//
//	Globals:
//		ingame - Flag set by game indicating if a game is in progress
//      abortgame - Flag set if the current game should be aborted (if a load
//			game fails)
//		loadedgame - Flag set if a game was loaded
//		abortprogram - Normally nil, this points to a terminal error message
//			if the program needs to abort
//		restartgame - Normally set to gd_Continue, this is set to one of the
//			difficulty levels if a new game should be started
//		PrintX, PrintY - Where the User Mgr will print (global coords)
//		WindowX,WindowY,WindowW,WindowH - The dimensions of the current
//			window
//

#include "id_heads.h"

#pragma hdrstop

#pragma warn    -pia

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
		char            *abortprogram;
		boolean         NoWait,
					HighScoresDirty;
		word            PrintX,PrintY;
		word            WindowX,WindowY,WindowW,WindowH;

//      Internal variables
#define ConfigVersion   1

static  char            *ParmStrings[] = {"COMP","NOCOMP"};
static  boolean         US_Started;

		boolean         Button0,Button1,
					CursorBad;
		int                     CursorX,CursorY;

		void            (*USL_MeasureString)(char far *,word *,word *) = VW_MeasurePropString,
					(*USL_DrawString)(char far *) = VWB_DrawPropString;

		boolean         (*USL_SaveGame)(int),(*USL_LoadGame)(int);
		void            (*USL_ResetGame)(void);
		SaveGame        Games[MaxSaveGames];
		// Keen Dreams import
		HighScore	Scores[MaxScores] =
					{
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000},
						{"",10000}
					};

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

//#define CTLPANELMUSIC 56

//      Internal routines

//      Public routines

///////////////////////////////////////////////////////////////////////////
//
//      USL_HardError() - Handles the Abort/Retry/Fail sort of errors passed
//                      from DOS.
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
//      USL_GiveSaveName() - Returns a pointer to a static buffer that contains
//              the filename to use for the specified save game
//
///////////////////////////////////////////////////////////////////////////
char *
USL_GiveSaveName(word game)
{
static	char	filename[32];
		char	*s,*t;

	for (s = "KDGOLD",t = filename;*s;)
		*t++ = *s++;
	*t++ = game + '0';
	for (s = "."SGEXTENSION;*s;)
		*t++ = *s++;
	*t = '\0';

	return(filename);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_SetLoadSaveHooks() - Sets the routines that the User Mgr calls after
//              reading or writing the save game headers
//
///////////////////////////////////////////////////////////////////////////
void
US_SetLoadSaveHooks(boolean (*load)(int),boolean (*save)(int),void (*reset)(void))
{
	USL_LoadGame = load;
	USL_SaveGame = save;
	USL_ResetGame = reset;
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ReadConfig() - Reads the configuration file, if present, and sets
//              things up accordingly. If it's not present, uses defaults. This file
//              includes the high scores.
//
///////////////////////////////////////////////////////////////////////////
 void
USL_ReadConfig(void)
{
	boolean         gotit, hadAdLib;
	char            sig[sizeof(EXTENSION)];
	word            version;
	int                     file;
	SDMode          sd;
	SMMode          sm;
	ControlType     ctl;

	if ((file = open("KDGOLD.CFG",O_BINARY | O_RDONLY)) != -1)
	{
		read(file,sig,sizeof(EXTENSION));
		read(file,&version,sizeof(version));
		if (strcmp(sig,EXTENSION) || (version != ConfigVersion))
		{
			close(file);
			goto rcfailed;
		}
		read(file,Scores,sizeof(HighScore) * MaxScores);
		read(file,&sd,sizeof(sd));
		read(file,&sm,sizeof(sm));
		read(file,&ctl,sizeof(ctl));
		read(file,&(KbdDefs[0]),sizeof(KbdDefs[0]));
		read(file,&showscorebox,sizeof(showscorebox));
		read(file,&compatibility,sizeof(compatibility));
		read(file,&QuietFX,sizeof(QuietFX));
		read(file,&hadAdLib,sizeof(hadAdLib));
		read(file,&oldattacking,sizeof(oldattacking));
		read(file,&attackscan,sizeof(attackscan));
		read(file,&mortimerunlocked,sizeof(mortimerunlocked));
		read(file,&nightmode,sizeof(nightmode));
		read(file,&GravisGamepad,sizeof(GravisGamepad));
		read(file,&GravisMap,sizeof(GravisMap));
		close(file);

		HighScoresDirty = false;
		gotit = true;
	}
	else
	{
rcfailed:
		sd = sdm_Off;
		sm = smm_Off;
		ctl = ctrl_Keyboard;
		showscorebox = true;

		oldattacking = false;

		gotit = false;
		HighScoresDirty = true;
	}

	SD_Default(gotit,sd,sm);
	IN_Default(gotit,ctl);
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_WriteConfig() - Writes out the current configuration, including the
//              high scores.
//
///////////////////////////////////////////////////////////////////////////
 void
USL_WriteConfig(void)
{
	word    version;
	int             file;

	version = ConfigVersion;
	file = open("KDGOLD.CFG", O_CREAT | O_BINARY | O_WRONLY,
				S_IREAD | S_IWRITE | S_IFREG);
	if (file != -1)
	{
		write(file,EXTENSION,sizeof(EXTENSION));
		write(file,&version,sizeof(version));
		write(file,Scores,sizeof(HighScore) * MaxScores);
		write(file,&SoundMode,sizeof(SoundMode));
		write(file,&MusicMode,sizeof(MusicMode));
		if      // Hack
		(
			(Controls[0] == ctrl_Joystick1)
		||      (Controls[0] == ctrl_Joystick2)
		)
			Controls[0] = ctrl_Keyboard;
		write(file,&(Controls[0]),sizeof(Controls[0]));
		write(file,&(KbdDefs[0]),sizeof(KbdDefs[0]));
		write(file,&showscorebox,sizeof(showscorebox));
		write(file,&compatibility,sizeof(compatibility));
		write(file,&QuietFX,sizeof(QuietFX));
		write(file,&AdLibPresent,sizeof(AdLibPresent));
		write(file,&oldattacking,sizeof(oldattacking));
		write(file,&attackscan,sizeof(attackscan));
		write(file,&mortimerunlocked,sizeof(mortimerunlocked));
		write(file,&nightmode,sizeof(nightmode));
		write(file,&GravisGamepad,sizeof(GravisGamepad));
		write(file,&GravisMap,sizeof(GravisMap));
		close(file);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_CheckSavedGames() - Checks to see which saved games are present
//              & valid
//
///////////////////////////////////////////////////////////////////////////
 void
USL_CheckSavedGames(void)
{
	boolean         ok;
	char            *filename;
	word            i;
	int                     file;
	SaveGame        *game;

	USL_SaveGame = 0;
	USL_LoadGame = 0;

	for (i = 0,game = Games;i < MaxSaveGames;i++,game++)
	{
		filename = USL_GiveSaveName(i);
		ok = false;
		if ((file = open(filename,O_BINARY | O_RDONLY)) != -1)
		{
			if
			(
				(read(file,game,sizeof(*game)) == sizeof(*game))
			&&      (!strcmp(game->signature,EXTENSION))
			&&      (game->oldtest == US_SAVEMAGIC)
			)
				ok = true;

			close(file);
		}

		if (ok)
			game->present = true;
		else
		{
			strcpy(game->signature,EXTENSION);
			game->present = false;
			strcpy(game->name,"Empty");
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Startup() - Starts the User Mgr
//
///////////////////////////////////////////////////////////////////////////
void
US_Startup(void)
{
	int     i;

	if (US_Started)
		return;


	US_InitRndT(true);              // Initialize the random number generator

	USL_ReadConfig();               // Read config file

	for (i = 1;i < _argc;i++)
	{
		switch (US_CheckParm(_argv[i],ParmStrings))
		{
		case 0:
			if (grmode == EGAGR)
				compatibility = true;
			break;
		case 1:
			compatibility = false;
			break;
		}
	}

	US_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Setup() - Does the disk access part of the User Mgr's startup
//
///////////////////////////////////////////////////////////////////////////
void
US_Setup(void)
{
	USL_CheckSavedGames();  // Check which saved games are present
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Shutdown() - Shuts down the User Mgr
//
///////////////////////////////////////////////////////////////////////////
void
US_Shutdown(void)
{
	if (!US_Started)
		return;

	if (!abortprogram)
		USL_WriteConfig();

	US_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CheckParm() - checks to see if a string matches one of a set of
//              strings. The check is case insensitive. The routine returns the
//              index of the string that matched, or -1 if no matches were found
//
///////////////////////////////////////////////////////////////////////////
int
US_CheckParm(char *parm,char **strings)
{
	char    cp,cs,
			*p,*s;
	int             i;

	while (!isalpha(*parm)) // Skip non-alphas
		parm++;

	for (i = 0;*strings && **strings;i++)
	{
		for (s = *strings++,p = parm,cs = cp = 0;cs == cp;)
		{
			cs = *s++;
			if (!cs)
				return(i);
			cp = *p++;

			if (isupper(cs))
				cs = tolower(cs);
			if (isupper(cp))
				cp = tolower(cp);
		}
	}
	return(-1);
}

///////////////////////////////////////////////////////////////////////////
//
//	USL_ScreenDraw() - Draws a chunk of the text screen (called only by
//		US_TextScreen())
//
///////////////////////////////////////////////////////////////////////////
 void
USL_ScreenDraw(word x,word y,char *s,byte attr)
{
	// DOS text-mode screen: nothing to show in the native companion
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ClearTextScreen() - Makes sure the screen is in text mode, clears it,
//              and moves the cursor to the leftmost column of the bottom line
//
///////////////////////////////////////////////////////////////////////////
 void
USL_ClearTextScreen(void)
{
	// DOS text-mode screen: nothing to show in the native companion
}

///////////////////////////////////////////////////////////////////////////
//
//	US_TextScreen() - Puts up the startup text screen
//	Note: These are the only User Manager functions that can be safely called
//		before the User Mgr has been started up
//
///////////////////////////////////////////////////////////////////////////
void
US_TextScreen(void)
{
	// DOS text-mode screen: nothing to show in the native companion
}


///////////////////////////////////////////////////////////////////////////
//
//	USL_Show() - Changes the appearance of one of the fields on the text
//		screen. Possibly adds a checkmark in front of it and highlights it
//
///////////////////////////////////////////////////////////////////////////
 void
USL_Show(word x,word y,word w,boolean show,boolean hilight)
{
	// DOS text-mode screen: nothing to show in the native companion
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_ShowMem() - Right justifies a longword in one of the memory fields on
//              the text screen
//
///////////////////////////////////////////////////////////////////////////
 void
USL_ShowMem(word x,word y,long mem)
{
	// DOS text-mode screen: nothing to show in the native companion
}

///////////////////////////////////////////////////////////////////////////
//
//      US_UpdateTextScreen() - Called after the ID libraries are started up.
//              Displays what hardware is present.
//
///////////////////////////////////////////////////////////////////////////
void
US_UpdateTextScreen(void)
{
	// DOS text-mode screen: nothing to show in the native companion
}

///////////////////////////////////////////////////////////////////////////
//
//      US_FinishTextScreen() - After the main program has finished its initial
//              loading, this routine waits for a keypress and then clears the screen
//
///////////////////////////////////////////////////////////////////////////
void
US_FinishTextScreen(void)
{
	// DOS text-mode screen: nothing to show in the native companion
}

//      Window/Printing routines

///////////////////////////////////////////////////////////////////////////
//
//      US_SetPrintRoutines() - Sets the routines used to measure and print
//              from within the User Mgr. Primarily provided to allow switching
//              between masked and non-masked fonts
//
///////////////////////////////////////////////////////////////////////////
void
US_SetPrintRoutines(void (*measure)(char far *,word *,word *),void (*print)(char far *))
{
	USL_MeasureString = measure;
	USL_DrawString = print;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_Print() - Prints a string in the current window. Newlines are
//              supported.
//
///////////////////////////////////////////////////////////////////////////
void
US_Print(char *s)
{
	char    c,*se;
	word    w,h;

	while (*s)
	{
		se = s;
		while ((c = *se) && (c != '\n'))
			se++;
		*se = '\0';

		USL_MeasureString(s,&w,&h);
		px = PrintX;
		py = PrintY;
		USL_DrawString(s);

		s = se;
		if (c)
		{
			*se = c;
			s++;

			PrintX = WindowX;
			PrintY += h;
		}
		else
			PrintX += w;
	}
}


///////////////////////////////////////////////////////////////////////////
//
//      US_PrintUnsigned() - Prints an unsigned long
//
///////////////////////////////////////////////////////////////////////////
void
US_PrintUnsigned(longword n)
{
	char    buffer[32];

	US_Print(ultoa(n,buffer,10));
}

///////////////////////////////////////////////////////////////////////////
//
//      US_PrintSigned() - Prints a signed long
//
///////////////////////////////////////////////////////////////////////////
void
US_PrintSigned(long n)
{
	char    buffer[32];

	US_Print(ltoa(n,buffer,10));
}

///////////////////////////////////////////////////////////////////////////
//
//      USL_PrintInCenter() - Prints a string in the center of the given rect
//
///////////////////////////////////////////////////////////////////////////
void
USL_PrintInCenter(char *s,Rect r)
{
	word    w,h,
			rw,rh;

	USL_MeasureString(s,&w,&h);
	rw = r.lr.x - r.ul.x;
	rh = r.lr.y - r.ul.y;

	px = r.ul.x + ((rw - w) / 2);
	py = r.ul.y + ((rh - h) / 2);
	USL_DrawString(s);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_PrintCentered() - Prints a string centered in the current window.
//
///////////////////////////////////////////////////////////////////////////
void
US_PrintCentered(char *s)
{
	Rect    r;

	r.ul.x = WindowX;
	r.ul.y = WindowY;
	r.lr.x = r.ul.x + WindowW;
	r.lr.y = r.ul.y + WindowH;

	USL_PrintInCenter(s,r);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CPrintLine() - Prints a string centered on the current line and
//              advances to the next line. Newlines are not supported.
//
///////////////////////////////////////////////////////////////////////////
void
US_CPrintLine(char *s)
{
	word    w,h;

	USL_MeasureString(s,&w,&h);

	if (w > WindowW)
		Quit("US_CPrintLine() - String exceeds width");
	px = WindowX + ((WindowW - w) / 2);
	py = PrintY;
	USL_DrawString(s);
	PrintY += h;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CPrint() - Prints a string in the current window. Newlines are
//              supported.
//
///////////////////////////////////////////////////////////////////////////
void
US_CPrint(char *s)
{
	char    c,*se;

	while (*s)
	{
		se = s;
		while ((c = *se) && (c != '\n'))
			se++;
		*se = '\0';

		US_CPrintLine(s);

		s = se;
		if (c)
		{
			*se = c;
			s++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ClearWindow() - Clears the current window to white and homes the
//              cursor
//
///////////////////////////////////////////////////////////////////////////
void
US_ClearWindow(void)
{
	VWB_Bar(WindowX,WindowY,WindowW,WindowH,WHITE);
	PrintX = WindowX;
	PrintY = WindowY;
}

///////////////////////////////////////////////////////////////////////////
//
//	US_DrawWindow() - Draws a frame and sets the current window parms
//
///////////////////////////////////////////////////////////////////////////
void
US_DrawWindow(word x,word y,word w,word h)
{
	word	i,
			sx,sy,sw,sh;

	WindowX = x * 8;
	WindowY = y * 8;
	WindowW = w * 8;
	WindowH = h * 8;
	/*
	// K1n9_Duk3 fix: these are redundant (will be set by US_ClearWindow)
	PrintX = WindowX;
	PrintY = WindowY;
	*/
	sx = (x - 1) * 8;
	sy = (y - 1) * 8;
	sw = (w + 1) * 8;
	sh = (h + 1) * 8;

	US_ClearWindow();

	if (mortimerunlocked == true && mortwindow == true)
	{
		VWB_DrawTile8M(sx,sy,12),VWB_DrawTile8M(sx,sy + sh,17);
		for (i = sx + 8;i <= sx + sw - 8;i += 8)
			VWB_DrawTile8M(i,sy,13),VWB_DrawTile8M(i,sy + sh,18);
		VWB_DrawTile8M(i,sy,14),VWB_DrawTile8M(i,sy + sh,19);

		for (i = sy + 8;i <= sy + sh - 8;i += 8)
			VWB_DrawTile8M(sx,i,15),VWB_DrawTile8M(sx + sw,i,16);
	}
	else
	{
		VWB_DrawTile8M(sx,sy,0),VWB_DrawTile8M(sx,sy + sh,6);
		for (i = sx + 8;i <= sx + sw - 8;i += 8)
			VWB_DrawTile8M(i,sy,1),VWB_DrawTile8M(i,sy + sh,7);
		VWB_DrawTile8M(i,sy,2),VWB_DrawTile8M(i,sy + sh,8);

		for (i = sy + 8;i <= sy + sh - 8;i += 8)
			VWB_DrawTile8M(sx,i,3),VWB_DrawTile8M(sx + sw,i,5);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	US_CenterWindow() - Generates a window of a given width & height in the
//		middle of the screen
//
///////////////////////////////////////////////////////////////////////////
void
US_CenterWindow(word w,word h)
{
	US_DrawWindow(((MaxX / 8) - w) / 2,((MaxY / 8) - h) / 2,w,h);
}

///////////////////////////////////////////////////////////////////////////
//
//	US_DrawWindow2() - Draws a frame and sets the current window parms
//
///////////////////////////////////////////////////////////////////////////
void
US_DrawWindow2(word x,word y,word w,word h)
{
	word	i,
			sx,sy,sw,sh;

	WindowX = x * 8;
	WindowY = y * 8;
	WindowW = w * 8;
	WindowH = h * 8;
	/*
	// K1n9_Duk3 fix: these are redundant (will be set by US_ClearWindow)
	PrintX = WindowX;
	PrintY = WindowY;
	*/
	sx = (x - 1) * 8;
	sy = (y - 1) * 8;
	sw = (w + 1) * 8;
	sh = (h + 1) * 8;

	US_ClearWindow();

	VWB_DrawTile8M(sx,sy,23),VWB_DrawTile8M(sx,sy + sh,28);
	for (i = sx + 8;i <= sx + sw - 8;i += 8)
		VWB_DrawTile8M(i,sy,24),VWB_DrawTile8M(i,sy + sh,29);
	VWB_DrawTile8M(i,sy,25),VWB_DrawTile8M(i,sy + sh,30);

	for (i = sy + 8;i <= sy + sh - 8;i += 8)
		VWB_DrawTile8M(sx,i,26),VWB_DrawTile8M(sx + sw,i,27);
}

///////////////////////////////////////////////////////////////////////////
//
//	US_CenterWindow2() - Generates a window of a given width & height in the
//		middle of the screen with green border
//
///////////////////////////////////////////////////////////////////////////
void
US_CenterWindow2(word w,word h)
{
	US_DrawWindow2(((MaxX / 8) - w) / 2,((MaxY / 8) - h) / 2,w,h);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_CenterSaveWindow() - Generates a window of a given width & height in
//              the middle of the screen, saving the background
//
///////////////////////////////////////////////////////////////////////////
void
US_CenterSaveWindow(word w,word h,memptr *save)
{
	word    x,y,
			screen;

	x = ((MaxX / 8) - w) / 2;
	y = ((MaxY / 8) - h) / 2;
	MM_GetPtr(save,(w * h) * CHARWIDTH);
	screen = bufferofs + panadjust + ylookup[y] + (x * CHARWIDTH);
	VW_ScreenToMem(screen,*save,w * CHARWIDTH,h);
	US_DrawWindow(((MaxX / 8) - w) / 2,((MaxY / 8) - h) / 2,w,h);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_RestoreSaveWindow() - Restores the background of the size of the
//              current window from the memory specified by save
//
///////////////////////////////////////////////////////////////////////////
void
US_RestoreSaveWindow(memptr *save)
{
	word    screen;

	screen = bufferofs + panadjust + ylookup[WindowY] + (WindowX * CHARWIDTH);
	VW_MemToScreen(*save,screen,WindowW * CHARWIDTH,WindowH);
	MM_FreePtr(save);
}

///////////////////////////////////////////////////////////////////////////
//
//      US_SaveWindow() - Saves the current window parms into a record for
//              later restoration
//
///////////////////////////////////////////////////////////////////////////
void
US_SaveWindow(WindowRec *win)
{
	win->x = WindowX;
	win->y = WindowY;
	win->w = WindowW;
	win->h = WindowH;

	win->px = PrintX;
	win->py = PrintY;
}

///////////////////////////////////////////////////////////////////////////
//
//      US_RestoreWindow() - Sets the current window parms to those held in the
//              record
//
///////////////////////////////////////////////////////////////////////////
void
US_RestoreWindow(WindowRec *win)
{
	WindowX = win->x;
	WindowY = win->y;
	WindowW = win->w;
	WindowH = win->h;

	PrintX = win->px;
	PrintY = win->py;
}

//      Cursor routines

#if 0
///////////////////////////////////////////////////////////////////////////
//
//      US_StartCursor() - Sets up the cursor for User Mgr use
//
///////////////////////////////////////////////////////////////////////////
void
US_StartCursor(void)
{
	CursorInfo      info;

	VW_SetCursor(CURSORARROWSPR);
	CursorX = MaxX / 2;
	CursorY = MaxY / 2;
	VW_MoveCursor(CursorX,CursorY);
	VW_ShowCursor();

	IN_ReadCursor(&info);   // Dispose of any accumulated movement
}

///////////////////////////////////////////////////////////////////////////
//
//      US_ShutCursor() - Cleans up after US_StartCursor()
//
///////////////////////////////////////////////////////////////////////////
void
US_ShutCursor(void)
{
	VW_HideCursor();
}

///////////////////////////////////////////////////////////////////////////
//
//      US_UpdateCursor() - Gets the new cursor position & button states from
//              the Input Mgr and tells the View Mgr where the cursor is
//
///////////////////////////////////////////////////////////////////////////
boolean
US_UpdateCursor(void)
{
	CursorInfo      info;

	IN_ReadCursor(&info);
	if (info.x || info.y || CursorBad)
	{
		CursorX += info.x;
		if (CursorX >= MaxX)
			CursorX = MaxX - 1;
		else if (CursorX < 0)
			CursorX = 0;

		CursorY += info.y;
		if (CursorY >= MaxY)
			CursorY = MaxY - 1;
		else if (CursorY < 0)
			CursorY = 0;

		VW_MoveCursor(CursorX,CursorY);
		CursorBad = false;
	}
	Button0 = info.button0;
	Button1 = info.button1;
	return(Button0 || Button1);
}
#endif

//      Input routines

///////////////////////////////////////////////////////////////////////////
//
//      USL_XORICursor() - XORs the I-bar text cursor. Used by US_LineInput()
//
///////////////////////////////////////////////////////////////////////////
 void
USL_XORICursor(int x,int y,char *s,word cursor)
{
	char    buf[MaxString];
	word    w,h;

	strcpy(buf,s);
	buf[cursor] = '\0';
	USL_MeasureString(buf,&w,&h);

	px = x + w - 1;
	py = y;
	USL_DrawString("\x80");
}

///////////////////////////////////////////////////////////////////////////
//
//      US_LineInput() - Gets a line of user input at (x,y), the string defaults
//              to whatever is pointed at by def. Input is restricted to maxchars
//              chars or maxwidth pixels wide. If the user hits escape (and escok is
//              true), nothing is copied into buf, and false is returned. If the
//              user hits return, the current string is copied into buf, and true is
//              returned
//
///////////////////////////////////////////////////////////////////////////
boolean
US_LineInput(int x,int y,char *buf,char *def,boolean escok,
				int maxchars,int maxwidth)
{
	boolean         redraw,
				cursorvis,cursormoved,
				done,result;
	ScanCode        sc;
	char            c,
				s[MaxString],olds[MaxString];
	word            i,
				cursor,
				w,h,
				len;
	longword        lasttime;

	VW_HideCursor();

	if (def)
		strcpy(s,def);
	else
		*s = '\0';
	*olds = '\0';
	cursor = strlen(s);
	cursormoved = redraw = true;

	cursorvis = done = false;
	lasttime = TimeCount;
	LastASCII = key_None;
	LastScan = sc_None;

	while (!done)
	{
		if (cursorvis)
			USL_XORICursor(x,y,s,cursor);


		sc = LastScan;
		LastScan = sc_None;
		c = LastASCII;
		LastASCII = key_None;


		switch (sc)
		{
		case sc_LeftArrow:
			if (cursor)
				cursor--;
			c = key_None;
			cursormoved = true;
			break;
		case sc_RightArrow:
			if (s[cursor])
				cursor++;
			c = key_None;
			cursormoved = true;
			break;
		case sc_Home:
			cursor = 0;
			c = key_None;
			cursormoved = true;
			break;
		case sc_End:
			cursor = strlen(s);
			c = key_None;
			cursormoved = true;
			break;

		case sc_Return:
			strcpy(buf,s);
			done = true;
			result = true;
			c = key_None;
			break;
		case sc_Escape:
			if (escok)
			{
				done = true;
				result = false;
			}
			c = key_None;
			break;

		case sc_BackSpace:
			if (cursor)
			{
				strcpy(s + cursor - 1,s + cursor);
				cursor--;
				redraw = true;
			}
			c = key_None;
			cursormoved = true;
			break;
		case sc_Delete:
			if (s[cursor])
			{
				strcpy(s + cursor,s + cursor + 1);
				redraw = true;
			}
			c = key_None;
			cursormoved = true;
			break;

		case 0x4c:      // Keypad 5
		case sc_UpArrow:
		case sc_DownArrow:
		case sc_PgUp:
		case sc_PgDn:
		case sc_Insert:
			c = key_None;
			break;
		}

		if (c)
		{
			len = strlen(s);
			USL_MeasureString(s,&w,&h);

			if
			(
				isprint(c)
			&&      (len < MaxString - 1)
			&&      ((!maxchars) || (len < maxchars))
			&&      ((!maxwidth) || (w < maxwidth))
			)
			{
				for (i = len + 1;i > cursor;i--)
					s[i] = s[i - 1];
				s[cursor++] = c;
				redraw = true;
			}
		}

		if (redraw)
		{
			px = x;
			py = y;
			USL_DrawString(olds);
			strcpy(olds,s);

			px = x;
			py = y;
			USL_DrawString(s);

			redraw = false;
		}

		if (cursormoved)
		{
			cursorvis = false;
			lasttime = TimeCount - TickBase;

			cursormoved = false;
		}
		if (TimeCount - lasttime > TickBase / 2)
		{
			lasttime = TimeCount;

			cursorvis ^= true;
		}
		if (cursorvis)
			USL_XORICursor(x,y,s,cursor);

		VW_UpdateScreen();
	}

	if (cursorvis)
		USL_XORICursor(x,y,s,cursor);
	if (!result)
	{
		px = x;
		py = y;
		USL_DrawString(olds);
	}
	VW_ShowCursor();
	VW_UpdateScreen();

	IN_ClearKeysDown();
	return(result);
}


///////////////////////////////////////////////////////////////////////////
//
//	US_FlushOnHotplug() - a controller came or went (SDL2 port)
//
///////////////////////////////////////////////////////////////////////////
void
US_FlushOnHotplug(void)
{
	IN_ClearKeysDown();
}
