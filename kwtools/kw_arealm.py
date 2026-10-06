"""Build Omnispeak's support files for Atroxian Realm from its source.

Atroxian Realm (Gridlock, 2016; GPL source) keeps what Omnispeak reads from
variable files in C tables: sprite actions, graphics chunk numbers, counts.
This writes them out as the files Keen Worlds loads for the world:

  AREALM.CKD    the world's variable file (file names, level names, chunk
                names the engine uses, then Keen 5's as the base)
  ARACTION.CKD  every action, from the `CK_action x = {...}` initialisers
  ARGFX.CKD     every graphics chunk name of GRAPHCKD.H as `%int`
  GFXINFOE.CKD  graphics counts and offsets (binary)
  AUDINFOE.CKD  sound counts and offsets (binary)

An action's function that the engine doesn't have (yet) is written as NULL
and listed at the end: those are the creatures still to be ported.

Usage:
  python3 tools/kw_arealm.py atroxian/arealm_src_v15/source play/mods/arealm \\
      omnispeak/data/keen5 omnispeak/src
"""
import os, re, struct, sys

def story_var(lines):
    """The story that scrolls away in the title sequence, as the engine's
    string ck_str_storyText (one quoted line a row)."""
    esc = lambda l: l.replace('\\', '\\\\').replace('"', '\\"')
    return '%string ck_str_storyText\n' + '\n'.join('\t"%s\\n"' % esc(l) for l in lines) + '\n\n'


ACTION_FILES = ['KD_KEEN.C', 'KD_OBJ.C', 'AR_MISC.C', 'AR_MAP.C', 'AR_ACT1.C', 'AR_ACT2.C', 'AR_ACT3.C']
TYPES = {'AT_UnscaledOnce': 'UnscaledOnce', 'AT_ScaledOnce': 'ScaledOnce', 'AT_Frame': 'Frame',
         'AT_UnscaledFrame': 'UnscaledFrame', 'AT_ScaledFrame': 'ScaledFrame'}

# Functions Omnispeak has under a newer name than Atroxian Realm's source
RENAMES = {
    'CK5_FallPlatSit': 'CK_FallPlatSit', 'CK5_FallPlatFall': 'CK_FallPlatFall', 'CK5_FallPlatRise': 'CK_FallPlatRise',
    'CK5_RedAxisPlatform': 'CK_AxisPlatform', 'CK5_PointItem': 'CK_PointItem', 'CK_ObjBadState': 'CK_ObjBadstate',
    'CK5_MapKeenStill': 'CK_MapKeenStill', 'CK5_MapKeenWalk': 'CK_MapKeenWalk',
    'CK_DeadlyCol2': 'CK_LethalCol', 'CK_RemoveOnContact': 'CK_DieOnContactDraw',
    'CK_KeenFlagThrown': 'CK_MapFlagThrown', 'CK_KeenFlagSearch': 'CK_MapFlagFall', 'CK_KeenFlagLands': 'CK_MapFlagLand',
}

# Actions Omnispeak's shared code asks for by a name Atroxian Realm's source
# doesn't use: the engine's name -> its action
ACTION_ALIASES = {
    'CK_ACT_AxisPlatform': 'CK5_act_redAxisPlatform', 'CK_ACT_StandPlatform': 'CK5_ACT_redStandPlatform',
    'CK_ACT_FallPlat0': 'CK5_ACT_FallPlat0', 'CK_ACT_FallPlat1': 'CK5_ACT_FallPlat1', 'CK_ACT_FallPlat2': 'CK5_ACT_FallPlat2',
    'CK_ACT_item': 'CK5_act_item', 'CK_ACT_CentilifeNotify1': 'CK_ACT_VitalinNotify1',
    'CK_ACT_MapFlag0': 'CK5_ACT_MapFlag0', 'CK_ACT_MapFlagFlips0': 'CK5_ACT_MapFlagFlips0',
    'CK_ACT_MapKeenStart': 'CK5_ACT_MapKeenStart', 'CK_ACT_MapKeenWalk0': 'CK5_ACT_MapKeenWalk0',
    'CK_ACT_NULL': 'CK_ACT_Null', 'CK_ACT_keenEnterDoor0': 'CK_ACT_keenEnterDoor1',
}

