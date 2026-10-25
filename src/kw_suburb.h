/* Suburb Shenanigans v1.2 native integration. GPL-2.0-or-later. */
#ifndef KW_SUBURB_H
#define KW_SUBURB_H
#include <stdbool.h>
struct CK_object;
bool SS_IsActive(void);
void SS_SetupFunctions(void);
bool SS_ScanObject(int info, int x, int y);
void SS_AfterScan(void);
void SS_AfterLoad(void);
bool SS_HandleLevelState(void);
bool SS_RevisitStart(void);
bool SS_IsStoryTrigger(struct CK_object *obj);
int SS_BagState(void);
bool SS_MapEntry(int *level);
void SS_SelfTest(void);
#endif
