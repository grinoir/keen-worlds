"""One-time hand fixes on the converted camera code of CK_PLAY.C (kw_ff_cam.c)."""
import re, sys
p = sys.argv[1]; t = open(p).read()
def sub(a, b, flags=0):
    global t
    t, n = re.subn(a, b, t, flags=flags)
    if not n: print('no match:', a[:60])
for a, b in (('originxmin', 'rf_scrollXMinUnit'), ('originxmax', 'rf_scrollXMaxUnit'), ('originymin', 'rf_scrollYMinUnit'),
             ('originymax', 'rf_scrollYMaxUnit'), ('RF_Scroll', 'RF_SmoothScroll'), ('RF_NewPosition', 'RF_Reposition'),
             ('RF_AdaptPosition', 'RF_Reposition'), ('scorescreenkludge', 'ck_inHighScores')):
    sub(r'\b%s\b' % a, b)
sub(r'\nvoid kw_ff_cam_SetupFunctions\(void\)\n\{[^}]*\}\n', '\n')
t = t.replace('#include "kw_fitf.h"\n', '''/*
Keen Worlds: Foray in the Forest's camera, converted from CK_PLAY.C of its
source (CenterActor, WorldScrollScreen, ScrollScreen, CK_WrapScroll; Nisaba
and K1n9_Duk3; GPL v2 or later) by tools/kw_fitf_port.py, with the hand fixes
of tools/kw_fitf_cam_fixes.py. The engine's camera functions hand over to
these in this world.
*/

#include "kw_fitf.h"
#include "kw_ff_keen.h"


// Where objects are kept active around the screen (its CalcInactivate; the
// engine does this inline after it scrolls).
static void FF_CalcInactivate(void)
{
	ck_activeX0Tile = CK_Cross_max(RF_UnitToTile(rf_scrollXUnit) - CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT), 0);
	ck_activeX1Tile = RF_UnitToTile(rf_scrollXUnit) + RF_PixelToTile(320) + CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT);
	ck_activeY0Tile = CK_Cross_max(RF_UnitToTile(rf_scrollYUnit) - CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT), 0);
	ck_activeY1Tile = RF_UnitToTile(rf_scrollYUnit) + RF_PixelToTile(208) + CK_INT(CK_activeLimit, DEFAULT_ACTIVE_LIMIT);
}
''', 1)
open(p, 'w').write(t)
