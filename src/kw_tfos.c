/*
Keen Worlds: The Terror from Outer Space (Dr. Colossus, 2013, v1.0), a Keen 4
patch mod. Its CKPatch file (mymod.PAT) repurposes some of the program's
creatures into its own aliens; these are those routines in C, read from the
patches (kwtools/kw_tfos.py names which action uses which through STAND_INS;
research/the-terror-from-outer-space.md in the workspace root has the audit).
The rest of its changes are settings in the Keen 4 code, off for every other
world. The target actions are Keen 4's own slots with the mod's graphics (the
%action lines carry the recoloured frames).
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include "ck_act.h"
#include "ck_def.h"
#include "ck_play.h"
#include "id_sd.h"

// A creature that kills Keen on touch and is stunned (not killed) by a shot.
// (The same shape as Operation Ocflore's OC_LethalStunnable, without the hop.)
static void TFOS_LethalStunnable(CK_object *a, CK_object *b, CK_action *stunned)
{
	if (b->type == CT_Player)
	{
		CK_KillKeen();
		return;
	}
	if (b->type == CT_Stunner)
	{
		a->velX = 0;
		CK_StunCreature(a, b, stunned);
	}
}

// $127A6 CK4_MineCol: the mod's "mine" is not lethal on contact; a shot stops
// and stuns it (its stunned frames are Lindsey's second action's slot).
void TFOS_MineCol(CK_object *a, CK_object *b)
{
	if (b->type == CT_Stunner)
	{
		a->velX = 0;
		CK_StunCreature(a, b, CK_ACTION(CK4_ACT_Lindsey2));
	}
}

// $10F93 CK4_InchwormCol: not Keen 4's inchworm-into-Foot mechanic at all; the
// mod's creature kills Keen on touch and a shot stuns it (stunned frames are
// the Smirky-teleport slot).
void TFOS_InchwormCol(CK_object *a, CK_object *b)
{
	TFOS_LethalStunnable(a, b, CK_ACTION(CK4_ACT_SmirkyTele1));
}

// $100CF/$10135 CK4_EggCol: a shot pops on the egg; when hatched it does not
// drop Keen 4's three shell bits — instead the shell itself hops off as a Lick
// (CK4_ACT_LickHop1), and what climbs out is a *walking* alien, not a flyer
// (CK4_ACT_BirdWalk0 in place of Keen 4's BirdHatched0).
void TFOS_EggCol(CK_object *a, CK_object *b)
{
	if (b->type != CT_Player && b->type != CT_Stunner)
		return;
	if (b->type == CT_Stunner)
		CK_ShotHit(b);

	a->type = CT_Friendly;
	a->active = OBJ_EXISTS_ONLY_ONSCREEN;
	CK_SetAction(a, CK_ACTION(CK4_ACT_LickHop1)); // the shell hops off, not inert debris

	CK_object *alien = CK_GetNewObj(true);
	alien->type = CT4_Bird;
	alien->active = OBJ_ACTIVE;
	alien->posX = a->posX;
	alien->posY = a->posY - CK_INT(CK4_BirdHatchYOffset, 0x80);
	alien->xDirection = a->posX < ck_keenObj->posX ? IN_motion_Right : IN_motion_Left;
	alien->yDirection = IN_motion_Down;
	CK_SetAction(alien, CK_ACTION(CK4_ACT_BirdWalk0)); // hatches walking, not flying
}

// $12950/$1298A/$129A7 CK4_DartGun (+ $FB33 CK4_Miragia2, $3D740/$3D77C new
// actions): the wall gun fires the same up/across/down darts, but the mod runs
// each dart through a one-frame prefix action whose only effect (its think is
// the repurposed CK4_Miragia2, `mov word[obj],0xF`) is to re-class the dart to
// the Bounder type. Firing the normal dart actions with that type set folds the
// whole prefix away, so the appended prefix actions and the dart-action repoints
// are accounted for here.
void TFOS_DartGun(CK_object *obj)
{
	CK_object *dart = CK_GetNewObj(true);
	dart->posX = obj->posX;
	dart->posY = obj->posY;
	dart->type = CT4_Bounder; // the mod's darts are Bounder-classed (its Miragia2 prefix)
	dart->active = OBJ_EXISTS_ONLY_ONSCREEN;

	switch (obj->user1)
	{
	case 0:
		dart->xDirection = IN_motion_None;
		dart->yDirection = IN_motion_Up;
		CK_SetAction(dart, CK_ACTION(CK4_ACT_DartUp0));
		break;
	case 1:
		dart->xDirection = IN_motion_Right;
		dart->yDirection = IN_motion_None;
		CK_SetAction(dart, CK_ACTION(CK4_ACT_DartHorz0));
		break;
	case 2:
		dart->xDirection = IN_motion_None;
		dart->yDirection = IN_motion_Down;
		CK_SetAction(dart, CK_ACTION(CK4_ACT_DartDown0));
		break;
	case 3:
		dart->xDirection = IN_motion_Left;
		dart->yDirection = IN_motion_None;
		CK_SetAction(dart, CK_ACTION(CK4_ACT_DartHorz0));
		break;
	}

	SD_PlaySound(CK_SOUNDNUM(SOUND_DARTSHOOT));
}

void TFOS_SetupFunctions(void)
{
	CK_ACT_AddColFunction("TFOS_MineCol", &TFOS_MineCol);
	CK_ACT_AddColFunction("TFOS_InchwormCol", &TFOS_InchwormCol);
	CK_ACT_AddColFunction("TFOS_EggCol", &TFOS_EggCol);
	CK_ACT_AddFunction("TFOS_DartGun", &TFOS_DartGun);
}
