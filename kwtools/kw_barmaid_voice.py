"""What the barmaid of the Border Village tavern has to say (Keen Worlds).

    python3 tools/kw_barmaid_voice.py omnispeak/data/voice omnispeak/src/kw_barmaid_talk.h
    python3 tools/kw_barmaid_voice.py - omnispeak/src/kw_barmaid_talk.h    # the text only

Her whole conversation is written here, once: she knows the worlds, who
made each of them, and GriNoir the great Linker whose portrait hangs on her
wall. Every line of hers is also spoken (Kokoro's "Bella", a different
woman from the ship computer; see tools/kw_landing_voice.py for how the
clips are made). This writes the clips (BAR_nn.RAW) and the conversation as
C data for the engine.

The conversation is a set of places ("nodes"). At each she says something
and Keen picks one of up to four things to say; she answers, and the talk
goes on at another place or ends. A line is given as it is written on the
card; where it must be said differently (names), SAID has the spelling for
the voice.
"""
import os, sys, textwrap

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from kw_landing_voice import speak_all

VOICE = 'af_bella'
LINE = 19         # characters in a line of her card (beside her picture)
LINES = 4         # lines on a card; longer answers go on over more cards
KEEN_LINE = 26    # characters in one of Keen's lines
END = None

# node: (what she says, [(what Keen says, her answer, where the talk goes on)])
TALK = {
    'hello': ("Well, if it isn't the hero of the Shadowlands! What can I get you?", [
        ("A root beer, please.", "Coming right up! The first one's on the house, hon.", 'more'),
        ("Who's in the picture?", None, 'picture'),
        ("About the worlds...", None, 'worlds'),
        ("Goodbye!", "Fly safe, Keen. And mind the slugs on your way out.", END),
    ]),
    'more': ("Anything else, hon?", [
        ("Another root beer!", "That one goes on your tab. Heroes pay too, you know.", 'more'),
        ("Who's in the picture?", None, 'picture'),
        ("About the worlds...", None, 'worlds'),
        ("Goodbye!", "Fly safe, Keen. And mind the slugs on your way out.", END),
    ]),
    'picture': ("That's GriNoir, the great Linker. He joined the worlds into one galaxy.", [
        ("What did he link?", "Every world you can fly to. Before him each one sat alone in the dark. "
                              "He strung them together like lanterns.", 'picture2'),
        ("He made the worlds?", "Oh no, hon, and he'd be the first to say so. Every world has its own makers. "
                                    "He only built the roads between.", 'picture2'),
        ("Why is he up there?", "He paid for the frame. And he tips well.", 'picture2'),
        ("Something else.", None, 'more'),
    ]),
    'picture2': ("Handsome frame, isn't it?", [
        ("What did he link?", "Every world you can fly to. Before him each one sat alone in the dark. "
                              "He strung them together like lanterns.", 'picture2'),
        ("He made the worlds?", "Oh no, hon, and he'd be the first to say so. Every world has its own makers. "
                                    "He only built the roads between.", 'picture2'),
        ("Why is he up there?", "He paid for the frame. And he tips well.", 'picture2'),
        ("Something else.", None, 'more'),
    ]),
    'worlds': ("You hear things, behind a bar. Which worlds do you want to know about?", [
        ("The old ones.", None, 'old'),
        ("Universe is Toast?", None, 'toast'),
        ("The far ones.", None, 'far'),
        ("Something else.", None, 'more'),
    ]),
    'old': ("Gnosticus, the Omegamatic, Fribbulus Xax. The classics! id Software made those, back in 1991.", [
        ("Who exactly?", "Tom Hall dreamed you up, hon. John Carmack and John Romero built it all, "
                         "Adrian Carmack painted it, and Bobby Prince wrote the tunes.", 'old'),
        ("Which is the best?", "This one, of course. We have root beer.", 'old'),
        ("Other worlds.", None, 'worlds'),
        ("Something else.", None, 'more'),
    ]),
    'toast': ("Krodacia, Calidune, and the HUB Command Center. Three places, one long story.", [
        ("Who made them?", "A fan called Ceilick began it, all in his own time. By the third he had Mink and Tulip "
                           "beside him, and half the fans of the galaxy helping.", 'toast'),
        ("Are they dangerous?", "One of them is a desert that wants you dead, hon. Pack a hat.", 'toast'),
        ("And after that?", "Aspermontis. You took your friends home and found the Oracle scorched and its "
                            "batteries gone. Bubbatom made that one, with Ceilick, Mink and Gridlock lending a hand.", 'toast'),
        ("Something else.", None, 'more'),
    ]),
    'far': ("Krisle is where the Atroxians live. Gnosticus Pi is our little sister planet, "
            "the one with the rings. And the Shikadi keep a station over Fribbulus Xax.", [
        ("Who made Krisle?", "Gridlock. Atroxian Realm, he called it. Years of work, with friends like Lemm and "
                             "Gamebird. Mind you, your ship breaks when you land there.", 'far'),
        ("And Gnosticus Pi?", "Nisaba and K1n9_Duk3, with a whole crowd of friends. Foray in the Forest. "
                            "They even drew the inside of your ship, hon.", 'far'),
        ("The Shikadi Station?", "That one hangs over Fribbulus Xax. Ceilick again, with Multimania doing the wiring. "
                                 "Operation Station Infiltration. Mind the big Ampton.", 'far'),
        ("Something else.", None, 'more'),
    ]),
    # ---- When every world is saved (KW_Finale in kw_world.c): Keen is put at
    # her bar and the talk begins here, at 'brag1'. Each of his lines is a
    # boast about one world (the fourth thing in its row: the world's id) and
    # is only offered if that world is installed. Whatever he picks, the talk
    # moves on; a round with nothing to offer is passed over.
    'brag1': ("Look who's back! They say you've been everywhere, hon. Go on, then. Impress me.", [
        ("I saved the Oracle!", "Eight old men out of the Shadowlands. I know, hon. Half of them drink here now.", 'brag2', 'keen4'),
        ("I blew the fuses!", "On the Omegamatic? A doomsday machine, and all it took was a few fuses.", 'brag2', 'keen5'),
        ("I rescued Molly!", "From the Bloogs! And she was the one meant to be minding you.", 'brag2', 'keen6'),
        ("I found the keys.", "Krodacia's keys? All of them? I can't find my own on a good day.", 'brag2', 'keen7'),
    ]),
    'brag2': ("Not bad. What else?", [
        ("I beat the desert.", "Calidune. I told you to pack a hat.", 'brag3', 'keen8'),
        ("I won on brains.", "At the HUB? Against the cleverest heads in the Universe, and you turned up with a pogo stick.", 'brag3', 'keen9'),
        ("I brought batteries.", "The eight accumulators! The Oracle hums again. It keeps the neighbours up.", 'brag3', 'keen95'),
    ]),
    'brag3': ("Go on, I'm listening.", [
        ("I got off Krisle.", "With a broken ship! The Atroxians still talk about it. Not kindly.", 'brag4', 'arealm'),
        ("I tidied the forest.", "On Gnosticus Pi, our little sister planet. They say it is quiet again.", 'brag4', 'fitf'),
        ("I took a station.", "The Shikadi one, over Fribbulus Xax? Mind the big Ampton, I said. Did you mind it?", 'brag4', 'osi'),
    ]),
    'brag4': ("Is that all?", [
        ("I climbed Meurglys.", "And met the local Elvis, I hear. Did he sing?", 'brag5', 'bounty'),
        ("I broke another one.", "A second Omegamatic? Some people never learn, hon.", 'brag5', 'ck55'),
        ("I found the address.", "Address Unknown? Nobody finds that place. That's the point of the name.", 'brag5', 'asylum'),
    ]),
    'brag5': ("Every world, saved. I'll say this once, Keen: well done. Sit down. This one's on the house. They all are, from now on.", [
        ("Thanks. A root beer!", "Coming right up, hero.", END),
        ("It was nothing.", "It was a whole galaxy, hon. Drink up.", END),
    ]),
}
START = 'hello'
# The world a boast belongs to, as the engine names it.
WORLD = {'keen4': 'KW_KEEN4', 'keen5': 'KW_KEEN5', 'keen6': 'KW_KEEN6', 'keen7': 'KW_KEEN7', 'keen8': 'KW_KEEN8',
         'keen9': 'KW_KEEN9', 'keen95': 'KW_KEEN95', 'arealm': 'KW_AREALM', 'fitf': 'KW_FITF', 'osi': 'KW_OSI',
         'bounty': 'KW_BOUNTY', 'ck55': 'KW_CK55', 'asylum': 'KW_ASYLUM'}
