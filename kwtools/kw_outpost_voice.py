"""Keen's outpost in Border Village, spoken (Keen Worlds): the opening scene
and the talks in the hut (Spot's, the Janitor's, Lindsey's).

    python3 kwtools/kw_outpost_voice.py omnispeak/data/voice omnispeak/src/kw_outpost_talk.h
    python3 kwtools/kw_outpost_voice.py - omnispeak/src/kw_outpost_talk.h    # the text only

Then kwtools/kw_voice_pack.py packs them. Everyone keeps their voice: Keen,
the narrator, the council elder, Princess Lindsey and the Janitor (Keen 4's,
kw_keen4_voice.py) and Spot (kw_keen789_voice.py). The scene's words are in
src/kw_story_scenes.c too, and the user approved them on 2026-10-31
(KEENS_OUTPOST.md in the workspace root). Spot's talk is written here, as
the barmaid's is in kw_barmaid_voice.py, and goes into the engine as C data.
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all
from kw_keen4_voice import KEEN, NARRATOR, COUNCIL as ELDER, LINDSEY, JANITOR, keen_says_file
from kw_keen789_voice import SPOT, STYLE
import kw_barmaid_voice as bar

CLIPS = {
    'OP_CUT_ROLL.RAW': (NARRATOR, ['The Council was scorched, but the village was not.',
                                   'The elder had been waiting by the old council hut, with a key.']),
    # (when The Eight Accumulators is not installed, the outpost opens after
    # Keen 4: there was no fire)
    'OP_CUT_ROLL2.RAW': (NARRATOR, ['Gnosticus Four was safe again.',
                                    'The elder had been waiting by the old council hut, with a key.']),
    'OP_CUT_EYES.RAW': (ELDER, ['The Council needs a pair of eyes on Gnosticus.', 'Young eyes.']),
    'OP_CUT_SCHOOL.RAW': (KEEN, ["I live on Earth.", "I've got school."]),
    'OP_CUT_SOUP.RAW': (ELDER, ['And a ship that can be here before your soup cools.', "We've seen it."]),
    'OP_CUT_SHELF.RAW': (SPOT, ['It needs a roof.', 'And a shelf.', 'You keep everything in your bag.']),
    'OP_CUT_VISIT.RAW': (LINDSEY, ["I'll visit.", "It's quieter than a palace."]),
    'OP_CUT_MOP.RAW': (JANITOR, ["I'll bring a mop."]),
    'OP_CUT_FINE.RAW': (KEEN, ['...Fine.', "But it's a hideout, not a home."]),
    'OP_CUT_TUESDAY.RAW': (NARRATOR, ['It was a home by Tuesday.']),
}

# Who talks in the hut: (what they say, [(what Keen says, the answer, where
# the talk goes on)]), as the barmaid's talk is written. Spot lives there; the
# Janitor keeps it clean; Lindsey visits after every other world saved.
END = None
SPOT_TALK = {
    'hello': ("You're back! I dusted the shelf. Well, I looked at it.", [
        ("How's the outpost?", "Quiet. The elder brings soup. It is not as good as your mom's.", 'more'),
        ("Seen anyone?", "Lindsey came by. She says the bunk is too hard for a princess. I said good.", 'more'),
        ("See you, Spot.", "Bring back something for the shelf!", END),
    ]),
    'more': ("Anything else?", [
        ("Want to come along?", "Next time. Someone has to guard the shelf.", END),
        ("See you, Spot.", "Bring back something for the shelf!", END),
    ]),
}
JANITOR_TALK = {
    'hello': ("Mind the floor, it's wet. I mop it every day. Nobody walks on it but you.", [
        ("You're on the Council?", "No, I'm the janitor. Everybody makes that mistake. Then they hand me a mop.", END),
        ("Thanks for the mopping.", "A clean hideout is a happy hideout. That's what I always say. Nobody listens.", END),
        ("See you!", "Wipe your feet on the way out.", END),
    ]),
}
LINDSEY_TALK = {
    'hello': ("Hello, Commander! I came to see the shelf everyone talks about.", [
        ("What do you think of it?", "It needs a crown. Every good shelf has a crown.", END),
        ("How's the palace?", "Loud. Very loud. That's why I'm here.", END),
        ("Bye, Lindsey!", "Goodbye, Commander. Fly safe.", END),
    ]),
}
# The shelves (one card per keepsake; not spoken) and what Spot says the
# first time he sees each (spoken), by world, as kw_outpost.py's KEEPSAKES.
KEEPSAKE_TEXT = {
    'KEEN4': "A red gem from the Shadowlands. It opened a door once.",
    'KEEN5': "A fuse from the Omegamatic. It won't blow up the galaxy any more.",
    'KEEN6': "A can of Bloog soda from Fribbulus Xax. Unopened. Nobody dares.",
    'KEEN7': "A key from Krodacia. Nobody knows which door it fits.",
    'KEEN8': "A jar of Calidune sand. It gets everywhere anyway.",
    'KEEN9': "A brain in a jar, from the Battle of the Brains. It is thinking about it.",
    'KEEN95': "One of the eight accumulators of Aspermontis. Still warm.",
    'AREALM': "A Krile crystal: a thank-you for saving Krisle.",
    'FITF': "The cryptic note that led Keen to Gnosticus Pi.",
    'ALPHAMATIC': "VitaCorp's decryption key, on a floppy disk. Label: DO NOT COPY.",
    'RTTS': "A lantern from the Shadowlands. Spot says it's creepy.",
    'OSI': "A pass for the Shikadi Station. Expired.",
    'CK55': "A gear from the Omegamatic II. It still turns, slowly.",
    'ASYLUM': "A letter with no address. Keen delivered it anyway.",
    'BOUNTY': "Rick Uller's wanted poster. Wanted no more.",
    'SUBURB': "Mortimer's dummy head. It still sparks sometimes.",
    'AURORA': "The can of Vorta Cola Craig spilled on the Navi-Com. Empty now.",
    'OCFLORE': "A vial of Ostrum from Ocflore V. The Ormch wanted it. It glows.",
    'CANMORE': "The keycard that opened Keen's cell on Azura Astrid.",
    'SAINT': "A flower from the meadow on Aurora Bliss where Craig rests.",
    'PARTY': "Shikadi soda from Captain Craig's party. No brewskies.",
}
SPOT_NEW = {
    'KEEN4': "A shiny gem! I'll put it where the sun hits it.",
    'KEEN5': "A fuse? Is it going to explode? ...Good.",
    'KEEN6': "Bloog soda! Don't open it. I've seen what it does.",
    'KEEN7': "A key! What does it open? Nothing? Perfect.",
    'KEEN8': "Sand. You brought me sand. ...I love it.",
    'KEEN9': "Is that a brain? It's looking at me.",
    'KEEN95': "An accumulator! It's warm. I'm sitting on it.",
    'AREALM': "A crystal from Krisle! It hums when I sing.",
    'FITF': "That's the note! Who wrote it? ...Don't tell me.",
    'ALPHAMATIC': "A floppy disk! Can I play games on it?",
    'RTTS': "A lantern from the Shadowlands. I'm never going back there.",
    'OSI': "A station pass. I'm keeping it, so they can't come back.",
    'CK55': "A gear! Look, it spins! ...It stopped.",
    'ASYLUM': "A letter with no address? That's the saddest letter.",
    'BOUNTY': "Rick Uller! Wanted no more. You got him!",
    'SUBURB': "Mortimer's head?! Is it... is it off?",
    'AURORA': "Vorta Cola! Who drank it? ...Craig drank it.",
    'OCFLORE': "It glows! Is it supposed to glow? I'm not touching it.",
    'CANMORE': "A keycard! Can it open the fridge?",
    'SAINT': "A flower for Craig. I'll water it every day.",
    'PARTY': "There was a party? And nobody invited Spot?",
}

# (the C array, the clips' prefix, the voice)
TALKS = [('kw_spotTalk', SPOT_TALK, 'OP_SPOT_', SPOT),
         ('kw_janitorTalk', JANITOR_TALK, 'OP_JAN_', JANITOR),
         ('kw_lindseyTalk', LINDSEY_TALK, 'OP_LIN_', LINDSEY)]


def main(folder, header):
    lines = ['/* Generated by kwtools/kw_outpost_voice.py -- do not edit by hand. */',
             '// Talks in the outpost (Spot\'s, the Janitor\'s, Lindsey\'s): at each place they',
             '// say something and Keen picks a line; the answer, then the talk goes on at',
             '// another place (-1: it ends).']
    picks, total = [], 0
    for array, talk, prefix, voice in TALKS:
        names = list(talk)
        clips, files = {}, {}

        def clip(text):
            if text not in files:
                files[text] = '%s%02d.RAW' % (prefix, len(files))
                clips[files[text]] = bar.spoken(text)
            return files[text]

        lines.append('static const KW_TalkNode %s[] = {' % array)
        for name, (says, choices) in talk.items():
            lines.append('\t{ // %d: %s' % (names.index(name), name))
            lines.append('\t\t%s, "%s",' % (bar.c_string(bar.pages(says)), clip(says)))
            lines.append('\t\t{')
            for keen, reply, then in choices:
                assert len(keen) <= bar.KEEN_LINE, keen
                picks.append(keen)
                lines.append('\t\t\t{%s, %s, "%s", %d, -1},' % (
                    bar.c_string(keen), bar.c_string(bar.pages(reply)), clip(reply), names.index(then) if then else -1))
            lines += ['\t\t},', '\t},']
        lines.append('};')
        total += len(clips)
        if folder != '-':
            speak_all(clips, voice, folder, **STYLE.get(voice, {}))
    import kw_outpost
    assert set(KEEPSAKE_TEXT) == set(SPOT_NEW) == set(kw_outpost.KEEPSAKES)
    lines.append('// In shelf order (kw_outpost.py): a card for each keepsake, and what Spot says')
    lines.append('// the first time he sees it, with its clip.')
    lines.append('static const char *const kw_keepsakeText[] = {')
    for world in kw_outpost.KEEPSAKES:
        lines.append('\t%s,' % bar.c_string(bar.pages(KEEPSAKE_TEXT[world])))
    lines += ['};', 'static const char *const kw_spotNewText[][2] = {']
    news = {}
    for world in kw_outpost.KEEPSAKES:
        news['OP_SPNEW_%s.RAW' % world] = bar.spoken(SPOT_NEW[world])
        lines.append('\t{%s, "OP_SPNEW_%s.RAW"},' % (bar.c_string(bar.pages(SPOT_NEW[world])), world))
    lines.append('};')
    if folder != '-':
        speak_all(news, SPOT, folder, **STYLE.get(SPOT, {}))
        speak_all({keen_says_file(p): bar.spoken(p) for p in dict.fromkeys(picks)}, KEEN, folder, **STYLE[KEEN])
        by_voice = {}
        for name, (voice, words) in CLIPS.items():
            by_voice.setdefault(voice, {})[name] = words
        for voice, scene in by_voice.items():
            speak_all(scene, voice, folder, **STYLE.get(voice, {}))
    open(header, 'w').write('\n'.join(lines) + '\n')
    print('%d talk clips, %d scene clips' % (total, len(CLIPS)))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
