/*
Keen Worlds: travelling between worlds (Keen 4, 5, 6 and fan games) in one
running game.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_WORLD_H
#define KW_WORLD_H

#include <stdbool.h>

#include "id_fs.h"

// One config file (settings and the high-score table) and one set of save
// slots for all worlds, instead of CONFIG.CKx and SAVEGAM?.CKx per episode.
#define KW_CONFIG_FILE "KWORLDS.CFG"
#define KW_CONFIG_TAG "KWC"
#define KW_SAVE_FILE_PATTERN "KWORLDS?.SAV"
#define KW_SAVE_TAG "KWS"
#define KW_SAVE_MAGIC 0x574B // "KW"

// ---- The world registry ----
// A world is a game Keen can fly to: an original episode or a fan game built
// on one of them. Each has its own progress, data folder and settings file;
// Keen Worlds' own additions (ships, residents, art) belong to one world, not
// to every world sharing its engine episode.
typedef enum KW_World
{
	KW_KEEN4,   // Gnosticus IV
	KW_KEEN5,   // The Omegamatic
	KW_KEEN6,   // Fribbulus Xax
	KW_KEEN7,   // Krodacia: "The Keys of Krodacia" (fan game on Keen 4)
	KW_KEEN8,   // Calidune: "Dead in the Desert" (fan game on Keen 6)
	KW_KEEN9,   // HUB Command Center: "Battle of the Brains" (fan game on Keen 5)
	KW_AREALM,  // Krisle: "Atroxian Realm" (its own game, on the Keen 5 code)
	KW_FITF,    // Gnosticus Pi: "Foray in the Forest" (its own game, on the Keen 5 code)
	KW_OSI,     // The Shikadi Station: "Operation Station Infiltration" (its own code, on Keen 5's)
	KW_KEEN95,  // Aspermontis: "The Eight Accumulators" (fan level pack on Keen 4)
	KW_CK55,    // The Omegamatic II: "Keen 5.5" (fan level pack on Keen 5)
	KW_ASYLUM,  // Address Unknown: "Keen Asylum" (fan level pack on Keen 5)
	KW_BOUNTY,  // Meurglys III: "Bounty Hunting Brouhaha" (fan game on Keen 4)
	KW_SHIP,    // The Megarocket: Keen Worlds' own world, the inside of Keen's ship (on Keen 4)
	KW_DREAM,   // Tuberia: "Keen Dreams", the dream Keen has in the ship's bed (its own code, on Keen 4's)
	KW_MARS,    // Marooned on Mars - Galaxy: in-engine arcade adventure, never a planet
	KW_ALPHAMATIC, // Atroxian Realm's sequel (append: retain existing save IDs)
	KW_SUBURB, // Earth: Suburb Shenanigans (append: retain existing save IDs)
	KW_NUM_WORLDS,
	KW_NO_WORLD = -1,
} KW_World;

// The world currently loaded.
int KW_CurrentWorld(void);
// A slower reveal after falling asleep or waking; consumed once by loading.
bool KW_FadeDreamIn(void);

// Whether a world's files are installed (fan games: in play/mods/<id>/).
bool KW_IsWorldPresent(int world);

// The world's name in the Megarocket menu, and its short id (e.g. "keen7").
const char *KW_WorldName(int world);
const char *KW_WorldId(int world);

// Which world the player has asked to travel to, or KW_NO_WORLD.
extern int kw_pendingWorld;

// Ask the game loop to travel to another world once the current play loop
// ends.
void KW_RequestWorld(int world);

// Unload the current world's data and load another in its place, swapping
// the per-world parts of the game state. Keen arrives by its ship or on its
// world map.
void KW_SwitchWorld(int world);

// Called after a world's original ending has played: the run continues on
// that world's map, and the world is remembered as completed.
void KW_AfterWorldEnding(void);

// Whether a world's ending has been seen in this run.
bool KW_IsWorldCompleted(int world);

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

// On a world map, Up near the launch spot asks to board the Megarocket.
// Other buttons continue to enter ordinary level markers.
void KW_TryLaunch(struct CK_object *mapKeen);

// Called every frame from the play loop: opens the Megarocket menu when asked
// for, and handles the debug shortcut (on a world map, hold W and press 4, 5,
// 6 or 7).
void KW_PlayLoopHook(void);
// The remake shares the running engine and saves; arcade tallies stay isolated.
bool KW_BeginArcade(void);
bool KW_CanReturnToCabin(void);
void KW_TestArcadeSession(void);
void KW_RequestCabinReturn(void);
void KW_FinishMenuActions(void);
bool KW_ArcadeOver(bool won);
bool KW_RestartArcade(int difficulty);
bool KW_FadeArcadeIn(void);

// Whether the loaded level is the current world's Megarocket level.
bool KW_IsInShipLevel(void);

// Called when map-Keen spawns: in a world with a Megarocket parked on its
// map, arriving (or starting fresh) puts him right beside it.
struct CK_object;
void KW_PlaceMapKeenAtShip(struct CK_object *mapKeen);

// ---- Saved games ----

// Written at the start of every save: the world it was made in, every
// world's progress and all of Keen's items.
bool KW_WriteSaveBlock(FS_File fp);

// Reads the block. Returns true if the file is a Keen Worlds save of a world
// that is installed. If that is another world, the load is postponed
// (*deferred is set): it finishes after the menu closes, via
// KW_FinishDeferredLoad(). Otherwise the progress is applied at once.
bool KW_ReadSaveBlock(FS_File fp, bool *deferred);

// The save file being loaded (set by the menu's load routine).
extern const char *kw_loadingFileName;

// Called before play resumes: completes a postponed load by switching world
// and loading the save there. Returns true if it did so.
bool KW_FinishDeferredLoad(void);

// ---- New games ----

// Remember the world the program was started in (the start world): the
// launch episode, or /WORLD <id> (e.g. /WORLD keen7).
void KW_SetStartWorld(void);

// A new game always begins in the start world.
void KW_ReturnToStartWorld(void);

// Wait until no buttons are held (after a menu or dialogue), so the press
// that closed it doesn't act again.
void KW_WaitForButtonsUp(void);

// Developer self-test, run with /KWTEST.
void KW_SelfTest(void);
void KW_ChronicleSelfTest(void);
void KW_TestBoarding(void);
void KW_TestMenuScreens(void);
// Developer tests: real save slots, always run with an isolated USERPATH.
bool KW_TestWriteSave(void);
bool KW_TestReadSave(void);
void KW_Screenshot(const char *worldId, int level, const char *path);

// A new game: Keen begins inside his ship (when the Megarocket's world is
// there), landed on the world the game starts in.
void KW_BeginInShip(void);

// The game loop, before the ship lands on a world Keen has not been to:
// that world's own story.
void KW_StoryBeforeLanding(void);

#endif
