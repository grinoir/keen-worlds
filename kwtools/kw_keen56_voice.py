"""Keen 5 and Keen 6 speak: their lines as voice clips (Keen Worlds).

    python3 tools/kw_keen56_voice.py omnispeak/data/voice

The words are the games' own (their STRINGS files and ending scripts). Keen
and the narrator keep the voices they have in Keen 4 (tools/kw_keen4_voice.py);
Keen 6 brings three new ones:

  Keen                    bm_fable
  The narrator            bm_lewis     the endings, and the storybook lines
                                       inside Keen 6's windows ("It says,")
  The Grabbiter           am_fenrir
  The Bloogstar's door    am_adam      made a machine's
  Molly McMire            af_jessica

Keen 6 writes like a storybook: one window can hold the narrator and then
somebody's words. Such a clip is spoken in turns and joined. The endings'
last, selling lines ("See you Christmas '92...") are left silent.

Clips are K5_*.RAW and K6_*.RAW in the voice folder (signed 16-bit mono, low
byte first, 22050 Hz). The game plays K5_END_<nn>.RAW (K5_ENDS_<nn>.RAW in
Keen 5's secret ending, where it differs) and K6_END_<nn>.RAW with page nn
of an ending.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE   # how Keen's voice is made to say "Keen"

KEEN, NARRATOR, GRABBITER, DOOR, MOLLY = 'bm_fable', 'bm_lewis', 'am_fenrir', 'am_adam', 'af_jessica'
GAP = 0.25   # seconds between one speaker and the next

# clip -> turns: (voice, sentences)
CLIPS = {
    # ---- Keen 5
    'K5_KEEN_FUSE.RAW': [(KEEN, ["One of the four machines protecting the main elevator shaft:", "toast!"])],
    'K5_KEEN_FUSE_KORATH.RAW': [(KEEN, ["I wonder what that fuse was for...."])],
    'K5_END_00.RAW': [(NARRATOR, ["You have completed Goodbye, Galaxy!", "Commander Keen has saved the day once again!",
                                  "But things are still in motion...."])],
    'K5_END_01.RAW': [(NARRATOR, ["With their Quantum Explosion Dynamo in ruins, the Shikadi rush to their getaway ship,"
                                  " and hightail it back to their own galaxy."])],
    'K5_ENDS_01.RAW': [(NARRATOR, ["With their Quantum Explosion Dynamo in ruins, the Shikadi rush to their getaway ship,"
                                   " to hightail it back to their own galaxy.",
                                   "Due to a freak fuse malfunction, they cannot take off,"
                                   " and are arrested by the Korath Three Police, for double-parking."])],
    'K5_END_02.RAW': [(NARRATOR, ["Keen rummages through the control room of the Q E D,"
                                  " for clues as to why the Shikadi wanted to blow up the galaxy.",
                                  "He stumbles upon a note written in the Standard Galactic Alphabet."])],
    'K5_END_04.RAW': [(NARRATOR, ["Keen takes the note with him to decipher when he gets home,"
                                  " not seeing anything else of interest...."])],
    'K5_ENDS_04.RAW': [(NARRATOR, ["Keen takes the note with him to decipher on the long space voyage home,"
                                   " not seeing anything else of interest...."])],
    'K5_END_07.RAW': [(NARRATOR, ["Back at home after having saved the galaxy,"
                                  " Billy feeds his rain-soaked parents ice cream, and gently shakes them back to consciousness.",
                                  "Hopefully, this won't affect his allowance."])],
    # ---- Keen 6
    'K6_KEEN_SANDWICH.RAW': [(KEEN, ["This is the second biggest sandwich I ever saw!"])],
    'K6_KEEN_ROPE.RAW': [(KEEN, ["Wow!", "A rope and grappling hook!", "They look useful!"])],
    'K6_KEEN_PASSCARD.RAW': [(KEEN, ["What's this?", "Cool!", "A passcard for the Bloogstar Rocket!",
                                     "It can fly through their force field."])],
    'K6_KEEN_CLIFF.RAW': [(KEEN, ["What a tall cliff!", "Wish I had a rope and grappling hook."])],
    'K6_GRABBITER_HUNGRY.RAW': [(KEEN, ["Oh no!", "It's a slavering Grabbiter!"]), (NARRATOR, ["He says,"]),
                                (GRABBITER, ["Get me lunch, and I'll tell ya a secret!"])],
    'K6_GRABBITER_FED.RAW': [(NARRATOR, ["The Grabbiter grabs the gigantic sandwich, downs it in one bite, and says,"]),
                             (GRABBITER, ["Here's your secret.", "Big meals make me sleepy!"])],
    'K6_DOOR.RAW': [(NARRATOR, ["The door makes a loud blooping noise.", "It says,"]),
                    (DOOR, ["Passcard required for entry."])],
    'K6_END_00.RAW': [(MOLLY, ["Gee, thanks Billy,"]), (NARRATOR, ["said Molly."]),
                      (MOLLY, ["Boy, my brother is gonna get in trouble for this."]),
                      (KEEN, ["Your brother?"]),
                      (NARRATOR, ["Keen asked.", "Molly is the older sister of his former nemesis, Mortimer McMire.",
                                  "But he was caught in the explosion of his Mangling Machine...", "wasn't he?",
                                  "Could he be alive?"]),
                      (KEEN, ["But he's...", "he's..."])],
    'K6_END_01.RAW': [(MOLLY, ["He's in trouble is what he is.", "He told the Bloogs to take me hostage.",
                               "In exchange, he told them where to find the Stupendous Sandwich of Chungella Four."]),
                      (KEEN, ["Yeah, I've seen it,"]),
                      (NARRATOR, ["Keen said.", "So, Mortimer's back, eh? thought Keen."]),
                      (KEEN, ["Where is Mortimer?"])],
    'K6_END_02.RAW': [(MOLLY, ["Oh,"]), (NARRATOR, ["said Molly."]),
                      (MOLLY, ["He said something about blowing up the entire universe or something.",
                               "Well, let's get back before your folks get home."]),
                      (KEEN, ["Yeah, we'd better...", "THE UNIVERSE?!"]), (NARRATOR, ["Keen said."]), (KEEN, ["Yikes!"])],
}
# the door is a machine
ROBOT = ("afftfilt=real='hypot(re,im)*sin(0)':imag='hypot(re,im)*cos(0)':win_size=512:overlap=0.75,"
         "volume=1.5,alimiter=limit=0.95")


def main(folder):
    os.makedirs(folder, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        # every turn is spoken by itself, a voice at a time
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
                if voice == DOOR:
                    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', path, '-af', ROBOT] + raw + [path + '.fx'], check=True)
                    path += '.fx'
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(path, 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
            print('%-26s %5.1f s' % (name, len(out) / 2 / RATE))
    mine = [f for f in os.listdir(folder) if f.startswith(('K5_', 'K6_'))]
    print('%d clips, %.1f MB' % (len(mine), sum(os.path.getsize(os.path.join(folder, f)) for f in mine) / 1e6))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
