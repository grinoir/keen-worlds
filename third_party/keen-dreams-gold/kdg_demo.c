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

// KDG_DEMO.C

#include "kdg_def.h"

#pragma	hdrstop

#define RLETAG	0xABCD

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

boolean scorescreenkludge;

char far NOMEMTOLOADGAME[] =
  "Not enough memory\n"
  "to load game!\n";

char far INSMEMFORBGMUSIC[] =
  "Insufficient memory\n"
  "for background music!\n";

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

//===========================================================================

/*
============================
=
= RunDemo
=
============================
*/

void RunDemo(int num)
{
  	int basenum;
  	int far *demodata;

  	NewGame();
  	num += DEMO0;
  	CA_CacheGrChunk(num);
  	demodata = grsegs[num];
  	gamestate.mapon = demodata[0];
  	DemoSize = demodata[1];
  	MM_GetPtr((memptr *)&DemoBuffer, DemoSize);
	MM_SetLock(&grsegs[num], true);
	_fmemcpy(DemoBuffer, ((char _seg *)grsegs[num])+4, DemoSize);
  	MM_FreePtr(&grsegs[num]);

  	IN_StartDemoPlayback(DemoBuffer, DemoSize);

  	SetupGameLevel(true);
#ifdef DEMO_DEBUG
  	StartDemoDataCollection('P');
#endif
	// K1n9_Duk3 fix: don't try to play the demo if we ran out of memory
	if (mmerror)
	{
		mmerror = false;
	}
	else
	{	
		PlayLoop();
	}

  	IN_StopDemo();

	MM_FreePtr((memptr *)&DemoBuffer);
	VW_FixRefreshBuffer();
	CA_ClearMarks();

#if 0
  	// Note (ktx58): Removed BioMenace specific (?) code
  	if (basenum != 1 && basenum != 4)
  	{
  	  CheckLastScan();
  	}

  	if (basenum == 4)
 	{
    		memcpy(&gamestate, &statecopy, sizeof(gamestate));
  	}
#endif
}


//===========================================================================

#if 0
/*
============================
=
= CheckLastScan
=
============================
*/

void CheckLastScan(void)
{
	if (LastScan)
	{
		if (storedemo)
		{
			playstate = resetgame;
			restartgame = gd_Normal;
			IN_ClearKeysDown();
			NewGame();
		}
		else
		{
			US_ControlPanel();
			if (restartgame)
			{
				playstate = resetgame;
			}
			else if (loadedgame)
			{
				playstate = loadedgame;
			}
			DemoLoop();
		}
	}
}

#endif

//===========================================================================

/*
=====================
=
= NewGame
=
= Set up new game to start from the beginning
=
=====================
*/

void NewGame (void)
{
	word	i;
	objtype *ob;

	gamestate.worldx = 0;		// spawn keen at starting spot

	gamestate.mapon = restartgame == gd_Grand ? 21 : 22;
	if (nightmode == true)
		gamestate.mapon = restartgame == gd_Grand ? 24 : 23;
	gamestate.score = 0;
	gamestate.nextextra = 20000;
	// Note (ktx58): Health on hard level is 99 instead of 100 
	// due to scorebox limitations.
	gamestate.lives = (restartgame == gd_Keen || restartgame == gd_Grand) ? 99 : 3;
	gamestate.flowerpowers = gamestate.boobusbombs = 0;
	gamestate.damagetime = 0;
	gamestate.keys = 0;
	gamestate.keycard = 0;	
	gamestate.ammo = 0;
	for (i = 0;i < GAMELEVELSWITHOUTTITLE; i++)
		gamestate.leveldone[i] = false;
}

//===========================================================================

/*
=====================
=
= WaitOrKey
=
=====================
*/

int WaitOrKey (int vbls)
{
	while (vbls--)
	{
		IN_ReadControl(0,&c);		// get player input
		if (LastScan || c.button0 || c.button1)
		{
			IN_ClearKeysDown ();
			return 1;
		}
		VW_WaitVBL(1);
	}
	return 0;
}


//===========================================================================

/*
=====================
=
= GameOver
=
=====================
*/

