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

// For /KWTEST: NULL if every dialogue line fits its card, else a description.
const char *KW_CheckDialogueFits(void);

struct CK_object;
struct KW_Art;
// Border Village's tavern art (for /KWTEST): boards on the door (before);
// the bar with the barmaid, furniture and the creator's portrait (after).
extern const struct KW_Art kw_innArt, kw_signArt, kw_tavernBoardsArt, kw_tavernBarArt, kw_tavernFurnitureArt, kw_portraitArt;

// For /KWTEST: the tavern's door positions and destinations.
void KW_TavernDoors(int *streetX, int *streetY, int *roomX, int *roomY);
int KW_TavernBarmaidX(void);
// For /KWTEST: count hostile objects and residents in the loaded level.
void KW_CountLevelPeople(int *hostiles, int *residents, struct CK_object **firstResident);

#endif
