/*
Keen Worlds: Foray in the Forest's dialogue windows, converted from CK_SPEC.C
of its source (Nisaba and K1n9_Duk3; GPL v2 or later) by
tools/kw_fitf_port.py, with the hand fixes of tools/kw_fitf_dialog_fixes.py.
*/

#include "kw_fitf.h"
#include "kw_ff_keen.h"

void CK_MeasureMultiline(const char *str, uint16_t *w, uint16_t *h);


//===========================================================================

/*
====================
=
= CacheAndDrawPic
=
= Helper routine for showing dialog messages. Automatically caches the desired
= PIC chunk, draws it, and makes the chunk purgable.
=
====================
*/

static void   FF_CacheAndDrawPic(int16_t x, int16_t y, int16_t pic)
{
	if (!ca_graphChunks[pic])
	{
		CA_CacheGrChunk(pic);
		;	// Note: this just makes the chunk purgable, it can still be drawn
	}
	VHB_DrawBitmap(x, y, pic);
}

/*
====================
=
= CK_DialogWindow
=
= Helper routine for showing dialog messages. Draws a dialog window with a
= potrait image and the desired text, then (optionally) waits for a key press
= after the specified minimum wait.
=
====================
*/

static void   FF_DialogWindow(int16_t pic, int16_t str, bool left, int16_t wait)
{
	uint16_t w, h;

	US_CenterWindow(26, 8);
	US_SetWindowW(US_GetWindowW() - (48));
	if (left)
	{
		FF_CacheAndDrawPic(US_GetWindowX(), US_GetWindowY(), pic);
		US_SetWindowX(US_GetWindowX() + (48));
	}
	else
	{
		FF_CacheAndDrawPic(US_GetWindowX()+US_GetWindowW(), US_GetWindowY(), pic);
	}
	SM_MeasureMultiline(str, &w, &h);
	US_SetPrintY(US_GetPrintY() + ((US_GetWindowH()-h)/2));
	SM_CPrint(str);
	VH_UpdateScreen();
	if (wait)
	{
		VL_DelayTics(wait);
		IN_ClearKeysDown();
		IN_WaitButton();
	}
}

/*
====================
=
= CK_DialogThumbsUp
=
= Helper routine for showing dialog messages. Draws Keen's thumbs up potrait
= image and then (optionally) waits for a key press after the specified minimum
= wait. The window and the text are not re-drawn, only the portrait.
=
====================
*/

void  FF_DialogThumbsUp(int16_t wait)
{
	FF_CacheAndDrawPic(US_GetWindowX()+US_GetWindowW(), US_GetWindowY(), FF_CD_KEENPIC);
	FF_CacheAndDrawPic(US_GetWindowX()+US_GetWindowW(), US_GetWindowY()+44, FF_CD_ONEPIC);
	VH_UpdateScreen();
	if (wait)
	{
		VL_DelayTics(wait);
		IN_ClearKeysDown();
		IN_WaitButton();
	}
}

/*
====================
=
= CK_DialogEnter
=
= Helper routine for showing dialog messages. Prepares everything for displaying
= a dialog window on top of the main game screen.
=
====================
*/

void  FF_DialogEnter(void)
{
	CA_UpLevel();
	CA_SetGrPurge();
	VL_FixRefreshBuffer();
}

/*
====================
=
= CK_DialogExit
=
= Helper routine for showing dialog messages. Takes care of the cleanup when the
= dialog end and the game is about to continue.
=
====================
*/

void  FF_DialogExit(void)
{
	CA_DownLevel();
	FF_ForceScoreboxUpdate();
	RF_ForceRefresh();
}

/*
====================
=
= CK_JanitorDialog
=
= Dialog for Keen talking to the janitor in various levels.
=
====================
*/

