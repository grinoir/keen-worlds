"""Keen Dreams' game code and sprite states, for Omnispeak (Keen Worlds).

Keen Dreams (id Software 1991; source released under the GPL in 2014) is the
ancestor of the Keen 4-6 engine Omnispeak rebuilds: its objects, states and
wall clipping are the same ideas under older names. This writes, from its
source:

  play/mods/dreams/KDACTION.KDR   every sprite state, as an action
  play/mods/dreams/DREAMS.KDR     the world's variable file
  omnispeak/src/kw_dreams_defs.h  its chunk, sound and object-type names

and, when a fourth folder is given, a first-pass conversion of KD_KEEN.C,
KD_ACT1.C and KD_ACT2.C to Omnispeak's names (kw_kd_keen.c, kw_kd_act1.c,
kw_kd_act2.c). Those are a starting point for the hand port in
omnispeak/src, not finished files: don't write them over the ported ones.

Usage:
  python3 tools/kw_dreams_port.py KDSRC play/mods/dreams omnispeak/src [OUT]

The data files in play/mods/dreams come from tools/kw_dreams.py.
"""
import os, re, struct, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_fitf import strip, header_values, story_var

GAME_FILES = ['kd_keen.c', 'kd_act1.c', 'kd_act2.c']
PROGRESS = {'step': 'UnscaledOnce', 'slide': 'ScaledOnce', 'think': 'Frame',
            'stepthink': 'UnscaledFrame', 'slidethink': 'ScaledFrame'}

# Its object classes. Keen is the engine's CT_Player (2); the rest are numbered
# from 40 so that none is taken for one of the engine's own types.
CLASSES = ['nothing', 'keenobj', 'powerobj', 'doorobj', 'bonusobj', 'broccoobj', 'tomatobj', 'carrotobj',
           'celeryobj', 'asparobj', 'grapeobj', 'taterobj', 'cartobj', 'frenchyobj', 'melonobj', 'turnipobj',
           'cauliobj', 'brusselobj', 'mushroomobj', 'squashobj', 'apelobj', 'peapodobj', 'peabrainobj',
           'boobusobj', 'shotobj', 'inertobj']

