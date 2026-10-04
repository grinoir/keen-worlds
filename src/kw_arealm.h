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
	int16_t firebombcount, firebombtics; // Atroxus's fire bombs
	int32_t firebombresettimer;
	bool endscreen_played[5];
	bool doSecretEnding;      // touched the secret level's Krile
	int16_t atroxusBoxesPlayed;
	int16_t atroxusBoxesLevels[3]; // the levels whose Atroxus box has been played
} AR_State;
extern AR_State ar_state;

// Its own vertical acceleration helper (CK_PhysAccelVert in its KD_PHYS.C).
void AR_PhysAccelVert(CK_object *obj, int accY, int velLimit);

#define STARTPICS 6 // its first picture chunk (GRAPHCKD.H)
extern uint8_t ca_levelbit;
extern uint8_t ca_graphChunkNeeded[CA_MAX_GRAPH_CHUNKS];

// Object pointers its code keeps in an object's user fields (16 bits there).
static inline uint16_t AR_ObjToUser(CK_object *obj)
{
	return obj ? CK_ConvertObjPointerTo16BitOffset(obj) : 0;
}
static inline CK_object *AR_ObjFromUser(uint16_t user)
{
	return user ? CK_ConvertObj16BitOffsetToPointer(user) : NULL;
}

// Is Atroxian Realm the running game? (It runs as Keen 5 for the engine.)
bool AR_IsActive(void);
// The game is won: its ending screens.
void AR_WinGame(void);
void AR_GeneratorWinMessage(void);
void AR_ElderWinMessage(void);
void AR_BridgeWinMessage(void);

void AR_SpawnAtroxian(int tileX, int tileY);
void AR_SpawnAtroxite(int tileX, int tileY);
void AR_SpawnAtroxus(int tileX, int tileY);
void AR_SpawnAtroxusBox(int tileX, int tileY);
void AR_SpawnAtroxusDialogue(int tileX, int tileY);
void AR_SpawnAxoclaw(int tileX, int tileY);
void AR_SpawnAxosis(int tileX, int tileY);
void AR_SpawnBridge(int tileX, int tileY);
void AR_SpawnBWBElder(int tileX, int tileY);
void AR_SpawnElder(int tileX, int tileY);
void AR_SpawnGenerator(int tileX, int tileY);
void AR_SpawnKrileGhost(int tileX, int tileY);
void AR_SpawnKrileGuardian(int tileX, int tileY);
void AR_SpawnMoxel(int tileX, int tileY);
void AR_SpawnSecretKrile(int tileX, int tileY);

// Its story and dialogue screens (EndScreens in its CK_TEXT.C), by number.
void AR_EndScreens(int dialogue);

void CK_Fall(CK_object *obj);
void CK_Fall2(CK_object *obj);
void CK_Glide(CK_object *obj);
void AR_DrawHitFlash(CK_object *obj, uint16_t *healthinfo, int adjX, int adjY);
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
