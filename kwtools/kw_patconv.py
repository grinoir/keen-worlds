"""Convert a fan game's CKPatch file into Omnispeak variable-file lines.

A CKPatch (.PAT) file overwrites bytes of the original DOS program. Omnispeak
is not that program, so each patch has to become a setting. This tool does the
mechanical part and lists the rest:

  actions   every sprite action the patches change (or add), as `%action`
            lines ready for the mod's variable file (after its `%override`)
  code      every patch inside program code: the function it is in (named
            from the action table), the old and new bytes - these need a
            matching `%int`, or new C, by hand
  data      other data patches (tables, numbers); text patches are skipped
  check     compare a finished variable file with the patched program:
            every action must come out identical

Usage:
  python3 tools/kw_patconv.py <actions|code|data|all> KEENx.EXE FILE.PAT DATADIR [NEW=OLD ...]
  python3 tools/kw_patconv.py check KEENx.EXE FILE.PAT DATADIR MOD.CKx [NEW=OLD ...]

NEW=OLD names our function NEW for one of the program's: OLD is a function
name (ours stands in for the program's patched one) or SEG:OFF, the address
of code the patches add (e.g. KW9_MineCol=06B9:015F).

KEENx.EXE is the unmodified v1.4 program the mod patches (fan games ship
it; it is only read here). DATADIR is Omnispeak's data folder for that game
(e.g. omnispeak/data/keen6e14): its ACTION and GFXCHUNK files name things.
"""
import glob, os, re, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from unlzexe import unlzexe

ACTION_SIZE = 30
TYPES = ['UnscaledOnce', 'ScaledOnce', 'Frame', 'UnscaledFrame', 'ScaledFrame']


def load_image(path):
    """The program's load image (no MZ header), unpacked if LZEXE-packed."""
    d = open(path, 'rb').read()
    hdr = struct.unpack_from('<H', d, 8)[0] * 16
    if d[0x1C:0x20] == b'LZ91':
        return unlzexe(path)
    return d[hdr:]


def strip_comments(text):
    out = []
    for line in text.split('\n'):
        o, q = '', False
        for c in line:
            if c == '"':
                q = not q
            if c == '#' and not q:
                break
            o += c
        out.append(o)
    return '\n'.join(out)


def parse_pat(path):
    """[(offset, bytes)] for every active %patch, in file order."""
    txt = strip_comments(open(path, encoding='latin1').read())
    pats = []
    for m in re.finditer(r'%patch\s+\$([0-9A-Fa-f]+)((?:(?!%)[\s\S])*)', txt):
        b = bytearray()
        for t in re.finditer(r'"([^"]*)"|(\$[0-9A-Fa-f]+|-?\d+)(RL|RW|W|L)?(?![0-9A-Za-z])', m.group(2), re.I):
            if t.group(1) is not None:
                b += t.group(1).encode('latin1')
            else:
                # $ is hex, a bare number decimal. No suffix: one byte ($0060
                # is byte $60, as CKPatch reads it).
                v = int(t.group(2)[1:], 16) if t.group(2)[0] == '$' else int(t.group(2))
                n = {'W': 2, 'RW': 2, 'L': 4, 'RL': 4}.get((t.group(3) or '').upper(), 1)
                b += (v & (256 ** n - 1)).to_bytes(n, 'little')
        pats.append((int(m.group(1), 16), bytes(b)))
    return pats


def tokens(line):
    return re.findall(r'"[^"]*"|[^\s,]+', strip_comments(line))


