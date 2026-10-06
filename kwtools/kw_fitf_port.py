"""First-pass conversion of Foray in the Forest's C code to Omnispeak's names.

    python3 tools/kw_fitf_port.py foray/src_v20/SOURCE/CK_ACT.C OUT.c \\
        play/mods/fitf/FFACTION.CKS omnispeak/src/kw_fitf_defs.h

Its source uses Keen Dreams names (objtype, ob->x, NewState, xtry...) for what
Omnispeak calls CK_object, obj->posX, CK_SetAction, ck_nextX. This renames
them, drops the state tables (they are in FFACTION.CKS) and prefixes its
chunk, sound and type names with FF_. The result is a starting point for a
hand port, not a finished file.
"""
import re, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_fitf import strip, action_name

FIELDS = {  # objtype -> CK_object
    'obclass': 'type', 'needtoreact': 'visible', 'needtoclip': 'clipped', 'nothink': 'timeUntillThink',
    'x': 'posX', 'y': 'posY', 'xdir': 'xDirection', 'ydir': 'yDirection', 'xmove': 'deltaPosX', 'ymove': 'deltaPosY',
    'xspeed': 'velX', 'yspeed': 'velY', 'ticcount': 'actionTimer', 'state': 'currentAction', 'shapenum': 'gfxChunk',
    'priority': 'zLayer', 'left': 'clipRects.unitX1', 'top': 'clipRects.unitY1', 'right': 'clipRects.unitX2',
    'bottom': 'clipRects.unitY2', 'midx': 'clipRects.unitXmid', 'tileleft': 'clipRects.tileX1',
    'tiletop': 'clipRects.tileY1', 'tileright': 'clipRects.tileX2', 'tilebottom': 'clipRects.tileY2',
    'tilemidx': 'clipRects.tileXmid', 'hitnorth': 'topTI', 'hitsouth': 'bottomTI', 'hiteast': 'rightTI',
    'hitwest': 'leftTI', 'sprite': 'sde', 'temp1': 'user1', 'temp2': 'user2', 'temp3': 'user3', 'temp4': 'user4',
    # statetype -> CK_action (after ->state / ->currentAction)
    'leftshapenum': 'chunkLeft', 'rightshapenum': 'chunkRight', 'think': 'think', 'contact': 'collide',
    'react': 'draw', 'timer': 'timer',
}
WORDS = {
    'objtype': 'CK_object', 'statetype': 'CK_action', 'boolean': 'bool', 'xtry': 'ck_nextX', 'ytry': 'ck_nextY',
    'tics': 'SD_GetSpriteSync()', 'player': 'ck_keenObj', 'new': 'new_object', 'gamestate': 'ck_gameState',
    'GetNewObj': 'CK_GetNewObj', 'NewState': 'CK_SetAction', 'ChangeState': 'CK_SetAction2',
    'RemoveObj': 'CK_RemoveObj',
    'RF_TileToGlobal': 'RF_TileToUnit', 'RF_GlobalToTile': 'RF_UnitToTile', 'RF_PlaceSprite': 'RF_AddSpriteDraw',
    'RF_RemoveSprite': 'RF_RemoveSpriteDraw', 'mapwidth': 'CA_GetMapWidth()', 'mapheight': 'CA_GetMapHeight()',
    'mapon': 'ca_mapOn', 'spritedraw': 'false', 'maskdraw': 'true', 'whitedraw': 'true',
    'word': 'uint16_t', 'byte': 'uint8_t', 'register': '', 'near': '', 'far': '', 'pascal': '', 'huge': '',
    'motion_Left': 'IN_motion_Left', 'motion_Right': 'IN_motion_Right', 'motion_Up': 'IN_motion_Up',
    'motion_Down': 'IN_motion_Down', 'motion_None': 'IN_motion_None',
    'ac_no': 'OBJ_INACTIVE', 'ac_yes': 'OBJ_ACTIVE', 'ac_allways': 'OBJ_ALWAYS_ACTIVE',
    'ac_removable': 'OBJ_EXISTS_ONLY_ONSCREEN', 'ABS': 'abs',
    'CT_nothing': 'CT_Nothing', 'CT_Item': 'CT5_Item', 'CT_Platform': 'CT5_Platform',
    'CT_EnemyShot': 'CT5_EnemyShot', 'CT_StunnedCreature': 'CT5_StunnedCreature', 'CT_MapFlag': 'CT5_MapFlag',
    'PIXGLOBAL': '16', 'TILEGLOBAL': '256', 'TickBase': '70',
    'OnScreen': 'CK_ObjectVisible', 'ClipToWalls': 'CK_PhysUpdateNormalObj',
    'FullClipToWalls': 'CK_PhysFullClipToWalls', 'PushObj': 'CK_PhysUpdateSimpleObj',
    'ClipToSpriteSide': 'CK_PhysPushX', 'ClipToSpriteTop': 'CK_PhysPushY', 'ClipToSprite': 'CK_PhysPushXY',
    'CalcBounds': 'CK_ResetClipRects',
    'cl_noclip': 'CLIP_not', 'cl_midclip': 'CLIP_normal', 'cl_fullclip': 'CLIP_simple',
    'gd_Easy': 'D_Easy', 'gd_Normal': 'D_Normal', 'gd_Hard': 'D_Hard',
    'lasttimecount': 'SD_GetLastTimeCount()', 'playstate': 'ck_gameState.levelState',
    'ck_mapState': 'ff_map', 'grneeded': 'ca_graphChunkNeeded',
    'stringnames': 'int', 'VW_UpdateScreen': 'VH_UpdateScreen', 'VW_WaitVBL': 'VL_DelayTics',
    'IN_Ack': 'IN_WaitButton', 'VWB_DrawPic': 'VHB_DrawBitmap', 'VW_FixRefreshBuffer': 'VL_FixRefreshBuffer',
    'VWB_Bar': 'VHB_Bar', 'grsegs': 'ca_graphChunks', 'NOWAYSND': 'FF_OLDNOWAYSND', 'screenfaded': 'vl_screenFaded',
    'graphicnums': 'int', 'RF_PixelToGlobal': 'RF_PixelToUnit', 'originxglobal': 'rf_scrollXUnit',
    'originyglobal': 'rf_scrollYUnit', 'gd_Expert': 'FF_D_Expert', 'dir_None': 'IN_dir_None',
    'keenkilled': 'ff_keenKilled', 'push_none': '0', 'push_down': '1',
}
# gamestate.x -> where the engine (or our FF_State) keeps it
GAMESTATE = {
    'mapon': 'ck_gameState.currentLevel', 'ammo': 'ck_gameState.numShots', 'score': 'ck_gameState.keenScore',
    'lives': 'ck_gameState.numLives', 'leveldone': 'ck_gameState.levelsDone', 'nextextra': 'ck_gameState.nextKeenAt',
    'drops': 'ck_gameState.numCentilife', 'difficulty': 'ck_gameState.difficulty', 'godMode': 'ck_godMode',
    'worldx': 'ck_gameState.mapPosX', 'worldy': 'ck_gameState.mapPosY',
}
CONTROL = {'xaxis': 'xDirection', 'yaxis': 'yDirection', 'dir': 'dir', 'button0': 'jump', 'button1': 'pogo'}
ENGINE = set(WORDS.values())


