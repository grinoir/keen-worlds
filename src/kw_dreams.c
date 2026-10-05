/*
Keen Worlds: Keen Dreams (id Software, 1991) as a world - the dream Keen has
in the Megarocket's bed.

Keen Dreams is the engine Keen 4-6 grew out of, so it runs here as Keen 4
with its own level setup, Keen, items and creatures (kw_kd_keen.c,
kw_kd_act1.c, kw_kd_act2.c: its own source, converted by
tools/kw_dreams_port.py). This file is what its KD_PLAY.C did around them:
reading a level's info plane, scrolling, the map of Tuberia, and the rules of
dying and finishing a level.

The game's data is the player's own copy (the shareware release is free to
pass on), converted to the engine's file layout by tools/kw_dreams.py.

Licensed under the GNU GPL v2 or later, like Omnispeak and the Keen Dreams
source.
*/

#include "kw_dreams.h"

extern CK_EpisodeDef kd_episode;

extern int ck_activeX0Tile, ck_activeX1Tile, ck_activeY0Tile, ck_activeY1Tile;
extern CK_object *ck_scoreBoxObj;
void CK_DemoSignSpawn(void);

bool KD_IsActive(void)
{
	return ck_currentEpisode == &kd_episode;
}

// ---- Each frame, before anything thinks (the top of its PlayLoop) ----

void KD_FrameStart(void)
{
	KD_CalcSingleGravity();
	kd_c.xaxis = ck_inputFrame.xDirection;
	kd_c.yaxis = ck_inputFrame.yDirection;
	kd_c.dir = ck_inputFrame.dir;
	kd_c.button0 = ck_keenState.jumpIsPressed;
	// It has two buttons: jump and throw. Both of the engine's others throw.
	kd_c.button1 = ck_keenState.pogoIsPressed || ck_keenState.shootIsPressed;
	if (!kd_c.button0)
		kd_button0held = false;
	if (!kd_c.button1)
		kd_button1held = false;
	ck_keenState.platform = NULL; // nothing carries Keen along here
}

// ---- Scrolling (its ScrollScreen, and where a level first looks) ----

static void KD_CalcInactivate(void)
{
	int limit = CK_INT(CK_activeLimit, 6);
	ck_activeX0Tile = CK_Cross_max(RF_UnitToTile(rf_scrollXUnit) - limit, 0);
	ck_activeX1Tile = CK_Cross_max(RF_UnitToTile(rf_scrollXUnit) + RF_PixelToTile(320) + limit, 0);
	ck_activeY0Tile = CK_Cross_max(RF_UnitToTile(rf_scrollYUnit) - limit, 0);
	ck_activeY1Tile = CK_Cross_max(RF_UnitToTile(rf_scrollYUnit) + RF_PixelToTile(208) + limit, 0);
}

void KD_CentreCamera(CK_object *ob)
{
	int32_t orgx = (int32_t)ob->posX - RF_PixelToUnit(150);
	int32_t orgy = (int32_t)ob->posY - RF_PixelToUnit(84);
	if (orgx < 0)
		orgx = 0;
	if (orgy < 0)
		orgy = 0;
	RF_Reposition(orgx, orgy);
	KD_CalcInactivate();
}

