"""Operation Ocflore speaks (Keen Worlds): its dialogue windows, in its own
words.

    python3 kwtools/kw_ocflore_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. A clip has the name of the window
it is spoken in, with the world's prefix OC_ (see kw_music.c). Keen and the
narrator keep their voices. New in this world:

  The Dorsal     (Lindsey's windows)       bf_isabella, lowered

Who says what: the narrator tells of each relic Keen finds (where Keen 4 has
a council member's thanks), Keen answers; the Dorsal, Ocflore's sea people,
give the hints where Keen 4 has Princess Lindsey.
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K, NARRATOR as N

DORSAL = 'bf_isabella'
STYLE = dict(BASE_STYLE)
STYLE[DORSAL] = dict(pitch=0.92)
GAP = 0.25
EFFECT = {}

CLIPS = {
    'OC_COUNCIL.RAW': [(N, ["Keen comes across an artifact the old Dorsal spoke of.", "An elaborate relic."])],
    'OC_KEEN_COUNCIL0.RAW': [(K, ["Such an intricate bit of gear!"])],
    'OC_KEEN_COUNCIL1.RAW': [(K, ["I wonder how these things work."])],
    'OC_KEEN_COUNCIL2.RAW': [(K, ["What now?"])],
    'OC_LINDSEY1.RAW': [(DORSAL, ["It's dangerous to stumble in teleporters at the Swallowed Spire."])],
    'OC_LINDSEY2.RAW': [(DORSAL, ["Keep an eye out for a purple key hidden inside a coral wall."])],
    'OC_KEEN_LINDSEY1.RAW': [(K, ["Don't you worry!"])],
    'OC_KEEN_LINDSEY2.RAW': [(K, ["Alright, I'll keep my eyes peeled!"])],
    'OC_KEEN_WETSUIT.RAW': [(K, ["This is mine now!", "Let's swim!"])],
    'OC_KEEN_CANTSWIM.RAW': [(K, ["I can't swim!"])],
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
