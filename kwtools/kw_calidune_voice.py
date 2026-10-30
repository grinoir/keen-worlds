"""Calidune's arrival and ending scenes, spoken (Keen Worlds).

    python3 kwtools/kw_calidune_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. Keen and the ship computer keep
their voices (kw_keen4_voice.STYLE; the computer has its two chime notes).
The words are Keen Worlds' own, in src/kw_calidune_scene.c.
"""
import sys
from kw_landing_voice import speak_all, VOICE as COMPUTER
from kw_keen4_voice import KEEN, STYLE

CLIPS = {
    'CD_CUT_WARN.RAW': (COMPUTER, ['Warning. A transporter beam has locked onto you.',
                                   "It has Mortimer's signature."]),
    'CD_CUT_AGAIN.RAW': (KEEN, ['Not again!']),
    'CD_CUT_SAND.RAW': (KEEN, ['Sand, sun... and no ship.', 'Thanks a lot, Mortimer.']),
    'CD_CUT_COPY.RAW': (COMPUTER, ['You found a copy of me.', 'It is not as good.']),
    'CD_CUT_OFF.RAW': (KEEN, ["It got me off Calidune, didn't it?"]),
}


def main(output):
    by_voice = {}
    for name, (voice, words) in CLIPS.items():
        by_voice.setdefault(voice, {})[name] = words
    for voice, clips in by_voice.items():
        speak_all(clips, voice, output, **STYLE.get(voice, {}))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
