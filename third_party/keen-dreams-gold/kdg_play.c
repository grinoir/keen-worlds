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

// KDG_PLAY.C

#include "kdg_def.h"
#pragma	hdrstop
#include <SDL.h>



/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define	INACTIVATEDIST	10

#define MAXMOVE	(TILEGLOBAL-17)

enum {
	MAINMENU_LUMP,    // 0
	MMAINMENU_LUMP,	  // 1
	DEMO_LUMP,	  // 2
	PONG_LUMP,	  // 3
	KEEN_LUMP,        // 4
	KEEN2_LUMP,       // 5
	KEENEXIT_LUMP,	  // 6
	INLEVEL_LUMP,	  // 7
	WORLDKEEN_LUMP,   // 8
	BROCCO_LUMP,      // 9
	TOMAT_LUMP,       // 10
	BTOMAT_LUMP,	  // 11
	CARROT_LUMP,      // 12
	ASPAR_LUMP,       // 13
	GRAPE_LUMP,       // 14
	TATER_LUMP,       // 15
	TATERII_LUMP,     // 16
	CART_LUMP,   	  // 17
	MCART_LUMP,       // 18
	FRENCHY_LUMP,     // 19
	MELON_LUMP,       // 20
	CANTALOUPE_LUMP,  // 21
	SQUASH_LUMP,      // 22
	ALTSQUASH_LUMP,	  // 23
	APEL_LUMP,        // 24
	PEA_LUMP,         // 25
	BOOBUS_LUMP,      // 26
	SBOOBUS_LUMP,     // 27
	PBOOBUS_LUMP,     // 28
	BUSTLING_LUMP,	  // 29
	CELERY_LUMP,      // 30
	MUSHROOM_LUMP,    // 31
	BRUSSEL_LUMP,     // 32
	TURNIP_LUMP,      // 33
	CAULI_LUMP, 	  // 34
	RHUBARB_LUMP,     // 35
	PARSNIP_LUMP,     // 36
	HORSERAD_LUMP,    // 37
	PUMPKIN_LUMP,	  // 38
	EGGPLANT_LUMP,	  // 39
	PEAR_LUMP,	  // 40
	BEET_LUMP,	  // 41
	BEAN_LUMP,	  // 42
	BABERRY_LUMP,	  // 43
	KUMQUAT_LUMP,	  // 44
	CORN_LUMP,	  // 45
	MACHINE_LUMP,     // 46
	GARGOYLE_LUMP,	  // 47
	MOSQUITO_LUMP,	  // 48
	GUARDROBOT_LUMP,  // 49
	PINKOWL_LUMP,	  // 50
	CAMEOS_LUMP,	  // 51
	ROVER_LUMP,	  // 52
	KID_LUMP,	  // 53
	STEELCART_LUMP,	  // 54
	MORT_LUMP,	  // 55
	MORT2_LUMP,	  // 56
	MORTEXIT_LUMP,	  // 57
	WORLDMORT_LUMP,	  // 58
	BLUEFLOWERPOWER_LUMP,	  // 59
	KEENLOWSHOOT_LUMP,	  // 60
	MORTLOWSHOOT_LUMP,	  // 61
	KEYCARD_LUMP,	  // 62
	NUMLUMPS          // 63
};

int lumpstart[NUMLUMPS] = {
	MAINMENU_LUMP_START,
	MMAINMENU_LUMP_START,
	DEMO_LUMP_START,
	PONG_LUMP_START,
	KEEN_LUMP_START,
	KEEN2_LUMP_START,
	KEENEXIT_LUMP_START,
	INLEVEL_LUMP_START,
	WORLDKEEN_LUMP_START,
	BROCCO_LUMP_START,
	TOMAT_LUMP_START,
	BTOMAT_LUMP_START,
	CARROT_LUMP_START,
	ASPAR_LUMP_START,
	GRAPE_LUMP_START,
	TATER_LUMP_START,
	TATERII_LUMP_START,
	CART_LUMP_START,
	MCART_LUMP_START,
	FRENCHY_LUMP_START,
	MELON_LUMP_START,
	CANTALOUPE_LUMP_START,
	SQUASH_LUMP_START,
	ALTSQUASH_LUMP_START,
	APEL_LUMP_START,
	PEA_LUMP_START,
	BOOBUS_LUMP_START,
	SBOOBUS_LUMP_START,
	PBOOBUS_LUMP_START,
	BUSTLING_LUMP_START,
	CELERY_LUMP_START,
	MUSHROOM_LUMP_START,
	BRUSSEL_LUMP_START,
	TURNIP_LUMP_START,
	CAULI_LUMP_START,
	RHUBARB_LUMP_START,
	PARSNIP_LUMP_START,
	HORSERAD_LUMP_START,
	PUMPKIN_LUMP_START,
	EGGPLANT_LUMP_START,
	PEAR_LUMP_START,
	BEET_LUMP_START,
	BEAN_LUMP_START,
	BABERRY_LUMP_START,
	KUMQUAT_LUMP_START,
	CORN_LUMP_START,
	MACHINE_LUMP_START,
	GARGOYLE_LUMP_START,
	MOSQUITO_LUMP_START,
	GUARDROBOT_LUMP_START,
	PINKOWL_LUMP_START,
	CAMEOS_LUMP_START,
	ROVER_LUMP_START,
	KID_LUMP_START,
	STEELCART_LUMP_START,
	MORT_LUMP_START,
	MORT2_LUMP_START,
	MORTEXIT_LUMP_START,
	WORLDMORT_LUMP_START,
	BLUEFLOWERPOWER_LUMP_START,
	KEENLOWSHOOT_LUMP_START,
	MORTLOWSHOOT_LUMP_START,
	KEYCARD_LUMP_START,
};

int lumpend[NUMLUMPS] = {
	MAINMENU_LUMP_END,
	MMAINMENU_LUMP_END,
	DEMO_LUMP_END,
	PONG_LUMP_END,
	KEEN_LUMP_END,
	KEEN2_LUMP_END,
	KEENEXIT_LUMP_END,
	INLEVEL_LUMP_END,
	WORLDKEEN_LUMP_END,
	BROCCO_LUMP_END,
	TOMAT_LUMP_END,
	BTOMAT_LUMP_END,
	CARROT_LUMP_END,
	ASPAR_LUMP_END,
	GRAPE_LUMP_END,
	TATER_LUMP_END,
	TATERII_LUMP_END,
	CART_LUMP_END,
	MCART_LUMP_END,
	FRENCHY_LUMP_END,
	MELON_LUMP_END,
	CANTALOUPE_LUMP_END,
	SQUASH_LUMP_END,
	ALTSQUASH_LUMP_END,
	APEL_LUMP_END,
	PEA_LUMP_END,
	BOOBUS_LUMP_END,
	SBOOBUS_LUMP_END,
	PBOOBUS_LUMP_END,
	BUSTLING_LUMP_END,
	CELERY_LUMP_END,
	MUSHROOM_LUMP_END,
	BRUSSEL_LUMP_END,
	TURNIP_LUMP_END,
	CAULI_LUMP_END,
	RHUBARB_LUMP_END,
	PARSNIP_LUMP_END,
	HORSERAD_LUMP_END,
	PUMPKIN_LUMP_END,
	EGGPLANT_LUMP_END,
	PEAR_LUMP_END,
	BEET_LUMP_END,
	BEAN_LUMP_END,
	BABERRY_LUMP_END,
	KUMQUAT_LUMP_END,
	CORN_LUMP_END,
	MACHINE_LUMP_END,
	GARGOYLE_LUMP_END,
	MOSQUITO_LUMP_END,
	GUARDROBOT_LUMP_END,
	PINKOWL_LUMP_END,
	CAMEOS_LUMP_END,
	ROVER_LUMP_END,
	KID_LUMP_END,
	STEELCART_LUMP_END,
	MORT_LUMP_END,
	MORT2_LUMP_END,
	MORTEXIT_LUMP_END,
	WORLDMORT_LUMP_END,
	BLUEFLOWERPOWER_LUMP_END,
	KEENLOWSHOOT_LUMP_END,
	MORTLOWSHOOT_LUMP_END,
	KEYCARD_LUMP_END,
};

boolean lumpneeded[NUMLUMPS];

//boolean		lumpneeded[NUMLUMPS];
//int			lumpstart[NUMLUMPS] =

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

exittype	playstate;
gametype	gamestate;

boolean		demorecord,demoplayback;
char		far *demoptr, far *lastdemoptr;
memptr		demobuffer;

boolean		keenkilled;

boolean		debugok;

boolean 	showscorebox;

boolean		button0held,button1held;
objtype		*new,*check,*player,*scoreobj,*riding;

unsigned	originxtilemax,originytilemax;

ControlInfo	c;

objtype dummyobj;
objtype		*riding;
boolean		onmushroom;

char far l0n[] = "The Land of Tuberia";
char far l1n[] = "Horseradish Hill";
char far l2n[] = "Melon Mines";
char far l3n[] = "Bridge Bottoms";
char far l4n[] = "Rhubarb Rapids";
char far l5n[] = "Parsnip Pass";
char far l6n[] = "Celery Sector";
char far l7n[] = "Spud City";
char far l8n[] = "Cauliflower Quarter";
char far l9n[] = "Apple Acres";
char far l10n[] = "Grape Grove";
char far l11n[] = "Turnip Trapway";
char far l12n[] = "Brussels Sprout Bay";
char far l13n[] = "Mushroom Mere";
char far l14n[] = "Squash Swamp";
char far l15n[] = "Lettuce Lake";
char far l16n[] = "Steel Bridge";
char far l17n[] = "Guard Tower";
char far l18n[] = "Castle Basement";
char far l19n[] = "Castle Tuberia";
char far l20n[] = "Royal Chamber";
char far l21n[] = "Mountain Top";
char far l22n[] = "Hill Top";
char far l23n[] = "Hill Top";
char far l24n[] = "Mountain Top";
char far l25n[] = "Horseradish Hill";
char far l26n[] = "Cantaloupe Mine";
char far l27n[] = "Bridge Bottoms";
char far l28n[] = "Rhubarb Rapids";
char far l29n[] = "Parsnip Pass";
char far l30n[] = "Kumquat Chasm";
char far l31n[] = "Veggie Verge";
char far l32n[] = "Beet Street";
char far l33n[] = "Spud City";
char far l34n[] = "Potato Point";
char far l35n[] = "Pea Pod Park";
char far l36n[] = "Pumpkin Plot";
char far l37n[] = "Apple Acres";
char far l38n[] = "Fruity Forest";
char far l39n[] = "Grape Grove";
char far l40n[] = "Brussels Sprout Bay";
char far l41n[] = "Squash Swamp";
char far l42n[] = "Coastal Creek";
char far l43n[] = "Castle Tuberia";
char far l44n[] = "Eggplant Estate";
char far l45n[] = "Castle Yard";
char far l46n[] = "Mount Tuberest";
char far l47n[] = "Title Page";

char far *levelnames[GAMELEVELS] = {
	l0n,
	l1n,
	l2n,
	l3n,
	l4n,
	l5n,
	l6n,
	l7n,
	l8n,
	l9n,
	l10n,
	l11n,
	l12n,
	l13n,
	l14n,
	l15n,
	l16n,
	l17n,
	l18n,
	l19n,
	l20n,
	l21n,
	l22n,
	l23n,
	l24n,
	l25n,
	l26n,
	l27n,
	l28n,
	l29n,
	l30n,
	l31n,
	l32n,
	l33n,
	l34n,
	l35n,
	l36n,
	l37n,
	l38n,
	l39n,
	l40n,
	l41n,
	l42n,
	l43n,
	l44n,
	l45n,
	l46n,
	l47n
};

char far INSMEMTOLOADLEVEL[] =
  "Insufficient memory\n"
  "to load level!\n";

char far BADENDING1[] =
  "As Keen collapses in sleepiness\n"
  "from the evil magic,\n"
  "the protuberant couple and their\n"
  "Veggie Army marches off into Tuberia,\n" 
  "searching for more captives\n"
  "unfortunate enough to wind up here.\n"
  "The screams of the menaced children\n" 
  "off in the distance eventually stir\n"
  "Keen from his slumber.\n"
  "\n"
  "HOWEVER...\n"
  "There's a problem. They knew\n"
  "someone would try to turn the\n"
  "Dream Machine off, so they locked it away.\n"
  "\n"
  "...Keen was too weak for them.\n";

char far BADENDING2[] =
  "\n\n\n"
  "Without a key, and drowsily hearing the\n"
  "machine humming behind the sturdy door,\n"
  "Keen tries in-vain to bang it\n"
  "open with his fists.\n"
  "It's too late. A Tater Troop bursts in\n" 
  "and suddenly, Keen jolts awake in\n"
  "bed with a scream:\n\n"
  "\"NOOOOOOO!!!!\"\n";

char far MORTBADENDING[] =
  "As Mortimer collapses in sleepiness\n"
  "from King Boobus' evil magic,\n"
  "the protuberant king and his\n"
  "Veggie Army marches off into Tuberia.\n"
  "The screams of the menaced children\n"
  "off in the distance eventually stir\n"
  "Mortimer from his slumber.\n"
  "Drowsily hearing the Dream Machine\n"
  "running behind a locked door, he tries\n"
  "in-vain to bang open the sturdy door\n"
  "with his fists.\n"
  "It's too late. A Tater Troop bursts in\n"
  "and Mortimer awakes with a start.\n\n"
  "It seems even a potato is\n"
  "smarter than him!\n";

