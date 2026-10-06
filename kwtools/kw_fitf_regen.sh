#!/bin/sh
# Redo the one-time conversions and their scripted hand fixes.
cd /home/grinoir/Projects/CommanderKeen
A="play/mods/fitf/FFACTION.CKS omnispeak/src/kw_fitf_defs.h"
python3 tools/kw_fitf_port.py foray/src_v20/SOURCE/CK_ACT.C /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_act.c $A
cp /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_act.c omnispeak/src/kw_ff_act.c
python3 /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/patch_act.py omnispeak/src/kw_ff_act.c
python3 - <<'PY'
a=open('omnispeak/src/kw_ff_act.c').read()
i=a.index('typedef int turntype_fwd;\n')+len('typedef int turntype_fwd;\n'); j=a.index('\n\n',i)
protos=[l for l in a[i:j].splitlines() if not l.startswith('static')]
open('omnispeak/src/kw_ff_act.h','w').write('// Foray in the Forest: the functions of kw_ff_act.c (written with it by the\n// one-time conversion of its CK_ACT.C).\n#ifndef KW_FF_ACT_H\n#define KW_FF_ACT_H\n'+'\n'.join(protos)+'\nvoid kw_ff_act_SetupFunctions(void);\n#endif\n')
PY
python3 tools/kw_fitf_port.py foray/src_v20/SOURCE/CK_KEEN.C /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_keen.c $A
cp /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_keen.c omnispeak/src/kw_ff_keen.c
python3 /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/patch_keen.py omnispeak/src/kw_ff_keen.c
python3 tools/kw_fitf_port.py foray/src_v20/SOURCE/CK_KEEN2.C /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_map.c $A
cp /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_map.c omnispeak/src/kw_ff_map.c
python3 /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/patch_map.py omnispeak/src/kw_ff_map.c
sed -i 's|#include "kw_fitf.h"|#include "kw_fitf.h"\n#include "kw_ff_keen.h"|' omnispeak/src/kw_ff_map.c
tr -d '\r' < foray/src_v20/SOURCE/CK_PLAY.C | sed -n 3836,4230p > /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/cam_slice.c
python3 tools/kw_fitf_port.py /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/cam_slice.c /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_cam.c $A foray/src_v20/SOURCE/x
cp /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_cam.c omnispeak/src/kw_ff_cam.c
python3 /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/patch_cam.py omnispeak/src/kw_ff_cam.c
tr -d '\r' < foray/src_v20/SOURCE/CK_SPEC.C | sed -n 720,1190p > /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/dlg_slice.c
python3 tools/kw_fitf_port.py /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/dlg_slice.c /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_dialog.c $A foray/src_v20/SOURCE/x
cp /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_dialog.c omnispeak/src/kw_ff_dialog.c
python3 /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/patch_dialog.py omnispeak/src/kw_ff_dialog.c
tr -d '\r' < foray/src_v20/SOURCE/CK_STATE.C | sed -n 1,1800p > /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/state_full.c
python3 - <<'PY'
# the movement file is converted without the mappings to the engine's
# movement functions (it is what those hand over to)
import sys
sys.path.insert(0, 'tools')
import kw_fitf_port as P
for k in ('OnScreen', 'ClipToWalls', 'FullClipToWalls', 'PushObj', 'ClipToSpriteSide', 'ClipToSpriteTop', 'ClipToSprite', 'CalcBounds', 'NewState', 'ChangeState'):
    P.WORDS.pop(k, None)
sys.argv = ['x', '/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/state_full.c', '/tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_state.c', 'play/mods/fitf/FFACTION.CKS', 'omnispeak/src/kw_fitf_defs.h', 'foray/src_v20/SOURCE/x']
P.main()
PY
cp /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/kw_ff_state.c omnispeak/src/kw_ff_state.c
python3 /tmp/claude-1000/-home-grinoir-Projects-CommanderKeen/c1440f9f-6313-47f2-b69e-08e8e86832b8/scratchpad/fitf/patch_state.py omnispeak/src/kw_ff_state.c
