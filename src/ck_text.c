/*
Omnispeak: A Commander Keen Reimplementation
Copyright (C) 2012 David Gow <david@ingeniumdigital.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kw_version.h"
#include "id_ca.h"
#include "id_in.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_cross.h"
#include "ck_def.h"
#include "ck_text.h"
#include "kw_music.h"
#include "kw_skip.h"
#include "kw_world.h"
#include "kw_suburb.h"

int help_delay, help_pic, help_y, help_x;
char *help_ptr;
int help_line;
int help_line_startx[18], help_line_endx[18];
int help_cur_page, help_num_pages, help_topic;
int help_full_page;
/* The chunks for each of the topics */
#ifdef HAS_HELPSCREEN
chunk_id_t help_chunks[] = {
	CK_CHUNKID(TEXT_HELPMENU), CK_CHUNKID(TEXT_CONTROLS), CK_CHUNKID(TEXT_STORY), CK_CHUNKID(TEXT_ORDER), CK_CHUNKID(TEXT_ABOUTID)

};
#endif

void RipToEOL(void)
{
	while (*help_ptr++ != '\n')
		;
}

int ParseNumber(void)
{
	char s[81];
	char *p;
	char c;

	/* Find first digit */
	c = *(char *)help_ptr;
	while (c < '0' || c > '9')
	{
		help_ptr++;
		c = *help_ptr;
	}

	/* Copy digits into array */
	p = s;
	do
	{
		*p++ = c; /* will be digit */

		help_ptr++;
		c = *help_ptr;
	} while (c >= '0' && c <= '9');

	*p = 0; /* Null terminator */

	return atoi(s);
}

void ParsePicCommand(void)
{
	help_y = ParseNumber();
	help_x = ParseNumber();
	help_pic = ParseNumber();
	RipToEOL();
}

/*   F.00A1                       sub_276         proc    near */
void ParseTimedCommand(void)
{
	help_y = ParseNumber();
	help_x = ParseNumber();
	help_pic = ParseNumber();
	help_delay = ParseNumber();
	RipToEOL();
}

static KW_Presentation ck_timedPageControls;
static bool ck_timedPageActive, ck_timedPageSkipped;

void TimedPicCommand(void)
{
	// int timeCount = 0;

	ParseTimedCommand();
	// VW_WaitVBL( 1 );
	VL_GetTics(1);
	// VWL_ScreenToScreen( AZ : A7B4, AZ : A7B2, 40, 200 );
	VL_Present();
	// (long) TimeCount = 0;
	// SD_SetTimeCount(0);

	if (!getenv("KW_AUTOPAGES") && ck_timedPageActive)
	{
		VH_UpdateScreen();
		uint32_t started=SD_GetTimeCount();
		while ((uint32_t)(SD_GetTimeCount()-started)<(uint32_t)help_delay)
		{
			KW_LinePoll();
			if (KW_PollPresentation(&ck_timedPageControls,false))
			{
				ck_timedPageSkipped=true;
				KW_StopVoice();
				while (*help_ptr && !(help_ptr[0]=='^' && (CK_Cross_toupper(help_ptr[1])=='P' || CK_Cross_toupper(help_ptr[1])=='E')))
					++help_ptr;
				help_full_page=1;
				KW_ClearPresentationHint(&ck_timedPageControls);
				return;
			}
			KW_PresentWithSkipHint(&ck_timedPageControls,false);
			VL_DelayTics(1);
		}
		KW_ClearPresentationHint(&ck_timedPageControls);
	}
	else if (!getenv("KW_AUTOPAGES")) VL_DelayTics(help_delay);
	// Alphamatic intentionally places a dummy timed picture below the
	// visible page (e.g. Y=400) to pause without drawing. DOS could write
	// outside its displayed page; a native surface must stay in bounds.
	VH_BitmapTableEntry dimensions=VH_GetBitmapTableEntry(help_pic-ca_gfxInfoE.offBitmaps);
	if (help_x>=0 && help_y>=0 && help_x+dimensions.width*8<=320 && help_y+dimensions.height<=200)
		VHB_DrawBitmap(help_x & 0xFFF8, help_y, help_pic);
}

// KEEN WORLDS: Foray in the Forest's text pages (its CK_TEXT.C; K1n9_Duk3's
// additions to the layout language). Its pages are green, a box can have a
// colour, a picture can be a sprite or a tile, and there are commands the
// games' own viewer doesn't have: ^; a comment, ^I a condition (the text up
// to the next ^I is left out when it is false), ^M music, ^S a sound, ^W
// wait for the sound, ^Q quiet, ^D a pause, ^F the font, ^H the high scores.
bool FF_IsActive(void);
bool FF_TextCondition(int number);
// What a page of its says depends on its conditions, so its pages are spoken
// by the stretches of text between page marks and conditions: each stretch
// that is shown is noted by where its first word is in the text
// (help_units), and its clip is named after that place
// (tools/kw_fitf_end_voice.py).
static const char *help_textStart;
static int help_units[12], help_numUnits;
static bool help_unitPending;
void FF_StartMusic(uint16_t song, bool repeat);
#define HELP_BACKCOLOUR (FF_IsActive() ? 2 : 4)