char far QUITCONFTEXT[] =
  "Are you sure you wanna\n"
  "quit to DOS now?\n\n"
  "Press Y to quit\n"
  "Press N or Esc to back out\n";

char far CHEATNIGHTGAMEFINALE[] =
  "HEY! I know what you're\n"
  "trying to do! You know\n"
  "better than that!\n";

char far USERCHEATTEXT[] =
  "Cheat Option!\n"
  "\n"
  "You just got a key,\n"
  "99 pellets, and an\n"
  "extra keen!\n";

char far NIGHTUSERCHEATTEXT[] =
  "Cheat Option!\n"
  "\n"
  "You just got a key,\n"
  "99 ammos, and an\n"
  "extra keen!\n";

char far MORTUSERCHEATTEXT[] =
  "Cheat Option!\n"
  "\n"
  "You just got a key,\n"
  "99 pellets, and an\n"
  "extra mort!\n";

char far MORTNIGHTUSERCHEATTEXT[] =
  "Cheat Option!\n"
  "\n"
  "You just got a key,\n"
  "99 ammos, and an\n"
  "extra mort!\n";

char far KEYCARDUSERCHEATTEXT[] =
  "Cheat Option!\n"
  "\n"
  "You just got 1 keycard,\n"
  "99 pellets, and an\n"
  "extra keen!\n";

char far MORTKEYCARDUSERCHEATTEXT[] =
  "Cheat Option!\n"
  "\n"
  "You just got 1 keycard,\n"
  "99 pellets, and an\n"
  "extra mort!\n";

char far MORTFIGHTINTROTEXT[] =
  "\n\n"
  "By some cosmic coincidence, the\n"
  "Dream Machine has brought your enemy,\n" 
  "Mortimer McMire, to the Land of\n"
  "Tuberia, as well!\n\n"
  "Managing to evade the two Tater Soldiers,\n"
  "he takes a pogo stick laying on the ground.\n"
  "As he approaches, he sees Commander Keen.\n"
  "But... like with this strange adventure,\n"
  "is Mortimer a figment of Keen's imagination,\n"
  "or is he the real deal?\n";

char far MORTENDING[] =
  "Sending his supposedly-imaginary\n"
  "arch-nemesis back to sleep,\n"
  "Keen returns to the Dream Machine,\n"
  "disconnecting it for good, its menace\n"
  "to no-longer threaten anyone.\n\n"
  "HOWEVER...\n\n"
  "Elsewhere, Mortimer awakes in sheer\n"
  "frustration, with baggy eyes.\n"
  "It seems he can't elude his\n"
  "less-intelligent counterpart,\n"
  "even in his dreams!\n";

char far SUPERSECRETMESSAGETEXT[] =
  "Ah...! So you found this secret message!\n"
  "Well, that deserves secrets, then!\n"
  "\n"
  "1. PLAY AS MORTIMER MCMIRE:\n"
  "\n"
  "If you would like to unlock Mortimer,\n"
  "try typing in \"GRANDINTELLECT\"\n"
  "in the high score table after\n"
  "beating him with Keen on Mountain Top.\n"
  "\n"
  "2. PLAY IN NIGHT MODE:\n"
  "\n"
  "If you would like to unlock night\n"
  "mode, try typing in \"DARKNESSTUBERIA\"\n"
  "in the high score table after\n"
  "defeating the boss with Keen or Mortimer.\n";

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

// for asm scaning of map planes
unsigned	mapx,mapy,mapxcount,mapycount,maptile,mapspot;

int			plummet;

int			objectcount;

objtype		objarray[MAXACTORS],*lastobj,*objfreelist;

unsigned			centerlevel;

int			oldtileleft,oldtiletop,oldtileright,oldtilebottom,oldtilemidx;
int			oldleft,oldtop,oldright,oldbottom,oldmidx;
int			leftmoved,topmoved,rightmoved,bottommoved,midxmoved;

int			topmove,bottommove,midxmove;

int			inactivateleft,inactivateright,inactivatetop,inactivatebottom;

int			fadecount;

int			oldattackcount;

boolean		bombspresent;
boolean		oldattacking;

void	CheckKeys (void);
void	CalcInactivate (void);
void	InitObjArray (void);
void	GetNewObj (boolean usedummy);
void	RemoveObj (objtype *gone);
void	ScanInfoPlane (void);
void	PatchWorldMap (void);
void	MarkTileGraphics (void);
void	FadeAndUnhook (void);
void	SetupGameLevel (boolean loadnow);
void	ScrollScreen (objtype *ob);
void	MoveObjVert (objtype *ob, int ymove);
void	MoveObjHoriz (objtype *ob, int xmove);
void	GivePoints (unsigned points);
void	ClipToEnds (objtype *ob);
void	ClipToEastWalls (objtype *ob);
void	ClipToWestWalls (objtype *ob);
void	ClipToWalls (objtype *ob);
void	ClipToSpriteSide (objtype *push, objtype *solid);
void	ClipToSprite (objtype *push, objtype *solid, boolean squish);
void 	ClipPushSprite(objtype *push, objtype *solid, boolean squish);
int		DoActor (objtype *ob,int tics);
void	StateMachine (objtype *ob);
void	NewState (objtype *ob,statetype near *state);
void	PlayLoop (void);
void	GameLoop (void);
void	EndGame (void);
void	ConfirmQuickQuit (void);
void	UserCheat (void);
void	DebugActivate (void);


/*
================
=
= UserCheat
=
================
*/

void UserCheat(void)
{
	int i;

	for (i=sc_A; i<=sc_Z; i++)	//Note: this does NOT check the keys in alphabetical order!
	{
		if (i != sc_B && i != sc_A && i != sc_T && Keyboard[i])
		{
			return;
		}
	}
	VW_FixRefreshBuffer ();
	US_CenterWindow(20, 7);
	PrintY += 2;
	if (gamestate.difficulty == gd_Grand)
	{
		if (mapon >= 23 && mapon <= 46)
			US_CPrintFar(MORTNIGHTUSERCHEATTEXT);
		else
			if (mapon == 18)
				US_CPrintFar(MORTKEYCARDUSERCHEATTEXT);
			else
				US_CPrintFar(MORTUSERCHEATTEXT);
	}
	else
	{
		if (mapon >= 23 && mapon <= 46)
			US_CPrintFar(NIGHTUSERCHEATTEXT);
		else
			if (mapon == 18)
				US_CPrintFar(KEYCARDUSERCHEATTEXT);
			else
				US_CPrintFar(USERCHEATTEXT);
	}
	VW_UpdateScreen();
	IN_Ack();
	RF_ForceRefresh();
	if (mapon >= 23 && mapon <= 46)
		gamestate.ammo = 99;
	else
		gamestate.flowerpowers = 99;
	gamestate.lives++;
	if (mapon == 18)
		gamestate.keycard=1;
	else
		gamestate.keys++;
}


/*
==================
=
= DebugActivate
=
==================
*/

void DebugActivate (void)
{
	VW_FixRefreshBuffer ();
	US_CenterWindow(20, 2);
	PrintY += 2;
	US_Print("Debug keys active");
	VW_UpdateScreen();
	IN_ClearKeysDown();
	IN_Ack();
	RF_ForceRefresh();
	debugok = true;
}


/*
==============================================
=
= ConfirmQuickQuit
=
= Basic conception partially taken from Wolf3d
=
==============================================
*/

//
// K1n9_Duk3's words:
//
// F7 is now an instant quit button (maybe it was in the older versions, too, I haven't checked).
// I thinks that's bad user interface design. It's way too easy to
// accidentally hit F7 when you're trying to hit another key, like F8 to toggle the score box.
// Good user interface should at least add a confirmation dialog. Keen 4-6 also had
// an instant quit option, but they used the combination CTRL-Q, which is far less likely to be
// pressed by accident (unless you're using an extremely weird control scheme).
//
// Szemigi's answer:
//
// Instead of using CTRL-Q from Keen 4-6 I've added a confirmation window that lets the player
// to choose whether they wanna instantly quit or not.
//

void ConfirmQuickQuit(void)
{
	VW_FixRefreshBuffer();
	US_CenterWindow (34,8);
	PrintY+= 8;
	US_CPrintFar(QUITCONFTEXT);
	VW_UpdateScreen();
	IN_ClearKeysDown();
	while(!Keyboard[sc_Y] && !Keyboard[sc_N] && !Keyboard[sc_Escape]) { IN_PumpEvents(); SDL_Delay(1); }
	{
		if (Keyboard[sc_Y])
		{
			Quit(nil);
		}
		else if (Keyboard[sc_N] || Keyboard[sc_Escape])
		{
			RF_ForceRefresh ();
			IN_ClearKeysDown();
			return;
		}
	}
	while(Keyboard[sc_Y] || Keyboard[sc_N] || Keyboard[sc_Escape]) { IN_PumpEvents(); SDL_Delay(1); }
	{
		if (Keyboard[sc_Y])
		{
			Quit(nil);
		}
		else if (Keyboard[sc_N] || Keyboard[sc_Escape])
		{
			RF_ForceRefresh ();
			IN_ClearKeysDown();
			return;
		}
	}
}


/*
=====================
=
= EndGame
=
=====================
*/

void EndGame (void)
{
	playstate = endgame;
}


/*
=====================
=
= CheckKeys
=
=====================
*/

void CheckKeys (void)
{
	unsigned	cities;


	if (screenfaded)			// don't do anything with a faded screen
		return;

//
// B-A-T cheat code
//
	if (Keyboard[sc_B] && Keyboard[sc_A] && Keyboard[sc_T])
	{
		UserCheat();
	}

//
// A+2 cheat
//
	if (Keyboard[sc_A] && Keyboard[sc_2] && Keyboard[statusscan])
	{
		DebugActivate();
	}

//
// space for status screen
//
	if (Keyboard[statusscan] || (GravisGamepad && GravisAction[ga_Status]))
	{
		StatusWindow ();
		IN_ClearKeysDown();
		RF_ForceRefresh();
		lasttimecount = TimeCount;
	}

//
// F1 to enter help screens
//

	if (LastScan == sc_F1)
	{
		StopMusic();
		HelpScreens();
		StartMusic(gamestate.mapon);
		if (showscorebox)
		{
			scoreobj->temp2 = -1;
			scoreobj->temp1 = -1;
			scoreobj->temp3 = -1;
			scoreobj->temp4 = -1;
		}
		RF_ForceRefresh();
	}

//
// instant quit
//

	if (LastScan == sc_F7)
	{
		ConfirmQuickQuit();
	}

//
// F8 to toggle score box (taken from BioMenace)
//
  	if (LastScan == sc_F8)
  	{
		showscorebox ^= true;

			if (!showscorebox && scoreobj->sprite)
			{
				RF_RemoveSprite(&scoreobj->sprite);
			}

    		if (showscorebox)
    		{
				*((int*)&scoreobj->temp1) = -1;
				scoreobj->temp3 = -1;
				scoreobj->temp4 = -1;
				FixScoreBox();
    		}

    	IN_ClearKeysDown();
    	RF_ForceRefresh();
    	lasttimecount = TimeCount;  // BUG: should be the other way around

	}

//
// boss key
//

	if (Keyboard[sc_F9])
	{
		// boss key: a black screen until Escape (there is no DOS prompt)
		VW_ClearVideo(BLACK);
		VW_SetScreen(displayofs,0);
		SD_MusicOff();
		IN_ClearKeysDown();
		while (LastScan != sc_Escape) { IN_PumpEvents(); SDL_Delay(1); }
		VW_SetScreenMode(GRMODE);
		RF_ForceRefresh();
		IN_ClearKeysDown();
		lasttimecount = TimeCount;	// BUG: should be the other way around
		SD_MusicOn();
	}

//
// pause key weirdness can't be checked as a scan code
//
	if (Paused)
	{
		SD_MusicOff();
		VW_FixRefreshBuffer ();
		US_CenterWindow (8,3);
		US_PrintCentered ("PAUSED");
		VW_UpdateScreen ();
		IN_Ack();
		RF_ForceRefresh ();
		Paused = false;
		SD_MusicOn();
	}

//
// ESC to enter control panel
//
	if (LastScan == sc_Escape)
	{
		VW_FixRefreshBuffer ();
		StopMusic();
		US_ControlPanel();
		RF_FixOfs();
		// SD_MusicOn; // Please check this, the function is not actually called with this syntax. (ktx58)
		US_CenterWindow (26,8);
		US_CPrint ("Loading");
		CA_CacheMarks("Reloading...", 0);		 // Inspired by Rescue Rover 2
		VW_UpdateScreen ();
//		VW_ClearVideo(BLACK);				 // ...
		IN_ClearKeysDown();
		
		if (!loadedgame)
			StartMusic(gamestate.mapon);
		
		if (!showscorebox && scoreobj->sprite)
		{
			RF_RemoveSprite(&scoreobj->sprite);
		}
		if (showscorebox)
		{
			scoreobj->temp2 = -1;
			scoreobj->temp1 = -1;
			scoreobj->temp3 = -1;
			scoreobj->temp4 = -1;
			FixScoreBox();
		}
		IN_ClearKeysDown();
		if (restartgame)
			playstate = resetgame;
		else if (!loadedgame)
			RF_ForceRefresh();		// don't refresh if loading a new game

		lasttimecount = TimeCount;
	}

//
// F10-? debug keys
//
	if (debugok && Keyboard[sc_F10])
	{
		if (DebugKeys())
		{
			RF_ForceRefresh();
			lasttimecount = TimeCount;
		}
	}

}

//===========================================================================


