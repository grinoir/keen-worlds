"""Send in the Clorts speaks (Keen Worlds): its one in-game window and its
ending, in the game's own words.

    python3 kwtools/kw_clorts_voice.py play/mods/clorts omnispeak/data/keen5 omnispeak/data/voice

Then kwtools/kw_voice_pack.py packs them to OGG. Clips carry the world's
prefix SC_ (see kw_music.c). In the levels, Keen speaks the machine window,
rewritten as his cloaking-device quip (SC_KEEN_FUSE). The ending (chunk
TEXT_END, read from the player's graphics with kw_end_text, as for the other
Galaxy endings) is a conversation, spoken page by page (SC_END_<page>); its
speakers are named in the text (no colour codes), so they are read here and
voiced by name. Keen, Rick Uller, Mortimer and the narrator keep the voices
they have everywhere. New here:

  The Clorts     (the cloned creatures)    am_puck, raised

Who says what: Keen (bm_fable), Rick Uller (am_liam, from Bounty and The
Prisoner's Dilemma), Mortimer McMire (bm_daniel), the Clorts (dim, then
suddenly erudite), and the narrator (bm_lewis) for the scene-setting lines.
"""
import os, re, subprocess, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_landing_voice import speak_all, RATE
from kw_keen4_voice import STYLE as BASE_STYLE, KEEN as K, NARRATOR as N
from kw_keen789_voice import MORTIMER
from kw_packs_voice import RICK as R
from kw_end_text import pages

CLORT = 'am_puck'
STYLE = dict(BASE_STYLE)
STYLE[CLORT] = dict(pitch=1.15)
GAP = 0.3
TEXT_END = 4918   # Keen 5's ending-text chunk (GFXCHUNK.CK5: TEXT_END)

# Who each name in the ending is (the text labels them, e.g. "Mort:", "Clort
# #5512:"). The narrator reads anything without a speaker.
VOICE = {'Keen': K, 'Rick': R, 'Mort': MORTIMER, 'Clort': CLORT, 'Clorts': CLORT}

# The cloaking-device quip Keen says at a machine (ck5_str_fuseDestroyed).
FUSE = ["Cool!", "I wonder what this cloaking device was hiding."]

# A speaker turn in the ending: a name (perhaps with a ", ..." aside before the
# colon), then the quote. Anything outside such turns is the narrator's.
TURN = re.compile(r'([A-Z][A-Za-z]*(?: #\d+)?)[^:"]*:\s*"([^"]*)"')


def ending_turns(folder, keen5):
    """Each ending page as a list of (voice, [lines]) turns (empty pages give
    no clip)."""
    egadict = os.path.join(keen5, 'EGADICT.CK5')
    out = {}
    for i, page in enumerate(pages(folder, TEXT_END, egadict, ext='CK5')):
        text = ' '.join(t for _, t in page).strip()
        if not text:
            continue
        turns, last = [], 0
        for m in TURN.finditer(text):
            lead = text[last:m.start()].strip(' "')
            if lead:
                turns.append((N, [lead]))
            name = m.group(1).split(' #')[0]
            turns.append((VOICE.get(name, N), [m.group(2).strip()]))
            last = m.end()
        tail = text[last:].strip(' "')
        if tail:
            turns.append((N, [tail]))
        if not turns:   # pure narration (no quoted speaker), e.g. "Later..."
            turns = [(N, [text])]
        out['SC_END_%02d.RAW' % i] = turns
    return out


def main(folder, keen5, out):
    os.makedirs(out, exist_ok=True)
    clips = {'SC_KEEN_FUSE.RAW': [(K, FUSE)]}
    clips.update(ending_turns(folder, keen5))
    with tempfile.TemporaryDirectory() as tmp:
        by_voice, parts = {}, {}
        for name, turns in clips.items():
            for i, (voice, words) in enumerate(turns):
                seg = 'SEG_%s_%d.RAW' % (name[:-4], i)
                by_voice.setdefault(voice, {})[seg] = words
                parts.setdefault(name, []).append((voice, seg))
        for voice, segs in by_voice.items():
            speak_all(segs, voice, tmp, **STYLE.get(voice, {}))
        for name, segs in parts.items():
            buf = bytearray()
            for k, (voice, seg) in enumerate(segs):
                if k:
                    buf += bytes(2 * int(GAP * RATE))
                buf += open(os.path.join(tmp, seg), 'rb').read()
            open(os.path.join(out, name), 'wb').write(buf)
            print('%-20s %5.1f s' % (name, len(buf) / 2 / RATE))


if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