// (its ParseHexNumber: the next hex number on this line, or `val` if none)
static int ParseHexNumber(int val)
{
	unsigned char ch = (unsigned char)CK_Cross_toupper(*help_ptr);
	while (!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F')))
	{
		if (ch == '\n' || ch == '\r')
			return val;
		ch = (unsigned char)CK_Cross_toupper(*++help_ptr);
	}
	val = 0;
	while (true)
	{
		if (ch >= '0' && ch <= '9')
			val = (val << 4) + ch - '0';
		else if (ch >= 'A' && ch <= 'F')
			val = (val << 4) + ch - 'A' + 10;
		else
			break;
		ch = (unsigned char)CK_Cross_toupper(*++help_ptr);
	}
	return val;
}

// (its TextDelay: show what is drawn so far, then wait; not in the scripted
// dialogue test, which turns the pages itself)
static void FF_TextDelay(int tics)
{
	VH_UpdateScreen();
	if (!getenv("KW_AUTOTALK"))
		IN_UserInput(tics, false);
}

// Its commands; false for one the games' viewer handles itself.
static bool FF_HandleCommand(void)
{
	char c = CK_Cross_toupper(*help_ptr);
	switch (c)
	{
	case ';':
		RipToEOL();
		return true;
	case 'H': // (the high scores are not drawn here)
		help_ptr++;
		return true;
	case 'I':
	{
		int num = ParseNumber();
		RipToEOL();
		if (!FF_TextCondition(num))
			while (!(help_ptr[0] == '^' && CK_Cross_toupper(help_ptr[1]) == 'I') &&
				!(help_ptr[0] == '^' && CK_Cross_toupper(help_ptr[1]) == 'E'))
				help_ptr++;
		help_unitPending = true;
		return true;
	}
	case 'M':
		switch (CK_Cross_toupper(help_ptr[1]))
		{
		case 'P':
			SD_MusicOff();
			break;
		case 'R':
			SD_MusicOn();
			break;
		case 'O':
			FF_StartMusic(ParseNumber(), false);
			break;
		default:
			FF_StartMusic(ParseNumber(), true);
			break;
		}
		RipToEOL();
		return true;
	case 'S':
		SD_PlaySound(ParseNumber());
		RipToEOL();
		return true;
	case 'W':
		SD_WaitSoundDone();
		RipToEOL();
		return true;
	case 'Q':
		StopMusic();
		SD_StopSound();
		RipToEOL();
		return true;
	case 'D':
	{
		int tics = ParseNumber();
		RipToEOL();
		FF_TextDelay(tics);
		return true;
	}
	case 'F':
		US_SetPrintFont(ParseNumber());
		return true;
	}
	return false;
}

void HandleCommand(void)
{
	int16_t i, w, h, midx, wrapx, miny, maxy;
	VH_BitmapTableEntry bmpinfo;

	help_ptr++;
	if (FF_IsActive() && FF_HandleCommand())
		return;
	switch (CK_Cross_toupper(*help_ptr))
	{
	case 'B':
		/* Solid red box y, x, w, h */
		help_y = ParseNumber();
		help_x = ParseNumber();
		w = ParseNumber();
		h = ParseNumber();
		// (Foray in the Forest: in a colour of its own, if one is given)
		VHB_Bar(help_x, help_y, w, h, FF_IsActive() ? ParseHexNumber(HELP_BACKCOLOUR) : 4);
		RipToEOL();
		break;

	case 'P':
	case 'E':
		/* Page or end of text */
		help_full_page = 1;
		help_ptr--;
		break;

	case 'C':
		/* Set the text color */
		help_ptr++;

		/* A single hex digit is used for the text color */
		i = CK_Cross_toupper(*help_ptr);
		if (i >= '0' && i <= '9')
			US_SetPrintColour(i - '0');
		else if (i >= 'A' && i <= 'F')
			US_SetPrintColour(i - 'A' + 10);

		/* We're writing on a red background, so take that into account */
		US_SetPrintColour(US_GetPrintColour() ^ HELP_BACKCOLOUR);
		help_ptr++;
		break;

	case 'L':
		/* Set the next location for text */
		US_SetPrintY(ParseNumber());

		/* Set the y position to the nearest line */
		help_line = (US_GetPrintY() - 10) / 10;
		US_SetPrintY(help_line * 10 + 10);

		/* Set the x position */
		US_SetPrintX(ParseNumber());

		/* Skip to the next line */
		while (*help_ptr++ != '\n')
			;
		break;

	case 'T':
		TimedPicCommand();
		break;

	case 'G':
		/* Draw the picture */
		ParsePicCommand();
		// KEEN WORLDS (Foray in the Forest): it may be a sprite or a tile
		if (FF_IsActive() && help_pic >= ca_gfxInfoE.offSprites && help_pic < ca_gfxInfoE.offSprites + ca_gfxInfoE.numSprites)
		{
			VH_SpriteTableEntry spr = VH_GetSpriteTableEntry(help_pic - ca_gfxInfoE.offSprites);
			CA_CacheGrChunk(help_pic);
			VHB_DrawSprite(help_x - (spr.originX >> 4), help_y - (spr.originY >> 4), help_pic);
			w = spr.width * 8;
			h = spr.height;
		}
		else if (FF_IsActive() && help_pic >= ca_gfxInfoE.offTiles16 && help_pic < ca_gfxInfoE.offTiles16 + ca_gfxInfoE.numTiles16)
		{
			CA_CacheGrChunk(help_pic); // (the level's tiles are not kept while text is shown)
			VHB_DrawTile16(help_x & ~7, help_y, help_pic - ca_gfxInfoE.offTiles16);
			w = h = 16;
		}
		else if (FF_IsActive() && help_pic >= ca_gfxInfoE.offTiles16m && help_pic < ca_gfxInfoE.offTiles16m + ca_gfxInfoE.numTiles16m)
		{
			CA_CacheGrChunk(help_pic);
			VHB_DrawTile16M(help_x & ~7, help_y, help_pic - ca_gfxInfoE.offTiles16m);
			w = h = 16;
		}
		else
		{
		VHB_DrawBitmap(help_x & ~7, help_y, help_pic);
		bmpinfo = VH_GetBitmapTableEntry(help_pic - ca_gfxInfoE.offBitmaps);
		w = bmpinfo.width * 8;
		h = bmpinfo.height;
		}

		/* Wrap text either on the left or the right-hand-side of the picture */
		midx = help_x + w / 2;
		if (midx > 160)
			wrapx = help_x - 8;
		else
			wrapx = help_x + w + 8;

		/* Get the first line that the picture covers */
		miny = (help_y - 10) / 10;
		if (miny < 0)
			miny = 0;

		/* Get the last line that the picture covers */
		maxy = (help_y + h - 10) / 10;
		if (maxy >= 18)
			maxy = 17;

		/* Set the lines' starting or finishing text positions */
		for (i = miny; i <= maxy; i++)
		{
			if (midx > 160) /* Wrapping to the left */
				help_line_endx[i] = wrapx;
			else /* Wrapping to the right */
				help_line_startx[i] = wrapx;
		}

		/* Make sure the next print doesn't start in the middle of the pic */
		if (help_line_startx[help_line] > US_GetPrintX())
			US_SetPrintX(help_line_startx[help_line]);
		break;
	}
}

void NewLine(void)
{
	char c;

	help_line++;
	if (help_line == 18)
	{
		/* Move back to line 1 and read input till the next ^P or ^E command */
		help_full_page = 1;
		while (1)
		{
			if (*help_ptr == '^')
			{
				c = CK_Cross_toupper(*(help_ptr + 1));
				if (c == 'E' || c == 'P')
				{
					help_full_page = 1;
					return;
				}
			}
			help_ptr++;
		}
	}

	/* Move to the next line */
	US_SetPrintX(help_line_startx[help_line]);
	US_SetPrintY(US_GetPrintY() + 10);
}

void HandleChar(void)
{
	char c;
	c = *help_ptr++;

	/* If the last character was a newline, do something about it! */
	if (c == '\n')
		NewLine();
	// KEEN WORLDS (Foray in the Forest): a tab goes on to the next 16th pixel
	else if (c == '\t' && FF_IsActive())
	{
		int x = US_GetPrintX() + 16 - 10;
		US_SetPrintX(x + 10 - (x % 16));
	}
}

void HandleWord(void)
{
	char buf[80];
	uint16_t w, h, maxx;
	int16_t i;

	// KEEN WORLDS: the first word of a stretch of text (see help_units)
	if (help_unitPending && help_textStart)
	{
		if (help_numUnits < (int)(sizeof(help_units) / sizeof(help_units[0])))
			help_units[help_numUnits++] = (int)(help_ptr - help_textStart);
		help_unitPending = false;
	}

	/* Read a word into the buffer */
	buf[0] = *help_ptr++;
	i = 1;
	while (*help_ptr > ' ')
	{
		buf[i++] = *help_ptr++;
		if (i == 80)
			Quit("PageLayout: Word limit exceeded");
	}

	/* Terminate the buffer with a null */
	buf[i] = 0;

	/* Measure the word */
	VH_MeasurePropString(buf, &w, &h, US_GetPrintFont());

	/* See if we can find a line to fit the word on */
	while (help_line_endx[help_line] < US_GetPrintX() + w)
	{
		NewLine();
		if (help_full_page)
			return;
	}

	/* Display the word */
	maxx = US_GetPrintX() + w;
	VHB_DrawPropString(buf, US_GetPrintX(), US_GetPrintY(), US_GetPrintFont(), US_GetPrintColour());
	US_SetPrintX(maxx);

	/* Handle spaces between this word and the next */
	while (*help_ptr == ' ')
	{
		US_SetPrintX(US_GetPrintX() + 7);
		help_ptr++;
	}
}

static bool ck_textBlackBackground; // KEEN WORLDS, see CK_ShowTextPages

void PageLayout(int show_status)
{
	int8_t old_print_color;
	int i;
	char c;

	/* Save the current print color */
	old_print_color = US_GetPrintColour();
	US_SetPrintColour(14 ^ HELP_BACKCOLOUR); // (yellow on the page's colour: 10 on red)

	/* We want to scanout from the right offset. */
	VL_SetScrollCoords(0, 0);

	/* Fill the background and draw the border */
	// KEEN WORLDS: ck_textBlackBackground gives a plain black page (some of
	// Atroxian Realm's story screens)
	VHB_Bar(0, 0, 320, 200, ck_textBlackBackground ? 0 : HELP_BACKCOLOUR);
#ifdef HAS_HELPSCREEN
	if (ck_currentEpisode->ep != EP_CK6 && !ck_textBlackBackground)
	{
		VHB_DrawBitmap(0, 0, CK_CHUNKNUM(PIC_BORDERTOP));     /* Top border */
		VHB_DrawBitmap(0, 8, CK_CHUNKNUM(PIC_BORDERLEFT));    /* Left border */
		VHB_DrawBitmap(312, 8, CK_CHUNKNUM(PIC_BORDERRIGHT)); /* Right border */
		if (show_status)
			VHB_DrawBitmap(8, 176, CK_CHUNKNUM(PIC_BORDERBOTTOMSTATUS)); /* Bottom status bar */
		else
			VHB_DrawBitmap(8, 192, CK_CHUNKNUM(PIC_BORDERBOTTOM)); /* Bottom border */
	}
#endif

	/* Set the lines' start and end positions so the text stays within the border */
	for (i = 0; i < 18; i++)
	{
		help_line_startx[i] = 10;
		help_line_endx[i] = 310;
	}

	/* The text should start inside the border */
	US_SetPrintX(10);
	US_SetPrintY(10);
	help_line = 0;
	help_full_page = 0;
	help_numUnits = 0;
	help_unitPending = true;

	/* Skip any nonprinting characters at the start */
	while (*help_ptr <= ' ')
		help_ptr++;

	/* Make sure we start with a "new page" command */
	if (!(*help_ptr++ == '^' && CK_Cross_toupper(*help_ptr) == 'P'))
		Quit("PageLayout: Text not headed with ^P");

	/* Move to the next line after the ^P */
	while (*help_ptr++ != '\n')
		;

	do
	{
		/* Parse a command or print a word */
		c = *help_ptr;
		if (c == '^')
			HandleCommand();
		else if (c <= ' ')
			HandleChar();
		else
			HandleWord();
	} while (help_full_page == 0);

	/* Set the page number that we're on */
	help_cur_page++;

	/* Show the current page number */
	if (show_status)
	{
		char buf[64];
		snprintf(buf, sizeof(buf), "pg %d of %d", help_cur_page, help_num_pages);

		US_SetPrintColour(8);
		US_SetPrintY(186);
		US_SetPrintX(218);
		VHB_DrawPropString(buf, US_GetPrintX(), US_GetPrintY(), US_GetPrintFont(), US_GetPrintColour());
	}

	/* Restore the original print color */
	US_SetPrintColour(old_print_color);
}

void BackPage(void)
{
	help_cur_page--;

	/* Look backwards for a '^P' */
	do
	{
		help_ptr--;
	} while (!(*help_ptr == '^' && CK_Cross_toupper(*(help_ptr + 1)) == 'P'));
}

extern uint8_t ca_levelnum;
extern uint8_t ca_levelbit;
extern uint8_t ca_graphChunkNeeded[CA_MAX_GRAPH_CHUNKS];

void CacheLayoutGraphics(void)
{
	char *pstart;
	char *pmax;
	char c;

	/* Initialise the pointers */
	pstart = help_ptr;
	pmax = pstart + 30000;

	help_num_pages = help_cur_page = 0;

	/* Cache the border graphics */
#ifdef HAS_HELPSCREEN
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_BORDERTOP));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_BORDERLEFT));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_BORDERRIGHT));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_BORDERBOTTOMSTATUS));
	CA_MarkGrChunk(CK_CHUNKNUM(PIC_BORDERBOTTOM));