FIELDS = {  # objtype -> CK_object
    'obclass': 'type', 'needtoreact': 'visible', 'needtoclip': 'clipped', 'nothink': 'timeUntillThink',
    'x': 'posX', 'y': 'posY', 'xdir': 'xDirection', 'ydir': 'yDirection', 'xmove': 'deltaPosX', 'ymove': 'deltaPosY',
    'xspeed': 'velX', 'yspeed': 'velY', 'ticcount': 'actionTimer', 'state': 'currentAction', 'shapenum': 'gfxChunk',
    'left': 'clipRects.unitX1', 'top': 'clipRects.unitY1', 'right': 'clipRects.unitX2',
    'bottom': 'clipRects.unitY2', 'midx': 'clipRects.unitXmid', 'tileleft': 'clipRects.tileX1',
    'tiletop': 'clipRects.tileY1', 'tileright': 'clipRects.tileX2', 'tilebottom': 'clipRects.tileY2',
    'tilemidx': 'clipRects.tileXmid', 'hitnorth': 'topTI', 'hitsouth': 'bottomTI', 'hiteast': 'rightTI',
    'hitwest': 'leftTI', 'sprite': 'sde', 'temp1': 'user1', 'temp2': 'user2', 'temp3': 'user3', 'temp4': 'user4',
    # statetype -> CK_action
    'leftshapenum': 'chunkLeft', 'rightshapenum': 'chunkRight', 'think': 'think', 'contact': 'collide',
    'react': 'draw', 'tictime': 'timer', 'nextstate': 'next', 'pushtofloor': 'stickToGround',
    'skippable': 'protectAnimation',
}
WORDS = {
    'objtype': 'CK_object', 'statetype': 'CK_action', 'boolean': 'bool', 'tics': 'SD_GetSpriteSync()',
    'player': 'ck_keenObj', 'new': 'kd_new', 'GetNewObj': 'KD_GetNewObj', 'NewState': 'KD_NewState',
    'ChangeState': 'CK_SetAction2', 'RemoveObj': 'CK_RemoveObj', 'GivePoints': 'CK_IncreaseScore',
    'RF_PlaceSprite': 'RF_AddSpriteDraw', 'RF_RemoveSprite': 'RF_RemoveSpriteDraw',
    'mapwidth': 'CA_GetMapWidth()', 'mapheight': 'CA_GetMapHeight()', 'mapon': 'ca_mapOn',  # (the map now loaded; gamestate.mapon is the one to load next)
    'spritedraw': 'false', 'maskdraw': 'true',
    'word': 'uint16_t', 'byte': 'uint8_t', 'register': '', 'near': '', 'far': '', 'pascal': '', 'huge': '',
    '_seg': '',
    'yes': 'OBJ_ACTIVE', 'no': 'OBJ_INACTIVE', 'allways': 'OBJ_ALWAYS_ACTIVE', 'removable': 'OBJ_EXISTS_ONLY_ONSCREEN',
    'PIXGLOBAL': '16', 'TILEGLOBAL': '256', 'G_T_SHIFT': '8', 'G_P_SHIFT': '4', 'TickBase': '70',
    'ClipToWalls': 'CK_PhysUpdateNormalObj', 'ClipToSpriteSide': 'CK_PhysPushX', 'ClipToSprite': 'CK_PhysPushXY',
    'CalcBounds': 'CK_ResetClipRects',
    'gd_Easy': 'D_Easy', 'gd_Normal': 'D_Normal', 'gd_Hard': 'D_Hard',
    'lasttimecount': 'SD_GetLastTimeCount()', 'TimeCount': 'SD_GetTimeCount()',
    'godmode': 'ck_godMode', 'jumpcheat': 'ck_gameState.jumpCheat',
    'VW_UpdateScreen': 'VH_UpdateScreen', 'VW_WaitVBL': 'VL_DelayTics', 'IN_Ack': 'IN_WaitButton',
    'VW_FixRefreshBuffer': 'VL_FixRefreshBuffer',
    'originxglobal': 'rf_scrollXUnit', 'originyglobal': 'rf_scrollYUnit',
    'originxtile': 'KD_ORIGINXTILE', 'originytile': 'KD_ORIGINYTILE',
    'originxtilemax': 'KD_ORIGINXTILEMAX', 'originytilemax': 'KD_ORIGINYTILEMAX',
    'dir_North': 'IN_dir_North', 'dir_East': 'IN_dir_East', 'dir_South': 'IN_dir_South', 'dir_West': 'IN_dir_West',
    'dir_None': 'IN_dir_None', 'button0held': 'kd_button0held', 'button1held': 'kd_button1held',
    'ABS': 'abs', 'singlegravity': 'kd_singlegravity', 'bounceangle': 'kd_bounceangle', 'inline': 'inLine',
}
GAMESTATE = {
    'mapon': 'ck_gameState.currentLevel', 'flowerpowers': 'ck_gameState.numShots', 'score': 'ck_gameState.keenScore',
    'lives': 'ck_gameState.numLives', 'leveldone': 'ck_gameState.levelsDone', 'nextextra': 'ck_gameState.nextKeenAt',
    'difficulty': 'ck_gameState.difficulty', 'worldx': 'ck_gameState.mapPosX', 'worldy': 'ck_gameState.mapPosY',
}
PLAYSTATE = {'died': 'LS_Died', 'levelcomplete': 'LS_LevelComplete', 'victorious': 'KD_LS_Victorious'}

