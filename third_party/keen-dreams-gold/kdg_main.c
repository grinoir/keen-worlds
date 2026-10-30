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

// KDG_MAIN.C
/*
=============================================================================

							KEEN DREAMS

					An Id Software production

=============================================================================
*/


#include "kdg_def.h"
#pragma hdrstop
#include <SDL.h>
#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#define mkdir _mkdir
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

ScanCode statusscan = sc_Enter;
ScanCode attackscan = sc_Space;

char		str[80],str2[20];
boolean		singlestep,jumpcheat,godmode,storedemo,tedlevel,jumpheld,pogoheld,attackheld,jumpbutton,pogobutton,attackbutton,mortimerbedgfx,greenball;
int		extravbls; // Imported from Catacomb 3-D
word	tedlevelnum;
int		g_minTics = 2;
int		_argc;
char		**_argv;
void			ClearMemory (void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

void	DebugMemory (void);
int		DebugKeys (void);
void	ShutdownId (void);
void	Quit (char *error);
void	InitGame (void);
int	main (int argc, char **argv);
void	WallDebug (void);
void	TestSprites(void);

//===========================================================================

/*
==========================
=
= ClearMemory
=
==========================
*/
void ClearMemory (void)
{
	MM_SortMem ();
}

/*
==================
=
= CountObjects
=
==================
*/

void CountObjects(void)
{
	int activeobjects, inactiveobjects;
	objtype *ob;

	activeobjects = inactiveobjects = 0;
	for (ob=player; ob; ob=ob->next)
	{
		if (ob->active)
		{
			activeobjects++;
		}
		else
		{
			inactiveobjects++;
		}
	}
	VW_FixRefreshBuffer();
	US_CenterWindow(18, 4);
	PrintY += 7;
	US_Print("Active Objects :");
	US_PrintUnsigned(activeobjects);
	US_Print("\nInactive Objects:");
	US_PrintUnsigned(inactiveobjects);
	VW_UpdateScreen();
	IN_Ack();
}

/*
==================
=
= DebugMemory
=
==================
*/

void DebugMemory (void)
{
	VW_FixRefreshBuffer();
	if (mminfo.EMSmem == false || mminfo.XMSmem == false)
		US_CenterWindow (25,10);
	else
		US_CenterWindow (24,10);

	US_CPrint ("Memory Usage");
	US_CPrint ("------------");
	US_Print ("Total     :");
	US_PrintUnsigned((mminfo.mainmem+mminfo.EMSmem+mminfo.XMSmem)/1024);
	US_Print ("kb\nFree      :");
	US_PrintUnsigned (MM_UnusedMemory()/1024);
	US_Print ("kb\nWith purge:      ");
	US_PrintUnsigned (MM_TotalFree()/1024);
	US_Print ("kb\nStandard     :");
	US_PrintUnsigned(mminfo.mainmem/1024);
	if (mminfo.EMSmem == false)
		US_Print ("kb\nWithout expanded (EMS)     :");
	else
		US_Print ("kb\nWith expanded (EMS)     :");
	if (mminfo.EMSmem == 0)
		US_PrintUnsigned((mminfo.EMSmem == 0)/1024);
	else
		US_PrintUnsigned((mminfo.mainmem+mminfo.EMSmem)/1024);
	if (mminfo.XMSmem == false)
		US_Print ("kb\nWithout extended (XMS)     :");
	else
		US_Print ("kb\nWith extended (XMS)     :");
	if (mminfo.XMSmem == 0)
		US_PrintUnsigned((mminfo.XMSmem == 0)/1024);
	else
		US_PrintUnsigned((mminfo.mainmem+mminfo.XMSmem)/1024);
	US_Print ("kb\n");
	VW_UpdateScreen();
	IN_Ack ();
#if GRMODE == EGAGR
	MM_ShowMemory ();
#endif
}

/*
===================
=
= TestSprites
=
===================
*/

#define DISPWIDTH	110
#define	TEXTWIDTH   40
void TestSprites(void)
{
	int hx,hy,sprite,oldsprite,bottomy,topx,shift;
	spritetabletype far *spr;
	void	*block;
	unsigned	mem,scan;


	VW_FixRefreshBuffer();
	US_CenterWindow (30,17);

	US_CPrint ("Sprite Test");
	US_CPrint ("-----------");

	hy=PrintY;
	hx=(PrintX+56)&(~7);
	topx = hx+TEXTWIDTH;

	US_Print ("Chunk:\nWidth:\nHeight:\nOrgx:\nOrgy:\nXl:\nYl:\nXh:\nYh:\n"
			  "Shifts:\nMem:\n");

	bottomy = PrintY;

	sprite = STARTSPRITES;
	shift = 0;

	do
	{
		if (sprite>=STARTTILE8)
			sprite = STARTTILE8-1;
		else if (sprite<STARTSPRITES)
			sprite = STARTSPRITES;

		spr = &spritetable[sprite-STARTSPRITES];
		block = grsegs[sprite];

		VWB_Bar (hx,hy,TEXTWIDTH,bottomy-hy,WHITE);

		PrintX=hx;
		PrintY=hy;
		US_PrintUnsigned (sprite);US_Print ("\n");PrintX=hx;
		US_PrintUnsigned (spr->width);US_Print ("\n");PrintX=hx;
		US_PrintUnsigned (spr->height);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->orgx);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->orgy);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->xl);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->yl);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->xh);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->yh);US_Print ("\n");PrintX=hx;
		US_PrintSigned (spr->shifts);US_Print ("\n");PrintX=hx;
		if (!block)
		{
			US_Print ("-----");
		}
		else
		{
			mem = spr->width*8*spr->height;	// one byte per pixel, no shifts
			US_PrintUnsigned (mem);
		}

		oldsprite = sprite;
		do
		{
		//
		// draw the current shift, then wait for key
		//
			VWB_Bar(topx,hy,DISPWIDTH,bottomy-hy,WHITE);
			if (block)
			{
				PrintX = topx;
				PrintY = hy;
				US_Print ("Shift:");
				US_PrintUnsigned (shift);
				US_Print ("\n");
				VWB_DrawSprite (topx+16+shift*2,PrintY,sprite);
			}

			VW_UpdateScreen();

			scan = IN_WaitForKey ();

			switch (scan)
			{
			case sc_UpArrow:
				sprite++;
				break;
			case sc_DownArrow:
				sprite--;
				break;
			case sc_LeftArrow:
				if (--shift == -1)
					shift = 3;
				break;
			case sc_RightArrow:
				if (++shift == 4)
					shift = 0;
				break;
			case sc_Escape:
				return;
			}

		} while (sprite == oldsprite);

  } while (1);


}

