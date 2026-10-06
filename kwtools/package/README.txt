COMMANDER KEEN: WORLDS - version @VERSION@
==============================================

Keen 4, 5 and 6 and the fan games, joined into one galaxy that Keen
travels in the Bean-with-Bacon Megarocket.

This is a beta. Every world can be played from landing to ending, but
few people have done it yet. Things will be wrong. Finding them is the
point. What is known to be missing is listed in section 5.


What Keen Worlds adds
---------------------

  - One galaxy. Thirteen worlds, the ship and a dream in one game, with
    one saved game for all of it.
  - A ship to walk around in. The Megarocket has a cabin, a flight console
    to choose a world at, a hatch to step out of, and a bed. The ship's
    computer says where it has landed.
  - Everybody talks. Every conversation in every world is spoken, each
    character in a voice of its own, Keen too. The music steps aside
    while they speak.
  - Stories read aloud. A narrator reads each world's story the first
    time Keen flies there, and Keen Worlds' own at the start.
  - Worlds that remember. A level Keen has finished can be visited again
    and may have changed: in the Border Village on Gnosticus IV people
    have moved in, and there is an inn.
  - Two ways to play. Free Flight, with every world open, or the
    Chronicle, where the worlds open in the order of the story.
  - A dream. Go to bed in the ship and Keen dreams Keen Dreams, the lost
    episode.
  - A beginning and an end of its own: a title, a tune, a story, and an
    ending for when every world is saved.
  - Credit where it is due. Every world names its makers when Keen first
    lands there.
  - Nothing to convert. A fan game is unpacked into its folder, and it is
    a world.

The same is in the game: press F1 for the guide.


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
over the ones here first. Other versions of Keen 6 do not fit; the program
says so when it starts instead of running.

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
  F1           the guide
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


5. What is not there yet
------------------------

No need to report these:

  - Life in the levels Keen has saved. When a level is done, Keen can go
    back and find it changed: people living there, an inn, someone to
    talk to. So far only one level has this, the Border Village on
    Gnosticus IV (Keen 4). More comes in later versions.
  - The voices cannot be turned off or made quieter by themselves.
  - Inside the dream (the ship's bed) there is no menu. Save in the ship.
  - Two fan games are not in: The Prisoner's Dilemma and Send in the
    Clorts.
  - The Shikadi Station has no ending text, and one of Gnosticus Pi's
    endings is not spoken.
  - The Windows version has been tried on few machines.

Good to know, and meant to be so:

  - Krisle is a one-way trip: the ship cannot leave until that world is
    saved. The game warns before flying there.
  - In the Chronicle, worlds open in the order of the story. In Free
    Flight everything is open.
  - Nothing is carried into or out of the dream.


6. Reporting a problem
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


7. Who made this
----------------

Keen Worlds is built on Omnispeak, an open-source rebuild of the Keen 4-6
engine (see AUTHORS.txt). The program is under the GNU General Public
License, version 2 (LICENSE.txt); its source is in source.tar.gz.

Commander Keen is by id Software. Every fan game is its authors' work,
and each one names its makers when Keen first lands there.

Keen Worlds was linked together by GriNoir.
