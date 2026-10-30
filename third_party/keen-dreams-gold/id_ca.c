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

// ID_CA.C

/*
=============================================================================

Id Software Caching Manager
---------------------------

Must be started BEFORE the memory manager, because it needs to get the headers
loaded into the data segment

=============================================================================
*/

#include "id_heads.h"
#include <unistd.h>
#include <SDL.h>
#pragma hdrstop

/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

typedef struct
{
  unsigned bit0,bit1;	// 0-255 is a character, > is a pointer to a node
} huffnode;


PACKED(mapfiletype)
{
	uint16_t	RLEWtag;
	int32_t		headeroffsets[100];
	byte		headersize[100];		// headers are very small
	byte		tileinfo[];
} mapfiletype;


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

byte 		_seg	*tinf;
char		ca_datapath[1024];
int			mapon;
boolean			mortimerunlocked;
boolean			mortwindow;
boolean			nightmode;

uint16_t	*mapsegs[3];
maptype		_seg	*mapheaderseg[NUMMAPS];
byte		_seg	*audiosegs[NUMSNDCHUNKS];
void		_seg	*grsegs[NUMCHUNKS];

byte		far grneeded[NUMCHUNKS];
byte		ca_levelbit,ca_levelnum;

// Note (ktx58): Removed titleptr as it is not compatible with
// far level names, It is also not present in Keen 4-6. Please test.
int			profilehandle;
/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/



int32_t		*grstarts;	// array of offsets in egagraph, -1 for sparse
int32_t		*audiostarts;	// array of offsets in audio / audiot









int			grhandle;		// handle to EGAGRAPH
int			maphandle;		// handle to MAPTEMP / GAMEMAPS
int			audiohandle;	// handle to AUDIOT / AUDIO

long		chunkcomplen,chunkexplen;

SDMode		oldsoundmode;

/*
=============================================================================

					   LOW LEVEL ROUTINES

=============================================================================
*/

/*
============================
=
= CAL_GetGrChunkLength
=
= Gets the length of an explicit length chunk (not tiles)
= The file pointer is positioned so the compressed data can be read in next.
=
============================
*/

void CAL_GetGrChunkLength (int chunk)
{
	int32_t	len;

	lseek(grhandle,grstarts[chunk],SEEK_SET);
	read(grhandle,&len,sizeof(len));
	chunkexplen = SDL_SwapLE32(len);
	chunkcomplen = grstarts[chunk+1]-grstarts[chunk]-4;
}

/*
==========================
=
= CA_OpenData / CAL_LoadDataFile
=
= Keen Worlds companion: Gold's data files are read from ca_datapath, so
= the working folder can hold the saves and configuration.
=
==========================
*/

int CA_OpenData (const char *name)
{
	char	path[1200];

	snprintf(path,sizeof(path),"%s/%s",ca_datapath,name);
	return open(path,O_RDONLY | O_BINARY);
}

static void *CAL_LoadDataFile (const char *name, const char *altname, long *length)
{
	int	handle;
	void	*data;
	off_t	size;

	handle = CA_OpenData(name);
	if (handle == -1 && altname)
		handle = CA_OpenData(altname);
	if (handle == -1)
	{
		snprintf(str,sizeof(str),"Can't open %s!",name);
		Quit(str);
	}
	size = lseek(handle,0,SEEK_END);
	lseek(handle,0,SEEK_SET);
	data = malloc(size+16);
	memset(data,0,size+16);
	if (read(handle,data,size) != size)
	{
		snprintf(str,sizeof(str),"Can't read %s!",name);
		Quit(str);
	}
	close(handle);
	if (length)
		*length = size;
	return data;
}


/*
==========================
=
= CA_FarRead
=
= Read from a file to a far pointer
=
==========================
*/

boolean CA_FarRead (int handle, byte far *dest, long length)
{
	while (length > 0)
	{
		ssize_t	got = read(handle,dest,length);
		if (got <= 0)
			return false;
		dest += got;
		length -= got;
	}
	return true;
}