# Sound names Omnispeak's shared code plays -> Atroxian Realm's sound
SOUNDS = {
    'SOUND_KEENWALK0': 'KEENWALK1SND', 'SOUND_KEENWALK1': 'KEENWALK2SND', 'SOUND_KEENJUMP': 'JUMPSND',
    'SOUND_KEENLAND': 'LANDSND', 'SOUND_KEENSHOOT': 'THROWSND', 'SOUND_KEENPOGO': 'KEENPOGOSND',
    'SOUND_GOTITEM': 'GOTITEMSND', 'SOUND_GOTSTUNNER': 'GOTSTUNNERSND', 'SOUND_GOTCENTILIFE': 'GOTVITALINSND',
    'SOUND_UNKNOWN12': 'LEVELSTARTSND', 'SOUND_LEVELEXIT': 'LEVELDONESND', 'SOUND_NEEDKEYCARD': 'NOWAYSND',
    'SOUND_KEENHITCEILING': 'KEENHITCEILINGSND', 'SOUND_GOTEXTRALIFE': 'GOTEXTRALIFESND',
    'SOUND_OPENGEMDOOR': 'OPENGEMDOORSND', 'SOUND_OPENSECURITYDOOR': 'OPENGEMDOORSND', 'SOUND_GOTGEM': 'GOTGEMSND',
    'SOUND_KEENFALL': 'KEENFALLSND', 'SOUND_KEENOUTOFAMMO': 'KEENOUTOFAMMOSND', 'SOUND_KEENDIE': 'KEENDIESND',
    'SOUND_KEENSHOTHIT': 'KEENSHOTHITSND', 'SOUND_STATUSDOWN': 'STATUSDOWNSND', 'SOUND_STATUSUP': 'STATUSUPSND',
    'SOUND_FLAGFLIP': 'FLAGFLIPSND', 'SOUND_FLAGLAND': 'FLAGLANDSND', 'SOUND_ENEMYSHOOT': 'SENTRYSHOOTSND',
    'SOUND_ENEMYSHOTHIT': 'SENTRYSHOTIMPACTSND', 'SOUND_KEENPADDLE': 'KEENPADDLESND', 'SOUND_PONGWALL': 'BALLBOUNCESND',
    'SOUND_COMPPADDLE': 'COMPPADDLESND', 'SOUND_COMPSCORE': 'COMPSCOREDSND', 'SOUND_KEENSCORE': 'KEENSCOREDSND',
    'SOUND_GOTKEYCARD': 'GOTEXTRALIFESND',  # item 12 is its 3-up
    # Keen 5 sounds it has no use for: anything inside its 52 sounds
    'SOUND_UNKNOWN41': 'NOWAYSND', 'SOUND_UNKNOWN52': 'GENERATOREXPLODESND', 'SOUND_UNKNOWN63': 'LEVELSTARTSND',
    'SOUND_GALAXYEXPLODE': 'GENERATOREXPLODESND', 'SOUND_GALAXYEXPLODEPRE': 'GENERATOREXPLODESND',
    'SOUND_KEENLANDONFUSE': 'LANDSND',
}

