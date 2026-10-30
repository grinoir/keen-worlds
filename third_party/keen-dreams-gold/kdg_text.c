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
 * Return to the Shadowlands
 * Copyright (C) 2021 Ceilick & K1n9_Duk3
 *
 * 5.
 * Reconstructed Commander Keen 4-6
 * Copyright (C) 2021 K1n9_Duk3
 *
 * 6.
 * Keen Dreams Unofficial Tech Demo
 * Copyright (C) 2015 NY00123
 *
 * 7.
 * Wolfenstein 3-D Source Code
 * Copyright (C) 1995 Id Software
 *
 * 8.
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * 9.
 * Catacomb 3-D Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "kdg_def.h"
#include <SDL.h>

/*
=============================================================================

TEXT FORMATTING COMMANDS
------------------------
^C<hex digit>  			Change text color
^E[enter]				End of layout (all pages)
^P[enter]				start new page, must be the first chars in a layout
^L<x>,<y>[ENTER]		Locate to a specific spot, x in pixels, y in lines

=============================================================================
*/

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define BACKCOLOR WHITE

#define WORDLIMIT 80
#define FONTHEIGHT 10
#define TOPMARGIN 10
#define BOTTOMMARGIN 10
#define LEFTMARGIN 10
#define RIGHTMARGIN 10
#define PICMARGIN 8
#define SPACEWIDTH 7
#define TEXTROWS ((200-TOPMARGIN-BOTTOMMARGIN)/FONTHEIGHT)
#define SCREENPIXWIDTH 320
#define SCREENMID (SCREENPIXWIDTH/2)
#define TABWIDTH		16		// K1n9_Duk3 addition

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

int pagenum,numpages;
int leftmargin[TEXTROWS],rightmargin[TEXTROWS];
unsigned char		far *text;	// K1n9_Duk3 fix: need unsigned chars!
int rowon;
int picx,picy,picnum,picdelay;
boolean layoutdone;

int helpmenupos;

void CacheLayoutGraphics (void);

//===========================================================================

/*
=====================
=
= RipToEOL
=
=====================
*/

void RipToEOL(void)
{
	while (*text++ != '\n')		// scan to end of line
	;
}


/*
=====================
=
= ParseNumber
=
=====================
*/

int ParseNumber(void)
{
	unsigned char	ch;	// K1n9_Duk3 fix: need unsigned chars!
	char	num[80],*numptr;

//
// scan until a number is found
//
	ch = *text;
	while (ch < '0' || ch >'9')
		ch = *++text;

//
// copy the number out
//
	numptr = num;
	do
	{
		*numptr++ = ch;
		ch = *++text;
	} while (ch >= '0' && ch <= '9');
	*numptr = 0;

	return atoi (num);
}


/*
=====================
=
= ParsePicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void ParsePicCommand(void)
{
	picy = ParseNumber();
	picx = ParseNumber();
	picnum = ParseNumber();
	RipToEOL();
}

void ParseTimedCommand(void)
{
	picy = ParseNumber();
	picx = ParseNumber();
	picnum = ParseNumber();
	picdelay = ParseNumber();
	RipToEOL();
}


/*
=====================
=
= ParseHexNumber		(K1n9_Duk3 addition)
=
=====================
*/