/*
=======================
=
= CalcInactivate
=
=======================
*/

void CalcInactivate (void)
{
	originxtilemax = originxtile+PORTTILESWIDE-1;
	originytilemax = originytile+PORTTILESHIGH-1;

	inactivateleft = originxtile-INACTIVATEDIST;
	if (inactivateleft < 0)
		inactivateleft = 0;
	inactivateright = originxtilemax+INACTIVATEDIST;
	inactivatetop = originytile-INACTIVATEDIST;
	if (inactivatetop < 0)
		inactivatetop = 0;
	inactivatebottom = originytilemax+INACTIVATEDIST;
}


//===========================================================================


/*
#############################################################################

					The objarray data structure

#############################################################################

Objarray containt structures for every actor currently playing.  The structure
is accessed as a linked list starting at *player, ending when ob->next ==
NULL.  GetNewObj inserts a new object at the end of the list, meaning that
if an actor spawn another actor, the new one WILL get to think and react the
same frame.  RemoveObj unlinks the given object and returns it to the free
list, but does not damage the objects ->next pointer, so if the current object
removes itself, a linked list following loop can still safely get to the
next element.

<backwardly linked free list>

#############################################################################
*/


/*
=========================
=
= InitObjArray
=
= Call to clear out the entire object list, returning them all to the free
= list.  Allocates a special spot for the player.
=
=========================
*/

void InitObjArray (void)
{
	int	i;

	for (i=0;i<MAXACTORS;i++)
	{
		objarray[i].prev = &objarray[i+1];
		objarray[i].next = NULL;
	}

	objarray[MAXACTORS-1].prev = NULL;

	objfreelist = &objarray[0];
	lastobj = NULL;

	objectcount = 0;

//
// give the player and score the first free spots
//
	GetNewObj (false);
	player = new;
	GetNewObj (false);
	scoreobj = new;
}

//===========================================================================

/*
=========================
=
= GetNewObj
=
= Sets the global variable new to point to a free spot in objarray.
= The free spot is inserted at the end of the liked list
=
= When the object list is full, the caller can either have it bomb out ot
= return a dummy object pointer that will never get used
=
=========================
*/

void GetNewObj (boolean usedummy)
{
	if (!objfreelist)
	{
		if (usedummy)
		{
			new = &dummyobj;
			return;
		}
		Quit ("GetNewObj: No free spots in objarray!");
	}

	new = objfreelist;
	objfreelist = new->prev;
	memset (new,0,sizeof(*new));

	if (lastobj)
		lastobj->next = new;
	new->prev = lastobj;	// new->next is allready NULL from memset

	new->active = yes;
	new->needtoclip = true;
	lastobj = new;

	objectcount++;
}

//===========================================================================

/*
=========================

=
= IndexOfObjInLinkedList, ObjAtIndexInLinkedList, GetFirstObjectWithClass
=
= For a workaround that fixes issues related to object pointers
= in saved games.
=
=========================

*/

unsigned IndexOfObjectInLinkedList(objtype *objptr)
{
	objtype *obj;
	unsigned counter = 0;

	for (obj = player; obj; obj = obj->next, counter++)
	{
		if (obj == objptr) 
			return counter;
	}

	// For debuggigng, may be removed later
	Quit("IndexOfObjInLinkedList: object not found");
	return 0; // To avoid "return value" warnings

}

objtype * ObjectAtIndexInLinkedList(unsigned index)
{
	objtype *obj;
	unsigned counter = 0;

	for (obj = player; obj; obj = obj->next, counter++)
	{
		if (counter == index) 
			return obj;
	}

	// For debuggigng, may be removed later
	Quit("ObjAtIndexInLinkedList: object not found");
	return NULL; // To avoid "return value" warnings

}

objtype * GetFirstObjectWithClass(classtype obclass)
{
	objtype *obj;

	for (obj = player; obj; obj = obj->next)
	{
		if (obj->obclass == obclass) 
			return obj;
	}

	// Return NULL if not found, not considered an error
	return NULL;

}


//===========================================================================

/*
=========================
=
= RemoveObj
=
= Add the given object back into the free list, and unlink it from it's
= neighbors
=
=========================
*/

void RemoveObj (objtype *gone)
{
	if (gone == player)
		Quit ("RemoveObj: Tried to remove the player!");

//
// erase it from the refresh manager
//
	RF_RemoveSprite (&gone->sprite);

//
// fix the next object's back link
//
	if (gone == lastobj)
		lastobj = (objtype *)gone->prev;
	else
		gone->next->prev = gone->prev;

//
// fix the previous object's forward link
//
	gone->prev->next = gone->next;

//
// add it back in to the free list
//
	gone->prev = objfreelist;
	objfreelist = gone;
}

//===========================================================================


void HandleInfo (void)
{
	objtype *ob;

	switch (maptile)
	{
	case 31:
		bombspresent = true;
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 32:
		SpawnBonus(mapx,mapy,maptile-21);
		new->active = false;
		break;
	case 33:
		SpawnDoor(mapx,mapy);
		new->active = false;
		break;
	case 34:
		SpawnEggplant(mapx,mapy);
		new->active = false;
		lumpneeded[EGGPLANT_LUMP] = true;
		break;	
	case 35:
		if (gamestate.difficulty != gd_Grand && mapon == 21)
		{
			SpawnMort(mapx,mapy);
			new->active = false;
			lumpneeded[MORT_LUMP] = true;
			lumpneeded[BLUEFLOWERPOWER_LUMP] = true;
		}
		break;	
	case 36:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnHorseradish(mapx,mapy);
			new->active = false;
			lumpneeded[HORSERAD_LUMP] = true;
		}
		else
		{
			SpawnBean(mapx,mapy);
			new->active = false;
			lumpneeded[BEAN_LUMP] = true;
		}
		break;
	case 37:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnParsnip(mapx,mapy);
			new->active = false;
			lumpneeded[PARSNIP_LUMP] = true;
		}
		else
		{
			SpawnBaberry(mapx,mapy);
			new->active = false;
			lumpneeded[BABERRY_LUMP] = true;
		}
		break;
	case 38:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnTurnip(mapx,mapy);
			new->active = false;
			lumpneeded[TURNIP_LUMP] = true;
		}
		else
		{
			SpawnCorn(mapx,mapy);
			new->active = false;
			lumpneeded[CORN_LUMP] = true;		
		}
		break;
	case 39:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnCauliflower(mapx,mapy);
			new->active = false;
			lumpneeded[CAULI_LUMP] = true;
		}
		else
		{
			SpawnMosquito(mapx,mapy);
			new->active = false;
			lumpneeded[MOSQUITO_LUMP] = true;
		}
		break;
	case 40:
		SpawnRhubarb(mapx,mapy);
		new->active = false;
		lumpneeded[RHUBARB_LUMP] = true;
		break;
	case 41:
		SpawnBrocco(mapx,mapy);
		new->active = false;
		lumpneeded[BROCCO_LUMP] = true;
		break;
	case 42:
		SpawnTomat(mapx,mapy);
		new->active = false;
		if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Hard)
		{
			lumpneeded[BTOMAT_LUMP] = true;
		}
		else
		{
			lumpneeded[TOMAT_LUMP] = true;
		}
		break;
	case 43:
		SpawnCarrot(mapx,mapy);
		new->active = false;
		lumpneeded[CARROT_LUMP] = true;
		break;
	case 44:
		SpawnPumpkin(mapx,mapy);
		new->active = false;
		lumpneeded[PUMPKIN_LUMP] = true;
		break;
	case 45:
		SpawnAspar(mapx,mapy);
		new->active = false;
		lumpneeded[ASPAR_LUMP] = true;
		break;
	case 46:
		SpawnGrape(mapx,mapy);
		new->active = false;
		lumpneeded[GRAPE_LUMP] = true;
		break;
	case 47:
		SpawnTater(mapx,mapy);
		new->active = false;
		if (gamestate.difficulty == gd_Easy)
		{
			lumpneeded[TATERII_LUMP] = true;
		}
		lumpneeded[TATER_LUMP] = true;
		break;
	case 48:
		SpawnCart(mapx,mapy);
		lumpneeded[CART_LUMP] = true;
		break;
	case 49:
		SpawnFrenchy(mapx,mapy);
		new->active = false;
		lumpneeded[FRENCHY_LUMP] = true;
		break;
	case 50:
	case 51:
	case 52:
	case 53:
		SpawnMelon(mapx,mapy,maptile-50);
		new->active = false;
		if (mapon >= 23 && mapon <= 46)
		{
			lumpneeded[CANTALOUPE_LUMP] = true;
		}
		else
		{
			lumpneeded[MELON_LUMP] = true;
		}
		break;
	case 54:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnMachine(mapx,mapy);
			new->active = false;
			lumpneeded[MACHINE_LUMP] = true;
		}
		else
		{
			SpawnGuardRobot(mapx,mapy);
			new->active = false;
			lumpneeded[GUARDROBOT_LUMP] = true;
		}
		break;
	case 55:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnMushroom(mapx,mapy);
			new->active = false;
			lumpneeded[MUSHROOM_LUMP] = true;
		}
		else
		{
			SpawnKumquat(mapx,mapy);
			new->active = false;
			lumpneeded[KUMQUAT_LUMP] = true;
		}
		break;
	case 56:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnBrussel(mapx,mapy);
			new->active = false;
			lumpneeded[BRUSSEL_LUMP] = true;
		}
		else
		{
			SpawnSteelCart(mapx,mapy);
			new->active = false;
			lumpneeded[STEELCART_LUMP] = true;
		}
		break;
	case 57:
		SpawnSquasher(mapx,mapy);
		new->active = false;
		if (mapon >= 23 && mapon <= 46)
		{
			lumpneeded[ALTSQUASH_LUMP] = true;
		}
		lumpneeded[SQUASH_LUMP] = true;
		break;
	case 58:
		SpawnApel(mapx,mapy);
		new->active = false;
		lumpneeded[APEL_LUMP] = true;
		break;
	case 59:
		SpawnPeaPod(mapx,mapy);
		new->active = false;
		lumpneeded[PEA_LUMP] = true;
		break;
	case 60:
		SpawnPeaBrain(mapx,mapy);
		new->active = false;
		lumpneeded[PEA_LUMP] = true;
		break;
	case 61:
		if (mapon == 18)
		{
			SpawnMetalDoor(mapx,mapy);
			new->active = false;
		}
		else
		{
			SpawnBoobus(mapx,mapy);
			if (gamestate.difficulty == gd_Easy)
			{
				lumpneeded[BUSTLING_LUMP] = true;
			}
			else if (gamestate.difficulty == gd_Hard)
			{
				lumpneeded[SBOOBUS_LUMP] = true;
			}
			else if (gamestate.difficulty == gd_Keen)
			{
				lumpneeded[PBOOBUS_LUMP] = true;
			}
			else
			{
				lumpneeded[BOOBUS_LUMP] = true;
			}
		}
		break;
	case 62:
		if (mapon >= 0 && mapon <= 22)
		{
			SpawnCelery(mapx,mapy);
			new->active = false;
			lumpneeded[CELERY_LUMP] = true;
		}
		else
		{
			SpawnKid(mapx,mapy);
			new->active = false;
			lumpneeded[KID_LUMP] = true;
		}
		break;
	case 65:
		if (gamestate.difficulty != gd_Grand)
		{
			SpawnCameo1(mapx,mapy);
			new->active = false;
			lumpneeded[CAMEOS_LUMP] = true;
		}
		break;		
	case 66:
		SpawnMCart(mapx,mapy);
		lumpneeded[MCART_LUMP] = true;
		break;
	case 67:
		if (mapon >= 23 && mapon <= 46)
		{
			SpawnBomb(mapx,mapy);
			new->active = false;
			bombspresent = true;
		}
		else
		{
			SpawnKeycard(mapx,mapy);
			new->active = false;
			lumpneeded[KEYCARD_LUMP] = true;
		}
		break;
	case 68:
		if (gamestate.difficulty != gd_Grand)
		{
			SpawnCameo2(mapx,mapy);
			new->active = false;
			lumpneeded[CAMEOS_LUMP] = true;
		}
		break;
	case 70:
	case 71:
		if (gamestate.difficulty != gd_Grand)
		{
			SpawnKeen(mapx,mapy,maptile == 70 ? 1 : -1);
			if (DemoMode)
			{
				lumpneeded[DEMO_LUMP] = true;
			}
			lumpneeded[KEEN_LUMP] = true;
			if (mapon >= 23 && mapon <= 46)
			{
				lumpneeded[KEEN2_LUMP] = true;
				lumpneeded[KEENEXIT_LUMP] = true;
				lumpneeded[KEENLOWSHOOT_LUMP] = true;
			}
			lumpneeded[INLEVEL_LUMP] = true;
		}
		break;
	case 531:
	case 532:
		if (gamestate.difficulty == gd_Grand)
		{
			SpawnKeen(mapx,mapy,maptile == 531 ? 1 : -1);
			if (DemoMode)
			{
				lumpneeded[DEMO_LUMP] = true;
			}
			lumpneeded[MORT_LUMP] = true;
			if (mapon >= 23 && mapon <= 46)
			{
				lumpneeded[MORT2_LUMP] = true;
				lumpneeded[MORTEXIT_LUMP] = true;
				lumpneeded[MORTLOWSHOOT_LUMP] = true;
			}
			lumpneeded[INLEVEL_LUMP] = true;
		}
	case 575:
		if (gamestate.difficulty != gd_Grand && mapon == 0)
		{
			SpawnWorldKeen(mapx,mapy);
			lumpneeded[WORLDKEEN_LUMP] = true;
			lumpneeded[INLEVEL_LUMP] = true;
		}
		break;
	case 651:
		if (mapon >= 23 && mapon <= 46)
		{
			SpawnGargoyle(mapx,mapy);
			new->active = false;
			lumpneeded[GARGOYLE_LUMP] = true;
		}
		break;
	case 698:
		if (mapon >= 23 && mapon <= 46)
		{
			SpawnPear(mapx,mapy);
			new->active = false;
			lumpneeded[PEAR_LUMP] = true;
		}
		break;
	case 712:
		if (mapon >= 23 && mapon <= 46)
		{
			SpawnBeet(mapx,mapy);
			new->active = false;
			lumpneeded[BEET_LUMP] = true;
		}
		break;
	case 759:
		SpawnRover(mapx,mapy);
		new->active = false;
		lumpneeded[ROVER_LUMP] = true;
		lumpneeded[MORTEXIT_LUMP] = true;
		lumpneeded[KEENEXIT_LUMP] = true;
		break;		
	case 760:
		if (gamestate.difficulty == gd_Grand && mapon == 10)
		{
			SpawnPinkOwl2(mapx,mapy);
		}
		else if (mapon == 22 || gamestate.difficulty == gd_Grand && mapon == 21)
		{
			SpawnPinkOwl3(mapx,mapy);
		}
		else if (mapon == 46)
		{
			SpawnPinkOwl(mapx,mapy);
		}
		else
		{
			return;
		}
		new->active = false;
		lumpneeded[PINKOWL_LUMP] = true;
		break;
	case 799:
		if (gamestate.difficulty == gd_Grand && mapon == 0)
		{
			SpawnWorldKeen(mapx,mapy);
			lumpneeded[WORLDMORT_LUMP] = true;
			lumpneeded[INLEVEL_LUMP] = true;
		}
		break;
	}

	if (new->active != allways)
		new->active = false;
}

