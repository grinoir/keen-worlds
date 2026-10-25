# Chocolate Keen native arcade runtime

Source: https://github.com/jamesfmackenzie/chocolatekeen
Commit: `5a7665e80b19d7f0dedbc3806bfc20f0925ae38f`
License: GPL-2.0 (see COPYING; individual files retain their notices).
Authors: NY00123, QuantumG, Lemm, James Mackenzie and contributors.

Only engine source and resources are vendored. No original game data is included.
The DOS executables are read as data for the native engine, never executed.

Local integration changes:
- app/chocolate-keen.c accepts explicit game-data and per-game save directories.
- platform/input_stub.c resolves those directories and case-insensitive data names.
- The build includes string.h for strict C99 compilers.
- The build uses SDL2 software rendering and all three episodes.
- Arcade exit goes straight back to the cabin without the DOS order/info
  screen. The native player's graphics fade out before shutdown.

Keen Worlds launches the native companion with an argument vector, suspends
its own simulation, and restores the cabin when the child exits. The runtime
has no access to Keen Worlds' game state or world completion records.