#endif

	do
	{
		if (*help_ptr == '^')
		{
			help_ptr++;
			c = CK_Cross_toupper(*help_ptr);

			/* Count pages */
			if (c == 'P')
			{
				help_num_pages++;
			}
			/* If we've reached the end, rewind the help pointer and return */
			if (c == 'E')
			{
				CA_CacheMarks(0);
				help_ptr = pstart;
				return;
			}
			/* Cache ordinary graphics */
			if (c == 'G')
			{
				ParsePicCommand();
				ca_graphChunkNeeded[help_pic] |= ca_levelbit;
			}
			/* Cache timed graphics */
			if (c == 'T')
			{
				ParseTimedCommand();
				ca_graphChunkNeeded[help_pic] |= ca_levelbit;
			}
		}
		else
		{
			help_ptr++;
		}
	} while (help_ptr < pmax);

	/* If we got here, we didn't find the end of the text */
	Quit("CacheLayoutGraphics: No ^E to terminate file!");
}

#ifdef HAS_HELPSCREEN

int ShowHelp(void)
{
	int ymove, key;
	IN_ControlFrame cinfo;
	//CONTROLLER_STATUS cstatus;

	/* Erase the screen */
	VHB_Bar(0, 0, 320, 200, 4);

	/* Cache graphics */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_HELPMENU));     /* Help menu pic */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_HELPPOINTER));  /* Help menu pointer */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_BORDERTOP));    /* Top border */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_BORDERLEFT));   /* Left border */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_BORDERRIGHT));  /* Right border */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_BORDERBOTTOM)); /* Bottom border */

	/* Draw the border and the main menu pic */
	VHB_DrawBitmap(0, 0, CK_CHUNKNUM(PIC_BORDERTOP));      /* Top border */
	VHB_DrawBitmap(0, 8, CK_CHUNKNUM(PIC_BORDERLEFT));     /* Left border */
	VHB_DrawBitmap(312, 8, CK_CHUNKNUM(PIC_BORDERRIGHT));  /* Right border */
	VHB_DrawBitmap(8, 192, CK_CHUNKNUM(PIC_BORDERBOTTOM)); /* Bottom border */
	VHB_DrawBitmap(96, 8, CK_CHUNKNUM(PIC_HELPMENU));      /* Menu picture */

	ymove = 0;
	IN_ClearKeysDown();
	while (1)
	{
		IN_PumpEvents();

		/* Make sure the pointer is in a valid place */
		if (help_topic < 0)
			help_topic = 0;
		else if (help_topic > 4)
			help_topic = 4;

		/* Draw the pointer */
		VHB_DrawBitmap(48, 48 + help_topic * 24, CK_CHUNKNUM(PIC_HELPPOINTER));
		VL_SetScrollCoords(0, 0);
		VH_UpdateScreen();

		/* Erase the pointer */
		VHB_Bar(48, 48 + help_topic * 24, 39, 24, 4);

		/* Check for input */
		IN_ReadControls(0, &cinfo);
		// get_controller_status( &cstatus );

		/* Handle keys */
		if (IN_GetLastScan() != IN_SC_None)
		{
			key = IN_GetLastScan();
			IN_ClearKeysDown();
			switch (key)
			{
			case IN_SC_UpArrow:
				help_topic--;
				break;
			case IN_SC_Escape:
				VL_ClearScreen(4);
				return -1;
			case IN_SC_Enter:
				VL_ClearScreen(4);
				return help_topic;
			case IN_SC_DownArrow:
				help_topic++;
				break;
			}
		}

		/* Handle other input */
		//ymove += cstatus.dy;
		if (/*cstatus.button1 || cstatus.button2 || */ cinfo.jump || cinfo.pogo)
		{
			VL_ClearScreen(4);
			return help_topic;
		}

		if (ymove < -40)
		{
			ymove += 40;
			help_topic--;
		}
		else if (ymove > 40)
		{
			ymove -= 40;
			help_topic++;
		}
	}
}