/*
==========================
=
= CA_SegWrite
=
= Write from a file to a far pointer
=
==========================
*/

boolean CA_FarWrite (int handle, byte far *source, long length)
{
	while (length > 0)
	{
		ssize_t	put = write(handle,source,length);
		if (put <= 0)
			return false;
		source += put;
		length -= put;
	}
	return true;
}


/*
==========================
=
= CA_WriteFile
=
= Writes a file from a memory buffer
=
==========================
*/

boolean CA_WriteFile (char *filename, void far *ptr, long length)
{
	int handle;
	long size;

	handle = open(filename,O_CREAT | O_BINARY | O_WRONLY,
				S_IREAD | S_IWRITE | S_IFREG);

	if (handle == -1)
		return false;

	if (!CA_FarWrite (handle,ptr,length))
	{
		close (handle);
		return false;
	}
	close (handle);
	return true;
}


/*
==========================
=
= CA_LoadFile
=
= Allocate space for and load a file
=
==========================
*/


/*
============================================================================

		COMPRESSION routines, see JHUFF.C for more

============================================================================
*/


/*
======================
=
= CA_RLEWcompress
=
======================
*/




/*
======================
=
= CA_RLEWexpand
= length is COMPRESSED length
=
======================
*/

long CA_RLEWCompress (uint16_t *source, long length, uint16_t *dest,
  uint16_t rlewtag)
{
  long complength;
  uint16_t value,count,i;
  uint16_t *start,*end;

  start = dest;

  end = source + (length+1)/2;

//
// compress it
//
  do
  {
    count = 1;
    value = *source++;
    while (source<end && *source == value)
    {
      count++;
      source++;
    }
    if (count>3 || value == rlewtag)
    {
    //
    // send a tag / count / value string
    //
      *dest++ = rlewtag;
      *dest++ = count;
      *dest++ = value;
    }
    else
    {
    //
    // send word without compressing
    //
      for (i=1;i<=count;i++)
	*dest++ = value;
	}

  } while (source<end);

  complength = 2*(dest-start);
  return complength;
}

void CA_RLEWexpand (uint16_t *source, uint16_t *dest,long length,
  uint16_t rlewtag)
{
	uint16_t	value,count,i;
	uint16_t	*end = dest + length/2;

	rlewtag = SDL_SwapLE16(rlewtag);
	while (dest < end)
	{
		value = *source++;
		if (value != rlewtag)
			*dest++ = SDL_SwapLE16(value);
		else
		{
			count = SDL_SwapLE16(*source++);
			value = SDL_SwapLE16(*source++);
			for (i=1;i<=count && dest < end;i++)
				*dest++ = value;
		}
	}
}



/*
=============================================================================

					 CACHE MANAGER ROUTINES

=============================================================================
*/


/*
======================
=
= CAL_SetupGrFile
=
======================
*/

void CAL_SetupGrFile (void)
{
	long	length;
	int	i;

//
// Gold links its headers into the executable; the companion reads the same
// files from the source folder of the release (KDGSRC/static).
//
	grstarts = CAL_LoadDataFile("KDGOLD.EHD","KDGSRC/static/KDGOLD.EHD",&length);
	if (length < (NUMCHUNKS+1)*4)
		Quit ("KDGOLD.EHD does not match this version of Keen Dreams Gold!");
	for (i=0;i<=NUMCHUNKS;i++)
		grstarts[i] = SDL_SwapLE32(grstarts[i]);

//
// Open the graphics file, leaving it open until the game is finished
//
	grhandle = CA_OpenData("KDGOLD.EGA");
	if (grhandle == -1)
		Quit ("Cannot open KDGOLD.EGA!");

//
// load the pic and sprite headers into the arrays in the data segment
//
	MM_GetPtr((memptr *)&pictable,NUMPICS*sizeof(pictabletype));
	CAL_GetGrChunkLength(STRUCTPIC);		// position file pointer
	CA_FarRead (grhandle,(byte far *)pictable,NUMPICS*sizeof(pictabletype));

	MM_GetPtr((memptr *)&picmtable,NUMPICM*sizeof(pictabletype));
	CAL_GetGrChunkLength(STRUCTPICM);		// position file pointer
	CA_FarRead (grhandle,(byte far *)picmtable, NUMPICM*sizeof(pictabletype));

	MM_GetPtr((memptr *)&spritetable,NUMSPRITES*sizeof(spritetabletype));
	CAL_GetGrChunkLength(STRUCTSPRITE);	// position file pointer
	CA_FarRead (grhandle,(byte far *)spritetable, NUMSPRITES*sizeof(spritetabletype));
}