void
GameOver (void)
{
	VW_FixRefreshBuffer ();
	US_CenterWindow (16,3);

	US_PrintCentered("Game Over!");

	VW_UpdateScreen ();
	IN_ClearKeysDown ();
	IN_Ack ();

	VW_FadeOut ();
	RF_FixOfs();
	VW_ClearVideo(BLACK);

	RF_FixOfs();

	VW_UpdateScreen ();
	
}

//===========================================================================

/*
==================
=
= StatusWindow
=
==================
*/

void StatusWindow (void)
{
	_fstrcpy(str, levelnames[mapon]);

	SD_PlaySound (STATUSONSND);
	VW_FixRefreshBuffer ();
	if (mapon != 18)
		US_CenterWindow(27,12);
	else
		US_CenterWindow(27,14);
	if (mapon >= 23 && mapon <= 46)
		fontcolor = 6;		// light blue text
	else
		fontcolor = F_SECONDCOLOR;	// light red text
	US_CPrint("LOCATION");
	fontcolor = F_BLACK;
	PrintY += -1;
	US_CPrint(str);

	US_Print("\n");
	PrintX += 58;
	PrintY += 28;
	if (mapon >= 23 && mapon <= 46)
		if (DemoMode)
			fontcolor = 13;
		else
			fontcolor = 11;
	else
		if (DemoMode)
			fontcolor = 13;
		else
			fontcolor = F_FIRSTCOLOR;
	US_Print("Score");
	fontcolor = F_BLACK;
	PrintX += 12;
	PrintY += 0;
	US_PrintUnsigned(gamestate.score);	// K1n9_Duk3's fix
	US_Print("\n");

	WindowX += 8;
	WindowW -= 8;
	WindowY += 20;
	WindowH -= 20;
	PrintX = WindowX;
	PrintY = WindowY;

	US_Print("\n");
	if (gamestate.difficulty == gd_Grand)
	{
		VWB_DrawTile8(PrintX,PrintY,56);
		VWB_DrawTile8(PrintX + 8,PrintY,57);
	}
	else
	{
		VWB_DrawTile8(PrintX,PrintY,26);
		VWB_DrawTile8(PrintX + 8,PrintY,27);
	}
	
	PrintX += 24;
	US_PrintSigned(gamestate.lives);
	US_Print("\n");

	if (mapon == 21 && gamestate.difficulty != gd_Grand)
	{
		VWB_DrawTile8(PrintX,PrintY + 4,56);
		VWB_DrawTile8(PrintX + 8,PrintY + 4,57);
	}
	else
	{
		VWB_DrawTile8(PrintX,PrintY,32);
		VWB_DrawTile8(PrintX + 8,PrintY,33);
		VWB_DrawTile8(PrintX,PrintY + 8,34);
		VWB_DrawTile8(PrintX + 8,PrintY + 8,35);
	}
	PrintX += 24;
	PrintY += 3;
	fontcolor = F_BLACK;
	if (mapon >= 23 && mapon <= 46)
	{
		if (gamestate.boobusbombs == 48)
		{
			fontcolor = 13;
		}
	}
	else if (mapon != 20)
	{
		if (gamestate.boobusbombs == 48)
		{
			fontcolor = 13;
		}
	}
	if (mapon == 21 && gamestate.difficulty != gd_Grand)
	{
		US_PrintUnsigned(mortinlevel->temp4);		// Szemigi: Boobus Bomb counter has no significance in Mountain Top, so let's count Mortimer's health.
	}
	else
	{
		US_PrintUnsigned(gamestate.boobusbombs);
	}
	US_Print("\n");

	WindowX += 50;
	WindowW -= 50;
	PrintX = WindowX;
	PrintY = WindowY;

	US_Print("\n");
	if (mapon >= 23 && mapon <= 46)
		if (DemoMode)
			fontcolor = 13;
		else
			fontcolor = 11;
	else
		if (DemoMode)
			fontcolor = 13;
		else
			fontcolor = F_FIRSTCOLOR;
	if (gamestate.difficulty == gd_Grand)
		US_Print("Extra Mort At");
	else
		US_Print("Extra Keen At");
	fontcolor = F_BLACK;
#if 0
	x = PrintX;
#endif
	PrintX += 14;
	US_PrintSigned(gamestate.nextextra);
	US_Print("\n");

	PrintX += -40;
	if (mapon >= 23 && mapon <= 46)
		if (gamestate.difficulty == gd_Grand)
			VWB_DrawTile8(PrintX - 1,PrintY + 17,49);
		else
			VWB_DrawTile8(PrintX - 1,PrintY + 17,65);
	else
		VWB_DrawTile8(PrintX - 1,PrintY + 17,36);		
	PrintX += 14;
	PrintY += 17;
	if (mapon >= 23 && mapon <= 46)
		US_PrintUnsigned(gamestate.ammo);
	else
		US_PrintUnsigned(gamestate.flowerpowers);
	US_Print("\n");

	PrintX += 0;
	PrintY += -24;
	if (mapon >= 23 && mapon <= 46)
		if (DemoMode)
			fontcolor = 13;
		else
			fontcolor = 11;
	else
		if (DemoMode)
			fontcolor = 13;
		else
			fontcolor = F_FIRSTCOLOR;
	US_Print("Difficulty");
	fontcolor = F_BLACK;
	PrintX += 4;
	switch (gamestate.difficulty)
	{
	case 1:
		VWB_DrawTile8(PrintX + 8,PrintY + 0,37);
		break;
	case 2:
		VWB_DrawTile8(PrintX + 8,PrintY + 0,38);
		break;
	case 3:
		VWB_DrawTile8(PrintX + 8,PrintY + 0,39);
		break;
	case 4:
		VWB_DrawTile8(PrintX + 8,PrintY + 0,40);
		break;
	case 5:
		VWB_DrawTile8(PrintX + 8,PrintY + 0,75);
		break;
	}

	PrintX = 16;
	if (mapon != 18)
	{
		VWB_DrawTile8(PrintX + 202,PrintY - 0,24);
		VWB_DrawTile8(PrintX + 212,PrintY - 0,25);
	}
	else
	{
		VWB_DrawTile8(PrintX + 46,PrintY + 30,108);
		VWB_DrawTile8(PrintX + 54,PrintY + 30,109);
		VWB_DrawTile8(PrintX + 46,PrintY + 38,110);
		VWB_DrawTile8(PrintX + 54,PrintY + 38,111);
	}
	if (mapon != 18)
	{
		PrintX += 224;
		PrintY += 0;
	}
	else
	{
		PrintX += 64;
		PrintY += 32;
	}
	if (mapon == 18)
		US_PrintUnsigned(gamestate.keycard);
	else
		US_PrintUnsigned(gamestate.keys);
	US_Print("\n");

	US_Print("\n");

	US_Print("\n");

	PrintX += 24;
	PrintY += 4;		
	US_Print("\n");

	if (mapon >= 23 && mapon <= 46)
	{
		if (DemoMode)
			CA_MarkGrChunk(PRESSAKEYGREENPIC);
		else
			CA_MarkGrChunk(PRESSAKEYREDPIC);
	}
	else if (DemoMode)
	{
		CA_MarkGrChunk(PRESSAKEYGREENPIC);
	}
	else
	{
		CA_MarkGrChunk(PRESSAKEYBLUEPIC);
	}
	CA_CacheMarks(NULL, 0);
	if (mapon >= 23 && mapon <= 46)
	{
		if (DemoMode)
			VWB_DrawPic(120, 126, PRESSAKEYGREENPIC);
		else
			VWB_DrawPic(120, 126, PRESSAKEYREDPIC);
	}
	else if (DemoMode)
	{
		VWB_DrawPic(120, 126, PRESSAKEYGREENPIC);
	}
	else
	{
		if (mapon != 18)
			VWB_DrawPic(120, 126, PRESSAKEYBLUEPIC);
		else
			VWB_DrawPic(120, 134, PRESSAKEYBLUEPIC);
	}
	VW_UpdateScreen ();
	IN_Ack();
	RF_ForceRefresh ();
	SD_PlaySound (STATUSOFFSND);
}