void HelpScreens(void)
{
	int16_t oldfont, page_changed, n;

	/* Save the variables */
	oldfont = US_GetPrintFont();

	/* Init stuff */
	VL_SetScrollCoords(0, 0);
	VL_ClearScreen(4);
	US_SetPrintFont(0);

	if (ck_currentEpisode->ep == EP_CK5)
		StartMusic(CK_INT(CK_HelpMusicLevel,19));

	while (1)
	{
		n = ShowHelp();
		VL_ClearScreen(4);

		/* If the user pressed Esc */
		if (n == -1)
		{
			/* Clean up and return */
			// CA_DownLevel();
			IN_ClearKeysDown();
			US_SetPrintFont(oldfont);
			VL_ClearScreen(4);
			// RF_Reset();
			if (ck_currentEpisode->ep == EP_CK5)
				StopMusic();
			return;
		}

		// Suburb's full illustrated family story remains available in Help;
		// arrival uses its shorter narrated scroll, as the original did.
		if (SS_IsActive() && n==2)
		{
			ck_textPagesVoice="STORY";
			CK_ShowTextPages(CK_CHUNKNUM(TEXT_STORY),CK_INT(CK_HelpMusicLevel,25),false);
			ck_textPagesVoice=NULL;
			continue;
		}
		/* Cache the chunk with the topic */
		n = CK_LookupChunk(help_chunks[n]);
		CA_CacheGrChunk(n);

		/* Set up the help ptr and initialise the parser */
		help_ptr = (char *)ca_graphChunks[n];
		CacheLayoutGraphics();

		page_changed = 1;
		do
		{
			if (page_changed)
			{
				page_changed = 0;
				PageLayout(1);

				VL_SwapOnNextPresent();
				VL_Present();
			}

			/* Wait for the user to press a key */
			IN_ClearKeysDown();
			while (IN_GetLastScan() == IN_SC_None)
				IN_PumpEvents();

			/* Take action */
			switch (IN_GetLastScan())
			{
			case IN_SC_UpArrow:
			case IN_SC_PgUp:
			case IN_SC_LeftArrow:
				if (help_cur_page <= 1)
					break;
				BackPage(); /* This one only takes us back to the *current* page */
				BackPage();
				page_changed = 1;
				break;
			case IN_SC_RightArrow:
			case IN_SC_DownArrow:
			case IN_SC_PgDown:
				if (help_cur_page >= help_num_pages)
					break;
				page_changed = 1;
				break;
			}
		} while (IN_GetLastScan() != IN_SC_Escape);

		/* Uncache the topic we just saw */
		MM_FreePtr(&ca_graphChunks[n]);
		IN_ClearKeysDown();

		/* Return to main help menu loop */
	}
}