//==========================================================================


/*
======================
=
= CAL_SetupMapFile
=
======================
*/

void CAL_SetupMapFile (void)
{
//
// load maphead.ext (offsets and tileinfo for map file)
//
	tinf = CAL_LoadDataFile("KDGOLD.MHD","KDGSRC/static/KDGOLD.MHD",NULL);

//
// open the data file
//
	if ((maphandle = CA_OpenData("KDGOLD.MAP")) == -1)
		Quit ("Can't open KDGOLD.MAP!");
}

//==========================================================================


/*
======================
=
= CAL_SetupAudioFile
=
======================
*/

void CAL_SetupAudioFile (void)
{
	long	length;
	int	i;

	audiostarts = CAL_LoadDataFile("KDGOLD.AHD","KDGSRC/static/KDGOLD.AHD",&length);
	for (i=0;i<length/4;i++)
		audiostarts[i] = SDL_SwapLE32(audiostarts[i]);

//
// open the data file
//
	if ((audiohandle = CA_OpenData("KDGOLD.AUD")) == -1)
		Quit ("Can't open KDGOLD.AUD!");
}

//==========================================================================


/*
======================
=
= CA_Startup
=
= Open all files and load in headers
=
======================
*/

void CA_Startup (void)
{

	CAL_SetupMapFile ();
	CAL_SetupGrFile ();
	CAL_SetupAudioFile ();

	mapon = -1;
	ca_levelbit = 1;
	ca_levelnum = 0;
}

//==========================================================================


/*
======================
=
= CA_Shutdown
=
= Closes all files
=
======================
*/

void CA_Shutdown (void)
{

	close (maphandle);
	close (grhandle);
}

//===========================================================================

/*
======================
=
= CA_CacheAudioChunk
=
======================
*/

void CA_CacheAudioChunk (int chunk)
{
	long	pos,compressed; // avoid unused variable warning

	if (audiosegs[chunk])
	{
		MM_SetPurge ((memptr *)&audiosegs[chunk],0);
		return;							// allready in memory
	}

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = audiostarts[chunk] + 4;
	// If the audio is uncompressed, it can be read directly into the
	// allocated memory chunk.
	// Loosely based on a fix added by K1n9_Duk3 to patch-based mods
	lseek(audiohandle,pos,SEEK_SET);
	compressed = audiostarts[chunk + 1] - pos;
	
	MM_GetPtr ((memptr *)&audiosegs[chunk],compressed);
	CA_FarRead(audiohandle,audiosegs[chunk],compressed);
	
}

//===========================================================================

/*
======================
=
= CA_LoadAllSounds
=
= Purges all sounds, then loads all new ones (mode switch)
=
======================
*/

void CA_LoadAllSounds (void)
{
	unsigned	start,i;

	switch (oldsoundmode)
	{
	case sdm_Off:
		goto cachein;
	case sdm_PC:
		start = STARTPCSOUNDS;
		break;
	case sdm_AdLib:
		start = STARTADLIBSOUNDS;
		break;
	}

	for (i=0;i<NUMSOUNDS;i++,start++)
		if (audiosegs[start])
			MM_SetPurge ((memptr *)&audiosegs[start],3);		// make purgable

cachein:

	switch (SoundMode)
	{
	case sdm_Off:
		return;
	case sdm_PC:
		start = STARTPCSOUNDS;
		break;
	case sdm_AdLib:
		start = STARTADLIBSOUNDS;
		break;
	}

	for (i=0;i<NUMSOUNDS;i++,start++)
		CA_CacheAudioChunk (start);

	oldsoundmode = SoundMode;
}

