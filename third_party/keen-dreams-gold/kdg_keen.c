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

// KDG_KEEN.C

#include "kdg_def.h"
#pragma hdrstop

/*

player->temp1 = pausetime / pointer to zees when sleeping
player->temp2 = pausecount / stagecount
player->temp3 =
player->temp4 =


*/


/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

#define KEENPRIORITY	1

#define PLACESPRITE RF_PlaceSprite (&ob->sprite,ob->x,ob->y,ob->shapenum, \
	spritedraw,KEENPRIORITY);

#define	MINPOLEJUMPTICS	19	// wait tics before allowing a pole regram

#define SPDRUNJUMP		16
#define SPDPOLESIDEJUMP	8
#define WALKAIRSPEED	8
#define	JUMPTIME		16
#define	POLEJUMPTIME	10
#define	SPDJUMP			40
#define	SPDPOLEJUMP		20

#define SPDPOWERUP		-64
#define SPDPOWER		64
#define SPDPOWERX		20	// Note(Szemigi): First X coordinate for Keen's placing state
#define SPDPOWERX2		64	// 2nd note(Szemigi): Second X coordinate for Keen's placing state
#define SPDPOWERY		-20
#define POWERCOUNT		50

#define MAXXSPEED   24

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

int bonuspoints[] = {100,200,500,1000,2000,5000};

int 	invincible;

int	singlegravity;
unsigned	bounceangle[8][8] =
{
{0,0,0,0,0,0,0,0},
{7,6,5,4,3,2,1,0},
{5,4,3,2,1,0,15,14},
{5,4,3,2,1,0,15,14},
{3,2,1,0,15,14,13,12},
{9,8,7,6,5,4,3,2},
{9,8,7,6,5,4,3,2},
{11,10,9,8,7,6,5,4}
};

char far PINKOWLTEXT[] =
  "Congratulations!\n"
  "You've successfully\n"
  "survived the night\n"
  "in Tuberia!\n";

char far PINKOWLTEXT2[] =
  "Your Flower Power won't\n"
  "work as strongly as it\n"
  "has been, after this area!\n";

char far PINKOWLTEXT3[] =
  "The King and Queen's army\n"
  "will be coming, so you\n"
  "shouldn't stay here!\n";

char far DAVETEXT[] =
  "Ugh... What happened?!\n"
  "I was in Dr. Nemesis'\n"
  "psych ward, with a crazy\n"
  "nurse with a scalpel\n"
  "over me, now I'm here!\n";

char far SNAKETEXT1[] =
  "Thanks for getting me\n"
  "outta here! I gotta go\n"
  "stop Dr. Helix von\n"
  "Vile's fiendish plot!\n";

char far SNAKETEXT2[] =
  "No problem, Sir Logan!\n";

char far ROVERTEXT1[] =
  "Arf! Arf!\n";

char far ROVERTEXT2[] =
  "Don't worry, I get you\n"
  "outta here, Rover!\n";

char far VORTICONTEXT1[] =
  "Get me out of here!\n"
  "This place is all\n"
  "strange and weird!\n";

char far VORTICONTEXT2[] =
  "This doesn't seem like\n"
  "Vorticon VI to me...\n";

char far VORTICONTEXT3[] =
  "This kingdom is terrible!\n"
  "Does the King even know\n"
  "how to rule?!\n";

char far SPOTTEXT1[] =
  "Hey, Master!\n";

char far SPOTTEXT2[] =
  "Wait, you can talk?\n";

char far SPOTTEXT3[] =
  "I guess I can here!\n";

char far MORTFIGHTTEXT1[] =
  "YOU'RE here, Mortimer?!\n"
  "But... you can't be real!\n"
  "You were caught\n"
  "in the Mangling Machine\n"
  "explosion!\n";

char far MORTFIGHTTEXT2[] =
  "YOU'RE as fake as this\n"
  "world the Dream Machine\n"
  "created! I won't let\n"
  "you win! Never ever!\n";

char far MORTFIGHTTEXT3[] =
  "Sleepy or not, neither\n"
  "the Dream Machine or\n"
  "my mind is going to\n"
  "pull tricks on me!\n";

char far MORTFIGHTTEXT4[] =
  "Huh, HE was the fake.\n"
  "He's still here!\n";

char far BOMBTEXTPART1[] =
  "You can't defeat\n"
  "King Boobus Tuber\n"
  "with less than ";

char far BOMBTEXTPART2[] =
  "You can't defeat\n"
  "Queen Bustling Tuberina\n"
  "with less than ";

char far BIRDTEXT[] =
  "No time for napping.\n"
  "You must save the\n"
  "kids from King Boobus!\n";

char far BIRDTEXT2[] =
  "Hey, don't sleep now!\n"
  "Stop Queen Bustling and\n"
  "rescue the children!\n";

char far BIRDTEXT3[] =
  "You really want to\n"
  "sleep again?\n"
  "Okay, but then resume\n"
  "your quest...\n";

char far BIRDTEXT4[] =
  "You look familiar...\n"
  "But in any case, you\n"
  "you are not allowed\n"
  "to come up here!\n";

char far NORTHTEXT[] =
  "MOUNT TUBEREST:\n\n"
  "Eyes will be watching you...\n"
  "\n\n\n\n\n\n\n\n\n\n"
  "(Press any key)\n";

char far SOUTHTEXT[] =
  "KUMQUAT CHASM:\n\n"
  "Watch the water currents!\n"
  "\n\n\n\n\n\n\n\n\n\n"
  "(Press any key)\n";

char far WESTTEXT[] =
  "FRUITY FOREST:\n\n"
  "Don't throw yourself for a loop & get lost!\n"
  "\n\n\n\n\n\n\n\n\n\n"
  "(Press any key)\n";

char far EASTTEXT[] =
  "LETTUCE LAKE:\n\n"
  "Lush, but dense greenery here!\n"
  "\n\n\n\n\n\n\n\n\n\n"
  "(Press any key)\n";

char far SOUTHWESTTEXT[] =
  "POTATO POINT:\n\n"
  "Mind your step or you'll get mashed!\n"
  "\n\n\n\n\n\n\n\n\n\n"
  "(Press any key)\n";

char far MORTBEDTEXT[] =
  "Only trained climbers\n"
  "are allowed to climb this\n"
  "mountain. Go and visit 20\n"
  "places in Tuberia to\n"
  "train yourself up.\n";

char far MORTBEDTEXT2[] =
  "We need your help!\n"
  "The king's gone mad\n"
  "and kidnapped lots of\n"
  "children!\n";

char far KEENROVEREXITTEXT1[] =
  "Rover! What are you\n"
  "doing here?\n";

char far KEENROVEREXITTEXT2[] =
  "Arf!\n";

char far KEENROVEREXITTEXT3[] =
  "Oh, is there more to\n"
  "explore down there?\n"
  "Thanks!\n";

char far MORTROVEREXITTEXT1[] =
  "Out of my way, you\n"
  "stupid mutt.\n";

char far MORTROVEREXITTEXT2[] =
  "Grrrrrr...\n";

char far MORTROVEREXITTEXT3[] =
  "I know my own way\n"
  "around here!\n";

char far NEWTUBERIATEXT[] =
  "\n"
  "NEW TUBERIA NEWS FLASH:\n\n"
  "\"All Boobus Bombs have been\n"
  "collected and removed from New Tuberia.\n"
  "In other news, an elusive individual\n"
  "unlike any other child found thus far\n"
  "has been seen in rare instances,\n"
  "but unable to be captured!\n"
  "But that's another helping\n"
  "of veggies...\"\n";

char far NEWTUBERIATEXT2[] =
  "\n"
  "NEW TUBERIA NEWS FLASH:\n\n"
  "\"All Boobus Bombs have been\n"
  "collected and removed from New Tuberia.\n"
  "Tuberia experiences a strange setback\n"
  "as the entire world seems to \"blip\"\n"
  "out of existence for a whole day, yet\n"
  "returns without a change!\n"
  "What could be causing this\n"
  "mysterious phenomenon?\"\n";

char far KIDTEXT[] =
  "I still have to learn these\n"
  "strange letters!\n";

char far KEYCARDTEXT[] =
  "Tuberian Royal Keycard\n\n"
  "This special key card is meant for the\n"
  "more secure parts of the castle.\n";

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

int	jumptime;
long	leavepoletime;		// TimeCount when jumped off pole


/*
=============================================================================

						 SCORE BOX ROUTINES

=============================================================================
*/

void	SpawnScore (void);
void ScoreThink (objtype *ob);
void ScoreReact (objtype *ob);
void DrawDemoPlaque (objtype *ob);

void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize);

statetype near s_score	= {NULL,NULL,think,false,
	false,0, 0,0, ScoreThink , NULL, ScoreReact, NULL};
statetype near s_demo        = {DEMOPLAQUESPR,DEMOPLAQUESPR,think,false,
	false,0, 0,0, NULL, NULL, NULL, NULL};


/*
======================
=
= SpawnScore
=
======================
*/

void	SpawnScore (void)
{
	scoreobj->obclass = inertobj;
	scoreobj->active = allways;
	scoreobj->needtoclip = false;
	*((long *)&(scoreobj->temp1)) = -1;		// force score to be updated
	scoreobj->temp3 = -1;			// and flower power
	scoreobj->temp4 = -1;			// and lives
	if (!DemoMode)
	{
		NewState(scoreobj, &s_score);
	}
	else
	{
		NewState(scoreobj, &s_demo);
		CA_MarkGrChunk(DEMOPLAQUESPR);
	}
}

void	FixScoreBox (void)
{
	int scoresprite = gamestate.difficulty != gd_Grand ?
		SCOREBOXSPR : SCOREBOX2SPR;
	unsigned	width, planesize;
	unsigned smallplane,bigplane;
	byte	far	*dest;

// draw boobus bomb if on level 20, else flower power
	// one byte per pixel (SDL2 port): no planes, the mask is in the colour
	width = spritetable[scoresprite-STARTSPRITES].width * 8;
	planesize = 0;
	dest = (byte far *)grsegs[scoresprite]
		+ width*16 + 6*CHARWIDTH;
	if (mapon == 20)
	{
		if (gamestate.difficulty == gd_Grand)
		{
			MemDrawChar (84,dest,width,planesize);
			MemDrawChar (85,dest+CHARWIDTH,width,planesize);
			MemDrawChar (64,dest+width*8,width,planesize);
			MemDrawChar (107,dest+width*8+CHARWIDTH,width,planesize);
		}
		else
		{
			MemDrawChar (20,dest,width,planesize);
			MemDrawChar (21,dest+CHARWIDTH,width,planesize);
			MemDrawChar (22,dest+width*8,width,planesize);
			MemDrawChar (23,dest+width*8+CHARWIDTH,width,planesize);
		}
	}
	else if (gamestate.difficulty != gd_Grand && mapon == 21)
	{
		MemDrawChar (41,dest,width,planesize);
		MemDrawChar (42,dest+CHARWIDTH,width,planesize);
		MemDrawChar (43,dest+width*8,width,planesize);
		MemDrawChar (44,dest+width*8+CHARWIDTH,width,planesize);
	}
	else if (mapon >= 23 && mapon <= 46)
	{
		if (gamestate.difficulty == gd_Grand)
		{
			MemDrawChar (71,dest,width,planesize);
			MemDrawChar (72,dest+CHARWIDTH,width,planesize);
			MemDrawChar (73,dest+width*8,width,planesize);
			MemDrawChar (74,dest+width*8+CHARWIDTH,width,planesize);
		}
		else
		{
			MemDrawChar (50,dest,width,planesize);
			MemDrawChar (51,dest+CHARWIDTH,width,planesize);
			MemDrawChar (52,dest+width*8,width,planesize);
			MemDrawChar (53,dest+width*8+CHARWIDTH,width,planesize);
		}
	}
	else
	{
		if (gamestate.difficulty == gd_Easy)
		{
			MemDrawChar (45,dest,width,planesize);
			MemDrawChar (46,dest+CHARWIDTH,width,planesize);
			MemDrawChar (47,dest+width*8,width,planesize);
			MemDrawChar (48,dest+width*8+CHARWIDTH,width,planesize);
		}
		else if (gamestate.difficulty == gd_Grand)
		{
			MemDrawChar (66,dest,width,planesize);
			MemDrawChar (67,dest+CHARWIDTH,width,planesize);
			MemDrawChar (68,dest+width*8,width,planesize);
			MemDrawChar (69,dest+width*8+CHARWIDTH,width,planesize);
		}
		else
		{
			MemDrawChar (28,dest,width,planesize);
			MemDrawChar (29,dest+CHARWIDTH,width,planesize);
			MemDrawChar (30,dest+width*8,width,planesize);
			MemDrawChar (31,dest+width*8+CHARWIDTH,width,planesize);
		}
	}

}


/*
======================
=
= MemDrawChar
=
======================
*/

#if GRMODE == EGAGR

void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize)
{
	// The SDL2 port's version: an 8x8 tile into a one-byte-per-pixel sprite
	VW_UnmaskedSubRectToPAL8(&(((byte*)(grsegs[STARTTILE8]))[char8*32]), dest, 0, 0, width, 8, 8, 8, 1);
	(void)planesize;
}
#endif

#if GRMODE == CGAGR

void MemDrawChar (int char8,byte far *dest,unsigned width,unsigned planesize)
{
	int source = (int)grsegs[STARTTILE8];

asm	mov	si,[char8]
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1
asm	shl	si,1		// index into char 8 segment

asm 	mov 	ds,[WORD PTR source]
//asm	mov	ds,[WORD PTR grsegs+STARTTILE8*2]
asm	mov	es,[WORD PTR dest+2]

asm	mov	bx,[width]
asm	sub	bx,2

asm	mov	di,[WORD PTR dest]

asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw
asm	add	di,bx
asm	movsw

asm	mov	ax,ss
asm	mov	ds,ax

	planesize++;		// shut the compiler up
}
#endif


/*
====================
=
= ShiftScore
=
====================
*/
#if GRMODE == EGAGR
void ShiftScore (void)
{
	// Sprites are drawn at any pixel; there are no shifted copies to update.
}
#endif


void ResetScoreBox(void)
{
  scoreobj->temp1 = scoreobj->temp2 = scoreobj->temp3 = scoreobj->temp4 = -1;
}

/*
===============
=
= ScoreThink
=
===============
*/

void ScoreThink (objtype *ob)
{
	int scoresprite = gamestate.difficulty != gd_Grand ?
		SCOREBOXSPR : SCOREBOX2SPR;
	char		str[12],*ch;		// K1n9_Duk3's fix
	byte		far *dest;
	unsigned	i, length, width, planesize, number;
	boolean changed;
	unsigned short	u;

	if (scorescreenkludge)
		return;

	if (DemoMode)
	{
		DrawDemoPlaque(ob);
		return;
	}

	if (!showscorebox)
		return;

//
// score changed
//
	if ((gamestate.score>>16) != ob->temp1
		|| (int16_t)gamestate.score != ob->temp2 )
	{
		// one byte per pixel (SDL2 port): no planes, the mask is in the colour
		width = spritetable[scoresprite-STARTSPRITES].width * 8;
		planesize = 0;
		dest = (byte far *)grsegs[scoresprite]
			+ width*4 + 1*CHARWIDTH;

		ultoa (gamestate.score,str,10);		// K1n9_Duk3's fix

		// erase leading spaces
		length = strlen(str);
		if (gamestate.difficulty == gd_Grand)
		{
			for (i=9;i>length;i--)
				MemDrawChar (87,dest+=CHARWIDTH,width,planesize);
		}
		else
		{
			for (i=9;i>length;i--)
				MemDrawChar (0,dest+=CHARWIDTH,width,planesize);
		}

		// draw digits
		ch = str;
		if (gamestate.difficulty == gd_Grand)
		{
			while (*ch)
				MemDrawChar (*ch++ - '0'+88,dest+=CHARWIDTH,width,planesize);
		}
		else
		{
			while (*ch)
				MemDrawChar (*ch++ - '0'+1,dest+=CHARWIDTH,width,planesize);
		}

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		ob->needtoreact = true;
		ob->temp1 = gamestate.score>>16;
		ob->temp2 = gamestate.score;

		changed = true;
	}

//
// flower power changed
//
	if (mapon == 20)
	{
		number = gamestate.boobusbombs;
	}
	else 
	{
		if (gamestate.difficulty != gd_Grand && mapon == 21 && mortinlevel)
			number = mortinlevel->temp4;
		else if (mapon >= 23 && mapon <= 46)
			number = gamestate.ammo;	
		else
			number = gamestate.flowerpowers;
	}

	if (number != ob->temp3)
	{
		// one byte per pixel (SDL2 port): no planes, the mask is in the colour
		width = spritetable[scoresprite-STARTSPRITES].width * 8;
		planesize = 0;
		dest = (byte far *)grsegs[scoresprite]
			+ width*20 + 8*CHARWIDTH;

		if (number > 99)
			strcpy (str,"99");
		else
			ltoa (number,str,10);

		// erase leading spaces
		length = strlen(str);
		if (gamestate.difficulty == gd_Grand)
		{
			for (i=2;i>length;i--)
				MemDrawChar (87,dest+=CHARWIDTH,width,planesize);
		}
		else
		{
			for (i=2;i>length;i--)
				MemDrawChar (0,dest+=CHARWIDTH,width,planesize);
		}

		// draw digits
		ch = str;
		if (gamestate.difficulty == gd_Grand)
		{
			while (*ch)
				MemDrawChar (*ch++ - '0'+88,dest+=CHARWIDTH,width,planesize);
		}
		else
		{
			while (*ch)
				MemDrawChar (*ch++ - '0'+1,dest+=CHARWIDTH,width,planesize);
		}

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		ob->needtoreact = true;
//		ob->temp3 = gamestate.flowerpowers;
		ob->temp3 = number;			// Fix this bug by changing it to 'number'.
	}

		// Szemigi:
		// 
		// BUGFIX: Originally, if the player has 99 bombs and throws one away,
		// the counter doesn't change back to 99 on the score box (!) after
		// picking that one bomb back up. Instead, it will always show 98.
		// It has now been fixed by adding 'number' instead of 'gamestate.flowerpowers'.
		// Thanks for ktx58's help!

//
// update needed icons, if necessary
// originally taken from K1n9_Duk3's BioMenace source code, modifying the grenade icon's updating code
//
	if (mapon == 20)
	{
		number = gamestate.boobusbombs;
	}
	else if (gamestate.difficulty != gd_Grand && mapon == 21)
	{
		number = mortinlevel->temp4;
	}
	else if (mapon >= 23 && mapon <= 46)
	{
		number = gamestate.ammo;
	}
	else
	{
		number = gamestate.flowerpowers;
	}
	if (ob->temp3 != number || ob->temp4 != number)
	{
		// one byte per pixel (SDL2 port): no planes, the mask is in the colour
		width = spritetable[scoresprite-STARTSPRITES].width * 8;
		planesize = 0;
		dest = (byte far *)grsegs[scoresprite]
			+ width*16 + 6*CHARWIDTH;
		if (mapon == 20)
		{
			if (gamestate.difficulty == gd_Grand)
			{
				MemDrawChar (84,dest,width,planesize);
				MemDrawChar (85,dest+CHARWIDTH,width,planesize);
				MemDrawChar (64,dest+width*8,width,planesize);
				MemDrawChar (107,dest+width*8+CHARWIDTH,width,planesize);
			}
			else
			{
				MemDrawChar (20,dest,width,planesize);
				MemDrawChar (21,dest+CHARWIDTH,width,planesize);
				MemDrawChar (22,dest+width*8,width,planesize);
				MemDrawChar (23,dest+width*8+CHARWIDTH,width,planesize);
			}
		}
		else if (gamestate.difficulty != gd_Grand && mapon == 21)
		{
			MemDrawChar (41,dest,width,planesize);
			MemDrawChar (42,dest+CHARWIDTH,width,planesize);
			MemDrawChar (43,dest+width*8,width,planesize);
			MemDrawChar (44,dest+width*8+CHARWIDTH,width,planesize);
		}
		else if (mapon >= 23 && mapon <= 46)
		{
			if (gamestate.difficulty == gd_Grand)
			{
				MemDrawChar (71,dest,width,planesize);
				MemDrawChar (72,dest+CHARWIDTH,width,planesize);
				MemDrawChar (73,dest+width*8,width,planesize);
				MemDrawChar (74,dest+width*8+CHARWIDTH,width,planesize);
			}
			else
			{
				MemDrawChar (50,dest,width,planesize);
				MemDrawChar (51,dest+CHARWIDTH,width,planesize);
				MemDrawChar (52,dest+width*8,width,planesize);
				MemDrawChar (53,dest+width*8+CHARWIDTH,width,planesize);
			}
		}
		else
		{
			if (gamestate.difficulty == gd_Easy)
			{
				MemDrawChar (45,dest,width,planesize);
				MemDrawChar (46,dest+CHARWIDTH,width,planesize);
				MemDrawChar (47,dest+width*8,width,planesize);
				MemDrawChar (48,dest+width*8+CHARWIDTH,width,planesize);
			}
			else if (gamestate.difficulty == gd_Grand)
			{
				MemDrawChar (66,dest,width,planesize);
				MemDrawChar (67,dest+CHARWIDTH,width,planesize);
				MemDrawChar (68,dest+width*8,width,planesize);
				MemDrawChar (69,dest+width*8+CHARWIDTH,width,planesize);
			}
			else
			{
				MemDrawChar (28,dest,width,planesize);
				MemDrawChar (29,dest+CHARWIDTH,width,planesize);
				MemDrawChar (30,dest+width*8,width,planesize);
				MemDrawChar (31,dest+width*8+CHARWIDTH,width,planesize);
			}
		}
		ob->needtoreact = true;
	}

//
// lives changed
//
	if (gamestate.lives != ob->temp4)
	{
		// one byte per pixel (SDL2 port): no planes, the mask is in the colour
		width = spritetable[scoresprite-STARTSPRITES].width * 8;
		planesize = 0;
		dest = (byte far *)grsegs[scoresprite]
			+ width*20 + 3*CHARWIDTH;

		u = 99;					// Szemigi: The counter now stops at 99 instead of overflowing on the score box.
		if (gamestate.lives > u)
			strcpy (str,"99");
		else
			ltoa (gamestate.lives,str,10);

		// erase leading spaces
		length = strlen(str);
		dest -= CHARWIDTH;
		if (gamestate.difficulty == gd_Grand)
		{
			for (i=2;i>length;i--)
				MemDrawChar (87,dest+=CHARWIDTH,width,planesize);
		}
		else
		{
			for (i=2;i>length;i--)
				MemDrawChar (0,dest+=CHARWIDTH,width,planesize);
		}

		// draw digits
		ch = str;
		if (gamestate.difficulty == gd_Grand)
		{
			while (*ch)
				MemDrawChar (*ch++ - '0'+88,dest+=CHARWIDTH,width,planesize);
		}
		else
		{
			while (*ch)
				MemDrawChar (*ch++ - '0'+1,dest+=CHARWIDTH,width,planesize);
		}

#if GRMODE == EGAGR
		ShiftScore ();
#endif
		ob->needtoreact = true;
		ob->temp4 = gamestate.lives;

		changed = true;
	}

	if (changed)
		ShiftScore();

	if (ob->x != originxglobal || ob->y != originyglobal)
	{
		ob->x = originxglobal;
		ob->y = originyglobal;
		changed = true;
	}

	if (changed)
#if GRMODE == EGAGR
		RF_PlaceSprite(&ob->sprite, ob->x+4*PIXGLOBAL, ob->y+4*PIXGLOBAL, scoresprite, spritedraw, 3);
#elif GRMODE == CGAGR
		RF_PlaceSprite(&ob->sprite, ob->x+4*PIXGLOBAL, ob->y+4*PIXGLOBAL, scoresprite, spritedraw, 3);
#endif
}


