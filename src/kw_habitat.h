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

// Whether the current level is in its peaceful, inhabited state.
bool KW_IsLevelPeaceful(void);

// Called every frame from the play loop: Keen talks to a resident when he
// stands next to one and presses up.
void KW_CheckTalk(void);

struct CK_object;
// For /KWTEST: count hostile objects and residents in the loaded level.
void KW_CountLevelPeople(int *hostiles, int *residents, struct CK_object **firstResident);

#endif