//===========================================================================


/*
======================
=
= CAL_ShiftSprite
=
= Make a shifted (one byte wider) copy of a sprite into another area
=
======================
*/







//===========================================================================

/*
======================
=
= CAL_LoadUncompressedSprite
=
= Generate shifts and set up sprite structure for a given sprite
=
======================
*/

void CAL_LoadUncompressedSprite (int chunk, long readbytes)
{
	spritetabletype far *spr;
	long	expanded;
	memptr	temp;

//
// calculate sizes
//
	spr = &spritetable[chunk-STARTSPRITES];
	expanded = spr->width * spr->height;
	if (readbytes > expanded * 5)
		readbytes = expanded * 5;

//
// read the planar shape (mask and four planes), then convert it to one byte
// per pixel as the SDL2 port draws sprites (0xff is transparent); shifts are
// not needed
//
	MM_GetPtr (&temp, expanded * 5);
	memset(temp, 0, expanded * 5);
	CA_FarRead(grhandle,temp,readbytes);
	MM_GetPtr (&grsegs[chunk],expanded * 8);
	VW_MaskedToPAL8(temp, grsegs[chunk], 0, 0, spr->width * 8, spr->width * 8, spr->height);
	MM_FreePtr(&temp);

	switch (spr->shifts)
	{
	case	1:
	case	2:
	case	3:
	case	4:
		break;
	default:
		Quit ("CAL_CacheSprite: Bad shifts number!");
	}
}

/*
======================
=
= CAL_LoadUncompressedGrChunk
=
= Does whatever is needed with uncompressed graphics data
=
======================
*/

void CAL_LoadUncompressedGrChunk (int chunk)
{
	long	pos,expanded;

	pos = grstarts[chunk];

	if (pos<0)							// $FFFFFFFF start is a sparse tile
	  return;
	
	lseek(grhandle,pos,SEEK_SET);

	if (chunk >= STARTTILE8 && chunk < STARTEXTERNS)
	{
	//
	// expanded sizes of tile8/16/32 are implicit
	//

#define BLOCK		32
#define MASKBLOCK	40


		if (chunk<STARTTILE8M)			// tile 8s are all in one chunk!
			expanded = BLOCK*NUMTILE8;
		else if (chunk<STARTTILE16)
			expanded = MASKBLOCK*NUMTILE8M;
		else if (chunk<STARTTILE16M)	// all other tiles are one/chunk
			expanded = BLOCK*4;
		else if (chunk<STARTTILE32)
			expanded = MASKBLOCK*4;
		else if (chunk<STARTTILE32M)
			expanded = BLOCK*16;
		else
			expanded = MASKBLOCK*16;
	}
	else
	{
	//
	// everything else has an explicit size longword
	//
		int32_t	len;
		read(grhandle,&len,sizeof(len));
		expanded = SDL_SwapLE32(len);
	}

//
// allocate final space, but do not decompress
// Sprites need to have shifts made and various other junk
//
	if (chunk>=STARTSPRITES && chunk< STARTTILE8)
		CAL_LoadUncompressedSprite(chunk,expanded);
	else
	{
		MM_GetPtr (&grsegs[chunk],expanded);
		CA_FarRead(grhandle,(byte far *)grsegs[chunk],expanded);
	}
}


/*
======================
=
= CAL_ReadGrChunk
=
= Gets a chunk off disk, optimizing reads to general buffer
=
======================
*/


/*
======================
=
= CA_CacheGrChunk
=
= Makes sure a given chunk is in memory, loadiing it if needed
=
======================
*/

