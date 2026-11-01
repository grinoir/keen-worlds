"""The endings of Dave216's games (and Operation Ocflore's) are spoken
(Keen Worlds).

    python3 kwtools/kw_dave216_end_voice.py play/mods omnispeak/data/voice

Their ending texts are read from the player's game files (kw_end_text.py)
and spoken page by page: <PREFIX>_END_<page>.RAW, and _ENDS_ for Canmore's
Redemption's body-armour ending (its secret ending, ck_text.c). Dave216
prints each speaker in a colour of his own; ROLES gives each colour's voice
per game. The narrator reads the rest. Operation Ocflore's ending has no
colours: its lines are given here, in its own words. Keen, the narrator and
the friends keep their voices; Craig, Myah and the Dorsal have the
provisional ones of their worlds; the radio call at the end of Aurora Bliss
is the Krile advisor of Krisle (Atroxian Realm, whose planet is invaded) on a
crackling radio.

Not spoken: "- says Keen" after a line (the voice says who it is), stage
directions in asterisks are read as words, the pages that tell the
passwords of Canmore's archives (Keen Worlds opens their worlds itself).

Needs what kw_landing_voice.py needs.
"""
import os, re, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K, NARRATOR as N, LINDSEY, JANITOR
from kw_keen789_voice import MORTIMER, BARKER
from kw_aurora_voice import CRAIG, STYLE as AURORA_STYLE
from kw_canmore_voice import MYAH
from kw_ocflore_voice import DORSAL, STYLE as OCFLORE_STYLE
from kw_end_text import pages

KRILE = 'af_kore'
ALLAIN = 'am_santa'   # PROVISIONAL (samples in art/voices/allain_*)
RADIO = 'radio'   # (a role, not a voice: the Krile on the radio)
VOICES = {RADIO: KRILE}
STYLE = dict(BASE_STYLE)
STYLE[CRAIG] = AURORA_STYLE[CRAIG]
STYLE[DORSAL] = OCFLORE_STYLE[DORSAL]
STYLE[ALLAIN] = dict(pitch=0.8)
EFFECT = {RADIO: 'highpass=f=500,lowpass=f=2800,acrusher=bits=8:mix=0.3,tremolo=f=9:d=0.5,volume=1.4'}
GAP = 0.35
SAID = {'BwB': 'Bean-with-Bacon', 'Navi-Com': 'Navi-Com', 'finnaly': 'finally', 'Vexidar VI': 'Vexidar Six',
        'Xantis VIII': 'Xantis Eight', '314': 'three-fourteen', "I've think": 'I think',
        'RADIO STARTS STUTTERING': 'The radio starts stuttering.', 'riffle': 'rifle', 'tensity': 'tension',
        'freighted': 'frightened', 'Morty, Morty Morty': 'Morty, Morty, Morty', 'with\xa0you': 'with you', '- shouts out Keen as he sees': 'Keen sees', '-says Craig as he throws': 'Craig throws'}

# game id: (folder, chunk, dictionary (None: Keen 5's in Omnispeak's data), ext, prefix, secret chunk, roles)
GAMES = {
    'aurora': ('aurora', 4743, 'aurora', 'CK4', 'AB', None, {'B': K, 'D': CRAIG, 'E': N, 'F': RADIO}),
    'canmore': ('canmore', 4918, None, 'CK5', 'CR', 4919, {'A': N, '2': MORTIMER, 'F': K, '9': CRAIG}),
    'saint': ('saint', 4743, 'aurora', 'CK4', 'SA', None, {'E': N}),
    'party': ('party', 4743, 'aurora', 'CK4', 'PA', None,
              {'D': CRAIG, 'B': K, 'E': N, '2': MYAH, 'A': BARKER, 'F': JANITOR, '3': LINDSEY}),
    'revival': ('revival', 4743, 'revival', 'CK4', 'RV', None,
                {'E': N, 'B': K, 'D': CRAIG, 'C': ALLAIN, '2': MYAH, 'A': BARKER, '6': MORTIMER, '3': LINDSEY}),
    'revivalc': ('revivalc', 4743, 'revivalc', 'CK4', 'RC', None,
                 {'A': N, 'F': K, '9': CRAIG, '8': ALLAIN, '6': MYAH, 'E': BARKER, '2': MORTIMER, '7': LINDSEY}),
}
# The colours of pages not spoken: the passwords of Canmore's archives, the
# riddle at the end of Revival of Bliss (Keen Worlds opens what they open)
SKIP = {'canmore': {'C3'}, 'revival': {'C7'}, 'revivalc': {'C3'}}
D = DORSAL
OCFLORE = {
    'OC_END_00.RAW': [(N, ["With the three force-field relics in hand, Commander Keen makes his way to the heart of the "
                           "Dorsal Obelisk.", "Deep below the Obelisk surface, Keen is met by a trio of Dorsal."])],
    'OC_END_01.RAW': [(D, ["Greetings Commander Keen, please place the relics above this crystal, we have no time to lose!"]),
                      (K, ["Don't sweat it guys."]),
                      (D, ["It's been many centuries since this device was last operated. I sure hope it still works."])],
    'OC_END_02.RAW': [(D, ["Grab onto something stable while I turn this thing on. Things could get violent!"])],
    'OC_END_09.RAW': [(K, ["Holy smokes! Are you sure this force-field hasn't caused any excess damage to Ocflore?",
                           "That was insane!"]),
                      (D, ["No, this device has only targeted the Ormch and their associated technologies.",
                           "Ocflore's geology and ecosystems will have remained intact."])],
    'OC_END_10.RAW': [(K, ["So much for the almighty Ormch. These guys didn't stand a chance!"]),
                      (D, ["Don't be so sure of that, the Ormch will not have taken our actions lightly.",
                           "They will likely be after you Commander, so we might as well attack them head-on.",
                           "We shall travel to the homeworld of the Ormch, Xantis Eight, to put an end to any future "
                           "plans for the Ocflore Project."])],
    'OC_END_11.RAW': [(K, ["As much as I'd like to join you, my dinner is probably cold, and I could use some rest."]),
                      (D, ["Sorry Commander, I'm afraid relaxation will have to wait...",
                           "and besides, we are already on our way to Xantis Eight."]),
                      (K, ["What do you mean, already on our way?"])],
    'OC_END_14.RAW': [(N, ["To be continued..."])],
}
ATTRIBUTION = re.compile(r'^"?\s*-\s*(says|asks|murmurs|yells|groans|shouts)\b[^.!?]{0,40}[.!?]?$', re.I)