/*
===============
=
= ScoreReact
=
===============
*/

void ScoreReact (objtype *ob)
{
	int scoresprite = gamestate.difficulty != gd_Grand ?
		SCOREBOXSPR : SCOREBOX2SPR;
	ob->x = originxglobal;
	ob->y = originyglobal;

#if GRMODE == EGAGR
	RF_PlaceSprite (&ob->sprite
		,ob->x+4*PIXGLOBAL
		,ob->y+4*PIXGLOBAL
		,showscorebox ? scoresprite : 0
		,spritedraw
		,PRIORITIES-1);
#endif
#if GRMODE == CGAGR
	RF_PlaceSprite (&ob->sprite
		,ob->x+8*PIXGLOBAL
		,ob->y+8*PIXGLOBAL
		,showscorebox ? scoresprite : 0
		,spritedraw
		,PRIORITIES-1);
#endif
}


/*
===============
=
= DrawDemoPlaque
=
===============
*/

void DrawDemoPlaque(objtype *ob)
{
	if (ob->x != originxglobal || ob->y != originyglobal)
	{
		ob->x = originxglobal;
		ob->y = originyglobal;
		RF_PlaceSprite(&ob->sprite, ob->x + 160*PIXGLOBAL - 32*PIXGLOBAL, ob->y + 8*PIXGLOBAL, DEMOPLAQUESPR, spritedraw, 3);
	}
}


/*
=============================================================================

						FLOWER POWER ROUTINES

temp1 = 8, same as power bonus
temp2 = initial direction

=============================================================================
*/

void	PowerCount			(objtype *ob);
void	PowerContact 		(objtype *ob, objtype *hit);
void	PowerReact			(objtype *ob);

statetype near s_flowerpower1	= {FLOWERPOWER1SPR,FLOWERPOWER1SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, PowerContact, PowerReact, &s_flowerpower2};
statetype near s_flowerpower2	= {FLOWERPOWER2SPR,FLOWERPOWER2SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, PowerContact, PowerReact, &s_flowerpower1};

statetype near s_flowerpower3	= {FLOWERPOWER3SPR,FLOWERPOWER3SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, PowerContact, PowerReact, &s_flowerpower4};
statetype near s_flowerpower4	= {FLOWERPOWER4SPR,FLOWERPOWER4SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, PowerContact, PowerReact, &s_flowerpower3};

statetype near s_flowerpower5	= {FLOWERPOWER5SPR,FLOWERPOWER5SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, PowerContact, PowerReact, &s_flowerpower6};
statetype near s_flowerpower6	= {FLOWERPOWER6SPR,FLOWERPOWER6SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, PowerContact, PowerReact, &s_flowerpower5};

statetype near s_boobusbomb1	= {BOOBUSBOMB1SPR,BOOBUSBOMB1SPR,stepthink,false,
	false,5, 0,0, ProjectileThink, PowerContact, PowerReact, &s_boobusbomb2};
statetype near s_boobusbomb2	= {BOOBUSBOMB3SPR,BOOBUSBOMB3SPR,stepthink,false,
	false,5, 0,0, ProjectileThink, PowerContact, PowerReact, &s_boobusbomb1};

statetype near s_boobusbomb3	= {BOOBUSBOMB1SPR,BOOBUSBOMB1SPR,stepthink,false,
	false,20, 0,0, NULL, ShotContact, PowerReact, &s_boobusbomb4};
statetype near s_boobusbomb4	= {BOOBUSBOMB2SPR,BOOBUSBOMB2SPR,stepthink,false,
	false,20, 0,0, NULL, ShotContact, PowerReact, &s_boobusbomb3};

statetype near s_boobusbombthrown1	= {BOOBUSBOMB1SPR,BOOBUSBOMB1SPR,stepthink,false,
	false,20, 0,0, NULL, PowerContact, PowerReact, &s_boobusbombthrown2};
statetype near s_boobusbombthrown2	= {BOOBUSBOMB2SPR,BOOBUSBOMB2SPR,stepthink,false,
	false,20, 0,0, NULL, PowerContact, PowerReact, &s_boobusbombthrown1};

statetype near s_bombexplode	= {BOOBUSBOOM1SPR,BOOBUSBOOM1SPR,step,false,
	false,5, 0,0, NULL, NULL, DrawReact6, &s_bombexplode2};
statetype near s_bombexplode2= {BOOBUSBOOM2SPR,BOOBUSBOOM2SPR,step,false,
	false,5, 0,0, NULL, NULL, DrawReact6, &s_bombexplode3};
statetype near s_bombexplode3= {BOOBUSBOOM3SPR,BOOBUSBOOM3SPR,step,false,
	false,5, 0,0, NULL, NULL, DrawReact6, &s_bombexplode4};
statetype near s_bombexplode4= {BOOBUSBOOM1SPR,BOOBUSBOOM1SPR,step,false,
	false,5, 0,0, NULL, NULL, DrawReact6, &s_bombexplode5};
statetype near s_bombexplode5= {BOOBUSBOOM2SPR,BOOBUSBOOM2SPR,step,false,
	false,5, 0,0, NULL, NULL, DrawReact6, &s_bombexplode6};
statetype near s_bombexplode6= {BOOBUSBOOM3SPR,BOOBUSBOOM3SPR,step,false,
	false,5, 0,0, NULL, NULL, DrawReact6, NULL};

statetype near s_powerblink1	= {FLOWERPOWER1SPR,FLOWERPOWER1SPR,step,false,
	false,5, 0,0, PowerCount, PowerContact, DrawReact, &s_powerblink2};
statetype near s_powerblink2	= {-1,-1,step,false,
	false,5, 0,0, PowerCount, PowerContact, DrawReact, &s_powerblink1};

statetype near s_powerblink3	= {FLOWERPOWER3SPR,FLOWERPOWER3SPR,step,false,
	false,5, 0,0, PowerCount, PowerContact, DrawReact, &s_powerblink4};
statetype near s_powerblink4	= {-1,-1,step,false,
	false,5, 0,0, PowerCount, PowerContact, DrawReact, &s_powerblink3};

/*
====================
=
= SpawnBomb
=
====================
*/

void SpawnBomb (int tilex, int tiley)
{
	GetNewObj (false);

	new->needtoclip = false;
	new->obclass = boobusbombobj;
	new->x = tilex<<G_T_SHIFT;
	new->y = (tiley<<G_T_SHIFT)-0*BLOCKSIZE;
	NewState (new,&s_boobusbomb3);
}


/*
===============
=
= ThrowPower
=
===============
*/

void ThrowPower (unsigned x, unsigned y, int dir)
{
	statetype near *startstate;

	if (mapon == 20)
	{
		if (!gamestate.boobusbombs)
		{
			SD_PlaySound (NOWAYSND);
			return;						// no bombs to throw
		}
		SD_PlaySound (THROWBOMBSND);
		gamestate.bombsthislevel--;
		gamestate.boobusbombs--;
	}
	else if (mapon >= 23 && mapon <= 46)
	{
		if (!gamestate.ammo)
		{
			SD_PlaySound (NOWAYSND);
			return;						// no ammos to shoot
		}
		SD_PlaySound (THROWSND);
		gamestate.ammo--;
	}
	else
	{
		if (!gamestate.flowerpowers)
		{
			SD_PlaySound (NOWAYSND);
			return;						// no flower power to throw
		}
		SD_PlaySound (THROWSND);
		gamestate.flowerpowers--;
	}



	GetNewObj (true);
	new->obclass = powerobj;
	new->temp2 = dir;
	new->x = x;
	new->y = y;
	new->tileleft = new->tileright = x>>G_T_SHIFT;
	new->tiletop = new->tilebottom = y>>G_T_SHIFT;
	new->ydir = -1;

	switch (dir)
	{
	case dir_North:
		new->xspeed = 0;
		new->yspeed = SPDPOWERUP;
		break;
	case dir_East:
		if (mapon == 20)
		{
			new->xspeed = SPDPOWER;
			new->yspeed = SPDPOWERY;
		}
		else
		{
			if (player->state == &s_keenplace)
			{
				new->xspeed = SPDPOWERX2;
				new->yspeed = -SPDPOWERY;
			}
			else
			{
				new->xspeed = SPDPOWER;
				new->yspeed = SPDPOWERY;
			}
		}
		break;
	case dir_South:
		new->xspeed = 0;
		new->yspeed = SPDPOWER;
		break;
	case dir_West:
		if (mapon == 20)
		{
			new->xspeed = -SPDPOWER;
			new->yspeed = SPDPOWERY;
		}
		else
		{
			if (player->state == &s_keenplace)
			{
				new->xspeed = -SPDPOWERX;
				new->yspeed = SPDPOWERY;
			}
			else
			{
				new->xspeed = -SPDPOWER;
				new->yspeed = SPDPOWERY;
			}
		}
		break;
	default:
		Quit ("ThrowPower: Bad dir!");
	}

	if (mapon != 20)
	{
		new->temp1 = 8;  				// flower power bonus
		if (gamestate.difficulty == gd_Easy)
			startstate = &s_flowerpower3;
		else if (gamestate.difficulty == gd_Grand)
			startstate = &s_flowerpower5;
		else
			startstate = &s_flowerpower1;
	}
	else
	{
		new->temp1 = 10;  				// boobus bomb bonus
		startstate = &s_boobusbomb1;
	}
	new->active = (mapon != 20) ? removable : yes;

#if 0
	new->x -= 5*new->xspeed;		// make sure they hit nearby walls
	new->y -= 5*new->yspeed;
#endif
	NewState (new,startstate);
#if 0
	new->xmove = 5*new->xspeed;
	new->ymove = 5*new->yspeed;
	ClipToWalls (new);
#endif
}

/*
============================
=
= PowerCount
=
============================
*/

void PowerCount(objtype *ob)
{
	ob->temp2 += tics;
	ob->shapenum = 0;
	if (ob->temp2 > 50)
	{
		if (gamestate.difficulty == gd_Easy)
			return;
		else
			RemoveObj(ob);
	}
	else if (gamestate.difficulty == gd_Grand)
	{
		RemoveObj(ob);
	}
}


/*
============================
=
= CalcSingleGravity
=
============================
*/

void	CalcSingleGravity (void)
{
	unsigned	speed;
	long	i;
//
// only accelerate on odd tics, because of limited precision
//
	speed = 0;
	singlegravity = 0;
	for (i=lasttimecount-tics;i<lasttimecount;i++)
	{
		if (i&1)
		{
			speed+=ACCGRAVITY;
			if (speed>SPDMAXY)
			  speed=SPDMAXY;
		}
		singlegravity+=speed;
	}

	singlegravity/=2;
}


/*
============================
=
= PowerContact
=
============================
*/

void	PowerContact (objtype *ob, objtype *hit)
{
	unsigned	x,y,yspot,xspot;

	switch (hit->obclass)
	{
	case	taterobj:
	case	broccoobj:
	case	tomatobj:
	case	carrotobj:
	case    celeryobj:
	case	parsobj:
	case	asparobj:
	case	frenchyobj:
	case	squashobj:
	case	cauliobj:
	case	apelobj:
	case	peapodobj:
	case	peabrainobj:
	case	eggplantobj:
	case	pumpkinobj:
		ChangeToFlower (hit);
		RemoveObj (ob);
		break;
	case	turnipobj:
		if (!--hit->temp4)
		{
			ChangeToFlower (hit);
			RemoveObj (ob);
			hit->temp1 = 0;		// death count
		}
		SD_PlaySound (FLOWERPOWERSND);
		ChangeState (ob,&s_poof);
		if (hit->temp4 == 0)
		{
			ChangeState (ob,&s_poofto1);
		}
		break;
	case    mortobj:
		if (hit->state == &s_mortstand)
			return;

		if (!--hit->temp4)
		{
			gamestate.score += 1000000;
			hit->obclass = mortdeadobj;
			ChangeState (hit,&s_mortdie1);
			SD_PlaySound (FLOWERPOWERSND);
			ChangeState (ob,&s_poofto1);
			SD_PlaySound (WAKEUPSND);
			hit->temp1 = 0;		// death count
		}
		SD_PlaySound (FLOWERPOWERSND);
		if (hit->temp4 != 0)
		{
			ChangeState (ob,&s_poof);
		}
		else if (hit->temp4 == 0)
		{
			ChangeState (ob,&s_poofto1);
		}
		break;
	case	boobusobj:
		if (!--hit->temp4)
		{
		// killed boobus
			if (gamestate.difficulty == gd_Easy)
			{
				gamestate.score += 100000;
			}
			else if (gamestate.difficulty == gd_Normal || gamestate.difficulty == gd_Hard || gamestate.difficulty == gd_Keen)
			{
				gamestate.score += 200000;
			}
			else if (gamestate.difficulty == gd_Grand)
			{
				gamestate.score += 500000;
			}
			hit->obclass = inertobj;
			ChangeState (hit,&s_boobusdie);
			hit->temp1 = 0;		// death count
		}
		SD_PlaySound (BOMBBOOMSND);
		ChangeState (ob,&s_bombexplode);
		break;
	}
}


/*
============================
=
= PowerReact
=
============================
*/

