// KEEN WORLDS: Operation Station Infiltration's ck5_misc.c, converted by
// tools/kw_osi_port.py (names changed from CK5_/ck5_ to OS5_/os5_; see there).
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

#include <stdio.h>
#include "id_ca.h"
#include "id_fs.h"
#include "id_in.h"
#include "kw_skip.h"
#include "id_rf.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_game.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "kw_os5_ep.h"
#include "kw_osi.h"
#include "kw_music.h" // KEEN WORLDS: Keen speaks his lines

CK_EpisodeDef osi_episode = {
	EP_CK5,
	"CK5",
	"EPISODE.CK5",
	&OS5_ScanInfoLayer,
	&OS5_IsPresent,
};

// Contains some keen-5 specific functions.

// Check if all the game files are present.
bool OS5_IsPresent()
{
	// User-provided files
	if (!FS_IsKeenFilePresent("EGAGRAPH.CK5"))
		return false;
	if (!FS_IsKeenFilePresent("GAMEMAPS.CK5"))
		return false;
	if (!FS_IsKeenFilePresent("AUDIO.CK5"))
		return false;

	// Omnispeak-provided files
	if (!FS_IsOmniFilePresent("EGAHEAD.CK5"))
		return false;
	if (!FS_IsOmniFilePresent("EGADICT.CK5"))
		return false;
	if (!FS_IsOmniFilePresent("GFXINFOE.CK5"))
		return false;
	if (!FS_IsOmniFilePresent("MAPHEAD.CK5"))
		return false;
	// Map header file may include the tile info
	//if (!FS_IsOmniFilePresent("TILEINFO.CK5"))
	//	return false;
	if (!FS_IsOmniFilePresent("AUDIODCT.CK5"))
		return false;
	if (!FS_IsOmniFilePresent("AUDIOHHD.CK5"))
		return false;
	if (!FS_IsOmniFilePresent("AUDINFOE.CK5"))
		return false;

	if (!FS_IsOmniFilePresent("ACTION.CK5"))
		return false;

	// We clearly have all of the required files.
	return true;
}

void OS5_PurpleAxisPlatform(CK_object *obj)
{
	uint16_t nextPosUnit, nextPosTile;

	if (ck_nextX || ck_nextY)
	{
		return;
	}
	//TODO: Implement properly.
	ck_nextX = obj->xDirection * 12 * SD_GetSpriteSync();
	ck_nextY = obj->yDirection * 12 * SD_GetSpriteSync();

	if (obj->xDirection == 1)
	{
		nextPosUnit = obj->clipRects.unitX2 + ck_nextX;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (obj->clipRects.tileX2 != nextPosTile && CA_TileAtPos(nextPosTile, obj->clipRects.tileY1, 2) == 0x1F)
		{
			obj->xDirection = -1;
			//TODO: Change DeltaVelocity
			ck_nextX -= (nextPosUnit & 255);
		}
	}
	else if (obj->xDirection == -1)
	{
		nextPosUnit = obj->clipRects.unitX1 + ck_nextX;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (obj->clipRects.tileX1 != nextPosTile && CA_TileAtPos(nextPosTile, obj->clipRects.tileY1, 2) == 0x1F)
		{
			obj->xDirection = 1;
			//TODO: Change DeltaVelocity
			//CK_PhysUpdateX(obj, 256 - nextPosUnit&255);
			ck_nextX += (256 - nextPosUnit) & 255;
		}
	}
	else if (obj->yDirection == 1)
	{
		nextPosUnit = obj->clipRects.unitY2 + ck_nextY;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (obj->clipRects.tileY2 != nextPosTile && CA_TileAtPos(obj->clipRects.tileX1 + 1, nextPosTile, 2) == 0x1F)
		{
			if (CA_TileAtPos(obj->clipRects.tileX1, nextPosTile - 2, 2) == 0x1F)
			{
				//Stop the platform.
				obj->visible = true;
				ck_nextY = 0;
			}
			else
			{
				obj->yDirection = -1;
				//TODO: Change DeltaVelocity
				ck_nextY -= (nextPosUnit & 255);
			}
		}
	}
	else if (obj->yDirection == -1)
	{
		nextPosUnit = obj->clipRects.unitY1 + ck_nextY;
		nextPosTile = RF_UnitToTile(nextPosUnit);
		if (obj->clipRects.tileY1 != nextPosTile && CA_TileAtPos(obj->clipRects.tileX1 + 1, nextPosTile, 2) == 0x1F)
		{
			if (CA_TileAtPos(obj->clipRects.tileX1, nextPosTile + 2, 2) == 0x1F)
			{
				// Stop the platform.
				obj->visible = true;
				ck_nextY = 0;
			}
			else
			{
				obj->yDirection = 1;
				//TODO: Change DeltaVelocity
				ck_nextY += 256 - (nextPosUnit & 255);
			}
		}
	}
}