class Game:
    def __init__(self, exe, datadir):
        self.img = load_image(exe)
        self.ints = {}
        self.lines = {}   # action name -> its 13 tokens
        self.byoff = {}   # data-segment offset -> action name
        for f in sorted(glob.glob(os.path.join(datadir, 'GFXCHUNK.CK?'))):
            self.read_vars(f)
        for f in sorted(glob.glob(os.path.join(datadir, 'ACTION.CK?'))):
            self.read_vars(f)
        self.find_dseg()
        self.funcs = {0: 'NULL'}  # far pointer -> function name
        # Functions only ever used to collide. A patch that makes one an
        # action's think function gets nothing useful on DOS (it reads a
        # second argument that isn't there), so that think is NULL for us.
        self.collide_only = {f[10] for f in self.lines.values()} - \
            {x for f in self.lines.values() for x in (f[9], f[11])} - {'NULL'}
        for name, f in self.lines.items():
            a = self.unpack(self.img, int(f[0], 0))
            for ptr, fn in zip(a[8:11], f[9:12]):
                if self.funcs.setdefault(ptr, fn) != fn:
                    sys.exit('function pointer %08X is both %s and %s' % (ptr, self.funcs[ptr], fn))

    def stand_ins(self, pairs):
        for pair in pairs:
            new, old = pair.split('=')
            if ':' in old:
                seg, off = (int(x, 16) for x in old.split(':'))
                self.funcs[seg << 16 | off] = new
            else:
                ptr = [p for p, n in self.funcs.items() if n == old]
                if not ptr:
                    sys.exit('no function %s' % old)
                self.funcs[ptr[0]] = new

    def read_vars(self, path):
        for line in open(path, encoding='latin1'):
            t = tokens(line)
            if len(t) >= 3 and t[0] == '%int':
                self.ints[t[1]] = self.num(t[2])
            elif len(t) == 15 and t[0] == '%action':
                self.lines[t[1]] = t[2:]
                self.byoff[int(t[2], 0)] = t[1]

    def num(self, tok):
        return self.ints[tok[1:]] if tok.startswith('@') else int(tok, 0)

    def find_dseg(self):
        """The data segment's image offset: where every action's timer matches."""
        probe = sorted(self.byoff)[:40]
        if not probe:
            sys.exit('no actions read from the data folder')
        for base in range(0, len(self.img) - max(probe) - ACTION_SIZE, 16):
            if all(struct.unpack_from('<h', self.img, base + o + 10)[0] ==
                   self.num(self.lines[self.byoff[o]][6]) for o in probe) and \
               all(struct.unpack_from('<h', self.img, base + o)[0] ==
                   self.num(self.lines[self.byoff[o]][1]) for o in probe):
                self.dseg = base
                return
        sys.exit('data segment not found: is this the v1.4 program for this data folder?')

    def unpack(self, img, off):
        p = self.dseg + off
        return struct.unpack_from('<8h', img, p) + struct.unpack_from('<3I', img, p + 16) + \
            struct.unpack_from('<H', img, p + 28)

    def patched(self, pats):
        img = bytearray(self.img)
        for off, b in pats:
            img[off:off + len(b)] = b
        return bytes(img)

    def funcname(self, ptr):
        return self.funcs.get(ptr, '?%04X:%04X' % (ptr >> 16, ptr & 0xFFFF))

    def thinkname(self, ptr):
        name = self.funcname(ptr)
        return 'NULL' if name in self.collide_only else name

    def function_at(self, off):
        """(name, offset into it) of the action function containing a code offset."""
        best = None
        for ptr, name in self.funcs.items():
            lin = (ptr >> 16) * 16 + (ptr & 0xFFFF)
            if ptr and lin <= off and (best is None or lin > best[0]):
                best = (lin, name)
        if best is None or off - best[0] > 0x400:
            return None
        return best[1], off - best[0]

    def new_actions(self, pats):
        """Data-segment offsets where the patches build whole new actions."""
        new = {}
        img = self.patched(pats)
        for off, b in pats:
            d = off - self.dseg
            if d < 0 or len(b) < ACTION_SIZE or len(b) % ACTION_SIZE:
                continue
            for k in range(0, len(b), ACTION_SIZE):
                a = self.unpack(img, d + k)
                if d + k not in self.byoff and all(p in self.funcs for p in a[8:11]):
                    new[d + k] = 'KW_ACT_New%04X' % (d + k)
        return new

    def action_line(self, name, off, a, names, base=None):
        """The %action line for unpacked action `a`; unchanged fields keep base tokens."""
        f = list(base) if base else [''] * 13
        old = self.unpack(self.img, off) if base else None
        vals = ['0x%04X' % off, str(a[0]), str(a[1]), TYPES[a[2]] if 0 <= a[2] < 5 else str(a[2]),
                str(a[3]), str(a[4]), str(a[5]), str(a[6]), str(a[7]),
                self.thinkname(a[8]), self.funcname(a[9]), self.funcname(a[10]),
                names.get(a[11], 'NULL' if a[11] == 0 else '?0x%04X' % a[11])]
        for i in range(1, 13):
            j = i - 1 if i < 12 else 11
            # A function is compared by name: ours may stand in for the program's.
            if not base or (f[i] != vals[i] if 9 <= i <= 11 else old[j] != a[j]):
                f[i] = vals[i]
        f[0] = vals[0]
        return '%%action %s %s' % (name, ' '.join(f))


