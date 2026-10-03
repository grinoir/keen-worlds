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