boolean
SaveTheGame(int file)
{
	word	i,size,compressed,expanded;
	objtype	*o;
	int	save1,save2;
	memptr	bigbuffer;
	boolean	writeresult;

	if (!CA_FarWrite(file,(byte far *)&gamestate,sizeof(gamestate)))
		return(false);

	expanded = mapwidth * mapheight * 2;
	MM_GetPtr (&bigbuffer,expanded);

	for (i = 0;i < 3;i++)	// Write all three planes of the map
	{
//
// leave a word at start of compressed data for compressed length
//
		compressed = CA_RLEWCompress ((uint16_t *)mapsegs[i]
			,expanded,((uint16_t *)bigbuffer)+1,RLETAG);

		*(uint16_t *)bigbuffer = compressed;

		if (!CA_FarWrite(file,(byte far *)bigbuffer,compressed+2) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}
	}

	// Clear "temp1" of player if there was a "zee" object in the level
	save1 = (int)GetFirstObjectWithClass(zeeobj);

	if (save1)
	{
		if (player->temp1 == (int)save1) 
			player->temp1 = 0; else save1 = 0;
	}
 
	for (o = player;o;o = o->next)
	{
		if (o->obclass == pooffromobj)
		{
			save2 = o->temp1;
			o->temp1 = (int)IndexOfObjectInLinkedList((objtype *)(o->temp1));
		}

		writeresult = CA_FarWrite(file,(byte far *)o,sizeof(objtype));

		if (o->obclass == pooffromobj)
			o->temp1 = save2;

		if (!writeresult)
		{
			if (save1)
				player->temp1 = save1;

			MM_FreePtr (&bigbuffer);
			return(false);
		}
	}

	if (save1)
		player->temp1 = save1;


	MM_FreePtr (&bigbuffer);
	return(true);
}


