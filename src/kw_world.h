/*
Keen Worlds: travelling between Keen 4, 5 and 6 in one running game.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_WORLD_H
#define KW_WORLD_H

#include <stdbool.h>

// Which episode (4, 5 or 6) the player has asked to travel to, or 0.
extern int kw_pendingWorld;

// Ask the game loop to travel to another episode (4, 5 or 6) once the
// current play loop ends.
void KW_RequestWorld(int episode);

// Unload the current episode's data and load another one in its place,
// swapping the per-world parts of the game state. Keen arrives on the new
// world's map.
void KW_SwitchWorld(int episode);

// Called after a world's original ending has played: the run continues on
// that world's map, and the world is remembered as completed.
void KW_AfterWorldEnding(void);

// Whether the current world's ending has been seen in this run.
bool KW_IsWorldCompleted(int episode);

// Forget every world's progress (called when a new game starts).
void KW_ResetWorlds(void);

// The world map's starting tile, where the Megarocket stands. Set when the
// map spawns Keen.
void KW_SetLaunchSpot(int tileX, int tileY);

// Keep map Keen on the short, visible Keen 5 boarding bridge instead of
// allowing a route into the unused exterior space around the station.
void KW_ConstrainKeen5Dock(struct CK_object *mapKeen);

// Called after a level's normal object spawn pass. A one-shot cross-world
// arrival is moved beside the Megarocket without changing ordinary entries.
void KW_PlaceArrivalAtShip(void);

// Called when Keen presses a button on the world map without standing on a
// level entrance. Near the launch spot this asks for the travel menu.
void KW_TryLaunch(struct CK_object *mapKeen);

// Called every frame from the play loop: opens the Megarocket menu when asked
// for, and handles the debug shortcut (on a world map, hold W and press 4, 5
// or 6).
void KW_PlayLoopHook(void);

// Developer self-test, run with /KWTEST.
void KW_SelfTest(void);

#endif