void	PowerReact (objtype *ob)
{
	unsigned wall,absx,absy,angle,newangle;
	unsigned long speed;

	PLACESPRITE;
	if (ob->hiteast || ob->hitwest)
	{
		ob->xspeed= -ob->xspeed/2;
		ob->obclass = bonusobj;
	}

	if (ob->hitsouth)
	{
		if ( ob->hitsouth == 17)	// go through pole holes
		{
			if (ob->temp2 != dir_North)
				ob->obclass = bonusobj;
			ob->top-=32;
			ob->y-=32;
			ob->xspeed = 0;
			ob->x = ob->tilemidx*TILEGLOBAL + 4*PIXGLOBAL;
		}
		else
		{
			ob->yspeed= -ob->yspeed/2;
		}
		return;
	}

	wall = ob->hitnorth;
	if ( wall == 17)	// go through pole holes
	{
		if (ob->temp2 != dir_South)
			ob->obclass = bonusobj;
		ob->bottom+=32;
		ob->y+=32;
		ob->xspeed = 0;
		ob->x = ob->tilemidx*TILEGLOBAL + 4*PIXGLOBAL;
	}
	else if (wall)
	{
		ob->obclass = bonusobj;
		if (ob->yspeed < 0)
			ob->yspeed = 0;

		absx = abs(ob->xspeed);
		absy = ob->yspeed;
		if (absx>absy)
		{
			if (absx>absy*2)	// 22 degrees
			{
				angle = 0;
				speed = absx*286;	// x*sqrt(5)/2
			}
			else				// 45 degrees
			{
				angle = 1;
				speed = absx*362;	// x*sqrt(2)
			}
		}
		else
		{
			if (absy>absx*2)	// 90 degrees
			{
				angle = 3;
				speed = absy*256;
			}
			else
			{
				angle = 2;		// 67 degrees
				speed = absy*286;	// y*sqrt(5)/2
			}
		}
		if (ob->xspeed > 0)
			angle = 7-angle;

		speed >>= 1;
		newangle = bounceangle[ob->hitnorth][angle];
		switch (newangle)
		{
		case 0:
			ob->xspeed = speed / 286;
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 1:
			ob->xspeed = speed / 362;
			ob->yspeed = -ob->xspeed;
			break;
		case 2:
			ob->yspeed = -(speed / 286);
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 3:

		case 4:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 5:
			ob->yspeed = -(speed / 286);
			ob->xspeed = ob->yspeed / 2;
			break;
		case 6:
			ob->xspeed = ob->yspeed = -(speed / 362);
			break;
		case 7:
			ob->xspeed = -(speed / 286);
			ob->yspeed = ob->xspeed / 2;
			break;

		case 8:
			ob->xspeed = -(speed / 286);
			ob->yspeed = -ob->xspeed / 2;
			break;
		case 9:
			ob->xspeed = -(speed / 362);
			ob->yspeed = -ob->xspeed;
			break;
		case 10:
			ob->yspeed = speed / 286;
			ob->xspeed = -ob->yspeed / 2;
			break;
		case 11:

		case 12:
			ob->xspeed = 0;
			ob->yspeed = -(speed / 256);
			break;
		case 13:
			ob->yspeed = speed / 286;
			ob->xspeed = ob->yspeed / 2;
			break;
		case 14:
			ob->xspeed = speed / 362;
			ob->yspeed = speed / 362;
			break;
		case 15:
			ob->xspeed = speed / 286;
			ob->yspeed = ob->xspeed / 2;
			break;
		}

		if (speed < 256*16)
		{
			if (mapon == 20)
			{
				ob->needtoclip = cl_midclip;
				ChangeState (ob,&s_boobusbombthrown1);
			}
			else
			{
				if (gamestate.difficulty == gd_Easy)
					ChangeState (ob,&s_powerblink3);
				else if (gamestate.difficulty == gd_Grand)
					RemoveObj(ob);
				else
					ChangeState (ob,&s_powerblink1);
				ob->needtoclip = 0;
				ob->temp2 = 0;		// blink time
			}
		}
	}
}


/*
=============================================================================

							   MINI KEEN

player->temp1 = dir
player->temp2 = animation stage

#define WORLDKEENSLEEP1SPR		378
#define WORLDKEENSLEEP2SPR		379
#define WORLDKEENWAVE1SPR		380
#define WORLDKEENWAVE2SPR		381
#define WORLDMORTSLEEP1SPR		881
#define WORLDMORTSLEEP2SPR		882
#define WORLDMORTWAVE1SPR		883
#define WORLDMORTWAVE2SPR		884

=============================================================================
*/


void	SpawnWorldKeen (int tilex, int tiley);
void	KeenWorldThink		(objtype *ob);
void	KeenWorldWalk		(objtype *ob);
void	DrawWorldMort		(objtype *ob);

#pragma warn -sus

statetype near s_worldkeen	= {NULL,NULL,stepthink,false,
	false,360, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeenwave1};

statetype near s_worldkeenwave1= {WORLDKEENWAVE1SPR,WORLDKEENWAVE1SPR,stepthink,false,
	false,20, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeenwave2};
statetype near s_worldkeenwave2= {WORLDKEENWAVE2SPR,WORLDKEENWAVE2SPR,stepthink,false,
	false,20, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeenwave3};
statetype near s_worldkeenwave3= {WORLDKEENWAVE1SPR,WORLDKEENWAVE1SPR,stepthink,false,
	false,20, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeenwave4};
statetype near s_worldkeenwave4= {WORLDKEENWAVE2SPR,WORLDKEENWAVE2SPR,stepthink,false,
	false,20, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeenwave5};
statetype near s_worldkeenwave5= {WORLDKEENWAVE1SPR,WORLDKEENWAVE1SPR,stepthink,false,
	false,20, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeenwait};

statetype near s_worldkeenwait	= {WORLDKEEND3SPR,WORLDKEEND3SPR,stepthink,false,
	false,400, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeensleep1};

statetype near s_worldkeensleep1	= {WORLDKEENSLEEP1SPR,WORLDKEENSLEEP1SPR,stepthink,false,
	false,30, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeensleep2};
statetype near s_worldkeensleep2	= {WORLDKEENSLEEP2SPR,WORLDKEENSLEEP2SPR,stepthink,false,
	false,90, 0,0, KeenWorldThink, NULL, DrawWorldMort, &s_worldkeensleep2};

statetype near s_worldwalk	= {NULL,NULL,slide,true,
	false,4, 24,24, KeenWorldWalk, NULL, DrawWorldMort, &s_worldwalk};

#pragma warn +sus

/*
======================
=
= DrawWorldMort
=
======================
*/

void DrawWorldMort(objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1)
	{
		switch (gamestate.difficulty)
		{			
		case gd_Grand:
			shape += WORLDMORTL1SPR - WORLDKEENL1SPR;
			break;
		}
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}


/*
======================
=
= SpawnWorldKeen
=
======================
*/

void	SpawnWorldKeen (int tilex, int tiley)
{
	player->obclass = keenobj;
	if (!gamestate.worldx)
	{
		player->x = tilex<<G_T_SHIFT;
		player->y = tiley<<G_T_SHIFT;
	}
	else
	{
		player->x = gamestate.worldx;
		player->y = gamestate.worldy;
	}

	player->active = yes;
	player->needtoclip = true;
	player->xdir = 1;
	player->ydir = 1;
	player->temp1 = dir_West;
	player->temp2 = 3;
	player->shapenum = 3+WORLDKEENL1SPR-1;				// feet together
	NewState (player,&s_worldkeen);
}


/*
======================
=
= CheckEnterLevel
=
======================
*/

void	CheckEnterLevel (objtype *ob)
{
	int	x,y,tile,bombsneeded = 0;

	for (y=ob->tiletop;y<=ob->tilebottom;y++)
		for (x=ob->tileleft;x<=ob->tileright;x++)
		{
			tile = *((uint16_t *)mapsegs[2]+mapbwidthtable[y]/2+x);
			if (tile >= 1 && tile <=20 || tile >= 63 && tile <=63 || tile >= 64 && tile <=64 || tile >= 69 && tile <=69)
			{
			//
			// enter level!
			//
				if (tile == 20)
				{
					// Note (ktx58): Simplified the code
					
					switch (gamestate.difficulty)
					{
						case 1:
							bombsneeded = 8;
							break;
						case 2:
							bombsneeded = 12;
							break;
						case 3:
							bombsneeded = 16;
							break;
						case 4:
							bombsneeded = 20;
							break;
						case 5:
							bombsneeded = 24;
							break;
					}
					
					if (gamestate.boobusbombs < bombsneeded)
					{

						SD_PlaySound (NOWAYTWOSND);
						VW_FixRefreshBuffer();
						US_CenterWindow (26,6);
						PrintY+= 8;
						if (gamestate.difficulty == gd_Easy)
							_fstrcpy(str, BOMBTEXTPART2);
						else
							_fstrcpy(str, BOMBTEXTPART1);
						itoa(bombsneeded, str2, 10);
						strcat(str, str2);
						strcat(str, " bombs!\n");
						US_CPrint(str);
						VW_UpdateScreen ();
						IN_Ack();
						RF_ForceRefresh ();
						return;
					}
				}
				if (tile == 63)
				{
					SD_PlaySound (NOWAYTWOSND);
					CA_CacheGrChunk(OWLPIC);

					VW_FixRefreshBuffer ();
					US_CenterWindow(26, 8);
					VWB_DrawPic(WindowX, WindowY, OWLPIC);
					PrintY += 6;
					WindowW -= 48;
					WindowX += 48;
					if (gamestate.difficulty == gd_Easy)
						if (US_RndT() < 0x80)
							US_CPrintFar(BIRDTEXT3);
						else
							US_CPrintFar(BIRDTEXT2);
					else if (gamestate.difficulty == gd_Grand)
						US_CPrintFar(BIRDTEXT4);
					else
						if (US_RndT() < 0x80)
							US_CPrintFar(BIRDTEXT3);
						else
							US_CPrintFar(BIRDTEXT);
					VW_UpdateScreen ();
					IN_Ack();
					RF_ForceRefresh ();
					return;
				}
				if (tile == 69)
				{
					SD_PlaySound (NOWAYTWOSND);
					CA_CacheGrChunk(COMPASSNORTHPIC);
					CA_CacheGrChunk(COMPASSSOUTHPIC);
					CA_CacheGrChunk(COMPASSWESTPIC);
					CA_CacheGrChunk(COMPASSEASTPIC);
					CA_CacheGrChunk(COMPASSSOUTHWESTPIC);

					VW_FixRefreshBuffer ();
					US_CenterWindow (36,21);
    					VWB_DrawPic(135, 77, COMPASSNORTHPIC);
					PrintY += 8;
					US_CPrintFar(NORTHTEXT);
					VW_UpdateScreen ();
					IN_Ack();

					VW_FixRefreshBuffer ();
					US_CenterWindow (36,21);
    					VWB_DrawPic(135, 77, COMPASSSOUTHPIC);
					PrintY += 8;
					US_CPrintFar(SOUTHTEXT);
					VW_UpdateScreen ();
					IN_Ack();

					VW_FixRefreshBuffer ();
					US_CenterWindow (36,21);
    					VWB_DrawPic(135, 77, COMPASSWESTPIC);
					PrintY += 8;
					US_CPrintFar(WESTTEXT);
					VW_UpdateScreen ();
					IN_Ack();

					VW_FixRefreshBuffer ();
					US_CenterWindow (36,21);
    					VWB_DrawPic(135, 77, COMPASSEASTPIC);
					PrintY += 8;
					US_CPrintFar(EASTTEXT);
					VW_UpdateScreen ();
					IN_Ack();

					VW_FixRefreshBuffer ();
					US_CenterWindow (36,21);
    					VWB_DrawPic(135, 77, COMPASSSOUTHWESTPIC);
					PrintY += 8;
					US_CPrintFar(SOUTHWESTTEXT);
					VW_UpdateScreen ();
					IN_Ack();

					RF_ForceRefresh ();
					return;
				}
				if (tile == 64)
				{
					SD_PlaySound (NOWAYTWOSND);
					SD_WaitSoundDone();
					CA_CacheGrChunk(OWLPIC);

					VW_FixRefreshBuffer ();
					VW_FixRefreshBuffer ();
					US_CenterWindow(26, 8);
					VWB_DrawPic(WindowX, WindowY, OWLPIC);
					PrintY += 6;
					WindowW -= 48;
					WindowX += 48;
					if (gamestate.difficulty == gd_Grand)
						if (US_RndT() < 0x80)
							US_CPrintFar(MORTBEDTEXT2);
						else
							US_CPrintFar(BIRDTEXT3);
					else
						US_CPrintFar(MORTBEDTEXT);
					VW_UpdateScreen();
					IN_ClearKeysDown();
					IN_Ack();
					RF_ForceRefresh ();
					return;
				}
				gamestate.worldx = ob->x;
				gamestate.worldy = ob->y;
				gamestate.mapon = tile-0;
				playstate = warptolevel;
				SD_PlaySound (ENTERLEVELSND);
			}
		}
}


/*
======================
=
= KeenWorldThink
=
======================
*/

void	KeenWorldThink (objtype *ob)
{
	if (c.dir!=dir_None)
	{
		// ob->x;  // Note (ktx58):
		// ob->y;  // Please check, these actually do nothing.
		ob->state = &s_worldwalk;
		ob->temp2 = 0;
		KeenWorldWalk (ob);
	}

	if (jumpbutton || attackbutton)
		CheckEnterLevel(ob);
}


/*
======================
=
= KeenWorldWalk
=
======================
*/

int worldshapes[8] = {WORLDKEENU1SPR-1,WORLDKEENUR1SPR-1,WORLDKEENR1SPR-1,
	WORLDKEENDR1SPR-1,WORLDKEEND1SPR-1,WORLDKEENDL1SPR-1,WORLDKEENL1SPR-1,
	WORLDKEENUL1SPR-1};

int worldanims[4] ={2,3,1,3};

void	KeenWorldWalk (objtype *ob)
{
	if (++ob->temp2==4)
		ob->temp2 = 0;

	if (jumpbutton || attackbutton)
		CheckEnterLevel(ob);

	if (c.dir == dir_None)
	{
		ob->state = &s_worldkeen;
		ob->shapenum = worldshapes[ob->temp1]+3;
		return;
	}

	ob->xdir = c.xaxis;
	ob->ydir = c.yaxis;
	ob->temp1 = c.dir;
	ob->shapenum = worldshapes[ob->temp1]+worldanims[ob->temp2];

	if (ob->temp2 == 1)
	{
		SD_PlaySound(KEENWALKSND);
	}
	else if (ob->temp2 == 3)
	{
		SD_PlaySound(KEENWALKSND);
	}
}


/*
=============================================================================

								KEEN

player->temp1 = pausetime / pointer to zees when sleeping
player->temp2 = pausecount / stagecount
player->temp3 =
player->temp4 = pole x location

=============================================================================
*/

void 	KeenBounceThink		(objtype *ob);
void 	KeenPogoThink		(objtype *ob);
void	KeenPogoReact		(objtype *ob);
void	KeenStandThink		(objtype *ob);
void	KeenPauseThink		(objtype *ob);
void	KeenHitThink		(objtype *ob);
void	KeenGoSleepThink	(objtype *ob);
void	KeenSleepThink		(objtype *ob);
void	KeenDieThink		(objtype *ob);
void	KeenDieThink2		(objtype *ob);
void	KeenDuckThink		(objtype *ob);
void	KeenDropDownThink	(objtype *ob);
void	KeenWalkThink		(objtype *ob);
void	KeenAirThink		(objtype *ob);
void	KeenPoleThink		(objtype *ob);
void	KeenClimbThink		(objtype *ob);
void	KeenDropThink		(objtype *ob);
void	KeenThrow		(objtype *ob);
void	KeenShootThink		(objtype *ob);

void	KeenContact 		(objtype *ob, objtype *hit);
void	KeenLookUpDownContact 	(objtype *ob, objtype *hit);
void	KeenSimpleReact		(objtype *ob);
void	KeenStandReact		(objtype *ob);
void	KeenWalkReact		(objtype *ob);
void	KeenPoleReact		(objtype *ob);
void	KeenAirReact		(objtype *ob);
void	KeenSlideReact		(objtype *ob);

void	ZeeContact 		(objtype *ob);

// Added the new think functions
void 	ExplodeShot	(objtype *ob);
void	ShotReact	(objtype *ob);
void	StunnedReact	(objtype *ob);
void	ShotThink	(objtype *ob);
void	KeenPullUpThink1 (objtype *ob);
void	KeenPullUpThink2 (objtype *ob);
void	KeenPullUpThink3 (objtype *ob);
void	KeenPulledUpThink (objtype *ob);
void 	SpawnShot(unsigned x, unsigned y, int dir);
void	StunObj		(objtype *ob, objtype *shot, statetype near *stunstate);
void	ShotContact 	(objtype *ob, objtype *hit);

void	KeenHoldThink	(objtype *ob);
void	KeenReactWithPriority	(objtype *ob);

void	DrawZee(objtype *ob);

void	KeenExitThink(void);

#pragma warn -sus

//-------------------

statetype near s_keenexit1 = {KEENEXIT1SPR, KEENEXIT1SPR, step, false, false, 9, 0, -64, WalkSound, NULL, DrawReact5, &s_keenexit2};
statetype near s_keenexit2 = {KEENEXIT2SPR, KEENEXIT2SPR, step, false, false, 9, 0, -64, WalkSound, NULL, DrawReact5, &s_keenexit3};
statetype near s_keenexit3 = {KEENEXIT3SPR, KEENEXIT3SPR, step, false, false, 9, 0, -64, WalkSound, NULL, DrawReact5, &s_keenexit4};
statetype near s_keenexit4 = {KEENEXIT4SPR, KEENEXIT4SPR, step, false, false, 9, 0, -64, WalkSound, NULL, DrawReact5, &s_keenexit5};
statetype near s_keenexit5 = {KEENEXIT5SPR, KEENEXIT5SPR, step, false, false, 9, 0, -64, KeenExitThink, NULL, DrawReact5, &s_keenexit5};

//-------------------

statetype near s_keenpogodown	= {KEENPOGOL2SPR,KEENPOGOR2SPR,step,true,
	false,1, 0,0, KeenBounceThink, KeenContact, KeenPogoReact, &s_keenpogo};
statetype near s_keenpogo	= {KEENPOGOL2SPR,KEENPOGOR2SPR,think,true,
	false,0, 0,0, KeenPogoThink, KeenContact, KeenPogoReact, &s_keenpogo2};
statetype near s_keenpogo2	= {KEENPOGOL1SPR,KEENPOGOR1SPR,think,true,
	false,0, 0,0, KeenPogoThink, KeenContact, KeenPogoReact, NULL};

//-------------------

statetype near s_keenlosehealth	= {KEENLHSPR,KEENLHSPR,stepthink,false,
	true,10, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenstand2};
statetype near s_keenstand2	= {KEENSTANDLSPR,KEENSTANDRSPR,stepthink,false,
	true,4, 0,16, KeenHitThink, KeenContact, KeenStandReact, &s_keenstand};


//-------------------

statetype near s_keenzee1	= {ZEE1SPR,ZEE1SPR,step,false,
	false,30, 0,0, NULL, ZeeContact, DrawZee, &s_keenzee2};
statetype near s_keenzee2	= {ZEE2SPR,ZEE2SPR,step,false,
	false,30, 0,0, NULL, ZeeContact, DrawZee, &s_keenzee3};
statetype near s_keenzee3	= {ZEE3SPR,ZEE3SPR,step,false,
	false,30, 0,0, NULL, ZeeContact, DrawZee, &s_keenzee1};

//-------------------

statetype near s_keenstand	= {KEENSTANDLSPR,KEENSTANDRSPR,stepthink,false,
	true,4, 0,16, KeenPauseThink, KeenContact, KeenStandReact, &s_keenstand};
statetype near s_keenpauselook= {KEENLOOKULSPR,KEENLOOKURSPR,stepthink,false,
	true,60, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenstand};
statetype near s_keenyawn1	= {KEENYAWN1SPR,KEENYAWN1SPR,stepthink,false,
	true,40, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenyawn2};
statetype near s_keenyawn2	= {KEENYAWN2SPR,KEENYAWN2SPR,stepthink,false,
	true,40, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenyawn3};
statetype near s_keenyawn3	= {KEENYAWN3SPR,KEENYAWN3SPR,stepthink,false,
	true,40, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenyawn4};
statetype near s_keenyawn4	= {KEENYAWN4SPR,KEENYAWN4SPR,stepthink,false,
	true,40, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenstand};