void CA_CacheGrChunk (int chunk)
{
	long	pos,compressed,expanded;
	int		next;

	grneeded[chunk] |= ca_levelbit;		// make sure it doesn't get removed
	if (grsegs[chunk])
	{
		MM_SetPurge (&grsegs[chunk],0);
		return;							// allready in memory
	}

	CAL_LoadUncompressedGrChunk (chunk);
}



//==========================================================================

/*
======================
=
= CAL_MapPlane / CAL_FreeMapPlane
=
= Keen Worlds companion: each plane has MAPPAD empty rows above and below.
= DOS kept a plane in its own 64K segment, so a tile row or column worked out
= from a position that wrapped (Keen jumping off a map's edge) still read
= inside it; here such reads land in the empty rows instead of past the end.
=
======================
*/

static uint16_t	*mapplanestore[3];

static void CAL_MapPlane (int plane, long size)
{
	long	pad = (long)MAPPAD * mapheaderseg[mapon]->width * 2 + 0x20000;

	free(mapplanestore[plane]);
	mapplanestore[plane] = calloc(1, pad + size + pad);
	if (!mapplanestore[plane])
		Quit("CA_CacheMap: out of memory");
	mapsegs[plane] = (uint16_t *)((byte *)mapplanestore[plane] + pad);
}

static void CAL_FreeMapPlane (int plane)
{
	free(mapplanestore[plane]);
	mapplanestore[plane] = NULL;
	mapsegs[plane] = NULL;
}

/*
======================
=
= CA_CacheMap
=
======================
*/

void CA_CacheMap (int mapnum)
{
	long	pos,compressed,expanded;
	int		plane;
	memptr	*dest,bigbufferseg,buffer2seg;
	unsigned	size;
	uint16_t	*source;


//
// free up memory from last map
//
	if (mapon>-1 && mapheaderseg[mapon])
		MM_SetPurge ((memptr *)&mapheaderseg[mapon],3);
	for (plane=0;plane<3;plane++)
		if (mapsegs[plane])
			CAL_FreeMapPlane (plane);

	mapon = mapnum;


//
// load map header
// The header will be cached if it is still around
//
	if (!mapheaderseg[mapnum])
	{
		pos = ((mapfiletype	_seg *)tinf)->headeroffsets[mapnum];
		if (pos<0)						// $FFFFFFFF start is a sparse map
		  Quit ("CA_CacheMap: Tried to load a non existant map!");

		MM_GetPtr((memptr *)&mapheaderseg[mapnum],sizeof(maptype));
		lseek(maphandle,pos,SEEK_SET);

		CA_FarRead (maphandle,(memptr)mapheaderseg[mapnum],sizeof(maptype));
	}
	else
		MM_SetPurge ((memptr *)&mapheaderseg[mapnum],0);

//
// load the planes in
// If a plane's pointer still exists it will be overwritten (levels are
// allways reloaded, never cached)
//

	size = mapheaderseg[mapnum]->width * mapheaderseg[mapnum]->height * 2;

	for (plane = 0; plane<3; plane++)
	{
		CAL_MapPlane (plane, size);
		dest = (memptr *)&mapsegs[plane];

		pos = mapheaderseg[mapnum]->planestart[plane];
		compressed = mapheaderseg[mapnum]->planelength[plane];
		lseek(maphandle,pos,SEEK_SET);
		if (compressed<=BUFFERSIZE)
			source = bufferseg;
		else
		{
			MM_GetPtr(&bigbufferseg,compressed);
			MM_SetLock (&bigbufferseg,true);
			source = bigbufferseg;
		}

		CA_FarRead(maphandle,(byte far *)source,compressed);
		CA_RLEWexpand (source+2, *dest,size,
		((mapfiletype _seg *)tinf)->RLEWtag);

		if (compressed>BUFFERSIZE)
			MM_FreePtr(&bigbufferseg);
	}
}

//===========================================================================

