/*
Keen Worlds: Foray in the Forest (Nisaba and K1n9_Duk3) as a world - shared
by its ported files (kw_fitf.c, kw_ff_*.c).
Licensed under the GNU GPL v2 or later, like Omnispeak and Foray in the
Forest.
*/

#ifndef KW_FITF_H
#define KW_FITF_H

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
#include "ck5_ep.h"
#include "kw_fitf_defs.h"

// Is Foray in the Forest the running game? (It runs as Keen 5 for the engine.)
bool FF_IsActive(void);

// ---- Compile-time switches of its source (CK_DEF.H, ID_HEADS.H, ID_RF.H) ----
#define BETTER_WARP
#define NEWMODS
#define MINTICS 2
#define MAXTICS 5

// ---- Constants of its CK_DEF.H ----
#define FF_GAMELEVELS 27
#define MAXFALLSPEED 70
#define SLOPEMASK 7
#define FOREGROUNDPRIORITY 3
#define MAXPRIORITY 3 // its 4: it has a sprite layer more than the engine
#define PLATFORMEDGE (0x18 | 1)
#define PLATFORMBLOCK 31
#define DIRARROWSTART 91
#define DIRARROWEND 99
#define SKYPESTBLOCK 0x8F
#define INFONUMBER_0 0x90
#define INFONUMBER_MAX (INFONUMBER_0 + 53)
#define CHECKPOINT_INVINCIBILITY 70
#define DEFAULT_MUSIC_RATE (8 * 70)

#define MAP_WORLDMAP 0
#define MAP_BWBMEGAROCKET 1
#define MAP_DOPEFISHLEVEL 11
#define MAP_MUSHROOMLEVEL1 16
#define MAP_SKOVLEVEL 17
#define MAP_TEMPLELEVEL 18
#define MAP_SLUGBUSTSLEVEL 19
#define MAP_BONUSLEVEL 20
#define MAP_HIGHSCORES 21
#define MAP_MUSHROOMLEVEL2 22
#define MAP_TARDISLEVEL 23
#define MAP_ACMELABLEVEL 24
#define MAP_VOOL 26
#define BONUSLEVEL_SCORELIMIT 500000
#define SLUGCANS_TOTAL 17

// custom hitbox for the Crab (tile collisions only)
#define CRAB_X1 0
#define CRAB_X2 (24 * 16)
#define CRAB_Y1 (1 * 16)
#define CRAB_Y2 (25 * 16)
#define CRAB_XMID ((CRAB_X1 + CRAB_X2) / 2)
#define CRAB_YMID ((CRAB_Y1 + CRAB_Y2) / 2)

// Map access (its MAPSPOT macros; reads only - writes use CA_SetTileAtPos)
#define BACKSPOT(x, y) CA_TileAtPos((x), (y), 0)
#define FORESPOT(x, y) CA_TileAtPos((x), (y), 1)
#define INFOSPOT(x, y) CA_TileAtPos((x), (y), 2)

// Level done flags, saved flags
#define DF_DONE 1
#define DF_PERFECT 2
#define DF_CHEATED 4

// Its tile "misc" values beyond Keen 5's
enum
{
	MF_DeadlyWater = 35, MF_Warp, MF_FireHazard, MF_MindMachine, MF_CloneMachine, MF_GemDoor, MF_QuickDoor,
	MF_AmmoStation, MF_MusicTrigger, MF_3UP, MF_Water, MF_DoubleDoor1, MF_DoubleDoor2, MF_Quicksand, MF_Dialog,
	MF_Snailas, MF_Foobs, MF_BetterWarp, MF_Foreground = 0x80
};
#define INTILE_TYPEMASK 0x7F
#define TI_TOP_BREAKABLE 57

// Ways a level ends, beyond the engine's (its exittype)
enum
{
	ex_stillplaying = LS_Playing, ex_died = LS_Died, ex_completed = LS_LevelComplete,
	ex_clonedjanitor = 40, ex_landed, ex_brainfucled, ex_bonuscomplete, ex_mushroomtrigger, ex_secrettrigger
};

typedef enum { arrow_North, arrow_East, arrow_South, arrow_West, arrow_NorthEast, arrow_SouthEast,
	arrow_SouthWest, arrow_NorthWest, NUMARROWDIRS } FF_ArrowDir;

// ---- What its game state has beyond Keen 5's (gametype in its CK_DEF.H) ----
typedef struct FF_State
{
	int32_t oldscore;
	int16_t drops;
	uint16_t collectiblesTotal; // collected so far (bits)
	uint16_t janitorMessageShown; // (bits)
	uint16_t slugcans;
	bool cheated;
	int16_t brainfucled;
	uint16_t continuesUsed, jokeNumber;
	uint8_t savebits;
	uint16_t freeRestartsPerCheckpoint;
	int16_t maxammo, shotsinclip;
} FF_State;
extern FF_State ff_state;

// ---- Its per-level state (CK_MapState) ----
typedef struct FF_MapState
{
	uint16_t collectiblesNew; // collected in this level (bits)
	uint16_t slugcans;
	int16_t nextLevel;
	uint16_t doneFlags;
	uint16_t dreamMsg, warpInfo;
	bool checkpointSaved;
	CK_object *riding;
	CK_object *checkpointTouched, *lastCheckpoint;
	CK_object *nisasi, *mindcontrolled, *quicksand, *mummy;
	bool playerkludgeclipcancel;
	int16_t invincibilityTimer;
	int16_t stunCounter, stunCooldown;
	int16_t moontile, inwater;
	int16_t brainfucl;
	int16_t machinesActive;
	int16_t music;
	int16_t shaketics, shakecount, shakeX, shakeY;
	int16_t screenYpx, wrapScroll;
	int16_t warpDeltaX, warpDeltaY;
	uint16_t itemsTotal, itemsCollected;
} FF_MapState;
extern FF_MapState ff_map;