statetype near s_keenwait1	= {KEENWAITL2SPR,KEENWAITR2SPR,stepthink,false,
	true,90, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenwait2};
statetype near s_keenwait2	= {KEENWAITL1SPR,KEENWAITR1SPR,stepthink,false,
	true,10, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenwait3};
statetype near s_keenwait3	= {KEENWAITL2SPR,KEENWAITR2SPR,stepthink,false,
	true,90, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenwait4};
statetype near s_keenwait4	= {KEENWAITL1SPR,KEENWAITR1SPR,stepthink,false,
	true,10, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenwait5};
statetype near s_keenwait5	= {KEENWAITL2SPR,KEENWAITR2SPR,stepthink,false,
	true,90, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenwait6};
statetype near s_keenwait6	= {KEENWAITL3SPR,KEENWAITR3SPR,stepthink,false,
	true,70, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenstand};

statetype s_keengosleep1= {KEENSLEEP1SPR,KEENSLEEP1SPR,stepthink,false,
	true,20, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keengosleep2};
statetype s_keengosleep2= {KEENSLEEP2SPR,KEENSLEEP2SPR,step,false,
	true,20, 0,0, KeenGoSleepThink, KeenContact, KeenStandReact, &s_keensleep1};
statetype s_keensleep1= {KEENSLEEP3SPR,KEENSLEEP3SPR,stepthink,false,
	true,120, 0,0, KeenSleepThink, KeenContact, KeenStandReact, &s_keensleep2};
statetype s_keensleep2= {KEENSLEEP4SPR,KEENSLEEP4SPR,stepthink,false,
	true,120, 0,0, KeenSleepThink, KeenContact, KeenStandReact, &s_keensleep1};

statetype near s_keenturn	= {KEENTURNLSPR,KEENTURNRSPR,stepthink,false,
	true,10, 0,0, KeenPauseThink, KeenContact, KeenStandReact, &s_keenstand};

statetype near s_keengetup	= {KEENGETUPLSPR,KEENGETUPRSPR,step,false,
	true,15, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenstand};

statetype near s_keendie1		= {KEENDREAM1SPR,KEENDREAM1SPR,step,false,
	false,20, 0,0, NULL, NULL, KeenSimpleReact, &s_keendie2};
statetype near s_keendie2		= {KEENDREAM2SPR,KEENDREAM2SPR,step,false,
	false,20, 0,0, NULL, NULL, KeenSimpleReact, &s_keendie3};
statetype near s_keendie3		= {KEENDREAM3SPR,KEENDREAM3SPR,step,false,
	false,120, 0,0, KeenDieThink, NULL, KeenSimpleReact, &s_keendie3};

statetype near s_keendead   = {-1, -1, think, false, false, 10, 0, 0, 0, 0, DrawReact, NULL};

statetype near s_keenlookup	= {KEENLOOKULSPR,KEENLOOKURSPR,think,false,
	true,0, 0,0, KeenStandThink, KeenLookUpDownContact, KeenStandReact, &s_keenlookup};

statetype near s_keenduck	= {KEENDUCKSPR,KEENDUCKSPR,think,false,
	true,0, 0,0, KeenDuckThink, KeenLookUpDownContact, KeenStandReact, &s_keenduck};

statetype near s_keenplace	= {KEENGETUPLSPR,KEENGETUPRSPR,step,true,
	true,15, 0,0, KeenThrow, KeenContact, KeenStandReact, &s_keenduck};	// K1n9_Duk3 mod 2025-05-26: switch to s_keenduck instead of s_keenstand

statetype near s_keenlowshoot1 = {KEENLOWSHOOTLSPR,KEENLOWSHOOTRSPR,step,false,
	true,9, 0,0, KeenShootThink, KeenContact, DrawReact9, &s_keenlowshoot2};
statetype near s_keenlowshoot2 = {KEENLOWSHOOTLSPR,KEENLOWSHOOTRSPR,step,false,
	true,6, 0,0, NULL, KeenContact, DrawReact9, &s_keenstand};

statetype near s_keenpickup	= {KEENGETUPLSPR,KEENGETUPRSPR,step,true,
	true,15, 0,0, NULL, KeenContact, KeenStandReact, &s_keenstand};

statetype near s_keendrop	= {KEENDUCKSPR,KEENDUCKSPR,step,false,
	false,0, 0,0, KeenDropDownThink, KeenContact, KeenSimpleReact, &s_keenjumpup3};

//-------------------

statetype near s_keenpole	= {KEENSHINNYL1SPR,KEENSHINNYR1SPR,think,false,
	false,0, 0,0, KeenPoleThink, KeenContact, KeenSimpleReact, &s_keenpole};

statetype near s_keenclimb1	= {KEENSHINNYL1SPR,KEENSHINNYR1SPR,slidethink,false,
	false,8, 0,8, KeenClimbThink, KeenContact, KeenSimpleReact, &s_keenclimb2};
statetype near s_keenclimb2	= {KEENSHINNYL2SPR,KEENSHINNYR2SPR,slidethink,false,
	false,8, 0,8, KeenClimbThink, KeenContact, KeenSimpleReact, &s_keenclimb3};
statetype near s_keenclimb3	= {KEENSHINNYL3SPR,KEENSHINNYR3SPR,slidethink,false,
	false,8, 0,8, KeenClimbThink, KeenContact, KeenSimpleReact, &s_keenclimb1};

statetype near s_keenslide1	= {KEENSLIDED1SPR,KEENSLIDED1SPR,slide,false,
	false,8, 0,24, KeenDropThink, KeenContact, KeenSimpleReact, &s_keenslide2};
statetype near s_keenslide2	= {KEENSLIDED2SPR,KEENSLIDED2SPR,slide,false,
	false,8, 0,24, KeenDropThink, KeenContact, KeenSimpleReact, &s_keenslide3};
statetype near s_keenslide3	= {KEENSLIDED3SPR,KEENSLIDED3SPR,slide,false,
	false,8, 0,24, KeenDropThink, KeenContact, KeenSimpleReact, &s_keenslide4};
statetype near s_keenslide4	= {KEENSLIDED4SPR,KEENSLIDED4SPR,slide,false,
	false,8, 0,24, KeenDropThink, KeenContact, KeenSimpleReact, &s_keenslide1};

statetype near s_keenpolethrow1	= {KEENPTHROWL1SPR,KEENPTHROWR1SPR,step,false,
	false,8, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenpolethrow2};
statetype near s_keenpolethrow2	= {KEENPTHROWL2SPR,KEENPTHROWR2SPR,step,true,
	false,1, 0,0, KeenThrow, KeenContact, KeenSimpleReact, &s_keenpolethrow3};
statetype near s_keenpolethrow3	= {KEENPTHROWL2SPR,KEENPTHROWR2SPR,step,false,
	false,10, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenpole};

statetype near s_keenpolethrowup1 = {KEENPLTHROWU1SPR,KEENPRTHROWU1SPR,step,false,
	false,8, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenpolethrowup2};
statetype near s_keenpolethrowup2 = {KEENPLTHROWU2SPR,KEENPRTHROWU2SPR,step,true,
	false,1, 0,0, KeenThrow, KeenContact, KeenSimpleReact, &s_keenpolethrowup3};
statetype near s_keenpolethrowup3 = {KEENPLTHROWU2SPR,KEENPRTHROWU2SPR,step,false,
	false,10, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenpole};

statetype near s_keenpolethrowdown1 = {KEENPLTHROWD1SPR,KEENPRTHROWD1SPR,step,false,
	false,8, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenpolethrowdown2};
statetype near s_keenpolethrowdown2 = {KEENPLTHROWD2SPR,KEENPRTHROWD2SPR,step,true,
	false,1, 0,0, KeenThrow, KeenContact, KeenSimpleReact, &s_keenpolethrowdown3};
statetype near s_keenpolethrowdown3 = {KEENPLTHROWD2SPR,KEENPRTHROWD2SPR,step,false,
	false,10, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenpole};


//-------------------

statetype near s_keenwalk1	= {KEENRUNL1SPR,KEENRUNR1SPR,slidethink,true,
	true,6, 24,0, KeenWalkThink, KeenContact, KeenWalkReact, &s_keenwalk2};
statetype near s_keenwalk2	= {KEENRUNL2SPR,KEENRUNR2SPR,slidethink,true,
	true,6, 24,0, KeenWalkThink, KeenContact, KeenWalkReact, &s_keenwalk3};
statetype near s_keenwalk3	= {KEENRUNL3SPR,KEENRUNR3SPR,slidethink,true,
	true,6, 24,0, KeenWalkThink, KeenContact, KeenWalkReact, &s_keenwalk4};
statetype near s_keenwalk4	= {KEENRUNL4SPR,KEENRUNR4SPR,slidethink,true,
	true,6, 24,0, KeenWalkThink, KeenContact, KeenWalkReact, &s_keenwalk1};

statetype near s_keenthrow1	= {KEENTHROWL1SPR,KEENTHROWR1SPR,step,true,
	true,4, 0,0, NULL, KeenContact, KeenStandReact, &s_keenthrow2};
statetype near s_keenthrow2	= {KEENTHROWL2SPR,KEENTHROWR2SPR,step,false,
	true,4, 0,0, NULL, KeenContact, KeenStandReact, &s_keenthrow3};
statetype near s_keenthrow3	= {KEENTHROWL3SPR,KEENTHROWR3SPR,step,true,
	true,1, 0,0, KeenThrow, KeenContact, KeenStandReact, &s_keenthrow4};
statetype near s_keenthrow4	= {KEENTHROWL3SPR,KEENTHROWR3SPR,step,false,
	true,10, 0,0, NULL, KeenContact, KeenStandReact, &s_keenstand};

statetype near s_keenthrowup1	= {KEENTHROWU3SPR,KEENTHROWU1SPR,step,false,
	true,8, 0,0, NULL, KeenContact, KeenStandReact, &s_keenthrowup2};
statetype near s_keenthrowup2	= {KEENTHROWU4SPR,KEENTHROWU2SPR,step,true,
	true,1, 0,0, KeenThrow, KeenContact, KeenStandReact, &s_keenthrowup3};
statetype near s_keenthrowup3	= {KEENTHROWU4SPR,KEENTHROWU2SPR,step,false,
	true,10, 0,0, NULL, KeenContact, KeenStandReact, &s_keenstand};

//-------------------

statetype near s_keenfell	= {KEENSTUNNEDSPR,KEENSTUNNEDSPR,step,false,
	true,60, 0,0, ProjectileThink, KeenContact, KeenStandReact, &s_keengetup};

statetype near s_keenjumpup1	= {KEENJUMPUL1SPR,KEENJUMPUR1SPR,think,false,
	false,0, 0,0, KeenAirThink, KeenContact, KeenAirReact, &s_keenjumpup2};
statetype near s_keenjumpup2	= {KEENJUMPUL2SPR,KEENJUMPUR2SPR,think,false,
	false,0, 0,0, KeenAirThink, KeenContact, KeenAirReact, &s_keenjumpup3};
statetype near s_keenjumpup3	= {KEENJUMPUL1SPR,KEENJUMPUR1SPR,think,false,
	false,0, 0,0, KeenAirThink, KeenContact, KeenAirReact, &s_keenjumpup3};

statetype near s_keenjump1	= {KEENJUMPL1SPR,KEENJUMPR1SPR,think,false,
	false,0, 0,0, KeenAirThink, KeenContact, KeenAirReact, &s_keenjump2};
statetype near s_keenjump2	= {KEENJUMPL2SPR,KEENJUMPR2SPR,think,false,
	false,0, 0,0, KeenAirThink, KeenContact, KeenAirReact, &s_keenjump3};
statetype near s_keenjump3	= {KEENJUMPL3SPR,KEENJUMPR3SPR,think,false,
	false,0, 0,0, KeenAirThink, KeenContact, KeenAirReact, &s_keenjump3};

statetype near s_keenairthrow1= {KEENJLTHROWL1SPR,KEENJRTHROWR1SPR,stepthink,false,
	false,8, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenairthrow2};
statetype near s_keenairthrow2= {KEENJLTHROWL2SPR,KEENJRTHROWR2SPR,step,true,
	false,1, 0,0, KeenThrow, KeenContact, KeenAirReact, &s_keenairthrow3};
statetype near s_keenairthrow3= {KEENJLTHROWL2SPR,KEENJRTHROWR2SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenjumpup3};

statetype near s_keenairthrowup1= {KEENJLTHROWU1SPR,KEENJRTHROWU1SPR,stepthink,false,
	false,8, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenairthrowup2};
statetype near s_keenairthrowup2= {KEENJLTHROWU2SPR,KEENJRTHROWU2SPR,step,true,
	false,1, 0,0, KeenThrow, KeenContact, KeenAirReact, &s_keenairthrowup3};
statetype near s_keenairthrowup3= {KEENJLTHROWU2SPR,KEENJRTHROWU2SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenjumpup3};

statetype near s_keenairthrowdown1= {KEENJLTHROWD1SPR,KEENJRTHROWD1SPR,stepthink,false,
	false,8, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenairthrowdown2};
statetype near s_keenairthrowdown2= {KEENJLTHROWD2SPR,KEENJRTHROWD2SPR,step,true,
	false,1, 0,0, KeenThrow, KeenContact, KeenAirReact, &s_keenairthrowdown3};
statetype near s_keenairthrowdown3= {KEENJLTHROWD2SPR,KEENJRTHROWD2SPR,stepthink,false,
	false,10, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenjumpup3};
    
statetype near s_keenholdon= {KEENHANGLSPR,KEENHANGRSPR,step,false,
	false,12, 0,0, NULL, KeenContact, KeenReactWithPriority, &s_keenholdon2};
statetype near s_keenholdon2= {KEENHANGLSPR,KEENHANGRSPR,think,false,
	false,0, 0,0, KeenHoldThink, KeenContact, KeenReactWithPriority, NULL};

statetype near s_keenclimbup= {KEENCLIMBEDGEL1SPR,KEENCLIMBEDGER1SPR,step,false,
	false,10, 0,0, KeenPullUpThink1, KeenContact, KeenReactWithPriority, &s_keenclimbup2};
statetype near s_keenclimbup2= {KEENCLIMBEDGEL2SPR,KEENCLIMBEDGER2SPR,step,false,
	false,10, 0,0, KeenPullUpThink2, KeenContact, KeenReactWithPriority, &s_keenclimbup3};
statetype near s_keenclimbup3= {KEENCLIMBEDGEL3SPR,KEENCLIMBEDGER3SPR,step,false,
	false,10, 0,0, KeenPullUpThink3, KeenContact, KeenReactWithPriority, &s_keenclimbup4};
statetype near s_keenclimbup4= {KEENCLIMBEDGEL4SPR, KEENCLIMBEDGER4SPR,step,false,
	false,10, 0,0, KeenPulledUpThink, KeenContact, KeenReactWithPriority, &s_keenclimbup5};
statetype near s_keenclimbup5= {KEENSTANDLSPR,KEENSTANDRSPR,step,false,
	false,6, 0,0, NULL, KeenContact, KeenSimpleReact, &s_keenstand};

statetype near s_keenshoot1 = {KEENSHOOTLSPR, KEENSHOOTRSPR,step,false,
	true,9, 0,0, KeenShootThink, KeenContact, KeenStandReact, &s_keenshoot2};
statetype near s_keenshoot2 = {KEENSHOOTLSPR, KEENSHOOTRSPR,step,false,
	true,6, 0,0, NULL, KeenContact, KeenStandReact, &s_keenstand};

statetype near s_keenshootup1 = {KEENSHOOTUSPR, KEENSHOOTUSPR,step,false,
	true,9, 0,0, KeenShootThink, KeenContact, KeenStandReact, &s_keenshootup2};
statetype near s_keenshootup2 = {KEENSHOOTUSPR, KEENSHOOTUSPR,step,false,
	true,6, 0,0, NULL, KeenContact, KeenStandReact, &s_keenlookup};

statetype near s_keenpoleshoot1 = {KEENPSHOOTLSPR, KEENPSHOOTRSPR,step,false,
	false,9, 0,0, KeenShootThink, KeenLookUpDownContact, KeenSimpleReact, &s_keenpoleshoot2};
statetype near s_keenpoleshoot2 = {KEENPSHOOTLSPR, KEENPSHOOTRSPR,step,false,
	false, 6, 0,0, NULL, KeenLookUpDownContact, KeenSimpleReact, &s_keenpole};

statetype near s_keenpoleshootup1 = {KEENPLSHOOTUSPR, KEENPRSHOOTUSPR,step,false,
	false, 9, 0,0, KeenShootThink, KeenLookUpDownContact, KeenSimpleReact, &s_keenpoleshootup2};
statetype near s_keenpoleshootup2 = {KEENPLSHOOTUSPR, KEENPRSHOOTUSPR,step,false,
	false, 6, 0,0, NULL, KeenLookUpDownContact, KeenSimpleReact, &s_keenpole};

statetype near s_keenpoleshootdown1 = {KEENPLSHOOTDSPR, KEENPRSHOOTDSPR,step,false,
	false, 9, 0,0, KeenShootThink, KeenLookUpDownContact, KeenSimpleReact, &s_keenpoleshootdown2};
statetype near s_keenpoleshootdown2 = {KEENPLSHOOTDSPR, KEENPRSHOOTDSPR,step,false,
	false, 6, 0,0, NULL, KeenLookUpDownContact, KeenSimpleReact, &s_keenpole};

statetype near s_keenairshoot1 = {KEENJLSHOOTLSPR, KEENJRSHOOTRSPR,stepthink,false,
	false, 9, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenairshoot2};
statetype near s_keenairshoot2 = {KEENJLSHOOTLSPR, KEENJRSHOOTRSPR,stepthink,true,
	false, 1, 0,0, KeenShootThink, KeenContact, KeenAirReact, &s_keenairshoot3};
statetype near s_keenairshoot3 = {KEENJLSHOOTLSPR, KEENJRSHOOTRSPR,stepthink,false,
	false, 6, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenjump3};

statetype near s_keenairshootup1 = {KEENJSHOOTUSPR, KEENJSHOOTUSPR, stepthink,false,
	false, 9, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenairshootup2};
statetype near s_keenairshootup2 = {KEENJSHOOTUSPR, KEENJSHOOTUSPR, stepthink,true,
	false, 1, 0,0, KeenShootThink, KeenContact, KeenAirReact, &s_keenairshootup3};
statetype near s_keenairshootup3 = {KEENJSHOOTUSPR, KEENJSHOOTUSPR, stepthink,false,
	false, 6, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenjump3};

statetype near s_keenairshootdown1 = {KEENJSHOOTDSPR, KEENJSHOOTDSPR, stepthink,false,
	false, 9, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenairshootdown2};
statetype near s_keenairshootdown2 = {KEENJSHOOTDSPR, KEENJSHOOTDSPR, stepthink,true,
	false, 1, 0,0, KeenShootThink, KeenContact, KeenAirReact, &s_keenairshootdown3};
statetype near s_keenairshootdown3 = {KEENJSHOOTDSPR, KEENJSHOOTDSPR, stepthink,false,
	false, 6, 0,0, ProjectileThink, KeenContact, KeenAirReact, &s_keenjump3};

