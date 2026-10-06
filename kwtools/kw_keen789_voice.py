"""Keen 7, 8 and 9 speak: the lines of "The Universe Is Toast!" (Ceilick's
trilogy: The Keys of Krodacia, Dead in the Desert, Battle of the Brains) as
voice clips (Keen Worlds).

    python3 tools/kw_keen789_voice.py omnispeak/data/voice

The words are the games' own (their MOD files and ending scripts). Recurring
characters keep their voices (tools/kw_keen4_voice.py, kw_keen56_voice.py);
new here:

  Mortimer McMire   bm_daniel
  Lt. Barker        am_michael    (and Keen's friends on the vid-screen)
  The Dude          am_puck       the impostor Keen
  Spot              af_nova       raised a little

The games run on the engines of Keen 4, 6 and 5, so a clip has the name of
the window it is spoken in, with the world's own prefix: K7_COUNCIL.RAW is
what is said in Keen 7 where Keen 4 has a council member's thanks (there:
Keen finding a key). The narrator reads the prose between the quotes; the
endings' last, selling pages are silent. K9_ENDS_<nn>.RAW: the pages of Keen
9's secret ending that differ.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN, NARRATOR, LINDSEY, JANITOR

MORTIMER, BARKER, DUDE, SPOT = 'bm_daniel', 'am_michael', 'am_puck', 'af_nova'
STYLE = dict(BASE_STYLE)
STYLE[SPOT] = dict(pitch=1.12)
GAP = 0.25   # seconds between one speaker and the next

K, N, M, B = KEEN, NARRATOR, MORTIMER, BARKER
CLIPS = {
    # ---- Keen 7 (the windows of Keen 4)
    'K7_COUNCIL.RAW': [(K, ["Alright!", "This is a key to Mort's base!"])],
    'K7_KEEN_COUNCIL0.RAW': [(K, ["Now to look for more keys!"])],
    'K7_KEEN_COUNCIL1.RAW': [(K, ["I still need more keys!"])],
    'K7_KEEN_COUNCIL2.RAW': [(K, ["Woo-hoo!"])],
    'K7_KEEN_COUNCIL3.RAW': [(K, ["I wonder where the rest of the keys are?"])],
    'K7_KEEN_COUNCIL4.RAW': [(K, ["I need more keys!"])],
    'K7_KEEN_COUNCIL5.RAW': [(K, ["I wonder where the rest of the keys are?"])],
    'K7_KEEN_COUNCIL6.RAW': [(K, ["Great! The last key!", "Mort is toast!"])],
    'K7_LINDSEY1.RAW': [(B, ["Thanks for rescuing me!", "I would have jumped out, but it's too dark in here!"])],
    'K7_LINDSEY2.RAW': [(B, ["You can get a swim coin in Monk's house.", "Good luck, Commander Keen!"])],
    'K7_KEEN_LINDSEY1.RAW': [(K, ["No problem.", "Let's go!"])],
    'K7_KEEN_LINDSEY2.RAW': [(K, ["Thanks for the good advice, Lieutenant Barker!"])],
    'K7_KEEN_CANTSWIM.RAW': [(K, ["I need a coin."])],
    'K7_KEEN_WETSUIT.RAW': [(K, ["A swim coin!", "I bet I can use this!"])],
    'K7_JANITOR1.RAW': [(JANITOR, ["Thanks for going to all that trouble, but I'm just the janitor for the High Council."])],
    'K7_JANITOR2.RAW': [(JANITOR, ["I tried to tell the Shikadi that, but they just wouldn't listen..."])],
    'K7_KEEN_JANITOR.RAW': [(K, ["This had better be a joke."])],
    'K7_JANITOR4.RAW': [(JANITOR, ["Sorry.", "You aren't mad, are you?"])],
    'K7_END_00.RAW': [(K, ["Now that I have all the keys, I can open the door,"
                           " and destroy Mortimer's Universal Toaster Cannon!"])],
    'K7_END_02.RAW': [(K, ["I wonder what will be on the other side."]), (None, 1.6),
                      (K, ["Hmmm.", "It sure is taking a long time to open."])],
    'K7_END_04.RAW': [(M, ["Ha ha ha, Commander Clown, you fool!", "Once again you have fallen right into my hands.",
                           "The doors have closed behind you, and there is no escape!"])],
    'K7_END_05.RAW': [(M, ["This isn't really a Universal Toaster Cannon.", "What a stupid idea.",
                           "This is an Interstellar Transporter!"])],
    'K7_END_06.RAW': [(M, ["And you're going on a one way trip out of this galaxy,"
                           " where you'll be out of my way forever,", "and I'll be free to destroy the Universe!"])],
    'K7_END_07.RAW': [(M, ["Goodbye for the last time, three-fourteen!"])],
    'K7_END_08.RAW': [(N, ["The Interstellar Transporter rumbles to life!"])],
    'K7_END_09.RAW': [(N, ["The transporter beam begins to ignite!"])],
    'K7_END_10.RAW': [(N, ["The beam shoots out from Krodacia across space!"])],
    'K7_END_11.RAW': [(N, ["Keen is propelled through the galaxy at tremendous speed!"])],
    # ---- Keen 8 (the windows of Keen 6)
    'K8_KEEN_SANDWICH.RAW': [(K, ["A power source!", "I should bring it to the ship!"])],
    'K8_KEEN_ROPE.RAW': [(K, ["Wow! A lantern!", "I should hang on to this!"])],
    'K8_KEEN_PASSCARD.RAW': [(K, ["What's this?", "It looks like a key-card.", "I should take this with me, just in case!"])],
    'K8_GRABBITER_HUNGRY.RAW': [(DUDE, ["Don't stop now!", "Go find the battery so I...", "we, can get off this planet!"])],
    'K8_GRABBITER_FED.RAW': [(N, ["The Dude grabs the battery from Keen."]),
                             (DUDE, ["Ha ha! Now I can leave, and you'll be left behind!", "So long, sucker!"])],
    'K8_DOOR.RAW': [(K, ["Hmm.", "I need some kind of pass card to enter."])],
    'K8_KEEN_CLIFF.RAW': [(K, ["It's too dark in there!", "I need a lantern."])],
    'K8_END_00.RAW': [(N, ["With the imposter Commander Keen out of commission,"
                           " our hero quickly plants the Power Cell in the strange Bean-with-Bacon."]),
                      (K, ["Hopefully this works!"]),
                      (N, ["Keen says aloud to himself, as he powers up the vessel.",
                           "The crisp smell of bacon and fried beans greets his nostrils,"
                           " as the ship breaks through the planet's atmosphere."])],
    'K8_END_01.RAW': [(K, ["I'm glad I'm out of the desert, but where to now?", "I don't know where Mortimer is!"]),
                      (N, ["Suddenly the ship's vid-screen blurs to life.",
                           "For a long moment, only static and a distorted transmission gets through."])],
    'K8_END_02.RAW': [(N, ["The image clears, and Commander Keen recognizes his friends!"]),
                      (B, ["Commander Keen, thank goodness!", "We've been searching all over for you!",
                           "We must stop Mortimer McMire before it's too late!"]),
                      (K, ["No sweat, guys, just lead me to Mortimer.", "The only thing that's going to be toast is him!"])],
    # ---- Keen 9 (the window of Keen 5's fuse message: a friend's hint)
    'K9_HINT_LINDSEY.RAW': [(LINDSEY, ["You won't be able to defeat Mortimer with your stunner alone!"])],
    'K9_HINT_BARKER.RAW': [(B, ["Beware of the Krodacian Overlords!"])],
    'K9_HINT_SPOT.RAW': [(SPOT, ["The Shikadi are invincible while climbing poles!"])],
    'K9_HINT_JANITOR.RAW': [(JANITOR, ["There's something mysterious beneath the giant crater."])],
    'K9_END_00.RAW': [(N, ["Commander Keen narrowly rolls onto the safety of a platform, after opening the bridge above!",
                           "Mortimer McMire manages to grab a hold of the ledge as well!",
                           "With the station's four energy cores destroyed, a violent chain reaction ensues,"
                           " as the base begins to self-destruct!"])],
    'K9_END_01.RAW': [(M, ["You fool!", "Look what you've done!"]),
                      (N, ["Mortimer shouts furiously, hanging over a mechanical chasm.",
                           "Keen stands triumphantly over the ledge.",
                           "The wormholes Mortimer had intended to use to destroy the universe become unstable,"
                           " and begin opening at random intervals.",
                           "Having destroyed the station before Mortimer could detonate the sun, the wormholes are harmless!"])],
    'K9_END_02.RAW': [(K, ["This is the end of your Toast-Star, Mortimer,",
                           "and the end of your reign of terror throughout the universe!"])],
    'K9_END_03.RAW': [(N, ["The Grand Intellect manages a sneer."]),
                      (M, ["This battle isn't over, three-fourteen!",
                           "I won't rest until you, and all the other pathetic lifeforms in the universe, are erased from existence!"])],
    'K9_END_04.RAW': [(N, ["Commander Keen grits his teeth, swallowing his pride."]),
                      (K, ["Why?", "Why should everything be destroyed?", "You're the smartest being the universe has known.",
                           "Think of all the good you could do with your intelligence!"])],
    'K9_END_05.RAW': [(N, ["Mortimer laughs maniacally."]),
                      (M, ["Because they don't deserve it!", "They're all idiots, and incapable of appreciating such genius!",
                           "And no matter the I Q score, you're just as stupid as the rest of them!"])],
    'K9_END_06.RAW': [(K, ["That's not true!"]), (N, ["Keen insists."]),
                      (K, ["The Vorticons, the Shikadi, and Krodacians, they all saw in you a supreme intelligence, and a leader!",
                           "Face justice, Mortimer, and then show the universe the good you're capable of!"]),
                      (N, ["Keen holds out his hand to Mortimer."])],
    'K9_END_07.RAW': [(N, ["Mortimer seems conflicted.", "He looks Commander Keen in the eye."])],
    'K9_END_08.RAW': [(N, ["A sinister smile creeps over his face."]),
                      (M, ["Never with your help.", "It's the end for you, Commander Keen!"])],
    'K9_END_09.RAW': [(N, ["Letting go of the ledge, Mortimer whips out his Hyper Pistol,"
                           " firing several shots as he falls down the chasm.",
                           "Commander Keen barely ducks aside to avoid them."])],
    'K9_END_10.RAW': [(N, ["Keen chances a glance over the edge, seeing no sign of Mortimer in the depths below."])],
    'K9_END_11.RAW': [(N, ["Without much time to spare, with the station blowing up around him,"
                           " Commander Keen hurries to the transporter booth, and quickly engages the machine."])],
    'K9_END_12.RAW': [(N, ["Arriving back at the HUB, Keen meets up with his friends, and gives them the news.",
                           "The threat of Mortimer McMire is over!", "The universe is safe once again!"])],
    # the secret ending: one page more, so the last ones are two pages on
    'K9_ENDS_12.RAW': [(N, ["Meanwhile, in an empty quadrant in space..."])],
    'K9_ENDS_14.RAW': [(N, ["Arriving back at the HUB, Keen meets up with his friends, and gives them the news.",
                            "The threat of Mortimer McMire is over!", "The universe is safe once again!"])],
}


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
            print('%-26s %5.1f s' % (name, len(out) / 2 / RATE))
    mine = [f for f in os.listdir(folder) if f.startswith(('K7_', 'K8_', 'K9_'))]
    print('%d clips, %.1f MB' % (len(mine), sum(os.path.getsize(os.path.join(folder, f)) for f in mine) / 1e6))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