void FF_JanitorDialog(int16_t number)
{
	uint16_t bit = 1 << number;
	FF_DialogEnter();
	SD_PlaySound(FF_JANITORDIALOGSND);

	if (ff_state.janitorMessageShown & bit)
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORJOKE0+number, true, 30);
	
	switch (number)
	{
	case 0:	// Nisaba patch: Level 2 Dialog 
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE0, true, 30);
		FF_DialogWindow(FF_CD_KEENPIC, FF_S_JANITORREPLY0, false, 30);
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE1, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY1, false, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE2, true, 30);
		break;
		
	case 1:	// Level 4 Dialog 
		if (ff_state.janitorMessageShown)	// K1n9_Duk3 mod: only show "You again?" text when another janitor message was shown before
			FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY2, false, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE3, true, 30);
		FF_DialogWindow(FF_CD_KEENPIC, FF_S_JANITORREPLY3, false, 30);
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE4, true, 30);
		break;
		
	case 2:	// Level 8 Dialog 
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE5, true, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE6, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY4, false, 30);
		FF_DialogThumbsUp(30);
		break;
		
	case 3:	// Level 9 Dialog 
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE7, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY5, false, 30);
		break;
		
	case 4:	// Level 11 Dialog 
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE8, true, 30);
		FF_DialogWindow(FF_KEENDIVERPIC, FF_S_JANITORREPLY6, false, 30);
		break;
		
	case 5:	// Level 14 Dialog 
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE9, true, 30);
		FF_DialogWindow(FF_KEENMASKPIC, FF_S_JANITORREPLY7, false, 30);
		break;
		
	case 6:	// Level 15 Dialog 
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE10, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY8, false, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE11, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY9, false, 30);
		break;
		
	case 7:	// Level 24.1 Dialog 
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY10, false, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE12, true, 30);
		FF_DialogWindow(FF_CD_KEENPIC, FF_S_JANITORREPLY15, false, 30);
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE18, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY9, false, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE19, true, 30);
		break;
		
	case 8:	// Level 24.2 Dialog 
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY11, false, 30);
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE13, true, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE14, true, 30);
		FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMESSAGE20, true, 30);
		break;
		
	case 9:	// Level 24.3 Dialog 
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE15, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY12, false, 30);
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE16, true, 30);
		FF_DialogWindow(FF_CD_KEENDONUTPIC, FF_S_JANITORREPLY13, false, 30);
		break;
		
	case 10:	// Level 24.4 Dialog 
		FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORMESSAGE17, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_JANITORREPLY14, false, 30);
		break;
	}
	
	ff_state.janitorMessageShown |= bit;
	FF_DialogExit();
}

/*
====================
=
= CK_JanitorSleepMonolog
=
= For when the Janitor touches Keen while Keen is sleeping.
=
====================
*/

void FF_JanitorSleepMonolog(void)
{
	FF_DialogEnter();
	SD_PlaySound(FF_JANITORDIALOGSND);

	FF_DialogWindow(FF_JANITORMOPPIC, FF_S_JANITORMONOLOG0+ff_map.dreamMsg, true, 0);
	IN_ClearKeysDown();
	IN_UserInput(8*70, true);	// show message for 8 seconds, then resume the game

	if (++ff_map.dreamMsg >= FF_S_JANITORMONOLOGEND-FF_S_JANITORMONOLOG0)
		ff_map.dreamMsg = 0;

	FF_DialogExit();
}

/*
====================
=
= CK_JanitorSleepMonolog
=
= For when the Janitor touches Keen while Keen is stunned.
=
====================
*/

void FF_JanitorStunnedMonolog(void)
{
	FF_DialogEnter();
	SD_PlaySound(FF_JANITORDIALOGSND);

	FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORSTUNNEDMSG0, true, 30);

	FF_DialogExit();
}

/*
====================
=
= CK_JanitorNisasiDialog
=
= For when the Janitor touches Keen while Keen has a Nisasi stuck on his head.
=
====================
*/

void FF_JanitorNisasiDialog(int16_t number)
{
	uint16_t oldfont = US_GetPrintFont();

	FF_DialogEnter();
	SD_PlaySound(FF_JANITORDIALOGSND);

	CA_CacheGrChunk(FF_STARTFONT+3);

	FF_DialogWindow(FF_JANITORPIC, FF_S_JANITORNISASIMESSAGE0+number, true, 30);
	US_SetPrintFont(3);
	FF_DialogWindow(FF_KEENNISASIPIC, FF_S_JANITORNISASIREPLY0+number, false, 30);
	US_SetPrintFont(oldfont);

	FF_DialogExit();
}

/*
====================
=
= CK_LindseyDialog
=
= Dialog for Keen talking to Princess Lindsey in the bonus level.
=
====================
*/

void FF_LindseyDialog(int16_t number)
{
	FF_DialogEnter();
	SD_PlaySound(FF_FOOTAPPEARSND);
	FF_DialogWindow(FF_LINDSEYPIC, FF_S_LINDSEYMESSAGE0+number, true, 30);
	FF_DialogWindow(number==2? FF_KEENDONUTPIC : FF_CD_KEENPIC, FF_S_LINDSEYREPLY0+number, false, 30);

	if (number == 2)
	{
		FF_DialogWindow(FF_LINDSEYPIC, FF_S_LINDSEYMESSAGE3, true, 30);
	/*
		VW_FadeOut();
		VW_ClearVideo(BLACK);
		VW_SetDefaultColors();
	*/
		CA_DownLevel();	// don't redraw the level, we're going straight to the loading screen anyway
	}
	else
	{
		FF_DialogThumbsUp(30);
		FF_DialogExit();
	}
}

/*
====================
=
= CK_MiscDialog
=
= Miscellaneous dialog messages for interacting with special dialog tiles in the
= level.
=
====================
*/