/*
==========================
=
= ScanInfoPlane
=
= Spawn all actors and mark down special places
=
==========================
*/

void ScanInfoPlane (void)
{
	unsigned	x,y,i,j;
	int			tile;
	uint16_t		*start;

	InitObjArray();			// start spawning things with a clean slate

	memset (lumpneeded,0,sizeof(lumpneeded));

//
// This doesn't really need to be in asm.  I thought it was a bottleneck,
// but I was wrong...
//

	start = mapsegs[2];
	for (mapy = 0; mapy < mapheight; mapy++)
		for (mapx = 0; mapx < mapwidth; mapx++)
		{
			tile = *start++;
			if (!tile)
				continue;
			maptile = tile;
			HandleInfo ();
		}

	for (i=0;i<NUMLUMPS;i++)
		if (lumpneeded[i])
			for (j=lumpstart[i];j<=lumpend[i];j++)
				CA_MarkGrChunk(j);

	if (gamestate.difficulty == gd_Grand)
	{
		CA_MarkGrChunk(SCOREBOX2SPR);	// K1n9_Duk3 addition
	}
	else
	{
		CA_MarkGrChunk(SCOREBOXSPR);	// K1n9_Duk3 addition
	}
}

//===========================================================================


/*
==========================
=
= PatchWorldMap
=
= Takes out blocking squares and puts in dones
=
==========================
*/

void PatchWorldMap (void)
{
	unsigned	size,spot,info,foreground;

	size = mapwidth*mapheight;
	spot = 0;
	do
	{
		info = *(mapsegs[2] + spot);
		// finished a city here?
		if (info>=1 && info<=20 && gamestate.leveldone[info-0])
		{
			*(mapsegs[2] + spot) = 0;
			foreground = *(mapsegs[1] + spot);
			if (foreground == 130)
				*(mapsegs[1]+spot) = 0;	// not blocking now
			else if (foreground == 90)
			{
			// plant done flag
				*(mapsegs[1]+spot) = 133;
				*(mapsegs[1]+(spot-mapwidth-1)) = 131;
				*(mapsegs[1]+(spot-mapwidth)) = 132;
			}
		}
		spot++;
	} while (spot<size);
}

//===========================================================================

/*
==========================
=
= FadeAndUnhook
=
= Latch this onto the refresh so the screen only gets faded in after two
= refreshes.  This lets all actors draw themselves to both pages before
= fading the screen in.
=
==========================
*/

void FadeAndUnhook (void)
{
	if (++fadecount==2)
	{
		RF_ForceRefresh();
		VW_FadeIn ();
		RF_SetRefreshHook (NULL);
		// K1n9_Duk3 fix:
		//lasttimecount = TimeCount;	// don't adaptively time the fade
		TimeCount = lasttimecount;	// this is how it's done in Keen4-6
	}
}

//===========================================================================

// K1n9_Duk3 fix:

 void PurgeSpritesAndTiles(void)
{
	register int i;

	// purge all sprites except the paddle war sprites
	// (in case the menu still needs them)
	for (i=DEMOPLAQUESPR; i<STARTSPRITES+NUMSPRITES; i++)
	{
		if (grsegs[i])
		{
			MM_SetPurge(&grsegs[i], 1);
		}
	}
	// purge all tiles (unmasked and masked)
	for (i=STARTTILE16; i<STARTTILE16M+NUMTILE16M; i++)
	{
		if (grsegs[i])
		{
			MM_SetPurge(&grsegs[i], 1);
		}
	}
}

/*
==========================
=
= SetupGameLevel
=
= Load in map mapon and cache everything needed for it
=
==========================
*/

void	SetupGameLevel (boolean loadnow)
{
	long	orgx,orgy;
//
// randomize if not a demo
//
	if (DemoMode)
	{
		US_InitRndT(false);
		if (gamestate.difficulty == gd_Grand)
		{
			gamestate.difficulty = gd_Grand;
		}
		else
		{
			gamestate.difficulty = gd_Normal;
		}
	}
	else
	{
		US_InitRndT(true);
	}
	
	bombspresent = false;
	mortinlevel = NULL;  // clear the "Mort in level" variable

	// K1n9_Duk3 fix: make sure there is some memory available so CA_CacheMap won't fail
	PurgeSpritesAndTiles();	// can't just call CA_SetGrPurge here - menu might still need some graphics chunks
	StopMusic();

//
// load the level header and three map planes
//
	CA_CacheMap (gamestate.mapon);

//
// let the refresh manager set up some variables
//
	RF_NewMap ();

//
// decide which graphics are needed and spawn actors
//
	CA_ClearMarks ();

	if (!mapon)
		PatchWorldMap ();

	if (mapon!=47)			// map 47 is the title screen
		ScanInfoPlane ();
	RF_MarkTileGraphics ();

//
// have the caching manager load and purge stuff to make sure all marks
// are in memory
//
	if (loadnow)
	{
		if (DemoMode)
		{
			CA_CacheMarks("DEMO", 0);			
		}
		else 
		{
			if (bombspresent)
			{
				if (gamestate.difficulty == gd_Hard || gamestate.difficulty == gd_Keen || gamestate.difficulty == gd_Grand)
				{
					bombspresent = false;
				}
				else
				{
					VW_FixRefreshBuffer ();
					US_DrawWindow (10,1,20,2);
					if (mapon >= 23 && mapon <= 46)
      						US_PrintCentered ("Fear the Boobus Bombs!");
					else
						US_PrintCentered ("Boobus Bombs Near!");
					VW_UpdateScreen ();
				}
			}
			_fstrcpy(str, levelnames[mapon]);
			CA_CacheMarks(str, 0);
		}
	}

	if (mapon!=47 && loadnow)			// map 47 is the title level
	{
		VW_FadeOut ();
		fadecount = 0;
		RF_SetRefreshHook (&FadeAndUnhook);
		SpawnScore ();

//
// start the initial view position to center the player
//
		orgx = (long)player->x - (150<<G_P_SHIFT);
		orgy = (long)player->y-(84<<G_P_SHIFT);
		if (orgx<0)
			orgx=0;
		if (orgy<0)
			orgy=0;

		RF_NewPosition (orgx,orgy);
		CalcInactivate ();
	}
}

/*

// PushObj: NEW, From K1n9_Duk3's source code, slightly modified
// Moves the current object xtry/ytry units, clipping to walls

/*
============================
=
= PushObj
=
============================
*/

void PushObj(objtype *ob)
{
	unsigned oldx, oldy;
	unsigned oldleft, oldtop, oldright, oldbottom, oldmidx;
	spritetabletype far *shape;
	
	oldx = ob->x;
	oldy = ob->y;

//
// move the shape
//
	ob->x += ob->xmove;
	ob->y += ob->ymove;

	ob->needtoreact = true;

	if (!ob->shapenum)				// can't get a hit rect with no shape!
	{
		return;
	}


	shape = &spritetable[ob->shapenum-STARTSPRITES];
#if 0
	oldtileright = ob->tileright;
	oldtiletop = ob->tiletop;
	oldtileleft = ob->tileleft;
	oldtilebottom = ob->tilebottom;
	oldtilemidx = ob->tilemidx;
#endif
	oldright = ob->right;
	oldtop = ob->top;
	oldleft = ob->left;
	oldbottom = ob->bottom;
	oldmidx = ob->midx;

	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right-ob->left)/2;

	ob->tileleft = (ob->left)>>(G_T_SHIFT);
	ob->tileright = (ob->right)>>(G_T_SHIFT);
	ob->tiletop = (ob->top)>>(G_T_SHIFT);
	ob->tilebottom = (ob->bottom)>>(G_T_SHIFT);
	ob->tilemidx = (ob->midx)>>(G_T_SHIFT);

	if (ob->needtoclip)
	{
		leftmoved = ob->left - oldleft;
		rightmoved = ob->right - oldright;
		topmoved = ob->top - oldtop;
		bottommoved = ob->bottom - oldbottom;
		midxmoved = ob->midx - oldmidx;

		ClipToEnds(ob);
#if 0
		ClipToSides(ob);
#endif
	}

	ob->xmove = ob->xmove + (ob->x - oldx);
	ob->ymove = ob->ymove + (ob->y - oldy);
}

// ClipToSpriteTop: NEW, From K1n9_Duk3's source code

/*
============================
=
= ClipToSpriteTop
=
============================
*/

void ClipToSpriteTop(objtype *push, objtype *solid)
{
	int temp, ymove, bottominto;

	//
	// amount the push shape can be pushed
	//
	ymove = push->ymove - solid->ymove;

	//
	// amount it is inside
	//
	bottominto = push->bottom - solid->top;

	if (bottominto >= 0 && bottominto <= ymove)
	{
		
		if (push == player)
		{
			riding = solid;
		}
		push->ymove = -bottominto;
		temp = push->state->pushtofloor;
		push->state->pushtofloor = false;
		PushObj(push);
		push->state->pushtofloor = temp;
		if (!push->hitsouth)
		{
			push->hitnorth = 25;
		}
	}
}

// RideVerticalPlatform: NEW, Based on HandleRiding in K1n9_Duk3's
//   source code

/*
============================
=
= RideVerticalPlatform
=
============================
*/

void RideVerticalPlatform(objtype *ob)
{
	objtype *plat;

	plat = riding;
	if (ob->right < plat->left || ob->left > plat->right)
	{
		riding = NULL;
		onmushroom = false;
	}
	else if (ob->ymove < 0)
	{
		riding = NULL;
		onmushroom = false;
		if (plat->ymove < 0)
		{
			ob->xmove = 0;
			ob->ymove = plat->ymove;
			PushObj(ob);
		}
	}
	else
	{
		ob->xmove = plat->xmove;
		ob->ymove = plat->top - ob->bottom - 16;
		PushObj(ob);
			
		// NOTE (ktx58): Code specific to horizontal platforms has been removed        
		if (ob->hitsouth)
		{
			riding = NULL;
			onmushroom = false;
		}
		else
		{
			ob->hitnorth = 25;
		}
	}
}

//===========================================================================

/*
==================
=
= CenterActor
=
==================
*/

void CenterActor(objtype *ob)
{
	int orgx, orgy;

	centerlevel = 140;
	if (ob->x < 152*PIXGLOBAL)
	{
		orgx = 0;
	}
	else
	{
		orgx = ob->x - 152*PIXGLOBAL;
	}
	if (mapon == 0)
	{
		if (ob->y < 80*PIXGLOBAL)
		{
			orgy = 0;
		}
		else
		{
			orgy = ob->y - 80*PIXGLOBAL;
		}
	}
	else
	{
		if (ob->bottom < 140*PIXGLOBAL)
		{
			orgy = 0;
		}
		else
		{
			orgy = ob->bottom - 140*PIXGLOBAL;
		}
	}
	if (!scorescreenkludge)
	{
		RF_NewPosition(orgx, orgy);
	}

//
// update limits for onscreen and inactivate checks
//
	originxtilemax = (originxtile + PORTTILESWIDE) - 1;
	originytilemax = (originytile + PORTTILESHIGH) - 1;
	inactivateleft = originxtile - INACTIVATEDIST;
	if (inactivateleft < 0)
	{
		inactivateleft = 0;
	}
	inactivateright = originxtilemax + INACTIVATEDIST;
	if (inactivateright < 0)
	{
		inactivateright = 0;
	}
	inactivatetop = originytile - INACTIVATEDIST;
	if (inactivatetop < 0)
	{
		inactivatetop = 0;
	}
	inactivatebottom = originytilemax + INACTIVATEDIST;
	if (inactivatebottom < 0)
	{
		inactivatebottom = 0;
	}
}

//===========================================================================

