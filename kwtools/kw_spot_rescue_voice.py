"""Spot's rescue on Fribbulus Xax, spoken (Keen Worlds).

    python3 kwtools/kw_spot_rescue_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. Keen, Spot, the ship computer and
the narrator keep their voices. The words are Keen Worlds' own (the Spot
story's third episode was never released), in src/kw_story_scenes.c.
"""
import sys
from kw_landing_voice import speak_all, VOICE as COMPUTER
from kw_keen4_voice import KEEN, NARRATOR
from kw_keen789_voice import SPOT, STYLE

CLIPS = {
    'SP_CUT_ORBIT.RAW': (NARRATOR, ['With the Shikadi shield down, the Megarocket drops through the clouds of Fribbulus Xax.',
                                    'The Bloogs are holding Spot in Bloogton. Keen has been here before.']),
    'SP_CUT_LAND.RAW': (COMPUTER, ['Landing in Bloogton.', 'I am detecting one Yorp. It is complaining loudly.']),
    'SP_CUT_SPOT1.RAW': (SPOT, ['Keen! I knew you would come!', 'They kept trying to teach me to fetch.']),
    'SP_CUT_LOCK.RAW': (KEEN, ['Nobody kidnaps my Yorp.', "Hold still, I'll get the lock."]),
    'SP_CUT_HOME.RAW': (KEEN, ["Come on, Spot. We're going home."]),
    'SP_CUT_SPOT2.RAW': (SPOT, ['Can we stop for snacks on the way?']),
    'SP_CUT_DEPART.RAW': (NARRATOR, ['The Bloogs watched them go.',
                                     'None of them tried to stop the boy with the pogo stick.', 'Not again.']),
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