void KD_ScrollScreen(void)
{
	CK_object *player = ck_keenObj;
	int16_t xscroll, yscroll;

	// walked off the edge of the map? That is how its levels end.
	if ((int)player->clipRects.unitX1 < rf_scrollXMinUnit ||
		(int)player->clipRects.unitX2 > rf_scrollXMaxUnit + 20 * 256)
	{
		ck_gameState.levelState = LS_LevelComplete;
		return;
	}

	if ((int)player->posX < rf_scrollXUnit + SCROLLWEST)
		xscroll = player->posX - (rf_scrollXUnit + SCROLLWEST);
	else if ((int)player->posX > rf_scrollXUnit + SCROLLEAST)
		xscroll = player->posX - (rf_scrollXUnit + SCROLLEAST);
	else
		xscroll = 0;

	if ((int)player->posY < rf_scrollYUnit + SCROLLNORTH)
		yscroll = player->posY - (rf_scrollYUnit + SCROLLNORTH);
	else if ((int)player->posY > rf_scrollYUnit + SCROLLSOUTH)
		yscroll = player->posY - (rf_scrollYUnit + SCROLLSOUTH);
	else
		yscroll = 0;

	// Its RF_Scroll stopped at the map's borders; the engine's smooth scroll
	// leaves that to the caller.
	if (rf_scrollXUnit + xscroll < rf_scrollXMinUnit)
		xscroll = rf_scrollXMinUnit - rf_scrollXUnit;
	else if (rf_scrollXUnit + xscroll > rf_scrollXMaxUnit)
		xscroll = rf_scrollXMaxUnit - rf_scrollXUnit;
	if (rf_scrollYUnit + yscroll < rf_scrollYMinUnit)
		yscroll = rf_scrollYMinUnit - rf_scrollYUnit;
	else if (rf_scrollYUnit + yscroll > rf_scrollYMaxUnit)
		yscroll = rf_scrollYMaxUnit - rf_scrollYUnit;

	if (xscroll || yscroll)
	{
		RF_SmoothScroll(xscroll, yscroll);
		KD_CalcInactivate();
		ck_scoreBoxObj->visible = true;
	}
}

// ---- The map of Tuberia (its PatchWorldMap): a finished level stops
// blocking the way, or gets a flag ----

void KD_PatchWorldMap(void)
{
	int width = CA_GetMapWidth(), size = width * CA_GetMapHeight();
	for (int spot = 0; spot < size; ++spot)
	{
		uint16_t info = CA_mapPlanes[2][spot];
		if (info < 3 || info > 18 || !ck_gameState.levelsDone[info - 2])
			continue;
		CA_mapPlanes[2][spot] = 0;
		uint16_t foreground = CA_mapPlanes[1][spot];
		if (foreground == 130)
			CA_mapPlanes[1][spot] = 0; // not blocking now
		else if (foreground == 90 && spot > width)
		{
			// plant a "done" flag
			CA_mapPlanes[1][spot] = 133;
			CA_mapPlanes[1][spot - width - 1] = 131;
			CA_mapPlanes[1][spot - width] = 132;
		}
	}
}

// ---- A level's info plane (its HandleInfo and ScanInfoPlane) ----

bool kd_bombspresent;

void KD_ScanInfoLayer(void)
{
	int mapW = CA_GetMapWidth(), mapH = CA_GetMapHeight();

	CA_ClearLumps();
	kd_bombspresent = false;
	kd_state.bombsthislevel = 0;

	if (ca_mapOn == 0)
		KD_PatchWorldMap();

	// Its sprites were loaded by "lumps", one per creature; there is room
	// for them all now.
	for (int i = 0; i < ca_gfxInfoE.numSprites; ++i)
		CA_MarkGrChunk(ca_gfxInfoE.offSprites + i);

	for (int y = 0; y < mapH; ++y)
	{
		for (int x = 0; x < mapW; ++x)
		{
			int tile = CA_TileAtPos(x, y, 2);
			if (!tile)
				continue;
			kd_new = NULL;
			switch (tile)
			{
			case 1:
				KD_SpawnKeen(x, y, 1);
				CK_DemoSignSpawn();
				break;
			case 2:
				KD_SpawnKeen(x, y, -1);
				CK_DemoSignSpawn();
				break;
			case 19:
				KD_SpawnWorldKeen(x, y);
				CK_DemoSignSpawn();
				break;
			case 31:
				kd_bombspresent = true;
				// fall through
			case 21: case 22: case 23: case 24: case 25: case 26:
			case 27: case 28: case 29: case 30: case 32:
				KD_SpawnBonus(x, y, tile - 21);
				break;
			case 33:
				KD_SpawnDoor(x, y);
				break;
			case 41:
				KD_SpawnBrocco(x, y);
				break;
			case 42:
				KD_SpawnTomat(x, y);
				break;
			case 43:
				KD_SpawnCarrot(x, y);
				break;
			case 45:
				KD_SpawnAspar(x, y);
				break;
			case 46:
				KD_SpawnGrape(x, y);
				break;
			case 47:
				KD_SpawnTater(x, y);
				break;
			case 48:
				KD_SpawnCart(x, y);
				break;
			case 49:
				KD_SpawnFrenchy(x, y);
				break;
			case 50: case 51: case 52:
				KD_SpawnMelon(x, y, tile - 50);
				break;
			case 57:
				KD_SpawnSquasher(x, y);
				break;
			case 58:
				KD_SpawnApel(x, y);
				break;
			case 59:
				KD_SpawnPeaPod(x, y);
				break;
			case 60:
				KD_SpawnPeaBrain(x, y);
				break;
			case 61:
				KD_SpawnBoobus(x, y);
				break;
			}
			// Everything it spawned sleeps until it scrolls into view,
			// unless it asked to be always awake.
			if (kd_new && kd_new->active != OBJ_ALWAYS_ACTIVE)
				kd_new->active = OBJ_INACTIVE;
		}
	}
}