/*
==================
=
= WorldScrollScreen
=
= Scroll if Keen is nearing an edge
=
==================
*/

void WorldScrollScreen(objtype *ob)
{
	int xscroll, yscroll;

	if (keenkilled)
		return;

	if (ob->left < originxmin || ob->right > originxmax + 320*PIXGLOBAL)
	{
		if (mapon != 0)
			playstate = levelcomplete;
		return;
	}

	if (!plummet && ob->bottom > originymax + 13*TILEGLOBAL)
	{
		if (mapon != 0)
		{
			SD_PlaySound(PLUMMETSND);
			godmode = false;
			plummet = true;
			// Note (ktx58): On "Keen" and "Grand" difficulty, "plummet" is re-used  as a
			// "level must be restarted" (e.g. player has hit the bottom of 
			// the level) variable.
			KillKeen();
			return;
		}
	}

	if (ob->left < originxglobal + 9*TILEGLOBAL)
	{
		xscroll = ob->left - (originxglobal + 9*TILEGLOBAL);
	}
	else if (ob->right > originxglobal + 12*TILEGLOBAL)
	{
		xscroll = ob->right + 16 - (originxglobal + 12*TILEGLOBAL);
	}
	else
	{
		xscroll = 0;
	}

	if (ob->top < originyglobal + 5*TILEGLOBAL)
	{
		yscroll = ob->top - (originyglobal + 5*TILEGLOBAL);
	}
	else if (ob->bottom > originyglobal + 7*TILEGLOBAL)
	{
		yscroll = ob->bottom - (originyglobal + 7*TILEGLOBAL);
	}
	else
	{
		yscroll = 0;
	}

	if (!xscroll && !yscroll)
		return;

//
// don't scroll more than one tile per frame
//
	if (xscroll >= 0x100)
	{
		xscroll = 0xFF;
	}
	else if (xscroll <= -0x100)
	{
		xscroll = -0xFF;
	}
	if (yscroll >= 0x100)
	{
		yscroll = 0xFF;
	}
	else if (yscroll <= -0x100)
	{
		yscroll = -0xFF;
	}

	RF_Scroll(xscroll, yscroll);
	scoreobj->needtoreact = true;

//
// update limits for onscreen and inactivate checks
//
	originxtilemax = (originxtile + PORTTILESWIDE) - 1;
	originytilemax = (originytile + PORTTILESHIGH) - 1;
	inactivateleft = originxtile - INACTIVATEDIST;
	if (inactivateleft < 0)
	{
		inactivateleft = 0;
	}
	inactivateright = originxtilemax + INACTIVATEDIST;
	if (inactivateright < 0)
	{
		inactivateright = 0;
	}
	inactivatetop = originytile - INACTIVATEDIST;
	if (inactivatetop < 0)
	{
		inactivatetop = 0;
	}
	inactivatebottom = originytilemax + INACTIVATEDIST;
	if (inactivatebottom < 0)
	{
		inactivatebottom = 0;
	}
}

//==========================================================================

/*
===============
=
= ScrollScreen
=
= Scroll if Keen is nearing an edge
= Set playstate to levelcomplete
=
===============
*/
void ScrollScreen (objtype *ob)
{
	int xscroll, yscroll, pix, speed;
	unsigned bottom;

	if (keenkilled)
		return;

//
// walked off edge of map?
//
	if (ob->left < originxmin || ob->right > originxmax + 320*PIXGLOBAL)
	{
		if (mapon == 18)
		{
			gamestate.keycard--;
		}
		playstate = levelcomplete;
		return;
	}

//
// fallen off bottom of world?
//

	if (!plummet && ob->bottom > originymax + 13*TILEGLOBAL)
	{
		SD_PlaySound(PLUMMETSND);
		godmode = false;
		plummet = true;
		// Note (ktx58): On "Keen" and "Grand" difficulty, "plummet" is re-used  as a
		// "level must be restarted" (e.g. player has hit the bottom of 
		// the level) variable.
		KillKeen();
		return;
	}

	xscroll=yscroll=0;

	if (ob->x < originxglobal + SCROLLWEST)
	{
		xscroll = ob->x - (originxglobal + SCROLLWEST);
	}
	else if (ob->x > originxglobal + SCROLLEAST)
	{
		xscroll = ob->x - (originxglobal + SCROLLEAST);
	}

	if (ob->state == &s_keenlookup)
	{
		if (centerlevel+tics > 167)
		{
			pix = 167-centerlevel;
		}
		else
		{
			pix = tics;
		}
		centerlevel += pix;
		yscroll = CONVERT_PIXEL_TO_GLOBAL(-pix);
	}
	else if (ob->state == &s_keenduck)
	{
		if (centerlevel-tics < 33)
		{
			pix = centerlevel + -33;
		}
		else
		{
			pix = tics;
		}
		centerlevel -= pix;
		yscroll = CONVERT_PIXEL_TO_GLOBAL(pix);
	}

  	if (ob->hitnorth || !ob->needtoclip)
	{
		// Note by lethal_guitar: There's some difference elsewhere in the Keen
		// Dreams version of the code that I haven't figured out yet, which causes
		// this bit here to have undesired results - switching between standing and
		// walking will scroll the screen up & down, climbing on poles results in
		// very jerky scrolling etc. Not doing this is most likely not quite
		// correct, but gives much better results overall so let's do it like this
		// for now.
		// Needs more investigation to fully understand what's actually going on.
		/*yscroll += ob->ymove;*/

		bottom = originyglobal + yscroll + CONVERT_PIXEL_TO_GLOBAL(centerlevel);
		if (ob->bottom == bottom)
		{
			// player didn't move, no additional scrolling
		}
		else
		{
			if (ob->bottom < bottom)
			{
				pix = bottom - ob->bottom;
			}
			else
			{
				pix = ob->bottom - bottom;
			}
			speed = CONVERT_PIXEL_TO_GLOBAL(pix) >> 7;
			if (speed > 0x30)
			{
				speed = 0x30;
			}
			speed *= tics;
			if (speed < 0x10)
			{
				if (pix < 0x10)
				{
					speed = pix;
				}
				else
				{
					speed = 0x10;
				}
			}
			if (ob->bottom < bottom)
			{
				yscroll -= speed;
			}
			else
			{
				yscroll += speed;
			}
		}
	}
	else
	{
		centerlevel = 140;
	}

	pix = (ob->bottom-32*PIXGLOBAL)-(originyglobal+yscroll);
	if (pix < 0)
	{
		yscroll += pix;
	}
	pix = (ob->bottom+32*PIXGLOBAL)-(originyglobal+yscroll+200*PIXGLOBAL);
	if (pix > 0)
	{
		yscroll += pix;
	}

//
// don't scroll more than one tile per frame
//
	if (xscroll >= 0x100)
	{
		xscroll = 0xFF;
	}
	else if (xscroll <= -0x100)
	{
		xscroll = -0xFF;
	}
	if (yscroll >= 0x100)
	{
		yscroll = 0xFF;
	}
	else if (yscroll <= -0x100)
	{
		yscroll = -0xFF;
	}

	if (xscroll || yscroll)
	{
		RF_Scroll(xscroll, yscroll);
		CalcInactivate();
		scoreobj->needtoreact = true;
	}
}


//==========================================================================

/*
====================
=
= GivePoints
=
= Grants extra men at 20k,40k,80k,160k,320k
=
====================
*/

void GivePoints (unsigned points)
{
	gamestate.score += points;
	if (gamestate.score >= gamestate.nextextra)
	{
		SD_PlaySound (EXTRAKEENSND);
		gamestate.lives++;
		gamestate.nextextra*=2;
	}
}


//==========================================================================

/*
====================
=
= MoveObjVert
=
====================
*/

void MoveObjVert (objtype *ob, int ymove)
{
	ob->y += ymove;
	ob->top += ymove;
	ob->bottom += ymove;
	ob->tiletop = ob->top >> G_T_SHIFT;
	ob->tilebottom = ob->bottom >> G_T_SHIFT;
}


/*
====================
=
= MoveObjHoriz
=
====================
*/

void MoveObjHoriz (objtype *ob, int xmove)
{
	ob->x += xmove;
	ob->left += xmove;
	ob->right += xmove;
	ob->tileleft = ob->left >> G_T_SHIFT;
	ob->tileright = ob->right >> G_T_SHIFT;
}


/*
=============================================================================

					Actor to tile clipping rouitnes

=============================================================================
*/

// walltype / x coordinate (0-15)

