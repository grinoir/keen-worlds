"""Build Omnispeak's support files for Foray in the Forest from its source.

Foray in the Forest (Nisaba and K1n9_Duk3; v2.0, GPL source) keeps what
Omnispeak reads from variable files in C: sprite states, graphics chunk
numbers, counts. This writes them out as the files Keen Worlds loads:

  FITF.CKS      the world's variable file (file names, level names, chunk
                and sound names the engine uses, then Keen 5's as the base)
  FFACTION.CKS  every sprite state, as an action
  FFGFX.CKS     every graphics chunk name of GRAPHCKS.H as `%int`
  AUDINFOE.CKS  sound counts and offsets (binary)

and omnispeak/src/kw_fitf_defs.h, its names for the ported C code. The game's
own GFXINFOE.CKS is used as it is.

Its source descends from Atroxian Realm's (so from Omnispeak's Keen 5 code),
rewritten with Keen Dreams names: a `statetype s_x` is an action. A state
function the engine doesn't have (yet) is written as NULL and listed at the
end: those are still to be ported. Keen's own states that Keen 5 has under
the same name keep the engine's functions for now.

Usage:
  python3 tools/kw_fitf.py foray/src_v20/SOURCE play/mods/fitf \\
      omnispeak/data/keen5 omnispeak/src [own]

With "own" the game's own Keen sprites are used; otherwise its "classic"
graphics (the Keen of Keen 4-6) when EGAGRAPH.CKC, EGAHEAD.CKC and
EGADICT.CKC are in the folder.
"""
import os, re, struct, subprocess, sys, tempfile

def story_var(lines):
    """The story that scrolls away in the title sequence, as the engine's
    string ck_str_storyText (one quoted line a row)."""
    esc = lambda l: l.replace('\\', '\\\\').replace('"', '\\"')
    return '%string ck_str_storyText\n' + '\n'.join('\t"%s\\n"' % esc(l) for l in lines) + '\n\n'


def text_chunk(folder, number):
    """A text chunk of the game's graphics file (Huffman-packed, with its length in front)."""
    graph = open(os.path.join(folder, 'EGAGRAPH.CKS'), 'rb').read()
    head = open(os.path.join(folder, 'EGAHEAD.CKS'), 'rb').read()
    nodes = struct.unpack('<510H', open(os.path.join(folder, 'EGADICT.CKS'), 'rb').read()[:1020])
    start = lambda n: int.from_bytes(head[n * 3:n * 3 + 3], 'little')
    a, nxt = start(number), number + 1
    while start(nxt) == 0xFFFFFF:
        nxt += 1
    length = struct.unpack_from('<I', graph, a)[0]
    src, out, node, p, bit = graph[a + 4:start(nxt)], bytearray(), 254, 0, 0
    while len(out) < length:
        v = nodes[node * 2 + ((src[p] >> bit) & 1)]
        bit += 1
        if bit == 8:
            bit, p = 0, p + 1
        if v < 256:
            out.append(v)
            node = 254
        else:
            node = v - 256
    return out.decode('latin1')


STATE_FILES = ['CK_STATE.C', 'CK_KEEN.C', 'CK_KEEN2.C', 'CK_PLAY.C', 'CK_ACT.C']
PROGRESS = {'step': 'UnscaledOnce', 'slide': 'ScaledOnce', 'think': 'Frame',
            'stepthink': 'UnscaledFrame', 'slidethink': 'ScaledFrame'}
PUSH = {'push_none': 0, 'push_down': 1, 'push_up': 2, 'push_left': 3, 'push_right': 4, 'push_side': 5,
        'false': 0, 'true': 1}

# Its functions that the engine has under another name
RENAMES = {
    'CK_ObjBadState': 'CK_ObjBadstate',
}

# Actions Omnispeak's shared code asks for by a name this source doesn't use:
# the engine's name -> its state
ACTION_ALIASES = {
}