// ---- Dying (the top of its HandleDeath): the keys go, and so do the
// bombs picked up in this level ----

void KD_KeenDied(void)
{
	kd_state.keys = 0;
	kd_state.boobusbombs -= kd_state.bombsthislevel;
	kd_state.bombsthislevel = 0;
}

// ---- A new dream (its NewGame) ----

void KD_NewGame(void)
{
	memset(&kd_state, 0, sizeof(kd_state));
}

void KD_SetupFunctions(void)
{
	KD_Keen_SetupFunctions();
	KD_Act1_SetupFunctions();
	KD_Act2_SetupFunctions();
}

// The player's game files, converted (tools/kw_dreams.py), and our support
// files, all in the world's own folder.
bool KD_IsPresent(void)
{
	static const char *files[] = {"EGAGRAPH.KDR", "EGAHEAD.KDR", "EGADICT.KDR", "GAMEMAPS.KDR", "MAPHEAD.KDR",
		"AUDIO.KDR", "AUDIOHHD.KDR", "AUDIODCT.KDR", "GFXINFOE.KDR", "AUDINFOE.KDR", "KDACTION.KDR"};
	for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
		if (!FS_IsKeenFilePresent(files[i]))
			return false;
	return true;
}

// The engine treats it as Keen 4 (its descendant); only the file extension
// and the level setup differ.
CK_EpisodeDef kd_episode = {
	EP_CK4,
	"KDR",
	"DREAMS.KDR",
	&KD_ScanInfoLayer,
	&KD_IsPresent,
};

// ---- The box shown while a level loads: a plain window (its own said
// "Loading"; Keen 4's counts down with pictures it doesn't have) ----

void KD_BeginCacheBox(const char *title, int numChunks)
{
	uint16_t w, h;
	(void)numChunks;
	US_CenterWindow(20, 8);
	CK_MeasureMultiline(title, &w, &h);
	US_SetPrintY(US_GetPrintY() + (US_GetWindowH() - h) / 2 - 4);
	US_CPrint(title);
	VH_UpdateScreen();
}

// ---- Its part of a saved game: three words in Keen Worlds' block ----

void KD_GetState(int16_t *out)
{
	out[0] = kd_state.boobusbombs;
	out[1] = kd_state.bombsthislevel;
	out[2] = kd_state.keys;
}

void KD_SetState(const int16_t *in)
{
	kd_state.boobusbombs = in[0];
	kd_state.bombsthislevel = in[1];
	kd_state.keys = in[2];
}

// ---- The status window (its StatusWindow, which its own source calls a
// placeholder): what the score box has no room for ----

