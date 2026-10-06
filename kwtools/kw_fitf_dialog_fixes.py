"""One-time hand fixes on the converted dialogue code of CK_SPEC.C (kw_ff_dialog.c)."""
import re, sys
p = sys.argv[1]; t = open(p).read()
t = re.sub(r'\nvoid kw_ff_dialog_SetupFunctions\(void\)\n\{[^}]*\}\n', '\n', t)
t = t.replace('#include "kw_fitf.h"\n', '''/*
Keen Worlds: Foray in the Forest's dialogue windows, converted from CK_SPEC.C
of its source (Nisaba and K1n9_Duk3; GPL v2 or later) by
tools/kw_fitf_port.py, with the hand fixes of tools/kw_fitf_dialog_fixes.py.
*/

#include "kw_fitf.h"
#include "kw_ff_keen.h"

void CK_MeasureMultiline(const char *str, uint16_t *w, uint16_t *h);
''', 1)
open(p, 'w').write(t)