/*
======================
=
= CA_UpLevel
=
= Goes up a bit level in the needed lists and clears it out.
= Everything is made purgable
=
======================
*/

void CA_UpLevel (void)
{
	int i;

	if (ca_levelnum==7)
		Quit ("CA_UpLevel: Up past level 7!");

//	for (i=0;i<NUMCHUNKS;i++)
//			if (grsegs[i])
//				MM_SetPurge(&grsegs[i],3);

	ca_levelbit<<=1;
	ca_levelnum++;
}

//===========================================================================

/*
======================
=
= CA_DownLevel
=
= Goes down a bit level in the needed lists and recaches
= everything from the lower level
=
======================
*/

void CA_DownLevel (void)
{
	if (!ca_levelnum)
		Quit ("CA_DownLevel: Down past level 0!");
	ca_levelbit>>=1;
	ca_levelnum--;
	// Note (ktx58): Removed titleptr as it is not compatible with
	// far level names, It is also not present in Keen 4-6. Please test.
	CA_CacheMarks(NULL, 1);
}

//===========================================================================

/*
======================
=
= CA_ClearMarks
=
= Clears out all the marks at the current level
=
======================
*/

void CA_ClearMarks (void)
{
	int i;

	for (i=0;i<NUMCHUNKS;i++)
		grneeded[i]&=~ca_levelbit;
}


//===========================================================================

/*
======================
=
= CA_ClearAllMarks
=
= Clears out all the marks on all the levels
=
======================
*/

void CA_ClearAllMarks (void)
{
	// Note (ktx58): Please check
	memset (grneeded,0,sizeof(grneeded));
	ca_levelbit = 1;
	ca_levelnum = 0;
}


//===========================================================================


/*
======================
=
= CA_CacheMarks
=
======================
*/

#define NUMBARS	(17l*8)
#define BARSTEP	8
#define MAXEMPTYREAD	1024

