"""The Shikadi Station's endings, spoken (Keen Worlds).

    python3 kwtools/kw_osi_end_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. The words are Operation Station
Infiltration's own (its text chunks 5008, the ending, and 5009, the ending
after the Korath fuse). A page n is OS_END_<n>.RAW; the fuse ending's pages
are OS_ENDS_<n>.RAW where they differ, and its first three pages, the same
as the ending's, fall back to OS_END_ (ck_text.c, help_endgame). Elsewhere
the station says Keen 5's words and uses Keen 5's clips (kw_music.c).

Who speaks is told by the colour of the text: D and C the two Bloogs, F the
officer and 7 the Sergeant aboard the station, the rest the narrator. New:

  The first Bloog    am_onyx      lowered
  The second Bloog   am_santa     lowered
  The officer        am_echo      raised
  The Sergeant       bm_daniel    lowered

The last page's "Find out in the next episode" is not read: Keen Worlds goes
on to Spot's rescue, not to that game. The fuse ending's page 4 is a moving
picture with no words; its clip is a moment of silence, so the ending's own
page 4 (the Bloogs) doesn't play over it.

Needs what kwtools/kw_landing_voice.py needs (Node, kokoro-js, ffmpeg).
"""
import os, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, NARRATOR

BLOOG1, BLOOG2, OFFICER, SERGEANT = 'am_onyx', 'am_santa', 'am_echo', 'bm_daniel'
STYLE = dict(BASE_STYLE)
STYLE[BLOOG1] = dict(pitch=0.82)
STYLE[BLOOG2] = dict(pitch=0.85)
STYLE[OFFICER] = dict(pitch=1.1)
STYLE[SERGEANT] = dict(pitch=0.88)
GAP = 0.25   # seconds between one speaker and the next

N = NARRATOR
BLOOGS = [(BLOOG1, ["Happy Bloog Day!", "Going to the big picnic after work?"]),
          (BLOOG2, ["I won't miss it!", "Alpha One, may he roost forever, has personally organized the menu!"]),
          (BLOOG1, ["Strange that an off-worlder would have such exquisite taste,",
                    "but Alpha One, may his gizzard be ever full, is a great leader!"])]
AFTER = [  # the pages that follow the Bloogs, in both endings
    [(N, ["The two Bloogs laugh together as they trundle off to their factory jobs."])],
    [(N, ["Elsewhere on Fribbulus Xax..."])],
    [(N, ["A sinister shadow snickers..."])],
    [(N, ["Will Keen crash the picnic in time?", "Will the Bloogs make a meal out of Spot?",
          "And who is this mysterious Alpha One?"])],
]
CLIPS = {
    'OS_END_00.RAW': [(N, ["Congratulations!", "You have completed Operation Station Infiltration.",
                           "Commander Keen is one step closer to saving his pet Yorp, Spot!",
                           "Let's see what happens next..."])],
    'OS_END_01.RAW': [(N, ["With the mainframe of the Omegamatrix destroyed,",
                           "the shield protecting Fribbulus Xax is permanently disabled!",
                           "Commander Keen returns to his Bean-with-Bacon Megarocket and begins his approach to the planet."])],
    'OS_END_02.RAW': [(N, ["The Shikadi once again flee in their getaway ship toward their home galaxy.",
                           "It's a shame the Milky Way has such intrusive neighbors..."])],
    'OS_END_03.RAW': [(N, ["Meanwhile, on Fribbulus Xax..."])],
    'OS_END_04.RAW': BLOOGS,
    'OS_ENDS_03.RAW': [(N, ["Meanwhile, aboard the Omegamatrix..."])],
    'OS_ENDS_04.RAW': [(None, 0.3)],
    'OS_ENDS_05.RAW': [(OFFICER, ["They've gone, Sergeant, the whole lot of them.", "The station is in shambles."]),
                       (SERGEANT, ["Whoever broke the fuse to our jail cell does not play nice when it comes to the Shikadi.",
                                   "All signs point to the vigilante Commander Keen!"])],
    'OS_ENDS_06.RAW': [(OFFICER, ["Should we pursue the Commander, sir?"])],
    'OS_ENDS_07.RAW': [(SERGEANT, ["Negative, the situation has already ballooned out of control.",
                                   "What started as a galactic no trespass warning has escalated to crimes of"
                                   " kidnapping officers of the law, the obstruction of justice,"
                                   " tacky wallpaper, and expired vehicle tabs.",
                                   "Let's get this bucket of bolts impounded and be done with it."])],
    'OS_ENDS_08.RAW': [(N, ["Meanwhile, on Fribbulus Xax..."])],
    'OS_ENDS_09.RAW': BLOOGS,
}
for i, turns in enumerate(AFTER):
    CLIPS['OS_END_%02d.RAW' % (5 + i)] = turns
    CLIPS['OS_ENDS_%02d.RAW' % (10 + i)] = turns


def main(folder):
    os.makedirs(folder, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        by_voice, parts = {}, {}
        for name, turns in CLIPS.items():
            for i, (voice, words) in enumerate(turns):
                if voice is None:
                    parts.setdefault(name, []).append((None, words))   # a pause of so many seconds
                    continue
                seg = 'SEG_%s_%d.RAW' % (name[:-4], i)
                by_voice.setdefault(voice, {})[seg] = words
                parts.setdefault(name, []).append((voice, seg))
        for voice, segs in by_voice.items():
            speak_all(segs, voice, tmp, **STYLE.get(voice, {}))
        for name, segs in parts.items():
            out = bytearray()
            for k, (voice, seg) in enumerate(segs):
                if voice is None:
                    out += bytes(2 * int(seg * RATE))
                    continue
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(os.path.join(tmp, seg), 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
            print('%-18s %5.1f s' % (name, len(out) / 2 / RATE))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
