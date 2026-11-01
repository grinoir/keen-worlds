/*
Keen Worlds: Canmore's Redemption (Aurora Bliss II, Dave216, 2023, v1.0.2),
a Keen 5 patch mod. Its CKPatch file replaces some of the program's routines
with its own; these are those routines in C, read from the patches
(kwtools/kw_canmore.py names which action uses which; research/ocflore-and-
aurora-bliss.md in the workspace root has the audit). The rest of its changes
are settings in the Keen 5 code, off for every other world. Each function
names the patch address it was read from.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include "ck_act.h"
#include "ck_def.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "id_ca.h"
#include "id_rf.h"
#include "id_sd.h"

#define CR_ITEM_KEYCARD 12

extern chunk_id_t CK_ItemSpriteChunks[];
void KW9_FriendMessage(void);

// $1118D: Sparkies take three shots (CK5_SparkyHealth); Keen dies on touch.
void CR_SparkyCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
	{
		if (--a->user2 == 0)
			CK_StunCreature(a, b, CK_ACTION(CK5_ACT_SparkyStunned0));
		else
			CK_ShotHit(b);
	}
}

// $1129C: the keycard a stunned Ampton drops, where it stands.
static void CR_DropKeycard(CK_object *ampton)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT5_Item;
	obj->clipped = CLIP_normal;
	obj->zLayer = 2;
	obj->posX = ampton->posX;
	obj->posY = ampton->posY;
	obj->yDirection = -1;
	obj->user1 = CR_ITEM_KEYCARD;
	obj->gfxChunk = CK_LookupChunk(CK_ItemSpriteChunks[CR_ITEM_KEYCARD]);
	obj->user2 = obj->gfxChunk;
	obj->user3 = obj->gfxChunk + 2;
	CK_SetAction(obj, CK_ACTION(CK_ACT_item));
}

// $11643: Amptons are harmless (also sliding down a pole) and push Keen;
// Keen's shots don't hurt them, but an enemy's purple shot stuns one, and it
// drops a keycard.
void CR_AmptonCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		if (a->currentAction != CK_ACTION(CK5_ACT_AmptonPole2))
			CK_PhysPushX(b, a);
	}
	else if (b->type == CT_Stunner)
		CK_ShotHit(b);
	else if (b->type == CT5_EnemyShot)
	{
		a->velY = 0;
		a->clipped = CLIP_normal;
		a->yDirection = IN_motion_Down;
		SD_PlaySound(CK_SOUNDNUM(SOUND_AMPTONSTUN));
		CK_StunCreature(a, b, CK_ACTION(CK5_ACT_AmptonStunned0));
		CR_DropKeycard(a);
	}
}

// $79F8 (while it walks or waits): Korath kills Keen; it takes seven shots
// (CK5_KorathHealth), flashing at each, then is stunned.
void CR_KorathWalkCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	if (b->type != CT_Stunner)
		return;
	if (--a->user2 == 0)
	{
		a->velX = a->velY = 0;
		CK_StunCreature(a, b, CK_ACTION(CK5_ACT_KorathStun0));
		return;
	}
	a->user3 = 2;
	a->visible = true;
	CK_ShotHit(b);
}

// $13956 (Keen 5's own Korath collision, rewritten): deadly, and one shot
// stuns.
void CR_KorathCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_KillKeen();
	else if (b->type == CT_Stunner)
		CK_StunCreature(a, b, CK_ACTION(CK5_ACT_KorathStun0));
}

// $CAA7: every fuse marker is an unseen QED (CK5_QEDEveryLevel). Touching
// it, a friend speaks (Keen 9's window: kw_keen9.c), and it is gone; the
// high scores (level 15) have none.
void CR_QEDCol(CK_object *a, CK_object *b)
{
	if (b->type != CT_Player || ca_mapOn == 15)
		return;
	SD_PlaySound(3);
	KW9_FriendMessage();
	RF_ForceRefresh();
	CK_RemoveObj(a);
}

// $F986 (the program's C_ClipSide, for the Master's sparks): they push Keen
// aside, without the program's pushing him off ledges ($F996).
void CR_ClipSideCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
		CK_PhysPushX(b, a);
}

void CR_SetupFunctions(void)
{
	CK_ACT_AddColFunction("CR_SparkyCol", &CR_SparkyCol);
	CK_ACT_AddColFunction("CR_AmptonCol", &CR_AmptonCol);
	CK_ACT_AddColFunction("CR_KorathWalkCol", &CR_KorathWalkCol);
	CK_ACT_AddColFunction("CR_KorathCol", &CR_KorathCol);
	CK_ACT_AddColFunction("CR_QEDCol", &CR_QEDCol);
	CK_ACT_AddColFunction("CR_ClipSideCol", &CR_ClipSideCol);
}
