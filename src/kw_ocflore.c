/*
Keen Worlds: Operation Ocflore (Bubbatom, 2020, v1.3), a Keen 4 patch mod.
Its CKPatch file replaces some of the program's routines with its own; these
are those routines in C, read from the patches (kwtools/kw_ocflore.py names
which action uses which; research/ocflore-and-aurora-bliss.md in the
workspace root has the audit). The rest of its changes are settings in the
Keen 4 code, off for every other world.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include "ck_act.h"
#include "ck_def.h"
#include "ck_phys.h"
#include "ck_play.h"
#include "id_ca.h"
#include "id_rf.h"
#include "id_sd.h"

#define OC_CONVERTED_WORM 34 // (Ocflore's own class for an inchworm that met another)

void CK_KeenColFunc(CK_object *a, CK_object *b);

// A creature that kills Keen and is stunned by a shot, then hops.
static void OC_LethalStunnable(CK_object *a, CK_object *b, CK_action *stunned, bool hop)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	if (b->type != CT_Stunner)
		return;
	CK_StunCreature(a, b, stunned);
	if (hop)
	{
		a->velY = -0x18;
		a->velX = a->xDirection * 8;
	}
}

// Bounders kill Keen, and a shot stuns them.
void OC_BounderCol(CK_object *a, CK_object *b)
{
	OC_LethalStunnable(a, b, CK_ACTION(CK4_ACT_BounderStunned0), false);
}

// Poison slugs: one kind of stunned slug, and it hops.
void OC_SlugCol(CK_object *a, CK_object *b)
{
	OC_LethalStunnable(a, b, CK_ACTION(CK4_ACT_SlugStunned0), true);
}

// Skypests can be stunned (their squish action is the stunned one here).
void OC_SkypestCol(CK_object *a, CK_object *b)
{
	OC_LethalStunnable(a, b, CK_ACTION(CK4_ACT_SkypestSquish0), true);
}

// Sprites: a shot stops and stuns them (the stunned action is the old first
// frame of their bullet).
void OC_SpriteCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	if (b->type != CT_Stunner)
		return;
	a->velX = 0;
	a->clipped = CLIP_normal;
	CK_StunCreature(a, b, CK_ACTION(CK4_ACT_SpriteBullet0));
}

// Licks: always deadly; a shot stuns them (they bounce up a little).
void OC_LickCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	if (b->type != CT_Stunner)
		return;
	CK_StunCreature(a, b, CK_ACTION(CK4_ACT_LickStunned0));
	a->velY -= 0x10;
}

// Licks jump at Keen, always the long jump.
void OC_LickMove(CK_object *obj)
{
	obj->xDirection = obj->posX <= ck_keenObj->posX ? IN_motion_Right : IN_motion_Left;
	obj->velX = obj->xDirection * 32;
	obj->velY = -0x38;
}

// Inchworms (and the walking birds) keep away from Keen.
void OC_InchwormAway(CK_object *obj)
{
	obj->xDirection = obj->posX > ck_keenObj->posX ? IN_motion_Right : IN_motion_Left;
}

// Two inchworms meet: one of them is changed for a moment.
void OC_InchwormMeet(CK_object *a, CK_object *b)
{
	if (b->type != CT4_Inchworm)
		return;
	a->type = OC_CONVERTED_WORM;
	CK_SetAction2(a, CK_ACTION(KW_ACT_New011F)); // (the patches' own action: kwtools/kw_ocflore.py)
}

// ...and is an ordinary inchworm again when left alone.
void OC_WormRevert(CK_object *obj)
{
	obj->type = CT4_Inchworm;
}

// An inchworm touching a changed one becomes an Arachnut, in a puff of
// smoke, and the changed ones are gone.
void OC_InchwormCol(CK_object *a, CK_object *b)
{
	if (b->type != OC_CONVERTED_WORM)
		return;
	SD_PlaySound(CK_SOUNDNUM(SOUND_FOOTAPPEAR));
	a->type = CT4_Arachnut;
	CK_SetAction2(a, CK_ACTION(CK4_ACT_ArachnutWalk0));
	static const int16_t puffs[4][2] = {{-0x80, 0x100}, {0x100, 0x180}, {0x280, 0x100}, {0, -0x80}};
	for (int i = 0; i < 4; ++i)
	{
		CK_object *poof = CK_GetNewObj(true);
		poof->posX = a->posX + puffs[i][0];
		poof->posY = a->posY + puffs[i][1];
		poof->zLayer = PRIORITIES - 1;
		CK_SetAction(poof, CK_ACTION(CK4_ACT_FootPoof0));
	}
	for (CK_object *o = ck_keenObj->next, *next; o; o = next)
	{
		next = o->next;
		if (o->type == OC_CONVERTED_WORM)
			CK_RemoveObj(o);
	}
}

// Keen on his pogo stick squashes inchworms; anything else as usual.
void OC_KeenSquishCol(CK_object *a, CK_object *b)
{
	if (b->type == CT4_Inchworm || b->type == OC_CONVERTED_WORM)
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_SKYPESTSQUISH));
		b->type = CT_Friendly;
		CK_SetAction2(b, CK_ACTION(CK4_ACT_SkypestPreen0)); // (here: the squashed worm)
		return;
	}
	CK_KeenColFunc(a, b);
}

// Berkeloid flames burst on walls too, not only on the floor.
void OC_FireballDraw(CK_object *obj)
{
	if (obj->rightTI || obj->leftTI || obj->topTI)
	{
		SD_PlaySound(CK_SOUNDNUM(SOUND_FIREBALLLAND));
		CK_SetAction2(obj, CK_ACTION(CK4_ACT_FireballGround0));
	}
	RF_AddSpriteDraw(&(obj->sde), obj->posX, obj->posY, obj->gfxChunk, false, obj->zLayer);
}

// Markers 36-38: a flying blue bird, on easy and up, normal and up, hard.
bool OC_SpawnFlyingBird(int info, int tileX, int tileY)
{
	if (info < 36 || info > 38 || !CK_INT(OC_FlyingBirdMarkers, 0))
		return false;
	if (ck_gameState.difficulty < D_Easy + (info - 36))
		return true;
	CK_object *obj = CK_GetNewObj(false);
	obj->type = CT4_Bird;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = PRIORITIES - 2;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY) - 0xE1;
	CK_SetAction(obj, CK_ACTION(CK4_ACT_BirdFly0));
	CA_MARKLUMP(LUMP_EGG); // (the egg's group has the bird)
	return true;
}

void OC_SetupFunctions(void)
{
	CK_ACT_AddColFunction("OC_BounderCol", &OC_BounderCol);
	CK_ACT_AddColFunction("OC_SlugCol", &OC_SlugCol);
	CK_ACT_AddColFunction("OC_SkypestCol", &OC_SkypestCol);
	CK_ACT_AddColFunction("OC_SpriteCol", &OC_SpriteCol);
	CK_ACT_AddColFunction("OC_LickCol", &OC_LickCol);
	CK_ACT_AddFunction("OC_LickMove", &OC_LickMove);
	CK_ACT_AddFunction("OC_InchwormAway", &OC_InchwormAway);
	CK_ACT_AddColFunction("OC_InchwormMeet", &OC_InchwormMeet);
	CK_ACT_AddFunction("OC_WormRevert", &OC_WormRevert);
	CK_ACT_AddColFunction("OC_InchwormCol", &OC_InchwormCol);
	CK_ACT_AddColFunction("OC_KeenSquishCol", &OC_KeenSquishCol);
	CK_ACT_AddFunction("OC_FireballDraw", &OC_FireballDraw);
}
