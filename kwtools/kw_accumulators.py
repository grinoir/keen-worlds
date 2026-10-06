"""Write Keen Worlds' variable file for The Eight Accumulators ("Keen 9.5").

    python3 tools/kw_accumulators.py "accumulators/game/Keen 9.5" omnispeak/data/keen4 play/mods/keen95

The Eight Accumulators (Bubbatom, 2011, with help from Ceilick, Mink and
Gridlock) is a Keen 4 level pack: its own levels, graphics and texts, no new
behaviour. It changes the Keen 4 program with a CKPatch file (Mymod.PAT).
Omnispeak is not that program, so this tool turns the patches into settings:

- level names and entry texts (the patch file's %level lines);
- the music of each level (its table at $2F206);
- every text of the game the patches change: each string Omnispeak keeps in
  its Keen 4 variable files is looked for in the unpatched program, and
  read back from the same place after the patches are applied;
- the story that scrolls by (STORY.CK4), the high score names.

It copies the game's data files to the world's folder and writes MOD.CK4
there. Nothing else in the patch file is left over (checked: a patch that
lands outside those places is reported).
"""
import os, re, shutil, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_patconv import load_image, parse_pat, strip_comments

MUSIC_TABLE, MUSIC_LEVELS = 0x2F206, 20
HIGHSCORE_NAMES = 0x33234
DATA_FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'Egagraph.ck4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4',
              'Gamemaps.ck4': 'GAMEMAPS.CK4', 'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}


def unescape(s):
    return s.encode('latin1').decode('unicode_escape')


def escape(s):
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')


def read_strings(folder):
    """Omnispeak's Keen 4 strings: {name: text} and {name: [texts]} for arrays."""
    single, arrays = {}, {}
    for fn in ('STRINGS.CK4', 'EPISODE.CK4'):
        text = open(os.path.join(folder, fn), encoding='latin1').read()
        text = re.sub(r'(?m)#[^"\n]*$', '', text)
        for m in re.finditer(r'%(string|stringarray)\s+(\w+)((?:\s*,?\s*"(?:[^"\\]|\\.)*")+)', text):
            parts = m.group(3)
            if m.group(1) == 'string':
                single[m.group(2)] = unescape(''.join(re.findall(r'"((?:[^"\\]|\\.)*)"', parts)))
            else:
                # array items are parted by commas; pieces without a comma between them are one item
                items = [unescape(''.join(re.findall(r'"((?:[^"\\]|\\.)*)"', item))) for item in re.split(r',(?=\s*")', parts)]
                arrays[m.group(2)] = items
    return single, arrays


def cstring(image, at):
    end = image.index(b'\0', at)
    return image[at:end].decode('latin1')


def main(game, keen4, out):
    convert(game, keen4, out)


