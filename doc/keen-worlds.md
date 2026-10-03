# Keen Worlds implementation notes

This branch extends Omnispeak into one connected Commander Keen 4–6 game.
The first working connected-world milestone is commit `9011ebd`.

## Travel direction

The intended player flow is to board the Bean-with-Bacon Megarocket, rather
than selecting a destination beside a generic world-map spawn:

- Keen 4: use its playable **BWB Megarocket** level (level 18).
- Keen 5: use the ship docked beside the start of the Armageddon Machine map.
  Keen 5 has no separate Megarocket level.
- Keen 6: use its playable **BWB Megarocket** level (level 17). Its existing
  local Fribbulus-Xax rocket role remains separate from inter-world travel.

## Keen 5 dock bridge

The Keen 5 map-Keen spawn is at `(28, 95)`. The gangway immediately east of
it uses foreground tiles `0x62F` at `(29, 95)` and `0x665` at `(30, 95)`.
Both are solid on every side, and an exhaustive scan of all Keen 5 maps found
no other use of either tile ID. The first collision-only experiment was useful
for confirming the route, but it visibly left Keen walking through open space.

Keen Worlds now replaces those *map cells* at load time with foreground tiles
`0x64A` and `0x64B`, existing gray ship/platform art that forms a short metal
gangway to the docked ship. Both replacement tiles are naturally walkable. The
overlay is applied through `RF_ReplaceTiles`, which updates both the loaded
map and the renderer, so the gangway is visible immediately rather than only
after a later redraw.
Map-Keen movement is restricted to the horizontal bridge and stops at the
ship, while allowing him to walk back into the station. The change is made in
the decompressed runtime map, rather than by editing `GAMEMAPS.CK5` or
`TILEINFO.CK5`; reloading the map restores the supplied data.

The unused yellow map-border background (`0x352`) is also replaced in memory
with a sparse black starfield tile (`0x327`) used beside the dock. This removes
the yellow void without producing a dense, repetitive star pattern or changing
any supplied data.

### Replacement gate art

`assets/keen5-boarding-gate-concept.png` is an EGA-style visual brief for the
eventual purpose-drawn dock gate: a solid, lit metal gangway connecting the
station airlock to the Bean-with-Bacon. It is deliberately not packed into the
game yet. It must first be reduced and cut into 16×16 EGA tiles, then imported
through the Galaxy graphics toolchain; keeping it separate makes the current
functional build safe to revert independently of the art experiment.

