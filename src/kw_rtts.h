/* Return to the Shadowlands (Ceilick, v1.8): native code. GPL-2.0-or-later. */
#ifndef KW_RTTS_H
#define KW_RTTS_H
#include <stdbool.h>
bool RT_IsActive(void);
bool RT_ScanObject(int info, int x, int y);
void RT_ScoutDialog(void);
void RT_RememberScore(void);
void RT_RestoreScore(void);
void RT_SetupFunctions(void);
void RT_SelfTest(void);
#endif
