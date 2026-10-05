// KEEN WORLDS: Operation Station Infiltration's ck5_ep.h, converted by
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

#ifndef KW_OS5_EP_H
#define KW_OS5_EP_H

#include <stdbool.h>
#include "ck_ep.h"

/*
 * Contains definitions relevant only to Keen 5
 */
extern CK_EpisodeDef osi_episode;


/* Action functions setup */
void OS5_Obj1_SetupFunctions(void);
void OS5_Obj2_SetupFunctions(void);
void OS5_Obj3_SetupFunctions(void);
void OSI_Obj4_SetupFunctions(void);
void OS5_Map_SetupFunctions(void);
void OS5_SetupFunctions(void);
bool OS5_IsPresent(void);
void OS5_DefineConstants(void);

void OS5_ScanInfoLayer();

/* Spawning functions */

/* ck5_misc.c */
void OS5_SpawnFuseExplosion(int tileX, int tileY);
void OS5_SpawnLevelEnd(void);
void OS5_SpawnLightning(void);
CK_object *OS5_SpawnEnemyShot(int posX, int posY, CK_action *action);
void OS5_SpawnRedBlockPlatform(int tileX, int tileY, int direction, bool purple);
void OS5_SpawnItem(int tileX, int tileY, int itemNumber);

/* ck5_obj1.c */
void OS5_SpawnVolte(int tileX, int tileY);

/* ck5_obj2.c */
void OS5_SpawnSparky(int tileX, int tileY);
void OS5_SpawnSparky2(int tileX, int tileY);
void OS5_SpawnAmpton(int tileX, int tileY);
void OS5_SpawnSlice(int tileX, int tileY, int dir);
void OS5_SpawnSliceDiag(int tileX, int tileY);
void OS5_SpawnShelly(int tileX, int tileY);

/* ck5_obj3.c */
void OS5_SpawnMine(int tileX, int tileY);
void OS5_SpawnRobo(int tileX, int tileY);
void OS5_SpawnSpirogrip(int tileX, int tileY);
void OS5_SpawnSpindred(int tileX, int tileY);
void OS5_SpawnMaster(int tileX, int tileY);
void OS5_SpawnShikadi(int tileX, int tileY);
void OS5_SpawnShocksund(int tileX, int tileY);
void OS5_SpawnSphereful(int tileX, int tileY);
void OS5_SpawnKorath(int tileX, int tileY);
void OS5_QEDSpawn(int tileX, int tileY);

/* ck5_obj4.c */
void OS5_SpawnBigAmpton(int tileX, int tileY);
void OS5_SpawnRoboBlue(int tileX, int tileY);
void OS5_SpawnRoboGreen(int tileX, int tileY);
void OS5_SpawnRoboYellow(int tileX, int tileY);
void OS5_SpawnSparkyJr(int tileX, int tileY);
void OS5_SpawnSparky3(int tileX, int tileY);
void OS5_SpawnVlorg(int tileX, int tileY);

/* ck5_map.c */
void OS5_MapMiscFlagsCheck(CK_object *keen);
void CK_SpawnMapKeen(int tileX, int tileY);
void OS5_MapKeenTeleSpawn(int tileX, int tileY);

/* Map functions */
void OS5_AnimateMapTeleporter(int tileX, int tileY);
void OS5_AnimateMapElevator(int tileX, int tileY, int dir);

/* Misc functions */

/* ck5_misc.c */
void OS5_ExplodeGalaxy();
void OS5_FuseMessage();
void OSI_ShowGravityWarning();

#endif