/*
===================
=
= MaskOnTile
=
= Taken from FITF
=
===================
*/

void MaskOnTile(unsigned dest, unsigned source)
{
	unsigned _seg *sourceseg;
	unsigned _seg *destseg;
	unsigned sourceval, sourcemask, maskindex;
	int i;

//	CA_CacheGrChunk(STARTTILE16M+source);	// caching and uncaching in here requires less memory but takes longer

	sourceseg = grsegs[STARTTILE16M+source];
	destseg = grsegs[STARTTILE16M+dest];
	for (i=0; i<64; i++)
	{
		maskindex = i & 15;
		sourceval = sourceseg[16+i];
		sourcemask = sourceseg[maskindex];
		destseg[maskindex] &= sourcemask;
		destseg[16+i] &= sourcemask;
		destseg[16+i] |= sourceval;
	}

//	CA_UncacheGrChunk(STARTTILE16M+source);
}

/*
===================
=
= WallDebug
=
= Taken from FITF
=
===================
*/
//const char far WALLDEBUGERROREAST[] = "WallDebug: East wall other than 1:";
//const char far WALLDEBUGERRORWEST[] = "WallDebug: West wall other than 1:";


void WallDebug(void)
{
#define WALLTILESTART 72
#define NUMWALLTILES  89	// 7 tops + 7 bottoms + 2 right + 2 left
#define WALLTILEEND   (WALLTILESTART+NUMWALLTILES)
	int i, val;

	VW_FixRefreshBuffer();
	US_CenterWindow(24, 3);
	US_PrintCentered("WORKING");
	VW_UpdateScreen();
	// cache the collision icons:
	for (i=STARTTILE16M+WALLTILESTART; i<STARTTILE16M+WALLTILEEND; i++)
	{
		CA_CacheGrChunk(i);
	}

	// apply the collision icons to all tile graphics in the cache
	for (i=0; i<NUMTILE16M; i++)
	{
		if (!grsegs[STARTTILE16M+i])
		{
			continue;
		}
		val = tinf[i+NORTHWALL] & 7;
		if (val)
		{
			MaskOnTile(i, val+WALLTILESTART-1);
		}
		val = tinf[i+SOUTHWALL] & 7;
		if (val)
		{
			MaskOnTile(i, val+WALLTILESTART+7-1);
		}
		val = tinf[i+EASTWALL];
		if (val > 2)
		{
			/*
			//strcpy(str, "WallDebug: East wall > 2:");
			SM_CopyString(str, S_WALLDEBUGEASTWALL);
			itoa(i, str2, 10);
			strcat(str, str2);
			Quit(str);
			*/
			val = 2;
		}
		if (val)
		{
			MaskOnTile(i, val+WALLTILESTART+14-1);
		}
		val = tinf[i+WESTWALL];
		if (val > 2)
		{
			/*
			//strcpy(str, "WallDebug: West wall > 2:");
			SM_CopyString(str, S_WALLDEBUGWESTWALL)
			itoa(i, str2, 10);
			strcat(str, str2);
			Quit(str);
			*/
			val = 2;
		}
		if (val)
		{
			MaskOnTile(i, val+WALLTILESTART+16-1);
		}
	}
	// uncache the collision icons again (they take up 2880 bytes of memory)
	for (i=STARTTILE16M+WALLTILESTART; i<STARTTILE16M+WALLTILEEND; i++)
	{
		CA_UncacheGrChunk(i);
	}
#undef WALLTILESTART
#undef WALLTILEEND
#undef NUMWALLTILES
}