int	wallclip[8][16] = {			// the height of a given point in a tile
{ 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256},
{   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
{   0,0x08,0x10,0x18,0x20,0x28,0x30,0x38,0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78},
{0x80,0x88,0x90,0x98,0xa0,0xa8,0xb0,0xb8,0xc0,0xc8,0xd0,0xd8,0xe0,0xe8,0xf0,0xf8},
{   0,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0},
{0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,0x38,0x30,0x28,0x20,0x18,0x10,0x08,   0},
{0xf8,0xf0,0xe8,0xe0,0xd8,0xd0,0xc8,0xc0,0xb8,0xb0,0xa8,0xa0,0x98,0x90,0x88,0x80},
{0xf0,0xe0,0xd0,0xc0,0xb0,0xa0,0x90,0x80,0x70,0x60,0x50,0x40,0x30,0x20,0x10,   0}
};

boolean playerkludgeclipcancel;

// assignment within ifs are used heavily here, so turn off the warning
#pragma warn -pia

//int xtry, ytry;	// K1n9_Duk3 mod 2025-05-26: these aren't used anymore

/*
===========================
=
= ClipToEnds
=
===========================
*/

void ClipToEnds (objtype *ob)
{
	uint16_t	 *map,tile,facetile,info,wall;
	int	leftpix,rightpix,midtiles,toppix,bottompix;
	int	x,y,clip,move,totalmove,maxmove,midxpix;

	midxpix = (ob->midx&0xf0) >> 4;

	maxmove = -abs(midxmoved) - bottommoved - 16;
	map = (uint16_t *)mapsegs[1] +
		mapbwidthtable[oldtilebottom-1]/2 + ob->tilemidx;
	for (y=oldtilebottom-1 ; y<=ob->tilebottom ; y++,map+=mapwidth)
	{
		if (wall = tinf[NORTHWALL+*map])
		{
			clip = wallclip[wall&7][midxpix];
			move = ( (y<<G_T_SHIFT)+clip - 1) - ob->bottom;
			if (move<0 && move>=maxmove)
			{
				ob->hitnorth = wall;
				MoveObjVert (ob,move);
				return;
			}
		}
	}

	maxmove = abs(midxmoved) - topmoved + 16;
	map = (uint16_t *)mapsegs[1] +
		mapbwidthtable[oldtiletop+1]/2 + ob->tilemidx;
	for (y=oldtiletop+1 ; (int)y>=(int)ob->tiletop ; y--,map-=mapwidth)
	{
		if (wall = tinf[SOUTHWALL+*map])
		{
			clip = wallclip[wall&7][midxpix];
			move = ( ((y+1)<<G_T_SHIFT)-clip ) - ob->top;
			if (move > 0 && move<=maxmove)
			{
				totalmove = ob->ymove + move;
				if (totalmove < TILEGLOBAL && totalmove > -TILEGLOBAL)
				{
					ob->hitsouth = wall;
					MoveObjVert (ob,move);
				}
			}
		}
	}
}


/*
===========================
=
= ClipToEastWalls / ClipToWestWalls
=
===========================
*/

void ClipToEastWalls (objtype *ob)
{
	int			y,move,top,bottom;
	uint16_t	 *map,tile,info,wall;

	// clip to east walls if moving west

	top = ob->tiletop;
	if (ob->hitsouth>1)
		top++;			// on a slope inside a tile
	bottom = ob->tilebottom;
	if (ob->hitnorth>1)
		bottom--;			// on a slope inside a tile

	for (y=top;y<=bottom;y++)
	{
		map = (uint16_t *)mapsegs[1] +
			mapbwidthtable[y]/2 + ob->tileleft;

		if (ob->hiteast = tinf[EASTWALL+*map])
		{
			move = ( (ob->tileleft+1)<<G_T_SHIFT ) - ob->left;
			MoveObjHoriz (ob,move);
			return;
		}
	}
}


void ClipToWestWalls (objtype *ob)
{
	int			y,move,top,bottom;
	uint16_t	 *map,tile,info,wall;

	// check west walls if moving east

	top = ob->tiletop;
	if (ob->hitsouth>1)
		top++;			// on a slope inside a tile
	bottom = ob->tilebottom;
	if (ob->hitnorth>1)
		bottom--;			// on a slope inside a tile

	for (y=top;y<=bottom;y++)
	{
		map = (uint16_t *)mapsegs[1] +
			mapbwidthtable[y]/2 + ob->tileright;

		if (ob->hitwest = tinf[WESTWALL+*map])
		{
			move = ( (ob->tileright<<G_T_SHIFT ) -1) - ob->right;
			MoveObjHoriz (ob,move);
			return;
		}
	}
}

// turn 'possibly incorrect assignment' warnings back on
#pragma warn +pia


//==========================================================================

/*
================
=
= ClipToWalls
=
= Moves the current object xmove/ymove units, clipping to walls
=
================
*/

void ClipToWalls (objtype *ob)
{
	unsigned	x,y,tile;
	spritetabletype	far *shape;
	boolean	endfirst;

//
// make sure it stays in contact with a 45 degree slope
//
	if (ob->state->pushtofloor)
	{
		if (ob->xmove > 0)
			ob->ymove = ob->xmove + 16;
		else
			ob->ymove = -ob->xmove + 16;
	}

//
// move the shape
//
	if (ob->xmove > MAXMOVE)
		ob->xmove = MAXMOVE;
	else if (ob->xmove < -MAXMOVE)
		ob->xmove = -MAXMOVE;

	if (ob->ymove > MAXMOVE+16)			// +16 for push to floor
		ob->ymove = MAXMOVE+16;
	else if (ob->ymove < -MAXMOVE)
		ob->ymove = -MAXMOVE;

	ob->x += ob->xmove;
	ob->y += ob->ymove;

	ob->needtoreact = true;

	if (!ob->shapenum)				// can't get a hit rect with no shape!
		return;

	shape = &spritetable[ob->shapenum-STARTSPRITES];

	oldtileright = ob->tileright;
	oldtiletop = ob->tiletop;
	oldtileleft = ob->tileleft;
	oldtilebottom = ob->tilebottom;
	oldtilemidx = ob->tilemidx;

	oldright = ob->right;
	oldtop = ob->top;
	oldleft = ob->left;
	oldbottom = ob->bottom;
	oldmidx = ob->midx;

	ob->left = ob->x + shape->xl;
	ob->right = ob->x + shape->xh;
	ob->top = ob->y + shape->yl;
	ob->bottom = ob->y + shape->yh;
	ob->midx = ob->left + (ob->right - ob->left)/2;

	ob->tileleft = ob->left >> G_T_SHIFT;
	ob->tileright = ob->right >> G_T_SHIFT;
	ob->tiletop = ob->top >> G_T_SHIFT;
	ob->tilebottom = ob->bottom >> G_T_SHIFT;
	ob->tilemidx = ob->midx >> G_T_SHIFT;

	ob->hitnorth = ob->hiteast = ob->hitsouth = ob->hitwest = 0;

	if (!ob->needtoclip)
		return;

	leftmoved = ob->left - oldleft;
	rightmoved = ob->right - oldright;
	topmoved = ob->top - oldtop;
	bottommoved = ob->bottom - oldbottom;
	midxmoved = ob->midx - oldmidx;

//
// clip it
//

	ClipToEnds(ob);

	if (leftmoved < 0 || ob == player)	// make sure player gets cliped
		ClipToEastWalls (ob);
	if (rightmoved > 0 || ob == player)
		ClipToWestWalls (ob);
}


//==========================================================================


/*
==================
=
= ClipToSpriteSide
=
= Clips push to solid
=
==================
*/

void ClipToSpriteSide (objtype *push, objtype *solid)
{
	int xmove,leftinto,rightinto;

	//
	// amount the push shape can be pushed
	//
	xmove = solid->xmove - push->xmove;

	//
	// amount it is inside
	//
	leftinto = solid->right - push->left;
	rightinto = push->right - solid->left;

	if (leftinto>0 && leftinto<= xmove)
	{
		push->xmove = leftinto;
		if (push->state->pushtofloor)
			push->ymove = leftinto+16;
		ClipToWalls (push);
		push->hiteast = 1;
		return;
	}

	if (rightinto>0 && rightinto<= -xmove)
	{
		push->xmove = -rightinto;
		if (push->state->pushtofloor)
			push->ymove = rightinto+16;
		ClipToWalls (push);
		push->hitwest = 1;
		return;
	}

}

//==========================================================================


/*
==================
=
= ClipToSprite
=
= Clips push to solid
=
==================
*/

void ClipToSprite (objtype *push, objtype *solid, boolean squish)
{
	boolean temp;
	int walltemp,xmove,leftinto,rightinto,topinto,bottominto;

	xmove = solid->xmove - push->xmove;

	push->xmove = push->ymove = 0;

	//
	// left / right
	//
	leftinto = solid->right - push->left;
	rightinto = push->right - solid->left;

	if (leftinto>0 && leftinto<=xmove)
	{
		push->xmove = leftinto;
		walltemp = push->hitnorth;
		ClipToWalls (push);
		if (!push->hitnorth)
			push->hitnorth = walltemp;
		if (squish && push->hitwest)
			KillKeen ();
		push->hiteast = 1;
		return;
	}
	else if (rightinto>0 && rightinto<=-xmove)
	{
		push->xmove = -rightinto;
		walltemp = push->hitnorth;
		ClipToWalls (push);
		if (!push->hitnorth)
			push->hitnorth = walltemp;
		if (squish && push->hiteast)
			KillKeen ();
		push->hitwest = 1;
		return;
	}

	//
	// top / bottom
	//
	topinto = solid->bottom - push->top;
	bottominto = push->bottom - solid->top;

	if (bottominto>0)
	{
		push->ymove = -bottominto+16;
		push->xmove = solid->xmove;
		temp = push->state->pushtofloor;
		push->state->pushtofloor = false;
		walltemp = push->hitnorth;
		ClipToWalls (push);
		if (!push->hitnorth)
			push->hitnorth = walltemp;
		push->state->pushtofloor = temp;
		push->hitnorth = 25;
	}
	else if (topinto>0)
	{
		push->ymove = topinto;
		ClipToWalls (push);
		push->hitsouth = 25;
	}
}


//==========================================================================


/*
==================
=
= DoActor
=
= Moves an actor in its current state by a given number of tics.
= If that time takes it into the next state, it changes the state
= and returns the number of excess tics after the state change
=
==================
*/

int DoActor (objtype *ob,int tics)
{
	int	newtics,movetics,excesstics;
	statetype near *state;

	state = ob->state;

	if (state->progress == think)
	{
		if (state->think)
		{
			if (ob->nothink)
				ob->nothink--;
			else
#pragma warn -pro
				state->think(ob);
#pragma warn +pro
		}
		return 0;
	}

	newtics = ob->ticcount+tics;

	if (newtics < state->tictime || state->tictime == 0)
	{
		ob->ticcount = newtics;
		if (state->progress == slide || state->progress == slidethink)
		{
			if (ob->xdir)
				ob->xmove += ob->xdir == 1 ? tics*state->xmove
				: -tics*state->xmove;
			if (ob->ydir)
				ob->ymove += ob->ydir == 1 ? tics*state->ymove
				: -tics*state->ymove;
		}
		if (state->progress == slidethink || state->progress == stepthink)
		{
			if (state->think)
			{
				if (ob->nothink)
					ob->nothink--;
				else
#pragma warn -pro
					state->think(ob);
#pragma warn +pro
			}
		}
		return 0;
	}
	else
	{
		movetics = state->tictime - ob->ticcount;
		excesstics = newtics - state->tictime;
		ob->ticcount = 0;
		if (state->progress == slide || state->progress == slidethink)
		{
			if (ob->xdir)
				ob->xmove += ob->xdir == 1 ? movetics*state->xmove
				: -movetics*state->xmove;
			if (ob->ydir)
				ob->ymove += ob->ydir == 1 ? movetics*state->ymove
				: -movetics*state->ymove;
		}
		else
		{
			if (ob->xdir)
				ob->xmove += ob->xdir == 1 ? state->xmove : -state->xmove;
			if (ob->ydir)
				ob->ymove += ob->ydir == 1 ? state->ymove : -state->ymove;
		}

		if (state->think)
		{
			if (ob->nothink)
				ob->nothink--;
			else
#pragma warn -pro
				state->think(ob);
#pragma warn +pro
		}

		if (ob->state == state)
			ob->state = state->nextstate;	// go to next state
		else if (!ob->state)
			return 0;			// object removed itself
		return excesstics;
	}
}

//==========================================================================


/*
====================
=
= StateMachine
=
= Change state and give directions
=
====================
*/

void StateMachine (objtype *ob)
{
	int excesstics,oldshapenum;
	statetype near *state;

	ob->xmove = ob->ymove = 0;
	oldshapenum = ob->shapenum;

	state = ob->state;

	excesstics = DoActor(ob,tics);
	if (ob->state != state)
	{
		ob->ticcount = 0;		// start the new state at 0, then use excess
		state = ob->state;
	}

	while (excesstics)
	{
	//
	// passed through to next state
	//
		if (!state->skippable && excesstics >= state->tictime)
			excesstics = DoActor(ob,state->tictime-1);
		else
			excesstics = DoActor(ob,excesstics);
		if (ob->state != state)
		{
			ob->ticcount = 0;		// start the new state at 0, then use excess
			state = ob->state;
		}
	}

	if (!state)			// object removed itself
	{
		RemoveObj (ob);
		return;
	}


	//
	// if state->rightshapenum == NULL, the state does not have a standard
	// shape (the think routine should have set it)
	//
	if (state->rightshapenum)
	{
		if (ob->xdir>0)
			ob->shapenum = state->rightshapenum;
		else
			ob->shapenum = state->leftshapenum;
	}
	if (ob->shapenum == (unsigned)-1)
		ob->shapenum = 0;		// make it invisable this time

	if (ob->xmove || ob->ymove || ob->shapenum != oldshapenum)
	{
	//
	// actor moved or changed shape
	// make sure the movement is within limits (one tile)
	//
		ClipToWalls (ob);
	}
}

//==========================================================================


/*
====================
=
= NewState
=
====================
*/

void NewState (objtype *ob,statetype near *state)
{
	boolean temp;

	ob->state = state;

	if (state->rightshapenum)
	{
		if (ob->xdir>0)
			ob->shapenum = state->rightshapenum;
		else
			ob->shapenum = state->leftshapenum;
	}

	temp = ob->needtoclip;

	ob->needtoclip = false;

	ClipToWalls (ob);					// just calculate values

	ob->needtoclip = temp;

	if (ob->needtoclip)
		ClipToWalls (ob);

}

//==========================================================================

/*
============================
=
= PollControls
=
============================
*/

void PollControls (void)
{
	IN_ReadControl(0, &c);

	if (GravisGamepad && !DemoMode)
	{
		jumpbutton = GravisAction[ga_Jump];
		pogobutton = GravisAction[ga_Pogo];
		attackbutton = GravisAction[ga_Attack];
		if (!jumpbutton)
			jumpheld = false;
		if (!pogobutton)
			pogoheld = false;
		if (!attackbutton)
			attackheld = false;
	}
	else if (oldattacking || DemoMode)
	{
		if (c.button0 && c.button1)
		{
			attackbutton = true;
			jumpbutton = pogobutton = jumpheld = pogoheld = false;
		}
		else
		{
			attackbutton = attackheld = false;
			if (c.button0)
			{
				jumpbutton = true;
			}
			else
			{
				jumpbutton = jumpheld = false;
			}
			if (c.button1)
			{
				if (oldattackcount <= 8)
				{
					oldattackcount = oldattackcount + tics;
				}
				else
				{
					pogobutton = true;
				}
			}
			else
			{
				if (oldattackcount != 0)
				{
					pogobutton = true;
				}
				else
				{
					pogobutton = pogoheld = false;
				}
				oldattackcount = 0;
			}
		}
	}
	else
	{
		jumpbutton = c.button0;
		pogobutton = c.button1;			
		attackbutton = Keyboard[attackscan];
		if (!jumpbutton)
			jumpheld = false;
		if (!pogobutton)
			pogoheld = false;
		if (!attackbutton)
			attackheld = false;
	}
}

//==========================================================================

/*
============================
=
= PlayLoop
=
============================
*/

void PlayLoop (void)
{
	objtype *ob, *obj, *check;
	long	newtime;

	keenkilled = false;

	button0held = button1held = false;

	invincible = keenkilled = oldattackcount = 0;
	ingame = true;
	playstate = notdone;
	plummet = 0;

	centerlevel = 140;

	riding = NULL;
	onmushroom = false;
	CenterActor(player);

	FixScoreBox ();					// draw bomb/flower
	// Modified for fixing demo music (ktx58)
	StartMusic(gamestate.mapon);

    	if (DemoMode)
    	{
        	US_InitRndT(false);
    	}
    	else
    	{
        	US_InitRndT(true);
    	}
    	TimeCount = lasttimecount = tics = 3;

	do
	{
		PollControls();

//
// go through state changes and propose movements
//
		obj = player;
		do
		{
			if (!obj->active
			&& obj->tileright >= originxtile
			&& obj->tileleft <= originxtilemax
			&& obj->tiletop <= originytilemax
			&& obj->tilebottom >= originytile)
			{
				obj->needtoreact = true;
				obj->active = yes;
			}

			if (obj->active)
				StateMachine(obj);

			if ( (obj->active == true || obj->active == removable) &&
			(  obj->tileright < inactivateleft
			|| obj->tileleft > inactivateright
			|| obj->tiletop > inactivatebottom
			|| obj->tilebottom < inactivatetop) )
			{
				if (obj->active == removable)
					RemoveObj (obj);				// temp thing (shots, etc)
				else
				{
					if (US_RndT()<tics)				// let them get a random dist
					{
						RF_RemoveSprite (&obj->sprite);
						obj->active = no;
					}
				}
			}

			obj = (objtype *)obj->next;
		} while (obj);
		
		// Modification (ktx58): Added riding code
		if (riding) 
			RideVerticalPlatform(player);

		if (gamestate.damagetime)
			gamestate.damagetime--;
//
// check for and handle collisions between objects
//
		obj = player;
		do
		{
			if (obj->active)
			{
				check = (objtype *)obj->next;
				while (check)
				{
					if ( check->active
					&& obj->right > check->left
					&& obj->left < check->right
					&& obj->top < check->bottom
					&& obj->bottom > check->top)
					{
#pragma warn -pro
						if (obj->state->contact)
							obj->state->contact(obj,check);
						if (check->state->contact)
							check->state->contact(check,obj);
#pragma warn +pro
						if (!obj->obclass)
							break;				// contact removed object
					}
					check = (objtype *)check->next;
				}
			}
			obj = (objtype *)obj->next;
		} while (obj);

		if (mapon != 0)
		{
			ScrollScreen(player);
		}
		else
		{
			WorldScrollScreen(player);
		}
		if (DemoMode)
			ScoreThink(scoreobj);
		if (loadedgame)
		{
			loadedgame = false;
		}

//
// react to whatever happened, and post sprites to the refresh manager
//
		obj = player;
		do
		{
			if (obj->needtoreact && obj->state->react)
			{
				obj->needtoreact = false;
#pragma warn -pro
				obj->state->react(obj);
#pragma warn +pro
			}
			obj = (objtype *)obj->next;
		} while (obj);

//
// update the screen and calculate the number of tics it took to execute
// this cycle of events (for adaptive timing of next cycle)
//
		RF_Refresh();

		if (invincible)
		{
			if ((invincible = invincible - tics) < 0)
				invincible = 0;
		}

//
// single step debug mode
//
		if (singlestep)
		{
			VW_WaitVBL(14);
			lasttimecount = TimeCount;
		}
		// Imported from Catacomb 3-D
		if (extravbls)
			VW_WaitVBL(extravbls);

//
// handle user inputs
//
		if (DemoMode == demo_Playback)
		{
			if (!screenfaded && IN_IsUserInput())
			{
				playstate = levelcomplete;
				if (LastScan != sc_F1)
				{
					LastScan = sc_Space;
				}
			}
		}
		else if (DemoMode == demo_PlayDone)
		{
			 playstate = levelcomplete;
		}
		else
		{
			CheckKeys();
		}


#ifdef DEMO_DEBUG
		SaveDataForOneStepInDemo();
#endif
//
// E-N-D cheat
//
		if (Keyboard[sc_E] && Keyboard[sc_N] && Keyboard[sc_D])
		{
			if (mapon >= 23 && mapon <= 46)
			{
				playstate = nightvictorious;
			}
			else if (gamestate.difficulty != gd_Grand && mapon == 21)
			{
				playstate = mortendgame;
			}
			else
			{
				playstate = victorious;
			}
		}

		if (singlestep)
		{
			VW_WaitVBL(14);
			lasttimecount = TimeCount;
		}

#if 0
		CheckKeys();
#endif
		
	} while (!loadedgame && !playstate);

#ifdef DEMO_DEBUG
	// Kludge for demo recording ...
	if (demodebugbuf)
	{
		if (debugnameprefix)
			WriteDemoDebugData(); else debugnameprefix = 'R';
	}
#endif
	ingame = false;
}


//==========================================================================

/*
==========================
=
= GameFinale
=
==========================
*/

void GameFinale (void)
{
	objtype *ob;

	StopMusic();
	SD_PlaySound (BOOBUSGONESND);
	SD_WaitSoundDone();
//	StartMusic(56);

//	VW_FixRefreshBuffer ();
	FinaleLayout();
}


/*
==========================
=
= BadGameFinale
=
==========================
*/

void BadGameFinale (void)
{
	StopMusic();

	VW_FixRefreshBuffer ();

	US_CenterWindow (36,21);
	PrintY += 4;
	US_CPrintFar(BADENDING1);
	VW_UpdateScreen();
	SD_PlaySound (BOOBUSGONESND);
	SD_WaitSoundDone();
	VW_WaitVBL(60);
	IN_ClearKeysDown ();
	IN_Ack();

	US_CenterWindow (36,21);
	PrintY += 4;
	US_CPrintFar(BADENDING2);
	VW_UpdateScreen();
	VW_WaitVBL(60);
	IN_ClearKeysDown ();
	IN_Ack();

	VW_FadeOut ();
	RF_FixOfs();
	VW_ClearVideo(BLACK);

	VW_UpdateScreen();

	RF_FixOfs();

	CA_MarkGrChunk(SCREAM1PIC);
	CA_CacheMarks(NULL, 0);
	VW_FixRefreshBuffer();

	VWB_DrawPic(94, 37, SCREAM1PIC);
	VW_UpdateScreen();
	VW_FadeIn ();
	VW_WaitVBL(30);

	RF_FixOfs();

	CA_MarkGrChunk(SCREAM2PIC);
	CA_CacheMarks(NULL, 0);
	VW_FixRefreshBuffer();

	VWB_DrawPic(94, 37, SCREAM2PIC);
	VW_UpdateScreen();
	VW_WaitVBL(5);
	IN_ClearKeysDown ();

	RF_FixOfs();

	CA_MarkGrChunk(SCREAM3PIC);		
	CA_CacheMarks(NULL, 0);
	VW_FixRefreshBuffer();

	SD_PlaySound(KEENSCREAMSND);
	VWB_DrawPic(94, 37, SCREAM3PIC);
	VW_UpdateScreen();
	VW_WaitVBL(100);
	IN_ClearKeysDown ();

	VW_FadeOut ();
	RF_FixOfs();
	VW_ClearVideo(BLACK);

	VW_UpdateScreen();
	VW_FadeIn ();

	VW_FadeOut ();
	RF_FixOfs();

	VW_FixRefreshBuffer();

	VW_UpdateScreen();
	VW_FadeIn ();

	RF_FixOfs();

	StopMusic();
	StartMusic(52);

	CA_MarkGrChunk(GAMEOVERPIC);
	CA_CacheMarks(NULL, 0);
	VW_FixRefreshBuffer();

	VWB_DrawPic(85, 90, GAMEOVERPIC);
	VW_UpdateScreen();
	IN_ClearKeysDown ();
	IN_Ack ();

	StopMusic();
}


/*
==========================
=
= NightGameFinaleCheat
=
==========================
*/

void NightGameFinaleCheat (void)
{
	StopMusic();
	SD_PlaySound (BOOBUSGONESND);
	SD_WaitSoundDone();
	CA_CacheGrChunk(OWLPIC);

	VW_FixRefreshBuffer ();
	US_CenterWindow(27, 8);
	VWB_DrawPic(WindowX, WindowY, OWLPIC);
	PrintY += 6;
	WindowW -= 48;
	WindowX += 48;
	US_CPrintFar(CHEATNIGHTGAMEFINALE);
	VW_UpdateScreen();
   	VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_Ack();

	VW_FadeOut ();
	RF_FixOfs();
	VW_ClearVideo(BLACK);

	VW_UpdateScreen();

	RF_FixOfs();

	VW_UpdateScreen();
	VW_FadeIn ();
	IN_ClearKeysDown();

	StopMusic();
}


/*
==========================
=
= MortEnding
=
==========================
*/

void MortEnding (void)
{

  // Note (ktx58): I have "refactored" this function and removed the
  //   variables not actually needed. Please test. 

  	StopMusic();

	if (gamestate.difficulty != gd_Grand)
	{
  		VW_FixRefreshBuffer();
    		US_CenterWindow(30, 17);
    		PrintY += 4;

    		StopMusic();
    		US_CPrintFar(MORTENDING);
    		VW_UpdateScreen();
    		VW_WaitVBL(1);
    		SD_PlaySound (BOOBUSGONESND);
    		SD_WaitSoundDone();
    		IN_ClearKeysDown();
    		IN_Ack();
	}

	if (gamestate.difficulty == gd_Grand)
	{
  		VW_FixRefreshBuffer();
		US_CenterWindow (36,21);
		PrintY += 4;

    		StopMusic();
		US_CPrintFar(MORTBADENDING);
		VW_UpdateScreen();
    		VW_WaitVBL(1);
		SD_PlaySound (BOOBUSGONESND);
		SD_WaitSoundDone();
		IN_ClearKeysDown ();
		IN_Ack();
	}

    	VW_FadeOut ();
    	RF_FixOfs();
    	VW_ClearVideo(BLACK);

    	VW_UpdateScreen();

    	RF_FixOfs();

    	CA_MarkGrChunk(MORTSCREAM1PIC);
    	CA_CacheMarks(NULL, 0);
    	VW_FixRefreshBuffer();

    	VWB_DrawPic(94, 37, MORTSCREAM1PIC);
    	VW_UpdateScreen();
    	VW_FadeIn ();
    	VW_WaitVBL(30);

    	RF_FixOfs();

    	CA_MarkGrChunk(MORTSCREAM2PIC);
    	CA_CacheMarks(NULL, 0);
    	VW_FixRefreshBuffer();

    	VWB_DrawPic(94, 37, MORTSCREAM2PIC);
    	VW_UpdateScreen();
    	VW_WaitVBL(5);
    	IN_ClearKeysDown ();

    	RF_FixOfs();

    	CA_MarkGrChunk(MORTSCREAM3PIC);
    	CA_CacheMarks(NULL, 0);
    	VW_FixRefreshBuffer();

    	SD_PlaySound(KEENSCREAMSND);
    	VWB_DrawPic(94, 37, MORTSCREAM3PIC);
    	VW_UpdateScreen();
    	VW_WaitVBL(100);
    	IN_ClearKeysDown ();

    	VW_FadeOut ();
    	RF_FixOfs();
    	VW_ClearVideo(BLACK);

    	VW_UpdateScreen();
    	VW_FadeIn ();

    	VW_FadeOut ();
    	RF_FixOfs();

    	VW_FixRefreshBuffer();

    	VW_UpdateScreen();
    	VW_FadeIn ();

    	RF_FixOfs();

    	StopMusic();
    	StartMusic(52);

    	VW_FixRefreshBuffer ();

	CA_MarkGrChunk(MORTGAMEOVERPIC);
    	CA_CacheMarks(NULL, 0);
    	VW_FixRefreshBuffer();

	VWB_DrawPic(85, 90, MORTGAMEOVERPIC);
    	VW_UpdateScreen();
	if (gamestate.difficulty != gd_Grand)
	{
		gamestate.leveldone[mapon] = true;
	}
	if (gamestate.difficulty == gd_Keen && mapon >= 0 && mapon <= 22)
	{
		mortimerunlocked = true;
	}
    	IN_ClearKeysDown ();
    	IN_Ack ();

    	StopMusic();
}


//==========================================================================

/*
==================
=
= StartDemoRecord
=
==================
*/

void StartDemoRecord(void)
{
	boolean esc;
	char *endptr;
	unsigned long value;
	unsigned int keenrecordfirst, keenrecordlast, keenrecordskip=24, mortrecordfirst, mortrecordlast;

	VW_FixRefreshBuffer();
	US_CenterWindow(30, 3);
	PrintY += 6;
	if (gamestate.mapon <= 22)
	{
		if (gamestate.difficulty == gd_Grand)
		{
			mortrecordfirst = 1;
			mortrecordlast = 19;
		}
		else
		{
			keenrecordfirst = 1;
			keenrecordlast = 19;				
		}
	}
	else
	{
		if (gamestate.difficulty == gd_Grand)
		{
			mortrecordfirst = 24;
			mortrecordlast = 46;
		}
		else
		{
			keenrecordfirst = 23;
			keenrecordlast = 46;				
		}
	}
	strcpy(str, "  Record a demo from level(");
	if (gamestate.difficulty == gd_Grand)
	{
		itoa(mortrecordfirst, str2, 10);
	}
	else
	{
		itoa(keenrecordfirst, str2, 10);
	}
	strcat(str, str2);
	strcat(str, "-");
	if (gamestate.difficulty == gd_Grand)
	{
		itoa(mortrecordlast, str2, 10);
	}
	else
	{
		itoa(keenrecordlast, str2, 10);
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
			if (endptr != str && *endptr == 0 && value >= mortrecordfirst && value <= mortrecordlast)
			{
				gamestate.mapon = (byte)value;
				playstate = warptolevel;
				IN_StartDemoRecord(0x1000);
			}
		}
		else
		{
			if (endptr != str && *endptr == 0 && value >= keenrecordfirst && value <= keenrecordlast)
			{
				if (endptr != str && *endptr == 0 && value >= keenrecordskip && value <= keenrecordskip)
				{
					gamestate.mapon = (byte)value;
					playstate = false;
				}
				else
				{
					gamestate.mapon = (byte)value;
					playstate = warptolevel;
					IN_StartDemoRecord(0x1000);
				}
			}
		}
		if (endptr != str && *endptr == 0 && value >= keenrecordfirst && value <= keenrecordlast)
		{
			if (endptr != str && *endptr == 0 && value >= keenrecordskip && value <= keenrecordskip)
			{
				gamestate.mapon = (byte)value;
				playstate = false;
			}
		}
	}
#ifdef DEMO_DEBUG
	StartDemoDataCollection(0);
#endif
}