# Chunk names the engine's shared code asks for -> Keen Dreams' (it has one
# font, and none of the Keen 4 menu pictures)
CHUNKS = {
    'FON_MAINFONT': 3, 'FON_WATCHFONT': 3, 'FON_STARWARSFONT': 3,
    'SPR_SCOREBOX': 'SCOREBOXSPR',
}
SOUNDS = {
    'SOUND_KEENWALK0': 'KEENWALK1SND', 'SOUND_KEENWALK1': 'KEENWALK2SND', 'SOUND_KEENJUMP': 'JUMPSND',
    'SOUND_KEENLAND': 'LANDSND', 'SOUND_KEENSHOOT': 'THROWSND', 'SOUND_GOTITEM': 'GETPOINTSSND',
    'SOUND_GOTSTUNNER': 'GETPOWERSND', 'SOUND_UNKNOWN12': 'ENTERLEVELSND', 'SOUND_LEVELEXIT': 'LEVELDONESND',
    'SOUND_KEENHITCEILING': 'HITHEADSND', 'SOUND_NEEDKEYCARD': 'NOWAYSND', 'SOUND_KEENDIE': 'WAKEUPSND',
    'SOUND_GOTEXTRALIFE': 'EXTRAKEENSND', 'SOUND_KEENFALL': 'PLUMMETSND',
}


def action_name(state):
    return 'KD_ACT_' + (state[2:] if state.startswith('s_') else state)


def source(src, name):
    for cand in (name, name.upper()):
        p = os.path.join(src, cand)
        if os.path.exists(p):
            return open(p, encoding='latin1').read().replace('\r', '')
    sys.exit('%s is not in %s' % (name, src))


def game_functions(src):
    names = set()
    for f in GAME_FILES:
        text = strip(source(src, f))
        names.update(re.findall(r'^[A-Za-z_][\w \t\*]*?\b(\w+)\s*\([^;{}]*\)\s*\n\{', text, re.M))
    return names


def read_states(src):
    states, order = {}, []
    for name in GAME_FILES:
        text = strip(source(src, name))
        for m in re.finditer(r'\bstatetype\s+(?:far\s+)?(\w+)\s*=\s*\{([^}]*)\}', text):
            fields = [x.strip() for x in m.group(2).split(',')]
            if fields and fields[-1] == '':
                fields.pop()
            if len(fields) != 12:
                sys.exit('%s: state %s has %d fields' % (name, m.group(1), len(fields)))
            if m.group(1) == 's_score':
                continue  # the score box is the engine's own
            if m.group(1) not in states:
                order.append(m.group(1))
            states[m.group(1)] = fields
    return states, order