# Sound names Omnispeak's shared code plays -> its sound
SOUNDS = {
    'SOUND_KEENWALK0': 'KEENWALK1SND', 'SOUND_KEENWALK1': 'KEENWALK2SND', 'SOUND_KEENJUMP': 'KEENJUMPSND',
    'SOUND_KEENLAND': 'KEENLANDSND', 'SOUND_KEENSHOOT': 'KEENSHOOTSND', 'SOUND_KEENPOGO': 'KEENPOGOSND',
    'SOUND_GOTITEM': 'GOTITEMSND', 'SOUND_GOTSTUNNER': 'GOTSTUNNERSND', 'SOUND_GOTCENTILIFE': 'GOTVITALINSND',
    'SOUND_UNKNOWN12': 'ENTERLEVELSND', 'SOUND_LEVELEXIT': 'LEVELDONESND',
    'SOUND_KEENHITCEILING': 'KEENHITCEILINGSND',
}

# Chunk names Omnispeak's shared code asks for -> its name. Names missing
# here (or not in its header) keep Keen 5's number until they are mapped.
CHUNKS = {
    'FON_MAINFONT': 3, 'FON_WATCHFONT': 4, 'FON_STARWARSFONT': 5,
    'PIC_HELPMENU': 'H_MENUPIC',
    'PIC_BORDERTOP': 'H_TOPWINDOWPIC', 'PIC_BORDERLEFT': 'H_LEFTWINDOWPIC', 'PIC_BORDERRIGHT': 'H_RIGHTWINDOWPIC',
    'PIC_BORDERBOTTOMSTATUS': 'H_BOTTOMINFOPIC', 'PIC_BORDERBOTTOM': 'H_BOTTOMWINDOWPIC',
    'PIC_MENUCARD': 'CP_MAINMENUPIC', 'PIC_NEWGAMECARD': 'CP_NEWGAMEMENUPIC', 'PIC_LOADCARD': 'CP_LOADMENUPIC',
    'PIC_SAVECARD': 'CP_SAVEMENUPIC', 'PIC_CONFIGURECARD': 'CP_CONFIGMENUPIC', 'PIC_SOUNDCARD': 'CP_SOUNDMENUPIC',
    'PIC_MUSICCARD': 'CP_MUSICMENUPIC', 'PIC_KEYBOARDCARD': 'CP_KEYBOARDMENUPIC',
    'PIC_MOVEMENTCARD': 'CP_KEYMOVEMENTPIC', 'PIC_BUTTONSCARD': 'CP_KEYBUTTONPIC',
    'PIC_JOYSTICKCARD': 'CP_JOYSTICKMENUPIC', 'PIC_OPTIONSCARD': 'CP_OPTIONSMENUPIC',
    'PIC_PADDLEWAR': 'CP_PADDLEWARPIC', 'PIC_QUIT': 'CP_QUITPIC', 'PIC_JOYSTICK': 'CP_JOYSTICKPIC',
    'PIC_WRISTWATCH': 'CP_MENUSCREENPIC', 'MPIC_WRISTWATCHSCREEN': 'CP_MENUMASKPICM',
    'SPR_DEMOSIGN': 'DEMOSIGNSPR', 'SPR_SCOREBOX': 'SCOREBOXSPR',
    'SPR_PADDLE': 'PADDLESPR', 'SPR_BALL0': 'BALLSPR',
    'DEMOSTART': 'DEMO_LUMP_START',
    'SPR_BALL1': 'BALL2PIXELTOTHERIGHTSPR', 'SPR_BALL3': 'BALL3PIXELTOTHERIGHTSPR',
    'PIC_TITLESCREEN': 'TITLEPIC', 'PIC_STARWARS': 'STARWARSPIC',
    'PIC_KEENTALK1': 'KEENTALKPIC', 'PIC_KEENTALK2': 'KEENTALKPIC',
    # the "Keen enters" box: its picture of Keen, then the hand counting 5 to 1
    'PIC_COUNTDOWN5': 'CD_FIVEPIC', 'PIC_COUNTDOWN4': 'CD_FOURPIC', 'PIC_COUNTDOWN3': 'CD_THREEPIC',
    'PIC_COUNTDOWN2': 'CD_TWOPIC', 'PIC_COUNTDOWN1': 'CD_ONEPIC', 'PIC_COUNTDOWN0': 'CD_KEENPIC',
    'PIC_CREDIT1': 'I_IDMODPIC', 'PIC_CREDIT2': 'I_ARTDIRECTORPIC', 'PIC_CREDIT3': 'I_PROGRAMMERPIC',
    'PIC_CREDIT4': 'I_TOOLSMITHPIC',
    # the two pictures hanging on the status window as it scrolls: the engine's
    # "right" one (at x 136) is its top picture, the "left" one (at x 40) its left
    'MPIC_STATUSRIGHT': 'STATUSTOPPICM', 'MPIC_STATUSLEFT': 'STATUSLEFTPICM',
    'EXTERN_COMMANDER': 'I_COMMANDEREXTERN', 'EXTERN_KEEN': 'I_KEENEXTERN',
    'TEXT_HELPMENU': 'T_HELPGAME', 'TEXT_STORY': 'T_HELPSTORY', 'TEXT_CONTROLS': 'T_HELPCONTROLS',
    'TEXT_ABOUTID': 'T_HELPCREDITS', 'TEXT_ORDER': 'T_HELPNEWS', 'TEXT_END': 'T_END_WIN',
    'TEXT_SECRETEND': 'T_END_ALL',
    'SPR_STARS1': 'STUNNEDSTARS1SPR', 'SPR_VIVASPLASH1': 'VITALINBURST1SPR',
    'SPR_MAPKEEN_WALK1_W': 'WORLDKEENL1SPR', 'SPR_MAPKEEN_WALK1_E': 'WORLDKEENR1SPR',
    'SPR_MAPKEEN_WALK1_N': 'WORLDKEENU1SPR', 'SPR_MAPKEEN_WALK1_S': 'WORLDKEEND1SPR',
    'SPR_MAPKEEN_WALK1_SE': 'WORLDKEENDR1SPR', 'SPR_MAPKEEN_WALK1_SW': 'WORLDKEENDL1SPR',
    'SPR_MAPKEEN_WALK1_NW': 'WORLDKEENUL1SPR', 'SPR_MAPKEEN_WALK1_NE': 'WORLDKEENUR1SPR',
    'SPR_MAPKEEN_STAND_W': 'WORLDKEENL1SPR',
    'SPR_GEM_A1': 'REDGEM1SPR', 'SPR_GEM_B1': 'YELLOWGEM1SPR', 'SPR_GEM_C1': 'BLUEGEM1SPR', 'SPR_GEM_D1': 'GREENGEM1SPR',
    'SPR_100_PTS1': 'POINTS100ITEM1SPR', 'SPR_200_PTS1': 'POINTS200ITEM1SPR', 'SPR_500_PTS1': 'POINTS500ITEM1SPR',
    'SPR_1000_PTS1': 'POINTS1000ITEM1SPR', 'SPR_2000_PTS1': 'POINTS2000ITEM1SPR', 'SPR_5000_PTS1': 'POINTS5000ITEM1SPR',
    'SPR_STUNNER1': 'ITEMSTUNNER1SPR', 'SPR_SECURITYCARD_1': 'ITEM3UP1SPR',
    'SPR_BONUS100': 'POINTS100SHADOWSPR', 'SPR_BONUS200': 'POINTS200SHADOWSPR', 'SPR_BONUS500': 'POINTS500SHADOWSPR',
    'SPR_BONUS1000': 'POINTS1000SHADOWSPR', 'SPR_BONUS2000': 'POINTS2000SHADOWSPR',
    'SPR_BONUS5000': 'POINTS5000SHADOWSPR', 'SPR_BONUS1UP': 'POINTS1UPSHADOWSPR',
    'SPR_BONUSCLIP': 'STUNNERSHADOWSPR', 'SPR_BONUSGEM': 'GEMSHADOWSPR', 'SPR_BONUSSECURITYCARD': 'POINTS2UPSHADOWSPR',
}


