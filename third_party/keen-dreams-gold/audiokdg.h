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

/////////////////////////////////////////////////
//
// Original MUSE Header for .C3D
// Created Wed Oct 30 22:44:13 1991
// Modified by Szemigi to fit for Keen Dreams Gold
//
/////////////////////////////////////////////////

#define NUMSOUNDS		30
#define NUMSNDCHUNKS		91

//
// Sound names & indexes
//
typedef enum {
		UNUSEDSND,             	  // 0 (UNUSED SOUND)
		KEENSCREAMSND,            // 1
		JUMPSND,                  // 2
		LANDSND,                  // 3
		THROWSND,                 // 4
		POGOSND,                  // 5 (DIVESND was unused, so POGOSND replaces it)
		GETPOWERSND,              // 6
		GETPOINTSSND,             // 7
		GETBOMBSND,               // 8
		FLOWERPOWERSND,           // 9
		UNFLOWERPOWERSND,         // 10
		ENTERLEVELSND,            // 11
		WAKEUPSND,                // 12
		LEVELDONESND,             // 13
		NOWAYSND,                 // 14
		HITHEADSND,               // 15
		TATERSWINGSND,            // 16
		BOUNCESND,                // 17
		EXTRAKEENSND,             // 18
		OPENDOORSND,              // 19
		THROWBOMBSND,             // 20
		BOMBBOOMSND,              // 21
		BOOBUSGONESND,            // 22
		GETKEYSND,                // 23
		GRAPESCREAMSND,           // 24
		PLUMMETSND,               // 25
		STATUSONSND,              // 26
		STATUSOFFSND,             // 27
		KEENWALKSND,              // 28
		NOWAYTWOSND,              // 29 
		UNCACHEDSND,              // 30 (UNCACHED SOUND)
		LASTSOUND
} soundnames;

//
// Base offsets
//

#define STARTPCSOUNDS		0
#define STARTADLIBSOUNDS	30
#define STARTDIGISOUNDS		60
#define STARTMUSIC		61

//
// Music names & indexes
//
typedef enum {
	VEGEXPL_MUS,            	  // 0	
	SMOOTHIE_MUS,   		  // 1	
	DARKHERE_MUS,  		  	  // 2
	ROWSAD_MUS,  		  	  // 3
	ROCKIN_MUS,  		  	  // 4
	WAY_MUS,  		  	  // 5
	CABBAGE_MUS,  		  	  // 6
	NEWPOTAT_MUS,  		  	  // 7
	TURNIP_MUS,  		  	  // 8
	SPITTIN_MUS,  		  	  // 9
	GROVE_MUS,  		  	  // 10
	SLYBEAN_MUS,  		  	  // 11
	GRASS_MUS,  		  	  // 12
	STAMP_MUS,  		  	  // 13
	BULLFROG_MUS,			  // 14
	ASPALLEG_MUS,			  // 15
	HASH_MUS,			  // 16
	SLEEP_MUS,			  // 17
	DEEP_MUS,			  // 18
	ELECTRO_MUS,			  // 19
	ANTSPANT_MUS,			  // 20
	VEGGIES_MUS,			  // 21
	CALL_MUS,			  // 22
	ISCREAM_MUS,			  // 23
	NOBADDR_MUS,			  // 24
	BROCCOLI_MUS,			  // 25
	BLLYFRND_MUS,			  // 26
	GUESS_MUS,			  // 27
	ALREADY_MUS,			  // 28
	PLATE_MUS,			  // 29
	LASTMUSIC
} musicnames;

/////////////////////////////////////////////////
//
// Thanks for playing with MUSE!
//
/////////////////////////////////////////////////