# Chunk names Omnispeak's shared code asks for -> Atroxian Realm's name (or
# number). Names missing here keep Keen 5's number until they are mapped.
CHUNKS = {
    'PIC_ARROWDIM': 47, 'PIC_ARROWBRIGHT': 48,
    'SPR_SECURITYCARD_1': 'ITEM3UP1SPR', 'SPR_BONUSSECURITYCARD': 'ITEM1UPSHADOW',  # item 12 is its 3-up
    # the "Keen enters..." box and its countdown hands (115-120 in KD_GAME.C),
    # the story scroller's background (108), the talking pictures (6+105, 6+106)
    'PIC_COUNTDOWN5': 115, 'PIC_COUNTDOWN4': 116, 'PIC_COUNTDOWN3': 117, 'PIC_COUNTDOWN2': 118,
    'PIC_COUNTDOWN1': 119, 'PIC_COUNTDOWN0': 120, 'PIC_STARWARS': 108,
    'PIC_KEENTALK1': 111, 'PIC_KEENTALK2': 112, 'DEMOSTART': 5144,
    'FON_MAINFONT': 3, 'FON_WATCHFONT': 4, 'FON_STARWARSFONT': 5,
    'PIC_HELPMENU': 'H_MENUPIC', 'PIC_HELPPOINTER': 'H_HANDPIC',
    'PIC_BORDERTOP': 'H_TOPWINDOWPIC', 'PIC_BORDERLEFT': 'H_LEFTWINDOWPIC', 'PIC_BORDERRIGHT': 'H_RIGHTWINDOWPIC',
    'PIC_BORDERBOTTOMSTATUS': 'H_BOTTOMINFOPIC', 'PIC_BORDERBOTTOM': 'H_BOTTOMWINDOWPIC',
    'PIC_MENUCARD': 'CP_MAINMENUPIC', 'PIC_NEWGAMECARD': 'CP_NEWGAMEMENUPIC', 'PIC_LOADCARD': 'CP_LOADMENUPIC',
    'PIC_SAVECARD': 'CP_SAVEMENUPIC', 'PIC_CONFIGURECARD': 'CP_CONFIGMENUPIC', 'PIC_SOUNDCARD': 'CP_SOUNDMENUPIC',
    'PIC_MUSICCARD': 'CP_MUSICMENUPIC', 'PIC_KEYBOARDCARD': 'CP_KEYBOARDMENUPIC',
    'PIC_MOVEMENTCARD': 'CP_KEYMOVEMENTPIC', 'PIC_BUTTONSCARD': 'CP_KEYBUTTONPIC',
    'PIC_JOYSTICKCARD': 'CP_JOYSTICKMENUPIC', 'PIC_OPTIONSCARD': 'CP_OPTIONSMENUPIC',
    'PIC_PADDLEWAR': 'CP_PADDLEWARPIC', 'PIC_QUIT': 'CP_QUITPIC', 'PIC_JOYSTICK': 'CP_JOYSTICKPIC',
    'PIC_WRISTWATCH': 'CP_MENUSCREENPIC', 'MPIC_WRISTWATCHSCREEN': 'CP_MENUMASKPICM',
    'PIC_CREDIT1': 'I_CREDITS1', 'PIC_CREDIT2': 'I_CREDITS2', 'PIC_CREDIT3': 'I_CREDITS3', 'PIC_CREDIT4': 'I_CREDITS4',
    # the status window's two side pictures: the engine's "left" one (at x 40)
    # is its top picture, the engine's "right" one (at x 136) its left picture
    'PIC_TITLESCREEN': 'I_TITLEPIC', 'MPIC_STATUSLEFT': 'MPIC_STATUSTOP', 'MPIC_STATUSRIGHT': 'MPIC_STATUSLEFT',
    'SPR_PADDLE': 'PADDLESPR', 'SPR_BALL0': 'BALLSPR', 'SPR_BALL1': 'BALL1PIXELTOTHERIGHTSPR',
    'SPR_BALL3': 138, 'SPR_DEMOSIGN': 'DEMOSIGNSPR', 'SPR_SCOREBOX': 'SCOREBOXSPR',
    'SPR_STARS1': 'STUNNEDSTARS1SPR', 'SPR_VIVASPLASH1': 'VITALINBURST1SPR',
    'SPR_MAPKEEN_WALK1_W': 'WORLDKEENL1SPR', 'SPR_MAPKEEN_WALK1_E': 'WORLDKEENR1SPR',
    'SPR_MAPKEEN_WALK1_N': 'WORLDKEENU1SPR', 'SPR_MAPKEEN_WALK1_S': 'WORLDKEEND1SPR',
    'SPR_MAPKEEN_WALK1_SE': 'WORLDKEENDR1SPR', 'SPR_MAPKEEN_WALK1_SW': 'WORLDKEENDL1SPR',
    'SPR_MAPKEEN_WALK1_NW': 'WORLDKEENUL1SPR', 'SPR_MAPKEEN_WALK1_NE': 'WORLDKEENUR1SPR',
    'SPR_GEM_A1': 'REDGEM1SPR', 'SPR_GEM_B1': 'YELLOWGEM1SPR', 'SPR_GEM_C1': 'BLUEGEM1SPR', 'SPR_GEM_D1': 'GREENGEM1SPR',
    'SPR_100_PTS1': 'POINTS100ITEM1SPR', 'SPR_200_PTS1': 'POINTS200ITEM1SPR', 'SPR_500_PTS1': 'POINTS500ITEM1SPR',
    'SPR_1000_PTS1': 'POINTS1000ITEM1SPR', 'SPR_2000_PTS1': 'POINTS2000ITEM1SPR', 'SPR_5000_PTS1': 'POINTS5000ITEM1SPR',
    'SPR_1UP1': 'ITEM1UP1SPR', 'SPR_STUNNER1': 'ITEMSTUNNER1SPR',
    'SPR_BONUS100': 'POINTS100SHADOWSPR', 'SPR_BONUS200': 'POINTS200SHADOWSPR', 'SPR_BONUS500': 'POINTS500SHADOWSPR',
    'SPR_BONUS1000': 'POINTS1000SHADOWSPR', 'SPR_BONUS2000': 'POINTS2000SHADOWSPR',
    'SPR_BONUS5000': 'POINTS5000SHADOWSPR', 'SPR_BONUS1UP': 'POINTS1UPSHADOWSPR',
    'SPR_BONUSCLIP': 'STUNNERSHADOWSPR', 'SPR_BONUSGEM': 'GEMSHADOWSPR',
    'EXTERN_COMMANDER': 'I_INTROCOMMANDER', 'EXTERN_KEEN': 'I_INTROKEEN',
    'TEXT_HELPMENU': 'T_HELPMAIN', 'TEXT_STORY': 'T_HELPSTORY', 'TEXT_CONTROLS': 'T_HELPCONTROLS',
    'TEXT_ABOUTID': 'T_HELPABOUT', 'TEXT_END': 'T_NORMALEND', 'TEXT_ORDER': 'T_HELPORDER',
    'TEXT_SECRETEND': 'T_SECRETEND',
}