def strip(text):
    """Remove comments (in one pass: a /* inside a // comment is not one)."""
    pat = r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\''
    return re.sub(pat, lambda m: m.group(0) if m.group(0)[0] in '"\'' else ' ', text)


def read(src, name):
    return strip(open(os.path.join(src, name), encoding='latin1').read())


def header_values(path):
    """Every enum constant and plain #define of a C header, by compiling it
    (its enums use macros and arithmetic a regular expression can't follow)."""
    text = re.sub(r'#error[^\n]*', '', open(path, encoding='latin1').read())
    d = tempfile.mkdtemp(prefix='kw_fitf_')
    hdr = os.path.join(d, 'h.h')
    open(hdr, 'w', encoding='latin1').write(text)
    pre = subprocess.run(['gcc', '-E', '-P', '-x', 'c', hdr], capture_output=True, text=True, check=True).stdout
    names = []
    for body in re.findall(r'enum\s*\w*\s*\{([^}]*)\}', pre):
        names += [m.group(1) for m in re.finditer(r'\b([A-Za-z_]\w*)\s*(?:=[^,]*)?(?:,|$)', body.strip())]
    undef = set(re.findall(r'#undef\s+(\w+)', text))
    names += [n for n in re.findall(r'#define\s+([A-Z_]\w*)[ \t]+\S', strip(text))
              if n not in undef and not n.startswith('__') and not re.search(r'#define\s+%s\(' % n, text)]
    names = list(dict.fromkeys(names))
    prog = os.path.join(d, 'e.c')
    open(prog, 'w').write('#include <stdio.h>\n#include "%s"\nint main(void){\n' % hdr +
                          ''.join('printf("%s %%ld\\n",(long)(%s));\n' % (n, n) for n in names) + 'return 0;}\n')
    r = subprocess.run(['gcc', '-w', '-o', os.path.join(d, 'e'), prog], capture_output=True, text=True)
    if r.returncode:
        sys.exit('%s does not compile:\n%s' % (path, r.stderr[:2000]))
    out = subprocess.run([os.path.join(d, 'e')], capture_output=True, text=True, check=True).stdout
    return {a: int(b) for a, b in (line.split() for line in out.splitlines())}


