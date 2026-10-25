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


@SUPPLY@


2. Starting
-----------

  Windows   double-click KeenWorlds.cmd
  Linux     ./keenworlds.sh       (needs SDL2: the package is called
                                   libsdl2-2.0-0, sdl2 or SDL2)

The title appears, then the story. A key on the title opens the menu:
New Game. During the story, hold Esc for one second to skip to the menu.

You start inside the Megarocket. Walk to the flight console and push up to
choose where to fly. Walk to the hatch to go out. The bed is for sleeping.


3. Keys
-------

  Arrow keys   move, look up/down; Up enters doors and boards the ship
  Ctrl         jump
  Alt          pogo stick
  Space        shoot
  Enter        status
  Esc          menu: save, load, sound, keys
  F1           the guide
  F11          full screen

Voices and volume: Configure > Options > Voices. Select volume to cycle
0-100% in 10% steps. Music and effects have their own settings.

Hold Esc or the controller's mapped menu button for one second to skip stories.
Xbox defaults to Y; remapped buttons are respected. Skip progress appears
immediately while held. Otherwise the hint stays hidden until an ordinary
key or non-menu controller button reveals it briefly.
Once narration finishes, a fresh press advances story pages. Popup dialogue
uses a fresh tap (including Esc), with no hold gesture or skip hint. A tap
can advance a spoken popup early. Release before advancing again.
The pause menu has two columns, with journey controls and options. Use
arrows or the controller to move between them; Esc resumes play. Return to
Cabin leaves the Galaxy arcade remake or wakes Keen from a dream.

The cabinet beside the bunk offers original Keen 1, 2 and 3. Press Up to
use it. Add your v1.31 games under arcade/keen1, keen2 and keen3; see
arcade/README.txt. Each keeps its own saves and scores. Exit the game to
return to the cabinet and cabin. Arcade completion is outside the Chronicle.
Under Keen 1, choose Original or Galaxy remake (Marooned on Mars - Galaxy).
The remake needs its converted files in arcade/mars. It runs inside the
current game and window, with progress in the shared save slots. Its own
score, lives, ammunition, items and difficulty remain separate from the
journey. New Game restarts that arcade adventure; End Game returns to the
cabin. Original Keen 1-3 keep their separate native player and save folders.


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
