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

#include "id_ca.h"
#include "id_in.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_cross.h"
#include "ck_def.h"
#include "kw_music.h" // KEEN WORLDS: Keen 4's ending is spoken
#include "kw_world.h"

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

	VL_DelayTics(help_delay);
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
			VHB_DrawTile16(help_x & ~7, help_y, help_pic - ca_gfxInfoE.offTiles16);
			w = h = 16;
		}
		else if (FF_IsActive() && help_pic >= ca_gfxInfoE.offTiles16m && help_pic < ca_gfxInfoE.offTiles16m + ca_gfxInfoE.numTiles16m)
		{
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
		StartMusic(19);

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

// KEEN WORLDS: show a text chunk's pages the way the ending text is shown
// (Atroxian Realm's story and dialogue screens), with a song while it runs,
// on the usual red bordered page or a plain black one.
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
	CacheLayoutGraphics();
	if (song >= 0)
		StartMusic(song);

	bool quit = false;
	while (help_cur_page < help_num_pages && !quit)
	{
		if (ck_textPagesVoice)
		{
			char clip[48];
			snprintf(clip, sizeof(clip), "%s_%02d", ck_textPagesVoice, (int)help_cur_page);
			KW_SayWorldLine(clip);
		}
		PageLayout(0);
		IN_ClearKeysDown();
		while (true)
		{
			VHB_DrawBitmap(0x12A & ~3, 0xB8, CK_CHUNKNUM(PIC_ARROWDIM));
			VH_UpdateScreen();
			KW_LinePoll(); // the page's lines are over: the music comes back up
			// (KW_AUTOPAGES=prefix: the scripted test saves a picture of each page)
			if (getenv("KW_AUTOPAGES"))
			{
				void KW_DumpScreenTo(const char *path);
				char path[600];
				snprintf(path, sizeof(path), "%s%02d.ppm", getenv("KW_AUTOPAGES"), (int)help_cur_page);
				KW_DumpScreenTo(path);
			}
			// (KW_AUTOTALK: the scripted dialogue test turns the pages itself)
			if (IN_UserInput(70, false) || getenv("KW_AUTOTALK"))
				break;
			VHB_DrawBitmap(0x12A & ~3, 0xB8, CK_CHUNKNUM(PIC_ARROWBRIGHT));
			VH_UpdateScreen();
			if (IN_UserInput(70, false))
			{
				quit = IN_GetLastScan() == IN_SC_Escape;
				break;
			}
		}
	}

	KW_LinesOver();
	MM_SetLock(&ca_graphChunks[textChunk], false);
	MM_FreePtr(&ca_graphChunks[textChunk]);
	if (song >= 0)
		StopMusic();
	CA_DownLevel();
	IN_ClearKeysDown();
	VL_ClearScreen(blackBackground ? 0 : 4);
	ck_textBlackBackground = false;
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
	if (/*ck_currentEpisode->ep == EP_CK5 &&*/ (CK_INT(CK5_SecretEndingAnyExit, 0) ? ck_gameState.levelsDone[CK_INT(CK5_SecretFuseLevel, 13)] >= 1 : ck_gameState.levelsDone[CK_INT(CK5_SecretFuseLevel, 13)] == 0x0E)) // KEEN WORLDS: variable
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
		PageLayout(0);
		IN_ClearKeysDown();
		//VW_SetScreen( AZ : A7B4, 0 );

		// This looks like it was written with a GOTO instead of an "advancePage" variable
		bool advancePage = false;
		while (!advancePage)
		{
			/* Draw the dim arrow and wait a short time */
			VHB_DrawBitmap(0x12A & ~3, 0xB8, CK_CHUNKNUM(PIC_ARROWDIM));
			VH_UpdateScreen();
			KW_LinePoll(); // KEEN WORLDS: the line is over: the music comes back up

			if (IN_UserInput(70, false))
			{
				advancePage = true;
				break;
			}
			/* Draw the bright arrow and wait a short time */
			VHB_DrawBitmap(0x12A & ~3, 0xB8, CK_CHUNKNUM(PIC_ARROWBRIGHT));
			VH_UpdateScreen();

			if (IN_UserInput(70, false))
			{
				advancePage = true;
				break;
			}
		}
	}

	/* Uncache our graphics and clean up */
	KW_LinesOver(); // KEEN WORLDS
	StopMusic();
#ifdef WITH_KEEN5
	if (CK_INT(CK5_SecretEndingAnyExit, 0) ? ck_gameState.levelsDone[CK_INT(CK5_SecretFuseLevel, 13)] >= 1 : ck_gameState.levelsDone[CK_INT(CK5_SecretFuseLevel, 13)] == LS_KorathFuse) // KEEN WORLDS: variable
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