def action_name(state, k5):
    """The engine's action name for a state: Keen 5's where it has the same
    one (s_keenStanding -> CK_ACT_keenStanding), else FF_ACT_<name>."""
    base = state[2:] if state.startswith('s_') else state
    for cand in ('CK_ACT_' + base, 'CK_ACT_' + base[0].lower() + base[1:]):
        if cand in k5:
            return cand
    return 'FF_ACT_' + base


def main():
    if len(sys.argv) not in (5, 6):
        sys.exit(__doc__)
    src, out, keen5, engine = sys.argv[1:5]
    have = set()
    for name in os.listdir(engine):
        if name.endswith('.c'):
            have.update(re.findall(r'Add(?:Col)?Function\("(\w+)"', open(os.path.join(engine, name), encoding='latin1').read()))
    chunks = header_values(os.path.join(src, 'GRAPHCKS.H'))
    audio = header_values(os.path.join(src, 'AUDIOCKS.H'))

    # --- binary info file (see ca_audinfo in id_ca.h)
    sounds, sndchunks, music = audio['NUMSOUNDS'], audio['NUMSNDCHUNKS'], audio['STARTMUSIC']
    aud = [sndchunks - music, sounds, sndchunks, 0, sounds, 2 * sounds, music]
    open(os.path.join(out, 'AUDINFOE.CKS'), 'wb').write(struct.pack('<7H', *aud))
    gfxinfo = struct.unpack('<23H', open(os.path.join(out, 'GFXINFOE.CKS'), 'rb').read()[:46])
    if gfxinfo[14] != chunks['NUMSPRITES'] or gfxinfo[22] != chunks['STARTEXTERNS']:
        sys.exit('GFXINFOE.CKS does not match GRAPHCKS.H')

    # --- names for the ported C code (omnispeak/src/kw_ff_*.c)
    ah = read(src, 'AUDIOCKS.H')
    enums = re.findall(r'typedef enum\s*\{([^}]*)\}\s*(\w+)', ah)
    sndnames = [n for body, tname in enums if tname == 'soundnames' for n in re.findall(r'\b([A-Z_]\w*)\b', body)]
    musnames = [n for body, tname in enums if tname == 'musicnames' for n in re.findall(r'\b([A-Z_]\w*)\b', body)]
    kd = read(src, 'CK_DEF.H')
    tbody = kd[kd.index('CT_nothing'):]
    types = re.findall(r'\b(CT_\w+)\b', tbody[:tbody.index('}')])
    # Its strings: GAMESTR.H names them, GAMESTR.CKS holds them (a table of
    # 16-bit offsets, then the texts).
    strnames = re.findall(r'^\s*(S_\w+),', read(src, 'GAMESTR.H'), re.M)
    sdata = open(os.path.join(out, 'GAMESTR.CKS'), 'rb').read()
    soffs = struct.unpack_from('<%dH' % (len(strnames) + 1), sdata, 0)
    if soffs[0] != 2 * (len(strnames) + 1):
        sys.exit('GAMESTR.CKS does not match GAMESTR.H')

    def gamestr(name):
        i = strnames.index(name)
        s = sdata[soffs[i]:soffs[i + 1]].split(b'\0')[0].decode('latin1')
        return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')
    with open(os.path.join(engine, 'kw_fitf_defs.h'), 'w') as f:
        f.write('// Foray in the Forest: its names for graphics chunks, sounds, songs and\n'
                '// object types, from GRAPHCKS.H, AUDIOCKS.H and CK_DEF.H of its source.\n'
                '// Generated by tools/kw_fitf.py - do not edit by hand.\n'
                '#ifndef KW_FITF_DEFS_H\n#define KW_FITF_DEFS_H\n\nenum\n{\n')
        for name, val in chunks.items():
            f.write('\tFF_%s = %d,\n' % (name, val))
        f.write('};\n\nenum\n{\n')
        for name in sndnames:
            f.write('\tFF_%s = %d,\n' % (name, audio[name]))
        f.write('};\n\n// Songs\nenum\n{\n')
        for name in musnames:
            f.write('\tFF_%s = %d,\n' % (name, audio[name]))
        f.write('};\n\n// Its strings (GAMESTR.H): FF_String() takes these\nenum\n{\n')
        for i, name in enumerate(strnames):
            f.write('\tFF_%s = %d,\n' % (name, i))
        f.write('\tFF_NUMGAMESTRINGS = %d,\n' % len(strnames))
        f.write('};\n\n// Object types (0-8 are the engine\'s own, with the same numbers)\nenum\n{\n')
        for i, name in enumerate(types):
            if i >= 9:
                f.write('\tFF_%s = %d,\n' % (name, i))
        f.write('};\n\n#endif\n')

    # Its colour palettes (CK_GAME.C): sixteen VGA colours per level, 6 bits a
    # channel, and GetLevelPalette's table of which level uses which.
    game = read(src, 'CK_GAME.C')
    pals = {}
    for m in re.finditer(r'byte\s+far\s+(\w+palette)\[\]\s*=\s*\{([^}]*)\}', game):
        vals = [int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]+)', m.group(2))]
        if len(vals) == 64:
            pals[m.group(1)] = [vals[i * 4 + 1:i * 4 + 4] for i in range(16)]
    sw = game[game.index('GetLevelPalette'):]
    sw = sw[:sw.index('default:')]
    by_level = {int(a): b for a, b in re.findall(r'case\s+(\d+):\s*return\s*\((\w+)\)', sw)}
    with open(os.path.join(engine, 'kw_fitf_pal.h'), 'w') as f:
        f.write('// Foray in the Forest: its colour palettes (CK_GAME.C of its source), as\n'
                '// 8-bit red, green, blue for the sixteen colours. Generated by\n'
                '// tools/kw_fitf.py - do not edit by hand.\n')
        order = ['defaultpalette'] + sorted(set(by_level.values()) - {'defaultpalette'})
        f.write('static const uint8_t ff_palettes[%d][16][3] = {\n' % len(order))
        for name in order:
            f.write('\t{ // %s\n\t\t' % name + ', '.join(
                '{%d, %d, %d}' % tuple((v << 2) | (v >> 4) for v in rgb) for rgb in pals[name]) + '},\n')
        f.write('};\n// The palette of each level (0: the default one)\nstatic const uint8_t ff_levelPalette[%d] = {' % (max(by_level) + 1))
        f.write(', '.join(str(order.index(by_level.get(i, 'defaultpalette'))) for i in range(max(by_level) + 1)) + '};\n')

    with open(os.path.join(out, 'FFGFX.CKS'), 'w') as f:
        f.write('# Foray in the Forest: graphics chunk names (GRAPHCKS.H). Written by tools/kw_fitf.py.\n')
        for name, val in chunks.items():
            f.write('%%int %s %d\n' % (name, val))

    # --- states -> actions
    k5 = {}
    for line in open(os.path.join(keen5, 'ACTION.CK5'), encoding='latin1'):
        t = line.split()
        if len(t) >= 15 and t[0] == '%action':
            k5[t[1]] = t
    states, order = {}, []
    for name in STATE_FILES:
        text = read(src, name)
        for m in re.finditer(r'\bstatetype\s+(?:far\s+)?(\w+)\s*=\s*\{([^}]*)\}', text):
            body = re.sub(r'\(\s*void\s*\(\s*\*\s*\)\s*\([^)]*\)\s*\)', '', m.group(2))  # function casts
            fields = [x.strip() for x in body.split(',')]
            if fields and fields[-1] == '':
                fields.pop()
            if len(fields) == 11:
                fields.append('NULL')  # no next state given
            if len(fields) != 12:
                sys.exit('%s: state %s has %d fields' % (name, m.group(1), len(fields)))
            if m.group(1) not in states:
                order.append(m.group(1))
            states[m.group(1)] = fields
    names = {s: action_name(s, k5) for s in order}

    # Plain #defines of the source files (timers and speeds of states)
    consts = {}
    for name in STATE_FILES + ['CK_DEF.H']:
        for m in re.finditer(r'#define[ \t]+(\w+)[ \t]+([^\n]+)', read(src, name)):
            consts.setdefault(m.group(1), m.group(2).strip())

    def num(tok, what, depth=0):
        """A state field as a number: arithmetic over numbers, chunk names
        and #defines."""
        def value(m):
            n = m.group(0)
            if n in ('false', 'NULL'):
                return '0'
            if n == 'true':
                return '1'
            if n in chunks:
                return str(chunks[n])
            if n in PUSH:
                return str(PUSH[n])
            if n in consts and depth < 8:
                return str(num(consts[n], what, depth + 1))
            sys.exit('cannot read %s "%s"' % (what, tok))
        expr = re.sub(r'\b[A-Za-z_]\w*\b', value, tok)
        if not re.fullmatch(r'[\d\sxXa-fA-F+\-*/()]+', expr):
            sys.exit('cannot read %s "%s"' % (what, tok))
        return int(eval(expr.replace('/', '//')))

    missing, funcs = {}, set()

    def fn(tok, state, k5name):
        tok = tok.lstrip('&').strip()
        if tok in ('NULL', '0'):
            return 'NULL'
        tok = RENAMES.get(tok, 'FF_' + (tok[3:] if tok.startswith('CK_') else tok))
        if tok in have:
            return tok
        missing.setdefault(tok, []).append(state)
        return k5name if k5name in have else 'NULL'

    nextptr = 0x6000
    with open(os.path.join(out, 'FFACTION.CKS'), 'w') as f:
        f.write('# Foray in the Forest: sprite actions, from the statetype tables of its\n'
                '# source. Written by tools/kw_fitf.py. The second value only has to be\n'
                '# unique (saved games name actions by it); Keen 5 names keep Keen 5\'s.\n')
        for s in order:
            a, name = states[s], names[s]
            base = k5.get(name)
            if base:
                ptr = int(base[2], 0)
            else:
                ptr, nextptr = nextptr, nextptr + 30
            nxt = a[11].lstrip('&').strip()
            if nxt in ('NULL', '0'):
                nxt = 'NULL'
            elif nxt not in states:
                sys.exit('%s: next state %s is not defined' % (s, nxt))
            else:
                nxt = names[nxt]
            fns = [fn(a[8 + i], s, base[11 + i] if base else None) for i in range(3)]
            funcs.update(x for x in fns if x != 'NULL')
            f.write('%%action %s 0x%04X %d %d %s %d %d %d %d %d %s %s %s %s\n' % (
                name, ptr, num(a[0], 'sprite'), num(a[1], 'sprite'), PROGRESS[a[2]],
                num(a[3], 'flag'), num(a[4], 'flag'), num(a[5], 'timer'), num(a[6], 'speed'), num(a[7], 'speed'),
                fns[0], fns[1], fns[2], nxt))
        f.write('\n# The engine\'s own names for some of these\n')
        for name, target in ACTION_ALIASES.items():
            f.write('%%action %s = %s\n' % (name, names[target]))

    # Which of its two graphics sets: its own Keen (EGAGRAPH.CKS), or the one
    # with the Keen of Keen 4-6 (its "classic" edition, EGAGRAPH.CKC with
    # EGAHEAD.CKC and EGADICT.CKC). Keen Worlds takes the classic one when it
    # is there, so that Keen looks the same in every world; "own" as a fifth
    # argument keeps the game's own Keen.
    classic = int(all(os.path.exists(os.path.join(out, n)) for n in ('EGAGRAPH.CKC', 'EGAHEAD.CKC', 'EGADICT.CKC'))
                  and 'own' not in sys.argv[5:])
    gext = 'CKC' if classic else 'CKS'

    # --- the world's variable file
    maphead = open(os.path.join(out, 'MAPHEAD.CKS'), 'rb').read()
    maps = open(os.path.join(out, 'MAPTEMP.CKS'), 'rb').read()
    levels = []
    for i in range(100):
        off = struct.unpack_from('<i', maphead, 2 + 4 * i)[0]
        levels.append(maps[off + 22:off + 38].split(b'\0')[0].decode('latin1') if off > 0 else '')
    while levels and not levels[-1]:
        levels.pop()
    with open(os.path.join(out, 'FITF.CKS'), 'w') as f:
        f.write('# Foray in the Forest (Nisaba and K1n9_Duk3; v2.0) as a Keen Worlds world.\n'
                '# Written by tools/kw_fitf.py from the game\'s GPL source; the data files\n'
                '# beside it are the player\'s copy of the game. It runs on the engine\'s Keen\n'
                '# 5 code, which its own source descends from, so Keen 5\'s variables are the\n'
                '# base.\n\n')
        f.write('%string ca_mapFile "MAPTEMP.CKS"\n%string ca_mapHead "MAPHEAD.CKS"\n'
                '%string ca_tileInfo "TILEINFO.CKS" # none: the tile properties are in MAPHEAD.CKS\n'
                + ('%%string ca_gfxInfoFile "GFXINFOE.CKS"\n%%string ca_graphDict "EGADICT.%s"\n'
                   '%%string ca_graphHead "EGAHEAD.%s"\n%%string ca_graphFile "EGAGRAPH.%s"\n' % (gext, gext, gext))
                + ('%%int ff_classicKeen %d # which of its two graphics sets is used (see kw_fitf.py)\n' % classic) +
                '%string ca_audioInfoFile "AUDINFOE.CKS"\n%string ca_audioDict "AUDIODCT.CKS"\n'
                '%int ca_audioUncompressed 1\n%string ca_audioFile "AUDIOT.CKS"\n%string ca_audioHead "AUDIOHED.CKS"\n\n')
        f.write('%int ck5_mapElevators 0\n\n')
        # Its small (8 pixel) tiles are numbered differently: menu icons from 44
        # (TileBase in its ID_US_2.C), the score box's blank 6 and digits from 7.
        f.write('# Small tiles: menu icons, score box digits\n')
        for i, name in enumerate(('itemTile', 'itemSelectedTile', 'itemCheckedTile', 'itemCheckedSelectedTile',
                                  'itemDisabledTile', 'itemDisabledSelectedTile')):
            f.write('%%int us_menu_%s %d\n' % (name, 44 + i))
        f.write('%int us_menu_scrollIconTopTile1 50\n%int us_menu_scrollIconBottomTile1 51\n'
                '%int us_menu_scrollIconTopTile2 54\n%int us_menu_scrollIconBottomTile2 55\n')
        f.write('%int ck_scoreBoxBlankTile 6\n%int ck_scoreBoxDigitOffset 41\n'
                '%int ck_scoreBoxScoreDigits 7\n%int ck_scoreBoxShotsColumn 6\n%int ck_scoreBoxLivesColumn 2\n\n')
        # Level names and "Keen enters" lines: its own strings where it has them
        lnames = [gamestr('S_LEVELNAME%d' % i) if 'S_LEVELNAME%d' % i in strnames else n for i, n in enumerate(levels)]
        ltexts = [gamestr('S_LEVELTEXT%d' % i) if 'S_LEVELTEXT%d' % i in strnames else 'Keen enters\\n%s' % n
                  for i, n in enumerate(levels)]
        f.write('%stringarray ck_str_levelNames\n' + ',\n'.join('\t"%s"' % n for n in lnames) + '\n\n')
        f.write('%stringarray ck_str_levelEntryText\n' + ',\n'.join('\t"%s"' % n for n in ltexts) + '\n\n')
        # Its story (the text chunk T_STARWARS, up to its end mark '^'), in a
        # font that is in ASCII order
        story = text_chunk(out, chunks['T_STARWARS']).split('^')[0].replace('\r', '')
        f.write(story_var(story.rstrip('\n').split('\n')))
        f.write('%int ck_starWarsPlainText 1\n\n')
        # The engine's own windows, in its words
        for var, name in (('ck_str_tryAgainIntro', 'S_YOUDIDNTMAKEITPAST'), ('ck_str_tryAgainTryAgain', 'S_TRYAGAIN'),
                          ('ck_str_exitToMap', 'S_EXITTOWORLDMAP'), ('ck_str_gameOver', 'S_GAMEOVER')):
            f.write('%%string %s "%s"\n' % (var, gamestr(name)))
        f.write('\n')
        # Song n is "level" n for the engine's StartMusic (see FF_SongFor in kw_fitf.c)
        f.write('%intarray CK_LevelMusic ' + ', '.join(str(i) for i in range(aud[0])) + '\n')
        f.write('%%int CK_NumMusicLevels %d\n\n' % aud[0])
        f.write('# Chunk names the engine\'s shared code uses, as Foray in the Forest numbers them\n')
        for name, val in CHUNKS.items():
            if isinstance(val, str) and val not in chunks:
                print('chunk name not in its header:', val)
                continue
            f.write('%%int %s %d\n' % (name, chunks[val] if isinstance(val, str) else val))
        f.write('\n# Sounds the engine\'s shared code plays, as Foray in the Forest numbers them\n')
        for name, val in SOUNDS.items():
            f.write('%%int %s %d\n' % (name, audio[val]))
        f.write('\n%include "FFGFX.CKS"\n%override "EPISODE.CK5"\n\n'
                '# After Keen 5\'s actions: Foray in the Forest\'s replace those of the same name.\n'
                '%include "FFACTION.CKS"\n')
    k5named = sum(1 for s in order if names[s] in k5)
    print('%d states (%d under Keen 5 names), %d chunk names, %d maps, %d sounds, %d songs' % (
        len(order), k5named, len(chunks), len([n for n in levels if n]), sounds, aud[0]))
    print('functions named by states: %d in the engine, %d still to port' % (len(funcs), len(missing)))
    print(' '.join(sorted(missing)))


if __name__ == '__main__':
    main()