#endif

// KEEN WORLDS: the guide, on F1 in every world (in place of the episode's
// own help and order screens). It is written here, in the markup of the
// games' help texts: ^P begins a page, ^cX at the start of a line sets its
// colour, and a line runs on until its end and is folded to the page: about
// 31 letters across, 16 lines down.
static char kw_guideText[] =
	"^P\n"
	"^cFCOMMANDER KEEN: WORLDS\n"
	"^cBVersion " KW_VERSION " (" KW_VERSION_NAME ")\n"
	"\n"
	"^cEOne Megarocket. A galaxy that remembers.\n"
	"\n"
	"^cEKeen 4, 5 and 6 and the fan games are one galaxy here. Keen flies between them in the Bean-with-Bacon Megarocket, and what he does on a world stays done.\n"
	"\n"
	"^cBArrows turn the pages. Esc goes back.\n"
	"^P\n"
	"^cFWHAT IS NEW\n"
	"\n"
	"^cFOne galaxy\n"
	"^cEThirteen worlds, the ship and a dream, in one game with one saved game.\n"
	"\n"
	"^cFA ship to walk around in\n"
	"^cEA cabin, a flight console, a hatch and a bed. The ship's computer says where it has landed.\n"
	"\n"
	"^cFEverybody talks\n"
	"^cEEvery conversation is spoken, each character in a voice of its own. The music steps aside.\n"
	"^P\n"
	"^cFWHAT IS NEW\n"
	"\n"
	"^cFStories read aloud\n"
	"^cEThe narrator reads each world's story the first time Keen flies there.\n"
	"\n"
	"^cFWorlds that remember\n"
	"^cEA level Keen has finished can be visited again. In the Border Village on Gnosticus IV people have moved in, and there is an inn.\n"
	"\n"
	"^cFA dream\n"
	"^cEGo to bed in the ship and Keen dreams Keen Dreams, the lost episode.\n"
	"^P\n"
	"^cFWHAT IS NEW\n"
	"\n"
	"^cFA beginning and an end\n"
	"^cEA title, a tune, a story, and an ending for when every world is saved.\n"
	"\n"
	"^cFCredit where it is due\n"
	"^cEEvery world names its makers when Keen first lands there.\n"
	"\n"
	"^cFNothing to convert\n"
	"^cEA fan game is unpacked into its folder, and it is a world.\n"
	"^P\n"
	"^cFTHE SHIP\n"
	"\n"
	"^cEA new game begins inside the Megarocket.\n"
	"\n"
	"^cFThe flight console\n"
	"^cEPush up at it to choose a world. A world in grey is not installed, or not open yet: choose it to read why.\n"
	"\n"
	"^cFThe hatch\n"
	"^cEWalk to it to step out onto the world the ship stands on.\n"
	"\n"
	"^cFThe bed\n"
	"^cEPush up at it to sleep. Do it again in the dream, or press Esc, to wake.\n"
	"^P\n"
	"^cFTWO WAYS TO PLAY\n"
	"\n"
	"^cFFree Flight\n"
	"^cEEvery world is open, in any order.\n"
	"\n"
	"^cFThe Chronicle\n"
	"^cEThe worlds of the story open in its order: saving one opens the next. Worlds outside the story are open from the start.\n"
	"\n"
	"^cFTake care\n"
	"^cEKrisle is a one-way trip. The ship cannot leave until that world is saved. The game warns first.\n"
	"^P\n"
	"^cFTHE KEYS\n"
	"\n"
	"^cEArrows: move, look, use doors\n"
	"^cECtrl: jump\n"
	"^cEAlt: pogo stick\n"
	"^cESpace: shoot\n"
	"^cEEnter: what Keen has\n"
	"^cEEsc: menu (save, load, keys)\n"
	"^cEF1: this guide\n"
	"^cEF11: full screen\n"
	"\n"
	"^cEWhere Keen has a choice of words, the arrows choose.\n"
	"^P\n"
	"^cFSTORIES AND CONVERSATIONS\n"
	"\n"
	"^cEIn stories, hold Esc or your controller menu button for one second to skip. Other keys briefly reveal the hidden corner hint.\n"
	"\n"
	"^cEPopup dialogue uses a fresh tap, including Esc, even during speech. There is no hold gesture or skip hint. Voices: Configure, Options, Voices.\n"
	"\n"
	"^cERelease the button before advancing again. Moving, jumping and shooting leave story scenes playing.\n"
	"^P\n"
	"^cFTHE ARCADE\n"
	"\n"
	"^cEPress Up at the cabinet beside the bunk for original Keen 1, 2 and 3. Add your v1.31 games in arcade/keen1, keen2 and keen3.\n"
	"\n"
	"^cEArcade games keep their own saves, scores and inventory. Exit to return to the cabin. They are outside the Chronicle.\n"
	"^P\n"
	"^cFTHE WORLDS\n"
	"\n"
	"^cEKeen Worlds brings the ship and the voices. The games are yours to add: Keen 4, 5 and 6 beside the program, each fan game unpacked into its folder under mods.\n"
	"\n"
	"^cEREADME.txt, beside the program, says which files and where each game comes from.\n"
	"\n"
	"^cEA missing world shows as not installed at the flight console. Choose it to read what it is.\n"
	"^P\n"
	"^cFSTILL TO COME\n"
	"\n"
	"^cEThis is a beta. Every world can be played, but few people have played them here yet.\n"
	"\n"
	"^cFLife in saved levels\n"
	"^cEOnly the Border Village has its people so far.\n"
	"\n"
	"^cFThe arcade remake\n"
	"^cEMarooned on Mars - Galaxy is not playable yet.\n"
	"\n"
	"^cFA menu in the dream\n"
	"^cESave in the ship for now.\n"
	"^P\n"
	"^cFWHO MADE IT\n"
	"\n"
	"^cECommander Keen is by id Software.\n"
	"\n"
	"^cEEvery fan game is the work of its makers. Keen Worlds only joins them, and each says who made it when Keen lands there.\n"
	"\n"
	"^cEThe engine is Omnispeak, by David Gow and others.\n"
	"\n"
	"^cEKeen Worlds was linked together by GriNoir.\n"
	"\n"
	"^cFFly well, Commander.\n"
	"^E\n";

