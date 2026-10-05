/*
Keen Worlds: Keen Dreams (id Software, 1991) as the dream Keen has in the
Megarocket's bed - shared by its ported files (kw_dreams.c, kw_kd_*.c).
Licensed under the GNU GPL v2 or later, like Omnispeak and the Keen Dreams
source.
*/

#ifndef KW_DREAMS_H
#define KW_DREAMS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
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
#include "ck_game.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "kw_dreams_defs.h"

// Is Keen Dreams the running game? (It runs as Keen 4 for the engine.)
bool KD_IsActive(void);

// ---- Constants of its KD_DEF.H ----
#define KD_GAMELEVELS 17
#define ACCGRAVITY 3
#define SPDMAXY 80
#define BLOCKSIZE (8 * 16) // for positioning starting actors
#define SCROLLEAST (256 * 11)
#define SCROLLWEST (256 * 9)
#define SCROLLSOUTH (256 * 8)
#define SCROLLNORTH (256 * 4)
#define KD_BOOBUSLEVEL 15 // Boobus Tuber's castle: Keen throws bombs there
#define KD_BOMBSNEEDED 12

// The screen's corner tiles, and the last tiles in view (its originxtile...)
#define KD_ORIGINXTILE RF_UnitToTile(rf_scrollXUnit)
#define KD_ORIGINYTILE RF_UnitToTile(rf_scrollYUnit)
#define KD_ORIGINXTILEMAX (KD_ORIGINXTILE + 20)
#define KD_ORIGINYTILEMAX (KD_ORIGINYTILE + 13)

// Its tile properties (the tinf[] tables), of a foreground tile
#define KD_NORTHWALL(tile) TI_ForeTop(tile)
#define KD_EASTWALL(tile) TI_ForeRight(tile)
#define KD_SOUTHWALL(tile) TI_ForeBottom(tile)
#define KD_WESTWALL(tile) TI_ForeLeft(tile)
#define KD_INTILE(tile) TI_ForeMisc(tile)

// Finishing the game is one more way a level ends
#define KD_LS_Victorious 30

// ---- What its game state has beyond the engine's (gametype in KD_DEF.H;
// flower power is the engine's shots) ----
typedef struct KD_State
{
	int16_t boobusbombs, bombsthislevel;
	int16_t keys;
} KD_State;
extern KD_State kd_state;

// ---- The player's input this frame (its ControlInfo c) ----
typedef struct KD_Control
{
	int16_t xaxis, yaxis;
	bool button0, button1; // jump, throw
	IN_Direction dir;
} KD_Control;
extern KD_Control kd_c;
extern bool kd_button0held, kd_button1held;

// The object GetNewObj last handed out (its global "new")
extern CK_object *kd_new;
static inline void KD_GetNewObj(bool usedummy)
{
	kd_new = CK_GetNewObj(usedummy);
}

// Its NewState: put an object in a state. (The engine's CK_SetAction also
// forgets what the object now thinking was about to move; Keen Dreams' did
// not.)
static inline void KD_NewState(CK_object *ob, CK_action *state)
{
	int16_t x = ck_nextX, y = ck_nextY;
	CK_SetAction(ob, state);
	ck_nextX = x;
	ck_nextY = y;
}

// ---- kw_dreams.c ----
void KD_FrameStart(void);
void KD_ScrollScreen(void);
void KD_ScanInfoLayer(void);
void KD_PatchWorldMap(void);

// ---- kw_kd_keen.c ----
extern int16_t kd_singlegravity;
extern uint16_t kd_bounceangle[8][8];
void KD_CalcSingleGravity(void);
void KD_SpawnWorldKeen(int16_t tilex, int16_t tiley);
void KD_SpawnKeen(int16_t tilex, int16_t tiley, int16_t dir);
void KD_KillKeen(void);
void KD_Keen_SetupFunctions(void);

// ---- kw_kd_act1.c ----
void KD_WalkReact(CK_object *ob);
void KD_DoGravity(CK_object *ob);
void KD_AccelerateX(CK_object *ob, int16_t dir, int16_t max);
void KD_FrictionX(CK_object *ob);
void KD_ProjectileThink(CK_object *ob);
void KD_VelocityThink(CK_object *ob);
void KD_DrawReact(CK_object *ob);
void KD_DrawReact2(CK_object *ob);
void KD_DrawReact3(CK_object *ob);
void KD_ChangeToFlower(CK_object *ob);
void KD_SpawnBonus(int16_t tilex, int16_t tiley, int16_t type);
void KD_SpawnDoor(int16_t tilex, int16_t tiley);
void KD_SpawnBrocco(int16_t tilex, int16_t tiley);
void KD_SpawnTomat(int16_t tilex, int16_t tiley);
void KD_SpawnCarrot(int16_t tilex, int16_t tiley);
void KD_SpawnAspar(int16_t tilex, int16_t tiley);
void KD_SpawnGrape(int16_t tilex, int16_t tiley);
void KD_Act1_SetupFunctions(void);

// ---- kw_kd_act2.c ----
void KD_SpawnTater(int16_t tilex, int16_t tiley);
void KD_SpawnCart(int16_t tilex, int16_t tiley);
void KD_SpawnFrenchy(int16_t tilex, int16_t tiley);
void KD_SpawnMelon(int16_t tilex, int16_t tiley, int16_t dir);
void KD_SpawnSquasher(int16_t tilex, int16_t tiley);
void KD_SpawnApel(int16_t tilex, int16_t tiley);
void KD_SpawnPeaPod(int16_t tilex, int16_t tiley);
void KD_SpawnPeaBrain(int16_t tilex, int16_t tiley);
void KD_SpawnBoobus(int16_t tilex, int16_t tiley);
void KD_Act2_SetupFunctions(void);

#endif
