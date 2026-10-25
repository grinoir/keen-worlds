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
#endif