void KD_StatusWindow(void)
{
	char text[200];
	VL_FixRefreshBuffer();
	US_CenterWindow(26, 10);
	US_SetPrintY(US_GetPrintY() + 4);
	US_CPrint(CK_STRINGELEMENT(ck_str_levelNames, ca_mapOn));
	US_SetPrintY(US_GetPrintY() + 6);
	snprintf(text, sizeof(text), "Score %d\nNext Keen at %d\nKeens %d   Flower power %d\nBoobus bombs %d of %d\nKeys %d",
		(int)ck_gameState.keenScore, (int)ck_gameState.nextKeenAt, ck_gameState.numLives, ck_gameState.numShots,
		kd_state.boobusbombs, KD_BOMBSNEEDED, kd_state.keys);
	US_CPrint(text);
	VH_UpdateScreen();
	IN_ClearKeysDown();
	IN_WaitButton();
	IN_ClearKeysDown();
}

// ---- The ending (its GameFinale): three windows of text, in its words ----

static void KD_FinalePage(int down, const char *text)
{
	US_CenterWindow(30, 21);
	US_SetPrintY(US_GetPrintY() + down);
	US_CPrint(text);
	VH_UpdateScreen();
	VL_DelayTics(60);
	IN_ClearKeysDown();
	IN_WaitButton();
}

void KD_GameFinale(void)
{
	VL_FixRefreshBuffer();
	SD_WaitSoundDone();
	KD_FinalePage(4,
		"Yes!  Boobus Tuber's hash-brown-\n"
		"like remains rained down from\n"
		"the skies as Commander Keen\n"
		"walked up to the Dream Machine.\n"
		"He analyzed all the complex\n"
		"controls and readouts on it, then\n"
		"pulled down a huge red lever\n"
		"marked \"On/Off Switch.\"  The\n"
		"machine clanked and rattled,\n"
		"then went silent. He had freed\n"
		"all the children from their\n"
		"vegetable-enforced slavery!\n"
		"Everything around Keen wobbled\n"
		"in a disconcerting manner, his\n"
		"eyelids grew heavy, and he\n"
		"fell asleep....\n");
	KD_FinalePage(9,
		"Billy woke up, looking around the\n"
		"room, the early morning sun\n"
		"shining in his face.  Nothing.\n"
		"No vegetables to be seen.  Was it\n"
		"all just a dream?\n\n"
		"Billy's mom entered the room.\n\n"
		"\"Good morning, dear. I heard some\n"
		"news on TV that you'd be\n"
		"interested in,\" she said, sitting\n"
		"by him on the bed.\n\n"
		"\"What news?\" Billy asked,\n"
		"still groggy.\n\n");
	KD_FinalePage(23,
		"\"The President declared today\n"
		"National 'I Hate Broccoli' Day.\n"
		"He said kids are allowed to pick\n"
		"one vegetable today, and they\n"
		"don't have to eat it.\"\n\n"
		"\"Aw, mom, I'm not afraid of any\n"
		"stupid vegetables,\" Billy said.\n"
		"\"But if it's okay with you, I'd\n"
		"rather not have any french fries\n"
		"for awhile.\"\n\n"
		"THE END");
	IN_ClearKeysDown();
}

// Is this one of the creatures flower power turns into a flower? (The list
// of its PowerContact.)
bool KD_IsVegetable(const CK_object *ob)
{
	switch (ob->type)
	{
	case KD_broccoobj: case KD_tomatobj: case KD_carrotobj: case KD_celeryobj: case KD_asparobj:
	case KD_taterobj: case KD_frenchyobj: case KD_squashobj: case KD_apelobj: case KD_peapodobj:
	case KD_peabrainobj:
		return true;
	}
	return false;
}

// ---- The score box (its FixScoreBox and ScoreThink): the engine's, which
// asks here what the third number is and which picture goes with it ----

int KD_ScoreBoxShots(void)
{
	return ca_mapOn == KD_BOOBUSLEVEL ? kd_state.boobusbombs : ck_gameState.numShots;
}

int KD_ScoreBoxIcon(void)
{
	return ca_mapOn == KD_BOOBUSLEVEL ? 20 : 28; // small tiles: the bomb, the flower
}