/*
==================
=
= EndDemoRecord
=
==================
*/

void EndDemoRecord(void)
{
	int handle;
	boolean esc;
	char filename[] = "DEMO?."EXTENSION;

	IN_StopDemo();
	VW_FixRefreshBuffer();
	US_CenterWindow(22, 3);
	PrintY += 6;
	US_Print("  Save as demo #(0-9):");
	VW_UpdateScreen();
	esc = !US_LineInput(px, py, str, NULL, true, 2, 0);
	if (!esc && str[0] >= '0' && str[0] <= '9')
	{
		filename[4] = str[0];
		handle = open(filename, O_BINARY|O_WRONLY|O_CREAT, S_IFREG|S_IREAD|S_IWRITE);
		if (handle == -1)
		{
			Quit("EndDemoRecord:  Cannot write demo file!");
		}
		write(handle, &mapon, sizeof(mapon));
		write(handle, &DemoOffset, sizeof(DemoOffset));
		CA_FarWrite(handle, DemoBuffer, DemoOffset);
		close(handle);
	}
	IN_FreeDemoBuffer();
}


//==========================================================================

/*
==========================
=
= HandleDeath
=
==========================
*/

void HandleDeath (void)
{
	unsigned	top,bottom,selection,y,color;

	if (mapon == 18)
		gamestate.keycard = 0;
	else
		gamestate.keys = 0;
	gamestate.boobusbombs -= gamestate.bombsthislevel;
	gamestate.lives--;
	if (gamestate.lives < 0)
		return;
//
// Just restart "special" levels after losing a life, so the player can't exit to the worldmap
//

	if (mapon >= 21 && mapon <= 46)
	{
		return;
	}

	_fstrcpy(str, levelnames[mapon]);
	
	VW_FixRefreshBuffer ();
	US_CenterWindow (26,8);
	PrintY += 4;
	US_CPrint("You didn't make it past");
	US_CPrint(str);
	PrintY += 8;
	top = PrintY-2;
	US_CPrint ("Try Again");
	PrintY += 4;
	bottom = PrintY-2;
	US_CPrint ("Exit to the Land of Tuberia");

	IN_ClearKeysDown();
	selection = 0;
	while (true)
	{
		if (selection)
		{
			y = bottom;
		}
		else
		{
			y = top;
		}

// draw select bar
		if ( (TimeCount / 16)&1 )
		{
			if (mapon >= 0 && mapon <= 22)
			{
				if (DemoMode)
					color = 10;		// light green
				else
					if (gamestate.difficulty == gd_Grand)
						color = 9;	// light blue
					else
						color = SECONDCOLOR;	// light red
			}
		}
		else
		{
			if (gamestate.difficulty == gd_Grand)
				color = 4;				// dark red
			else
				color = FIRSTCOLOR;			// dark blue
		}

		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y,color);
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y+1,color);
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y+12,color);
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y+13,color);
		VWB_Vlin (y+1,y+11, WindowX+4,color);
		VWB_Vlin (y+1,y+11, WindowX+5,color);
		VWB_Vlin (y+1,y+11, WindowX+WindowW-4,color);
		VWB_Vlin (y+1,y+11, WindowX+WindowW-5,color);

		VW_UpdateScreen ();

