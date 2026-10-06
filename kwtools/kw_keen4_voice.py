"""Keen 4 speaks: every line of dialogue in Secret of the Oracle as a voice
clip (Keen Worlds).

    python3 tools/kw_keen4_voice.py omnispeak/data/voice

The words are the game's own (its STRINGS file and its ending script). Each
character has a Kokoro voice of their own; the ship computer (af_heart) and
the barmaid (af_bella) keep theirs to themselves:

  Keen               bm_fable    (and so in every world, from now on); also
                                 every line he picks in a conversation
                                 (KEENSAY_*.RAW)
  Council members    bm_george   all eight, and the elder in the Border Village
  The Oracle         am_onyx     with an echo
  Princess Lindsey   bf_emma
  The janitor        am_eric
  The narrator       bm_lewis    "Meanwhile, back on Earth..."

The council member rescued under water says his line through the water (the
same clip, wobbled and muffled). Clips are K4_*.RAW in the voice folder:
signed 16-bit mono, low byte first, 22050 Hz. The game plays K4_END_<nn>.RAW
with page nn of the ending.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE

KEEN, COUNCIL, ORACLE, LINDSEY, JANITOR, NARRATOR = 'bm_fable', 'bm_george', 'am_onyx', 'bf_emma', 'am_eric', 'bm_lewis'
# How a voice is used, beyond its own sound (speak_all's options). Keen's
# voice says the word "Keen" near "Cain" unless it is pushed towards "kyeen"
# (the user picked this by ear, 2026-10-06: first am_puck sped up and raised,
# then this voice as it is).
STYLE = {
    KEEN: dict(speed=1.2, sounds={'kˈiːn': 'kjˈiːn'}),   # (a fifth faster than the voice talks by itself: asked for 2026-10-06)
}
# The elder's talk in the Border Village: what Keen can say (kw_habitat.c).
ELDER_CHOICES = ["How's the village?", "What's the new inn?", "Goodbye!"]


def keen_says_file(text):
    """The clip of a line Keen picks in a conversation: named after the line
    itself (FNV-1a of its text; KW_KeenSaysClip in kw_habitat.c does the
    same), so the conversations need no list of them."""
    h = 0x811C9DC5
    for b in text.encode('latin1'):
        h = ((h ^ b) * 0x01000193) & 0xFFFFFFFF
    return 'KEENSAY_%08X.RAW' % h


LINES = {
    KEEN: {
        # to the council members, in the order they are rescued
        'K4_KEEN_COUNCIL0.RAW': ["No sweat, oh guardian of wisdom!"],
        'K4_KEEN_COUNCIL1.RAW': ["Sounds like a plan, bearded one!"],
        'K4_KEEN_COUNCIL2.RAW': ["No problemo."],
        'K4_KEEN_COUNCIL3.RAW': ["Great.", "You know, you look a lot like the last guy I rescued..."],
        'K4_KEEN_COUNCIL4.RAW': ["Good idea, Gramps."],
        'K4_KEEN_COUNCIL5.RAW': ["May the road rise to meet your feet, Mister Member."],
        'K4_KEEN_COUNCIL6.RAW': ["Wise plan of action, your ancientness."],
        'K4_KEEN_COUNCIL7.RAW': ["You're the last one, fella.", "Let's both get back to the Oracle chamber!"],
        'K4_KEEN_LINDSEY1.RAW': ["Thanks, your Highness!"],
        'K4_KEEN_LINDSEY2.RAW': ["Thanks for the mysterious clue, Princess!"],
        'K4_KEEN_JANITOR.RAW': ["This had better be a joke."],
        'K4_KEEN_CANTSWIM.RAW': ["I can't swim!"],
        'K4_KEEN_WETSUIT.RAW': ["Cool!", "I can breathe under water now!"],
        # the ending
        'K4_END_00.RAW': ["Wow, this is really an impressive way to activate this monolith of ancient wisdom."],
        'K4_END_02B.RAW': ["Hmmm.", "Maybe it's got a dead battery or something."],
        'K4_END_08.RAW': ["These Shikadi are toast!"],
        'K4_END_09.RAW': ["Thanks for the scoop, Oracle.", "I'm outta here."],
        'K4_END_10.RAW': ["I hope it doesn't rain on my folks...."],
    },
    COUNCIL: {
        'K4_COUNCIL.RAW': ["Oh, thank you so much for rescuing me!", "I'll return to the Oracle chamber immediately."],
        'K4_END_02A.RAW': ["Everyone be silent.", "The Oracle is about to speak."],
        # the elder who has moved into the Border Village (Keen Worlds' own words)
        'K4_ELDER.RAW': ["Welcome back, Keen!", "The Border Village is safe again, thanks to you."],
        'K4_ELDER_0.RAW': ["Peaceful at last.", "The villagers are back in their homes."],
        'K4_ELDER_1.RAW': ["The barmaid opened it once the slugs left.", "Say hello!"],
        'K4_ELDER_2.RAW': ["Farewell, Keen.", "May the Oracle guide you."],
    },
    ORACLE: {
        'K4_END_04.RAW': ["The Shikadi are a race of shadow beings from the far side of the galaxy."],
        'K4_END_05.RAW': ["They are building an Armageddon Machine, and are planning to blow up the galaxy with it,",
                          "then rebuild it to suit their needs."],
        'K4_END_06.RAW': ["They live on the third planet of the Korath system, which is seventy-one thousand light-years from Gnosticus Four.",
                          "The Armageddon Machine is almost complete."],
        'K4_END_07.RAW': ["And this is what they look like."],
    },
    LINDSEY: {
        'K4_LINDSEY1.RAW': ["There's gear to help you swim in Three-Tooth Lake.", "It is hidden in Miragia."],
        'K4_LINDSEY2.RAW': ["The way to the Pyramid of the Forbidden lies under the Pyramid of Moons."],
    },
    JANITOR: {
        'K4_JANITOR1.RAW': ["Thanks for going to all that trouble, but I'm just the janitor for the High Council."],
        'K4_JANITOR2.RAW': ["I tried to tell the Shikadi that, but they just wouldn't listen..."],
        'K4_JANITOR4.RAW': ["Sorry.", "You aren't mad, are you?"],
    },
    NARRATOR: {
        'K4_END_11.RAW': ["Meanwhile, back on Earth..."],
    },
}


def effect(folder, src, dst, filters):
    raw = ['-f', 's16le', '-ar', str(RATE), '-ac', '1']
    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', os.path.join(folder, src), '-af', filters] +
                   raw + [os.path.join(folder, dst + '.tmp')], check=True)
    os.replace(os.path.join(folder, dst + '.tmp'), os.path.join(folder, dst))


def main(folder):
    os.makedirs(folder, exist_ok=True)
    for voice, clips in LINES.items():
        speak_all(clips, voice, folder, **STYLE.get(voice, {}))
    # what Keen says in the conversations with the elder and the barmaid
    import kw_barmaid_voice as bar
    picks = list(ELDER_CHOICES)
    for says, choices in bar.TALK.values():
        picks += [keen for keen, reply, then in choices]
    picks = list(dict.fromkeys(picks))
    speak_all({keen_says_file(p): bar.spoken(p) for p in picks}, KEEN, folder, **STYLE[KEEN])
    # the Oracle speaks in a great hall
    for name in LINES[ORACLE]:
        effect(folder, name, name, 'aecho=0.8:0.6:110|190:0.35|0.2,alimiter=limit=0.95')
    # the council member under water
    effect(folder, 'K4_COUNCIL.RAW', 'K4_COUNCIL_WET.RAW',
           'vibrato=f=8:d=0.7,tremolo=f=13:d=0.8,lowpass=f=1500,volume=1.6,alimiter=limit=0.95')
    # the ending's third page: the council member, then (when the Oracle has
    # stayed silent a while) Keen
    a = open(os.path.join(folder, 'K4_END_02A.RAW'), 'rb').read()
    b = open(os.path.join(folder, 'K4_END_02B.RAW'), 'rb').read()
    gap = max(0.5, 5.6 - len(a) / 2 / RATE)
    open(os.path.join(folder, 'K4_END_02.RAW'), 'wb').write(a + bytes(2 * int(gap * RATE)) + b)
    os.remove(os.path.join(folder, 'K4_END_02A.RAW'))
    os.remove(os.path.join(folder, 'K4_END_02B.RAW'))
    total = sum(os.path.getsize(os.path.join(folder, f)) for f in os.listdir(folder) if f.startswith('K4_'))
    print('%d clips, %.1f MB' % (len([f for f in os.listdir(folder) if f.startswith('K4_')]), total / 1e6))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