statetype near s_vortray1 = {STUN1SPR, STUN1SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray2};
statetype near s_vortray2 = {STUN2SPR, STUN2SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray3};
statetype near s_vortray3 = {STUN3SPR, STUN3SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray4};
statetype near s_vortray4 = {STUN4SPR, STUN4SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray1};

statetype near s_vorthit1  = {STUNHIT1SPR, STUNHIT1SPR, step,false,
	false, 12, 0,0, NULL, NULL, DrawReact, &s_vorthit2};
statetype near s_vorthit2 = {STUNHIT2SPR, STUNHIT2SPR, step,false,
	false, 12, 0,0, NULL, NULL, DrawReact, NULL};

statetype near s_vortray5 = {STUN5SPR, STUN5SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray6};
statetype near s_vortray6 = {STUN6SPR, STUN6SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray7};
statetype near s_vortray7 = {STUN7SPR, STUN7SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray8};
statetype near s_vortray8 = {STUN8SPR, STUN8SPR, slide,false,
	false, 6, 64,64, ShotThink, NULL, ShotReact, &s_vortray5};

statetype near s_vorthit3  = {STUNHIT3SPR, STUNHIT3SPR, step,false,
	false, 12, 0,0, NULL, NULL, DrawReact, &s_vorthit4};
statetype near s_vorthit4 = {STUNHIT4SPR, STUNHIT4SPR, step,false,
	false, 12, 0,0, NULL, NULL, DrawReact, NULL};


//===========================================================================

#pragma warn +sus

void DrawMort(objtype *ob, int priority)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += MORTSTANDRSPR - KEENSTANDRSPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,priority);
}

void DrawZee(objtype *ob)
{
	int shape = ob->shapenum;
	
	if (shape != 0 && shape != -1 && gamestate.difficulty == gd_Grand)
	{
		shape += ZEE4SPR - ZEE1SPR;
	}
	RF_PlaceSprite(&ob->sprite,ob->x,ob->y,shape,spritedraw,0);
}


/*
===============
=
= SpawnKeen
=
===============
*/

void SpawnKeen (int tilex, int tiley, int dir)
{
	player->obclass = keenobj;
	player->active = yes;
	player->needtoclip = true;
	player->x = tilex<<G_T_SHIFT;
	player->y = (tiley<<G_T_SHIFT) - BLOCKSIZE*2;

	player->xdir = dir;
	player->ydir = 1;
	NewState (player,&s_keenstand);
}


/*
===============
=
= StunObj
=
===============
*/

void StunObj(objtype *ob, objtype *shot, statetype near *stunstate)
{
	ExplodeShot(shot);
	ob->temp1 = ob->temp2 = ob->temp3 = 0;	// Note: ob->nothink should also be set to 0
	ob->temp4 = ob->obclass;
	ChangeState(ob, stunstate);
	ob->obclass = stunnedobj;
}

//==========================================================================

/*
======================
=
= CheckGrabPole
=
======================
*/

boolean	CheckGrabPole (objtype *ob)
{
	int	x;
	uint16_t  *map;

//
// kludgy bit to not let you grab a pole the instant you jump off it
//
	if (TimeCount < leavepoletime)
		leavepoletime = 0;
	else if (TimeCount - leavepoletime < MINPOLEJUMPTICS)
		return false;

	if (c.yaxis == -1)
		map = (uint16_t *)mapsegs[1]+
			mapbwidthtable[(ob->top+6*PIXGLOBAL)/TILEGLOBAL]/2;
	else
		map = (uint16_t *)mapsegs[1]+
			mapbwidthtable[ob->tilebottom]/2;

	x = (ob->left + (ob->right - ob->left)/2) >>G_T_SHIFT;

	map += x;

	if ((tinf[INTILE+*map]&0x7f) == 1)
	{
		ob->xmove = ((x<<G_T_SHIFT)-8*PIXGLOBAL) - ob->x;
		ob->ymove = c.yaxis*32;
		ob->temp4 = x;				// for future reference
		ob->needtoclip = false;		// can climb through pole holes
		ob->state = &s_keenpole;
		return true;
	}

	return false;
}


//==========================================================================

/*
============================
=
= WalkSound
=
============================
*/

void WalkSound(objtype *ob)
{
	SD_PlaySound(KEENWALKSND);
	ob++;			// shut up compiler
}


//==========================================================================

/*
============================
=
= KeenStandThink
=
============================
*/

void KeenStandThink (objtype *ob)
{

	if (c.xaxis)
	{
	// started walking
		ob->xdir = c.xaxis;
		ob->state = &s_keenwalk1;
		ob->xmove = ob->xdir*s_keenwalk1.xmove*tics;
		KeenWalkThink(ob);
		return;
	}
	else if (attackbutton && !attackheld)
	{
	// throw or shoot current xdir
		attackheld = true;
		if (c.yaxis == -1)
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenshootup1;
			else
				ob->state = &s_keenthrowup1;
		else if (c.yaxis == 1)	// K1n9_Duk3 mod 2025-05-26: use Down instead of placing key
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenlowshoot1;	// Szemigi addition: Shooting while ducking.
			else
				ob->state = &s_keenplace;
		else
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenshoot1; 
			else
				ob->state = &s_keenthrow1;
		pogoheld = true;
		return;
	}
	else if (jumpbutton && !jumpheld)
	{
	// jump straight up
		SD_PlaySound (JUMPSND);
		ob->xspeed = 0;
		ob->yspeed = -SPDJUMP;
		ob->xmove = 0;
		ob->ymove = 0;		// ob->yspeed*tics;			// DEBUG
		jumptime = JUMPTIME;	// -tics;
		ob->state = &s_keenjumpup1;
		jumpheld = true;
		return;
	}
	else if (pogobutton && !pogoheld)
	{
	// get on pogo
		pogoheld = true;
		SD_PlaySound(JUMPSND);
		ob->state = &s_keenpogodown;
		ob->xspeed = 0;
		ob->yspeed = -48;
		ob->ymove = 0;	// K1n9_Duk3 mod 2025-05-26: replaced ytry with ob->ymove
		jumptime = 24;
	}
	else
	{
		// K1n9_Duk3 mod 2025-05-27: cleaned up the code to make sure that it
		// works for joystick input and that it still works correctly when
		// rebinding the up/down keys
		switch (c.yaxis)
		{
		case -1:
			if (!CheckGrabPole(ob))			// try to go up/down pole first
			{
				ob->state = &s_keenlookup;
			}
			return;
			
		case 0:
			ob->state = &s_keenstand;
			return;
			
		case 1:
			if (!CheckGrabPole(ob))			// try to go up/down pole first
			{
				ob->state = &s_keenduck;
			}
			return;
		}
		// end of K1n9_Duk3 mod
	}
}

//===========================================================================

/*
=======================
=
= KeenPauseThink
=
= Do special animations in time
=
=======================
*/

void KeenPauseThink (objtype *ob)
{

	if (c.dir != dir_None || jumpbutton || pogobutton || attackbutton)
	{
		ob->temp1 = ob->temp2 = 0;			// not paused any more
		ob->state = &s_keenstand;
		KeenStandThink(ob);
		return;
	}

	if ((ob->hitnorth & ~7) != 24)	// if it is 0 now, keen is standing on a sprite
		ob->temp1 += tics;

	switch (ob->temp2)
	{
	case 0:
		if (ob->temp1 > 200)
		{
			ob->temp2++;
			ob->state = &s_keenpauselook;
			ob->temp1 = 0;
		}
		break;
	case 1:
		if (ob->temp1 > 300)
		{
			ob->temp2++;
			ob->state = &s_keenwait1;
			ob->temp1 = 0;
		}
		break;
	case 2:
		if (ob->temp1 > 700)
		{
			ob->temp2++;
			ob->state = &s_keenyawn1;
			ob->temp1 = 0;
		}
		break;
	case 3:
		if (ob->temp1 > 400)
		{
			ob->temp2++;
			ob->state = &s_keengosleep1;
			ob->temp1 = 0;
		}
		break;
	}
}

/*
====================================
=
= KeenHitThink
=
= When Keen gets hit in keen mode
=
====================================
*/

void KeenHitThink (objtype *ob)
{
	if (c.dir != dir_None || jumpbutton || pogobutton || attackbutton)
	{
		ob->temp1 = ob->temp2 = 0;			// not paused any more
		ChangeState (player,&s_keenlosehealth);
		KeenStandThink(ob);
		return;
	}
}

//===========================================================================

/*
=======================
=
= KeenGoSleepThink
=
=======================
*/

void KeenGoSleepThink (objtype *ob)
{
//
// spawn the zees above keens head
//
	GetNewObj (false);

	new->obclass = zeeobj;
	new->x = player->x;
	new->y = player->y-4*PIXGLOBAL;
	new->xdir = ob->xdir;
	new->ydir = ob->ydir;
	NewState (new,&s_keenzee1);
	ob->temp1 = ObjToInt(new);			// so they can be removed later
}


//===========================================================================

// Note from Szemigi: In the official game, I've realized that if Keen falls asleep
// (when the 'Zzz' animation sequence appears above his head) at a deadly enemy
// or on a hazard when god mode is turned on and then the player turns god mode off,
// Keen dies and the 'Zzz' sequence doesn't get removed. Now ZeeContact fixes
// this bug!

/*
===========================
=
= ZeeContact
=
===========================
*/

void ZeeContact(objtype *ob)
{
	ob->obclass = zeeobj;
	if (player->state == &s_keendie1 || player->state == &s_keenlosehealth || player->state == &s_keenfell)
	{
		RemoveObj(ob);
	}
	else
	{
		return;
	}
}

/*
=======================
=
= KeenSleepThink
=
=======================
*/

void KeenSleepThink (objtype *ob)
{
	if (c.dir != dir_None || jumpbutton || pogobutton || attackbutton)
	{
		if (ob->temp1  && ob->temp1 != (unsigned)&dummyobj)
			RemoveObj (IntToObj(ob->temp1));	// remove the zees
		ob->temp1 = ob->temp2 = 0;			// not paused any more
		ob->state = &s_keengetup;
	}
}


//===========================================================================

// New "pull up" think functions

/*
=======================
=
= KeenPullUpThink1
=
=======================
*/

void KeenPullUpThink1(objtype *ob)
{
	if (ob->xdir == 1)
	{
		ob->xmove = 8*PIXGLOBAL;
	}
	else
	{
		ob->ymove = -8*PIXGLOBAL;
	}
}

/*
=======================
=
= KeenPullUpThink2
=
=======================
*/

void KeenPullUpThink2(objtype *ob)
{
	if (ob->xdir == 1)
	{
		ob->xmove = 8*PIXGLOBAL;
		ob->ymove = -8*PIXGLOBAL;
	}
	else
	{
		ob->xmove = -8*PIXGLOBAL;
		ob->ymove = -8*PIXGLOBAL;
	}
}

/*
=======================
=
= KeenPullUpThink3
=
=======================
*/

#pragma argsused
void KeenPullUpThink3(objtype *ob)
{

	ob->ymove = -8*PIXGLOBAL;
}

/*
=======================
=
= KeenPulledUpThink
=
=======================
*/

void KeenPulledUpThink(objtype *ob)
{
	ob->needtoclip = true;
#if 0
	ob->priority = 1;
#endif
	ob->ymove = 8*PIXGLOBAL;
}

//===========================================================================

/*
=======================
=
= KeenDieThink
=
=======================
*/

void KeenDieThink (objtype *ob)
{
	ob++;			// shut up compiler
	playstate = died;
}


//===========================================================================

/*
=======================
=
= KeenDuckThink
=
=======================
*/

#define JUMPDOWNMUSHROOMSIZE 0x100

void KeenDuckThink (objtype *ob)
{
	uint16_t  *map, tile;
	int midtile,bottomtile,move;

	if (c.yaxis != 1)
	{
		ob->state = &s_keenstand;
		KeenStandThink(ob);
		return;
	}

	if (c.xaxis)
		ob->xdir = c.xaxis;

	if (jumpbutton && !jumpheld)
	{
	//
	// drop down a level
	//
		jumpheld = true;

		if (onmushroom)
		{
			// kludge to allow jumping down from mushroom
			onmushroom = false;
			riding = NULL;
			move = JUMPDOWNMUSHROOMSIZE;
		}
		else
		{
			midtile = (ob->tileleft + ob->tileright) >> 1;
			bottomtile = ob->tilebottom;
			map = (uint16_t *)mapsegs[1] + mapbwidthtable[bottomtile]/2
				+ midtile;
			tile = *map;
			if (tinf[WESTWALL+tile] || tinf[EASTWALL+tile]
				|| tinf[SOUTHWALL+tile])
				return;				// wall prevents drop down

			map += mapwidth;
			tile = *map;
			if (tinf[WESTWALL+tile] || tinf[EASTWALL+tile]
				|| tinf[SOUTHWALL+tile])
				return;				// wall prevents drop down

			move = PIXGLOBAL*(tics<4 ? 4: tics);
		}
		ob->bottom += move;
		ob->y += move;
		ob->xmove = ob->ymove = 0;
		ob->state = &s_keenjumpup3;
		ob->temp2 = 1;		// allready in last stage
		ob->xspeed = ob->yspeed = 0;
		SD_PlaySound (PLUMMETSND);
	}
	else if (attackbutton && !attackheld)	// K1n9_Duk3 addition 2025-05-26
	{
		if (mapon >= 23 && mapon <= 46)
			ob->state = &s_keenlowshoot1;	// Szemigi addition: Shooting while ducking.
		else
			ob->state = &s_keenplace;
		attackheld = true;	// disable this if you want Keen to throw endlessly until the button is no longer pressed
	}
	else
	{
		ob->state = &s_keenduck;
	}
}


//===========================================================================

/*
=======================
=
= KeenExitThink
=
=======================
*/

void KeenExitThink(void)
{
	playstate = levelcomplete;
}


//===========================================================================

/*
=======================
=
= KeenHoldThink
=
=======================
*/


void KeenHoldThink(objtype *ob)
{
	int tile;

	if (c.yaxis == -1 || ob->xdir == c.xaxis)
	{
		ob->state = &s_keenclimbup;
		if (ob->xdir == 1)
		{
			tile = *(mapsegs[1]+mapbwidthtable[ob->tiletop-1]/2+ob->tileright);
		}
		else
		{
			tile = *(mapsegs[1]+mapbwidthtable[ob->tiletop-1]/2+ob->tileleft);
		}
		if (ob->xdir == 1)
		{
			ob->ymove = -16*PIXGLOBAL;
		}
		else
		{
			ob->ymove = -8*PIXGLOBAL;
		}
#if 0
		if (!(tinf[INTILE+tile] & INTILE_FOREGROUND))
			ob->priority = 3;
#else
	(void)tile; // TODO, TODO
#endif
	}
	else if (c.yaxis == 1 || c.xaxis && ob->xdir != c.xaxis)
	{
		ob->state = &s_keenjump3;
		ob->needtoclip = true;
	}
}


//===========================================================================

/*
=======================
=
= KeenDropDownThink
=
=======================
*/

void KeenDropDownThink (objtype *ob)
{
	ob->needtoclip = true;
}


//===========================================================================

/*
=======================
=
= KeenWalkThink
=
=======================
*/

unsigned slopespeed[8] = {0,0,4,4,8,-4,-4,-8};

void KeenWalkThink (objtype *ob)
{
	int move;

	if (!c.xaxis)
	{
		// K1n9_Duk3 fix: change state FIRST because the function call might
		// trigger other state transitions and we DON'T want to cancel those
		ob->state = &s_keenturn;
		
		// K1n9_Duk3 fix: I'm still not sure if we need to change the clipping
		// at all at this point, but we definitely can't change it after calling
		// KeenPauseThink, since Keen might have started climbing a pole and that
		// needs to be done in noclip mode. I think this is just here to fix a
		// glitch that was caused by changing Keen from pole climbing back into
		// the turning state in the old version of this code, where the state
		// change and the clipping change were done after calling KeenStandThink.
		ob->needtoclip = true;
		
		// K1n9_Duk3 fix: call KeenPauseThink, not KeenStandThink

		KeenPauseThink (ob);
		return;
	}

	ob->xdir = c.xaxis;

	if (c.yaxis && CheckGrabPole(ob))		// try to go up/down pole
		return;

	if (attackbutton && !attackheld)
	{
	//
	// throw or shoot
	//
		attackheld = true;
		if (mapon >= 23 && mapon <= 46)
			ob->state = &s_keenshoot1;
		else
			ob->state = &s_keenthrow1;
		pogoheld = true;
		return;
	}

	if (jumpbutton && !jumpheld)
	{
	//
	// running jump
	//
		SD_PlaySound (JUMPSND);
		ob->xspeed = ob->xdir * SPDRUNJUMP;
		ob->yspeed = -SPDJUMP;
		ob->xmove = 0;
		ob->ymove = 0;					// ob->yspeed*tics;
		jumpheld = true;
		jumptime = JUMPTIME; //-tics;		// DEBUG
		ob->state = &s_keenjump1;
	}

	if (pogobutton && !pogoheld)
	{
	//
	// get on pogo
	//
		pogoheld = true;
		ob->state = &s_keenpogodown;
		SD_PlaySound(JUMPSND);
		ob->xspeed = ob->xdir * 16;
		ob->yspeed = -48;
		ob->xmove = 0;	// K1n9_Duk3 mod 2025-05-26: replaced xtry with ob->xmove
		jumptime = 24;
		return;
	}

	//
	// give speed for slopes
	//
	move = tics*slopespeed[ob->hitnorth&7];

	ob->xmove += move;

	//
	// handle walking sounds - taken from the reconstructed Keen 4-6 source
	//
	if (ob->state == &s_keenwalk1 && ob->temp3 == 0)
	{
		SD_PlaySound(KEENWALKSND);
		ob->temp3 = 1;
	}
	else if (ob->state == &s_keenwalk3 && ob->temp3 == 0)
	{
		SD_PlaySound(KEENWALKSND);
		ob->temp3 = 1;
	}
	else if (ob->state == &s_keenwalk2 ||ob->state == &s_keenwalk4)
	{
		ob->temp3 = 0;
	}
}

//===========================================================================

/*
=======================
=
= KeenAirThink
=
=======================
*/

