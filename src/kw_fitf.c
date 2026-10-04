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

void CK_DrawLongRight(int x, int y, int digits, int zerotile, int value);
void CK_MeasureMultiline(const char *str, uint16_t *w, uint16_t *h);
extern uint8_t ca_levelbit;
extern uint8_t ca_graphChunkNeeded[CA_MAX_GRAPH_CHUNKS];

// ---- Its "Keen enters" box (DialogDraw and DialogUpdate in CK_GAME.C): the
// text on the left, Keen's face on the right and under it a hand counting
// down from five. (The engine's own box takes six pictures in a row, which
// this game doesn't have.) ----

static int ff_handPic, ff_chunkCount, ff_chunkMax;

void FF_BeginCacheBox(const char *title, int numChunks)
{
	static const int pics[6] = {FF_CD_KEENPIC, FF_CD_FIVEPIC, FF_CD_FOURPIC, FF_CD_THREEPIC, FF_CD_TWOPIC, FF_CD_ONEPIC};
	uint16_t w, h;
	ff_handPic = 0;
	US_CenterWindow(26, 8);
	US_SetWindowW(US_GetWindowW() - 48);
	for (int i = 0; i < 6; i++)
	{
		CA_CacheGrChunk(pics[i]);
		ca_graphChunkNeeded[pics[i]] &= ~ca_levelbit;
		if (!ca_graphChunks[pics[i]])
		{
			ff_handPic = 5;
			break;
		}
		if (i == 0)
			VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW(), US_GetWindowY(), pics[i]);
	}
	US_SetPrintX(US_GetWindowX());
	CK_MeasureMultiline(title, &w, &h);
	US_SetPrintY(US_GetPrintY() + (US_GetWindowH() - h) / 2 - 4);
	US_CPrint(title);
	VH_UpdateScreen();
	ff_chunkCount = ff_chunkMax = numChunks / 6;
}

void FF_UpdateCacheBox(void)
{
	if (--ff_chunkCount == 0 && ff_handPic <= 4)
	{
		ff_chunkCount = ff_chunkMax;
		if (ca_graphChunks[FF_CD_FIVEPIC + ff_handPic])
		{
			VHB_DrawBitmap(US_GetWindowX() + US_GetWindowW(), US_GetWindowY() + 44, FF_CD_FIVEPIC + ff_handPic);
			VH_UpdateScreen();
		}
		// as the engine's own box: loading is very fast here
		int loadingDelay = CFG_GetConfigInt("loadingDelay", 10);
		if (loadingDelay)
			VL_DelayTics(loadingDelay);
		ff_handPic++;
	}
}

// ---- Its status window (DrawStatusWindow in CK_PLAY.C) ----