/*
================
=
= DebugKeys
=
================
*/
int DebugKeys (void)
{
	boolean esc;
	int i;
	char *endptr;
	unsigned long value;
	unsigned int keenwarpfirst, keenwarplast, keenwarpskip=24, mortwarpfirst, mortwarplast;
#if 0
	// Note (ktx58): please sceck tthis, these vars are unused
	statetype s_worldkeensleep1;
	statetype s_keendie1;
#endif

	// NOTE: Following the update to the ID Engine from Catacomb 3-D,
	// some cheat codes have further been imported from the same game,
	// while some additional code has been restored separately.
	// Furthermore, scancode definitions from ID_IN.H are used
	// used now instead of scancode values hardcoded here.

	if (Keyboard[sc_Z] && mapon!=0)	// Z = suicide
	{
		if (gamestate.difficulty == gd_Keen || gamestate.difficulty == gd_Grand)
			gamestate.lives--;			// Szemigi: Decrease the health point instead of taking all lives.
		else
			gamestate.lives = 0;
			KillKeen();
	}

	if (Keyboard[sc_Y])			// Y = game over
	{
			WallDebug();
			return true;
	}

	if (Keyboard[sc_T])			// T = sprite test
	{
		TestSprites();
		return 1;
	}

	if (Keyboard[sc_B] && ingame)		// B = border color
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(24, 3);
		PrintY += 6;
		US_Print(" Border color (0-15):");
		VW_UpdateScreen();
		esc = !US_LineInput(px, py, str, NULL, true, 2, 0);
		if (!esc)
		{
			value = strtoul(str, &endptr, 10);
			if (endptr != str && *endptr == 0 && value <= 15)
			{
				VW_ColorBorder(value);
			}
		}
		return true;
	}

	if (Keyboard[sc_C])		// C = count objects
	{
		CountObjects();
		return 1;
	}

	if (Keyboard[sc_D] && ingame)		// D = start / end demo record
	{
		if (DemoMode == demo_Off)
		{
			StartDemoRecord();
		}
		else if (DemoMode == demo_Record)
		{
			EndDemoRecord();
			playstate = levelcomplete;
		}
		return true;
	}

	if (Keyboard[sc_E] && ingame)	// DEBUG: end + 'E' to quit level
	{
		if (mapon == 46)
		{
			playstate = endgame;
		}
		else
		{
			playstate = levelcomplete;
		}
	}

	if (Keyboard[sc_G] && ingame)		// G = god mode
	{
		VW_FixRefreshBuffer();
		US_CenterWindow (12,2);
		if (godmode)
		  	US_PrintCentered ("God mode OFF");
		else
		  	US_PrintCentered ("God mode ON");
		VW_UpdateScreen();
		IN_Ack();
		godmode ^= 1;
		return 1;
	}
	else if (Keyboard[sc_I])			// I = item cheat
	{
		VW_FixRefreshBuffer();
		US_CenterWindow (12,3);
		US_PrintCentered ("Free items!");
		if (mapon == 18)
			gamestate.keycard=1;
		else
			gamestate.keys=99;
		GivePoints(3000);
		if (mapon >= 0 && mapon <= 22)
			gamestate.boobusbombs=99;
		if (mapon >= 23 && mapon <= 46)
			gamestate.ammo=99;
		else
			gamestate.flowerpowers=99;					
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
	else if (Keyboard[sc_J])			// J = jump cheat
	{
		jumpcheat^=1;
		VW_FixRefreshBuffer();
		US_CenterWindow (18,3);
		if (jumpcheat)
			US_PrintCentered ("Jump cheat ON");
		else
			US_PrintCentered ("Jump cheat OFF");
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
	else if (Keyboard[sc_M])			// M = memory info
	{
		DebugMemory();
		return 1;
	}
	else if (Keyboard[sc_N])			// N = No clipping
	{
		VW_FixRefreshBuffer();
		US_CenterWindow (18,3);
		if (player->needtoclip)
		{
			US_PrintCentered ("No clipping ON");
			player->needtoclip = false;
		}
		else
		{
			US_PrintCentered ("No clipping OFF");
			player->needtoclip = true;
		}
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
	else if (Keyboard[sc_P])			// P = pause with no screen disruptioon
	{
		IN_Ack();
		return 1;
	}
	else if (Keyboard[sc_S] && ingame)	// S = slow motion
	{
		singlestep^=1;
		VW_FixRefreshBuffer();
		US_CenterWindow (18,3);
		if (singlestep)
			US_PrintCentered ("Slow motion ON");
		else
			US_PrintCentered ("Slow motion OFF");
		VW_UpdateScreen();
		IN_Ack ();
		return 1;
	}
	else if (Keyboard[sc_V])			// V = extra VBLs
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(30,3);
		PrintY+=6;
		US_Print("  Add how many extra VBLs(0-8):");
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			value = strtoul(str, &endptr, 10);
			if (endptr != str && *endptr == 0 && value <= 8)
			{
				extravbls = value;
			}
		}
		return 1;
	}
	else if (Keyboard[sc_W] && ingame)	// W = warp to level
	{
		VW_FixRefreshBuffer();
		US_CenterWindow(26,3);
		PrintY+=6;
		if (gamestate.mapon <= 22)
		{
			if (gamestate.difficulty == gd_Grand)
			{
				mortwarpfirst = 0;
				mortwarplast = 21;
			}
			else
			{
				keenwarpfirst = 0;
				keenwarplast = 22;				
			}
		}
		else
		{
			if (gamestate.difficulty == gd_Grand)
			{
				mortwarpfirst = 24;
				mortwarplast = 46;
			}
			else
			{
				keenwarpfirst = 23;
				keenwarplast = 46;				
			}
		}
		strcpy(str, "  Warp to which level (");
		if (gamestate.difficulty == gd_Grand)
		{
			itoa(mortwarpfirst, str2, 10);
		}
		else
		{
			itoa(keenwarpfirst, str2, 10);
		}
		strcat(str, str2);
		strcat(str, "-");
		if (gamestate.difficulty == gd_Grand)
		{
			itoa(mortwarplast, str2, 10);
		}
		else
		{
			itoa(keenwarplast, str2, 10);
		}
		strcat(str, str2);
		strcat(str, "):");
		US_Print(str);
		
		VW_UpdateScreen();
		esc = !US_LineInput (px,py,str,NULL,true,2,0);
		if (!esc)
		{
			value = strtoul(str, &endptr, 10);
			if (gamestate.difficulty == gd_Grand)
			{
				if (endptr != str && *endptr == 0 && value >= mortwarpfirst && value <= mortwarplast)
				{
					gamestate.mapon = (byte)value;
					playstate = warptolevel;
				}
			}
			else
			{
				if (endptr != str && *endptr == 0 && value >= keenwarpfirst && value <= keenwarplast)
				{
					gamestate.mapon = (byte)value;
					playstate = warptolevel;
				}
			}
			if (endptr != str && *endptr == 0 && value >= keenwarpfirst && value <= keenwarplast)
			{
				if (endptr != str && *endptr == 0 && value >= keenwarpskip && value <= keenwarpskip)
				{
					gamestate.mapon = (byte)value;
					playstate = false;
				}
			}
		}
		return 1;
	}
	return 0;
}