void	KeenAirThink		(objtype *ob)
{
	if (jumpcheat && jumpbutton)
	{
		ob->yspeed = -40;
		jumptime = 18;
		jumpheld = true;
	}
	if (jumptime)
	{
		if (jumptime<tics)
		{
			ob->ymove = ob->yspeed*jumptime;
			jumptime = 0;
		}
		else
		{
			ob->ymove = ob->yspeed*tics;
			if (!jumpcheat)
				jumptime-=tics;
		}
		if (!jumpbutton)
			jumptime = 0;

		if (!jumptime)
		{
			ob->temp2 = 0;
			ob->state = ob->state->nextstate;	// switch to second jump stage
		}
	}
	else
	{
		if (gamestate.difficulty == gd_Easy)
		{
			DoMidGravity(ob);
		}
		else if (gamestate.difficulty == gd_Normal)
		{
			DoWeakGravity(ob);
		}
		else
		{
			DoHighGravity(ob);
		}
		if (ob->yspeed>0 && !ob->temp2)
		{
			ob->state = ob->state->nextstate;	// switch to third jump stage
			ob->temp2 = 1;
		}
	}

//-------------

	if (c.xaxis)
	{
		ob->xdir = c.xaxis;
		AccelerateX(ob, c.xaxis * 2, MAXXSPEED);
	}
	else
	{
		FrictionX(ob);
	}

	if (ob->hitsouth == 17)		// going through a pole hole
	{
		ob->xspeed = ob->xmove = 0;	// K1n9_Duk3 mod 2025-05-26: replaced xtry with ob->xmove
	}

	if (attackbutton && !attackheld)
	{
		attackheld = true;
	//
	// throw or shoot
	//
		switch (c.yaxis)
		{
		case -1:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenairshootup1;
			else
				ob->state = &s_keenairthrowup1;
			return;
		case 0:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenairshoot1;
			else
				ob->state = &s_keenairthrow1;
			return;
		case 1:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenairshootdown1;
			else
				ob->state = &s_keenairthrowdown1;
			return;
		}		
	}

	if (pogobutton && !pogoheld)
	{
		pogoheld = true;
		ob->state = &s_keenpogo;
		jumptime = 0;
		return;
	}

//	if (ob->hitsouth==17)		// going through a pole hole
//	{
//		ob->xspeed = ob->xmove = 0;
//	}


	if (c.yaxis == -1)
		CheckGrabPole (ob);

}

//===========================================================================

/*
=======================
=
= KeenBounceThink
=
= Gives an extra bit of height on the first pogo bounce and creates
= the "impossible pogo trick" when the jump key is held down
=
=======================
*/

void KeenBounceThink(objtype *ob)
{
	ob->yspeed = -48;
	ob->ymove = ob->yspeed * 6;	// K1n9_Duk3 mod 2025-05-26: replaced ytry with ob->ymove
	jumptime = 24;
	SD_PlaySound (POGOSND);
}

//===========================================================================

/*
=======================
=
= KeenPogoThink
=
=======================
*/

void KeenPogoThink(objtype *ob)
{
	if (jumptime)
	{
		if (jumpbutton || jumptime <= 9)
		{
			DoLowGravity(ob);
		}
		else
		{
			DoHighGravity(ob);
		}
		if (jumptime <= tics)
		{
			jumptime = 0;
		}
		else
		{
			jumptime = jumptime - tics;
		}
		if (jumptime == 0 && ob->state->nextstate)
			ob->state = ob->state->nextstate;
	}
	else
	{
		if (gamestate.difficulty == gd_Easy)
		{
			DoMidGravity(ob);
		}
		else if (gamestate.difficulty == gd_Normal)
		{
			DoWeakGravity(ob);
		}
		else
		{
			DoHighGravity(ob);
		}
	}

	if (c.xaxis)
	{
		if (ob->xspeed == 0)
			ob->xdir = c.xaxis;
		AccelerateX(ob, c.xaxis, 24);
	}
	else
	{
		ob->xmove = ob->xmove + ob->xspeed * tics;	// K1n9_Duk3 mod 2025-05-26: replaced xtry with ob->xmove
		if (ob->xspeed > 0)
		{
			ob->xdir = 1;
		}
		else if (ob->xspeed < 0)
		{
			ob->xdir = -1;
		}
	}

	if (ob->hitsouth == 17)		// going through a pole hole
	{
		ob->xspeed = ob->xmove = 0;	// K1n9_Duk3 mod 2025-05-26: replaced xtry with ob->xmove
	}

	if (attackbutton && !attackheld)
	{
		attackheld = true;
		switch (c.yaxis)
		{
		case -1:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenairshootup1;
			else
				ob->state = &s_keenairthrowup1;
			return;
		case 0:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenairshoot1;
			else
				ob->state = &s_keenairthrow1;
			return;
		case 1:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenairshootdown1;
			else
				ob->state = &s_keenairthrowdown1;
			return;		
		}
	}

	if (pogobutton && !pogoheld)
	{
		pogoheld = true;
		ob->state = &s_keenjump3;
	}
}

/*
============================
=
= KeenPogoReact
=
============================
*/

void KeenPogoReact(objtype *ob)
{
	if (ob->hiteast && ob->xdir == -1 || ob->hitwest && ob->xdir == 1)
		ob->xspeed = 0;

	if (ob->hitsouth)
	{
		if (ob->hitsouth == 17)	// jumping up through a pole hole
		{
			ob->y -= 2*PIXGLOBAL;
			ob->top -= 2*PIXGLOBAL;
			ob->xspeed = 0;
			ob->x = CONVERT_TILE_TO_GLOBAL(ob->tilemidx) - 2*PIXGLOBAL;
		}
		else
		{
			if (!jumpcheat)
			{
				SD_PlaySound(HITHEADSND);
				if (ob->hitsouth > 1)
				{
					ob->yspeed += 16;
					if (ob->yspeed < 0)	// push away from slopes
						ob->yspeed = 0;
				}
				else
				{
					ob->yspeed = 0;
				}
				jumptime = 0;
			}
		}
	}

	if (ob->hitnorth)
	{
		ob->ymove = 0;
		if ((ob->hitnorth & ~7) == 8)	// deadly floor!
		{
			KillKeen();
		}
		else
		{
			if (ob->hitnorth != 25 || !jumptime)	// KLUDGE to allow jumping off
			{
				ob->yspeed = -48;
				jumptime = 24;
				SD_PlaySound (POGOSND);
				ChangeState(ob, &s_keenpogo);
			}
		}
	}

	DrawMort(ob, KEENPRIORITY);
}


//===========================================================================

/*
=======================
=
= KeenPoleThink
=
=======================
*/

int	polexspeed[3] = {-SPDPOLESIDEJUMP,0,SPDPOLESIDEJUMP};

void	PoleActions (objtype *ob)
{
	if (c.xaxis)
		ob->xdir = c.xaxis;

	if (attackbutton && !attackheld)
	{
		attackheld = true;
		switch (c.yaxis)
		{
		case -1:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenpoleshootup1;
			else
				ob->state = &s_keenpolethrowup1;
			return;
		case 0:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenpoleshoot1;
			else
				ob->state = &s_keenpolethrow1;
			return;
		case 1:
			if (mapon >= 23 && mapon <= 46)
				ob->state = &s_keenpoleshootdown1;
			else
				ob->state = &s_keenpolethrowdown1;
			return;
		}
	}

	if (jumpbutton && !jumpheld)		// jump off the pole
	{
		SD_PlaySound (JUMPSND);
		ob->xspeed = polexspeed[c.xaxis+1];
		ob->yspeed = -SPDPOLEJUMP;
		ob->needtoclip = true;
		jumptime = POLEJUMPTIME;
		ob->state = &s_keenjump1;
		ob->ydir = 1;
		jumpheld = true;
		leavepoletime = TimeCount;
		return;
	}
}


void	KeenPoleThink		(objtype *ob)
{
	uint16_t  *map, tile;

	switch (c.yaxis)
	{
	case -1:
		ob->state = &s_keenclimb1;
		ob->ydir = -1;
		return;

	case 1:
		ob->state = &s_keenslide1;
		ob->ydir = 1;
		KeenDropThink (ob);
		return;
	}

	if (c.xaxis)
	{
	//
	// walk off pole if right next to ground
	//
		map = mapsegs[1] + (mapbwidthtable[ob->tilebottom+1]/2 + ob->tilemidx);
		tile = *map;
		if (tinf[NORTHWALL+tile])
		{
			ob->xspeed = 0;
			ob->yspeed = 0;
			ob->needtoclip = true;
			jumptime = 0;
			ob->state = &s_keenjump3;
			ob->ydir = 1;
			SD_PlaySound (PLUMMETSND);
			return;
		}
	}
	PoleActions (ob);
}


/*
=======================
=
= KeenClimbThink
=
=======================
*/

void	KeenClimbThink		(objtype *ob)
{
	uint16_t  *map;

	map = (uint16_t *)mapsegs[1]+mapbwidthtable[ob->tiletop]/2+ob->temp4;

	if ((tinf[INTILE+*map]&0x7f) != 1)
	{
		ob->ymove=0;
		ob->state = &s_keenpole;		// ran out of pole
		return;
	}

	switch (c.yaxis)
	{
	case 0:
		ob->state = &s_keenpole;
		ob->ydir = 0;
		break;

	case 1:
		ob->state = &s_keenslide1;
		ob->ydir = 1;
		KeenDropThink (ob);
		break;
	}

	PoleActions (ob);
}


/*
=======================
=
= KeenDropThink
=
=======================
*/

void	KeenDropThink		(objtype *ob)
{
	uint16_t  *map;

	map = (uint16_t *)mapsegs[1]+mapbwidthtable[ob->tilebottom]/2+ob->temp4;

	if ((tinf[INTILE+*map]&0x7f) != 1)
	{
		SD_PlaySound (PLUMMETSND);
		ob->state = &s_keenjumpup3;		// ran out of pole
		jumptime = 0;
		ob->temp2 = 1;
		ob->xspeed = polexspeed[c.xaxis+1];
		ob->yspeed = 0;
		ob->needtoclip = true;
		ob->tilebottom--;
		return;
	}

	switch (c.yaxis)
	{
	case -1:
		ob->state = &s_keenclimb1;
		ob->ydir = -1;
		break;

	case 0:
		ob->state = &s_keenpole;
		ob->ydir = 0;
		break;
	}

	PoleActions (ob);
}

//===========================================================================

/*
=======================
=
= KeenThrow
=
=======================
*/

void	KeenThrow	(objtype *ob)
{
// can't use &<var> in a switch statement...

	if (ob->state == &s_keenthrow3)
	{
		if (ob->xdir > 0)
			ThrowPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_East);
		else
			ThrowPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_West);
		return;
	}

	if (ob->state == &s_keenpolethrow2)
	{
		if (ob->xdir > 0)
			ThrowPower (ob->x+24*PIXGLOBAL,ob->y,dir_East);
		else
			ThrowPower (ob->x-8*PIXGLOBAL,ob->y,dir_West);
		return;
	}

	if (ob->state == &s_keenthrowup2)
	{
		ThrowPower (ob->x+4*PIXGLOBAL,ob->y-8*PIXGLOBAL,dir_North);
		return;
	}

	if (ob->state == &s_keenpolethrowup2)
	{
		ThrowPower (ob->x+8*PIXGLOBAL,ob->y-8*PIXGLOBAL,dir_North);
		return;
	}

	if (ob->state == &s_keenpolethrowdown2)
	{
		ThrowPower (ob->x+8*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_South);
		return;
	}

	if (ob->state == &s_keenairthrow2)
	{
		if (ob->xdir > 0)
			ThrowPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_East);
		else
			ThrowPower (ob->midx-4*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_West);
#if 0
		if (ob->xdir > 0)
			ThrowPower (ob->x+32*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_East);
		else
			ThrowPower (ob->x-16*PIXGLOBAL,ob->y+8*PIXGLOBAL,dir_West);
#endif

		new->xspeed += ob->xspeed/2;
		new->yspeed += ob->yspeed/2;
		return;
	}

	if (ob->state == &s_keenairthrowup2)
	{
		if (ob->xdir > 0)
			ThrowPower (ob->x+16*PIXGLOBAL,ob->y,dir_North);
		else
			ThrowPower (ob->x+4*PIXGLOBAL,ob->y,dir_North);
		new->xspeed += ob->xspeed/2;
		new->yspeed += ob->yspeed/2;
		return;
	}

	if (ob->state == &s_keenairthrowdown2)
	{
		if (ob->xdir > 0)
			ThrowPower (ob->x+3*PIXGLOBAL,ob->y+16*PIXGLOBAL,dir_South);
		else
			ThrowPower (ob->x+12*PIXGLOBAL,ob->y+16*PIXGLOBAL,dir_South);
		new->xspeed += ob->xspeed/2;
		new->yspeed += ob->yspeed/2;
		return;
	}

	if (ob->state == &s_keenplace)
	{
		if (mapon == 20)
		{
			if (ob->xdir == 1)
			{
				ThrowPower(ob->x + 14*PIXGLOBAL, ob->y + 20*PIXGLOBAL, dir_East);
			}
			else
			{
				ThrowPower(ob->x + 8*PIXGLOBAL, ob->y + 22*PIXGLOBAL, dir_West);
			}
			return;
		}
		else
		{
			if (ob->xdir > 0)
				ThrowPower (ob->x+2*PIXGLOBAL,ob->y+20*PIXGLOBAL,dir_East);
			else
				ThrowPower (ob->x+2*PIXGLOBAL,ob->y+22*PIXGLOBAL,dir_West);
			return;
		}
	}

	Quit ("KeenThrow: Bad state!");
}


/*
=======================
=
= KeenShootThink
=
=======================
*/

void KeenShootThink(objtype *ob)
{
// can't use &<var> in a switch statement...

	if (ob->state == &s_keenshoot1)
	{
		if (ob->xdir == 1)
		{
			SpawnShot(ob->x + 16*PIXGLOBAL, ob->y + 4*PIXGLOBAL, dir_East);
		}
		else
		{
			SpawnShot(ob->x - 8*PIXGLOBAL, ob->y + 4*PIXGLOBAL, dir_West);
		}
	}
	if (ob->state == &s_keenlowshoot1)
	{
		if (ob->xdir == 1)
		{
			SpawnShot(ob->x + 12*PIXGLOBAL, ob->y + 14*PIXGLOBAL, dir_East);
		}
		else
		{
			SpawnShot(ob->x + 6*PIXGLOBAL, ob->y + 14*PIXGLOBAL, dir_West);
		}
	}
	if (ob->state == &s_keenairshoot2)
	{
		if (ob->xdir == 1)
		{
			SpawnShot(ob->x + 16*PIXGLOBAL, ob->y + 2*PIXGLOBAL, dir_East);
		}
		else
		{
			SpawnShot(ob->x, ob->y + 2*PIXGLOBAL, dir_West);
		}
	}
	if (ob->state == &s_keenairshootdown2)
	{
		SpawnShot(ob->x + 8*PIXGLOBAL, ob->y + 18*PIXGLOBAL, dir_South);
	}
	if (ob->state == &s_keenairshootup2)
	{
		SpawnShot(ob->x + 5*PIXGLOBAL, ob->y - 10*PIXGLOBAL, dir_North);
	}
	if (ob->state == &s_keenshootup1)
	{
		SpawnShot(ob->x + 5*PIXGLOBAL, ob->y - 10*PIXGLOBAL, dir_North);
	}
	if (ob->state == &s_keenpoleshoot1)
	{
		if (ob->xdir == 1)
		{
			SpawnShot(ob->x + 16*PIXGLOBAL, ob->y + 4*PIXGLOBAL, dir_East);
		}
		else
		{
			SpawnShot(ob->x - 8*PIXGLOBAL, ob->y + 4*PIXGLOBAL, dir_West);
		}
	}
	if (ob->state == &s_keenpoleshootup1)
	{
		if (ob->xdir == 1)
		{
			SpawnShot(ob->x + 6*PIXGLOBAL, ob->y + 4*PIXGLOBAL, dir_North);
		}
		else
		{
			SpawnShot(ob->x + 12*PIXGLOBAL, ob->y + 4*PIXGLOBAL, dir_North);
		}
	}
	if (ob->state == &s_keenpoleshootdown1)
	{
		if (ob->xdir == 1)
		{
			SpawnShot(ob->x + 6*PIXGLOBAL, ob->y + 24*PIXGLOBAL, dir_South);
		}
		else
		{
			SpawnShot(ob->x + 12*PIXGLOBAL, ob->y + 24*PIXGLOBAL, dir_South);
		}
	}
}


/*
======================
=
= SpawnShot
=
======================
*/

void SpawnShot(unsigned x, unsigned y, int dir)
{
	if (!gamestate.ammo)
	{
		SD_PlaySound(THROWSND);
		return;
	}

	gamestate.ammo--;
	GetNewObj(true);
	new->x = x;
	new->y = y;
	new->priority = 0;
	new->obclass = stunshotobj;
	new->active = allways;
	SD_PlaySound(THROWBOMBSND);
	switch (dir)
	{
	case dir_North:
		new->xdir = 0;
		new->ydir = -1;
		break;
	case dir_East:
		new->xdir = 1;
		new->ydir = 0;
		break;
	case dir_South:
		new->xdir = 0;
		new->ydir = 1;
		break;
	case dir_West:
		new->xdir = -1;
		new->ydir = 0;
		break;
	default:
		Quit("SpawnShot: Bad dir!");
		break;
	}
	if (gamestate.difficulty == gd_Grand)
		NewState(new, &s_vortray5);
	else
		NewState(new, &s_vortray1);
}


/*
======================
=
= ExplodeShot
=
======================
*/

void ExplodeShot(objtype *ob)
{
	ob->obclass = inertobj;
	if (gamestate.difficulty == gd_Grand)
		ChangeState(ob, &s_vorthit3);
	else
		ChangeState(ob, &s_vorthit1);
	SD_PlaySound(BOMBBOOMSND);
}

/*
======================
=
= ShotThink
=
======================
*/

void ShotThink(objtype *ob)
{
	objtype *ob2;

	if (ob->tileright >= originxtile && ob->tilebottom >= originytile
		&& ob->tileleft <= originxtilemax && ob->tiletop <= originytilemax)
	{
		//object is visible, so do nothing
		return;
	}

	if (ob->tileright+10 < originxtile
		|| ob->tileleft-10 > originxtilemax
		|| ob->tilebottom+6 < originytile
		|| ob->tiletop-6 > originytilemax)
	{
		//shot is off-screen by more than half a screen, so remove it
		RemoveObj(ob);
		return;
	}

	//check for collisions with INACTIVE objects
	for (ob2 = player->next; ob2; ob2 = ob2->next)
	{
		if (!ob2->active && ob->right > ob2->left && ob->left < ob2->right
			&& ob->top < ob2->bottom && ob->bottom > ob2->top)
		{
			if (ob2->state->contact)
			{
				ob2->state->contact(ob2, ob);
				ob2->needtoreact = true;
				ob2->active = yes;
			}

			if (ob->obclass == nothing)	//BUG: obclass is 'inertobj' for the exploded shot
				break;
		}
	}
}

/*
======================
=
= ShotReact
=
======================
*/

void ShotReact(objtype *ob)
{
	int tile;

	if (ob->hitnorth == 1 && ob->tileleft != ob->tileright)
	{
		tile = *(mapsegs[1]+mapbwidthtable[ob->tiletop-1]/2+ob->tileright);
		if (tinf[NORTHWALL+tile] == 17)
		{
			ob->hitnorth = 17;
			ob->x += 0x100 - (ob->x & 0xFF);
		}
	}
	else if (ob->hitnorth == 17 && ob->tileleft != ob->tileright)
	{
		ob->x &= 0xFF00;
	}
	if (ob->hitsouth == 1 && ob->tileleft != ob->tileright)
	{
		tile = *(mapsegs[1]+mapbwidthtable[ob->tilebottom+1]/2+ob->tileright);
		if (tinf[SOUTHWALL+tile] == 17)
		{
			ob->hitsouth = 17;
			ob->x += 0x100 - (ob->x & 0xFF);
		}
	}
	else if (ob->hitsouth == 17 && ob->tileleft != ob->tileright)
	{
		ob->x &= 0xFF00;
	}
	if (ob->hitnorth == 17 || ob->hitsouth == 17)
	{
		ob->ymove = ob->state->ymove * tics * ob->ydir;
		ob->y += ob->ymove;
		ob->top += ob->ymove;
		ob->bottom += ob->ymove;
		ob->tiletop = CONVERT_GLOBAL_TO_TILE(ob->top);
		ob->tilebottom = CONVERT_GLOBAL_TO_TILE(ob->bottom);
	}
	else if (ob->hitnorth || ob->hitsouth || ob->hiteast || ob->hitwest)
	{
		ExplodeShot(ob);
	}
	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);
}