// MISC Keen 5 functions

// Teleporter Lightning Spawn

void OS5_SpawnLightning()
{
	CK_object *new_object;

	// Spawn the top lightning
	new_object = CK_GetNewObj(true);
	new_object->zLayer = 3;
	new_object->clipped = CLIP_not;
	new_object->type = 24;
	new_object->posX = RF_TileToUnit(ck_keenObj->clipRects.tileX1) - 0x80;
	new_object->posY = RF_TileToUnit(ck_keenObj->clipRects.tileY2) - 0x500;
	CK_SetAction(new_object, CK_GetActionByName("CK5_ACT_LightningH0"));

	// Spawn the vertical lightning that covers keen
	new_object = CK_GetNewObj(true);
	new_object->zLayer = 3;
	new_object->clipped = CLIP_not;
	new_object->type = 24;
	new_object->posX = RF_TileToUnit(ck_keenObj->clipRects.tileX1);
	new_object->posY = RF_TileToUnit(ck_keenObj->clipRects.tileY1) - 0x80;
	CK_SetAction(new_object, CK_GetActionByName("CK5_ACT_LightningV0"));

	SD_PlaySound(CK_SOUNDNUM(SOUND_UNKNOWN41));
}

// Fuse Explosion Spawn

uint8_t osi_colourFusePalette[] = {
	0x00, 56, 56, 7, 56, 7, 56, 7,
	56, 7, 7, 63, 7, 63, 63, 63, 0x00};

void OS5_SpawnFuseExplosion(int tileX, int tileY)
{
	CK_object *new_object = CK_GetNewObj(true);
	new_object->zLayer = 3;
	new_object->clipped = CLIP_not;
	new_object->type = 24;
	new_object->posX = RF_TileToUnit(tileX - 1);
	new_object->posY = RF_TileToUnit(tileY);
	CK_SetAction(new_object, CK_GetActionByName("CK5_ACT_FuseExplosion0"));
	SD_PlaySound(CK_SOUNDNUM(SOUND_UNKNOWN52));
	
	if (CK_INT(OSI_FuseColourEffectLevel, 17) == ck_gameState.currentLevel)
		VL_SetPaletteAndBorderColor(osi_colourFusePalette);
}

// Level Ending Object Spawn

void OS5_SpawnLevelEnd(void)
{
	CK_object *new_object = CK_GetNewObj(false);
	new_object->active = OBJ_ALWAYS_ACTIVE;
	new_object->clipped = CLIP_not;
	CK_SetAction(new_object, CK_GetActionByName("CK5_ACT_LevelEnd"));
}

// LevelEnd Behaviour
// If in the QED, end the game
// Otherwise, do the Korath Fuse message

void OS5_LevelEnd(CK_object *obj)
{
	ck_gameState.levelState = (ca_mapOn == CK_INT(CK5_QEDLevel, 12)) ? LS_DestroyedQED : LS_KorathFuse;
}

void OS5_SetupFunctions()
{
	//Quick hack as we haven't got a deadly function yet
	OS5_Obj1_SetupFunctions();
	OS5_Obj2_SetupFunctions();
	OS5_Obj3_SetupFunctions();
	OSI_Obj4_SetupFunctions();
	OS5_Map_SetupFunctions();
	CK_ACT_AddFunction("OS5_PurpleAxisPlatform", &OS5_PurpleAxisPlatform);
	CK_ACT_AddFunction("OS5_LevelEnd", &OS5_LevelEnd);
}

