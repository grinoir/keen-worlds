"""Gnosticus Pi's endings speak: the ending texts of Foray in the Forest as
voice clips (Keen Worlds).

    python3 tools/kw_fitf_end_voice.py play/mods/fitf omnispeak/data/voice [list]

Its endings are text chunks in its own layout language (see ck_text.c): what
a page says depends on conditions (what Keen collected, which secret levels
he found), so a page is spoken by its stretches of text: from a page mark
(^P) or a condition (^I) to the next. The engine notes where the first word
of each stretch it shows lies in the chunk, and plays FF_<ENDING>_<nnnnn>.RAW,
nnnnn being that place; this tool walks the chunk the same way to find the
stretches and their places.

Who speaks is told by the colour the text is in:
  F (white)    Keen
  E (yellow)   the narrator (also the colour a page begins in)
  B (cyan)     the janitors, or Princess Lindsey where her portrait is shown
  A, D         the narrator, where it is prose (lists of things found, in
               square brackets, are not read)
Other colours are effects (one word in many colours, a line said with the
mouth full) and are left silent, as is the "lose" ending, which is one
sentence shown a word at a time.

"list" as a third argument prints the stretches instead of speaking them.

Needs what tools/kw_landing_voice.py needs (Node, kokoro-js in
KW_KOKORO_DIR, ffmpeg).
"""
import os, re, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_fitf import text_chunk
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE, KEEN, NARRATOR, JANITOR, LINDSEY

ENDINGS = {'ENDWIN': 7766, 'ENDALL': 7767, 'ENDCHEAT': 7769, 'ENDMIN': 7770, 'ENDTRICK': 7771}
VOICE = {'F': KEEN, 'E': NARRATOR, 'B': JANITOR, 'A': NARRATOR, 'D': NARRATOR, 'Lindsey': LINDSEY}
JANITOR_PICS, LINDSEY_PIC = (42, 43), 44   # the portraits beside the cyan text
GAP = 0.25
REPLACE = [('"', ''), ('*', ''), ('#', 'Pi'), ('&', 'and'), ('BwB', 'Bean-with-Bacon'), ('no.', 'number'),
           ('IQ', 'I Q'), ('F10+G', 'F ten G'), ('_', ' ')]


def stretches(t):
    """[(place of the first word, [(colour, text), ...]), ...] of a chunk."""
    out, i, pending, colour, cur, portrait = [], 0, True, 'E', None, 0

    def eol(k):
        k = t.find('\n', k)
        return len(t) if k < 0 else k + 1
    while i < len(t):
        ch = t[i]
        if ch == '^' and i + 1 < len(t):
            c = t[i + 1].upper()
            if c == 'E':
                break
            if c == 'P':
                pending, colour, cur = True, 'E', None
                i = eol(i)
            elif c == 'I':
                pending, cur = True, None
                i = eol(i)
            elif c == 'C':
                colour = t[i + 2].upper()
                i += 3
            elif c == 'G':
                nums = re.findall(r'\d+', t[i + 2:eol(i)].split('#')[0])
                if len(nums) >= 3 and int(nums[2]) in JANITOR_PICS + (LINDSEY_PIC,):
                    portrait = int(nums[2])
                i = eol(i)
            elif c in 'LBTD;MSWQ':
                i = eol(i)
            elif c == 'F':
                i += 2
                while i < len(t) and (t[i].isdigit() or t[i] in ' ,'):
                    i += 1
            else:
                i += 2
        elif ch <= ' ':
            i += 1
        else:
            j = i
            while j < len(t) and t[j] > ' ':
                j += 1
            if pending:
                cur = []
                out.append((i, cur))
                pending = False
            if cur is not None:
                # (cyan is the janitors', or Lindsey's when hers is the portrait)
                who = 'Lindsey' if colour == 'B' and portrait == LINDSEY_PIC else colour
                if cur and cur[-1][0] == who:
                    cur[-1] = (who, cur[-1][1] + ' ' + t[i:j])
                else:
                    cur.append((who, t[i:j]))
            i = j
    return out


def spoken(text):
    for a, b in REPLACE:
        text = text.replace(a, b)
    text = re.sub(r'\s+', ' ', text).strip()
    parts = [s.strip() for s in re.split(r'(?<=[.!?])\s+(?=[A-Z])', text) if s.strip()]
    return [p for p in parts if re.search(r'[A-Za-z]{2}', p)]


def turns(runs):
    out = []
    for colour, text in runs:
        voice = VOICE.get(colour)
        if not voice or text.lstrip().startswith('['):
            continue
        # (the letters of a word shown one by one, rows of signs: not speech)
        if len(re.findall(r'[A-Za-z]{3,}', text)) < 1:
            continue
        words = spoken(text)
        if not words:
            continue
        if out and out[-1][0] == voice:
            out[-1] = (voice, out[-1][1] + words)
        else:
            out.append((voice, words))
    return out


def main(game, folder, listing=False):
    clips = {}
    for tag, chunk in ENDINGS.items():
        for place, runs in stretches(text_chunk(game, chunk)):
            tt = turns(runs)
            if tt:
                clips['FF_%s_%05d.RAW' % (tag, place)] = tt
    if listing:
        for name, tt in clips.items():
            print(name)
            for voice, words in tt:
                print('   %-9s %s' % (voice, ' '.join(words)[:150]))
        print(len(clips), 'clips,', sum(len(' '.join(w)) for tt in clips.values() for _, w in tt), 'letters')
        return
    os.makedirs(folder, exist_ok=True)
    for old in os.listdir(folder):
        if re.match(r'FF_END[A-Z]+_\d{5}\.RAW$', old):
            os.remove(os.path.join(folder, old))
    with tempfile.TemporaryDirectory() as tmp:
        by_voice, parts = {}, {}
        for name, tt in clips.items():
            for k, (voice, words) in enumerate(tt):
                seg = 'SEG_%s_%d.RAW' % (name[:-4], k)
                by_voice.setdefault(voice, {})[seg] = words
                parts.setdefault(name, []).append(seg)
        for voice, segs in by_voice.items():
            speak_all(segs, voice, tmp, **STYLE.get(voice, {}))
        for name, segs in parts.items():
            out = bytearray()
            for k, seg in enumerate(segs):
                if k:
                    out += bytes(2 * int(GAP * RATE))
                out += open(os.path.join(tmp, seg), 'rb').read()
            open(os.path.join(folder, name), 'wb').write(out)
    mine = [f for f in os.listdir(folder) if re.match(r'FF_END[A-Z]+_\d{5}\.RAW$', f)]
    print('%d clips, %.1f MB' % (len(mine), sum(os.path.getsize(os.path.join(folder, f)) for f in mine) / 1e6))


if __name__ == '__main__':
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2], sys.argv[3:] == ['list'])