/*
===============
=
= StunnedReact
=
===============
*/

void StunnedReact(objtype *ob)
{
	int starx, stary;

	if (ob->hitwest || ob->hiteast)
		ob->xspeed = 0;

	if (ob->hitsouth)
		ob->yspeed = 0;

	if (ob->hitnorth)
	{
		ob->xspeed = ob->yspeed = 0;
		if (ob->state->nextstate)
			ChangeState(ob, ob->state->nextstate);
	}

	RF_PlaceSprite(&ob->sprite, ob->x, ob->y, ob->shapenum, spritedraw, ob->priority);

	starx = stary = 0;
	switch (ob->temp4)
	{
	case taterobj:
		stary = -4*PIXGLOBAL;
		break;
	case squashobj:
//		starx = 8*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;
	case broccoobj:
//		starx = 8*PIXGLOBAL;
		break;
	case tomatobj:
//		starx = 4*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;
	case carrotobj:
		starx = 4*PIXGLOBAL;
//		stary = -350;	// -21.875 pixels (this one is a bit strange)
		break;
	case frenchyobj:
		stary = -8*PIXGLOBAL;
		break;
	case asparobj:
//		starx = -4*PIXGLOBAL;
		stary = -12*PIXGLOBAL;
		break;
	case peapodobj:
		stary = -8*PIXGLOBAL;
		break;
	case peabrainobj:
		starx = -6*PIXGLOBAL;
		stary = -12*PIXGLOBAL;
		break;
	case apelobj:
		starx = -6*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;
	case pumpkinobj:
		starx = 4*PIXGLOBAL;
		stary = -8*PIXGLOBAL;
		break;	
	case eggplantobj:
		stary = -8*PIXGLOBAL;
		break;
	case beetobj:
		stary = -8*PIXGLOBAL;
		break;
	case kumquatobj:
		stary = -20*PIXGLOBAL;
		break;
	case cornobj:
		stary = -8*PIXGLOBAL;
		break;
	default:
		Quit("No star spec for object!");
	}
finished:

	ob->temp1 = ob->temp1 + tics;
	if (ob->temp1 > 10)
	{
		ob->temp1 -= 10;
		if (++ob->temp2 == 3)
			ob->temp2 = 0;
	}

	if (gamestate.difficulty == gd_Grand)
		RF_PlaceSprite((void **)&ob->temp3, ob->x+starx, ob->y+stary, ob->temp2+STUNSTARS4SPR, spritedraw, 3);
	else
		RF_PlaceSprite((void **)&ob->temp3, ob->x+starx, ob->y+stary, ob->temp2+STUNSTARS1SPR, spritedraw, 3);
}


/*
===========================
=
= ShotContact
=
===========================
*/

void ShotContact(objtype *ob, objtype *hit)
{
	if (hit->obclass == stunshotobj)
	{
		if (mapon >= 23 && mapon <= 46)
		{
			switch (ob->obclass)
			{
				case taterobj:
					if (gamestate.difficulty == gd_Easy)
					{
						ob->needtoclip = true;
						StunObj(ob, hit, &s_taterfemaledead);
					}
					else
					{
						ob->needtoclip = true;
						StunObj(ob, hit, &s_taterdead);
					}
					break;	
				case broccoobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_broccodead);
					break;
				case squashobj:
					ob->needtoclip = true;
					if (mapon >= 23 && mapon <= 46)
					{
						StunObj(ob, hit, &s_altsquasherdead);
					}
					else
					{
						StunObj(ob, hit, &s_squasherdead);
					}
					break;
				case tomatobj:
					if (gamestate.difficulty == gd_Easy || gamestate.difficulty == gd_Hard)
					{
						ob->needtoclip = true;
						StunObj(ob, hit, &s_btomatdead);
					}
					else
					{
						ob->needtoclip = true;
						StunObj(ob, hit, &s_tomatdead);
					}
					break;
				case carrotobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_carrotdead);
					break;
				case frenchyobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_frenchydead);
					break;
				case asparobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_aspardead);
					break;
				case peapodobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_peapoddead);
					break;
				case peabrainobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_peabraindead);
					break;
				case apelobj:
					ob->needtoclip = true;			// Szemigi: Don't let the shot Apel fall through the entire level!
					StunObj(ob, hit, &s_apeldead);
					break;
				case boobusbombobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_bombexplode);
					gamestate.boobusbombs++;		// Szemigi: Count the amount of shot Boobus Bombs.
					if (gamestate.boobusbombs == 48 && mapon >= 23 && mapon <= 46)
					{
						SD_PlaySound(BOOBUSGONESND);
					}
					break;
				case pumpkinobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_pumpkindead);
					break;
				case eggplantobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_eggplantdead);
					break;
				case beetobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_beetdead);
					break;
				case baberryobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_baberrycry1);
					break;
				case kumquatobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_kumquatsit);
					break;
				case beanobj:
					ob->needtoclip = true;
					StunObj(ob, hit, &s_beanstunned);
					break;					
			}
		}
		ob->yspeed = -24;
		ob->xspeed = ob->xdir*8;
	}
}


/*
=============================================================================

						CONTACT ROUTINES

=============================================================================
*/

/*
============================
=
= KillKeen
=
============================
*/

void KillKeen (void)
{
	if (!godmode)
	{
		if (gamestate.difficulty == gd_Keen && gamestate.lives > 0 && !plummet || gamestate.difficulty == gd_Grand && gamestate.lives > 0 && !plummet)
		{
			if (!gamestate.damagetime)
			{
				gamestate.damagetime = 17;  // slitghtly less than 0.5 second
				gamestate.lives--;
				SD_PlaySound (WAKEUPSND);
				player->needtoclip = true;
				ChangeState (player,&s_keenlosehealth);
			}
		}
		else
		{
			keenkilled = true;
			SD_PlaySound (WAKEUPSND);
			player->needtoclip = false;
			ChangeState (player,&s_keendie1);
		}
	}
	else
	{
		return;
	}
}


/*
============================
=
= KeenContact
=
============================
*/

#define DOOREXITINGXPOS (-66)

