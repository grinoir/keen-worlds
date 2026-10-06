"""One-time hand fixes on the converted CK_STATE.C (kw_ff_state.c): its
movement, wall clipping and state machine (up to OnScreen; the gravity and
friction helpers after that are kw_ff_phys.c)."""
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
# the slice ends inside a comment
t = t[:t.rindex('/*')].rstrip() + '\n'
rep('int16_t ck_nextX;\nint16_t ck_nextY;\n', '// (ck_nextX and ck_nextY are the engine\'s)\n')
sub(r'tinf\[([^\]]+?)\s*\+\s*NORTHWALL\]', r'TI_ForeTop(\1)')
sub(r'tinf\[([^\]]+?)\s*\+\s*EASTWALL\]', r'TI_ForeRight(\1)')
sub(r'tinf\[([^\]]+?)\s*\+\s*SOUTHWALL\]', r'TI_ForeBottom(\1)')
sub(r'tinf\[([^\]]+?)\s*\+\s*WESTWALL\]', r'TI_ForeLeft(\1)')
sub(r'&FORESPOT\(([^;]*)\);', r'CA_TilePtrAtPos(\1, 1);')
sub(r'->progress\b', '->type')
for a, b in (('think', 'AT_Frame'), ('stepthink', 'AT_UnscaledFrame'), ('slidethink', 'AT_ScaledFrame'),
             ('slide', 'AT_ScaledOnce'), ('step', 'AT_UnscaledOnce')):
    sub(r'(->type\s*[!=]=\s*)%s\b' % a, r'\1' + b)
sub(r'state->deltaPos([XY])', r'state->vel\1')
sub(r'->skippable\b', '->protectAnimation')
sub(r'\bpushtype\b', 'int16_t')
sub(r'\bcl_custommidclip\b', 'FF_CLIP_custommid')
sub(r'ck_keenState\.mustAdjustNextX', 'ff_keenMustAdjustNextX')
sub(r'\bFF_Quit\(', 'Quit(')
# its ck_mapState.riding is the engine's ck_keenState.platform (as in the other converted files)
sub(r'ff_map\.riding', 'ck_keenState.platform')
# CalcBounds is the engine's CK_ResetClipRects
i = t.index('void FF_CalcBounds(CK_object *ob)\n{'); j = t.index('\n}\n', i) + 3
t = t[:i] + 'void FF_CalcBounds(CK_object *ob)\n{\n\t// KEEN WORLDS: the engine\'s is the same (sprite box, then the tile box)\n\tCK_ResetClipRects(ob);\n}\n' + t[j:]
sub(r'\nvoid kw_ff_state_SetupFunctions\(void\)\n\{[^}]*\}\n', '\n')
body = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/', ' ', t)
protos = []
for m in re.finditer(r'^((?:static\s+)?[A-Za-z_][\w \t\*]*?\b(FF_\w+)\s*\([^;{}]*\))\s*\n\{', body, re.M):
    sig = ' '.join(m.group(1).split())
    if sig not in protos: protos.append(sig)
open(p.replace('.c', '.h'), 'w').write('// Foray in the Forest: the functions of kw_ff_state.c (written with it by the\n// one-time conversion of its CK_STATE.C).\n#ifndef KW_FF_STATE_H\n#define KW_FF_STATE_H\n' + ''.join(s + ';\n' for s in protos if not s.startswith('static') and 'FF_CalcTileClipRects' not in s and 'FF_MoveObj' not in s) + '#endif\n')
t = t.replace('#include "kw_fitf.h"\n', '''/*
Keen Worlds: Foray in the Forest's movement, wall clipping and state
machine, converted from CK_STATE.C of its source (Nisaba and K1n9_Duk3; GPL
v2 or later) by tools/kw_fitf_port.py, with the hand fixes of
tools/kw_fitf_state_fixes.py. The engine's own functions hand over to these
in this world (ck_phys.c, ck_play.c): its states push objects to floors,
ceilings and walls, its slopes and custom clip boxes differ from Keen 5's.
*/

#include "kw_fitf.h"
#include "kw_ff_keen.h"
#include "kw_ff_state.h"

''' + ''.join(s + ';\n' for s in protos if s.startswith('static')), 1)
open(p, 'w').write(t)
print(len(protos), 'prototypes')