# How names are spelled for the voice.
SAID = {'Aspermontis': 'Asper-montis', 'Bubbatom': 'Bubba Tom', 'Shikadi': 'Shick-ah-dee', 'Multimania': 'Multi-mania', 'GriNoir': 'Grih noir', 'K1n9_Duk3': 'King Duke', 'id Software': 'I D Software', 'Ceilick': 'Sy lick',
        'Omegamatic II': 'Omegamatic Two', 'Krisle': 'Krizzle', 'Atroxians': 'A-troxians', 'Fribbulus Xax': 'Fribbulus Zax', 'hon': 'hun'}


def pages(text):
    """Her words as cards: lines of at most LINE characters, LINES to a card."""
    lines = textwrap.wrap(text, LINE)
    return '\f'.join('\n'.join(lines[i:i + LINES]) for i in range(0, len(lines), LINES))


def spoken(text):
    for written, said in SAID.items():
        text = text.replace(written, said)
    return [s.strip() for s in text.replace('! ', '!|').replace('? ', '?|').replace('. ', '.|').split('|') if s.strip()]


def c_string(text):
    return '"%s"' % text.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n').replace('\f', '\\f')


def main(folder, header):
    names = list(TALK)
    assert names[0] == START
    clips, files = {}, {}

    def clip(text):
        if text not in files:
            files[text] = 'BAR_%02d.RAW' % len(files)
            clips[files[text]] = spoken(text)
        return files[text]

    lines = ['/* Generated by tools/kw_barmaid_voice.py -- do not edit by hand. */',
             '// The barmaid\'s conversation: at each place she says something (text for the',
             '// cards, pages parted by \\f, and its voice clip) and Keen picks a line; she',
             '// answers (or not) and the talk goes on at another place (-1: it ends).',
             'static const KW_TalkNode kw_barmaidTalk[] = {']
    for name, (says, choices) in TALK.items():
        assert len(choices) <= 4
        lines.append('\t{ // %d: %s' % (names.index(name), name))
        lines.append('\t\t%s, "%s",' % (c_string(pages(says)), clip(says)))
        lines.append('\t\t{')
        for keen, reply, then, *world in choices:
            assert len(keen) <= KEEN_LINE, keen
            lines.append('\t\t\t{%s, %s, %s, %d, %s},' % (
                c_string(keen), c_string(pages(reply)) if reply else 'NULL',
                '"%s"' % clip(reply) if reply else 'NULL', names.index(then) if then else -1,
                WORLD[world[0]] if world else '-1'))
        lines += ['\t\t},', '\t},']
    lines.append('};')
    lines.append('#define KW_BARMAID_BRAG %d // where the talk begins when every world is saved' % names.index('brag1'))
    if folder != '-':     # ('-': only the text, when no spoken words have changed)
        speak_all(clips, VOICE, folder)
    open(header, 'w').write('\n'.join(lines) + '\n')
    print('%d clips, %d places' % (len(clips), len(names)))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