void	KeenContact (objtype *ob, objtype *hit)
{
	unsigned savex;
	switch (hit->obclass)
	{
	case	bonusobj:
		hit->obclass = inertobj;
		switch (hit->temp1)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			SD_PlaySound (GETPOINTSSND);
			hit->shapenum = BONUS100SPR+hit->temp1;
			GivePoints (bonuspoints[hit->temp1]);
			ChangeState (hit,&s_bonusrise);
			break;
		case 6:
			SD_PlaySound (EXTRAKEENSND);
			hit->shapenum = BONUS1UPSPR;
			gamestate.lives++;
			ChangeState (hit,&s_bonusrise);
			break;
		case 7:
			if (gamestate.difficulty == gd_Easy)
			{
				SD_PlaySound (GETPOINTSSND);
				hit->shapenum = BONUS10000SPR;
				GivePoints (10000);
			}
			else if (gamestate.difficulty == gd_Grand)
			{
				SD_PlaySound (EXTRAKEENSND);
				if (mapon >= 23 && mapon <= 46)
					hit->shapenum = BONUSSUPER4SPR;
				else
					hit->shapenum = BONUSSUPER3SPR;
				gamestate.lives+=2;
				if (mapon >= 23 && mapon <= 46)
					gamestate.ammo+=4;
				else
					gamestate.flowerpowers+=4;
				GivePoints (50000);
			}
			else
			{
				SD_PlaySound (EXTRAKEENSND);
				if (mapon >= 23 && mapon <= 46)
					hit->shapenum = BONUSSUPER2SPR;
				else
					hit->shapenum = BONUSSUPERSPR;
				gamestate.lives+=3;
				if (mapon >= 23 && mapon <= 46)
					gamestate.ammo+=8;
				else
					gamestate.flowerpowers+=8;
				GivePoints (20000);
			}
			ChangeState (hit,&s_bonusrise);
			break;
		case 8:
			if (mapon >= 23 && mapon <= 46)
				SD_PlaySound (GETBOMBSND);
			else
				SD_PlaySound (GETPOWERSND);
			if (mapon >= 23 && mapon <= 46)
				if (gamestate.difficulty == gd_Grand)
					hit->shapenum = BONUSPISTOL2SPR;
				else
					hit->shapenum = BONUSPISTOLSPR;
			else
				hit->shapenum = BONUSFLOWERSPR;
			if (mapon >= 23 && mapon <= 46)
				gamestate.ammo++;
			else
				gamestate.flowerpowers++;
			ChangeState (hit,&s_bonusrise);
			break;
		case 9:
			if (mapon >= 23 && mapon <= 46)
				SD_PlaySound (GETBOMBSND);
			else
				SD_PlaySound (GETPOWERSND);
			if (mapon >= 23 && mapon <= 46)
				hit->shapenum = BONUSAMMOSPR;
			else
				hit->shapenum = BONUSFLOWERUPSPR;
			if (mapon >= 23 && mapon <= 46)
				gamestate.ammo+=5;
			else
				gamestate.flowerpowers+=5;
			ChangeState (hit,&s_bonusrise);
			break;
		case 10:
			if (mapon == 20)
				hit->obclass = keenobj;
			else
				hit->obclass = keenbombobj;
			break;
		case 11:
			SD_PlaySound (GETKEYSND);
			hit->shapenum = BONUSKEYSPR;
			gamestate.keys++;
			ChangeState (hit,&s_bonusrise);
			break;
		}
		break;

	case	keycardobj:
		SD_PlaySound (GETKEYSND);
		hit->obclass = inertobj;
		hit->shapenum = KEYCARDUPSPR;
		if (!gamestate.text_keycard)
		{
			VW_FixRefreshBuffer ();
			US_CenterWindow (32,7);
			PrintY+= 8;
			US_CPrintFar(KEYCARDTEXT);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			gamestate.text_keycard = true;
		}
		gamestate.keycard=1;
		ChangeState (hit,&s_bonusrise);
		break;
	case	cornobj:
		ClipToSpriteSide (ob,hit);
		if (hit->state == &s_cornattack2)
		{
			if (US_RndT() < 0x80)
				KillKeen();
			else
				ChangeState(player,&s_keenfell);	
		}
		break;
	case	steelcartobj:
		KillKeen();
		break;
	case	kidobj:
		SD_PlaySound (NOWAYTWOSND);
		SD_WaitSoundDone();
		CA_CacheGrChunk(KIDPIC);

		VW_FixRefreshBuffer ();
		US_CenterWindow(27, 8);
		VWB_DrawPic(WindowX, WindowY, KIDPIC);
		PrintY += 6;
		WindowW -= 48;
		WindowX += 48;
		US_CPrintFar(KIDTEXT);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh ();
		RemoveObj(hit);
		break;
	case	kumquatobj:
		KillKeen();
		break;
	case	guardrobotobj:
		if (hit->state == &s_guardrobot3)
		{
			KillKeen();
		}
		break;
	case	beanobj:
		KillKeen();
		break;
	case 	mosquitoobj:
		KillKeen();
		break;
	case	roverobj:
		savex = hit->x;
		SD_PlaySound (NOWAYTWOSND);
		CA_MarkGrChunk(ROVERPIC);
		if (gamestate.difficulty == gd_Grand)
		{
			CA_MarkGrChunk(MORTPIC);
		}
		else
		{
			CA_MarkGrChunk(KEENPIC);
		}
		CA_CacheMarks(NULL, 0);
		VW_FixRefreshBuffer();

		US_CenterWindow(26, 8);
		if (gamestate.difficulty == gd_Grand)
		{
			VWB_DrawPic(WindowX, WindowY, MORTPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(MORTROVEREXITTEXT1);
		}
		else
		{
			VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
			WindowW -= 48;
			PrintY += 12;
			US_CPrintFar(KEENROVEREXITTEXT1);
		}
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();

		US_CenterWindow(26, 8);
		if (gamestate.difficulty == gd_Grand)
		{
			VWB_DrawPic(WindowX+WindowW-48, WindowY, ROVERPIC);
			WindowW -= 48;
			PrintY += 12;
			US_CPrintFar(MORTROVEREXITTEXT2);
		}
		else
		{
			VWB_DrawPic(WindowX, WindowY, ROVERPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(KEENROVEREXITTEXT2);
		}
		VW_UpdateScreen();
		VW_WaitVBL(60);
		IN_ClearKeysDown();
		IN_Ack();

		US_CenterWindow(26, 8);
		if (gamestate.difficulty == gd_Grand)
		{
			VWB_DrawPic(WindowX, WindowY, MORTPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(MORTROVEREXITTEXT3);
		}
		else
		{
			VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
			WindowW -= 48;
			PrintY += 12;
			US_CPrintFar(KEENROVEREXITTEXT3);
		}
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		
		RemoveObj(hit);
		RF_ForceRefresh ();
		ob->x = savex + DOOREXITINGXPOS;
		ChangeState(ob,&s_keenexit1);
		break;
	case	pinkowl3obj:
		SD_PlaySound (NOWAYTWOSND);
		SD_WaitSoundDone();
		CA_CacheGrChunk(OWLPIC);

		VW_FixRefreshBuffer ();
		US_CenterWindow(27, 8);
		VWB_DrawPic(WindowX, WindowY, OWLPIC);
		PrintY += 6;
		WindowW -= 48;
		WindowX += 48;
		US_CPrintFar(PINKOWLTEXT3);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh ();
		playstate = warptolevel;
		gamestate.mapon = 0;
		break;
	case	pinkowl2obj:
		SD_PlaySound (NOWAYTWOSND);
		SD_WaitSoundDone();
		CA_CacheGrChunk(OWLPIC);

		VW_FixRefreshBuffer ();
		US_CenterWindow(27, 8);
		VWB_DrawPic(WindowX, WindowY, OWLPIC);
		PrintY += 6;
		WindowW -= 48;
		WindowX += 48;
		US_CPrintFar(PINKOWLTEXT2);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh ();
		RemoveObj(hit);
		break;
	case	pinkowlobj:
		SD_PlaySound (NOWAYTWOSND);
		SD_WaitSoundDone();
		CA_CacheGrChunk(OWLPIC);

		VW_FixRefreshBuffer ();
		US_CenterWindow(27, 8);
		VWB_DrawPic(WindowX, WindowY, OWLPIC);
		PrintY += 6;
		WindowW -= 48;
		WindowX += 48;
		US_CPrintFar(PINKOWLTEXT);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh ();
		StopMusic();
		if (mapon == 46 && gamestate.boobusbombs == 48)
		{
			SD_PlaySound (BOOBUSGONESND);
			SD_WaitSoundDone();

			StartMusic(51);
			VW_FixRefreshBuffer ();
			US_CenterWindow2 (36,16);
			PrintY += 4;
			if (gamestate.difficulty == gd_Grand)
				US_CPrintFar(NEWTUBERIATEXT2);
			else
				US_CPrintFar(NEWTUBERIATEXT);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			StopMusic();
		}
		gamestate.leveldone[46] = true;
		playstate = endgame;
		break;
	case 	beetobj:
		SD_PlaySound (NOWAYSND);
		ClipToSprite (ob,hit,true);
		break;
	case	cameo1obj:
		switch (mapon)
		{
		case 6:
			CA_MarkGrChunk(SNAKEPIC);
		case 8:
			CA_MarkGrChunk(DAVEPIC);
		case 26:
			CA_MarkGrChunk(ROVERPIC);
		}
		CA_MarkGrChunk(KEENPIC);
		CA_CacheMarks(NULL, 0);
		VW_FixRefreshBuffer();

		switch (mapon)
		{
		case 6:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX, WindowY, SNAKEPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(SNAKETEXT1);
			VW_UpdateScreen();
			VW_WaitVBL(60);
			IN_ClearKeysDown();
			IN_Ack();

			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
			WindowW -= 48;
			PrintY += 12;
			US_CPrintFar(SNAKETEXT2);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		case 8:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(27, 8);
			VWB_DrawPic(WindowX, WindowY, DAVEPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(DAVETEXT);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		case 26:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX, WindowY, ROVERPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(ROVERTEXT1);
			VW_UpdateScreen();
			VW_WaitVBL(60);
			IN_ClearKeysDown();
			IN_Ack();

			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
			WindowW -= 48;
			PrintY += 12;
			US_CPrintFar(ROVERTEXT2);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		}
		break;
	case	cameo2obj:
		switch (gamestate.difficulty)
		{
		case gd_Easy:
			CA_MarkGrChunk(REDVORTICONPIC);
		case gd_Normal:
			CA_MarkGrChunk(YELLOWVORTICONPIC);
		case gd_Hard:
			CA_MarkGrChunk(BLUEVORTICONPIC);
		case gd_Keen:
			CA_MarkGrChunk(SPOTPIC);
		}
		CA_MarkGrChunk(KEENPIC);
		CA_CacheMarks(NULL, 0);
		VW_FixRefreshBuffer();

		switch (gamestate.difficulty)
		{
		case gd_Easy:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX, WindowY, REDVORTICONPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(VORTICONTEXT1);
			VW_UpdateScreen();
			VW_WaitVBL(60);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		case gd_Normal:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(27, 8);
			VWB_DrawPic(WindowX, WindowY, YELLOWVORTICONPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(VORTICONTEXT2);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		case gd_Hard:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX, WindowY, BLUEVORTICONPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(VORTICONTEXT3);
			VW_UpdateScreen();
			VW_WaitVBL(60);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		case gd_Keen:
			SD_PlaySound (NOWAYTWOSND);
			SD_WaitSoundDone();

			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX, WindowY, SPOTPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(SPOTTEXT1);
			VW_UpdateScreen();
			VW_WaitVBL(60);
			IN_ClearKeysDown();
			IN_Ack();

			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
			WindowW -= 48;
			PrintY += 12;
			US_CPrintFar(SPOTTEXT2);
			VW_UpdateScreen();
			VW_WaitVBL(30);
			IN_ClearKeysDown();
			IN_Ack();

			VW_FixRefreshBuffer ();
			US_CenterWindow(26, 8);
			VWB_DrawPic(WindowX, WindowY, SPOTPIC);
			PrintY += 6;
			WindowW -= 48;
			WindowX += 48;
			US_CPrintFar(SPOTTEXT3);
			VW_UpdateScreen();
			VW_WaitVBL(60);
			IN_ClearKeysDown();
			IN_Ack();
			RF_ForceRefresh ();
			RemoveObj(hit);
			break;
		}
		break;
	case	eggplantobj:
		if (hit->state == &s_eggplantwalk1 || hit->state == &s_eggplantwalk2)
		{
			if (godmode == true)
				return;
			else
				ChangeState (player,&s_keenfell);
		}
		break;
	case	pumpkinobj:
		if (hit->state != &s_pumpkin)
		{
			KillKeen();
		}
		break;
	case 	shot2obj:
		if (godmode == true)
			return;
		else
			ChangeState (player,&s_keenfell);
		break;
	case	keenobj:
		if (/*Keyboard[sc_DownArrow]*/ c.yaxis == 1)	// K1n9_Duk3 mod 2025-05-27: don't use hard-coded key codes
		{
			player->state = &s_keenpickup;
			SD_PlaySound (GETBOMBSND);
			hit->shapenum = BONUSBOMBSPR;
			hit->obclass = inertobj;
			gamestate.boobusbombs++;
			gamestate.bombsthislevel++;
			ChangeState (hit,&s_bonusrise);
		}
		break;
	case	keenbombobj:
		if (gamestate.boobusbombs == 47 && mapon != 20)
			SD_PlaySound (BOOBUSGONESND);
		else
			SD_PlaySound (GETBOMBSND);
		hit->shapenum = BONUSBOMBSPR;
		hit->obclass = inertobj;
		gamestate.boobusbombs++;
		gamestate.bombsthislevel++;
		ChangeState (hit,&s_bonusrise);
		break;
	case	boobusbombobj:
		if (godmode == false)
		{
			VW_UpdateScreen();
			IN_ClearKeysDown ();

			VW_UpdateScreen();

			if (gamestate.difficulty == gd_Grand)
				CA_MarkGrChunk(MORTEXPLODE1PIC);
			else
				CA_MarkGrChunk(KEENEXPLODE1PIC);
			CA_CacheMarks(NULL, 0);
			VW_FixRefreshBuffer();

			SD_PlaySound(KEENSCREAMSND);
			if (gamestate.difficulty == gd_Grand)
				VWB_DrawPic(154, 67, MORTEXPLODE1PIC);
			else
				VWB_DrawPic(154, 67, KEENEXPLODE1PIC);
			VW_UpdateScreen();
			VW_WaitVBL(50);

			if (gamestate.difficulty == gd_Grand)
				CA_MarkGrChunk(MORTEXPLODE2PIC);
			else
				CA_MarkGrChunk(KEENEXPLODE2PIC);				
			CA_CacheMarks(NULL, 0);
			VW_FixRefreshBuffer();

			SD_PlaySound(BOMBBOOMSND);
			if (gamestate.difficulty == gd_Grand)
				VWB_DrawPic(154, 67, MORTEXPLODE2PIC);
			else
				VWB_DrawPic(154, 67, KEENEXPLODE2PIC);
			VW_UpdateScreen();
			VW_WaitVBL(35);
			IN_ClearKeysDown ();

			if (gamestate.difficulty == gd_Grand)
				CA_MarkGrChunk(MORTEXPLODE3PIC);
			else
				CA_MarkGrChunk(KEENEXPLODE3PIC);			
			CA_CacheMarks(NULL, 0);
			VW_FixRefreshBuffer();

			SD_PlaySound(WAKEUPSND);
			if (gamestate.difficulty == gd_Grand)
				VWB_DrawPic(154, 67, MORTEXPLODE3PIC);
			else
				VWB_DrawPic(154, 67, KEENEXPLODE3PIC);			
			VW_UpdateScreen();
			VW_WaitVBL(100);
			IN_ClearKeysDown ();

			VW_FixRefreshBuffer();

			VW_UpdateScreen();
			RemoveObj(hit);
			playstate = died;
			RF_ForceRefresh ();
		}
		else
		{
			return;
		}
		break;
	case	mortobj:
		if (hit->state == &s_mortstand)
		{
			if (!gamestate.text_mort)
			{
				SD_PlaySound (NOWAYTWOSND);
				SD_WaitSoundDone();
				VW_FixRefreshBuffer ();
				CA_CacheGrChunk(KEENPIC);
				US_CenterWindow(26, 8);
				VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
				WindowW -= 48;
				PrintY += 5;
      				US_CPrintFar(MORTFIGHTTEXT1);
				VW_UpdateScreen();
				VW_WaitVBL(30);
				IN_ClearKeysDown();
				IN_Ack();

				CA_CacheGrChunk(MORTPIC);
				US_CenterWindow(26, 8);
				VWB_DrawPic(WindowX, WindowY, MORTPIC);
				PrintY += 6;
				WindowW -= 48;
				WindowX += 48;
      				US_CPrintFar(MORTFIGHTTEXT2);
				VW_UpdateScreen();
				VW_WaitVBL(30);
				IN_ClearKeysDown();
				IN_Ack();

				CA_CacheGrChunk(KEENPIC);
				US_CenterWindow(26, 8);
				VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
				WindowW -= 48;
				PrintY += 5;
      				US_CPrintFar(MORTFIGHTTEXT3);
				VW_UpdateScreen();
				VW_WaitVBL(30);
				IN_ClearKeysDown();
				IN_Ack();
				RF_ForceRefresh ();
				gamestate.text_mort = true;
				ChangeState (hit,&s_mortwalk1);
			}
		}
		if (hit->state == &s_mortthrow3 && gamestate.difficulty == gd_Easy)
		{
			if (godmode == true)
				return;
			else if (godmode == false)
				if (gamestate.difficulty == gd_Keen)
					return;
				else
					ChangeState (player,&s_keenfell);
		}
		break;
	case	mortdeadobj:
		SD_PlaySound (NOWAYTWOSND);
		SD_WaitSoundDone();
		CA_CacheGrChunk(KEENPIC);
		VW_FixRefreshBuffer ();
		US_CenterWindow(26, 8);
		VWB_DrawPic(WindowX+WindowW-48, WindowY, KEENPIC);
		WindowW -= 48;
		PrintY += 5;
      		US_CPrintFar(MORTFIGHTTEXT4);
		VW_UpdateScreen();
		VW_WaitVBL(30);
		IN_ClearKeysDown();
		IN_Ack();
		RF_ForceRefresh ();
		playstate = mortendgame;
		break;
	case	horseobj:
		if (hit->state == &s_horseradgrow2 || hit->state == &s_horseradgrow3 || hit->state == &s_horseradgrow4)
		{
			if (godmode == true)
				return;
			else if (godmode == false)
				ChangeState (player,&s_keenfell);
		}
		break;
	case	parsobj:
		if (hit->state == &s_parsnipstand)
			KillKeen ();
		break;
	case 	rhubobj:
		break;
	case	cauliobj:
		if (ob->state == &s_caulistop)
			ob->obclass = nothing;
		else if (hit->state == &s_cauli1 || hit->state == &s_cauli2)
			KillKeen ();
		break;
	case    turnipobj:
			KillKeen ();
		break;
	case 	keyobj:
		SD_PlaySound (GETKEYSND);
		hit->shapenum = BONUSKEYSPR;
		hit->obclass = inertobj;
		gamestate.keys+=1;
		ChangeState (hit,&s_bonusrise);
		break;
	case	brusselobj:
			KillKeen ();
		break;
	case    machineobj:
		gamestate.leveldone[20] = true;
		playstate = victorious;
		break;
	case    mushroomobj:
		onmushroom = true;
		ClipToSpriteTop(ob, hit);
		break;
	case	celeryobj:
		if (hit->state == &s_celerystop1)
			ChangeState (hit,&s_celeryscare1);
			KillKeen ();	
		if (hit->state == &s_celerystop2)
			ChangeState (hit,&s_celeryscare1);
			KillKeen ();		
		break;
	case	metaldoorobj:
		if (gamestate.keycard)
		{
			if (hit->state != &s_metaldoorraise)
			{
				SD_PlaySound (OPENDOORSND);
				ChangeState (hit,&s_metaldoorraise);					
			}
		}
		else
		{
			SD_PlaySound (NOWAYTWOSND);
			ClipToSpriteSide (ob,hit);
		}
		break;
	case	doorobj:
		if (gamestate.keys)
		{
			if (hit->state != &s_doorraise)
			{
				SD_PlaySound (OPENDOORSND);
				gamestate.keys--;
				ChangeState (hit,&s_doorraise);					
			}
		}
		else
		{
			SD_PlaySound (NOWAYSND);
			ClipToSpriteSide (ob,hit);
		}
		break;
	case	taterobj:
		if (hit->state == &s_taterattack2)
			KillKeen ();
		break;
	case	carrotobj:
		ClipToSpriteSide (ob,hit);
		if (!ob->needtoclip)		// got pushed off a pole
		{
			SD_PlaySound (PLUMMETSND);
			ob->needtoclip = true;
			ob->xspeed = ob->yspeed = 0;
			ChangeState(ob,&s_keenjump3);
			ob->temp2 = 1;
			jumptime = 0;
		}
		break;
	case	cartobj:
		onmushroom = false;
		ClipToSprite (ob,hit,true);
		break;
	case	mcartobj:
		onmushroom = false;
		ClipToSpriteTop(ob, hit);
		break;
	case	broccoobj:
		if (hit->state == &s_broccosmash3 || hit->state == &s_broccosmash4)
			KillKeen ();
		break;
	case	squashobj:
		if (hit->state == &s_squasherjump2)
		{
			KillKeen ();
		}
		else
		{
			ClipToSpriteSide (ob,hit);
			if (!ob->needtoclip)		// got pushed off a pole
			{
				SD_PlaySound (PLUMMETSND);
				ob->needtoclip = true;
				ob->xspeed = ob->yspeed = 0;
				ChangeState(ob,&s_keenjump3);
				ob->temp2 = 1;
				jumptime = 0;
			}
		}
		break;
	case	grapeobj:
		if (hit->state == &s_grapefall || hit->state == &s_grapefall2)
			KillKeen ();
		break;
	case	tomatobj:
	case	asparobj:
	case	apelobj:
	case	peabrainobj:
	case	boobusobj:
	case	shotobj:
			KillKeen ();
		break;

	}
}

void	KeenLookUpDownContact (objtype *ob, objtype *hit)
{
	switch (hit->obclass)
	{
	case    mushroomobj:
	case	mcartobj:
		onmushroom = hit->obclass == mushroomobj;
		// note (ktx58): This behaves like the Keen 4-6 KeenPosContact
		// function in the look up/down states.
		
		// ob->needtoclip = cl_midclip;

		if (ob->state == &s_keenlookup)
		{
			ChangeState(ob, &s_keenlookup);
		}

		if (ob->state == &s_keenduck)
		{
			ChangeState(ob, &s_keenduck);
		}
		jumptime = ob->xspeed = ob->yspeed = 0;
		ClipToSpriteTop(ob, hit);
		break;
	default:
		KeenContact(ob, hit);
		break;
	}
}


/*
=============================================================================

						 REACTION ROUTINES

=============================================================================
*/


/*
============================
=
= KeenSimpleReact
=
============================
*/

void	KeenSimpleReact (objtype *ob)
{
	DrawMort(ob, KEENPRIORITY);
}


void KeenReactWithPriority(objtype *ob)
{
	DrawMort(ob, ob->priority);
}


/*
============================
=
= KeenStandReact
=
============================
*/

void	KeenStandReact (objtype *ob)
{
	if (!ob->hitnorth)
	{
	//
	// walked off an edge
	//
		SD_PlaySound (PLUMMETSND);
		ob->xspeed = ob->xdir*WALKAIRSPEED;
		ob->yspeed = 0;	// K1n9_Duk3 addition 2025-05-26
		ChangeState (ob,&s_keenjump3);
		ob->temp2 = 1;
		jumptime = 0;
	}
	else if ( (ob->hitnorth & ~7) == 8)	// deadly floor!
	{
		KillKeen ();
	}

	DrawMort(ob, KEENPRIORITY);
}

/*
============================
=
= KeenWalkReact
=
============================
*/

void	KeenWalkReact (objtype *ob)
{
	if (!ob->hitnorth)
	{
	//
	// walked off an edge
	//
		ob->xspeed = ob->xdir*WALKAIRSPEED;
		ob->yspeed = 0;
		ChangeState (ob,&s_keenjump3);
		ob->temp2 = 1;
		jumptime = 0;
	}
	else if ( (ob->hitnorth & ~7) == 8)	// deadly floor!
	{
		KillKeen ();
		goto placeit;
	}

	if (ob->hiteast == 2)			// doors
	{

	}
	else if (ob->hitwest == 2)		// doors
	{

	}
	else if (ob->hiteast || ob->hitwest)
	{
	//
	// ran into a wall
	//
		ob->ticcount = 0;
		ob->state = &s_keenstand;
		ob->shapenum = ob->xdir == 1 ? ob->state->rightshapenum :
			ob->state->leftshapenum;
	}
placeit:

	DrawMort(ob, KEENPRIORITY);
}


/*
============================
=
= KeenAirReact
=
============================
*/

void	KeenAirReact (objtype *ob)
{
	int x,y;
	uint16_t  *map,mapextra;
	int oldtop, graby, ty, tile;

	if (ob->hiteast || ob->hitwest)
		ob->xspeed = 0;

	map = mapsegs[1] + (mapbwidthtable[ob->tiletop]/2) + ob->tileleft;
	mapextra = mapwidth - (ob->tileright - ob->tileleft+1);
	for (y=ob->tiletop;y<=ob->tilebottom;y++,map+=mapextra)
		for (x=ob->tileleft;x<=ob->tileright;x++,map++)
			if (tinf[SOUTHWALL+*map] == 17)	// jumping up through a pole hole
			{
				ob->xspeed = 0;
				ob->x = ob->tilemidx*TILEGLOBAL - 2*PIXGLOBAL;
				goto checknorth;
			}

	if (ob->hitsouth)
	{
		if (ob->hitsouth == 17)	// jumping up through a pole hole
		{
			ob->y -= 32;
			ob->top -= 32;
			ob->xspeed = 0;
			ob->x = ob->tilemidx*TILEGLOBAL - 2*PIXGLOBAL;
		}
		else if (!jumpcheat)
		{
			SD_PlaySound (HITHEADSND);

			if (ob->hitsouth > 1)
			{
				ob->yspeed += 16;
				if (ob->yspeed<0)	// push away from slopes
					ob->yspeed = 0;
			}
			else
				ob->yspeed = 0;
			jumptime = 0;
		}
	}

checknorth:
	if (!ob->hitnorth)
	{
	//
	// walked off an edge
	//
//		SD_PlaySound (PLUMMETSND);
//		ob->xspeed = ob->xdir*WALKAIRSPEED;
//		ChangeState (ob,&s_keenjump3);
//		ob->temp2 = 1;
//		jumptime = 0;
	}
	else if ( (ob->hitnorth & ~7) == 8)	// deadly floor!
	{
		KillKeen ();
	}

	if (ob->hitnorth)
	{
		if (!(ob->hitnorth == 25 && jumptime))	// KLUDGE to allow jumping off
		{										// sprites
			ob->temp1 = ob->temp2 = 0;
			// K1n9_Duk3 mod 2025-05-26: if throwing, switch to matching throw state and use correct timing
			if (ob->state == &s_keenairthrow1)
			{
				int temp = ob->ticcount;
				if (temp < 4)
				{
					if (mapon >= 23 && mapon <= 46)
						ChangeState (ob,&s_keenshoot1);
					else
						ChangeState (ob,&s_keenthrow1);
					ob->ticcount = temp;
				}
				else
				{
					temp -= 4;
					if (mapon >= 23 && mapon <= 46)
						ChangeState (ob,&s_keenshoot2);
					else
						ChangeState (ob,&s_keenthrow2);
					ob->ticcount = temp;
				}
			}
			else if (ob->state == &s_keenairthrow2)
			{
				// both are 1-tic states -- no ticcount adjustment necessary
				if (mapon >= 23 && mapon <= 46)
					ChangeState (ob,&s_keenshoot2);
				else
					ChangeState (ob,&s_keenthrow3);
			}
			else if (ob->state == &s_keenairthrowup1)
			{
				int temp = ob->ticcount;
				if (mapon >= 23 && mapon <= 46)
					ChangeState (ob,&s_keenshootup1);
				else
					ChangeState (ob,&s_keenthrowup1);
				ob->ticcount = temp;
			}
			else if (ob->state == &s_keenairthrowup2)
			{
				// both are 1-tic states -- no ticcount adjustment necessary
				if (mapon >= 23 && mapon <= 46)
					ChangeState (ob,&s_keenshootup2);
				else
					ChangeState (ob,&s_keenthrowup2);
			}
			else
			{
				ChangeState (ob,&s_keenstand);
			}
			// end of K1n9_Duk3 mod
			SD_PlaySound (LANDSND);
		}
	}
	else if (ob->ymove > 0)
	{
//
// check if there is an edge to grab.
//
		oldtop = ob->top - ob->ymove;
		graby = ((ob->top - 4*PIXGLOBAL) & 0xFF00) + 4*PIXGLOBAL;
		ty = (graby>>G_T_SHIFT) - 1;
		if (oldtop < graby && ob->top >= graby)
		{
			if (c.xaxis == -1)
			{
				map = mapsegs[1] + mapbwidthtable[ty]/2 + ob->tileleft;

				if (ob->hiteast)
					map--;
				tile = *map;
				if (!tinf[EASTWALL + tile] && !tinf[WESTWALL + tile]
					&& !tinf[NORTHWALL + tile] && !tinf[SOUTHWALL + tile]
					&& tinf[EASTWALL + map[mapwidth]] && tinf[NORTHWALL + map[mapwidth]]

					)
				{
					ob->xdir = -1;
					ob->needtoclip = false;
					ob->x = (ob->x & 0xFF00) + 8*PIXGLOBAL;
					ob->y = graby - 4*PIXGLOBAL;
					ob->yspeed = ob->ymove = 0;
					ChangeState(ob, &s_keenholdon);
				}
			}
			else if (c.xaxis == 1)
			{
				map = mapsegs[1] + mapbwidthtable[ty]/2 + ob->tileright;


				if (ob->hitwest)
					map++;
				tile = *map;
				if (!tinf[EASTWALL + tile] && !tinf[WESTWALL + tile]
					&& !tinf[NORTHWALL + tile] && !tinf[SOUTHWALL + tile]
					&& tinf[WESTWALL + map[mapwidth]] && tinf[NORTHWALL + map[mapwidth]]

					)
				{
					ob->xdir = 1;
					ob->needtoclip = false;
					ob->x = (ob->x & 0xFF00) + 16*PIXGLOBAL;
					ob->y = graby - 4*PIXGLOBAL;
					ob->yspeed = ob->ymove = 0;
					ChangeState(ob, &s_keenholdon);
				}
			}
		}
	}

	DrawMort(ob, KEENPRIORITY);
}

/*
============================
=
= KeenSlideReact
=
============================
*/

void	KeenSlideReact (objtype *ob)
{
	uint16_t  *map;

	if (ob->hitnorth)			// friction slow down
	{
		map = mapsegs[2] + (mapbwidthtable[ob->tiletop]/2 + ob->tileleft);
		if (!tinf[SOUTHWALL+*map] && !tinf[SOUTHWALL+*(map+1)])
			FrictionX(ob);
	}


	if (ob->hitwest || ob->hiteast || !ob->xspeed)
		ChangeState (ob, &s_keengetup);

	PLACESPRITE;
}