def write_data(src, out, engine):
    chunks = header_values(os.path.join(src, 'graphkdr.h'))
    audio = header_values(os.path.join(src, 'audiokdr.h'))
    sndnames = [n for n in audio if n.endswith('SND')]
    funcs = game_functions(src)

    with open(os.path.join(engine, 'kw_dreams_defs.h'), 'w') as f:
        f.write('// Keen Dreams: its names for graphics chunks, sounds and object types, from\n'
                '// GRAPHKDR.H, AUDIOKDR.H and KD_DEF.H of its source. Generated by\n'
                '// tools/kw_dreams_port.py - do not edit by hand.\n'
                '#ifndef KW_DREAMS_DEFS_H\n#define KW_DREAMS_DEFS_H\n\nenum\n{\n')
        for name, val in chunks.items():
            f.write('\tKD_%s = %d,\n' % (name, val))
        f.write('};\n\nenum\n{\n')
        for name in sndnames:
            f.write('\tKD_%s = %d,\n' % (name, audio[name]))
        f.write('};\n\n// Object types: Keen is the engine\'s CT_Player, the others are numbered\n'
                '// clear of the engine\'s own.\nenum\n{\n\tKD_nothing = 0,\n\tKD_keenobj = 2,\n')
        for i, name in enumerate(CLASSES[2:]):
            f.write('\tKD_%s = %d,\n' % (name, 40 + i))
        f.write('};\n\n#endif\n')

    # --- states -> actions
    states, order = read_states(src)
    consts = {}
    for name in GAME_FILES + ['kd_def.h']:
        for m in re.finditer(r'#define[ \t]+(\w+)[ \t]+([^\n]+)', strip(source(src, name))):
            consts.setdefault(m.group(1), m.group(2).strip())

    def num(tok, what, depth=0):
        def value(m):
            n = m.group(0)
            if n in ('false', 'NULL'):
                return '0'
            if n == 'true':
                return '1'
            if n in chunks:
                return str(chunks[n])
            if n in consts and depth < 8:
                return str(num(consts[n], what, depth + 1))
            sys.exit('cannot read %s "%s"' % (what, tok))
        expr = re.sub(r'\b[A-Za-z_]\w*\b', value, tok)
        if not re.fullmatch(r'[\d\sxXa-fA-F+\-*/()]+', expr):
            sys.exit('cannot read %s "%s"' % (what, tok))
        return int(eval(expr.replace('/', '//')))

    used = set()

    def fn(tok):
        tok = tok.lstrip('&').strip()
        if tok in ('NULL', '0'):
            return 'NULL'
        if tok not in funcs:
            sys.exit('state function %s is not defined in the game files' % tok)
        used.add(tok)
        return 'KD_' + tok

    with open(os.path.join(out, 'KDACTION.KDR'), 'w') as f:
        f.write('# Keen Dreams: sprite actions, from the statetype tables of its source.\n'
                '# Written by tools/kw_dreams_port.py. The second value only has to be\n'
                '# unique (saved games name actions by it).\n')
        for i, s in enumerate(order):
            a = states[s]
            nxt = a[11].lstrip('&').strip()
            if nxt in ('NULL', '0'):
                nxt = 'NULL'
            elif nxt not in states:
                sys.exit('%s: next state %s is not defined' % (s, nxt))
            else:
                nxt = action_name(nxt)
            f.write('%%action %s 0x%04X %d %d %s %d %d %d %d %d %s %s %s %s\n' % (
                action_name(s), 0x7000 + 30 * i, num(a[0], 'sprite'), num(a[1], 'sprite'), PROGRESS[a[2]],
                num(a[3], 'flag'), num(a[4], 'flag'), num(a[5], 'timer'), num(a[6], 'speed'), num(a[7], 'speed'),
                fn(a[8]), fn(a[9]), fn(a[10]), nxt))

    # --- the world's variable file
    maphead = open(os.path.join(out, 'MAPHEAD.KDR'), 'rb').read()
    maps = open(os.path.join(out, 'GAMEMAPS.KDR'), 'rb').read()
    levels = []
    for i in range(100):
        off = struct.unpack_from('<i', maphead, 2 + 4 * i)[0]
        levels.append(maps[off + 22:off + 38].split(b'\0')[0].decode('latin1') if off > 0 else '')
    while levels and not levels[-1]:
        levels.pop()
    # Its own level names (levelnames[] of KD_PLAY.C) read better than the
    # map headers' capitals.
    m = re.search(r'levelnames\[\w*\]\s*=\s*\{([^}]*)\}', strip(source(src, 'kd_play.c')))
    own = re.findall(r'"([^"]*)"', m.group(1)) if m else []
    names = [own[i] if i < len(own) and own[i] else n.title() for i, n in enumerate(levels)]
    with open(os.path.join(out, 'DREAMS.KDR'), 'w') as f:
        f.write('# Keen Dreams as a Keen Worlds world: the dream Keen has in the Megarocket\'s\n'
                '# bed. Written by tools/kw_dreams_port.py from the game\'s GPL source; the\n'
                '# data files beside it are converted from the player\'s copy of the game by\n'
                '# tools/kw_dreams.py. It runs on the engine\'s Keen 4 code - its descendant -\n'
                '# so Keen 4\'s variables are the base.\n\n')
        f.write('%string ca_mapFile "GAMEMAPS.KDR"\n%string ca_mapHead "MAPHEAD.KDR"\n'
                '%string ca_tileInfo "TILEINFO.KDR" # none: the tile properties are in MAPHEAD.KDR\n'
                '%string ca_gfxInfoFile "GFXINFOE.KDR"\n%string ca_graphDict "EGADICT.KDR"\n'
                '%string ca_graphHead "EGAHEAD.KDR"\n%string ca_graphFile "EGAGRAPH.KDR"\n'
                '%string ca_audioInfoFile "AUDINFOE.KDR"\n%string ca_audioDict "AUDIODCT.KDR"\n'
                '%string ca_audioFile "AUDIO.KDR"\n%string ca_audioHead "AUDIOHHD.KDR"\n\n')
        f.write('# The score box: its digits are small tiles 1-10 after a blank one\n'
                '%int ck_scoreBoxBlankTile 0\n%int ck_scoreBoxDigitOffset 47\n'
                '%int ck_scoreBoxScoreDigits 7\n%int ck_scoreBoxShotsColumn 6\n%int ck_scoreBoxLivesColumn 1\n\n')
        f.write('%stringarray ck_str_levelNames\n' + ',\n'.join('\t"%s"' % n for n in names) + '\n\n')
        f.write('%stringarray ck_str_levelEntryText\n' + ',\n'.join('\t"Keen dreams of\\n%s"' % n for n in names) + '\n\n')
        f.write('%string ck_str_tryAgainIntro "You didn\'t make it past"\n%string ck_str_tryAgainTryAgain "Try Again"\n'
                '%string ck_str_exitToMap "Exit to Tuberia"\n\n')
        f.write('%int CK_NumMusicLevels 0 # it has no music\n\n')
        f.write('# Chunk names the engine\'s shared code uses, as Keen Dreams numbers them\n')
        for name, val in CHUNKS.items():
            f.write('%%int %s %d\n' % (name, chunks[val] if isinstance(val, str) else val))
        f.write('\n# Sounds the engine\'s shared code plays, as Keen Dreams numbers them\n')
        for name, val in SOUNDS.items():
            f.write('%%int %s %d\n' % (name, audio[val]))
        f.write('\n%override "EPISODE.CK4"\n\n# After Keen 4\'s actions\n%include "KDACTION.KDR"\n')
    print('%d states, %d state functions, %d chunk names, %d levels' % (len(order), len(used), len(chunks), len(levels)))
    return chunks, audio, funcs, set(order)


