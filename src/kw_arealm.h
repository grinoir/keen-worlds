/*
Keen Worlds: Atroxian Realm (Gridlock, 2016) as a world - shared by its
ported files (kw_arealm.c, kw_ar_act*.c).
Licensed under the GNU GPL v2 or later, like Omnispeak and Atroxian Realm.
*/

#ifndef KW_AREALM_H
#define KW_AREALM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "id_ca.h"
#include "id_in.h"
#include "id_rf.h"
#include "id_sd.h"
#include "id_ti.h"
#include "id_us.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "ck5_ep.h"
#include "kw_arealm_defs.h"

// What its game state has beyond Keen 5's (CK_GameState in its KD_DEF.H).
typedef struct AR_State
{
	CK_object *krileGhost;    // the Krile ghost: one per level
	uint16_t axosisNum;       // Axosises left in the level
	int16_t generatorsDestroyed;
	int16_t vines;            // the vines have pulled Keen under
} AR_State;
extern AR_State ar_state;

// Its own vertical acceleration helper (CK_PhysAccelVert in its KD_PHYS.C).
void AR_PhysAccelVert(CK_object *obj, int accY, int velLimit);

void CK_BasicDrawFunc1(CK_object *obj);
void CK_BasicDrawFunc2(CK_object *obj);
void CK_BasicDrawFunc4(CK_object *obj);

void AR_SpawnLarvac(int tileX, int tileY);
void AR_SpawnBeetle(int tileX, int tileY);
void AR_SpawnMagmine(int tileX, int tileY);
void AR_SpawnFireball(int tileX, int tileY, int direction);
void AR_SpawnDartfrog(int tileX, int tileY);
void AR_SpawnSplider(int tileX, int tileY);
void AR_SpawnLarvbat(int tileX, int tileY);
void AR_SpawnHornet(int tileX, int tileY);
void AR_SpawnDartgun(int tileX, int tileY, int direction);

void AR_Act1_SetupFunctions(void);
void AR_Act2_SetupFunctions(void);
void AR_Act3_SetupFunctions(void);

#endif
