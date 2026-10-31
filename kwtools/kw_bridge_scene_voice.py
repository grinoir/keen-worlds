"""Keen Worlds' own bridge scenes, spoken: Krisle's new ship, the bounty
notice for Meurglys III, the messages after the Alphamatic, the Shikadi
signal before Gnosticus IV, the time-bending rings of Gnosticus Pi and
Billy home from school after Suburb Shenanigans.

    python3 kwtools/kw_bridge_scene_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. Everyone keeps their voice: Keen,
the ship computer (with its two chime notes) and Elder Krislen (Krisle's,
kw_arealm_voice.py). The words are in src/kw_story_scenes.c.
"""
import sys
from kw_landing_voice import speak_all, VOICE as COMPUTER
from kw_keen4_voice import KEEN, NARRATOR
from kw_arealm_voice import ELDER, STYLE as AR_STYLE
from kw_keen789_voice import SPOT, STYLE

STYLE = dict(STYLE, **{ELDER: AR_STYLE[ELDER]})

CLIPS = {
    # Krisle: the Krile could not mend the Megarocket
    'KR_CUT_ELDER.RAW': (ELDER, ['Your ship could not be mended.',
                                 'So we ordered you a new one, from Sklargifax Space Industries.']),
    'KR_CUT_SAME.RAW': (KEEN, ["It's... almost the same."]),
    'KR_CUT_SEATS.RAW': (COMPUTER, ['I have been moved into a cheaper ship.', 'The seats are plastic.']),
    'KR_CUT_FLY.RAW': (KEEN, ['Does it fly?']),
    'KR_CUT_MOSTLY.RAW': (COMPUTER, ['Mostly.']),
    # Meurglys III: why Keen hunts a bounty
    'BH_CUT_NOTICE.RAW': (COMPUTER, ['Bounty notice from Meurglys III.', 'Wanted: Rick Uller.',
                                     'Reward: a mystery prize.']),
    'BH_CUT_IN.RAW': (KEEN, ["A mystery prize? I'm in."]),
    'BH_CUT_KNOW.RAW': (COMPUTER, ['You do not even know what it is.']),
    'BH_CUT_MYSTERY.RAW': (KEEN, ["That's what makes it a mystery!"]),
    # after the Alphamatic
    'AB_CUT_MESSAGES.RAW': (COMPUTER, ['Two messages.', 'The council of Gnosticus IV want a word with you.',
                                       'And VitaCorp has put a price on your head.', 'One billion vitas.']),
    'AB_CUT_WORTH.RAW': (KEEN, ["A billion? I'm worth way more than that."]),
    'AB_CUT_SELL.RAW': (COMPUTER, ['I will not be selling you.']),
    'AB_CUT_HOME.RAW': (KEEN, ["Let's stop at home first.", "Spot's waiting in the yard."]),
    # before Gnosticus IV
    'K4_CUT_SIGNAL.RAW': (COMPUTER, ['Incoming signal.', 'Shikadi frequency.', 'Very rude.']),
    'K4_CUT_COUNCIL.RAW': (KEEN, ["They're going after the Gnosticene Council!", 'Set a course for Gnosticus IV.']),
    'K4_CUT_WIRES.RAW': (COMPUTER, ['Course set.', 'Please stop poking my wires.']),
    # before Gnosticus Pi: why its story can be any time
    'FF_CUT_RINGS.RAW': (COMPUTER, ['The note leads to Gnosticus Pi.', 'Its rings bend time.',
                                    'When we land, it may be last year.', 'Or next week.']),
    'FF_CUT_WHEN.RAW': (KEEN, ['So my plush friends could be anywhere...', 'and any-when!']),
    'FF_CUT_WORDS.RAW': (COMPUTER, ['Please do not invent words in my cockpit.']),
    # after Suburb Shenanigans: home from school
    'HM_CUT_DUSK.RAW': (NARRATOR, ['The school day ended, eventually.',
                                   "Mortimer's dummies were dealt with, at least until tomorrow."]),
    'HM_CUT_DAY.RAW': (KEEN, ['What a day.', 'Robot Mortimers at school, and I still have homework.']),
    'HM_CUT_SPACE.RAW': (SPOT, ['Can we go to space instead?']),
    'HM_CUT_FIRST.RAW': (KEEN, ['Homework first.', 'Then space.']),
    'HM_CUT_WARM.RAW': (COMPUTER, ['I have kept the engine warm.']),
    'HM_CUT_SHH.RAW': (KEEN, ['Shh! Mom will hear you!']),
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