def clean(text):
    for a, b in SAID.items():
        text = text.replace(a, b)
    text = text.replace('*', '').replace('"', ' ').replace("''", '')
    text = re.sub(r'\s*-\s*(says|murmurs|yells|groans)\s+[A-Z][\w. ]*$', '', text)
    text = re.sub(r'\s+', ' ', text).strip()
    return text


def sentences(text):
    return [s for s in re.split(r'(?<=[.!?])\s+', text) if s.strip(' .')]


def clips_of(mods, game):
    folder, number, dictf, ext, prefix, secret, roles = GAMES[game]
    path = os.path.join(mods, folder)
    egadict = os.path.join(mods, dictf, 'EGADICT.' + ext) if dictf else os.path.join(
        os.path.dirname(os.path.abspath(__file__)), '..', 'data', 'keen5', 'EGADICT.CK5')
    clips = {}
    for kind, chunk in (('END', number), ('ENDS', secret)):
        if chunk is None:
            continue
        for i, page in enumerate(pages(path, chunk, egadict, ext)):
            turns = []
            for colour, text in page:
                code = (colour or '')[1:].upper()
                if colour in SKIP.get(game, ()) or ATTRIBUTION.match(text) or text.strip().upper() == 'THE END':
                    continue   # (the passwords' page; "- says Keen"; the words THE END)
                voice = roles.get(code, N) if colour else N
                words = sentences(clean(text))
                if not words:
                    continue
                if turns and turns[-1][0] == voice:
                    turns[-1][1].extend(words)
                else:
                    turns.append((voice, words))
            if turns:
                clips['%s_%s_%02d.RAW' % (prefix, kind, i)] = turns
    return clips


def speak(clips, folder):
    with tempfile.TemporaryDirectory() as tmp:
        by_voice, parts = {}, {}
        for name, turns in clips.items():
            for i, (role, words) in enumerate(turns):
                seg = 'SEG_%s_%d.RAW' % (name[:-4], i)
                by_voice.setdefault(role, {})[seg] = words
                parts.setdefault(name, []).append((role, seg))
        for role, segs in by_voice.items():
            voice = VOICES.get(role, role)
            speak_all(segs, voice, tmp, **STYLE.get(voice, {}))
        raw = ['-f', 's16le', '-ar', str(RATE), '-ac', '1']
        for name, segs in parts.items():
            out = bytearray()
            for k, (role, seg) in enumerate(segs):
                path = os.path.join(tmp, seg)
                if role in EFFECT:
                    subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', path, '-af', EFFECT[role]] + raw + [path + '.fx'],
                                   check=True)
                    path += '.fx'
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(path, 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
            print('%-18s %5.1f s' % (name, len(out) / 2 / RATE))


def main(mods, folder, only=()):
    os.makedirs(folder, exist_ok=True)
    clips = {}
    for game in GAMES:
        if not only or game in only:
            clips.update(clips_of(mods, game))
    if not only or 'ocflore' in only:
        clips.update(OCFLORE)
    if os.environ.get('KW_VOICE_DRY'):
        for name, turns in clips.items():
            print(name)
            for voice, words in turns:
                print('   %-12s %s' % (VOICES.get(voice, voice), ' '.join(words)))
        return
    speak(clips, folder)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2], sys.argv[3:])