def classify(g, pats):
    acts, code, data = set(), [], []
    for off, b in pats:
        d = off - g.dseg
        if d < 0:
            code.append((off, b))
            continue
        hit = [o for o in g.byoff if o < d + len(b) and d < o + ACTION_SIZE]
        if hit:
            acts.update(hit)
        else:
            data.append((off, b))
    return acts, code, data


def is_text(b):
    return len(b) >= 4 and sum(32 <= c < 127 or c in (0, 10) for c in b) == len(b) and \
        sum(65 <= (c & 0xDF) <= 90 for c in b) >= 3


def cmd_actions(g, pats):
    acts, _, _ = classify(g, pats)
    img = g.patched(pats)
    new = g.new_actions(pats)
    names = dict(g.byoff)
    names.update(new)
    for off in sorted(acts):
        acts.add(off)
    for off in sorted(g.byoff):
        a = g.unpack(img, off)
        base = g.lines[names[off]]
        if a != g.unpack(g.img, off) or [g.thinkname(a[8]), g.funcname(a[9]), g.funcname(a[10])] != base[9:12]:
            print(g.action_line(names[off], off, a, names, base))
    for off in sorted(new):
        print(g.action_line(new[off], off, g.unpack(img, off), names) + '  # new')


def cmd_code(g, pats):
    _, code, _ = classify(g, pats)
    for off, b in code:
        old = g.img[off:off + len(b)]
        if is_text(b):
            continue
        fn = g.function_at(off)
        where = '%s+$%X' % fn if fn else '(no action function)'
        note = '  (no change)' if old == b else ''
        print('$%05X %-28s %s -> %s%s' % (off, where, old.hex(' '), b.hex(' '), note))


def cmd_data(g, pats):
    _, _, data = classify(g, pats)
    new = g.new_actions(pats)
    for off, b in data:
        if is_text(b) or (off - g.dseg) in new:
            continue
        print('$%05X ds:%04X %s -> %s' % (off, off - g.dseg, g.img[off:off + len(b)].hex(' '), b.hex(' ')))


def cmd_check(g, pats, modfile):
    """Every action of the patched program must equal base + the mod file's lines."""
    g.read_vars(modfile)
    img = g.patched(pats)
    rev = {v: k for k, v in g.funcs.items()}
    bad = 0
    for name, f in sorted(g.lines.items(), key=lambda x: int(x[1][0], 0)):
        off = int(f[0], 0)
        a = g.unpack(img, off)
        want = (g.num(f[1]), g.num(f[2]), TYPES.index(f[3]), g.num(f[4]), g.num(f[5]), g.num(f[6]),
                g.num(f[7]), g.num(f[8]), rev.get(f[9]), rev.get(f[10]), rev.get(f[11]),
                0 if f[12] == 'NULL' else int(g.lines[f[12]][0], 0))
        if g.funcname(a[8]) in g.collide_only:
            a = a[:8] + (0,) + a[9:]
        diff = [i for i in range(12) if a[i] != want[i]]
        if diff:
            bad += 1
            names = ['left', 'right', 'type', 'protect', 'stick', 'timer', 'velX', 'velY',
                     'think', 'collide', 'draw', 'next']
            print('%s: %s' % (name, ', '.join(
                '%s is %s, program has %s' % (names[i], f[i + 1], g.funcname(a[i]) if 8 <= i <= 10 else a[i])
                for i in diff)))
    print('%d actions checked, %d differ' % (len(g.lines), bad))
    return bad


def main():
    if len(sys.argv) < 5:
        sys.exit(__doc__)
    cmd, exe, pat, datadir = sys.argv[1:5]
    g = Game(exe, datadir)
    pats = parse_pat(pat)
    if cmd == 'check':
        g.stand_ins(sys.argv[6:])
        sys.exit(1 if cmd_check(g, pats, sys.argv[5]) else 0)
    g.stand_ins(sys.argv[5:])
    for name, fn in (('actions', cmd_actions), ('code', cmd_code), ('data', cmd_data)):
        if cmd in (name, 'all'):
            print('# %s' % name)
            fn(g, pats)


if __name__ == '__main__':
    main()