// ck_inter.c
void OS5_FixupVolteSpot(int tileX, int tileY)
{
	int dir;
	if (CA_TileAtPos(tileX - 1, tileY, 2) == 0x5C)
		dir = 1;
	else if (CA_TileAtPos(tileX + 1, tileY, 2) == 0x5E)
		dir = 3;
	else if (CA_TileAtPos(tileX, tileY - 1, 2) == 0x5D)
		dir = 2;
	else if (CA_TileAtPos(tileX, tileY + 1, 2) == 0x5B)
		dir = 0;
	else
		Quit("Volte did not spawn at bad spot!");

	CA_SetTileAtPos(tileX, tileY, 2, dir + 0x5B);
}

/*
 * Spawn an enemy projectile
 * Note that the behaviour is slightly different from DOS Keen
 * DOS Keen SpawnEnemyShot returns 0 if shot is spawned, or -1 otherwise
 * omnispeak OS5_SpawnEnemyShot returns pointer if succesful, NULL otherwise
 */

CK_object *OS5_SpawnEnemyShot(int posX, int posY, CK_action *action)
{
	CK_object *new_object = CK_GetNewObj(true);

	if (!new_object)
		return NULL;

	new_object->posX = posX;
	new_object->posY = posY;
	new_object->type = CT5_EnemyShot;
	new_object->active = OBJ_EXISTS_ONLY_ONSCREEN;
	CK_SetAction(new_object, action);

	if (CK_NotStuckInWall(new_object))
	{
		return new_object;
	}
	else
	{
		CK_RemoveObj(new_object);
		return NULL;
	}
}

void OS5_OpenMapTeleporter(int tileX, int tileY)
{
	uint16_t tile_array[4];
	for (int y = 0; y < 2; y++)
	{
		for (int x = 0; x < 2; x++)
		{
			tile_array[y * 2 + x] = CA_TileAtPos(5 * 2 + x, y, 1);
		}
	}
	RF_ReplaceTiles(tile_array, 1, tileX, tileY, 2, 2);
}

void OS5_CloseMapTeleporter(int tileX, int tileY)
{
	uint16_t tile_array[4];
	for (int y = 0; y < 2; y++)
	{
		for (int x = 0; x < 2; x++)
		{
			tile_array[y * 2 + x] = CA_TileAtPos(x, y, 1);
		}
	}
	RF_ReplaceTiles(tile_array, 1, tileX, tileY, 2, 2);
}

// TODO: Cache stuff here instead of spawner handlers
#define MAXLUMPS 46
static bool os5_lumpsNeeded[MAXLUMPS];
typedef enum
{
	Lump_0 = 0,
	Lump_Keen = 1,
	Lump_Candy = 2,
	Lump_Marshmallow = 3,
	Lump_Cola = 4,
	Lump_Stix = 5,
	Lump_SugarStoopies = 6,
	Lump_BagOSugar = 7,
	Lump_VitalinKeg = 8,
	Lump_Gems = 9,
	Lump_Stunner = 10,
	Lump_PinkShot = 11,
	Lump_MapKeen = 12,
	Lump_ShikadiMaster = 13,
	Lump_Shikadi = 14,
	Lump_Shocksund = 15,
	Lump_Sphereful = 16,
	Lump_Sparky = 17,
	Lump_Mine = 18,
	Lump_Slicestar = 19,
	Lump_RoboRed = 20,
	Lump_Spirogrip = 21,
	Lump_Ampton = 22,
	Lump_VolteFace = 23,
	Lump_PurplePlat = 24,
	Lump_Spindred = 25,
	Lump_Shelley = 26,
	Lump_RedPlat = 27,
	Lump_UnusedRedPlat = 28,
	Lump_Keycard = 29,
	Lump_Korath = 30,
	Lump_QEDFuse = 31,
	Lump_QEDExplosion = 32,
	Lump_Teleporter = 33,
	Lump_BigAmpton = 34,
	Lump_RoboBlue = 35,
	Lump_RoboGreen = 36,
	Lump_RoboYellow = 37,
	Lump_SparkyJr = 38,
	Lump_Sparky2 = 39,
	Lump_Sparky3 = 40,
	Lump_Vlorg = 41
} CK_Lumptype;

static int16_t os5_itemLumps[] =
	{
		Lump_Gems,
		Lump_Gems,
		Lump_Gems,
		Lump_Gems,
		Lump_Candy,
		Lump_Marshmallow,
		Lump_Cola,
		Lump_Stix,
		Lump_SugarStoopies,
		Lump_BagOSugar,
		Lump_VitalinKeg,
		Lump_Stunner};