int ParseHexNumber (int val)
{
	unsigned char ch;	// K1n9_Duk3 fix: need unsigned chars!

//
// scan until a hex number is found
//
	ch = toupper(*text);
	while (!( (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') ))
	{
		if (ch == '\n' || ch == '\r')
			return val;
		ch = toupper(*++text);
	}

//
// copy the number out
//
	val = 0;
	while (true)
	{
		if (ch >= '0' && ch <= '9')
		{
			val <<= 4;
			val += ch-'0'+0;
		}
		else if (ch >= 'A' && ch <= 'F')
		{
			val <<= 4;
			val += ch-'A'+0xA;
		}
		else
		{
			break;
		}
		ch = toupper(*++text);
	}

	return val;
}


/*
=====================
=
= TimedPicCommand
=
= Call with text pointing just after a ^P
= Upon exit text points to the start of next line
=
=====================
*/

void TimedPicCommand(void)
{
	ParseTimedCommand ();
//
// draw pic
//
	VWB_DrawPic (picx&~7,picy,picnum);
}


/*
=====================
=
= HandleCommand
=
=====================
*/

void HandleCommand(void)
{
	int i,margin,top,bottom;
	int picwidth,picheight,picmid;

	switch (toupper(*(++text)))
	{
	case 'P':		// ^P is start of next page, ^E is end of file
	case 'E':
		layoutdone = true;
		text--;
		break;

	case 'C':		// ^c<hex digit> changes text color
		i = toupper(*++text);	// K1n9_Duk3 note: toupper returns an unsigned char (only -1 is retuned as -1)
		if (i>='0' && i<='9')
			fontcolor = i-'0';
		else if (i>='A' && i<='F')
			fontcolor = i-'A'+10;

#if 0
		fontcolor *= 16;
		i = toupper(*++text);	// K1n9_Duk3 note: toupper returns an unsigned char (only -1 is retuned as -1)
		if (i>='0' && i<='9')
			fontcolor += i-'0';
		else if (i>='A' && i<='F')
			fontcolor += i-'A'+10;
#endif

		fontcolor ^= BACKCOLOR;
		text++;
		break;

	case 'L':
		py = ParseNumber();
		rowon = (py - 10)/10;
		py = rowon * 10 + 10;
		px = ParseNumber();
		while (*(text++) != '\n')	// scan to end of line
		;
		break;
	}
}


/*
=====================
=
= NewLine
=
=====================
*/

void NewLine (void)
{
	char	ch;

	if (++rowon == TEXTROWS)
	{
	//
	// overflowed the page, so skip until next page break
	//
		layoutdone = true;
		do
		{
			if (*text == '^')
			{
				ch = toupper(*(text+1));
				if (ch == 'E' || ch == 'P')
				{
					layoutdone = true;
					return;
				}
			}
			text++;

		} while (1);

	}
	px = leftmargin[rowon];
	py+= FONTHEIGHT;
}


/*
=====================
=
= HandleCtrls
=
=====================
*/

void HandleCtrls (void)
{
	char	ch;

	ch = *text++;			// get the character and advance

	switch (ch)
	{
	case '\n':
		NewLine ();
		break;
	case '\t':				// K1n9_Duk3 addition: handle tabs
		px += TABWIDTH-LEFTMARGIN;
		px += LEFTMARGIN-(px % TABWIDTH);
		break;
	}

}

/*
=====================
=
= HandleWord
=
=====================
*/

void HandleWord(void)
{
	char		word[WORDLIMIT];
	int			i,wordindex;
	unsigned	wwidth,wheight,newpos;


	//
	// copy the next word into [word]
	//
	word[0] = *text++;
	wordindex = 1;
	while (*text>32)
	{
		word[wordindex] = *text++;
		if (++wordindex == WORDLIMIT)
			Quit ("PageLayout: Word limit exceeded");
	}
	word[wordindex] = 0;		// stick a null at end for C

	//
	// see if it fits on this line
	//
	VW_MeasurePropString (word,&wwidth,&wheight);

	while (px+wwidth > rightmargin[rowon])
	{
		NewLine ();
		if (layoutdone)
			return;		// overflowed page
	}

	//
	// print it
	//
	newpos = px+wwidth;
	VWB_DrawPropString (word);
	px = newpos;

	//
	// suck up any extra spaces
	//
	while (*text == ' ')
	{
		px += SPACEWIDTH;
		text++;
	}
}

/*
=====================
=
= PageLayout
=
= Clears the screen, draws the pics on the page, and word wraps the text.
= Returns a pointer to the terminating command
=
=====================
*/

void PageLayout(boolean shownumber)
{
	int		i,oldcolor;
	unsigned char	ch;	// K1n9_Duk3 fix: need unsigned chars!

	oldcolor = fontcolor;

	fontcolor = BLACK^BACKCOLOR;

//
// clear the screen
//
	VWB_Bar(0, 0, 320, 200, BACKCOLOR);
	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
	{
		VWB_DrawPic(  8, 0, H_MTOPWINDOWPIC);
		VWB_DrawPic(  0, 0, H_MLEFTWINDOWPIC);
		VWB_DrawPic(312, 0, H_MRIGHTWINDOWPIC);
	}
	else
	{
		VWB_DrawPic(  8, 0, H_TOPWINDOWPIC);
		VWB_DrawPic(  0, 0, H_LEFTWINDOWPIC);
		VWB_DrawPic(312, 0, H_RIGHTWINDOWPIC);
	}
	if (shownumber)
	{
		if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
			VWB_DrawPic(8, 176, H_MBOTTOMINFOPIC);
		else
			VWB_DrawPic(8, 176, H_BOTTOMINFOPIC);
	}
	else
	{
		if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
			VWB_DrawPic(8, 189, H_MBOTTOMWINDOWPIC);
		else
			VWB_DrawPic(8, 189, H_BOTTOMWINDOWPIC);
	}

	for (i=0;i<TEXTROWS;i++)
	{
		leftmargin[i] = LEFTMARGIN;
		rightmargin[i] = SCREENPIXWIDTH-RIGHTMARGIN;
	}

	px = LEFTMARGIN;
	py = TOPMARGIN;
	rowon = 0;
	layoutdone = false;

//
// make sure we are starting layout text (^P first command)
//
	while (*text <= 32)
		text++;

	if (*text != '^' && toupper(*++text) != 'P')
		Quit ("PageLayout: Text not headed with ^P");

	while (*text++ != '\n')
	;

//
// process text stream
//
	do
	{
		ch = *text;

		if (ch == '^')
			HandleCommand ();
		else
/*
		if (ch == 9)
		{
		//Tab
		 px = (px+8)&0xf8;
		 text++;
		}
		else 
*/
		if (ch <= 32)
			HandleCtrls ();
		else
			HandleWord ();

	} while (!layoutdone);

	pagenum++;

	if (shownumber)
	{
		strcpy(str, "pg ");
		itoa(pagenum, str2, 10);
		strcat(str, str2);
		strcat(str, " of ");
		itoa(numpages, str2, 10);
		strcat(str, str2);

		py = 183;
		px = 223;
		fontcolor = BLACK^BACKCOLOR;	// K1n9_Duk3 fix

		VWB_DrawPropString (str);
	}

	fontcolor = oldcolor;
}

//===========================================================================

/*
=====================
=
= BackPage
=
= Scans for a previous ^P
=
=====================
*/

void BackPage(void)
{
	pagenum--;
	do
	{
		text--;
		if (*text == '^' && toupper(*(text+1)) == 'P')
			return;
	} while (1);
}

//===========================================================================


/*
=====================
=
= CacheLayoutGraphics
=
= Scans an entire layout file (until a ^E) marking all graphics used, and
= counting pages, then caches the graphics in
=
=====================
*/
void CacheLayoutGraphics(void)
{
	char	far *bombpoint, far *textstart;
	char	ch;

	textstart = text;
	bombpoint = text+30000;
	numpages = pagenum = 0;

	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
	{
		CA_MarkGrChunk(H_MTOPWINDOWPIC);
		CA_MarkGrChunk(H_MLEFTWINDOWPIC);
		CA_MarkGrChunk(H_MRIGHTWINDOWPIC);
		CA_MarkGrChunk(H_MBOTTOMINFOPIC);
		CA_MarkGrChunk(H_MBOTTOMWINDOWPIC);
	}
	else
	{
		CA_MarkGrChunk(H_TOPWINDOWPIC);
		CA_MarkGrChunk(H_LEFTWINDOWPIC);
		CA_MarkGrChunk(H_RIGHTWINDOWPIC);
		CA_MarkGrChunk(H_BOTTOMINFOPIC);
		CA_MarkGrChunk(H_BOTTOMWINDOWPIC);		
	}

	CA_ClearMarks();

	do
	{
		if (*text == '^')
		{
			ch = toupper(*(++text));
			if (ch == 'P')		// start of a page
			{
				numpages++;
			}
			if (ch == 'E')		// end of file, so load graphics and return
			{
				if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
				{
					CA_MarkGrChunk(H_MTOPWINDOWPIC);
					CA_MarkGrChunk(H_MLEFTWINDOWPIC);
					CA_MarkGrChunk(H_MRIGHTWINDOWPIC);
					CA_MarkGrChunk(H_MBOTTOMINFOPIC);
					CA_MarkGrChunk(H_MBOTTOMWINDOWPIC);
					CA_CacheMarks (NULL, 0);
				}
				else
				{
					CA_MarkGrChunk(H_TOPWINDOWPIC);
					CA_MarkGrChunk(H_LEFTWINDOWPIC);
					CA_MarkGrChunk(H_RIGHTWINDOWPIC);
					CA_MarkGrChunk(H_BOTTOMINFOPIC);
					CA_MarkGrChunk(H_BOTTOMWINDOWPIC);
					CA_CacheMarks (NULL, 0);		
				}
				text = textstart;
				return;
			}
		}
		else
		{
			text++;
		}

	} while (text < bombpoint);

	Quit("CacheLayoutGraphics: No ^E to terminate file!");
}

//===========================================================================

/*
=================
=
= HelpMenu
=
=================
*/
int HelpMenu(void)
{
	CursorInfo cursor;
	ControlInfo control;
	int ydelta;
	int key;

	VWB_Bar(0, 0, 320, 200, BACKCOLOR);

	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
		CA_CacheGrChunk(H_MHELPPIC);
	else
		CA_CacheGrChunk(H_HELPPIC);
	CA_CacheGrChunk(H_HANDPIC);
	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
	{
		CA_CacheGrChunk(H_MTOPWINDOWPIC);
		CA_CacheGrChunk(H_MLEFTWINDOWPIC);
		CA_CacheGrChunk(H_MRIGHTWINDOWPIC);
		CA_CacheGrChunk(H_MBOTTOMWINDOWPIC);
	}
	else
	{
		CA_CacheGrChunk(H_TOPWINDOWPIC);
		CA_CacheGrChunk(H_LEFTWINDOWPIC);
		CA_CacheGrChunk(H_RIGHTWINDOWPIC);
		CA_CacheGrChunk(H_BOTTOMWINDOWPIC);
	}

	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
	{
		VWB_DrawPic(  8,   0, H_MTOPWINDOWPIC);
		VWB_DrawPic(  0,   0, H_MLEFTWINDOWPIC);
		VWB_DrawPic(312,   0, H_MRIGHTWINDOWPIC);
		VWB_DrawPic(  8, 189, H_MBOTTOMWINDOWPIC);
		VWB_DrawPic( 96,   8, H_MHELPPIC);
	}
	else
	{
		VWB_DrawPic(  8,   0, H_TOPWINDOWPIC);
		VWB_DrawPic(  0,   0, H_LEFTWINDOWPIC);
		VWB_DrawPic(312,   0, H_RIGHTWINDOWPIC);
		VWB_DrawPic(  8, 189, H_BOTTOMWINDOWPIC);
		VWB_DrawPic( 96,   8, H_HELPPIC);	
	}

//	VWB_DrawPic( 96,   8, H_HELPPIC);

	ydelta = 0;
	IN_ClearKeysDown();
	do
	{
		if (helpmenupos < 0)
		{
			helpmenupos = 0;
		}

		else if (helpmenupos > 4)
		{
			helpmenupos = 4;
		}
		VWB_DrawPic(48, 24*helpmenupos+48, H_HANDPIC);
		VW_UpdateScreen();
		VWB_Bar(48, 24*helpmenupos+48, 39, 24, BACKCOLOR);
		IN_ReadControl(0, &control);
		IN_ReadCursor(&cursor);
		if (LastScan)
		{
			key = LastScan;
			IN_ClearKeysDown();
			switch (key)
			{
			case sc_UpArrow:
				helpmenupos--;
				break;
			case sc_DownArrow:
				helpmenupos++;
				break;
			case sc_Enter:
				VW_ClearVideo(BLACK);
				return helpmenupos;
			case sc_Escape:
				VW_ClearVideo(BLACK);
				return -1;
			}
		}
		ydelta += cursor.y;
		if (cursor.button0 || cursor.button1 || control.button0 || control.button1)
		{
			VW_ClearVideo(BLACK);
			return helpmenupos;
		}
		if (ydelta < -40)
		{
			ydelta += 40;
			helpmenupos--;
		}
		else if (ydelta > 40)
		{
			ydelta -= 40;
			helpmenupos++;
		}
	} while (1);
}

/*
=================
=
= HelpScreens
=
=================
*/
void HelpScreens(void)
{

	static int layouttable[5] =
	{
		T_CPART,
		T_THEGAMEART,
		T_STORYART,
		T_KEENINFOART,
		T_MEART
	};

	static int mlayouttable[5] =
	{
		T_MCPART,
		T_MTHEGAMEART,
		T_MSTORYART,
		T_MORTINFOART,
		T_MMEART
	};

	int olddisplayofs, oldbufferofs, oldfontnumber, temp;
	int pos;
	boolean newpage;

	oldfontnumber = fontnumber;
	olddisplayofs = displayofs;
	oldbufferofs = bufferofs;
	fontnumber = 0;

	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
	{
		CA_CacheGrChunk(H_MTOPWINDOWPIC);
		CA_CacheGrChunk(H_MLEFTWINDOWPIC);
		CA_CacheGrChunk(H_MRIGHTWINDOWPIC);
		CA_CacheGrChunk(H_MBOTTOMWINDOWPIC);
	}
	else
	{
		CA_CacheGrChunk(H_TOPWINDOWPIC);
		CA_CacheGrChunk(H_LEFTWINDOWPIC);
		CA_CacheGrChunk(H_RIGHTWINDOWPIC);
		CA_CacheGrChunk(H_BOTTOMWINDOWPIC);	
	}

	if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
	{
		VWB_DrawPic(  8,   0, H_MTOPWINDOWPIC);
		VWB_DrawPic(  0,   0, H_MLEFTWINDOWPIC);
		VWB_DrawPic(312,   0, H_MRIGHTWINDOWPIC);
		VWB_DrawPic(  8, 192, H_MBOTTOMWINDOWPIC);
	}
	else
	{
		VWB_DrawPic(  8,   0, H_TOPWINDOWPIC);
		VWB_DrawPic(  0,   0, H_LEFTWINDOWPIC);
		VWB_DrawPic(312,   0, H_RIGHTWINDOWPIC);
		VWB_DrawPic(  8, 192, H_BOTTOMWINDOWPIC);		
	}

#if GRMODE == EGAGR
	EGAMAPMASK(15);
#endif

	CA_UpLevel();
	CA_SetGrPurge();
	VW_ClearVideo(BLACK);

#if GRMODE == EGAGR
	RF_FixOfs();
	bufferofs = 0;
	displayofs = 0x8000;
	VW_SetScreen(displayofs, 0);
#endif

	StartMusic(55);

	do
	{
		pos = HelpMenu();

		VW_ClearVideo(BLACK);

		if (pos == -1)
		{
			// K1n9_Duk3 fix: stop the music before re-caching to make more memory available
			StopMusic();
			CA_ClearMarks();

			CA_DownLevel();
			IN_ClearKeysDown();
			bufferofs = oldbufferofs;
			displayofs = olddisplayofs;
			fontnumber = oldfontnumber;
			VW_ClearVideo(BLACK);
			RF_FixOfs();
			SD_FadeOutMusic();
			while (SD_MusicPlaying());
			StopMusic();
			StartMusic(53);
			return;
		}

		if (gamestate.difficulty == gd_Grand && mapon >= 0 && mapon <= 46)
		{
			pos = mlayouttable[pos];
		}	
		else
		{
			pos = layouttable[pos];
		}
		CA_CacheGrChunk(pos);
		MM_SetLock(&grsegs[pos], true);	// K1n9_Duk3 fix: prevent Memory Manager from messing with the text
		text = grsegs[pos];
		CA_ClearMarks();	// RTTS (Return to the Shadowlands) fix
		CacheLayoutGraphics();

		newpage = true;
		do
		{
			if (newpage)
			{
				newpage = false;
				PageLayout(true);
#if GRMODE == CGAGR
				VW_UpdateScreen();
#else
				VW_SetScreen(bufferofs, 0);
				temp = displayofs;
				displayofs = bufferofs;
				bufferofs = temp;
#endif
			}

			LastScan = 0;
			while (!LastScan) { IN_PumpEvents(); SDL_Delay(1); }

			switch (LastScan)
			{
			case sc_UpArrow:
			case sc_LeftArrow:
			case sc_PgUp:
				if (pagenum > 1)
				{
					BackPage();
					BackPage();
					newpage = true;
				}
				break;
			case sc_DownArrow:
			case sc_RightArrow:
			case sc_PgDn:
				if (pagenum < numpages)
				{
					newpage = true;
				}
				break;
			}
		} while (LastScan != sc_Escape);

		MM_FreePtr(&grsegs[pos]);
		IN_ClearKeysDown();
	} while (true);
}

/*
=================
=
= FinaleLayout
=
=================
*/
void FinaleLayout(void)
{
	char _seg *textseg;
	int i;

	VW_ClearVideo(BLACK);
	RF_FixOfs();
	CA_UpLevel();
	CA_ClearMarks();	// RTTS fix
	CA_SetGrPurge();
	if (gamestate.difficulty == gd_Grand)
	{
		CA_CacheGrChunk(H_MFLASHARROW2PIC);
		CA_CacheGrChunk(H_MFLASHARROW1PIC);
	}
	else
	{
		CA_CacheGrChunk(H_FLASHARROW2PIC);
		CA_CacheGrChunk(H_FLASHARROW1PIC);
	}

	if (gamestate.difficulty == gd_Easy)
	{
		CA_CacheGrChunk(T_QENDART);
		MM_SetLock(&grsegs[T_QENDART], true);	// RTTS fix
		textseg = grsegs[T_QENDART];		
	}
	else if (gamestate.difficulty == gd_Grand)
	{
		CA_CacheGrChunk(T_MENDART);
		MM_SetLock(&grsegs[T_MENDART], true);	// RTTS fix
		textseg = grsegs[T_MENDART];
	}
	else
	{
		CA_CacheGrChunk(T_KENDART);
		MM_SetLock(&grsegs[T_KENDART], true);	// RTTS fix
		textseg = grsegs[T_KENDART];					
	}

	text = textseg;
	CacheLayoutGraphics();

	if (gamestate.difficulty == gd_Easy)
		StartMusic(54);
	else if (gamestate.difficulty == gd_Grand)
		StartMusic(51);
	else
		StartMusic(49);

	while (pagenum < numpages)
	{
		PageLayout(false);
		IN_ClearKeysDown();
#if GRMODE == CGAGR
		VW_UpdateScreen();
#else
		VW_SetScreen(bufferofs, 0);
#endif

		do
		{

			if (gamestate.difficulty == gd_Grand)
				VWB_DrawPic(298, 180, H_MFLASHARROW1PIC);
			else
				VWB_DrawPic(298, 180, H_FLASHARROW1PIC);
#if GRMODE == CGAGR
			VW_UpdateScreen();
#endif
			for (i=0; i<TickBase; i++)
			{
				if (IN_IsUserInput())
				{
					goto nextpage;
				}
				VW_WaitVBL(1);
			}

			if (gamestate.difficulty == gd_Grand)
				VWB_DrawPic(298, 180, H_MFLASHARROW2PIC);
			else
				VWB_DrawPic(298, 180, H_FLASHARROW2PIC);
#if GRMODE == CGAGR
			VW_UpdateScreen();
#endif
			for (i=0; i<TickBase; i++)
			{
				if (IN_IsUserInput())
				{
					goto nextpage;
				}
				VW_WaitVBL(1);
			}
		} while (1);

nextpage:
		;	// Borland C++ 2.0 needs a semicolon here...
	}

	StopMusic();

	if (gamestate.difficulty == gd_Easy)
	{
		MM_FreePtr(&grsegs[T_QENDART]);
	}
	else if (gamestate.difficulty == gd_Grand)
	{
		MM_FreePtr(&grsegs[T_MENDART]);
	}
	else
	{
		MM_FreePtr(&grsegs[T_KENDART]);
	}
	if (gamestate.difficulty == gd_Grand)
	{
		MM_FreePtr(&grsegs[H_MFLASHARROW1PIC]);
		MM_FreePtr(&grsegs[H_MFLASHARROW2PIC]);
	}
	else
	{
		MM_FreePtr(&grsegs[H_FLASHARROW1PIC]);
		MM_FreePtr(&grsegs[H_FLASHARROW2PIC]);
	}
	CA_DownLevel();
	IN_ClearKeysDown();
#if GRMODE != CGAGR
	VW_ClearVideo(BLACK);
	RF_FixOfs();
#endif
	CA_FreeGraphics();
}
