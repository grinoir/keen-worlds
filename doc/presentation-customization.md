# Keen Worlds presentation customization

This file is the map for changing non-gameplay screens without touching the
original Keen data files. The first customized screen is the game-completion
card, shared by Keen 4, Keen 5, and Keen 6.

## Keen Worlds card (quit screen)

When the game quits, `Quit()` in `src/ck_quit.c` prints a Keen Worlds card to
the terminal as an 80×24 text screen: starfield, blue double-lined box. It
replaces each episode's original order screen ("Thanks for playing ... call
Apogee").

Edit the text in `src/kw_card.c`: `kw_cardByline`, `kw_cardTitle` and the
`kw_cardBody` lines (an empty string is a blank line). Keep lines under 54
characters so they fit the box. Colours and layout are in
`KW_PrintQuitCard()` in the same file.

The in-game completion pages (`help_endgame()` in `src/ck_text.c`) are the
**original** episode endings and are deliberately left as they are.

## Other presentation screens worth changing later

| What you see | Main place to inspect | Notes |
| --- | --- | --- |
| Help, story, controls, ordering and About screens | `src/ck_text.c` plus `TEXT_*` entries in each episode's `STRINGS.CKx` / graphics data | These are formatted, multi-page graphic-text screens. Unlike the shared completion card, they still come from episode data. |
| Title, menu and high-score screens | `src/ck_inter.c` | These combine code layout with cached graphics and strings. |
| Error screen | `src/ck_quit.c` | Fatal errors still print the plain message; the normal quit now shows the Keen Worlds card. |
| In-level messages and dialogs | `src/ck4_misc.c`, `src/ck5_misc.c`, `src/ck6_misc.c` | Episode-specific game text. |
| Travel menu | `src/kw_world.c`, `KW_TravelMenu()` | Already project-owned; destination names and heading are safe to edit there. |

## Safe workflow

1. Make a focused source change.
2. Start the game with `play/run.sh`: it rebuilds and copies the binary first,
   and prints the commit it is running.
3. Commit the presentation change separately. If a design experiment is not
   wanted, use `git revert <commit>` rather than editing original data files.

This approach keeps stock assets intact and makes every Keen Worlds screen
change easy for Claude or another collaborator to review or reverse.
