"""The Prisoner's Dilemma speaks (Keen Worlds): its item and rendezvous
windows, in both builds, in the game's own words.

    python3 kwtools/kw_prisoner_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them to OGG. A clip is named for the
window it is spoken in (Keen 6's), with the world's prefix: PD_ for the Keen
build, PR_ for the Rick build (see kw_music.c). The two builds share their
windows but swap who speaks: as Keen, you find the items and meet Rick Uller
at the rendezvous; as Rick, Rick finds them and meets Keen. Keen and Rick
keep the voices they have everywhere (bm_fable and am_liam). New here:

  A-Danac officer   (the "uniformed personnel only" cliff)   bf_alice

Who says what. The hero speaks the three pickups (the pogo, "Hello, old
friend!"; the cop's uniform; and the rendezvous quip). The rendezvous
partner speaks the Grabbiter's two windows (as Keen that is Rick, as Rick
that is Keen). The A-Danac officer turns the hero away from the restricted
area until the uniform is worn.
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K
from kw_packs_voice import RICK as R

OFFICER = 'bf_alice'
STYLE = dict(BASE_STYLE)
STYLE[OFFICER] = dict(pitch=0.96)   # a clipped, no-nonsense desk officer
GAP = 0.25

# The hero's three pickup lines and the rendezvous banter, in the game's
# words (one window, split into phrases for natural pauses). The speaker is
# given per build below.
SANDWICH = ["Hello, old friend!"]
ROPE = ["A cop's uniform!", "I'm sure this'll come in handy."]
PASSCARD = ["I know I said I was vegetarian,",
            "but it doesn't count if the animal isn't from Earth!"]
HUNGRY = ["Where were you?!", "Anyway, I'll look for the magistrate here,", "you go over there."]
FED = ["The Chief Magistrate is probably in one of these buildings.", "I'll check here, you check there."]
CLIFF = ["Scram!", "This area is for uniformed personnel only!"]

# (build prefix, the hero's voice, the rendezvous partner's voice)
BUILDS = [('PD', K, R), ('PR', R, K)]

CLIPS = {}
for prefix, hero, partner in BUILDS:
    CLIPS['%s_KEEN_SANDWICH.RAW' % prefix] = [(hero, SANDWICH)]
    CLIPS['%s_KEEN_ROPE.RAW' % prefix] = [(hero, ROPE)]
    CLIPS['%s_KEEN_PASSCARD.RAW' % prefix] = [(hero, PASSCARD)]
    CLIPS['%s_GRABBITER_HUNGRY.RAW' % prefix] = [(partner, HUNGRY)]
    CLIPS['%s_GRABBITER_FED.RAW' % prefix] = [(partner, FED)]
    CLIPS['%s_KEEN_CLIFF.RAW' % prefix] = [(OFFICER, CLIFF)]


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
        for name, segs in parts.items():
            out = bytearray()
            for k, (voice, seg) in enumerate(segs):
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(os.path.join(tmp, seg), 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
            print('%-24s %5.1f s' % (name, len(out) / 2 / RATE))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
