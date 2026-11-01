"""Revival of Bliss speaks (Keen Worlds): its dialogue windows in both
builds, in its own words.

    python3 kwtools/kw_revival_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. A clip has the name of the window
it is spoken in (Keen 4's), with the world's prefix: RV_ for the Keen build,
RC_ for the Craig build (see kw_music.c). Its endings are spoken by
kw_dave216_end_voice.py. Keen, Craig, Myah, Lt. Barker keep their voices.
New here, PROVISIONAL until the user picks by ear (see art/voices/):

  The Yorps       (Lindsey's windows)      bf_lily, raised
  The visions     (the Janitor's orb)      af_nicole (Keen's) and am_echo,
                                           lowered (Craig's), with an echo

Who says what. Keen build: Myah, where Keen 4 has the council, looks for an
officer-bot (Keen answers); the Yorps, who learned their English from
Craig, give the hints; in the orb Keen sees a vision that says "Reunion".
Craig build: Lt. Barker mends Craig's pogo stick, one repair at a time
(Craig answers); the Yorps again; Craig's vision says "Last".
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K
from kw_keen789_voice import BARKER
from kw_aurora_voice import CRAIG, STYLE as AURORA_STYLE
from kw_canmore_voice import MYAH

YORPS, VISION_K, VISION_C = 'bf_lily', 'af_nicole', 'am_echo'
STYLE = dict(BASE_STYLE)
STYLE[CRAIG] = AURORA_STYLE[CRAIG]
STYLE[YORPS] = dict(pitch=1.3)
STYLE[VISION_C] = dict(pitch=0.85)
ECHO = 'aecho=0.8:0.85:120|260:0.45|0.3,volume=1.1'
EFFECT = {VISION_K: ECHO, VISION_C: ECHO}
GAP = 0.25
C = CRAIG

CLIPS = {
    # ---- as Keen
    'RV_COUNCIL.RAW': [(MYAH, ["Human boy! This bot is not the right one.",
                               "I'll have to keep looking for an officer-bot."])],
    'RV_COUNCIL_LAST.RAW': [(MYAH, ["Yes! This is the right one!", "Let me just mount it on your helmet. There!",
                                    "You're now disguised! Go!"])],
    'RV_KEEN_COUNCIL0.RAW': [(K, ["This is one of those days..."])],
    'RV_KEEN_COUNCIL1.RAW': [(K, ["Just my luck. Darn.", "Bye, Myah."])],
    'RV_KEEN_COUNCIL2.RAW': [(K, ["Just lovely."])],
    'RV_KEEN_COUNCIL3.RAW': [(K, ["I wonder where Craig has gone to. Hmm...", "Fine. I'll see you..."])],
    'RV_KEEN_COUNCIL4.RAW': [(K, ["Are you serious?"])],
    'RV_KEEN_COUNCIL5.RAW': [(K, ["I wonder if Krile still have those kebabs..."])],
    'RV_KEEN_COUNCIL6.RAW': [(K, ["I really hope we're getting closer..."])],
    'RV_KEEN_COUNCIL7.RAW': [(K, ["Well, I'll be...", "Great, now I hope this works out.", "Wish me luck!"])],
    'RV_LINDSEY1.RAW': [(YORPS, ["Charmita, Capetano!", "Follow the path, and future will present itself.", "It is hidden."])],
    'RV_LINDSEY2.RAW': [(YORPS, ["Charmita, Capetano!", "Is our Senpai back on Aurora Bliss?", "Master Craig is the best! Right?"])],
    'RV_KEEN_LINDSEY1.RAW': [(K, ["That journal will aid."])],
    'RV_KEEN_LINDSEY2.RAW': [(K, ["Are we talking about the same Craig?"])],
    'RV_JANITOR1.RAW': [(K, ["This orb is showing me two guys sitting on a tree.", "Word reunion reverberates."])],
    'RV_JANITOR2.RAW': [(K, ["There is this hint of sadness in this projection.", "I wonder..."])],
    'RV_KEEN_JANITOR.RAW': [(VISION_K, ["Reunion...", "Reunion."])],
    'RV_JANITOR4.RAW': [(K, ["I should remember this..."])],
    'RV_KEEN_CANTSWIM.RAW': [(K, ["No I D chip!"])],
    # ---- as Craig
    'RC_COUNCIL.RAW': [(BARKER, ["Craig! I can help you fix your pogo!", "Just see me once I find all the R P stations."])],
    'RC_COUNCIL_LAST.RAW': [(BARKER, ["Alright. It's fixed.", "Sorry it took so long. I am not the best handyman.",
                                      "It should work perfectly."])],
    'RC_KEEN_COUNCIL0.RAW': [(C, ["What? One won't do?", "Okay, Barker."])],
    'RC_KEEN_COUNCIL1.RAW': [(C, ["How many screws do you need?"])],
    'RC_KEEN_COUNCIL2.RAW': [(C, ["Oh, dude..."])],
    'RC_KEEN_COUNCIL3.RAW': [(C, ["How difficult is it to fix a pogo? How?", "Fine. I'll see you..."])],
    'RC_KEEN_COUNCIL4.RAW': [(C, ["Oh, brother..."])],
    'RC_KEEN_COUNCIL5.RAW': [(C, ["Well, I never saw that one coming...", "Wow. O M G."])],
    'RC_KEEN_COUNCIL6.RAW': [(C, ["It's almost fixed.", "One repair to go."])],
    'RC_KEEN_COUNCIL7.RAW': [(C, ["It only took you eight full repair stations to fix a toy.", "Thanks!"])],
    'RC_LINDSEY1.RAW': [(YORPS, ["Oi, oi, senpai Craig!", "In the great hollow. Between the Earth and the Moon.", "Hidden switch."])],
    'RC_LINDSEY2.RAW': [(YORPS, ["Oi, oi, senpai Craig!", "Use the G-force to get past this dreadful enemy.",
                                 "Best of luck, senpai."])],
    'RC_KEEN_LINDSEY1.RAW': [(C, ["Is that haiku or what?"])],
    'RC_KEEN_LINDSEY2.RAW': [(C, ["Thanks, guy!", "What the heck is G-force?"])],
    'RC_JANITOR1.RAW': [(C, ["This orb is showing me two guys sitting on a sleepy tree.", "Word last echoes within."])],
    'RC_JANITOR2.RAW': [(C, ["Watching this makes me sad for some weird reason.", "I should go..."])],
    'RC_KEEN_JANITOR.RAW': [(VISION_C, ["Last... last...", "Last..."])],
    'RC_JANITOR4.RAW': [(C, ["I should remember this..."])],
    'RC_KEEN_CANTSWIM.RAW': [(C, ["Wrong way!"])],
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
