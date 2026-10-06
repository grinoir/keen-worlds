"""The smaller worlds speak (Keen Worlds): Aspermontis (The Eight
Accumulators), Meurglys III (Bounty Hunting Brouhaha), the Omegamatic II
(Keen 5.5) and Tuberia (Keen Dreams).

    python3 tools/kw_packs_voice.py omnispeak/data/voice

The words are the games' own. Recurring characters keep their voices
(tools/kw_keen4_voice.py and the tools after it); new here:

  Elvis              am_echo      lowered a little   (Meurglys III)
  The potion man     am_santa                         (Meurglys III)
  Rick Uller         am_liam                          (Meurglys III)
  The Narln          am_adam      raised              (the Omegamatic II)
  Billy's mom        af_sarah                         (Keen Dreams' ending)

A clip has the name of the window it is spoken in, with the world's prefix
(see tools/kw_keen789_voice.py): K95_ for Aspermontis, BH_ for Meurglys III,
K55_ for the Omegamatic II, KD_ for the dream. Address Unknown (Keen Asylum)
and the Shikadi Station say Keen 5's words where they speak, and use Keen
5's clips. The endings' last pages (what to download next, the author's
postscript) are silent.

Who speaks in Aspermontis's ending is read off its pictures (the pages are
Keen 4's ending re-lettered) and may be wrong in a line or two.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN, NARRATOR, LINDSEY, JANITOR, COUNCIL, ORACLE

ELVIS, POTION, RICK, NARLN, MOM = 'am_echo', 'am_santa', 'am_liam', 'am_adam', 'af_sarah'
STYLE = dict(BASE_STYLE)
STYLE[ELVIS] = dict(pitch=0.94)
STYLE[NARLN] = dict(pitch=1.15)
GAP = 0.25   # seconds between one speaker and the next
ECHO = 'aecho=0.8:0.6:110|190:0.35|0.2,alimiter=limit=0.95'            # the Oracle's hall
WET = 'vibrato=f=8:d=0.7,tremolo=f=13:d=0.8,lowpass=f=1500,volume=1.6,alimiter=limit=0.95'   # under water

K, N, C, J, R, A = KEEN, NARRATOR, COUNCIL, JANITOR, RICK, NARLN
CLIPS = {
    # ---- Aspermontis (the windows of Keen 4)
    'K95_COUNCIL.RAW': [(C, ["Thanks once again, Commander Keen.", "That explosion really shook me around."])],
    'K95_KEEN_COUNCIL0.RAW': [(K, ["Whatever you say."])],
    'K95_KEEN_COUNCIL1.RAW': [(K, ["Back to the Oracle, pronto."])],
    'K95_KEEN_COUNCIL2.RAW': [(K, ["I don't care."])],
    'K95_KEEN_COUNCIL3.RAW': [(K, ["I don't really care, just get back to the Oracle chamber..."])],
    'K95_KEEN_COUNCIL4.RAW': [(K, ["Yep.", "That's good."])],
    'K95_KEEN_COUNCIL5.RAW': [(K, ["Stop yapping, and get back to the Oracle..."])],
    'K95_KEEN_COUNCIL6.RAW': [(K, ["I am not having a good day, mate."])],
    'K95_KEEN_COUNCIL7.RAW': [(K, ["Finally!", "I've just about had enough of you guys.", "To the Oracle!"])],
    'K95_LINDSEY1.RAW': [(LINDSEY, ["It seems you dropped your wetsuit over at the Pyramid of Light."])],
    'K95_LINDSEY2.RAW': [(LINDSEY, ["Footsville holds the path to the Isle of Periculum, within a hut."])],
    'K95_KEEN_LINDSEY1.RAW': [(K, ["Umm.", "Are you sure?"])],
    'K95_KEEN_LINDSEY2.RAW': [(K, ["Thanks.", "I'll keep that in mind!"])],
    'K95_JANITOR1.RAW': [(J, ["Oh...", "Hi...", "Umm...", "Yeah...", "I kind of..."])],
    'K95_JANITOR2.RAW': [(J, ["I decided to help you out.", "But I got stuck here..."])],
    'K95_KEEN_JANITOR.RAW': [(K, ["Stuck?", "How?"])],
    'K95_JANITOR4.RAW': [(J, ["Giant flying stone feet..."])],
    'K95_KEEN_CANTSWIM.RAW': [(K, ["Wetsuit?"])],
    'K95_KEEN_WETSUIT.RAW': [(K, ["Wow!", "I didn't think I'd find this!"])],
    'K95_END_00.RAW': [(C, ["Now, why doesn't this thing work?"]), (None, 1.0), (J, ["I have found our problem!"])],
    'K95_END_01.RAW': [(J, ["This might have had something to do with it!"])],
    'K95_END_02.RAW': [(K, ["Now that the Janitor has figured out that problem,"
                            " let's find out what happened to Mortimer in the end..."])],
    'K95_END_03.RAW': [(K, ["Here are all eight of the accumulators."])],
    'K95_END_04.RAW': [(K, ["In they go..."]), (None, 1.0),
                       (C, ["Thanks for cleaning all that soot off the Oracle, Janitor."])],
    'K95_END_06.RAW': [(K, ["Let's get on with it."]), (None, 0.8), (K, ["The suspense is killing me."])],
    'K95_END_08.RAW': [(ORACLE, ["Mortimer McMire is still alive.", "No big surprise.", "It seems he is reading some notes."])],
    'K95_END_09.RAW': [(ORACLE, ["Hang on.", "It..."])],
    'K95_END_10.RAW': [(ORACLE, ["It seems he knows that we are watching him!"])],
    'K95_END_11.RAW': [(K, ["Now that that's over, I'm going home!", "I hope dinner isn't cold yet."])],
    'K95_END_12.RAW': [(C, ["Farewell.", "I'm sure we'll have more adventures in the future."])],
    'K95_END_13.RAW': [(K, ["Come on Spot, we're going home!"])],
    # ---- Meurglys III (the windows of Keen 4: Elvis where Lindsey is, the
    # potion man where the janitor is)
    'BH_LINDSEY1.RAW': [(ELVIS, ["Need a passcode?", "Try seven, three, zero, nine.", "Uh huh huh!"])],
    'BH_LINDSEY2.RAW': [(ELVIS, ["Plants on this island store nutrients by extruding sucrose rods."])],
    'BH_KEEN_LINDSEY1.RAW': [(K, ["Wow, you are the king!"])],
    'BH_KEEN_LINDSEY2.RAW': [(K, ["Sure looks like candy to me..."])],
    'BH_JANITOR1.RAW': [(POTION, ["Whoa there, young fella!", "Drink this potion.", "It'll fortify you for your quest."])],
    'BH_JANITOR2.RAW': [(POTION, ["It has a bit of a kick on it, so be careful."])],
    'BH_KEEN_JANITOR.RAW': [(K, ["I'm not sure about this."])],
    'BH_JANITOR4.RAW': [(POTION, ["There you go, down the hatch!", "How's the taste?"])],
    'BH_KEEN_WETSUIT.RAW': [(K, ["The jig is up.", "You're coming with me."])],
    'BH_END_00.RAW': [(N, ["Rick Uller sits in the lotus position.", "He is levitating about four feet off the carpet."]),
                      (R, ["Ah, Mister Blaze!", "You're just in time to join me for some transcendental meditation."]),
                      (K, ["Enough chit-chat.", "Where's the plutonium?"])],
    'BH_END_01.RAW': [(R, ["No one on Earth knows I'm here, so I'm assuming the bounty hunter's union sent you.",
                           "Look at you.", "The great Commander Keen, compromising his ethics for profit.",
                           "What did they promise you?", "Rocket upgrades?", "Your own planet?",
                           "Mining rights on one of the cheese moons?"]),
                      (K, ["...a bicycle."])],
    'BH_END_03.RAW': [(R, ["Alright, enough joking around.", "I have good news, bad news, and even worse news.",
                           "The good news is, the rightful owner of the plutonium has stolen it back.",
                           "The bad news is, he's using it to build a weapon.",
                           "Which is what I was trying to prevent by stealing it."]),
                      (K, ["What's the even worse news?"])],
    'BH_END_04.RAW': [(R, ["It's Mortimer McMire."])],
    'BH_END_05.RAW': [(K, ["Well, here we go again."])],
    'BH_END_06.RAW': [(R, ["Now now!", "He may have the upper hand, but now is not the time for despair.",
                           "After all, many hands make light work."]), (K, ["Huh?"])],
    'BH_END_07.RAW': [(R, ["I know this isn't normally your style, but would you consider a..."])],
    'BH_END_08.RAW': [(K, ["Partnership?"])],
    # ---- the Omegamatic II (Keen 5's fuse window; its ending talks to "you")
    'K55_KEEN_FUSE.RAW': [(K, ["Fuses?", "What fuses?"])],
    'K55_KEEN_FUSE_KORATH.RAW': [(K, ["Fuses?", "What fuses?"])],
    'K55_END_00.RAW': [(N, ["You have completed this obnoxiously hard level pack!",
                            "Commander Keen has saved the day once again!",
                            "But, as you might expect, things are still in motion...."])],
    'K55_END_01.RAW': [(N, ["Astonishingly, a trap door opens, and a Narln climbs out, and ambles toward you,"
                            " through the smoke billowing from the Q E D's giant, and now broken, fuse.",
                            "Needless to say, your jaw gapes open rather stupidly."]),
                       (K, ["Whaaaat...?"]), (N, ["The Narln looks at you shrewdly."]),
                       (A, ["Commander Keen, you didn't do so badly.", "We thought you might need a further test.",
                            "A pop test, if you will.",
                            "That last test was all well and good, but it wasn't much of a surprise, was it?"]),
                       (K, ["But, but who...?", "What...?"]),
                       (N, ["You trail off, unable to continue on to where, when, why, and how."])],
    'K55_END_02.RAW': [(N, ["The Narln reaches into his pocket, pulls out a small control, and pushes a button on it.",
                            "Instantly, a wall slides up, revealing a whole crowd of Narln,"
                            " some with clipboards, others working at computers and television monitors.",
                            "It strikes you as highly reminiscent of the opening scene in Monsters, Inc.",
                            "Understandably, you look around, somewhat startled.",
                            "Some of the Narln you recognize wave cheerfully at you.", "You wave vaguely in return.",
                            "The Narln before you smiles slightly, and pockets the device before continuing..."])],
    'K55_END_03.RAW': [(A, ["Planning for Operation KeenTest Two began while you were actually still involved in the first Test,"
                            " and construction began shortly thereafter.",
                            "We realized that there were several inherent flaws in the first Operation KeenTest.",
                            "For one, you were given a great deal of information before entering the ship.",
                            "We believed that a sudden, unexpected task, one you thought to be genuine,"
                            " was required to test your ability to withstand the stresses"
                            " associated with the fate of the universe being in your hands.",
                            "Believe me when I tell you that it has certainly been a difficult task keeping K T Two a secret.",
                            "A whole division of the Operation was assigned with the sole task"
                            " of the security and integrity of the project."])],
    'K55_END_04.RAW': [(K, ["So, uh, this has been a test,"]),
                       (N, ["you say rather dully, trying to catch up with the Narln."]),
                       (A, ["Yes,"]), (N, ["comes the irritated reply,"]),
                       (A, ["yes, a test.", "Now, I'm sure you want to know how well you did, hmmm?",
                            "Overall, we can't complain, except for the small matter of Quick and Easy.",
                            "Now, you opted to take that level's initial exit, rather than search it thoroughly.",
                            "As it is, you did indeed miss something, something we hoped you would not miss, but feared you would:",
                            "A fuse.", "You know how important hidden fuses are!",
                            "It was the destruction of a single fuse that enabled the Shikadi's arrest!"])],
    'K55_ENDS_04.RAW': [(K, ["So, uh, this has been a test,"]),
                        (N, ["you say rather dully, trying to catch up with the Narln."]),
                        (A, ["Yes,"]), (N, ["comes the irritated reply,"]),
                        (A, ["yes, a test.", "Now, I'm sure you want to know how well you did, hmmm?",
                             "Overall, we can't complain.", "We're especially proud of your behavior on Quick and Easy.",
                             "You opted to not take that level's initial exit, but rather to search it thoroughly.",
                             "As it is, you did find something, something we hoped you would not miss, but feared you would:",
                             "A fuse.", "You know how important hidden fuses are!",
                             "It was the destruction of a single fuse that enabled the Shikadi's arrest!"])],
    'K55_END_05.RAW': [(A, ["Now, there is not technically a consequence for failing to break this fuse;",
                            "rather, there is a lack of a reward."]),
                       (N, ["The Narln holds up a small pamphlet."]),
                       (A, ["This outlines the key secret areas of the ship, which contain many, many valuable items.",
                            "A good number of the areas are virtually impossible to access"
                            " without prior knowledge of their presence.", "That is what makes this"]),
                       (N, ["he shakes the pamphlet for emphasis"]),
                       (A, ["so valuable.", "Break that fuse next time, and this is yours.",
                            "Now, I'm sure you have a lot to ask us."]),
                       (N, ["You certainly do."])],
    'K55_ENDS_05.RAW': [(A, ["Since you chose the more difficult path, there is a reward for you."]),
                        (N, ["The Narln holds up a small pamphlet."]),
                        (A, ["This outlines the key secret areas of the ship, which contain many, many valuable items.",
                             "A good number of the areas are virtually impossible to access"
                             " without prior knowledge of their presence.", "That is what makes this"]),
                        (N, ["he shakes the pamphlet for emphasis"]),
                        (A, ["so valuable.", "Here, have it.", "I'm sure it will come in useful.",
                             "Now, I'm sure you have a lot to ask us."]),
                        (N, ["You certainly do."])],
    'K55_END_06.RAW': [(N, ["You have a rather long discussion with all of the Narln,"
                            " about the minutiae of your performance in the so-called Omegamatic Two.",
                            "In the final analysis, you certainly do have some weak points that Mort might turn to his advantage.",
                            "Both you and the Narln, however, can't complain,"
                            " and the Narln do admit that it was a rather difficult test.",
                            "After a final remark about the missed fuse, the first Narln offers you a free,"
                            " complimentary tune-up on your Bean-with-Bacon Megarocket, as well as a lift home."])],
    'K55_END_07.RAW': [(N, ["On the way home, during a lull in the conversation, you decide to turn on the ship's radio.",
                            "After some channel-surfing, you come upon...", "Mortimer McMire, discussing his next plan.",
                            "You turn to the Narln, about to make some comment about two surprise tests in one day,"
                            " but find him looking petrified.", "To be continued...."])],
    # ---- Tuberia (Keen Dreams: its ending's three windows, and the warning
    # at Boobus Tuber's castle)
    'KD_BOMBS.RAW': [(N, ["You can't possibly defeat King Boobus Tuber with less than twelve bombs!"])],
    'KD_END_00.RAW': [(N, ["Yes!", "Boobus Tuber's hash-brown-like remains rained down from the skies,"
                           " as Commander Keen walked up to the Dream Machine.",
                           "He analyzed all the complex controls and readouts on it,"
                           " then pulled down a huge red lever, marked On Off Switch.",
                           "The machine clanked and rattled, then went silent.",
                           "He had freed all the children from their vegetable-enforced slavery!",
                           "Everything around Keen wobbled in a disconcerting manner, his eyelids grew heavy, and he fell asleep...."])],
    'KD_END_01.RAW': [(N, ["Billy woke up, looking around the room, the early morning sun shining in his face.",
                           "Nothing.", "No vegetables to be seen.", "Was it all just a dream?",
                           "Billy's mom entered the room."]),
                      (MOM, ["Good morning, dear.", "I heard some news on T V that you'd be interested in,"]),
                      (N, ["she said, sitting by him on the bed."]),
                      (K, ["What news?"]), (N, ["Billy asked, still groggy."])],
    'KD_END_02.RAW': [(MOM, ["The President declared today National I Hate Broccoli Day.",
                             "He said kids are allowed to pick one vegetable today, and they don't have to eat it."]),
                      (K, ["Aw, mom, I'm not afraid of any stupid vegetables,"]), (N, ["Billy said."]),
                      (K, ["But if it's okay with you, I'd rather not have any french fries for a while."]),
                      (N, ["The end."])],
}
EFFECTS = {ORACLE: ECHO}


def main(folder):
    os.makedirs(folder, exist_ok=True)
    raw = ['-f', 's16le', '-ar', str(RATE), '-ac', '1']
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
            if voice in EFFECTS:
                for seg in segs:
                    path = os.path.join(tmp, seg)
                    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', path, '-af', EFFECTS[voice]] + raw + [path + '.fx'], check=True)
                    os.replace(path + '.fx', path)
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
    # Aspermontis's council member under water
    src, dst = os.path.join(folder, 'K95_COUNCIL.RAW'), os.path.join(folder, 'K95_COUNCIL_WET.RAW')
    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', src, '-af', WET] + raw + [dst + '.tmp'], check=True)
    os.replace(dst + '.tmp', dst)
    mine = [f for f in os.listdir(folder) if f.startswith(('K95_', 'BH_', 'K55_', 'KD_'))]
    print('%d clips, %.1f MB' % (len(mine), sum(os.path.getsize(os.path.join(folder, f)) for f in mine) / 1e6))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