def convert(game, keen4, out, pat='Mymod.PAT', files=DATA_FILES, extra=(), skip=('keen4.tli', 'ck4load.bin'),
            title='The Eight Accumulators ("Keen 9.5": Bubbatom, 2011)', tool='tools/kw_accumulators.py', handled=(),
            level_names=None):
    """The work, for any Keen 4 mod of this kind (tools/kw_bounty.py uses it
    too). extra: lines for the variable file that stand for patches in
    program code; handled: the offsets of those patches, so that they are
    not reported as left over. level_names: {level: name} given outright."""
    pat_path = os.path.join(game, pat)
    image = load_image(os.path.join(game, 'KEEN4E.EXE'))
    patched = bytearray(image)
    spans = []
    for off, data in parse_pat(pat_path):
        patched[off:off + len(data)] = data
        spans.append([off, off + len(data), False])
    pat_text = strip_comments(open(pat_path, encoding='latin1').read())
    for m in re.finditer(r'%patchfile\s+\$([0-9A-Fa-f]+)\s+"?([\w.]+)"?', pat_text):
        data = open(os.path.join(game, [f for f in os.listdir(game) if f.lower() == m.group(2).lower()][0]), 'rb').read()
        off = int(m.group(1), 16)
        if m.group(2).lower() in skip:
            continue   # tile properties (a file of its own here); the loading screen's text-mode picture
        patched[off:off + len(data)] = data
        spans.append([off, off + len(data), False])

    # Every patch lands in a C string of the program: find where that string
    # begins, which of Omnispeak's strings it is (spaces and line ends are
    # not always the same in its variable files), and read it back patched.
    norm = lambda t: re.sub(r'\s+', ' ', t).strip()
    found = {}   # normalised original text -> the patched text

    def note_strings():
        for span in spans:
            start = image.rfind(b'\0', 0, span[0]) + 1
            found[norm(cstring(image, start))] = (cstring(patched, start), span)

    def changed(text):
        """The text as the patched program has it, or None if no patch touches it."""
        raw = text.encode('latin1') + b'\0'
        at = image.find(raw)
        if at >= 0:
            # found as it is: read it back from the same place
            new = cstring(patched, at)
            for span in spans:
                if span[0] < at + max(len(raw), len(new) + 1) and span[1] > at:
                    span[2] = True
            return new if new != text else None
        # not letter for letter (Omnispeak's file has a typing error or other
        # spacing): the string a patch lands in that begins the same way
        key = norm(text)[:12]
        for original, (new, span) in found.items():
            if original.startswith(key):
                for other in spans:
                    start = image.rfind(b'\0', 0, other[0]) + 1
                    if norm(cstring(image, start)) == original:
                        other[2] = True
                return new
        return None

    note_strings()
    single, arrays = read_strings(keen4)
    lines = ['# %s as a Keen Worlds world.' % title,
             '# Written by %s from the mod\'s CKPatch file; the data' % tool,
             '# files beside it are the player\'s copy of the mod. It runs on Keen 4.', '',
             '%string ca_tileInfo "KEEN4.TLI"', '']
    # level names and entry texts
    names, entries = {}, {}
    for m in re.finditer(r'%level\.name\s+(\d+)\s+([^\n]*)', pat_text):
        names[int(m.group(1))] = m.group(2).strip()
    for m in re.finditer(r'%level\.entry\s+(\d+)\s+((?:(?!%)[\s\S])*)', pat_text):
        entries[int(m.group(1))] = '\n'.join(l.strip() for l in m.group(2).strip().split('\n') if l.strip())
    names.update(level_names or {})   # (names the caller knows: where Omnispeak's own spelling differs from the program's)
    base_names, base_entries = arrays['ck_str_levelNames'], arrays['ck_str_levelEntryText']
    n = len(base_names)
    # (a mod may also write them over the program's own, as texts)
    def item(table, base, i):
        if i in table:
            return table[i]
        new = changed(base[i])
        return new if new is not None else base[i]
    lines.append('%stringarray ck_str_levelNames\n' + ',\n'.join('\t"%s"' % escape(item(names, base_names, i)) for i in range(n)) + '\n')
    lines.append('%stringarray ck_str_levelEntryText\n' + ',\n'.join('\t"%s"' % escape(item(entries, base_entries, i)) for i in range(len(base_entries))) + '\n')
    # music
    music = [patched[MUSIC_TABLE + 2 * i] | patched[MUSIC_TABLE + 2 * i + 1] << 8 for i in range(MUSIC_LEVELS)]
    for span in spans:
        if span[0] >= MUSIC_TABLE and span[1] <= MUSIC_TABLE + 2 * MUSIC_LEVELS:
            span[2] = True
    lines.append('%intarray CK_LevelMusic ' + ', '.join(map(str, music)) + '\n')
    # texts
    count = 0
    for name, text in sorted(single.items()):
        new = changed(text)
        if new is not None:
            lines.append('%%string %s "%s"' % (name, escape(new)))
            count += 1
    for name, items in sorted(arrays.items()):
        if name in ('ck_str_levelNames', 'ck_str_levelEntryText', 'ck_defaultHighScoreNames'):
            continue
        news = [changed(t) for t in items]
        if any(x is not None for x in news):
            lines.append('%%stringarray %s\n' % name + ',\n'.join('\t"%s"' % escape(x if x is not None else t) for x, t in zip(news, items)))
            count += 1
    # the names in the high score table: eight, 64 bytes apart
    scores = [cstring(patched, HIGHSCORE_NAMES + 0x40 * i) for i in range(8)]
    for span in spans:
        if span[0] >= HIGHSCORE_NAMES and span[1] <= HIGHSCORE_NAMES + 0x40 * 8:
            span[2] = True
    lines.append('%stringarray ck_defaultHighScoreNames\n' + ',\n'.join('\t"%s"' % escape(x) for x in scores))
    if extra:
        lines += ['', '# The mod\'s changes to the program\'s rules, as settings'] + list(extra)
    for span in spans:
        if span[0] in handled:
            span[2] = True
    lines += ['', '%override "EPISODE.CK4"', '']
    os.makedirs(out, exist_ok=True)
    for src, dst in files.items():
        shutil.copyfile(os.path.join(game, src), os.path.join(out, dst))
    open(os.path.join(out, 'MOD.CK4'), 'w', encoding='latin1').write('\n'.join(lines))
    print('wrote %s/MOD.CK4: %d level names, %d texts changed' % (out, len(names), count))
    for off, end, used in spans:
        if not used:
            print('  patch not turned into a setting: $%X (%d bytes) %r' % (off, end - off, bytes(patched[off:off + min(24, end - off)])))


if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
