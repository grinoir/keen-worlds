COMMANDER KEEN: WORLDS - test version
=====================================

Keen 4, 5 and 6 and the fan games, joined into one galaxy that Keen
travels in the Bean-with-Bacon Megarocket.

This is a test version. Things will be wrong. Finding them is the point.


1. What you need to add
-----------------------

Keen Worlds brings the game program and its own files. It does not
contain the Commander Keen games: those are yours to supply.

Copy these files into this folder, next to the program. All must be
version 1.4 (EGA).

  Keen 4: Secret of the Oracle       GAMEMAPS.CK4  EGAGRAPH.CK4  AUDIO.CK4
  Keen 5: The Armageddon Machine     GAMEMAPS.CK5  EGAGRAPH.CK5  AUDIO.CK5
  Keen 6: Aliens Ate My Baby Sitter  GAMEMAPS.CK6  EGAGRAPH.CK6  AUDIO.CK6

Keen 4 is shareware and free (Apogee's CK4SW14 package). Keen 5 and 6 are
from your own copy. You can start with Keen 4 alone; the others show as
"not installed" in the ship's flight menu.

If your Keen 6 is version 1.5, copy the files from the keen6-v1.5 folder
over the ones here first.

The fan games go in the mods folder. See mods/README.txt.


2. Starting
-----------

  Windows   double-click KeenWorlds.cmd
  Linux     ./keenworlds.sh       (needs SDL2: the package is called
                                   libsdl2-2.0-0, sdl2 or SDL2)

The title appears, then the story. Any key opens the menu: New Game.

You start inside the Megarocket. Walk to the flight console and push up to
choose where to fly. Walk to the hatch to go out. The bed is for sleeping.


3. Keys
-------

  Arrow keys   move, look up and down, enter doors
  Ctrl         jump
  Alt          pogo stick
  Space        shoot
  Enter        status
  Esc          menu: save, load, sound, keys
  F11          full screen

A voice speaks the conversations. Press a key to move a text on.


4. What to try
--------------

  - A new game in Free Flight. Fly to a world from each group in the
    flight menu.
  - Play a world through to its ending.
  - Save in one world, fly to another, load the save again.
  - Go to bed in the ship.
  - A new game in the Chronicle, where the story decides the order.


5. Reporting a problem
----------------------

Please tell:

  - which world and level you were in, and what you did just before;
  - what happened, and what you expected;
  - the version: the line in VERSION.txt.

And send, if you can:

  - keenworlds-log.txt from this folder (copy it before you start the
    game again; each start writes a new one);
  - a saved game from just before the problem: the KWORLDS?.SAV files in
    this folder;
  - a screenshot if something looks wrong.


6. Who made this
----------------

Keen Worlds is built on Omnispeak, an open-source rebuild of the Keen 4-6
engine (see AUTHORS.txt). The program is under the GNU General Public
License, version 2 (LICENSE.txt); its source is in source.tar.gz.

Commander Keen is by id Software. Every fan game is its authors' work,
and each one names its makers when Keen first lands there.

Keen Worlds was linked together by GriNoir.
