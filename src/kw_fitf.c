/*
Keen Worlds: Foray in the Forest (Nisaba and K1n9_Duk3) as a world.

Foray in the Forest is a game of its own with public GPL source. That source
descends from Atroxian Realm's (so from Omnispeak's Keen 5 code), rewritten
with Keen Dreams names. Here it runs on the engine's Keen 5 code with its own
data files (extension CKS), its own variable files (written by
tools/kw_fitf.py from its source) and its own level setup below.

So far: its maps, graphics, sounds and sprite states load, its level setup
and creatures (kw_ff_scan.c, kw_ff_act.c) are converted from its source, and
Keen is still the engine's Keen 5 Keen. Keen's changes, its game rules,
dialogues and checkpoints are stubs below (see
../../research/foray-in-the-forest.md).

Licensed under the GNU GPL v2 or later, like Omnispeak and Foray in the
Forest.
*/

#include "id_fs.h"
#include "ck_ep.h"
#include "kw_fitf.h"
#include "kw_ff_act.h"

extern CK_EpisodeDef ff_episode;

FF_State ff_state;
FF_MapState ff_map;
bool ff_keenKilled, ff_keenUpHeld;
int ff_brainfucled;

bool FF_IsActive(void)
{
	return ck_currentEpisode == &ff_episode;
}

// ---- Small helpers of its CK_STATE.C ----

void FF_CalcTileClipRects(CK_object *ob)
{
	ob->clipRects.unitXmid = (ob->clipRects.unitX2 - ob->clipRects.unitX1) / 2 + ob->clipRects.unitX1;
	ob->clipRects.tileX1 = RF_UnitToTile(ob->clipRects.unitX1);
	ob->clipRects.tileX2 = RF_UnitToTile(ob->clipRects.unitX2);
	ob->clipRects.tileY1 = RF_UnitToTile(ob->clipRects.unitY1);
	ob->clipRects.tileY2 = RF_UnitToTile(ob->clipRects.unitY2);
	ob->clipRects.tileXmid = RF_UnitToTile(ob->clipRects.unitXmid);
}

void FF_MoveObjVert(CK_object *ob, int ymove)
{
	ob->posY += ymove;
	ob->clipRects.unitY1 += ymove;
	ob->clipRects.unitY2 += ymove;
	ob->clipRects.tileY1 = RF_UnitToTile(ob->clipRects.unitY1);
	ob->clipRects.tileY2 = RF_UnitToTile(ob->clipRects.unitY2);
}

void FF_MoveObjHoriz(CK_object *ob, int xmove)
{
	ob->posX += xmove;
	ob->clipRects.unitX1 += xmove;
	ob->clipRects.unitX2 += xmove;
	ob->clipRects.unitXmid += xmove;
	ob->clipRects.tileX1 = RF_UnitToTile(ob->clipRects.unitX1);
	ob->clipRects.tileX2 = RF_UnitToTile(ob->clipRects.unitX2);
	ob->clipRects.tileXmid = RF_UnitToTile(ob->clipRects.unitXmid);
}

// Its sprites drawn flipped upside down. TODO: flip; drawn upright for now.
void FF_DrawSprite(RF_SpriteDrawEntry **sde, int x, int y, int chunk, int drawtype, int zLayer)
{
	RF_AddSpriteDraw(sde, x, y, chunk, false, zLayer);
}

// ---- Not ported yet: parts of its CK_KEEN.C, CK_GAME.C and CK_SPEC.C the
// creatures call. Each does the least that keeps the game going. ----

// TODO: Keen stunned by a creature (StunKeen in CK_KEEN.C)
void FF_StunKeen(uint16_t midx)
{
	(void)midx;
}

bool FF_KeenIsDead(void)
{
	return ck_gameState.levelState == LS_Died || ff_keenKilled;
}

// TODO: its own Keen spawn (SpawnKeen in CK_KEEN.C)
void FF_SpawnKeen(int tileX, int tileY, int direction)
{
	CK_SpawnKeen(tileX, tileY, direction ? direction : 1);
}

void CK_KeenColFunc(CK_object *a, CK_object *b);

// TODO: its own Keen contact, walk and pause code (CK_KEEN.C); creatures
// only compare a state's function with these.
void FF_KeenContact(CK_object *ob, CK_object *hit)
{
	CK_KeenColFunc(ob, hit);
}
void FF_KeenWalkThink(CK_object *ob)
{
	(void)ob;
}
void FF_KeenPauseThink(CK_object *ob)
{
	(void)ob;
}

// TODO: switches (ToggleSwitch in CK_KEEN.C)
void FF_ToggleSwitch(int x, int y)
{
	(void)x;
	(void)y;
}

// TODO: dream bubbles (CK_KEEN.C)
void FF_RemoveDreamBubbles(void)
{
}
void FF_SpawnDreamBubble(uint16_t x, uint16_t y, uint16_t type)
{
	(void)x;
	(void)y;
	(void)type;
}

// TODO: checkpoints (CK_GAME.C)
bool FF_SaveCheckpoint(void)
{
	return false;
}
bool FF_LoadCheckpoint(void)
{
	return false;
}

// TODO: dialogue windows (CK_SPEC.C)
void FF_MiscDialog(int number)
{
	(void)number;
}
void FF_JanitorDialog(int number)
{
	(void)number;
}

// TODO: music tempo changes and two sounds in a row (its ID_SD.C)
void SD_SetMusicRate(int rate)
{
	(void)rate;
}
void SD_PlayTwoSounds(int first, int second)
{
	(void)second;
	SD_PlaySound(first);
}

// Its level setup: kw_ff_scan.c (ScanInfoPlane in CK_SPEC.C).
void FF_ScanInfoPlane(void);
void FF_ScanInfoLayer(void)
{
	memset(&ff_map, 0, sizeof(ff_map));
	ff_keenKilled = false;
	FF_ScanInfoPlane();
}

void FF_SetupFunctions(void)
{
	kw_ff_act_SetupFunctions();
}

// The player's game files and our generated support files, all in the
// world's own folder.
bool FF_IsPresent(void)
{
	static const char *files[] = {"EGAGRAPH.CKS", "EGAHEAD.CKS", "EGADICT.CKS", "MAPTEMP.CKS", "MAPHEAD.CKS",
		"AUDIOT.CKS", "AUDIOHED.CKS", "GFXINFOE.CKS", "AUDINFOE.CKS", "FFACTION.CKS", "FFGFX.CKS"};
	for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
		if (!FS_IsKeenFilePresent(files[i]))
			return false;
	return true;
}

// The engine treats it as Keen 5 (its source's ancestor); only the file
// extension and the level setup differ.
CK_EpisodeDef ff_episode = {
	EP_CK5,
	"CKS",
	"FITF.CKS",
	&FF_ScanInfoLayer,
	&FF_IsPresent,
};