boolean
LoadTheGame(int file)
{
	word	i,j,size;
	objtype	*o;
	int orgx,orgy;
	objtype		*prev,*next,*followed;
	unsigned	compressed,expanded;
	memptr	bigbuffer;

	if (!CA_FarRead(file,(byte far *)&gamestate,sizeof(gamestate)))
		return(false);

// drop down a cache level and mark everything, so when the option screen
// is exited it will be cached

	ca_levelbit >>= 1;
	ca_levelnum--;
	SetupGameLevel(false);
	if (mmerror)
	{
		mmerror = false;
		VW_FixRefreshBuffer ();
		US_CenterWindow(26, 8);
		PrintY += 20;
		US_CPrintFar(NOMEMTOLOADGAME);
		VW_UpdateScreen();
		IN_Ack();
		return false;
	}
	ca_levelbit <<= 1;
	ca_levelnum++;

	expanded = mapwidth * mapheight * 2;
	MM_BombOnError(false);	// This should use false to avoid an instant crash
	MM_GetPtr(&bigbuffer, expanded);
	MM_BombOnError(true);	// This should use true to force an instant crash
	if (mmerror)
	{
		mmerror = false;
		VW_FixRefreshBuffer ();
		US_CenterWindow(26, 8);
		PrintY += 20;
		US_CPrintFar(NOMEMTOLOADGAME);
		VW_UpdateScreen();
		IN_Ack();
		return false;
	}
	for (i = 0;i < 3;i++)	// Read all three planes of the map
	{
		if (!CA_FarRead(file,(byte far *)&compressed,sizeof(compressed)) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

		if (!CA_FarRead(file,(byte far *)bigbuffer,compressed) )
		{
			MM_FreePtr (&bigbuffer);
			return(false);
		}

		CA_RLEWexpand ((uint16_t *)bigbuffer,
			(uint16_t *)mapsegs[i],compressed,RLETAG);
	}

	MM_FreePtr (&bigbuffer);

	// Read the object list back in - assumes at least one object in list

	InitObjArray ();
	new = player;
	prev = new->prev;
	next = new->next;
	if (!CA_FarRead(file,(byte far *)new,sizeof(objtype)))
		return(false);
	new->prev = prev;
	new->next = next;
	new->needtoreact = true;
	new->sprite = NULL;
	new = scoreobj;
	while (true)
	{
		prev = new->prev;
		next = new->next;
		if (!CA_FarRead(file,(byte far *)new,sizeof(objtype)))
			return(false);
		followed = new->next;
		new->prev = prev;
		new->next = next;
		new->needtoreact = true;
		new->sprite = NULL;

		// Set mortinlevel variable if necessary when loading a saved game
		if (new->obclass == mortobj)
			 mortinlevel = new;

		if (followed)
			GetNewObj (false);
		else
			break;
	}

	// -- Fix pointers in temp1 for "pooffromobj" and "zeeobj" --
	// Note that this has to be done after all objects have been loaded
	// into the object array as the flower object may be after the
	// pooffromobj in the linked list.

	for (o = player;o;o = o->next)
	{
		if (o->obclass == pooffromobj)
		{
			o->temp1 = (int)ObjectAtIndexInLinkedList(o->temp1);
		}
		else
		{
			if (o->obclass == zeeobj)
				player->temp1 = (int)o;
		}
	}

	*((long *)&(scoreobj->temp1)) = -1;		// force score to be updated
	scoreobj->temp3 = -1;			// and flower power
	scoreobj->temp4 = -1;			// and lives

	return(true);
}

void
ResetGame(void)
{
	NewGame ();

	ca_levelnum--;
	CA_ClearMarks();
#if 0
	titleptr[ca_levelnum] = NULL;		// don't reload old level
#endif
	ca_levelnum++;
}

 boolean
MoveTitleTo(int offset)
{
	boolean		done;
	int			dir,
				chunk,
				move;
	longword	lasttime,delay;

	if (offset < originxglobal)
		dir = -1;
	else
		dir = +1;

	chunk = dir * PIXGLOBAL;

	done = false;
	delay = 1;
	while (!done)
	{
		lasttime = TimeCount;
		move = delay * chunk;
		if (chunk < 0)
			done = originxglobal + move <= offset;
		else
			done = originxglobal + move >= offset;
		if (!done)
		{
			RF_Scroll(move,0);
			RF_Refresh();
		}
		if (IN_IsUserInput())
			return(true);
		delay = TimeCount - lasttime;
	}
	if (originxglobal != offset)
	{
		RF_Scroll(offset - originxglobal,0);
		RF_Refresh();
	}
	return(false);
}

 boolean
Wait(longword time)
{
	time += TimeCount;
	while ((TimeCount < time) && (!IN_IsUserInput()))
	{
		if (!(TimeCount % MINTICS))
			RF_Refresh();
	}
	return(IN_IsUserInput());
}

 boolean
ShowText(int offset,WindowRec *wr,char far *s)
{
	// note (ktx58): Argument "s" ix now a far pointer
	if (MoveTitleTo(offset))
		return(true);

	US_RestoreWindow(wr);
	US_CPrintFar(s);
	VW_UpdateScreen ();

	if (Wait(TickBase * 5))
		return(true);

	US_RestoreWindow(wr);
	US_CPrintFar(s);
	VW_UpdateScreen ();
	return(false);
}

//=============================================================================================================================

/*** StopMusic was originally imported and adapted from Catacomb 3-D (C3_PLAY.C) through NY00123's Keen Dreams Tech Demo ***/

//=============================================================================================================================

/*
=================
=
= StopMusic
=
=================
*/

// Note (ktx58): Enable music memory management fixes
#define FIX_MUSIC_MEMORY_ISSUES

void StopMusic(void)
{
	int	i;

	SD_MusicOff();
	for (i = 0; i<LASTMUSIC;i++)
	{
		if (audiosegs[STARTMUSIC+i])
		{
#ifdef FIX_MUSIC_MEMORY_ISSUES
			//unlock any music blocks so that they can be purged
			MM_SetLock((memptr *)&audiosegs[STARTMUSIC+i], false);
#endif
			MM_SetPurge((memptr *)&audiosegs[STARTMUSIC+i], PURGE_FIRST);
		}
	}
}


//============================================================================================

/*** StartMusic was imported and adapted from the reconstructed Keen 4-6 source ***/

//============================================================================================


/*
=================
=
= StartMusic
=
=================
*/

void StartMusic(int num)
{
	static int songs[] =
	{
	 	VEGEXPL_MUS,		// Map Level: Land of Tuberia
	 	SMOOTHIE_MUS,		// Level 1: Horseradish Hill
	 	DARKHERE_MUS,		// Level 2: Melon Mines
	 	ROWSAD_MUS,		// Level 3: Bridge Bottoms
	 	ROCKIN_MUS,		// Level 4: Rhubarb Rapids
	 	WAY_MUS,		// Level 5: Parsnip Pass
	 	CABBAGE_MUS,		// Level 6: Celery Sector
	 	NEWPOTAT_MUS,		// Level 7: Spud City
	 	TURNIP_MUS,		// Level 8: Cauliflower Quarter
	 	SPITTIN_MUS,		// Level 9: Apple Acres
	 	GROVE_MUS,		// Level 10: Grape Grove
	 	SLYBEAN_MUS,		// Level 11: Turnip Trapway
	 	GRASS_MUS,		// Level 12: Brussels Sprout Bay
	 	STAMP_MUS,		// Level 13: Mushroom Mere
	 	BULLFROG_MUS,		// Level 14: Squash Swamp
	 	BULLFROG_MUS,		// Level 15: Lettuce Lake
	 	SLEEP_MUS,		// Level 16: Steel Bridge
	 	WAY_MUS,		// Level 17: Guard Tower
	 	DEEP_MUS,		// Level 18: Castle Basement
		ASPALLEG_MUS,		// Level 19: Castle Tuberia
		HASH_MUS,		// Level 20: Royal Chamber
		ANTSPANT_MUS,		// Level 21: Mountain Top
		SLEEP_MUS,		// Level 22: Hill Top
		SLEEP_MUS,		// Level 23: Hill Top
		ANTSPANT_MUS,		// Level 24: Mountain Top
	 	SMOOTHIE_MUS,		// Level 25: Horseradish Hill
		DARKHERE_MUS,		// Level 26: Cantaloupe Mine
		ROWSAD_MUS,		// Level 27: Bridge Bottoms
		ROCKIN_MUS,		// Level 28: Rhubarb Rapids
		WAY_MUS,		// Level 29: Parsnip Pass
		STAMP_MUS,		// Level 30: Kumquat Chasm
		ELECTRO_MUS,		// Level 31: Veggie Verge
		SMOOTHIE_MUS,		// Level 32: Beet Street
		NEWPOTAT_MUS,		// Level 33: Spud City
		NEWPOTAT_MUS,		// Level 34: Potato Point
		SMOOTHIE_MUS,		// Level 35: Pea Pod Park
		DARKHERE_MUS,		// Level 36: Pumpkin Plot
		SPITTIN_MUS,		// Level 37: Apple Acres
		SPITTIN_MUS,		// Level 38: Fruity Forest
		GROVE_MUS,		// Level 39: Grape Grove
		BULLFROG_MUS,		// Level 40: Brussels Sprout Bay
		BULLFROG_MUS,		// Level 41: Squash Swamp
		GRASS_MUS,		// Level 42: Coastal Creek
		ASPALLEG_MUS,		// Level 43: Castle Tuberia
		ELECTRO_MUS,		// Level 44: Eggplant Estate
		ELECTRO_MUS,		// Level 45: Castle Yard
		WAY_MUS,		// Level 46: Mount Tuberest
		VEGGIES_MUS,		// Level 47: Title Page
	 	BROCCOLI_MUS,		// Level 48: Story Screen
	 	BLLYFRND_MUS,		// Level 49: Easy mode's ending
		GUESS_MUS,		// Level 50: Name typing if high score reached
		CALL_MUS,		// Level 51: Secret level intro text
	 	ISCREAM_MUS,		// Level 52: Bad ending / Mortimer's ending
		ALREADY_MUS,		// Level 53: Main menu
	 	NOBADDR_MUS,		// Level 54: Normal, hard and keen mode's ending
		PLATE_MUS		// Level 55: Help
	};

	int song;
	boolean wasfaded;

	// K1n9_Duk3 fix: Use StopMusic instead of SD_MusicOff to make sure old music is purgable
	StopMusic();

	if (MusicMode != smm_AdLib)
		return;
	
	song = songs[num];
		
	if (song == -1 || MusicMode != smm_AdLib)
	{
		return;
	}

	MM_BombOnError(false);
	CA_CacheAudioChunk(STARTMUSIC+song);
	MM_BombOnError(true);
	if (mmerror)
	{
		mmerror = false;
		if (!DemoMode)
		{
			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			PrintY += 20;
			US_CPrintFar(INSMEMFORBGMUSIC);
			VW_UpdateScreen();
			wasfaded = screenfaded;
			if (wasfaded)
			{
				VW_SetDefaultColors();
			}
			IN_ClearKeysDown();
			IN_UserInput(3*TickBase, false);
			if (wasfaded)
			{
				VW_FadeOut();
			}
		}
	}
	else
	{
	// K1n9_Duk3 fix: prevent memory manager from messing with the music
		MM_SetLock((memptr *) &audiosegs[STARTMUSIC + song], true);

		SD_StartMusic((MusicGroup far *) audiosegs[STARTMUSIC + song]);
	}
}

char far democredits1[] = "Original Game\n"
						"John Carmack\n"
						"\n"
						"Utilities\n"
						"John Romero\n"
						"\n"
						"Interface/Sound\n"
						"Jason Blochowiak\n"
						"\n"
						"Creative Director\n"
						"Tom Hall\n"
						"\n"
						"Music\n"
						"Robert Prince\n"
						"\n"
						"Art\n"
						"Adrian Carmack\n";
                        
char far democredits2[] = "Commander Keen\n"
						"Copyright 1990-91\n"
						"Id Software, Inc.\n"
						"\n"
						"Gamer's Edge\n"
						"\"Keen Dreams\"\n"
						"Copyright 1991-93\n"
						"Softdisk, Inc.\n"
						"\n"
						"Javier M. Chavez\n"
						"\"Keen Dreams\"\n"
						"Copyright 2014\n"
						"\n"
						"Szemigi\n"
						"\"Keen Dreams\n"
						"Gold Edition\"\n"
						"Copyright 2026\n";


/*
=====================
=
= DemoLoop
=
=====================
*/

void
DemoLoop (void)
{
	/* char		*s; */
	word		move;
	longword	lasttime;
	WindowRec	mywin;

//
// demo loop
//
	US_SetLoadSaveHooks(LoadTheGame,SaveTheGame,ResetGame);
	restartgame = gd_Continue;
	while (true)
	{

		while (!restartgame && !loadedgame)
		{
			playstate = notdone;
			VW_InitDoubleBuffer();
			IN_ClearKeysDown();

			while (true)
			{
				gamestate.mapon = 47;		// title map number
				loadedgame = false;
				SetupGameLevel(true);

				VW_FadeOut ();
				RF_FixOfs();
				
				StartMusic(47);

				// Display the Title map
				RF_NewPosition((5 * TILEGLOBAL) + (TILEGLOBAL / 2),
								(TILEGLOBAL * 2) + (TILEGLOBAL / 2)
								+ (TILEGLOBAL / 4));
				RF_ForceRefresh();
				RF_Refresh();
				RF_Refresh();

				VW_UpdateScreen ();
				VW_FadeIn();

				if (Wait(TickBase * 2))
					break;

				mywin.x = (16 * 13) + 4;
				mywin.y = 0;
				mywin.w = 16 * 7;
				mywin.h = 200;
				mywin.px = mywin.x + 0;
				mywin.py = mywin.y + 10;

				if (ShowText((9 * TILEGLOBAL) - (PIXGLOBAL * 2),&mywin,democredits1))
					break;

				mywin.x = 4;
				mywin.y = 0;
				mywin.w = 16 * 7;
				mywin.h = 200;
				mywin.px = mywin.x + 0;
				mywin.py = mywin.y + 10;

				if (ShowText((2 * TILEGLOBAL) + (PIXGLOBAL * 2),&mywin,democredits2))
					break;

				if (MoveTitleTo((5 * TILEGLOBAL) + (TILEGLOBAL / 2)))
					break;
				if (Wait(TickBase * 3))
					break;

				CA_MarkGrChunk(KEENDGOLDPIC);
				CA_CacheMarks(NULL, 0);
				SD_PlaySound (THROWBOMBSND);
				VW_FixRefreshBuffer();

				VWB_DrawPic(0, 0, KEENDGOLDPIC);
				UNCACHEGRCHUNK (KEENDGOLDPIC);
				VW_UpdateScreen ();
				IN_ClearKeysDown();

				if (IN_UserInput(TickBase * 8,false))
					break;

				StopMusic();
				if (nightmode == true)
				{
					RunDemo(3);
				}
				else
				{
					RunDemo(0);
				}

				if (LastScan)
				{
    					IN_ClearKeysDown();
    					break;
				}

				VW_FadeOut ();
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen ();

				RF_FixOfs();
				StartMusic(48);

				CA_MarkGrChunk(STORYSCREENPIC);
				CA_CacheMarks(NULL, 0);
				VW_FixRefreshBuffer();

				VWB_DrawPic(0, 0, STORYSCREENPIC);
				UNCACHEGRCHUNK (STORYSCREENPIC);
				VW_UpdateScreen ();
				VW_FadeIn ();
				IN_ClearKeysDown();
				IN_Ack();

				StopMusic();
				if (nightmode == true)
				{
					RunDemo(4);
				}
				else
				{
					RunDemo(1);
				}

				if (LastScan)
				{
    					IN_ClearKeysDown();
    					break;
				}

				StopMusic();

				VW_FadeOut ();
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen ();

				VW_FadeOut ();
				StartMusic(47);
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen ();

				CA_MarkGrChunk(TITLESCRPIC);
				CA_CacheMarks(NULL, 0);
				VW_FixRefreshBuffer();

				VWB_DrawPic(0, 0, TITLESCRPIC);
				UNCACHEGRCHUNK (TITLESCRPIC);

				VW_UpdateScreen ();
				VW_FadeIn ();
				IN_ClearKeysDown();

				if (IN_UserInput(TickBase * 8,false))
					break;

				VW_FadeOut ();
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen ();

				RF_FixOfs();
				VWB_Bar(0,0,320,200,FIRSTCOLOR);
				US_DisplayHighScores(-1);
				VW_UpdateScreen ();
				IN_ClearKeysDown();

				if (IN_UserInput(TickBase * 8,false))
					break;

				VW_FadeOut ();
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen ();

				StopMusic();
				gamestate.difficulty = gd_Grand;
				if (nightmode == true)
				{
					RunDemo(5);
				}
				else
				{
					RunDemo(2);
				}

				if (LastScan)
				{
    					IN_ClearKeysDown();
    					break;
				}

				StopMusic();
				gamestate.difficulty = gd_Normal;

				VW_FadeOut ();
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen ();

				RF_FixOfs();

				VW_UpdateScreen ();
				VW_FadeIn ();
				IN_ClearKeysDown();
			}

			StopMusic();

			US_ControlPanel ();
		}

		if (!loadedgame)
			NewGame();
		if (loadedgame)			// Szemigi: Zero difficulty bug fixed. See https://keenwiki.shikadi.net/wiki/Keen_Dreams_Bugs#Super_easy_difficulty 
		switch(gamestate.difficulty)
		{
		case 1:
			greenball = true;
			mortwindow = false;
			mortimerbedgfx = false;
			restartgame = gd_Easy;
			break;
		case 2:
			greenball = false;
			mortwindow = false;
			mortimerbedgfx = false;
			restartgame = gd_Normal;
			break;
		case 3:
			greenball = false;
			mortwindow = false;
			mortimerbedgfx = false;
			restartgame = gd_Hard;
			break;
		case 4:
			greenball = false;
			mortwindow = false;
			mortimerbedgfx = false;
			restartgame = gd_Keen;
			break;
		case 5:
			if (mortimerunlocked == true)
			{
				mortimerbedgfx = true;
				mortwindow = true;
				restartgame = gd_Grand;
			}
			break;
	}
		GameLoop();
	}
}

                                                    
