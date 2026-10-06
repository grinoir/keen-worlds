"""One-time hand fixes on the converted CK_KEEN.C (kw_ff_keen.c)."""
import re, sys
p = sys.argv[1]; t = open(p).read()
def sub(a, b, flags=0):
    global t
    t, n = re.subn(a, b, t, flags=flags)
    if not n: print('no match:', a[:60])
def rep(a, b):
    global t
    if a not in t: print('no match:', a[:60])
    t = t.replace(a, b)
sub(r'RF_AddSpriteDraw\(&(\w+)->user(\d)', r'RF_AddSpriteDrawUsing16BitOffset(&\1->user\2')
sub(r'RF_RemoveSpriteDraw\(&(\w+)->user(\d)\)', r'RF_RemoveSpriteDrawUsing16BitOffset(&\1->user\2)')
sub(r'\(\s*CK_object\s*\*\s*\)\s*(\w+->user\d)', r'FF_ObjFromUser(\1)')
sub(r'mapbwidthtable\[([^\]]+)\]\s*/\s*2', r'((\1) * CA_GetMapWidth())')
sub(r'mapsegs\[(\d)\]\[([^\]]+)\]', r'FF_MapAt(\1, \2)')
sub(r'\bRF_MemToMap\b', 'RF_ReplaceTiles')
sub(r'\bdir_(North|South|East|West)\b', r'IN_dir_\1')
sub(r'\bFF_Quit\(', 'Quit(')
sub(r'ck_keenState\.upheld', 'ff_keenUpHeld')
sub(r'ck_keenState\.leavepoletime', 'ck_keenState.poleGrabTime')
sub(r'ck_keenState\.mustAdjustNextX', 'ff_keenMustAdjustNextX')
sub(r'ff_state\.jumpCheat', 'ck_gameState.jumpCheat')
sub(r'ff_map\.keys', 'ck_gameState.keyGems')
sub(r'ff_map\.riding', 'ck_keenState.platform')
sub(r'\bFF_Report\([^;]*\);', ';')
sub(r'\bFF_CheckPosition\b', 'CK_NotStuckInWall')
sub(r'\bFF_CenterActor\b', 'CK_CentreCamera')
sub(r'\bVW_FadeUp\b', 'VL_FadeFromBlack')
sub(r'\bVW_FadeDown\b', 'VL_FadeToBlack')
sub(r'\bgrsegs\b', 'ca_graphChunks')
sub(r'\bDemoMode\b', 'IN_DemoGetMode()')
sub(r'\bdemo_Off\b', 'IN_Demo_Off')
sub(r'\bDirection\b(?=\s+\w)', 'IN_Direction')
sub(r'FF_MapAt\((\d), (\w+)\) = ([^;]+);', r'CA_SetTileAtPos((\2) % CA_GetMapWidth(), (\2) / CA_GetMapWidth(), \1, \3);')
sub(r'\(int16_t\)FF_SpawnSlideDoor\(([^;]*)\);', r'FF_ObjToUser(FF_SpawnSlideDoor(\1));')
sub(r'\(int16_t\)page\b', 'FF_ObjToUser(page)')
sub(r'\bs_keenRun(\d)\.', r'CK_ACTION(CK_ACT_keenRun\1)->')
sub(r'currentAction->deltaPos([XY])', r'currentAction->vel\1')
sub(r'currentAction->contact\b', 'currentAction->collide')
sub(r'\bCA_UncacheGrChunk\([^;]*\);', ';')
sub(r'\bNOWAYSND\b', 'FF_OLDNOWAYSND')
sub(r'^CK_keenState ck_keenState;[^\n]*\n', '// (ck_keenState is the engine\'s)\n', flags=re.M)
sub(r'\bck_KeenPoleOffs\b', 'ff_KeenPoleOffs')
# forward declarations of everything the file defines
body = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/', ' ', t)
protos = []
for m in re.finditer(r'^((?:static\s+)?[A-Za-z_][\w \t\*]*?\b(FF_\w+)\s*\([^;{}]*\))\s*\n\{', body, re.M):
    sig = ' '.join(m.group(1).split())
    if sig not in protos: protos.append(sig)
open(p.replace('.c', '.h'), 'w').write('// Foray in the Forest: the functions of %s (written with it by the one-time\n// conversion of its source).\n#ifndef KW_FF_KEEN_H\n#define KW_FF_KEEN_H\n' % p.split('/')[-1] + ''.join(s + ';\n' for s in protos if not s.startswith('static')) + 'void kw_ff_keen_SetupFunctions(void);\n#endif\n')
t = t.replace('#include "kw_fitf.h"\n', '''/*
Keen Worlds: Foray in the Forest's Keen, converted from CK_KEEN.C of its
source (Nisaba and K1n9_Duk3; GPL v2 or later) by tools/kw_fitf_port.py, with
the hand fixes of tools/kw_fitf_keen_fixes.py.
*/

#include "kw_fitf.h"
#include "kw_ff_act.h"
#include "kw_ff_keen.h"

''' + ''.join(s + ';\n' for s in protos if s.startswith('static')), 1)
open(p, 'w').write(t)
print(len(protos), 'prototypes')
