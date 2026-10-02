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

## Keen 5 dock overlay

The Keen 5 map-Keen spawn is at `(28, 95)`. The gangway immediately east of
it uses foreground tiles `0x62F` at `(29, 95)` and `0x665` at `(30, 95)`.
Both are solid on every side, and an exhaustive scan of all Keen 5 maps found
no other use of either tile ID.

Keen Worlds clears collision on precisely those two IDs after the Keen 5 data
files load. The change is deliberately made through the in-memory TileInfo
table, rather than by editing `GAMEMAPS.CK5` or `TILEINFO.CK5`. The table is
reloaded for each episode switch, so removing the two calls in
`KW_ApplyWorldPatches()` completely restores vanilla collision.

This follows normal Galaxy modding practice: foreground collision is defined
by TileInfo, and community tooling such as Abiathar exposes a Tile Property
Modifier for changing it. Sources: [KeenWiki's Galaxy modding tutorial](https://keenwiki.shikadi.net/wiki/Keen_Galaxy_Modding_Tutorial_%28patch_based%29)
and [Abiathar's documentation](https://abiathar.keenmodding.org/AbiatharHelp.pdf).

## Validation and rollback

- `make -C src -j2` must build without warnings or errors.
- Manually check that Keen can cross the Keen 5 gangway and that the change
  does not create another route out of the dock.
- Each focused step is committed separately. Use `git revert <commit>` to
  reverse just that checkpoint, or return to `9011ebd` for the previously
  play-tested connected-world build.