void CA_CacheMarks (char *title, boolean cachedownlevel)
{
	boolean dialog;
	int 	i,next,homex,homey,x,y,thx,thy,numcache,lastx,xl,xh;
	long	barx,barstep;
	//
	// save title so cache down level can redraw it
	//
    
	dialog = (title!=NULL);

	if (cachedownlevel)
		dialog = false;

	if (dialog)
	{
	//
	// draw dialog window (masked tiles 12 - 20 are window borders)
	//
		US_CenterWindow (26,8);
		homex = PrintX;
		homey = PrintY;

		US_CPrint ("Loading");
		if (DemoMode)
		{
			fontcolor = 13;
		}
		else if (mapon >= 0 && mapon <= 22)
		{
			if (mapon >= 0 && mapon <= 22)
				fontcolor = F_SECONDCOLOR;
			if (mapon >= 23 && mapon <= 47)
			{
				fontcolor = F_SECONDCOLOR;
			}
		}
		else
		{
			if (mapon >= 23 && mapon <= 46)
				fontcolor = 6;
			else if (mapon == 47)
				if (nightmode == true)
					fontcolor = 6;
				else
					fontcolor = F_SECONDCOLOR;
			if (mapon >= 0 && mapon <= 22)
			{
				fontcolor = 6;
			}
		}
		US_CPrint (title);
		fontcolor = F_BLACK;
		VW_UpdateScreen();

	}

	numcache = 0;
//
// go through and make everything not needed purgable
//
	for (i=0;i<NUMCHUNKS;i++)
		if (grneeded[i]&ca_levelbit)
		{
			if (grsegs[i])					// its allready in memory, make
				MM_SetPurge(&grsegs[i],0);	// sure it stays there!
			else
				numcache++;
		}
		else
		{
			if (grsegs[i])					// not needed, so make it purgeable
				MM_SetPurge(&grsegs[i],3);
		}

	if (!numcache)			// nothing to cache!
		return;

	if (dialog)
	{
	//
	// draw thermometer bar
	//
		thx = homex + 34;
		thy = homey + 36;
		if (mortimerunlocked == true && mortwindow == true)
		{
			VWB_DrawTile8(thx,thy,98);
			VWB_DrawTile8(thx,thy+8,101);
			VWB_DrawTile8(thx,thy+16,104);
			VWB_DrawTile8(thx+17*8,thy,100);
			VWB_DrawTile8(thx+17*8,thy+8,103);
			VWB_DrawTile8(thx+17*8,thy+16,106);
		}
		else
		{
			VWB_DrawTile8(thx,thy,11);
			VWB_DrawTile8(thx,thy+8,14);
			VWB_DrawTile8(thx,thy+16,17);
			VWB_DrawTile8(thx+17*8,thy,13);
			VWB_DrawTile8(thx+17*8,thy+8,16);
			VWB_DrawTile8(thx+17*8,thy+16,19);
		}
		for (x=thx+8;x<thx+17*8;x+=8)
		{
			if (mortimerunlocked == true && mortwindow == true)
			{
				VWB_DrawTile8(x,thy,99);
				VWB_DrawTile8(x,thy+8,102);
				VWB_DrawTile8(x,thy+16,105);
			}
			else
			{
				VWB_DrawTile8(x,thy,12);
				VWB_DrawTile8(x,thy+8,15);
				VWB_DrawTile8(x,thy+16,18);
			}
		}

		thx += 2;		// first line location
		thy += 5;
		barx = (long)thx<<16;
		lastx = thx;
		VW_UpdateScreen();
	}

//
// go through and load in anything still needed
//
	barstep = (NUMBARS<<16)/numcache;

	for (i=0;i<NUMCHUNKS;i++)
		if ( (grneeded[i]&ca_levelbit) && !grsegs[i])
		{
//
// update thermometer
//
			if (dialog)
			{
				barx+=barstep;
				xh = barx>>16;
				if (xh - lastx > BARSTEP)
				{
					for (x=lastx;x<=xh;x++)
						VWB_Vlin (thy,thy+13,x,14);
					lastx = xh;
					VW_UpdateScreen();
				}

			}
			
			// Note (ktx58): This eliminates a lot of optimizations.
			// As no decompression is done, the new code is fast in DosBox,
			// however it may be sub-optimal on a DOS machine.
			CAL_LoadUncompressedGrChunk(i);
		}

//
// finish up any thermometer remnants
//
		if (dialog)
		{
			xh = thx + NUMBARS;
			for (x=lastx;x<=xh;x++)
				VWB_Vlin (thy,thy+13,x,14);
			VW_UpdateScreen();
		}
}


//===========================================================================

/*
======================
=
= CA_FreeGraphics
=
======================
*/

void CA_FreeGraphics (void)
{
	int	i;

	for (i=0;i<NUMCHUNKS;i++)
		if (grsegs[i])
			MM_SetPurge ((memptr *)&grsegs[i],3);
}

/*
======================
=
= CA_SetAllPurge
=
= Make everything possible purgable
=
======================
*/

void CA_SetAllPurge (void)
{
	int i;

	CA_ClearMarks ();

//
// free cursor sprite and background save
//
	VW_FreeCursor ();

//
// free map headers and map planes
//
	for (i=0;i<NUMMAPS;i++)
		if (mapheaderseg[i])
			MM_SetPurge ((memptr *)&mapheaderseg[i],3);

	for (i=0;i<3;i++)
		if (mapsegs[i])
			CAL_FreeMapPlane (i);

//
// free sounds
//
	for (i=0;i<NUMSNDCHUNKS;i++)
		if (audiosegs[i])
			MM_SetPurge ((memptr *)&audiosegs[i],3);

//
// free graphics
//
	CA_FreeGraphics ();
}


void CA_SetGrPurge (void)
{
	int i;

//
// free graphics
//
	for (i=0;i<NUMCHUNKS;i++)
		if (grsegs[i])
			MM_SetPurge ((memptr *)&grsegs[i],3);
}