def strip(text):
    text = re.sub(r'/\*[\s\S]*?\*/', '', text)
    return re.sub(r'//.*', '', text)


def read(src, name):
    return strip(open(os.path.join(src, name), encoding='latin1').read())


def defines(text):
    return {m.group(1): int(m.group(2)) for m in re.finditer(r'#define\s+(\w+)\s+(\d+)\b', text)}


def enum_values(text):
    """name -> number for an enum body where some entries are `NAME=n`."""
    out, cur = {}, -1
    for m in re.finditer(r'\b([A-Za-z_]\w*)\s*(?:=\s*(\w+))?\s*(?:,|$)', text.strip()):
        name, val = m.group(1), m.group(2)
        if val is not None:
            cur = int(val, 0) if val[0].isdigit() else out[val]
        else:
            cur += 1
        out[name] = cur
    return out


def main():
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    src, out, keen5, engine = sys.argv[1:]
    have = set()
    for name in os.listdir(engine):
        if name.endswith('.c'):
            have.update(re.findall(r'Add(?:Col)?Function\("(\w+)"', open(os.path.join(engine, name), encoding='latin1').read()))
    gh = read(src, 'GRAPHCKD.H')
    gdef = defines(gh)
    chunks = enum_values(gh[gh.index('typedef enum'):gh.index('}', gh.index('typedef enum'))])
    chunks.update({k: v for k, v in gdef.items() if k.endswith(('_START', '_END'))})
    ah = read(src, 'AUDIOCKD.H')
    adef = defines(ah)

    # --- binary info files (see ca_gfxinfo, ca_audinfo in id_ca.h)
    g = gdef
    gfx = [g['NUMTILE8'], g['NUMTILE8M'], g['NUMTILE16'], g['NUMTILE16M'], g['NUMTILE32'], g['NUMTILE32M'],
           g['STARTTILE8'], g['STARTTILE8M'], g['STARTTILE16'], g['STARTTILE16M'], g['STARTTILE32'], g['STARTTILE32M'],
           g['STARTPICM'] - g['STARTPICS'], g['NUMPICM'], g['NUMSPRITES'],
           g['STARTPICS'], g['STARTPICM'], g['STARTSPRITES'],
           g['STRUCTPIC'], g['STRUCTPICM'], g['STRUCTSPRITE'],
           g['NUMCHUNKS'] - g['STARTEXTERNS'], g['STARTEXTERNS']]
    open(os.path.join(out, 'GFXINFOE.CKD'), 'wb').write(struct.pack('<%dH' % len(gfx), *gfx))
    sounds, sndchunks, music = adef['NUMSOUNDS'], adef['NUMSNDCHUNKS'], adef['STARTMUSIC']
    aud = [sndchunks - music, sounds, sndchunks, 0, sounds, 2 * sounds, music]
    open(os.path.join(out, 'AUDINFOE.CKD'), 'wb').write(struct.pack('<7H', *aud))

    # --- names for the ported C code (omnispeak/src/kw_ar_*.c)
    kd = read(src, 'KD_DEF.H')
    types = enum_values(kd[kd.index('typedef enum CK_ClassType'):kd.index('} CK_ClassType')])
    snd = enum_values(ah[ah.index('typedef enum'):ah.index('soundnames')])
    with open(os.path.join(engine, 'kw_arealm_defs.h'), 'w') as f:
        f.write('// Atroxian Realm: its names for graphics chunks, sounds, object types and\n'
                '// sprite caches, from GRAPHCKD.H, AUDIOCKD.H, KD_DEF.H and AR_MISC.C of its\n'
                '// source. Generated by tools/kw_arealm.py - do not edit by hand.\n'
                '#ifndef KW_AREALM_DEFS_H\n#define KW_AREALM_DEFS_H\n\nenum\n{\n')
        for name, val in chunks.items():
            if not name.endswith(('_START', '_END')):
                f.write('\t%s = %d,\n' % (name, val))
        f.write('};\n\nenum\n{\n')
        for name, val in snd.items():
            f.write('\t%s = %d,\n' % (name, val))
        f.write('};\n\n// Songs: StartMusic() takes these (its level number is its song number)\nenum\n{\n')
        mus = ah[ah.index('STARTMUSIC'):]
        for i, name in enumerate(re.findall(r'\b(\w+_MUS)\b', mus[mus.index('typedef enum'):mus.index('LASTMUSIC')])):
            f.write('\t%s = %d,\n' % (name, i))
        f.write('};\n\n// Object types (0-8 are the engine\'s own, with the same numbers)\nenum\n{\n')
        for name, val in types.items():
            if val >= 9:
                f.write('\tAR_%s = %d,\n' % (name, val))
        f.write('};\n\n#endif\n')

    # --- every chunk name
    with open(os.path.join(out, 'ARGFX.CKD'), 'w') as f:
        f.write('# Atroxian Realm: graphics chunk names (GRAPHCKD.H). Written by tools/kw_arealm.py.\n')
        for name, val in chunks.items():
            f.write('%%int %s %d\n' % (name, val))

    # --- actions
    k5 = {}
    for line in open(os.path.join(keen5, 'ACTION.CK5'), encoding='latin1'):
        t = line.split()
        if len(t) >= 15 and t[0] == '%action':
            k5[t[1]] = int(t[2], 0)
    actions, order = {}, []
    for name in ACTION_FILES:
        text = read(src, name)
        for m in re.finditer(r'\bCK_action\s+(\w+)\s*=\s*\{([^}]*)\}', text):
            body = re.sub(r'\(\s*void\s*\(\*\)\s*\([^)]*\)\s*\)', '', m.group(2))  # function casts
            fields = [x.strip() for x in body.split(',')]
            if fields and fields[-1] == '':
                fields.pop()
            if len(fields) != 12:
                sys.exit('%s: action %s has %d fields' % (name, m.group(1), len(fields)))
            if m.group(1) not in actions:
                order.append(m.group(1))
            actions[m.group(1)] = fields

    def num(tok, what):
        tok = tok.replace('(', '').replace(')', '')
        tok = {'false': '0', 'true': '1', 'NULL': '0'}.get(tok, tok)
        if re.fullmatch(r'-?(0x[0-9A-Fa-f]+|\d+)', tok):
            return int(tok, 0)
        if tok in chunks:
            return chunks[tok]
        m = re.fullmatch(r'(\w+)\s*([+-])\s*(\d+)', tok)
        if m and m.group(1) in chunks:
            return chunks[m.group(1)] + (int(m.group(3)) if m.group(2) == '+' else -int(m.group(3)))
        sys.exit('cannot read %s "%s"' % (what, tok))

    missing = set()

    def fn(tok):
        tok = tok.lstrip('&')
        if tok in ('NULL', '0'):
            return 'NULL'
        tok = RENAMES.get(tok, tok)
        if tok not in have:
            missing.add(tok)
            return 'NULL'
        return tok

    funcs = set()
    nextptr = 0x4000
    with open(os.path.join(out, 'ARACTION.CKD'), 'w') as f:
        f.write('# Atroxian Realm: sprite actions, from the CK_action tables of its source.\n'
                '# Written by tools/kw_arealm.py. The second value only has to be unique\n'
                '# (saved games name actions by it); Keen 5 names keep Keen 5\'s.\n')
        for name in order:
            a = actions[name]
            if name in k5:
                ptr = k5[name]
            else:
                ptr, nextptr = nextptr, nextptr + 30
            nxt = a[11].lstrip('&')
            nxt = 'NULL' if nxt in ('NULL', '0') else nxt
            if nxt != 'NULL' and nxt not in actions:
                sys.exit('%s: next action %s is not defined' % (name, nxt))
            funcs.update(x for x in (fn(a[8]), fn(a[9]), fn(a[10])) if x != 'NULL')
            f.write('%%action %s 0x%04X %d %d %s %d %d %d %d %d %s %s %s %s\n' % (
                name, ptr, num(a[0], 'sprite'), num(a[1], 'sprite'), TYPES[a[2]],
                num(a[3], 'flag'), num(a[4], 'flag'), num(a[5], 'timer'), num(a[6], 'speed'), num(a[7], 'speed'),
                fn(a[8]), fn(a[9]), fn(a[10]), nxt))

    with open(os.path.join(out, 'ARACTION.CKD'), 'a') as f:
        f.write('\n# The engine\'s own names for some of these\n')
        for name, target in ACTION_ALIASES.items():
            if target not in actions:
                sys.exit('alias target %s is not an action' % target)
            f.write('%%action %s = %s\n' % (name, target))

    # --- the world's variable file
    maphead = open(os.path.join(out, 'maphead.ckd'), 'rb').read()
    maps = open(os.path.join(out, 'maptemp.ckd'), 'rb').read()
    names = []
    for i in range(100):
        off = struct.unpack_from('<i', maphead, 2 + 4 * i)[0]
        if off <= 0:
            break
        names.append(maps[off + 22:off + 38].split(b'\0')[0].decode('latin1'))
    with open(os.path.join(out, 'AREALM.CKD'), 'w') as f:
        f.write('# Atroxian Realm (Gridlock, 2016; v1.5) as a Keen Worlds world. Written by\n'
                '# tools/kw_arealm.py from the game\'s GPL source; the data files beside it are\n'
                '# the player\'s copy of the game. It runs on the engine\'s Keen 5 code, which\n'
                '# its own source was written from, so Keen 5\'s variables are the base.\n\n')
        f.write('%string ca_mapFile "maptemp.ckd"\n%string ca_mapHead "maphead.ckd"\n'
                '%string ca_tileInfo "TILEINFO.CKD" # none: the tile properties are in maphead.ckd\n'
                '%string ca_gfxInfoFile "GFXINFOE.CKD"\n%string ca_graphDict "EGADICT.CKD"\n'
                '%string ca_graphHead "EGAHEAD.CKD"\n%string ca_graphFile "EGAGRAPH.CKD"\n'
                '%string ca_audioInfoFile "AUDINFOE.CKD"\n%string ca_audioDict "AUDIODCT.CKD"\n'
                '%int ca_audioUncompressed 1\n%string ca_audioFile "AUDIOT.CKD"\n%string ca_audioHead "AUDIOHED.CKD"\n\n')
        f.write('%%int CK_highScoreLevel %d\n%%int ck5_mapElevators 0\n' % names.index('HighScores'))
        f.write('%int ck_minDoneLevel 1\n%int ck_maxDoneLevel 25 # levels the map marks as done\n\n')
        f.write('%stringarray ck_str_levelNames\n' + ',\n'.join('\t"%s"' % n for n in names) + '\n\n')
        # Its own "Keen enters" lines (AR_MISC.C), in the order of its table.
        misc = open(os.path.join(src, 'AR_MISC.C'), errors='replace').read()
        texts = dict(re.findall(r'char (ck_levelEntryText_\w+)\[\] =\s*"((?:[^"\\]|\\.)*)";', misc))
        table = re.search(r'char \*ck_levelEntryTexts\[\] = \{(.*?)\};', misc, re.S).group(1)
        entry = [texts[n] for n in re.findall(r'ck_levelEntryText_\w+', table)]
        entry += ['Keen enters\\n%s' % n for n in names[len(entry):]]
        f.write('%stringarray ck_str_levelEntryText\n' + ',\n'.join('\t"%s"' % t for t in entry) + '\n\n')
        # Its story (ck_starWarsText in KD_INTER.C)
        inter = read(src, 'KD_INTER.C')
        body = re.search(r'ck_starWarsText\[\]\s*=(.*?);', inter, re.S).group(1)
        story = ''.join(m.group(1) for m in re.finditer(r'"((?:[^"\\]|\\.)*)"', body))
        story = story.replace('\\n', '\n').replace('\\"', '"')
        f.write(story_var(story.rstrip('\n').split('\n')))
        f.write('%%string ar_str_entryMapFinal "%s"\n%%string ar_str_entryCrashReturn "%s"\n\n'
                % (texts['ck_levelEntryText_0b'], texts['ck_levelEntryText_25b']))
        # Its songs are in level order: level n plays song n; later ones are for texts.
        f.write('%intarray CK_LevelMusic ' + ', '.join(str(i) for i in range(aud[0])) + '\n')
        f.write('%%int CK_NumMusicLevels %d\n\n' % aud[0])
        f.write('# Chunk names the engine\'s shared code uses, as Atroxian Realm numbers them\n')
        for name, val in CHUNKS.items():
            f.write('%%int %s %d\n' % (name, chunks[val] if isinstance(val, str) else val))
        # Sprite caches ("lumps"), in the order of its own table
        misc = read(src, 'AR_MISC.C')
        lumps = sorted(((int(m.group(2)), m.group(1)) for m in re.finditer(r'#define\s+(\w+LUMP)\s+(\d+)', misc)))
        def table(name):
            body = re.search(r'int\s+%s\[\]\s*=\s*\{([^}]*)\}' % name, misc).group(1)
            return [chunks[x.strip()] for x in body.split(',') if x.strip()]
        starts, ends = table('ck_lumpStarts'), table('ck_lumpEnds')
        assert len(starts) == len(ends) == len(lumps), (len(starts), len(ends), len(lumps))
        f.write('\n# Sounds the engine\'s shared code plays, as Atroxian Realm numbers them\n')
        for name, val in SOUNDS.items():
            f.write('%%int %s %d\n' % (name, snd[val]))
        items = ['GOTGEMSND'] * 4 + ['GOTITEMSND'] * 6 + ['GOTEXTRALIFESND', 'GOTSTUNNERSND', 'GOTEXTRALIFESND']
        f.write('%intarray ck_itemSounds ' + ', '.join(str(snd[x]) for x in items) + '\n')
        f.write('\n# Sprite caches (ck_lumpStarts/ck_lumpEnds in AR_MISC.C)\n%%int NUMLUMPS %d\n' % len(lumps))
        for i, name in lumps:
            f.write('%%int %s %d\n' % (name, i))
        f.write('%int LUMP_KEEN 0\n%int LUMP_MAPKEEN 1\n')
        f.write('%intarray lumpStarts ' + ', '.join(map(str, starts)) + '\n')
        f.write('%intarray lumpEnds ' + ', '.join(map(str, ends)) + '\n')
        f.write('%intarray ck_itemLumps ' + ', '.join('0' for _ in range(13)) + ' # items are in Keen\'s cache\n')
        f.write('\n%include "ARGFX.CKD"\n%override "EPISODE.CK5"\n\n'
                '# After Keen 5\'s actions: Atroxian Realm\'s replace those of the same name.\n'
                '%include "ARACTION.CKD"\n')
    print('%d actions, %d chunk names, %d maps, %d songs' % (len(order), len(chunks), len(names), aud[0]))
    print('functions named by actions: %d, still to port (NULL for now): %d' % (len(funcs) + len(missing), len(missing)))
    print(' '.join(sorted(missing)))


if __name__ == '__main__':
    main()