//===========================================================================

/*
==========================
=
= ShutdownId
=
= Shuts down all ID_?? managers
=
==========================
*/

void ShutdownId (void)
{
#ifdef DEMO_DEBUG
  MM_FreePtr(&demodebugseg);
#endif
  US_Shutdown ();
  SD_Shutdown ();
  IN_Shutdown ();
  RF_Shutdown ();
  VW_Shutdown ();
  CA_Shutdown ();
  MM_Shutdown ();
}

//===========================================================================

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{
	ShutdownId();
	if (error && *error)
	{
		// Keen Worlds shows this, and the log keeps it
		fprintf(stderr, "Keen Dreams Gold: %s\n", error);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Keen Dreams Gold Edition", error, NULL);
		exit(1);
	}
	exit(0);
}


//===========================================================================

/*
==================
=
= CheckMemory
=
==================
*/

void CheckMemory(void)
{
	int finscreen;

	if (mminfo.nearheap+mminfo.farheap+mminfo.EMSmem+mminfo.XMSmem >= MINMEMORY)
		return;

	Quit ("Not enough memory");
}


//===========================================================================

/*
==========================
=
= InitGame
=
= Load a few things right away
=
==========================
*/


void InitGame (void)
{
	void MML_UseSpace (int segstart, int seglength);

	int segstart,seglength;
	int i;

	// The DOS text-mode intro screen (INTRO.SCN) is not shown natively.

	US_TextScreen();

	MM_Startup ();
	VW_Startup ();
	RF_Startup ();
	IN_Startup ();
	SD_Startup ();
	US_Startup ();

	US_UpdateTextScreen();

	CA_Startup ();
	US_Setup ();

//
// load in and lock down some basic chunks
//

	CA_ClearMarks ();

	CA_MarkGrChunk(STARTFONT);
	CA_MarkGrChunk(STARTFONTM);
	CA_MarkGrChunk(STARTTILE8);
	CA_MarkGrChunk(STARTTILE8M);
	CA_MarkGrChunk(STARTFONT + 1);
	
	// Note (ktx58): To save memory, try to load only
	// the cursor icon sprite here
#if 0
	for (i=KEEN_LUMP_START;i<=KEEN_LUMP_END;i++)
		CA_MarkGrChunk(i);
#endif
	CA_CacheMarks (NULL, 0);

	MM_SetLock (&grsegs[STARTFONT],true);
	MM_SetLock (&grsegs[STARTFONTM],true);
	MM_SetLock (&grsegs[STARTTILE8],true);
	MM_SetLock (&grsegs[STARTTILE8M],true);
	MM_SetLock (&grsegs[STARTFONT + 1],true);

#if 0
	for (i=KEEN_LUMP_START;i<=KEEN_LUMP_END;i++)
		MM_SetLock (&grsegs[i],true);
#endif
	CA_LoadAllSounds ();

#ifdef DEMO_DEBUG
    MM_GetPtr(&demodebugseg, DEMO_DEBUG_BUF_SIZE);
    MM_SetLock(&demodebugseg, true);
#endif
    
	fontcolor = WHITE;

	US_FinishTextScreen();

	VW_SetScreenMode (GRMODE);
	VW_ClearVideo (BLACK);
}