void FF_MiscDialog(int16_t number)
{
	FF_DialogEnter();
	
	switch (number)
	{
	case -2:	// cheater TARDIS message
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_TARDISMESSAGE, false, 30);
		break;
		
	case -1:	// Spaceman Spiff message
		FF_StartMusic(FF_TheBigV_Hill_Yoshi, true);
		SD_PlaySound(FF_COMPSCOREDSND);
		if (!ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL])
			FF_DialogWindow(FF_SPACEMANSPIFFPIC, FF_S_SPACEMANSPIFFMESSAGE3, false, 30);
		else
		{
			FF_DialogWindow(FF_SPACEMANSPIFFPIC, FF_S_SPACEMANSPIFFMESSAGE1, false, 30);
			FF_DialogWindow(FF_SPACEMANSPIFFPIC, FF_S_SPACEMANSPIFFMESSAGE2, false, 30);
		}
		// draw a centered window that is 36 TILE8M tiles wide and 21 tiles high:
		// (the size is the client area, i.e. the white part; 36x21 tiles = 288x168 pixels)
		US_CenterWindow(36, 21);
		US_SetPrintY(US_GetPrintY() + (4));
		if (!ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL])
			SM_CPrint(FF_S_SPACEMANSPIFFMESSAGE5);
		else
			SM_CPrint(FF_S_SPACEMANSPIFFMESSAGE4);
		VH_UpdateScreen();

		VL_DelayTics(30);	// wait a while before reacting to user input
		IN_ClearKeysDown();
		IN_WaitButton();
	
		FF_StartMusic(ff_map.music, true);
		break;
		
	case 0:	// HAL dialog
		SD_PlaySound(FF_COMPSCOREDSND);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_HALMESSAGE1, false, 30);
		SD_PlaySound(FF_COMPSCOREDSND);
		FF_DialogWindow(FF_HALPIC, FF_S_HALREPLY1, true, 30);
		FF_DialogWindow(FF_KEENTALKPIC, FF_S_HALMESSAGE2, false, 30);
		SD_PlaySound(FF_COMPSCOREDSND);
		FF_DialogWindow(FF_HALPIC, FF_S_HALREPLY2, true, 30);
		SD_PlaySound(FF_REFILLUNITSND);
		break;
		
	case 1:	// Jukebox
		if (SD_GetMusicMode() == smm_AdLib)
		{
			FF_Jukebox();
			
			// this will get merged with the Snailas and Foobs In Space! cases:
			IN_ClearKeysDown();
		}
		else
		{
			SD_PlaySound(FF_GLITCHSND);
			FF_DialogWindow(FF_TRANSCEIVERPIC, FF_S_RADIOMESSAGE1, true, 30);
		}
		break;
		
	case 2:	// Snailas
		FF_FadeOut();	// optional
		FF_RunSnailas();
		
		// this will get merged with the Foobs In Space! case:
		FF_DelayedFade();	// to avoid display glitches in DOSBox
		IN_ClearKeysDown();
		break;
		
	case 3:	// Foobs In Space!
		FF_FadeOut();	// optional
		FF_RunFoobsInSpace();
		
		FF_DelayedFade();	// for consistency with Snailas behavior
		IN_ClearKeysDown();
		break;
		
	default:
		number -= 4;
		if ((uint16_t)number < FF_S_MISCMESSAGEEND-FF_S_MISCMESSAGE4)
		{
			FF_DialogWindow(FF_KEENTALKPIC, FF_S_MISCMESSAGE4+number, false, 30);
		}
		else
		{
			SD_PlaySound(FF_OLDNOWAYSND);
		}

	}
	
	FF_DialogExit();
}


/*
====================
=
= CK_BonusLevelDialog
=
= Dialog message for unlocking the bonus level.
=
====================
*/

void FF_BonusLevelDialog(void)
{
	if (ck_gameState.currentLevel == 0 && ck_gameState.keenScore >= BONUSLEVEL_SCORELIMIT
		&& !ck_gameState.levelsDone[MAP_BONUSLEVEL] && !ff_state.cheated)
	{
		// make sure dialog is only shown once:
		if (ff_state.collectiblesTotal & 0x8000)
			return;
		ff_state.collectiblesTotal |= 0x8000;
		
		FF_DialogEnter();
		
		SD_PlayTwoSounds(FF_FOOTAPPEARSND, FF_COLLECTEDALLPOINTSSND);
		FF_DialogWindow(FF_H_CANDYOVERFLOWPIC, ck_gameState.keenScore == BONUSLEVEL_SCORELIMIT? FF_S_BONUSLEVELINFO1 : FF_S_BONUSLEVELINFO2, true, 30);
			
		FF_DialogExit();
	}
}