def convert(src, name, outdir, chunks, audio, funcs, states):
    t = source(src, name)
    # state tables and their declarations go (they are in KDACTION.KDR)
    t = re.sub(r'^(?:extern\s+)?statetype\s+(?:far\s+)?\w+\s*(?:=\s*\{[^}]*\})?\s*;[^\n]*\n', '', t, flags=re.M)
    t = re.sub(r'#include\s+"[^"]*"\s*\n', '', t)
    t = re.sub(r'#pragma[^\n]*\n', '', t)
    # the map: a plane is an array of tiles, and its row table held byte offsets
    t = re.sub(r'\btinf\s*\[\s*(\w+)\s*\+\s*([^\]]+)\]', r'KD_\1(\2)', t)
    t = re.sub(r'\(\s*unsigned\s+(?:_seg|far)\s*\*\s*\)\s*mapsegs', 'mapsegs', t)
    t = re.sub(r'\bmapsegs\s*\[\s*(\d)\s*\]', r'CA_mapPlanes[\1]', t)
    t = re.sub(r'\bmapbwidthtable\s*\[([^\]]+)\]\s*/\s*2', r'((\1)*CA_GetMapWidth())', t)
    t = re.sub(r'\bgamestate\s*\.\s*(\w+)', lambda m: GAMESTATE.get(m.group(1), 'kd_state.' + m.group(1)), t)
    t = re.sub(r'(?<![\w.>])c\s*\.\s*(xaxis|yaxis|dir|button0|button1)\b', r'kd_c.\1', t)
    t = re.sub(r'\bplaystate\s*=\s*(\w+)', lambda m: 'ck_gameState.levelState = (CK_LevelState)' + PLAYSTATE[m.group(1)], t)
    # a state's own fields, read through its name: s_keenwalk1.xmove
    sfield = {'xmove': 'velX', 'ymove': 'velY'}
    t = re.sub(r'(?<!&)\b(s_\w+)\s*\.\s*(\w+)',
               lambda m: 'CK_ACTION(%s)->%s' % (action_name(m.group(1)), sfield.get(m.group(2), FIELDS.get(m.group(2), m.group(2)))), t)
    t = re.sub(r'&\s*(s_\w+)', lambda m: 'CK_ACTION(%s)' % action_name(m.group(1)), t)
    # a state's movement per tic is the action's velocity
    t = re.sub(r'->\s*state\s*->\s*(xmove|ymove)\b', lambda m: '->currentAction->' + sfield[m.group(1)], t)
    t = re.sub(r'(->|\.)(\w+)\b(?!\s*\()', lambda m: m.group(1) + FIELDS.get(m.group(2), m.group(2)), t)
    # What an object is about to move: its xmove and ymove while it thinks, the
    # engine's ck_nextX and ck_nextY. (Read back afterwards, in a react or
    # contact function, they are how far it did move: deltaPosX, deltaPosY.)
    t = re.sub(r'\b\w+->deltaPos([XY])(\s*(?:[+-]?=)(?!=))', r'ck_next\1\2', t)
    # booleans that are enums in the engine
    t = re.sub(r'->clipped\s*=\s*(true|1)\b', '->clipped = CLIP_normal', t)
    t = re.sub(r'->clipped\s*=\s*(false|0)\b', '->clipped = CLIP_not', t)
    t = re.sub(r'->active\s*=\s*(false|0)\b', '->active = OBJ_INACTIVE', t)
    t = re.sub(r'->active\s*=\s*(true|1)\b', '->active = OBJ_ACTIVE', t)

    def word(m):
        w = m.group(0)
        if w in WORDS:
            return WORDS[w]
        if w in funcs:
            return 'KD_' + w
        if w in chunks or w in audio or w in CLASSES:
            return 'KD_' + w
        return w
    parts = re.split(r'(//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\\n])*")', t)
    for i in range(0, len(parts), 2):
        s = re.sub(r'(?<!->)(?<![.\w])[A-Za-z_]\w*\b', word, parts[i])
        s = re.sub(r'\bunsigned\s+long\b', 'uint32_t', s)
        s = re.sub(r'\bunsigned\s+int\b', 'uint16_t', s)
        s = re.sub(r'\bunsigned\b(?!\s+(char|short))', 'uint16_t', s)
        s = re.sub(r'\blong\b', 'int32_t', s)
        s = re.sub(r'\bint\b', 'int16_t', s)
        parts[i] = s
    t = ''.join(parts)
    for var in ('PrintY', 'PrintX'):
        t = re.sub(r'\b%s\s*([+-])=\s*([^;]+);' % var, r'US_Set%s(US_Get%s() \1 (\2));' % (var, var), t)
    body = strip(t)
    one = re.findall(r'^void\s+(KD_\w+)\s*\(\s*CK_object\s*\*\s*\w+\s*\)\s*\n\{', body, re.M)
    two = re.findall(r'^void\s+(KD_\w+)\s*\(\s*CK_object\s*\*\s*\w+\s*,\s*CK_object\s*\*\s*\w+\s*\)\s*\n\{', body, re.M)
    base = 'kw_' + name[:-2]
    t += '\nvoid %s_SetupFunctions(void)\n{\n' % ('KD_' + name[3:-2].capitalize())
    t += ''.join('\tCK_ACT_AddFunction("%s", &%s);\n' % (n, n) for n in sorted(set(one)))
    t += ''.join('\tCK_ACT_AddColFunction("%s", &%s);\n' % (n, n) for n in sorted(set(two)))
    t += '}\n'
    path = os.path.join(outdir, base + '.c')
    open(path, 'w').write('#include "kw_dreams.h"\n\n' + t)
    print('wrote', path, len(t.splitlines()), 'lines')


def main():
    if len(sys.argv) not in (4, 5):
        sys.exit(__doc__)
    src, out, engine = sys.argv[1:4]
    chunks, audio, funcs, states = write_data(src, out, engine)
    if len(sys.argv) == 5:
        os.makedirs(sys.argv[4], exist_ok=True)
        for name in GAME_FILES:
            convert(src, name, sys.argv[4], chunks, audio, funcs, states)


if __name__ == '__main__':
    main()