static int16_t os5_lumpStarts[MAXLUMPS] =
	{
		0,
		0x6C,
		0xD2,
		0xD4,
		0xD6,
		0xD8,
		0xDA,
		0xDC,
		0xDE,
		0xE0,
		0xE9,
		0xEC,
		0xF2,
		0x130,
		0x140,
		0x151,
		0x166,
		0x16E,
		0x17A,
		0x181,
		0x183,
		0x189,
		0x195,
		0x1A1,
		0x1A6,
		0x1A8,
		0x1AC,
		0x1BE,
		0x1BF,
		0xCF,
		0x126,
		0x11B,
		0x11E,
		0x122,
		448,
		460,
		469,
		482,
		492,
		501,
		521,
		530
};

static int16_t os5_lumpEnds[MAXLUMPS] =
	{
		0,
		0xCE,
		0xD3,
		0xD5,
		0xD7,
		0xD9,
		0xDB,
		0xDD,
		0xDF,
		0xE8,
		0xEA,
		0xF1,
		0x11A,
		0x13F,
		0x150,
		0x165,
		0x16D,
		0x179,
		0x180,
		0x182,
		0x188,
		0x194,
		0x1A0,
		0x1A5,
		0x1A7,
		0x1AB,
		0x1BD,
		0x1BE,
		0x1BF,
		0xD1,
		0x12F,
		0x11D,
		0x121,
		0x125,
		459,
		468,
		481,
		491,
		500,
		520,
		529,
		537
};

extern int osi_numMasterTeleDests;

