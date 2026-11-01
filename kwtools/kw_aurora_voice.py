"""Aurora Bliss speaks (Keen Worlds): its dialogue windows, in its own words.

    python3 kwtools/kw_aurora_voice.py omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them. A clip has the name of the window
it is spoken in, with the world's prefix AB_ (see kw_music.c). Keen keeps
his voice. The voices chosen for this world:

  Craig          (Lindsey's windows)       am_liam, raised
  The Joy Bots   (council and janitor)     am_onyx, lowered, with the robot effect

Who says what: Craig speaks where Keen 4 has Princess Lindsey, the robots
("Joy Bots") where it has the council members and the janitor; Keen's
answers are his. The council's second, underwater line is never shown
(Aurora Bliss has no water level) and is not spoken.
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K
from kw_keen56_voice import ROBOT

CRAIG, BOT = 'am_liam', 'am_onyx'
STYLE = dict(BASE_STYLE)
STYLE[CRAIG] = dict(pitch=1.15)
STYLE[BOT] = dict(pitch=0.9)
GAP = 0.25
EFFECT = {BOT: ROBOT}

CLIPS = {
    'AB_LINDSEY1.RAW': [(CRAIG, ["What's up, Keen-dude?", "I've found this cool black hole that's full of riches, dude."])],
    'AB_LINDSEY2.RAW': [(CRAIG, ["What's up, Keen-dude?", "I am so danged bored.",
                                 "Have you, like, found the things for the thing?"])],
    'AB_KEEN_LINDSEY1.RAW': [(K, ["Cram it, Craig."])],
    'AB_KEEN_LINDSEY2.RAW': [(K, ["Craig, just go back inside the Bean-with-Bacon.", "Alright?"])],
    'AB_COUNCIL.RAW': [(BOT, ["Hey there, human!", "Do you want to be my new friend?", "I want to be your B F F."])],
    'AB_KEEN_COUNCIL0.RAW': [(K, ["Guess again, pal."])],
    'AB_KEEN_COUNCIL1.RAW': [(K, ["Nope.", "Just need your chips."])],
    'AB_KEEN_COUNCIL2.RAW': [(K, ["Well..."])],
    'AB_KEEN_COUNCIL3.RAW': [(K, ["I want you to be my new Navi-Com.", "Sorry, bot."])],
    'AB_KEEN_COUNCIL4.RAW': [(K, ["I need your RAM."])],
    'AB_KEEN_COUNCIL5.RAW': [(K, ["This isn't your day, Joy Bot.", "I'm sorry."])],
    'AB_KEEN_COUNCIL6.RAW': [(K, ["This should do it.", "Let's go home."])],
    'AB_KEEN_COUNCIL7.RAW': [(K, ["You're the last one, fella.", "Let's both get back to the Oracle chamber!"])],
    'AB_JANITOR1.RAW': [(BOT, ["Hey there, friend!", "Did you know that there is a secret map in the Door Temple?"])],
    'AB_JANITOR2.RAW': [(BOT, ["Please don't scrap me like the others!", "I want to join you!"])],
    'AB_KEEN_JANITOR.RAW': [(K, ["You can't join me, sorry."])],
    'AB_JANITOR4.RAW': [(BOT, ["I like hairdryers!", "They go woom!"])],
    'AB_KEEN_CANTSWIM.RAW': [(K, ["Air is toxic!"])],
    'AB_KEEN_WETSUIT.RAW': [(K, ["I can traverse the toxic cloud now!"])],
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
