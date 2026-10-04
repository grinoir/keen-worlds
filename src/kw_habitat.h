/*
Keen Worlds: habitable levels (peaceful once completed, with residents).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_HABITAT_H
#define KW_HABITAT_H

#include <stdbool.h>

// Called after a level's objects have spawned: if the level is completed,
// remove the enemies (and story triggers) and spawn any residents.
void KW_SetupLevelResidents(void);

// Whether a completed level keeps its world-map entrance so Keen can visit
// it again (every level, in every world).
bool KW_IsHabitableLevel(int level);

// Whether the current level is in its peaceful, inhabited state.
bool KW_IsLevelPeaceful(void);

// Called every frame from the play loop: Keen talks to a resident when he
// stands next to one and presses up.
void KW_CheckTalk(void);

// Called every frame: starts the tavern's own theme inside its room and
// restores the level theme when Keen walks back out.
void KW_UpdateMusicZone(void);

// Krisle's repaired Megarocket is on the loaded map (for /KWTEST).
bool KW_KrisleShipIsPlaced(void);

// Called every frame from the play loop: in a peaceful level, walking into
// the map's right edge leaves the level.
void KW_CheckPeacefulExit(void);

// For /KWTEST: NULL if every dialogue line fits its card, else a description.
const char *KW_CheckDialogueFits(void);

// Where Keen was put on entering a completed level (for the self-test).
typedef enum KW_RevisitPlacement
{
	KW_REVISIT_KEPT,  // no exit found: the level's own start
	KW_REVISIT_SPECIAL, // not a walking level (swimming): its own start
	KW_REVISIT_TABLE, // a hand-placed start
	KW_REVISIT_DOOR,  // at the exit door (Keen 5)
	KW_REVISIT_GOAL,  // by the broken fuses / the QED's place (Keen 5)
	KW_REVISIT_SIDE,  // the side opposite the level's start
	KW_REVISIT_ONLY_EXIT, // the level's only exit is by its start
} KW_RevisitPlacement;
KW_RevisitPlacement KW_LastRevisitPlacement(void);
// Where Keen first came into the level (its own start) and from which side.
void KW_GetLevelEntry(int *tileX, int *tileY, int *fromDirection);

// In a completed level deadly tiles (spikes, fire) don't hurt Keen.
bool KW_IsDeadlyTileHarmless(void);

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