void OS5_ScanInfoLayer()
{
	osi_numMasterTeleDests = 0;
	//TODO: Work out where to store current map number, etc.
	int mapW = CA_MapHeaders[ca_mapOn]->width;
	int mapH = CA_MapHeaders[ca_mapOn]->height;

	ck_gameState.ep.ck5.fusesRemaining = 0;

	for (int y = 0; y < mapH; ++y)
	{
		for (int x = 0; x < mapW; ++x)
		{
			int infoValue = CA_TileAtPos(x, y, 2);
			switch (infoValue)
			{
			case 1:
				CK_SpawnKeen(x, y, 1);
				CK_DemoSignSpawn();
				ca_graphChunkNeeded[0xEB] |= ca_levelbit;
				os5_lumpsNeeded[Lump_Keen] = true;
				break;
			case 2:
				CK_SpawnKeen(x, y, -1);
				CK_DemoSignSpawn();
				ca_graphChunkNeeded[0xEB] |= ca_levelbit;
				os5_lumpsNeeded[Lump_Keen] = true;
				break;

			case 3:
				CK_DemoSignSpawn();
				ca_graphChunkNeeded[0xEB] |= ca_levelbit;
				os5_lumpsNeeded[Lump_MapKeen] = true;
				if (ck_gameState.levelState != LS_TeleportToKorath)
					CK_SpawnMapKeen(x, y);
				break;

			case 6:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 5:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 4:
				OS5_SpawnSparky(x, y);
				os5_lumpsNeeded[Lump_Sparky] = true;
				break;

			case 9:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 8:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 7:
				OS5_SpawnMine(x, y);
				//OS5_SpawnVlorg(x, y);
				os5_lumpsNeeded[Lump_Mine] = true;
				break;

			case 12:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 11:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 10:
				OS5_SpawnSlice(x, y, CD_north);
				os5_lumpsNeeded[Lump_Slicestar] = true;
				break;

			case 15:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 14:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 13:
				OS5_SpawnRobo(x, y);
				os5_lumpsNeeded[Lump_RoboRed] = true;
				break;

			case 18:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 17:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 16:
				OS5_SpawnSpirogrip(x, y);
				os5_lumpsNeeded[Lump_Spirogrip] = true;
				break;

			case 21:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 20:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 19:
				OS5_SpawnSliceDiag(x, y);
				os5_lumpsNeeded[Lump_Slicestar] = true;
				break;

			case 24:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 23:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 22:
				OS5_SpawnSlice(x, y, CD_east);
				os5_lumpsNeeded[Lump_Slicestar] = true;
				break;

			case 25:
				RF_SetScrollBlock(x, y, true);
				break;
			case 26:
				//RF_SetScrollBlock(x, y, false);
				if (ck_gameState.levelState == LS_TeleportToKorath)
					OS5_MapKeenTeleSpawn(x, y);
				break;
			case 27:
			case 28:
			case 29:
			case 30:
				CK_SpawnAxisPlatform(x, y, infoValue - 27, false);
				os5_lumpsNeeded[Lump_RedPlat] = true;
				break;
			case 32:
				CK_SpawnFallPlat(x, y);
				os5_lumpsNeeded[Lump_RedPlat] = true;
				break;

			case 33:
				if (ck_gameState.difficulty > D_Easy)
					break;
			case 34:
				if (ck_gameState.difficulty > D_Normal)
					break;
			case 35:
				CK_SpawnStandPlatform(x, y);
				os5_lumpsNeeded[Lump_RedPlat] = true;
				break;

			case 36:
			case 37:
			case 38:
			case 39:
				CK_SpawnGoPlat(x, y, infoValue - 36, false);
				os5_lumpsNeeded[Lump_RedPlat] = true;
				break;
			case 40:
				CK_SneakPlatSpawn(x, y);
				os5_lumpsNeeded[Lump_RedPlat] = true;
				break;
			case 41:
				if (ck_gameState.currentLevel == CK_INT(CK5_QEDLevel, 12))
				{
					ck_gameState.ep.ck5.fusesRemaining = 4;
					OS5_QEDSpawn(x, y);
				}
				else
				{
					ck_gameState.ep.ck5.fusesRemaining++;
				}
				os5_lumpsNeeded[Lump_QEDFuse] = true;
				break;
			case 44:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 43:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 42:
				OS5_SpawnAmpton(x, y);
				os5_lumpsNeeded[Lump_Ampton] = true;
				break;

			case 53:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 49:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 45:
				CK_TurretSpawn(x, y, 0);
				os5_lumpsNeeded[Lump_PinkShot] = true;
				break;

			case 54:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 50:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 46:
				CK_TurretSpawn(x, y, 1);
				os5_lumpsNeeded[Lump_PinkShot] = true;
				break;

			case 55:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 51:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 47:
				CK_TurretSpawn(x, y, 2);
				os5_lumpsNeeded[Lump_PinkShot] = true;
				break;

			case 56:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 52:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 48:
				CK_TurretSpawn(x, y, 3);
				os5_lumpsNeeded[Lump_PinkShot] = true;
				break;

			case 69:
				// Spawn extra stunner if Keen has low ammo
				if (ck_gameState.numShots >= 5)
					break;
				infoValue = 68;
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
			case 67:
			case 68:
				CK_SpawnItem(x, y, infoValue - 57);
				os5_lumpsNeeded[os5_itemLumps[infoValue - 57]] = true;
				break;
			case 70:
				CK_SpawnItem(x, y, infoValue - 58); // Omegamatic Keycard
				os5_lumpsNeeded[Lump_Keycard] = true;
				break;

			case 73:
				if (ck_gameState.difficulty < D_Hard)
				{
					OS5_FixupVolteSpot(x, y);
					break;
				}
			case 72:
				if (ck_gameState.difficulty < D_Normal)
				{
					OS5_FixupVolteSpot(x, y);
					break;
				}
			case 71:
				OS5_SpawnVolte(x, y);
				os5_lumpsNeeded[Lump_VolteFace] = true;
				break;

			case 76:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 75:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 74:
				OS5_SpawnShelly(x, y);
				os5_lumpsNeeded[Lump_Shelley] = true;
				break;

			case 79:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 78:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 77:
				OS5_SpawnSpindred(x, y);
				os5_lumpsNeeded[Lump_Spindred] = true;
				break;

			case 80:
			case 81:
			case 82:
			case 83:
				CK_SpawnGoPlat(x, y, infoValue - 80, true);
				os5_lumpsNeeded[Lump_PurplePlat] = true;
				break;
			case 84:
			case 85:
			case 86:
			case 87:
				CK_SpawnAxisPlatform(x, y, infoValue - 84, true);
				os5_lumpsNeeded[Lump_PurplePlat] = true;
				break;

			case 90:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 89:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 88:
				OS5_SpawnMaster(x, y);
				os5_lumpsNeeded[Lump_ShikadiMaster] = true;
				break;

			case 101:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 100:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 99:
				OS5_SpawnShikadi(x, y);
				os5_lumpsNeeded[Lump_Shikadi] = true;
				break;

			case 104:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 103:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 102:
				OS5_SpawnShocksund(x, y);
				os5_lumpsNeeded[Lump_Shocksund] = true;
				break;

			case 107:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 106:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 105:
				OS5_SpawnSphereful(x, y);
				os5_lumpsNeeded[Lump_Sphereful] = true;
				break;

			case 124:
				OS5_SpawnKorath(x, y);
				os5_lumpsNeeded[Lump_Korath] = true;
				break;
			case 125:
				// TODO: Signal about teleportation (caching)
				os5_lumpsNeeded[Lump_Teleporter] = true;
				break;
			case 134:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 133:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 132:
				OS5_SpawnBigAmpton(x, y);
				os5_lumpsNeeded[Lump_BigAmpton] = true;
				break;
			case 137:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 136:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 135:
				os5_lumpsNeeded[Lump_Vlorg] = true;
				OS5_SpawnVlorg(x, y);
				break;
			case 0x83:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x82:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x81:
				OS5_SpawnRoboGreen(x, y);
				os5_lumpsNeeded[Lump_RoboGreen] = true;
				break;
			case 0x80:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x7F:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x7E:
				OS5_SpawnRoboBlue(x, y);
				os5_lumpsNeeded[Lump_RoboBlue] = true;
				break;
			case 0x8C:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x8B:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x8A:
				OS5_SpawnRoboYellow(x, y);
				os5_lumpsNeeded[Lump_RoboYellow] = true;
				break;
			case 0x8F:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x8E:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x8D:
				OS5_SpawnSparkyJr(x, y);
				os5_lumpsNeeded[Lump_SparkyJr] = true;
				break;
			case 0x97:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x96:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x95:
				OS5_SpawnSparky2(x, y);
				os5_lumpsNeeded[Lump_Sparky2] = true;
				break;
			case 0x9A:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x99:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x98:
				OS5_SpawnSparky3(x, y);
				os5_lumpsNeeded[Lump_Sparky3] = true;
				break;
			case 0xA1:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 0x9E:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 0x9C:
				osi_numMasterTeleDests++;
				break;
			}
		}
	}

	for (CK_object *obj = ck_keenObj; obj != NULL; obj = obj->next)
	{
		if (obj->active != OBJ_ALWAYS_ACTIVE)
			obj->active = OBJ_INACTIVE;
	}
	// TODO: Some more stuff (including opening elevator after breaking fuses)

	// Mark all of the chunks for needed lumps.
	for (int i = 0; i < MAXLUMPS; i++)
		if (os5_lumpsNeeded[i])
			for (int j = os5_lumpStarts[i]; j <= os5_lumpEnds[i]; j++)
				CA_MarkGrChunk(j);

	if (ck_gameState.currentLevel == 0)
	{
		int keenYTilePos = ck_keenObj->posY >> 8;

		// The top of the lower shaft is opened if you're above its entrance or
		// on Korath III.
		if (ck_gameState.levelsDone[CK_INT(CK5_MapTeleLevelNeeded1, 4)] &&
			ck_gameState.levelsDone[CK_INT(CK5_MapTeleLevelNeeded2, 6)] &&
			ck_gameState.levelsDone[CK_INT(CK5_MapTeleLevelNeeded3, 8)] &&
			ck_gameState.levelsDone[CK_INT(CK5_MapTeleLevelNeeded4, 10)] &&
			(keenYTilePos < CK_INT(CK5_MapTele1TopOpenAboveY, 75) || keenYTilePos > CK_INT(CK5_MapTele1TopOpenBelowY,100)))
		{
			OS5_CloseMapTeleporter(CK_INT(CK5_MapTele1BottomX, 24), CK_INT(CK5_MapTele1BottomY, 76));
			OS5_OpenMapTeleporter(CK_INT(CK5_MapTele1TopX, 22), CK_INT(CK5_MapTele1TopY, 55));
		}
		else
		{
			OS5_OpenMapTeleporter(CK_INT(CK5_MapTele1BottomX, 24), CK_INT(CK5_MapTele1BottomY, 76));
			OS5_CloseMapTeleporter(CK_INT(CK5_MapTele1TopX, 22), CK_INT(CK5_MapTele1TopY, 55));
		}

		// Unlock the bottom entrance to the upper shaft if we're below
		if (keenYTilePos > CK_INT(CK5_MapTele2BottomOpenBelowY, 39))
		{
			OS5_OpenMapTeleporter(CK_INT(CK5_MapTele2BottomX, 26), CK_INT(CK5_MapTele2BottomY, 55));
			OS5_CloseMapTeleporter(CK_INT(CK5_MapTele2TopX, 24), CK_INT(CK5_MapTele2TopY, 30));
		}

		// Unlock the top elevator when we're at the top or on Korath III.
		if (keenYTilePos < CK_INT(CK5_MapTele2TopOpenAboveY, 39) || keenYTilePos > CK_INT(CK5_MapTele2TopOpenBelowY, 100))
		{
			OS5_OpenMapTeleporter(CK_INT(CK5_MapTele2TopX, 24), CK_INT(CK5_MapTele2TopY, 30));
			OS5_CloseMapTeleporter(CK_INT(CK5_MapTele2BottomX, 26), CK_INT(CK5_MapTele2BottomY, 55));
		}
	}
}