//===========================================================================

/*
==========================
=
= main
=
==========================
*/

int main (int argc, char **argv)
{
	static char	*args[64];
	int	i, n = 0;

	SDL_SetMainReady();

	// Keen Worlds starts the companion as:
	//   keen-dreams-gold DATADIR SAVEDIR [-fullscreen=true|false] [game parameters]
	// The data are read from DATADIR; saves and KDGOLD.CFG go to SAVEDIR.
	if (argc < 3)
	{
		fprintf(stderr, "usage: %s DATADIR SAVEDIR [-fullscreen=true|false]\n", argv[0]);
		return 2;
	}
	snprintf(ca_datapath, 1024, "%s", argv[1]);
	mkdir(argv[2]
#ifndef _WIN32
		, 0755
#endif
		);	// (fails harmlessly if it is there)
	if (chdir(argv[2]))
	{
		fprintf(stderr, "Keen Dreams Gold: can't use the save folder %s\n", argv[2]);
		return 2;
	}
	args[n++] = argv[0];
	for (i = 3; i < argc && n < 63; i++)
	{
		if (!strcmp(argv[i], "-fullscreen=false"))
			args[n++] = "/WINDOWED";
		else if (strcmp(argv[i], "-fullscreen=true"))
			args[n++] = argv[i];
	}
	args[n] = NULL;
	_argc = n;
	_argv = args;

	InitGame();
	CheckMemory();

	DemoLoop();					// DemoLoop calls Quit when everything is done
	Quit("Demo loop exited???");
	return 0;
}

                        