// Object pointers its code keeps in an object's temp fields (16 bits here):
// the engine's 16-bit object offset plus one, so that 0 is "none" and Keen
// (offset 0) can be pointed at.
static inline int16_t FF_ObjToUser(CK_object *obj)
{
	return obj ? (int16_t)(CK_ConvertObjPointerTo16BitOffset(obj) + 1) : 0;
}
static inline CK_object *FF_ObjFromUser(int16_t user)
{
	return user ? CK_ConvertObj16BitOffsetToPointer((uint16_t)(user - 1)) : NULL;
}

// A map plane by offset (its mapsegs[plane][offset])
static inline uint16_t FF_MapAt(int plane, unsigned off)
{
	return CA_TileAtPos(off % CA_GetMapWidth(), off / CA_GetMapWidth(), plane);
}

#define SIGN(x) ((x) > 0 ? 1 : -1)
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#define DEFAULT_SHOTS 5
#define ObjectsCollide(a, b) ((a)->clipRects.unitX2 > (b)->clipRects.unitX1 && (a)->clipRects.unitX1 < (b)->clipRects.unitX2 && \
	(a)->clipRects.unitY1 < (b)->clipRects.unitY2 && (a)->clipRects.unitY2 > (b)->clipRects.unitY1)
// The tiles on screen (its originxtile...)
#define originxtile RF_UnitToTile(rf_scrollXUnit)
#define originytile RF_UnitToTile(rf_scrollYUnit)
#define originxtilemax (originxtile + 20 - 1)
#define originytilemax (originytile + 13 - 1)
#define FF_D_Expert 4 // its fourth difficulty
#define FF_CLIP_custommid CLIP_normal // TODO: its custom clip box (CK_CustomClipRects)

// Its sprite drawing beyond the engine's (kw_fitf.c). TODO: flipping,
// clipping and ordering are not done yet; they draw as plain sprites.
#define drawflipped 2
void FF_DrawSprite(RF_SpriteDrawEntry **sde, int x, int y, int chunk, int drawtype, int zLayer);
#define RF_PlaceSpriteAfter(sde, x, y, chunk, type, z, after) RF_AddSpriteDraw((RF_SpriteDrawEntry **)(sde), x, y, chunk, type, z)
#define RF_PlaceSpriteClipped(sde, x, y, chunk, type, z, clip) RF_AddSpriteDraw(sde, x, y, chunk, type, z)
#define RF_PlaceSpriteAfterClipped(user, x, y, chunk, type, z, after, a, b) RF_AddSpriteDrawUsing16BitOffset(user, x, y, chunk, type, z)

// Its strings (GAMESTR.CKS, its string manager KD_SM.C): by FF_S_ number.
const char *FF_String(int number);
#define SM_GetString(n) FF_String(n)
#define SM_Print(n) US_Print(FF_String(n))
#define SM_CPrint(n) US_CPrint(FF_String(n))
#define SM_PrintCentered(n) US_PrintCentered(FF_String(n))
#define SM_MeasureMultiline(n, w, h) CK_MeasureMultiline(FF_String(n), (w), (h))
#define SM_CopyString(dest, n) strcpy((dest), FF_String(n))
#define SM_ConcatString(dest, n) strcat((dest), FF_String(n))

// State kept outside its structures
extern bool ff_keenKilled, ff_keenUpHeld, ff_keenMustAdjustNextX;
extern int ff_brainfucled;

// ---- Parts of its other files the creatures call (kw_fitf.c) ----
void FF_MoveObjVert(CK_object *ob, int ymove);
void FF_MoveObjHoriz(CK_object *ob, int xmove);
void FF_CalcTileClipRects(CK_object *ob);
bool FF_SaveCheckpoint(void);
bool FF_LoadCheckpoint(void);
void FF_MiscDialog(int16_t number);
void FF_JanitorDialog(int16_t number);
void FF_LindseyDialog(int16_t number);
void FF_JanitorSleepMonolog(void);
void FF_JanitorStunnedMonolog(void);
void FF_JanitorNisasiDialog(int16_t number);
void FF_StartMusic(uint16_t song, bool repeat);
void FF_DelayedFade(void);
void FF_FadeOut(void);
void FF_Jukebox(void);
void FF_RunSnailas(void);
void FF_RunFoobsInSpace(void);
void FF_ForceScoreboxUpdate(void);
void SD_SetMusicRate(int rate);
void SD_PlayTwoSounds(int first, int second);

// ---- Its physics helpers (kw_ff_phys.c, from its CK_STATE.C) ----
void FF_DoEasyKeenGravity(CK_object *ob);
void FF_DoGravity(CK_object *ob);
void FF_DoWeakGravity(CK_object *ob);
void FF_DoTinyGravity(CK_object *ob);
void FF_FrictionX(CK_object *ob);
void FF_FrictionX2(CK_object *ob);
void FF_AccelerateX(CK_object *ob, int accX, int velLimit);
void FF_FrictionY(CK_object *ob);
void FF_AccelerateY(CK_object *ob, int accY, int velLimit);

#endif