void KW_ShowGuide(void)
{
	int16_t oldfont = US_GetPrintFont();
	int8_t oldColour = US_GetPrintColour();
	bool changed = true;
	// (KW_AUTOGUIDE=prefix: the scripted test saves a picture of each page)
	const char *shots = getenv("KW_AUTOGUIDE");

	VL_SetScrollCoords(0, 0);
	VL_ClearScreen(4);
	US_SetPrintFont(0);
	help_ptr = kw_guideText;
	CacheLayoutGraphics();
	IN_ClearKeysDown();
	do
	{
		if (changed)
		{
			changed = false;
			PageLayout(1);
			VL_SwapOnNextPresent();
			VL_Present();
			if (shots)
			{
				void KW_DumpScreenTo(const char *path);
				char path[600];
				snprintf(path, sizeof(path), "%s%02d.ppm", shots, (int)help_cur_page);
				KW_DumpScreenTo(path);
				if (help_cur_page >= help_num_pages)
					break;
				changed = true;
				continue;
			}
		}
		IN_ClearKeysDown();
		while (IN_GetLastScan() == IN_SC_None)
			IN_PumpEvents();
		switch (IN_GetLastScan())
		{
		case IN_SC_UpArrow:
		case IN_SC_PgUp:
		case IN_SC_LeftArrow:
			if (help_cur_page <= 1)
				break;
			BackPage(); /* (to the top of this page, then of the one before) */
			BackPage();
			changed = true;
			break;
		case IN_SC_RightArrow:
		case IN_SC_DownArrow:
		case IN_SC_PgDown:
		case IN_SC_Enter:
		case IN_SC_Space:
			if (help_cur_page >= help_num_pages)
				break;
			changed = true;
			break;
		}
	} while (IN_GetLastScan() != IN_SC_Escape);
	IN_ClearKeysDown();
	US_SetPrintFont(oldfont);
	US_SetPrintColour(oldColour);
	VL_ClearScreen(4);
}

