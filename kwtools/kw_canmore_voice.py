"""Canmore's Redemption speaks (Keen Worlds): its dialogue windows, in its own
words.

    python3 kwtools/kw_canmore_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. A clip has the name of the window
it is spoken in, with the world's prefix CR_ (see kw_music.c). Its windows
are Keen 9's friends' messages (kw_keen9.c KW9_FriendMessage, keyed by the
friend Keen 9 has there): one per level where a fuse marker is touched.
Keen keeps his voice, Craig the one he has in Aurora Bliss. New in this
world:

  Myah           (level 5)                 af_sky

Who says what: Myah gives Craig the body armour (level 5); Craig reads the
two notes he finds (levels 11 and 2: the windows show the notes); Keen, who
broke free with Mortimer, calls for help everywhere else.
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K
from kw_aurora_voice import CRAIG, STYLE as AURORA_STYLE

MYAH = 'af_sky'
STYLE = dict(BASE_STYLE)
STYLE[CRAIG] = AURORA_STYLE[CRAIG]
GAP = 0.25
EFFECT = {}

CLIPS = {
    'CR_HINT_LINDSEY.RAW': [(MYAH, ["Human boy, take this body armor.", "Al's lasers won't be able to pierce it!"])],
    'CR_HINT_BARKER.RAW': [(CRAIG, ["What is this?", "I wonder what... what does this mean?"])],
    'CR_HINT_SPOT.RAW': [(CRAIG, ["Hmmm...", "It seems like a piece of note.", "I'll take it."])],
    'CR_HINT_JANITOR.RAW': [(K, ["We broke free but...", "Mort passed out.", "Listen, find us a way out of here!"])],
    # Initiation Party, its epilogue: Keen arrives at the party (its council
    # member's window; Saint of Aurora's has only "...", unspoken)
    'PA_KEEN_COUNCIL0.RAW': [(K, ["Hey, sorry I am late.", "Let's eat!"])],
}


def main(folder):
    os.makedirs(folder, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        by_voice, parts = {}, {}
        for name, turns in CLIPS.items():
            for i, (voice, words) in enumerate(turns):
                seg = 'SEG_%s_%d.RAW' % (name[:-4], i)
                by_voice.setdefault(voice, {})[seg] = words
                parts.setdefault(name, []).append((voice, seg))
        for voice, segs in by_voice.items():
            speak_all(segs, voice, tmp, **STYLE.get(voice, {}))
        raw = ['-f', 's16le', '-ar', str(RATE), '-ac', '1']
        for name, segs in parts.items():
            out = bytearray()
            for k, (voice, seg) in enumerate(segs):
                path = os.path.join(tmp, seg)
                if voice in EFFECT:
                    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', path, '-af', EFFECT[voice]] + raw + [path + '.fx'], check=True)
                    path += '.fx'
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(path, 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
            print('%-24s %5.1f s' % (name, len(out) / 2 / RATE))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