def game_functions(srcdir):
    """Names of the functions its game files define (not the id libraries)."""
    names = set()
    for f in sorted(os.listdir(srcdir)):
        if f.endswith('.C') and not f.startswith('ID_') and f not in ('KD_SM.C',):
            text = strip(open(os.path.join(srcdir, f), encoding='latin1').read().replace('\r', ''))
            names.update(re.findall(r'^[A-Za-z_][\w \t\*]*?\b(\w+)\s*\([^;{}]*\)\s*\n\{', text, re.M))
    return names


def ff_name(name):
    """Our name for one of its functions."""
    return 'FF_' + (name[3:] if name.startswith('CK_') else name)


def main():
    src, out, actfile, defs = sys.argv[1:5]
    t = open(src, encoding='latin1').read().replace('\r', '')
    states = set(re.findall(r'\bstatetype\s+(?:far\s+)?(\w+)\s*(?:=|;)', strip(t)))
    k5 = set(re.findall(r'^%action (\w+)', open(os.path.join(os.path.dirname(actfile), '..', '..', '..', 'omnispeak',
             'data', 'keen5', 'ACTION.CK5'), encoding='latin1').read(), re.M))
    known = set(re.findall(r'^%action (\w+)', open(actfile).read(), re.M))
    ffnames = set(re.findall(r'\bFF_(\w+) =', open(defs).read()))
    funcs = game_functions(os.path.dirname(os.path.abspath(sys.argv[5])) if len(sys.argv) > 5 else os.path.dirname(os.path.abspath(src)))
    # state tables and their declarations go
    t = re.sub(r'^(?:extern\s+)?statetype\s+(?:far\s+)?\w+\s*(?:=\s*\{[^}]*\})?\s*;[^\n]*\n', '', t, flags=re.M)
    t = re.sub(r'#include\s+"[^"]*"\s*\n', '', t)
    t = re.sub(r'#pragma[^\n]*\n', '', t)

    t = re.sub(r'\basm\s+nop\s*;', ';', t)
    t = re.sub(r'\(\s*void\s*\*\s*\*\s*\)', '', t)
    t = re.sub(r'\bgamestate\s*\.\s*(\w+)', lambda m: GAMESTATE.get(m.group(1), 'ff_state.' + m.group(1)), t)
    t = re.sub(r'(?<![\w.>])c\s*\.\s*(xaxis|yaxis|dir|button0|button1)\b', lambda m: 'ck_inputFrame.' + CONTROL[m.group(1)], t)
    t = re.sub(r'\bSM_Quit\s*\(\s*S_(\w+)\s*\)', r'Quit("Foray in the Forest: \1")', t)
    t = re.sub(r'->\s*state\s*->\s*pushtofloor', '->currentAction->stickToGround', t)

    def state_ref(m):
        name = action_name(m.group(1), k5)
        return 'CK_ACTION(%s)' % name
    t = re.sub(r'&\s*(s_\w+)', state_ref, t)
    t = re.sub(r'(->|\.)(\w+)\b(?!\s*\()', lambda m: m.group(1) + FIELDS.get(m.group(2), m.group(2)), t)

    def word(m):
        w = m.group(0)
        if w in WORDS:
            return WORDS[w]
        if w in funcs:
            return ff_name(w)
        if w in ffnames and not w.startswith('CT_'):
            return 'FF_' + w
        if w.startswith('CT_') and w in ffnames:
            return 'FF_' + w
        return w
    # not inside strings or comments
    parts = re.split(r'(//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\\n])*")', t)
    for i in range(0, len(parts), 2):
        parts[i] = re.sub(r'(?<![>.\w])[A-Za-z_]\w*\b', word, parts[i])
    t = ''.join(parts)
    # window and print variables are functions in Omnispeak
    for var in ('PrintY', 'PrintX', 'WindowW', 'WindowX', 'WindowY', 'WindowH'):
        t = re.sub(r'\b%s\s*([+-])=\s*([^;]+);' % var, r'US_Set%s(US_Get%s() \1 (\2));' % (var, var), t)
        t = re.sub(r'\b%s\s*=\s*([^;=]+);' % var, r'US_Set%s(\1);' % var, t)
        t = re.sub(r'(?<![_\w])%s\b(?!\()' % var, 'US_Get%s()' % var, t)
    t = re.sub(r'\bfontnumber\s*=\s*([^;=]+);', r'US_SetPrintFont(\1);', t)
    t = re.sub(r'\bfontnumber\b', 'US_GetPrintFont()', t)
    t = re.sub(r'\bfontcolor\s*=\s*([^;=]+);', r'US_SetPrintColour(\1);', t)
    t = re.sub(r'\bfontcolor\b', 'US_GetPrintColour()', t)
    t = re.sub(r'\bCA_UncacheGrChunk\([^;]*\);', ';', t)
    t = re.sub(r'\bMusicMode\b', 'SD_GetMusicMode()', t)
    # Its compiler's int is 16 bits and its long 32: positions and distances
    # rely on wrapping at 16 bits (a level wider than 127 tiles has unit
    # positions past 32767), so the types are spelled out.
    parts = re.split(r'(//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\\n])*")', t)
    for i in range(0, len(parts), 2):
        s = parts[i]
        s = re.sub(r'\bunsigned\s+long\b', 'uint32_t', s)
        s = re.sub(r'\bunsigned\s+int\b', 'uint16_t', s)
        s = re.sub(r'\bsigned\s+int\b', 'int16_t', s)
        s = re.sub(r'\bunsigned\b(?!\s+(char|short))', 'uint16_t', s)
        s = re.sub(r'\blong\b', 'int32_t', s)
        s = re.sub(r'\bint\b', 'int16_t', s)
        parts[i] = s
    t = ''.join(parts)
    # register every think/contact/react-shaped function it defines
    body = strip(t)
    one = re.findall(r'^void\s+(FF_\w+)\s*\(\s*CK_object\s*\*\s*\w+\s*\)\s*\n\{', body, re.M)
    two = re.findall(r'^void\s+(FF_\w+)\s*\(\s*CK_object\s*\*\s*\w+\s*,\s*CK_object\s*\*\s*\w+\s*\)\s*\n\{', body, re.M)
    setup = os.path.splitext(os.path.basename(out))[0]
    t += '\nvoid %s_SetupFunctions(void)\n{\n' % setup
    t += ''.join('\tCK_ACT_AddFunction("%s", &%s);\n' % (n, n) for n in sorted(set(one)))
    t += ''.join('\tCK_ACT_AddColFunction("%s", &%s);\n' % (n, n) for n in sorted(set(two)))
    t += '}\n'
    open(out, 'w').write('#include "kw_fitf.h"\n\n' + t)
    print('wrote', out, len(t.splitlines()), 'lines')


if __name__ == '__main__':
    main()