// KEEN WORLDS: show a text chunk's pages the way the ending text is shown
// (Atroxian Realm's story and dialogue screens), with a song while it runs,
// on the usual red bordered page or a plain black one.
static bool CK_StoryLayoutPage(void)
{
	ck_timedPageSkipped=false;
	ck_timedPageActive=true;
	KW_BeginPresentation(&ck_timedPageControls);
	PageLayout(0);
	ck_timedPageActive=false;
	KW_EndPresentation(&ck_timedPageControls);
	return ck_timedPageSkipped;
}

// The pages can be spoken: ck_textPagesVoice names the screen, and page n
// plays the world's clip <prefix>_<name>_<nn>.RAW (tools/kw_arealm_voice.py).
const char *ck_textPagesVoice = NULL;

void CK_ShowTextPages(int textChunk, int song, bool blackBackground)
{
	ck_textBlackBackground = blackBackground;
	VL_ClearScreen(blackBackground ? 0 : 4);
	CA_UpLevel();
	CA_SetGrPurge();
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_ARROWDIM));
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_ARROWBRIGHT));
	CA_CacheGrChunk(textChunk);
	MM_SetLock(&ca_graphChunks[textChunk], true);
	help_ptr = (char *)ca_graphChunks[textChunk];
	help_textStart = help_ptr;
	CacheLayoutGraphics();
	if (song >= 0)
		StartMusic(song);

	while (help_cur_page < help_num_pages)
	{
		if (ck_textPagesVoice && !FF_IsActive())
		{
			char clip[48];
			snprintf(clip, sizeof(clip), "%s_%02d", ck_textPagesVoice, (int)help_cur_page);
			KW_SayWorldLine(clip);
		}
		if (CK_StoryLayoutPage()) break;
		if (ck_textPagesVoice && FF_IsActive())
		{
			// (Foray in the Forest: the stretches of text this page showed,
			// one after another)
			static char names[12][48];
			const char *list[12];
			for (int u = 0; u < help_numUnits; ++u)
			{
				snprintf(names[u], sizeof(names[u]), "%s_%05d", ck_textPagesVoice, help_units[u]);
				list[u] = names[u];
			}
			KW_SayWorldLines(list, help_numUnits);
		}
		VHB_DrawBitmap(0x12A & ~3, 0xB8, CK_CHUNKNUM(PIC_ARROWDIM));
		VH_UpdateScreen();
		if (getenv("KW_AUTOPAGES"))
		{
			void KW_DumpScreenTo(const char *path);
			char path[600];
			snprintf(path, sizeof(path), "%s%02d.ppm", getenv("KW_AUTOPAGES"), (int)help_cur_page);
			KW_DumpScreenTo(path);
		}
		if (!getenv("KW_AUTOTALK"))
		{
			KW_WaitPresentation(0, true);
			if (KW_LastPresentationWasSkipped()) break;
		}
	}

	KW_LinesOver();
	help_textStart = NULL;
	MM_SetLock(&ca_graphChunks[textChunk], false);
	MM_FreePtr(&ca_graphChunks[textChunk]);
	if (song >= 0)
		StopMusic();
	CA_DownLevel();
	IN_ClearKeysDown();
	VL_ClearScreen(blackBackground ? 0 : 4);
	ck_textBlackBackground = false;
}

