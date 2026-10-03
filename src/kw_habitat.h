/*
Keen Worlds: habitable levels (peaceful once completed, with residents).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_HABITAT_H
#define KW_HABITAT_H

#include <stdbool.h>

// Called after a level's objects have spawned: if the level is completed and
// has residents, remove the enemies and spawn the residents.
void KW_SetupLevelResidents(void);

// Whether a level of the current world has residents. Once completed, such a
// level keeps its entrance on the world map so Keen can visit it.
bool KW_IsHabitableLevel(int level);

// Whether the current level is in its peaceful, inhabited state.
bool KW_IsLevelPeaceful(void);

// Called every frame from the play loop: Keen talks to a resident when he
// stands next to one and presses up.
void KW_CheckTalk(void);

// Called every frame from the play loop: in a peaceful level, walking into
// the map's right edge leaves the level.
void KW_CheckPeacefulExit(void);

struct CK_object;
struct KW_Art;
// Border Village's art (for /KWTEST): the creator's portrait and the bar
// (after completion), the boarded-up stall (before).
extern const struct KW_Art kw_portraitArt, kw_barArt, kw_stallArt;
// For /KWTEST: count hostile objects and residents in the loaded level.
void KW_CountLevelPeople(int *hostiles, int *residents, struct CK_object **firstResident);

#endif