// erase select bar
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y,WHITE);
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y+1,WHITE);
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y+12,WHITE);
		VWB_Hlin (WindowX+4,WindowX+WindowW-4,y+13,WHITE);
		VWB_Vlin (y+1,y+11, WindowX+4,WHITE);
		VWB_Vlin (y+1,y+11, WindowX+5,WHITE);
		VWB_Vlin (y+1,y+11, WindowX+WindowW-4,WHITE);
		VWB_Vlin (y+1,y+11, WindowX+WindowW-5,WHITE);

		if (LastScan == sc_Escape)
		{
		}

		IN_ReadControl(0,&c);		// get player input
		if (jumpbutton || LastScan == sc_Return
		|| LastScan == sc_Space)
		{
			if (selection)
				gamestate.mapon = 0;		// exit to tuberia
			return;	
		}
		if (c.yaxis == -1 || LastScan == sc_UpArrow)
			selection = 0;
		else if (c.yaxis == 1 || LastScan == sc_DownArrow)
			selection = 1;
	}
}

//==========================================================================

/*
==========================
=
= CountWonLevels
=
==========================
*/

unsigned CountWonLevels(unsigned firstlevel, unsigned lastlevel)
{
	// Note (ktx58): level 22 (start level) is skipped, I can change this 
	unsigned	i, count=0;
	
	for (i= firstlevel; i<=lastlevel; i++)
	{
		if (gamestate.difficulty != gd_Grand)
		{
			if (i != 22 && gamestate.leveldone[i])
				count++;
		}
		else
		{			
			if (i != 21 && gamestate.leveldone[i])
				count++;
		}
	}
			
	return count;
}
//==========================================================================



/*
============================
=
= GameLoop
=
= A game has just started (after the cinematic or load game)
=
============================
*/

void GameLoop (void)
{
	long	orgx,orgy;
	objtype *ob;
	objtype *hit;
	char _seg *textseg;
	gamestate.difficulty = restartgame;
	restartgame = gd_Continue;

	do
	{
startlevel:
		if (loadedgame)
		{
			// Modified for fixing demo music (ktx58)
			// StartMusic(gamestate.mapon);
			switch(gamestate.difficulty)
			{
			case 1:
				greenball = true;
				mortwindow = false;
				mortimerbedgfx = false;
				break;
			case 2:
				greenball = false;
				mortwindow = false;
				mortimerbedgfx = false;
				break;
			case 3:
				greenball = false;
				mortwindow = false;
				mortimerbedgfx = false;
				break;
			case 4:
				greenball = false;
				mortwindow = false;
				mortimerbedgfx = false;
				break;
			case 5:
				if (mortimerunlocked == true)
				{
					mortimerbedgfx = true;
					mortwindow = true;
				}
				break;
			}
			loadedgame = false;

			//
			// start the initial view position to center the player
			//
			orgx = (long)player->x - (150<<G_P_SHIFT);
			orgy = (long)player->y-(84<<G_P_SHIFT);
			if (orgx<0)
				orgx=0;
			if (orgy<0)
				orgy=0;

			VW_FadeOut ();
			fadecount = 0;
			RF_SetRefreshHook (&FadeAndUnhook);
			RF_NewPosition (orgx,orgy);
			CalcInactivate ();
		}
		else 
		{
			VW_FixRefreshBuffer ();
			US_CenterWindow (26,8);
			US_CPrint ("Loading");

			VW_UpdateScreen ();
			gamestate.bombsthislevel = 0;
			gamestate.damagetime = 0;
			SetupGameLevel (true);
			// Modified for fixing demo music (ktx58)
			// StartMusic(gamestate.mapon);	
			if (mmerror)
			{
				if (gamestate.mapon != 0)
				{
					mmerror = false;
					VW_FixRefreshBuffer ();
					US_CenterWindow(20, 8);
					PrintY += 20;
					US_CPrintFar(INSMEMTOLOADLEVEL);
					VW_UpdateScreen();
					IN_Ack();
					gamestate.mapon = 0;		// exit to world map
					SetupGameLevel(true);
				}
				if (mmerror)
				{
					Quit("GameLoop: Insufficient memory to load world map!");
				}
			}
		}

		PlayLoop ();

		if (loadedgame)
			goto startlevel;
		// Note (ktx58): Please check this code, it had the same indentation
		// level as the goto, but it is actually not part of an if block.
		StopMusic();

		switch (playstate)
		{
		case warptolevel:			
			goto startlevel;
			// StopMusic();

		case died:
			HandleDeath ();
			break;

		case endgame:
			IN_ClearKeysDown();
			VW_FadeOut ();
			RF_FixOfs();
			VW_ClearVideo(BLACK);

			VW_UpdateScreen();

			RF_FixOfs();

			VW_UpdateScreen();
			VW_FadeIn ();
			IN_ClearKeysDown();

			goto done;

		case levelcomplete:
			if (mapon)
			{
				StopMusic();
				if (mapon == 19 && gamestate.boobusbombs == 48)
					SD_StopSound();
				else
					SD_PlaySound (LEVELDONESND);
			}
			gamestate.leveldone[mapon] = true;	// finished the level
			if (mapon != 0)
			{
				if (gamestate.mapon >= 23 && gamestate.mapon <= 45)
				{
					if (gamestate.difficulty == gd_Grand && mapon == 24)
					{
						gamestate.mapon = 25;
						playstate = warptolevel;
					}
					else if (gamestate.difficulty != gd_Grand && mapon == 23)
					{
						gamestate.mapon = 25;
						playstate = warptolevel;
					}
					else
					{			
						gamestate.mapon++;
						playstate = warptolevel;
					}
				}
				else if (gamestate.mapon == 46)
				{
					if (gamestate.difficulty != gd_Grand)
					{
						playstate = nightvictorious;
					}
					else
					{
						playstate = endgame;
					}
				}
				else
				{
					if (mapon == 19 && gamestate.boobusbombs == 48)
					{
						SD_PlaySound (BOOBUSGONESND);
						SD_WaitSoundDone();
						StopMusic();

						StartMusic(51);
						VW_FixRefreshBuffer ();
						US_CenterWindow2 (36,21);
						PrintY += 4;
						US_CPrintFar(SUPERSECRETMESSAGETEXT);
						VW_UpdateScreen();
						VW_WaitVBL(30);
						IN_ClearKeysDown();
						IN_Ack();
						RF_ForceRefresh ();
						StopMusic();
					}
					gamestate.mapon = 0;
				}
			}
			else
			{
				SD_PlaySound (ENTERLEVELSND);
			}
			break;

		case resetgame:
			return;

		case mortendgame:
			MortEnding();
			goto done;

		case nightvictorious:
			NightGameFinaleCheat();
			goto done;

		case victorious:
			GameFinale ();
			// Note (ktx58): Simplifed the code, please check
			if (CountWonLevels(1, 20) == 20 && gamestate.difficulty != gd_Grand && gamestate.mapon != 21)
			{
  				VW_FadeOut();
  				RF_FixOfs();

				gamestate.mapon = 21;
				playstate = warptolevel;
				SD_PlaySound (BOOBUSGONESND);
				SD_WaitSoundDone();
				StartMusic(51);
				US_CenterWindow (36,21);
				PrintY += 8;
				US_CPrintFar(MORTFIGHTINTROTEXT);
				VW_UpdateScreen();
  				VW_FadeIn();
				VW_WaitVBL(60);
				IN_ClearKeysDown ();
				IN_Ack();
				StopMusic();
				VW_FadeOut ();
				RF_FixOfs();
				VW_ClearVideo(BLACK);

				VW_UpdateScreen();

				RF_FixOfs();
			}
			else
			{
				goto done;
			}
		}


	} while (gamestate.lives>-1 && playstate!=victorious);

		if (mapon == 20)
		{
			if (gamestate.difficulty == gd_Grand)
				MortEnding();
			else
				BadGameFinale();
		}
		else
		{
			GameOver();
		}

done:
	if (mapon >= 23 && mapon <= 46)
	{
		if (gamestate.difficulty == gd_Grand)
		{
			US_CheckHighScore (gamestate.score,CountWonLevels(24, 46));
		}
		else
		{
			US_CheckHighScore (gamestate.score,CountWonLevels(23, 46));
		}
	}
	else
	{
		if (gamestate.difficulty == gd_Grand)
		{
			US_CheckHighScore (gamestate.score,CountWonLevels(1, 20));
		}
		else
		{
			US_CheckHighScore (gamestate.score,CountWonLevels(1, 21));
		}
	}
	VW_ClearVideo (FIRSTCOLOR);

	if (enternightmodecode && playstate != endgame && gamestate.lives > -1 && mapon == 20)
	{
		enternightmodecode = false;
		nightmode = true;
	}
	else
	{
		nightmode = false;
		if (entermortimercode && playstate != endgame && gamestate.lives > -1 && mapon == 21)
		{
			entermortimercode = false;
			mortimerunlocked = true;
		}
	}

	if (DemoMode)
	{
		DemoMode = false;
	}

	if (gamestate.difficulty == gd_Easy)
	{
		greenball = false;
	}

	if (gamestate.difficulty == gd_Grand)
	{
		mortimerbedgfx = false;
		mortwindow = false;
	}
}

#ifdef DEMO_DEBUG

uint16_t  * demodebugbuf = NULL;
memptr demodebugseg; 
char debugnameprefix;

void StartDemoDataCollection(char nameprefix)
{
	if (demodebugbuf)
		Quit("");

	demodebugbuf = MK_FP(demodebugseg, 0);
	debugnameprefix = nameprefix;
}

void WriteDemoDebugData(void)
{
	int handle;
	long timerval;

	if (!demodebugbuf)
		return;
        
	timerval = (long)(LocalTime & 0xffff) + 0x318d;
	str[0] = debugnameprefix;
	ltoa(timerval, str + 1, 10);
	strcat(str, ".BIN");
	handle = open(str, O_CREAT | O_BINARY | O_WRONLY,
		S_IREAD | S_IWRITE | S_IFREG);
    
	if (handle == -1)
		Quit("");
    
	CA_FarWrite(
		handle, (char far *)MK_FP(demodebugseg, 0), 
		FP_OFF(demodebugbuf));
	close(handle);
	// MM_FreePtr(&bufseg);
	demodebugbuf = NULL;
}

void SaveDataForOneStepInDemo(void)
{
	if (!demodebugbuf)
		return;

	demodebugbuf[0] = player->x;
	demodebugbuf[1] = player->y;
	demodebugbuf[2] = (player->xmove << 2) | ((player->xdir + 1) & 0x3);
	demodebugbuf[3] = (player->ymove << 2) | ((player->ydir + 1) & 0x3);
	demodebugbuf[4] = player->xspeed;
	demodebugbuf[5] = player->yspeed;
	demodebugbuf[6] = ((unsigned)lasttimecount & 0xfff) | 
		(player->hitnorth ? 0x1000 : 0) | (player->hiteast ? 0x2000 : 0) |
		(player->hitsouth ? 0x4000 : 0) | (player->hitwest ? 0x8000 : 0);
	demodebugbuf[7] = (unsigned)LocalTime;
	demodebugbuf += 8;  // actually 16 bytes!
        
	if (FP_OFF(demodebugbuf) >= DEMO_DEBUG_BUF_SIZE)
	{
		WriteDemoDebugData();
		SD_PlaySound(GRAPESCREAMSND);
	}
}

#endif