bool CK_UseSecretEnding(void)
{
	int level=CK_INT(CK5_SecretFuseLevel,13);
	return level>=0 && level<CK_LEVELS_TRACKED &&
		(CK_INT(CK5_SecretEndingAnyExit,0) ? ck_gameState.levelsDone[level]>=1 : ck_gameState.levelsDone[level]==LS_KorathFuse);
}

void help_endgame(void)
{
	char *ptext;
	bool secretEnding = false; // KEEN WORLDS: for the spoken pages

	/* Set up */
	VL_ClearScreen(4);
	// RF_Reset();
	CA_UpLevel();
	CA_SetGrPurge();
	// CA_SetGrPurge2();

	/* Cache the chunkss we need */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_ARROWDIM));    /* Dim arrow */
	CA_CacheGrChunk(CK_CHUNKNUM(PIC_ARROWBRIGHT)); /* Bright arrow */

	// Check for Korath Fuse
#ifdef WITH_KEEN5
	if (CK_UseSecretEnding())
	{
		CA_CacheGrChunk(CK_CHUNKNUM(TEXT_SECRETEND));
		ptext = (char *)ca_graphChunks[CK_CHUNKNUM(TEXT_SECRETEND)];
		secretEnding = true;
	}
	else
#endif
	{
		CA_CacheGrChunk(CK_CHUNKNUM(TEXT_END));
		ptext = (char *)ca_graphChunks[CK_CHUNKNUM(TEXT_END)];
	}

	/* Initialise the parser */
	help_ptr = (char *)ptext;
	CacheLayoutGraphics();

	/* Play some music */
	StartMusic(CK_INT(CK_EndingMusicLevel, 0));

	while (help_cur_page < help_num_pages)
	{
		// KEEN WORLDS: the page's lines are spoken (K4_END_<page>.RAW and so on,
		// by world: tools/kw_keen4_voice.py, kw_keen56_voice.py,
		// kw_keen789_voice.py; a page without a clip is silent)
		{
			char clip[32];
			// (a secret ending's own pages: <prefix>_ENDS_<page>.RAW)
			snprintf(clip, sizeof(clip), "ENDS_%02d", (int)help_cur_page);
			if (!(secretEnding && KW_SayWorldLine(clip)))
			{
				snprintf(clip, sizeof(clip), "END_%02d", (int)help_cur_page);
				KW_SayWorldLine(clip);
			}
		}
		/* Draw the current page (which also advances to the next page) */
		if (CK_StoryLayoutPage()) break;
		VHB_DrawBitmap(0x12A & ~3, 0xB8, CK_CHUNKNUM(PIC_ARROWDIM));
		VH_UpdateScreen();
		if (!getenv("KW_AUTOFINALE"))
		{
			KW_WaitPresentation(0, true);
			if (KW_LastPresentationWasSkipped()) break;
		}
	}

	/* Uncache our graphics and clean up */
	KW_LinesOver(); // KEEN WORLDS
	StopMusic();
#ifdef WITH_KEEN5
	if (CK_UseSecretEnding())
		MM_FreePtr(&ca_graphChunks[CK_CHUNKNUM(TEXT_SECRETEND)]);
	else
#endif
		MM_FreePtr(&ca_graphChunks[CK_CHUNKNUM(TEXT_END)]);
	MM_FreePtr(&ca_graphChunks[CK_CHUNKNUM(PIC_ARROWBRIGHT)]);
	MM_FreePtr(&ca_graphChunks[CK_CHUNKNUM(PIC_ARROWDIM)]);
	// CA_DownLevel();
	IN_ClearKeysDown();
	VL_ClearScreen(4);
	// RF_Reset();
	// CA_SetGrPurge();
}
