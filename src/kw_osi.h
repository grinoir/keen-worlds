/*
Keen Worlds: Operation Station Infiltration (Ceilick; programmed by
Multimania; GPL) as a world. The mod is built on Omnispeak itself: its own
Keen 5 files are brought in by tools/kw_osi_port.py (kw_os5_*.c). This is
what the mod added to the shared engine code, and is only in effect while
its world is running.
*/
#ifndef KW_OSI_H
#define KW_OSI_H

#include <stdbool.h>
#include <stdint.h>

struct CK_object;

bool OSI_IsActive(void);

// Its action file names the mod's own functions CK5_...; they are kept
// under OS5_... here. Returns the name to look up (in buf if changed).
const char *OSI_FunctionName(const char *name, char *buf, int size);

// A creature stuns Keen for a moment (its Sparkies, a shot thrown back by
// the Big Ampton).
void CK_StunKeen(struct CK_object *keen, struct CK_object *stunner);
extern int16_t ck_stunInvincibilityTimer;

// The screen shakes (the Big Ampton's steps): how much is left.
extern int osi_screenShakeOffset;

// Dying takes back what was gained in the level, as its settings say.
void OSI_RememberLevelStart(void);
void OSI_TakeBackOnDeath(void);

void OSI_SetupFunctions(void);

// After its variable file is read: its tile properties, and its lists as
// the engine reads them now.
void OSI_ChooseFiles(void);

#endif
