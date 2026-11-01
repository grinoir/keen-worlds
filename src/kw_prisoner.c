/*
Keen Worlds: The Prisoner's Dilemma (Levellass and the community, 2021,
v1.0), a Keen 6 patch mod in two builds (play as Keen or as Rick Uller). Its
CKPatch files add one routine of their own; this is that routine in C, read
from the patches (kwtools/kw_prisoner.py names the action that uses it;
research/ocflore-and-aurora-bliss.md in the workspace root has the audit).
The rest of its changes are settings in the Keen 6 code, off for every other
world. Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include "ck_act.h"
#include "ck_def.h"
#include "ck_play.h"

void CK6_ShowGetPasscard(void);

// $13BE1 (= 125A:1641): the rendezvous pickup (Keen 6's passcard sprite with
// its own graphics). Where getting the passcard would end the level, this
// shows its window (the rendezvous banter, in ck6_str_getPasscard) and the
// pickup is taken, but the level goes on. Keen meets Rick / Rick meets Keen.
void CK6_RendezvousCol(CK_object *a, CK_object *b)
{
	if (b->type != CT_Player)
		return;
	CK6_ShowGetPasscard(); // (shows the window, sets the passcard flag)
	CK_RemoveObj(a);
}

void CK6_Prisoner_SetupFunctions(void)
{
	CK_ACT_AddColFunction("CK6_RendezvousCol", &CK6_RendezvousCol);
}