This follows normal Galaxy modding practice: foreground collision is defined
by TileInfo, and community tooling such as Abiathar exposes a Tile Property
Modifier for changing it. Sources: [KeenWiki's Galaxy modding tutorial](https://keenwiki.shikadi.net/wiki/Keen_Galaxy_Modding_Tutorial_%28patch_based%29)
and [Abiathar's documentation](https://abiathar.keenmodding.org/AbiatharHelp.pdf).

## Ship launch locations

The original generic launch location is replaced as follows:

| Episode | Depart from | Arrive at |
| --- | --- | --- |
| Keen 4 | Megarocket hatch, level 18 | Megarocket level 18 |
| Keen 5 | Docked ship, three tiles east of the map spawn | Keen 5 map / dock |
| Keen 6 | Megarocket hatch, level 17 | Megarocket level 17 |

The hatch interaction boxes are deliberately wider than a single tile so a
player can stand naturally at the visible entrance. Press jump, pogo or fire
there to open the existing destination menu. On departure from a ship level,
Keen Worlds retains the map position saved when that level was entered; it
does not mistakenly save the in-level coordinate as a world-map coordinate.
The Keen 4 and 6 interaction bounds intentionally cover the complete visible
spaceship, rather than only the hatch tile.

When Keen arrives from another world, a one-shot flag moves the already-spawned
player to the clear approach immediately left of the ship: `(4, 12)` in Keen
4's level 18 and `(5, 22)` in Keen 6's level 17. The Keen 6 placement also avoids
the unrelated map-transition marker at `(15, 22)`. Entering these levels
normally from their own world maps continues to use their original entrance
spawns. The placement uses the same 241-unit vertical offset as
`CK_SpawnKeen()`, so Keen starts on the floor rather than embedded inside it.
Cross-world arrivals face left, toward the Megarocket; ordinary local entries
keep their original facing.

## Validation and rollback

- `make -C src -j2` must build without warnings or errors.
- Manually check that Keen can cross the Keen 5 gangway and that the change
  does not create another route out of the dock.
- Each focused step is committed separately. Use `git revert <commit>` to
  reverse just that checkpoint, or return to `9011ebd` for the previously
  play-tested connected-world build.

## Keen 5 boarding gate (our own art)

The bridge from the Omegamatic dock to Keen's ship is new EGA art, not stock
tiles: a gate with a pulsing energy field, a ribbed tube with running lights,
and a collar clamped onto the ship's nose. Three animation frames.

- Art: `tools/kw_gate_art.py` in the project root (outside this repo) draws
  it in the 16 EGA colours on a 48×48 canvas covering map tiles 29–31 ×
  94–96. `python3 tools/kw_gate_art.py preview OUT.png` shows it over the real
  dock; `csource src/kw_gate_art.h` regenerates the engine data.
- Engine: `KW_BuildKeen5Gate()` in `src/kw_world.c`, run when the Keen 5 map
  spawns Keen. Each gate tile is the art laid over the original tile beneath
  it, stored in an empty, unused masked-tile slot (990–1029). Those slots have
  no data in `EGAGRAPH.CK5`, so the cache never overwrites them. Tile
  properties are set at runtime: Keen's walkway row is walkable, other rows
  copy the original collision, and the frames link as a foreground
  animation. Player data files stay untouched.
- `/KWTEST` checks the gate is placed, all tiles exist and the walkway is
  walkable.

## Saved games, settings and high scores

- **One set of save slots** for all worlds: `KWORLDS0.SAV` … `KWORLDS9.SAV`
  (pattern `KW_SAVE_FILE_PATTERN` in `src/kw_world.h`), instead of
  `SAVEGAM?.CKx` per episode. Slot headers carry the tag `KWS` instead of the
  episode extension, so every save is listed in every world. Original
  Omnispeak/DOS saves are not loaded.
- **Save contents**: a Keen Worlds block (`KW_WriteSaveBlock`: id `KWLD`,
  version, the world it was saved in, every world's map position / levels
  done / visited / completed, and all episode items), followed by the normal
  Omnispeak save of the current level. Bump `KW_SAVE_VERSION` when the block
  changes (for example when RPG data is added).
- **Loading a save from another world** cannot switch episodes while the menu
  is open (it would free the menu's graphics). `CK_LoadGame` stops after the
  block and remembers the file; `KW_FinishDeferredLoad()` runs when play
  resumes, switches world and loads the save there.
- **One config file**, `KWORLDS.CFG` (settings and the single high-score
  table), instead of `CONFIG.CKx` per episode. Old per-episode configs are not
  read, so key bindings start at their defaults once.
- **New games** start in the world the program was launched in
  (`play/run.sh` → Keen 4; `run.sh 5` / `6` for testing), even if the last run
  ended elsewhere.
- **Endings**: after a world's original ending the run continues on that
  world's map; the world is marked completed. High scores are submitted at
  game over.

`/KWTEST` covers a cross-world and a same-world save/load round trip.

## Habitable levels (first slice)

Once Keen has completed a level that has **residents**, coming back finds it
peaceful: enemies are removed after the level spawns, and the residents are
added. Stand next to a resident and press **up** to talk.

- Residents are listed in `kw_residents[]` in `src/kw_habitat.c`: world,
  level, the tile they stand on (one row above the floor, like an info-plane
  spawn marker), their line and Keen's reply. Lines use `\n`.
- First resident: an Oracle Council elder in Keen 4's Border Village (level
  1), inside the first house. The house door already leads to a small room at
  the bottom left of the map (tile 7, 48); the elder stands at (4, 48).
- The elder reuses the council member's sprites and walking animation but is
  spawned as `CT_Friendly`, so touching him doesn't trigger a rescue. Residents
  carry a tag and their table index in `user4`, which is saved with the game.
- The conversation reuses the council rescue layout: elder portrait and his
  line, then Keen's portrait and reply.
- Completed levels normally lose their world-map entrance
  (`CK_MapLevelMarkAsDone` clears the info tiles). Levels with residents keep
  their `0xC0xx` entrance so Keen can visit; fences still open and the done
  flag still shows. Other completed levels stay closed, as in the originals.
- Known gap: Keen 4 dart guns share object type 1 with harmless objects and
  are not removed yet (Border Village has none).
- `/KWTEST` checks Border Village before (enemies, no resident) and after
  completion (no enemies, one elder in the house).

## New art in the maps (`src/kw_art.c`)

Both the Keen 5 boarding gate and the creator's portrait use one mechanism:
art drawn by a script in the project's `tools/` folder is generated into a
`src/kw_*_art.h` header, then `KW_PlaceArt()` lays each tile over the original
tile beneath it, stores it in an empty, unused masked-tile slot, sets its tile
properties (collision copied from the original, optional walkable row,
animation frames), and puts it on the loaded map. Player data files are never
changed. `KW_IsArtPlaced()` lets `/KWTEST` check it.

To add art: write a generator like `tools/kw_portrait_art.py` (draw on a
canvas, `preview` it over the real level, `csource` it into a header), then
describe it with a `KW_Art` and call `KW_PlaceArt()` when that level loads.

### When art appears

Level art is listed in `kw_levelArt[]` in `src/kw_habitat.c`, each with a
rule: `KW_ART_AFTER` (only once the level is completed — **the default for
Keen Worlds art**: the villagers are back with their things), `KW_ART_BEFORE`
(only until then) or `KW_ART_ALWAYS`. Each piece reserves its own range of
tile slots in its generator (`SLOT_RANGE` / `*_SLOTS`) so pieces never share
slots.

### Border Village's tavern

`tools/kw_barhouse.py` builds it from Border Village's own tiles plus new art
and writes `src/kw_tavern_art.h` (tile edits + art); the engine applies it in
`KW_ChangeLevel()` (`src/kw_habitat.c`).

- **The inn**: one long stone house replacing the fourth house, x 75–91,
  rows 9–15 (always there). It is art: the first house's picture stretched
  at the pixel level — a strip under a level stretch of roof is repeated, the
  wall below filled with plain wall texture and the roof's underside drawn as
  one straight band, so there are no seams — with a second door pasted on.
  The left door sits on the fourth house's door tiles (x 77–78) and still
  leads to its original room; the right one (x 85–86) is the tavern's. Door
  tiles stay under the art so both keep their door flags.
- **The BAR sign**: on an iron bracket fixed to the wall just right of the
  tavern door, below the roof's shadow line (always there; the boarded door
  already shows the tavern is closed).
- **Before liberation**: boards over the door (art with `clearMisc`, so the
  door flag is gone) and no door destination — it can't be entered.
- **After**: the door destinations are written (street door ↔ room door) and
  the room is furnished.
- **The room**: the first house's room widened (pole columns left out),
  carved into the rock band at x 58–81, rows 26–32 — high enough that the
  camera never shows the existing room below (rows 43–48). Inside: the creator's
  portrait just inside the door (`tools/kw_portrait_art.py`), two tables with
  stools, and the bar along the right end — shelves of bottles, a BAR sign,
  the counter, two barrels on the floor, and the barmaid behind the counter
  drying a glass (4 animation frames). She is drawn in the style and at the
  size of Keen's sprites (black outlines, about Princess Lindsey's size);
  `tools/kw_barhouse.py preview` shows the bar with Keen's real sprite for
  scale (`keenrender.Sprites`). Talk to her with **up** at the bar (a "painted" resident: part of
  the art, with a talk spot instead of a character object).
- Map edits have a `when` (always / after), level art a rule (after — the
  default — before, or always).

Tile slots: art never uses a tile slot that has graphics, appears on a level,
or is the target of an original animation link (Keen 4's tile 1275 links to
the empty 1276; reusing it broke the world map's animation chain).
`/KWTEST` loads every Keen 4 level after the changes.

### Leaving a peaceful level

Peaceful levels can also be left through the right side: walking right into
the wall at the map's right edge (within 5 tiles of it) ends the level, like
walking off the left side (`KW_CheckPeacefulExit`).