void OS5_ExplodeGalaxy()
{
	// purge_chunks()
	VL_SetScrollCoords(0, 0);

	VL_FadeToBlack();
	CA_CacheGrChunk(98);   // Game Over Pic

	VL_ClearScreen(0);

	StartMusic(CK_INT(CK5_GameOverMusicLevel, 18));

	VH_DrawBitmap(32, 80, 98);
	VL_Present();

	KW_WaitPresentation(24 * 70, false);

	StopMusic();
}

// Fuse Explosion Message

extern uint8_t ca_levelbit;
extern uint8_t ca_graphChunkNeeded[CA_MAX_GRAPH_CHUNKS];

void OS5_FuseMessage()
{
	SD_WaitSoundDone();

	// Cache the Keen thumbs up pic
	CA_UpLevel();
	ca_graphChunkNeeded[0x5A] |= ca_levelbit;
	ca_graphChunkNeeded[0x5B] |= ca_levelbit;
	CA_CacheMarks(0);

	// VW_SyncPages();

	// Draw Keen Talking
	US_CenterWindow(0x1A, 8);
	US_SetWindowW(US_GetWindowW() - 0x30);
	VHB_DrawBitmap(US_GetWindowW() + US_GetWindowX(), US_GetWindowY(), 0x5A);
	US_SetPrintY(US_GetPrintY() + 0xC);

	if (ck_gameState.currentLevel == CK_INT(CK5_SecretFuseLevel, 0xD))
		US_CPrint(CK_STRING(ck5_str_fuseDestroyedKorath));//"I wonder what that\nfuse was for....\n");
	else
		US_CPrint(CK_STRING(ck5_str_fuseDestroyed));//("One of the four\nmachines protecting the\nmain elevator shaft--\ntoast!\n");

	// VW_UpdateScreen();
	VL_Present();
	// KEEN WORLDS: Keen says it (Keen 5's words, Keen 5's clips)
	KW_SayWorldLine(ck_gameState.currentLevel == CK_INT(CK5_SecretFuseLevel, 0xD) ? "KEEN_FUSE_KORATH" : "KEEN_FUSE");
	// VW_WaitVBL(30);

	IN_ClearKeysDown();
	// TODO: Add Joystick compatability here
	// IN_WaitForButton();
	IN_WaitButton();

	// Draw the Keen Thumbs Up Pic
	VHB_DrawBitmap(US_GetWindowW() + US_GetWindowX(), US_GetWindowY(), 0x5B);
	VL_Present();
	// VW_WaitVBL(30);
	IN_ClearKeysDown();
	IN_WaitButton();
	CA_DownLevel();
	// StopMusic();
}

void OSI_ShowGravityWarning()
{
	//VW_SyncPages()
	US_CenterWindow(0x1A, 4);
	US_SetPrintY(US_GetPrintY()+0xC);
	US_CPrint(CK_STRING(osi_lowGravityWarning));
	
	VL_Present();
	IN_ClearKeysDown();
	IN_WaitButton();
}
