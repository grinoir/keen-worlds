"""Gnosticus Pi speaks: the dialogue windows of Foray in the Forest (Nisaba
and K1n9_Duk3's game) as voice clips (Keen Worlds).

    python3 tools/kw_fitf_voice.py foray/src_v20/SOURCE play/mods/fitf omnispeak/data/voice

The words are the game's own: its strings (GAMESTR.CKS, named by GAMESTR.H).
Every dialogue window of the game shows one string beside a portrait
(CK_DialogWindow), so a clip is named after its string's number,
FF_S<nnn>.RAW, and the engine plays the clip of whatever string a window
shows. Who says a string is told by its name:

  S_JANITORMESSAGE, -JOKE, -MONOLOG, ...   the janitor            am_eric
  S_JANITORREPLY, S_LINDSEYREPLY,
  S_HALMESSAGE, S_MISCMESSAGE              Keen                   (his voice)
  S_LINDSEYMESSAGE                         Princess Lindsey       bf_emma
  S_HALREPLY                               HAL                    am_onyx, slowly
  S_SPACEMANSPIFFMESSAGE                   Spaceman Spiff         af_river
  S_TARDISMESSAGE, S_BONUSLEVELINFO        the narrator

Some lines are not plain speech in the game either, and are made the same
way here: the janitor's two lines "under water" are wobbled and muffled,
Keen's line with his mouth full is muffled, and the line Keen says backwards
(the game prints it mirrored) is spoken and then turned round.

Not spoken: the endings (they are laid out word by word with colours and
alternatives, not as pages of speech), HAL's line of code letters, the
radio's beeps, Spaceman Spiff's two lists, and the jokes of the "continue?"
screen.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, re, struct, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN, NARRATOR, LINDSEY, JANITOR

HAL, SPIFF = 'am_onyx', 'af_river'
STYLE = dict(BASE_STYLE)
STYLE[HAL] = dict(speed=0.88)

WHO = [   # string name (without its number) -> voice
    (r'S_JANITOR(MESSAGE|JOKE|MONOLOG|STUNNEDMSG|NISASIMESSAGE)$', JANITOR),
    (r'S_(JANITORREPLY|JANITORNISASIREPLY|LINDSEYREPLY|HALMESSAGE|MISCMESSAGE)$', KEEN),
    (r'S_LINDSEYMESSAGE$', LINDSEY),
    (r'S_HALREPLY$', HAL),
    (r'S_SPACEMANSPIFFMESSAGE$', SPIFF),
    (r'S_(TARDISMESSAGE|BONUSLEVELINFO)$', NARRATOR),
]
SILENT = {'S_HALREPLY1', 'S_SPACEMANSPIFFMESSAGE4', 'S_SPACEMANSPIFFMESSAGE5'}
# Lines that are not written the way they are meant to sound
SAY = {
    'S_JANITORMESSAGE8': "Oh, thank you so much for rescuing me! I'll return to the Oracle chamber immediately. Blub.",
    'S_JANITORJOKE4': "Dopefish dreams. Blub.",
    'S_JANITORNISASIREPLY0': "Seems like a higher consciousness scrabbles about the convolutions of my brain.",
    'S_LINDSEYREPLY2': "Mmmf, mmmph mmf. Mmmph, mmm, mmmph. Blah, mmmpf...",
    'S_MISCMESSAGE44': "B-b-bounder... b-balloon... b-blower... foamy... puffer... I wonder if this mutation is reversible?!",
    'S_SPACEMANSPIFFMESSAGE1': "It's a magical world, Billy, ol' buddy... let's go exploring!",
    'S_JANITORJOKE10': "A, B, C. One, two, three. You, belong, to, me!",
}
WET = 'vibrato=f=8:d=0.7,tremolo=f=13:d=0.8,lowpass=f=1500,volume=1.6,alimiter=limit=0.95'
EFFECT = {
    'S_JANITORMESSAGE8': WET, 'S_JANITORJOKE4': WET,
    'S_JANITORNISASIREPLY0': 'areverse',
    'S_LINDSEYREPLY2': 'lowpass=f=900,volume=1.5,alimiter=limit=0.95',
}
REPLACE = [('*sigh*', 'Sigh.'), ('*', ''), ('Gnosticus #', 'Gnosticus Pi'), ('BwB', 'Bean-with-Bacon'), ('&', 'and'),
           ('@', 'at'), ('IQ', 'I Q'), ('"', ''), ('Mr. Em', 'Mister M'), ('coop', 'co-op')]


def spoken(text):
    for a, b in REPLACE:
        text = text.replace(a, b)
    text = re.sub(r'\s+', ' ', text.replace('\n', ' ')).strip()
    parts = [s.strip() for s in re.split(r'(?<=[.!?])\s+(?=[A-Z"\'])', text) if s.strip()]
    return [p for p in parts if re.search(r'[A-Za-z]', p)]


def main(src, game, folder):
    names = re.findall(r'^\s*(S_\w+),', open(os.path.join(src, 'GAMESTR.H'), encoding='latin1').read(), re.M)
    data = open(os.path.join(game, 'GAMESTR.CKS'), 'rb').read()
    offs = struct.unpack_from('<%dH' % (len(names) + 1), data, 0)
    by_voice, effects = {}, {}
    for i, name in enumerate(names):
        if name in SILENT:
            continue
        base = re.sub(r'\d+$', '', name)
        voice = next((v for pat, v in WHO if re.match(pat, base)), None)
        if not voice:
            continue
        text = SAY.get(name) or data[offs[i]:offs[i + 1]].split(b'\0')[0].decode('latin1')
        words = spoken(text)
        if not words:
            continue
        clip = 'FF_S%03d.RAW' % i
        by_voice.setdefault(voice, {})[clip] = words
        if name in EFFECT:
            effects[clip] = EFFECT[name]
    os.makedirs(folder, exist_ok=True)
    raw = ['-f', 's16le', '-ar', str(RATE), '-ac', '1']
    for voice, clips in by_voice.items():
        speak_all(clips, voice, folder, **STYLE.get(voice, {}))
    for clip, fx in effects.items():
        path = os.path.join(folder, clip)
        subprocess.run(['ffmpeg', '-v', 'error', '-y'] + raw + ['-i', path, '-af', fx] + raw + [path + '.fx'], check=True)
        os.replace(path + '.fx', path)
    mine = [f for f in os.listdir(folder) if f.startswith('FF_S')]
    print('%d clips, %.1f MB (%s)' % (len(mine), sum(os.path.getsize(os.path.join(folder, f)) for f in mine) / 1e6,
                                      ', '.join('%s %d' % (v, len(c)) for v, c in by_voice.items())))


if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
