# Keen Worlds presentation customization

This file is the map for changing non-gameplay screens without touching the
original Keen data files. The first customized screen is the game-completion
card, shared by Keen 4, Keen 5, and Keen 6.

## Shared completion card

`src/ck_text.c`, function `help_endgame()`, draws the card. Its editable text
is the `US_CPrint(...)` string in that function. The current copy is:

```text
Hacker GriNoir brings you

KEEN WORLDS

Tying together Keen 4, 5, 6, and fan games.
Visit completed levels and more.
```

The final `Press any key to continue` line is part of the same string. `\n`
creates a new line; use `\n\n` for a blank line. Keep each individual line
comfortably short, since the screen is 320 pixels wide.

The card deliberately uses `VL_ClearScreen(4)`, the red background used by
the original ending pages, and the engine's normal white proportional font.
It starts each episode's configured ending music and dismisses with one key.
All three original `TEXT_END` / `TEXT_SECRETEND` graphic-text pages are now
bypassed, so their different shareware/order-form messages can no longer
appear at a Keen Worlds completion.

The three callers stay in `src/ck_game.c`: the Keen 4 council ending, Keen 5
QED ending, and Keen 6 Molly ending. Do not change the function name unless
all three callers are changed too.

## Other presentation screens worth changing later

| What you see | Main place to inspect | Notes |
| --- | --- | --- |
| Help, story, controls, ordering and About screens | `src/ck_text.c` plus `TEXT_*` entries in each episode's `STRINGS.CKx` / graphics data | These are formatted, multi-page graphic-text screens. Unlike the shared completion card, they still come from episode data. |
| Title, menu and high-score screens | `src/ck_inter.c` | These combine code layout with cached graphics and strings. |
| Quit/error screen | `src/ck_quit.c` | Engine-owned presentation; change carefully because it is also used for fatal errors. |
| In-level messages and dialogs | `src/ck4_misc.c`, `src/ck5_misc.c`, `src/ck6_misc.c` | Episode-specific game text. |
| Travel menu | `src/kw_world.c`, `KW_TravelMenu()` | Already project-owned; destination names and heading are safe to edit there. |

## Safe workflow

1. Make a focused source change and run `make -C src -j2` from `omnispeak/`.
2. Copy `bin/omnispeak` to `../play/omnispeak` for a local play test.
3. Commit the presentation change separately. If a design experiment is not
   wanted, use `git revert <commit>` rather than editing original data files.

This approach keeps stock assets intact and makes every Keen Worlds screen
change easy for Claude or another collaborator to review or reverse.
