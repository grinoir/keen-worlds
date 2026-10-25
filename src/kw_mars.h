/* Marooned on Mars - Galaxy native arcade port. GPL-2.0-or-later. */
#ifndef KW_MARS_H
#define KW_MARS_H
#include <stdbool.h>
struct CK_object;
bool MARS_IsActive(void);
void MARS_SetupFunctions(void);
void MARS_AfterScan(void);
void MARS_MapFrame(void);
bool MARS_IceTile(struct CK_object *keen, int misc);
void MARS_SpawnMine(int x, int y);
void MARS_FilterInput(void);
void MARS_SelfTest(void);
#endif