void FF_DrawStatusWindow(void)
{
	int winX = 64, winY = 16, winW = 184, winH = 144, n;
	VHB_DrawTile8(winX, winY, 36);
	VHB_DrawTile8(winX, winY + winH, 41);
	for (n = winX + 8; winX + winW - 8 >= n; n += 8)
	{
		VHB_DrawTile8(n, winY, 37);
		VHB_DrawTile8(n, winY + winH, 42);
	}
	VHB_DrawTile8(n, winY, 38);
	VHB_DrawTile8(n, winY + winH, 43);
	for (n = winY + 8; winY + winH - 8 >= n; n += 8)
	{
		VHB_DrawTile8(winX, n, 39);
		VHB_DrawTile8(winX + winW, n, 40);
	}
	VHB_Bar(72, 24, 176, 136, 7);

	int oldcolor = US_GetPrintColour();
	uint16_t strW, strH;
	const char *name = CK_STRINGELEMENT(ck_str_levelNames, ca_mapOn);

	US_SetPrintY(27);
	US_SetWindowX(80);
	US_SetWindowW(161);
	US_SetPrintColour(15);
	US_CPrint("LOCATION");
	VHB_Bar(79 - 1, 38 - 1, 162 + 2, 20 + 2, 8);
	VHB_Bar(79, 38, 162, 20, 0);
	CK_MeasureMultiline(name, &strW, &strH);
	US_SetPrintY((20 - strH) / 2 + 40 - 2);
	US_SetPrintColour(10);
	US_CPrint(name);

	US_SetPrintY(61);
	US_SetWindowX(80);
	US_SetWindowW(64);
	US_SetPrintColour(15);
	US_CPrint("EXTRA");
	VHB_Bar(79, 71, 66, 10, 0);
	CK_DrawLongRight(88, 72, 7, 6, ck_gameState.nextKeenAt);
	VHB_Bar(95, 80, 1, 1, 8);
	VHB_Bar(119, 80, 1, 1, 8);

	US_SetPrintY(85);
	US_SetWindowX(81);
	US_SetWindowW(64);
	US_CPrint("SCORE");
	VHB_Bar(79, 95, 66, 10, 0);
	CK_DrawLongRight(88, 96, 7, 6, ck_gameState.keenScore);
	VHB_Bar(95, 104, 1, 1, 8);
	VHB_Bar(119, 104, 1, 1, 8);

	US_SetPrintY(61);
	US_SetWindowX(177);
	US_SetWindowW(64);
	US_SetPrintColour(15);
	US_CPrint("LEVEL");
	VHB_Bar(175, 71, 66, 10, 0);
	US_SetPrintY(72);
	US_SetWindowX(176);
	US_SetWindowW(64);
	US_SetPrintColour(10);
	US_CPrint(ff_state.cheated ? "CHEATER!" : ck_gameState.difficulty == D_Easy ? "Easy" :
		ck_gameState.difficulty == D_Normal ? "Normal" : ck_gameState.difficulty == D_Hard ? "Hard" : "Expert");

	US_SetPrintX(194);
	US_SetPrintY(85);
	US_SetPrintColour(15);
	US_Print("KEYS");
	VHB_Bar(175, 95, 66, 10, 0);
	for (int i = 0, x = 184; i < 4; i++, x += 13)
		VHB_DrawTile8(x, 96, ck_gameState.keyGems[i] ? i : 4);

	US_SetPrintX(80);
	US_SetPrintY(144);
	US_Print("AMMO");
	VHB_Bar(127, 143, 18, 10, 0);
	CK_DrawLongRight(128, 144, 2, 6, ck_gameState.numShots);

	US_SetPrintX(80);
	US_SetPrintY(112);
	US_Print("KEENS");
	VHB_Bar(127, 111, 18, 10, 0);
	CK_DrawLongRight(128, 112, 2, 6, ck_gameState.numLives);

	US_SetPrintX(80);
	US_SetPrintY(128);
	US_Print("DROPS");
	VHB_Bar(127, 127, 18, 10, 0);
	CK_DrawLongRight(128, 128, 2, 6, ck_gameState.numCentilife);

	// The six collectibles. TODO: the source draws eight rows of each
	// sprite, dark grey while it is missing; here only those found, whole,
	// and only if the sprite is in memory.
	US_SetPrintY(117);
	US_SetWindowX(177);
	US_SetWindowW(64);
	US_CPrint("GOODIES");
	VHB_Bar(175, 127, 66, 10, 0);
	VHB_Bar(175, 143, 66, 10, 0);
	uint16_t bits = ff_state.collectiblesTotal | ff_map.collectiblesNew;
	for (int i = 0; i < 6; i++)
	{
		int chunk = FF_COLLECTIBLETEDDY1SPR + 2 * i;
		if ((bits & (1 << i)) && ca_graphChunks[chunk])
			VHB_DrawSprite(182 - 4 + 21 * (i % 3), i < 3 ? 128 : 144, chunk);
	}
	US_SetPrintColour(oldcolor);
}

// Its level setup: kw_ff_scan.c (ScanInfoPlane in CK_SPEC.C).
void FF_ScanInfoPlane(void);
void FF_ScanInfoLayer(void)
{
	memset(&ff_map, 0, sizeof(ff_map));
	ff_keenKilled = false;
	FF_ScanInfoPlane();
}

void kw_ff_map_SetupFunctions(void);

void FF_SetupFunctions(void)
{
	kw_ff_act_SetupFunctions();
	kw_ff_map_SetupFunctions();
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
