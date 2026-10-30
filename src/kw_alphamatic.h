/* The Alphamatic 3.0 native integration. GPL-2.0-or-later. */
#ifndef KW_ALPHAMATIC_H
#define KW_ALPHAMATIC_H
#include <stdbool.h>
struct CK_object;
bool AL_IsActive(void);
void AL_SetupFunctions(void);
void AL_SpawnImpact(int x, int y);
void AL_AfterScan(void);
void AL_AfterDeath(void);
void AL_SelfTest(void);
void AL_PlayOpening(void);
// The opening's art, shared with Calidune's scenes: Keen's three poses
// (AL_SCENE_KEEN_W x AL_SCENE_KEEN_H; 0 idle, 1-2 talking) and the
// Megarocket facing right (AL_SCENE_SHIP_W x AL_SCENE_SHIP_H). A pixel of
// 16 or more is see-through.
#include <stdint.h>
#define AL_SCENE_KEEN_W 44
#define AL_SCENE_KEEN_H 64
#define AL_SCENE_SHIP_W 64
#define AL_SCENE_SHIP_H 28
#define AL_SCENE_SPOT_W 32
#define AL_SCENE_SPOT_H 44
const uint8_t *AL_KeenPose(int pose);
const uint8_t *AL_ShipPicture(void);
const uint8_t *AL_SpotPicture(void); // (AL_SCENE_SPOT_W x AL_SCENE_SPOT_H)
#endif
