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
#include "kw_ff_keen.h"

extern CK_EpisodeDef ff_episode;

FF_State ff_state;
FF_MapState ff_map;
bool ff_keenKilled, ff_keenUpHeld, ff_keenMustAdjustNextX;
int ff_brainfucled;

bool FF_IsActive(void)
{
	return ck_currentEpisode == &ff_episode;
}

// ---- Its strings (KD_SM.C): GAMESTR.CKS is a table of 16-bit offsets, one
// per string and one more, then the texts. Read once, when first asked. ----

const char *FF_String(int number)
{
	static uint8_t *data;
	static size_t size;
	if (!data)
	{
		FS_File f = FS_OpenKeenFile("GAMESTR.CKS");
		if (!FS_IsFileValid(f))
			Quit("Foray in the Forest: GAMESTR.CKS is missing");
		size = FS_GetFileSize(f);
		data = (uint8_t *)malloc(size + 1);
		if (!data || FS_Read(data, 1, size, f) != size)
			Quit("Foray in the Forest: can't read GAMESTR.CKS");
		data[size] = 0;
		FS_CloseFile(f);
		if (size < 2 * (FF_NUMGAMESTRINGS + 1) || (data[0] | (data[1] << 8)) != 2 * (FF_NUMGAMESTRINGS + 1))
			Quit("Foray in the Forest: GAMESTR.CKS is not the version this was built for");
	}
	if (number < 0 || number >= FF_NUMGAMESTRINGS)
		return "";
	size_t off = data[2 * number] | (data[2 * number + 1] << 8);
	return off < size ? (const char *)data + off : "";
}

// (FF_CalcTileClipRects, FF_MoveObjVert and FF_MoveObjHoriz are in
// kw_ff_state.c with the rest of its movement code.)

// ---- Not ported yet: parts of its CK_GAME.C, CK_PLAY.C and CK_SPEC.C that
// Keen and the creatures call. Each does the least that keeps the game
// going. ----

// ---- Checkpoints (SaveCheckpoint, LoadCheckpoint, SaveGameStuff and
// LoadGameStuff in CK_GAME.C). The source writes a CHKPOINT file holding the
// game state, the map and the whole object array; here the same is kept in
// memory, so a checkpoint doesn't outlive the level or the program. ----

extern CK_object ck_objArray[CK_MAX_OBJECTS];
extern CK_object *ck_freeObject, *ck_lastObject;
extern int ck_numObjects;

static struct
{
	bool valid;
	int level;
	CK_GameState gameState;
	FF_State state;
	FF_MapState map;
	int brainfucled;
	uint16_t *planes[3];
	size_t planeBytes;
	CK_object objects[CK_MAX_OBJECTS];
	CK_object *freeObject, *lastObject;
	int numObjects;
	int scrollX, scrollY;
} ff_checkpoint;

static void FF_ClearCheckpoint(void)
{
	for (int i = 0; i < 3; ++i)
	{
		free(ff_checkpoint.planes[i]);
		ff_checkpoint.planes[i] = NULL;
	}
	ff_checkpoint.valid = false;
}

bool FF_HasCheckpoint(void)
{
	return FF_IsActive() && ff_checkpoint.valid && ff_checkpoint.level == ca_mapOn && ff_map.checkpointSaved;
}

bool FF_SaveCheckpoint(void)
{
	FF_ClearCheckpoint();
	ff_map.checkpointSaved = true;
	ff_checkpoint.level = ca_mapOn;
	ff_checkpoint.gameState = ck_gameState;
	ff_checkpoint.state = ff_state;
	ff_checkpoint.map = ff_map;
	ff_checkpoint.brainfucled = ff_brainfucled;
	ff_checkpoint.planeBytes = (size_t)CA_GetMapWidth() * CA_GetMapHeight() * 2;
	for (int i = 0; i < 3; ++i)
	{
		ff_checkpoint.planes[i] = (uint16_t *)malloc(ff_checkpoint.planeBytes);
		if (!ff_checkpoint.planes[i])
		{
			FF_ClearCheckpoint();
			return ff_map.checkpointSaved = false;
		}
		memcpy(ff_checkpoint.planes[i], CA_mapPlanes[i], ff_checkpoint.planeBytes);
	}
	memcpy(ff_checkpoint.objects, ck_objArray, sizeof(ck_objArray));
	ff_checkpoint.freeObject = ck_freeObject;
	ff_checkpoint.lastObject = ck_lastObject;
	ff_checkpoint.numObjects = ck_numObjects;
	ff_checkpoint.scrollX = rf_scrollXUnit;
	ff_checkpoint.scrollY = rf_scrollYUnit;
	ff_checkpoint.valid = true;
	return true;
}

// The sprites an object has on screen (its own, and the extra ones some
// kinds keep in their temp fields) come off before the objects change.
static void FF_RemoveObjectSprites(CK_object *ob)
{
	RF_RemoveSpriteDraw(&ob->sde);
	switch (ob->type)
	{
	case FF_CT_FallBlock:
	case CT5_StunnedCreature:
	case FF_CT_Slug:
	case FF_CT_Checkpoint:
		RF_RemoveSpriteDrawUsing16BitOffset(&ob->user3);
		break;
	case CT5_Platform:
		RF_RemoveSpriteDrawUsing16BitOffset(&ob->user2);
		RF_RemoveSpriteDrawUsing16BitOffset(&ob->user3);
		break;
	}
}

bool FF_LoadCheckpoint(void)
{
	if (!ff_checkpoint.valid || ff_checkpoint.level != ca_mapOn ||
		ff_checkpoint.planeBytes != (size_t)CA_GetMapWidth() * CA_GetMapHeight() * 2)
		return false;

	CK_GameState old = ck_gameState;
	FF_State oldState = ff_state;
	for (CK_object *ob = ck_keenObj; ob; ob = ob->next)
		FF_RemoveObjectSprites(ob);

	ck_gameState = ff_checkpoint.gameState;
	ff_state = ff_checkpoint.state;
	if (ck_gameState.numShots < DEFAULT_SHOTS)
		ck_gameState.numShots = DEFAULT_SHOTS;
	if (ck_gameState.numShots < old.numShots)
		ck_gameState.numShots = old.numShots;
	if (!(oldState.cheated && !ff_state.cheated && old.numLives > ck_gameState.numLives))
		ck_gameState.numLives = old.numLives;
	ck_gameState.nextKeenAt = old.nextKeenAt;
	ck_gameState.numCentilife = old.numCentilife;
	ck_gameState.levelState = old.levelState;
	ff_state.janitorMessageShown = oldState.janitorMessageShown;
	ff_state.savebits = oldState.savebits;

	ff_map = ff_checkpoint.map;
	ff_brainfucled = ff_checkpoint.brainfucled;
	for (int i = 0; i < 3; ++i)
		memcpy(CA_mapPlanes[i], ff_checkpoint.planes[i], ff_checkpoint.planeBytes);
	memcpy(ck_objArray, ff_checkpoint.objects, sizeof(ck_objArray));
	ck_freeObject = ff_checkpoint.freeObject;
	ck_lastObject = ff_checkpoint.lastObject;
	ck_numObjects = ff_checkpoint.numObjects;
	ck_keenState.platform = NULL;
	ff_keenKilled = false;

	for (CK_object *ob = ck_keenObj; ob; ob = ob->next)
	{
		ob->visible = true; // force redraw
		ob->sde = NULL;
		switch (ob->type)
		{
		case FF_CT_FallBlock:
		case CT5_StunnedCreature:
		case FF_CT_Slug:
		case FF_CT_Checkpoint:
			ob->user3 = 0; // the stars, foreground, donut or overlay sprite
			break;
		case CT5_Platform:
			ob->user2 = ob->user3 = 0; // the thrusters
			break;
		}
	}
	FF_ForceScoreboxUpdate();
	RF_Reposition(ff_checkpoint.scrollX, ff_checkpoint.scrollY);
	RF_ForceRefresh();
	return true;
}

static void FF_ApplyDifficulty(bool newGame);

// ---- Out of lives (GameOver and CK_ContinueMenu in CK_GAME.C, and what
// its GameLoop does with the answer): the game over window, then the offer
// to continue (with a remark that changes each time, and a bar that runs
// out: "Quit" is chosen by itself), then the jokes it tells about
// continuing. Continuing puts Keen back on the map with the lives and ammo
// of his difficulty. Returns true to go on playing. ----

void CK_GameOver(void);
bool KW_DebugWait(void);
static bool ff_gameOverShown;

bool FF_GameOverWasShown(void)
{
	bool shown = ff_gameOverShown;
	ff_gameOverShown = false;
	return shown;
}

static void FF_SelectionBox(int y, int color)
{
	int x1 = US_GetWindowX() + 3, x2 = US_GetWindowX() + US_GetWindowW() - 4;
	VHB_HLine(x1, x2, y, color);
	VHB_HLine(x1, x2, y + 1, color);
	VHB_HLine(x1, x2, y + 12, color);
	VHB_HLine(x1, x2, y + 13, color);
	VHB_VLine(y + 1, y + 11, x1 + 1, color);
	VHB_VLine(y + 1, y + 11, x1, color);
	VHB_VLine(y + 1, y + 11, x2, color);
	VHB_VLine(y + 1, y + 11, x2 - 1, color);
}

static bool FF_ContinueMenu(void)
{
	const int bottomHeight = 36;
	uint16_t w, h;
	VHB_Bar(0, 0, 320, 200, 0);
	int msg = ff_state.continuesUsed + FF_S_RANOUTOFLIVES;
	if (msg > FF_S_RANOUTOFJOKES)
		msg = FF_S_RANOUTOFJOKES;
	else
		ff_state.continuesUsed++;
	CK_MeasureMultiline(FF_String(msg), &w, &h);
	int tw = (w + 7) / 8, th = (h + bottomHeight + 7) / 8;
	US_CenterWindow(tw > 26 ? tw : 26, th > 8 ? th : 8);
	US_SetPrintY(US_GetPrintY() + (US_GetWindowH() - h - bottomHeight) / 2);
	US_CPrint(FF_String(msg));

	int y1 = US_GetWindowY() + US_GetWindowH() - bottomHeight;
	US_SetPrintY(y1 + 2);
	US_CPrint(FF_String(FF_S_CONTINUE));
	US_SetPrintY(US_GetPrintY() + 4);
	int y2 = US_GetPrintY() - 2;
	US_CPrint(FF_String(FF_S_QUIT));
	IN_ClearKeysDown();
	VH_UpdateScreen();
	if (KW_DebugWait())
		return false; // (developer tool: nobody to choose)

	int sel = 1, timeout = (US_GetWindowW() - 8) * 4;
	while (IN_GetLastScan() != IN_SC_Escape)
	{
		IN_PumpEvents();
		VL_DelayTics(1);
		int y = sel ? y2 : y1;
		FF_SelectionBox(y, ((SD_GetTimeCount() >> 4) & 1) ? 12 : 1);
		if (timeout)
			VHB_Bar(US_GetWindowX() + 4, US_GetWindowY() + US_GetWindowH() - 6, timeout / 4, 3, 2);
		VH_UpdateScreen();
		FF_SelectionBox(y, 15);
		VHB_Bar(US_GetWindowX() + 4, US_GetWindowY() + US_GetWindowH() - 6, US_GetWindowW() - 8, 3, 15);

		IN_ReadControls(0, &ck_inputFrame);
		if (ck_inputFrame.jump || ck_inputFrame.pogo || IN_GetLastScan() == IN_SC_Enter || IN_GetLastScan() == IN_SC_Space)
		{
			IN_ClearKeysDown();
			return !sel;
		}
		if (ck_inputFrame.yDirection == IN_motion_Up || IN_GetLastScan() == IN_SC_UpArrow)
		{
			sel = 0;
			timeout = 0;
			IN_ClearKeysDown();
		}
		else if (ck_inputFrame.yDirection == IN_motion_Down || IN_GetLastScan() == IN_SC_DownArrow)
		{
			sel = 1;
			timeout = 0;
			IN_ClearKeysDown();
		}
		if (timeout > 0 && --timeout == 0)
			break;
	}
	IN_ClearKeysDown();
	return false;
}

bool FF_GameOverContinue(void)
{
	CK_GameOver();
	ff_gameOverShown = true; // the engine's own game over doesn't show it again
	if (!FF_ContinueMenu())
		return false;
	ff_gameOverShown = false;

	// the jokes: one or more windows, up to an empty string
	int winW = US_GetWindowW() / 8, winH = US_GetWindowH() / 8;
	int joke = ff_state.jokeNumber + FF_S_CONTINUEJOKE1;
	if (joke > FF_S_FINALCONTINUEJOKE)
		joke = FF_S_FINALCONTINUEJOKE;
	while (joke < FF_NUMGAMESTRINGS && *FF_String(joke))
	{
		uint16_t strW, strH;
		CK_MeasureMultiline(FF_String(joke), &strW, &strH);
		if ((strW + 7) / 8 > winW)
			winW = (strW + 7) / 8;
		if ((strH + 7) / 8 > winH)
			winH = (strH + 7) / 8;
		US_CenterWindow(winW, winH);
		US_SetPrintY(US_GetPrintY() + (US_GetWindowH() - strH) / 2);
		US_CPrint(FF_String(joke));
		VH_UpdateScreen();
		IN_WaitButton();
		joke++;
	}
	ff_state.jokeNumber = joke + 1 - FF_S_CONTINUEJOKE1;
	VHB_Bar(0, 0, 320, 200, 0);

	ff_state.oldscore = 0;
	ck_gameState.nextKeenAt = 20000;
	ck_gameState.numCentilife = 0;
	ck_gameState.currentLevel = 0;
	FF_ApplyDifficulty(true);
	return true;
}

// ---- Saved games (Keen Worlds' save block, kw_world.c). Its progress
// (FF_State) always; and, for a game saved inside one of its levels, the
// level's state and where each object sits in the object array: its code
// points at objects by their place there, and the engine loads objects
// packed in list order. A checkpoint isn't saved. ----

extern CK_object *ck_scoreBoxObj;

static int16_t FF_Slot(CK_object *ob)
{
	return (ob >= ck_objArray && ob < ck_objArray + CK_MAX_OBJECTS) ? (int16_t)(ob - ck_objArray) : -1;
}
static CK_object *FF_AtSlot(int16_t slot)
{
	return (slot >= 0 && slot < CK_MAX_OBJECTS) ? &ck_objArray[slot] : NULL;
}

#define FF_SAVE_STATE_WORDS 14
#define FF_SAVE_MAP_WORDS 29

static void FF_PackMap(const FF_MapState *m, int16_t *v)
{
	int16_t w[FF_SAVE_MAP_WORDS] = {(int16_t)m->collectiblesNew, (int16_t)m->slugcans, m->nextLevel, (int16_t)m->doneFlags,
		(int16_t)m->dreamMsg, (int16_t)m->warpInfo, -1 /* was: riding */, FF_Slot(m->checkpointTouched),
		FF_Slot(m->lastCheckpoint), FF_Slot(m->nisasi), FF_Slot(m->mindcontrolled), FF_Slot(m->quicksand),
		FF_Slot(m->mummy), m->playerkludgeclipcancel, m->invincibilityTimer, m->stunCounter, m->stunCooldown,
		m->moontile, m->inwater, m->brainfucl, m->machinesActive, m->music, m->shaketics, m->shakecount,
		m->screenYpx, m->wrapScroll, (int16_t)m->itemsTotal, (int16_t)m->itemsCollected, (int16_t)ff_brainfucled};
	memcpy(v, w, sizeof(w));
}

static void FF_UnpackMap(const int16_t *v, FF_MapState *m)
{
	memset(m, 0, sizeof(*m));
	m->collectiblesNew = v[0]; m->slugcans = v[1]; m->nextLevel = v[2]; m->doneFlags = v[3];
	m->dreamMsg = v[4]; m->warpInfo = v[5];
	m->checkpointTouched = NULL; m->lastCheckpoint = FF_AtSlot(v[8]);
	m->nisasi = FF_AtSlot(v[9]); m->mindcontrolled = FF_AtSlot(v[10]); m->quicksand = FF_AtSlot(v[11]);
	m->mummy = FF_AtSlot(v[12]);
	m->playerkludgeclipcancel = v[13] != 0; m->invincibilityTimer = v[14]; m->stunCounter = v[15];
	m->stunCooldown = v[16]; m->moontile = v[17]; m->inwater = v[18]; m->brainfucl = v[19];
	m->machinesActive = v[20]; m->music = v[21]; m->shaketics = v[22]; m->shakecount = v[23];
	m->screenYpx = v[24]; m->wrapScroll = v[25]; m->itemsTotal = v[26]; m->itemsCollected = v[27];
	ff_brainfucled = v[28];
	m->checkpointSaved = false; // a checkpoint isn't in the save
}

bool FF_WriteState(FS_File fp)
{
	int16_t s[FF_SAVE_STATE_WORDS] = {(int16_t)(ff_state.oldscore & 0xFFFF), (int16_t)(ff_state.oldscore >> 16),
		ff_state.drops, (int16_t)ff_state.collectiblesTotal, (int16_t)ff_state.janitorMessageShown,
		(int16_t)ff_state.slugcans, ff_state.cheated, ff_state.brainfucled, (int16_t)ff_state.continuesUsed,
		(int16_t)ff_state.jokeNumber, ff_state.savebits, (int16_t)ff_state.freeRestartsPerCheckpoint,
		ff_state.maxammo, ff_state.shotsinclip};
	// Is the game being saved inside one of its levels (or on its map)?
	int16_t count = FF_IsActive() ? (int16_t)ck_numObjects : 0;
	if (FS_WriteInt16LE(s, FF_SAVE_STATE_WORDS, fp) != FF_SAVE_STATE_WORDS || FS_WriteInt16LE(&count, 1, fp) != 1)
		return false;
	if (!count)
		return true;
	int16_t m[FF_SAVE_MAP_WORDS];
	FF_PackMap(&ff_map, m);
	if (FS_WriteInt16LE(m, FF_SAVE_MAP_WORDS, fp) != FF_SAVE_MAP_WORDS)
		return false;
	int16_t written = 0;
	for (CK_object *ob = ck_keenObj; ob; ob = ob->next, ++written)
	{
		int16_t slot = FF_Slot(ob);
		if (FS_WriteInt16LE(&slot, 1, fp) != 1)
			return false;
	}
	// (the count written above must be the number of slots that follow)
	return written == count;
}

static struct
{
	bool pending;
	int16_t map[FF_SAVE_MAP_WORDS];
	int16_t count;
	int16_t slots[CK_MAX_OBJECTS];
} ff_loaded;

bool FF_ReadState(FS_File fp)
{
	int16_t s[FF_SAVE_STATE_WORDS], count;
	ff_loaded.pending = false;
	if (FS_ReadInt16LE(s, FF_SAVE_STATE_WORDS, fp) != FF_SAVE_STATE_WORDS || FS_ReadInt16LE(&count, 1, fp) != 1)
		return false;
	ff_state.oldscore = (int32_t)(((uint32_t)(uint16_t)s[1] << 16) | (uint16_t)s[0]);
	ff_state.drops = s[2]; ff_state.collectiblesTotal = s[3]; ff_state.janitorMessageShown = s[4];
	ff_state.slugcans = s[5]; ff_state.cheated = s[6] != 0; ff_state.brainfucled = s[7];
	ff_state.continuesUsed = s[8]; ff_state.jokeNumber = s[9]; ff_state.savebits = (uint8_t)s[10];
	ff_state.freeRestartsPerCheckpoint = s[11]; ff_state.maxammo = s[12]; ff_state.shotsinclip = s[13];
	if (!count)
		return true;
	if (count < 0 || count > CK_MAX_OBJECTS ||
		FS_ReadInt16LE(ff_loaded.map, FF_SAVE_MAP_WORDS, fp) != FF_SAVE_MAP_WORDS ||
		FS_ReadInt16LE(ff_loaded.slots, count, fp) != (size_t)count)
		return false;
	ff_loaded.count = count;
	ff_loaded.pending = true;
	return true;
}

// The engine has loaded the save's level and objects (packed into the
// first places of the object array, in list order): put every object back
// in the place it had when the game was saved, then the level's state.
static void FF_ApplyLoadedState(void)
{
	ff_loaded.pending = false;
	int n = 0;
	for (CK_object *ob = ck_keenObj; ob; ob = ob->next)
		++n;
	bool ok = FF_IsActive() && n == ff_loaded.count && n == ck_numObjects && ff_loaded.slots[0] == 0;
	static bool used[CK_MAX_OBJECTS];
	memset(used, 0, sizeof(used));
	for (int i = 0; ok && i < n; ++i)
	{
		int16_t slot = ff_loaded.slots[i];
		if (slot < 0 || slot >= CK_MAX_OBJECTS || used[slot] || ck_keenObj + i != &ck_objArray[i])
			ok = false;
		else
			used[slot] = true;
	}
	if (!ok)
		return; // not the save's level after all: leave everything as loaded
	static CK_object packed[CK_MAX_OBJECTS];
	int scoreBoxIndex = (int)(ck_scoreBoxObj - ck_objArray);
	memcpy(packed, ck_objArray, sizeof(CK_object) * n);
	for (int i = 0; i < n; ++i)
	{
		CK_object *ob = &ck_objArray[ff_loaded.slots[i]];
		*ob = packed[i];
		ob->prev = i ? &ck_objArray[ff_loaded.slots[i - 1]] : NULL;
		ob->next = i + 1 < n ? &ck_objArray[ff_loaded.slots[i + 1]] : NULL;
		ob->sde = NULL;
		ob->visible = true;
		switch (ob->type)
		{
		case FF_CT_FallBlock:
		case CT5_StunnedCreature:
		case FF_CT_Slug:
		case FF_CT_Checkpoint:
			ob->user3 = 0; // the stars, foreground, donut or overlay sprite
			break;
		case CT5_Platform:
			ob->user2 = ob->user3 = 0; // the thrusters
			break;
		}
	}
	ck_lastObject = &ck_objArray[ff_loaded.slots[n - 1]];
	if (scoreBoxIndex >= 0 && scoreBoxIndex < n)
		ck_scoreBoxObj = &ck_objArray[ff_loaded.slots[scoreBoxIndex]];
	// the free places, chained as the engine chains them (by "prev")
	ck_freeObject = NULL;
	for (int slot = CK_MAX_OBJECTS - 1; slot >= 0; --slot)
		if (!used[slot])
		{
			memset(&ck_objArray[slot], 0, sizeof(CK_object));
			ck_objArray[slot].prev = ck_freeObject;
			ck_freeObject = &ck_objArray[slot];
		}
	FF_UnpackMap(ff_loaded.map, &ff_map);
	FF_ForceScoreboxUpdate();
}

void FF_PlayLoopStart(void);

static int16_t ff_songsHeard[FF_LASTMUSIC];

void FF_ResetState(void)
{
	memset(ff_songsHeard, 0, sizeof(ff_songsHeard));
	memset(&ff_state, 0, sizeof(ff_state));
	ff_loaded.pending = false;
}

// For /KWTEST: a save made in one of its levels comes back with every object
// in its place and the level's state. Returns what is wrong, or NULL.
const char *FF_TestSaveRoundTrip(bool (*save)(void), bool (*load)(void))
{
	// make a gap in the object array, and state worth checking
	CK_object *victim = NULL, *marked = NULL;
	for (CK_object *ob = ck_keenObj->next ? ck_keenObj->next->next : NULL; ob && ob->next; ob = ob->next)
	{
		if (!victim)
			victim = ob;
		else
			marked = ob; // ends as the last but one
	}
	if (!victim || !marked)
		return NULL; // too few objects in this level to say anything
	CK_RemoveObj(victim);
	// (CK_RemoveObj leaves the place free one frame later in the play loop;
	// the place is free now for our purpose)
	int markedSlot = FF_Slot(marked), markedType = marked->type, markedX = marked->posX;
	ff_map.lastCheckpoint = marked;
	ff_map.itemsCollected = 7;
	ff_map.music = 9;
	ff_state.slugcans = 5;
	ff_state.collectiblesTotal = 0x21;
	int count = ck_numObjects;
	if (!save())
		return "couldn't write a save";
	ff_map.lastCheckpoint = NULL;
	ff_map.itemsCollected = 0;
	ff_state.slugcans = 0;
	ff_state.collectiblesTotal = 0;
	if (!load())
		return "couldn't read the save";
	FF_PlayLoopStart();
	if (ff_state.slugcans != 5 || ff_state.collectiblesTotal != 0x21)
		return "its progress wasn't loaded";
	if (ck_numObjects != count)
		return "the number of objects changed";
	if (ff_map.itemsCollected != 7 || ff_map.music != 9)
		return "the level's state wasn't loaded";
	if (ff_map.lastCheckpoint != &ck_objArray[markedSlot] || ff_map.lastCheckpoint->type != markedType ||
		ff_map.lastCheckpoint->posX != markedX)
		return "an object isn't in the place it was saved in";
	int n = 0;
	for (CK_object *ob = ck_keenObj; ob; ob = ob->next)
	{
		if (ob->next && ob->next->prev != ob)
			return "the object list is broken";
		if (!ob->next && ob != ck_lastObject)
			return "the last object is wrong";
		++n;
	}
	if (n != count)
		return "the object list has the wrong length";
	// a new object must get a free place, not one in use
	CK_object *fresh = CK_GetNewObj(false);
	for (CK_object *ob = ck_keenObj; ob; ob = ob->next)
		if (ob != fresh && ob == &ck_objArray[FF_Slot(fresh)])
			return "a place in use was handed out again";
	return NULL;
}

// The play loop is about to start (the engine calls this in this world).
void FF_PlayLoopStart(void)
{
	// Developer tool: KW_AUTOFOOBS=1 shows its "Foobs in Space!" screen once
	// (KW_AUTOFOOBS=snailas: its "Snailas" screen)
	static bool foobsShown = false;
	if (getenv("KW_AUTOFOOBS") && !foobsShown)
	{
		foobsShown = true;
		if (!strcmp(getenv("KW_AUTOFOOBS"), "snailas"))
			FF_RunSnailas();
		else
			FF_RunFoobsInSpace();
		RF_ForceRefresh();
	}
	if (ff_loaded.pending)
	{
		FF_ApplyLoadedState();
		// A game saved after a checkpoint was touched: the checkpoint itself
		// (a copy of the whole level, kept in memory) is not in the saved
		// game, so the moment of the save stands in for it. Dying then goes
		// back to where the game was saved, not to the start of the level.
		// (A checkpoint still in memory is from the game that was being
		// played before the load.)
		if (ff_map.checkpointSaved)
			FF_SaveCheckpoint();
		else
			FF_ClearCheckpoint();
	}
}

// ---- Dying (HandleDeath in CK_GAME.C). A touched checkpoint gives a number
// of free restarts by difficulty; after those a restart costs a life like
// any other. The menu itself is the engine's ("try again" or "restart from
// checkpoint", and "exit to the map"). TODO: its third choice, loading the
// last saved game. Returns true to go on playing from the checkpoint. ----

bool CK_TryAgainMenu(void);

bool FF_HandleDeath(void)
{
	int checkpointCounter = 0;
	bool gotFreeRestart = false;
	int level = ca_mapOn;
	if (ff_map.checkpointSaved && ff_map.lastCheckpoint && ff_checkpoint.valid)
	{
		checkpointCounter = ff_map.lastCheckpoint->user1;
		if (checkpointCounter <= (int)ff_state.freeRestartsPerCheckpoint)
		{
			gotFreeRestart = true;
			checkpointCounter++;
		}
	}
	// The menu takes a life and only shows while one is left; a free restart
	// gives it back.
	if (gotFreeRestart)
		ck_gameState.numLives++;
	CK_TryAgainMenu();
	if (ck_gameState.numLives < 0)
		return false; // game over
	if (ck_gameState.currentLevel != level)
	{
		// left for the map: the free restart wasn't used
		if (gotFreeRestart)
			ck_gameState.numLives--;
		return false;
	}
	if (checkpointCounter && FF_CheckpointLoad())
	{
		ck_gameState.levelState = LS_Playing;
		if (ff_map.lastCheckpoint)
			ff_map.lastCheckpoint->user1 = checkpointCounter;
		return true;
	}
	return false; // the level starts over
}

// Its dialogue windows are in kw_ff_dialog.c. What they call:

void FF_FadeOut(void)
{
	VL_FadeToBlack();
}

// The score box is redrawn next frame.
void FF_ForceScoreboxUpdate(void)
{
	if (ck_scoreBoxObj)
		ck_scoreBoxObj->user1 = ck_scoreBoxObj->user2 = ck_scoreBoxObj->user3 = ck_scoreBoxObj->user4 = -1;
}

// Its jukebox, the Photachion Transceiver in the ship (CK_Jukebox in
// CK_PLAY.C): pick a song from its playlist and play it; on leaving, the
// level goes on with the song that is playing. Only songs already heard in
// play can be chosen, as in the game.
static int ff_playingSong = -1;
// The songs heard in play so far (its gamestate.musicplayed): the jukebox
// plays only these. Kept in saved games (FF_GetSongsHeard).

void FF_GetSongsHeard(int16_t *out, int count)
{
	for (int i = 0; i < count; ++i)
		out[i] = i < FF_LASTMUSIC ? ff_songsHeard[i] : 0;
}

void FF_SetSongsHeard(const int16_t *in, int count)
{
	for (int i = 0; i < FF_LASTMUSIC; ++i)
		ff_songsHeard[i] = (i < count && in[i] == 1) ? 1 : 0;
}
void CK_MeasureMultiline(const char *str, uint16_t *w, uint16_t *h);

void FF_StartMusic(uint16_t song, bool repeat);

void FF_Jukebox(void)
{
	// (its songs by number, in the order of its playlist)
	static const int playlist[] = {0, 31, 1, 2, 3, 4, 24, 5, 6, 7, 8, 9, 10, 30, 11, 12, 13, 14, 15, 33, 34, 35,
		16, 29, 17, 27, 18, 19, 20, 39, 22, 38, 21};
	const int numSongs = (int)(sizeof(playlist) / sizeof(playlist[0]));
	int song = 0;
	for (int i = 0; i < numSongs; ++i)
		if (playlist[i] == ff_playingSong)
			song = i;

	US_CenterWindow(26, 12);
	CA_CacheGrChunk(FF_TRANSCEIVERPIC);
	VHB_DrawBitmap(US_GetWindowX(), US_GetWindowY() + 8, FF_TRANSCEIVERPIC);
	US_CPrint(FF_String(FF_S_JUKEBOXTOP));
	int nameY = US_GetPrintY() + 2;
	US_SetPrintY(US_GetWindowY() + US_GetWindowH() - 30);
	int nameH = US_GetPrintY() - nameY;
	US_CPrint(FF_String(FF_S_JUKEBOXBOTTOM));
	int nameX = US_GetWindowX() + 65, nameW = US_GetWindowW() - 65;
	US_SetWindowX(nameX);
	US_SetWindowW(nameW);

	IN_ClearKeysDown();
	bool redraw = true;
	while (true)
	{
		if (redraw)
		{
			// the song's name, with a mark when it is the one playing
			char name[160];
			snprintf(name, sizeof(name), "%s%s", FF_String(FF_S_MUSICNAME0 + playlist[song]),
				playlist[song] == ff_playingSong ? "\n(playing)" : !ff_songsHeard[playlist[song]] ? "\n(not heard yet)" : "");
			uint16_t w, h;
			CK_MeasureMultiline(name, &w, &h);
			VHB_Bar(nameX, nameY, nameW, nameH, 15);
			US_SetPrintY(nameY + (nameH - (int)h) / 2);
			US_CPrint(name);
			VH_UpdateScreen();
			redraw = false;
		}
		IN_PumpEvents();
		IN_ScanCode scan = IN_GetLastScan();
		IN_ReadControls(0, &ck_inputFrame);
		if (scan == IN_SC_Escape || ck_inputFrame.pogo)
			break;
		if (ck_inputFrame.jump || scan == IN_SC_Enter || scan == IN_SC_Space)
		{
			if (ff_songsHeard[playlist[song]])
				FF_StartMusic(playlist[song], true);
			else
				SD_PlaySound(FF_OLDNOWAYSND); // only songs already heard in play
			redraw = true;
			while (ck_inputFrame.jump)
			{
				IN_PumpEvents();
				IN_ReadControls(0, &ck_inputFrame);
			}
		}
		int step = (ck_inputFrame.yDirection < 0 || ck_inputFrame.xDirection < 0 || scan == IN_SC_UpArrow || scan == IN_SC_LeftArrow) ? -1 :
			(ck_inputFrame.yDirection > 0 || ck_inputFrame.xDirection > 0 || scan == IN_SC_DownArrow || scan == IN_SC_RightArrow) ? 1 : 0;
		if (step && song + step >= 0 && song + step < numSongs)
		{
			song += step;
			redraw = true;
		}
		if (scan != IN_SC_None)
			IN_SetLastScan(IN_SC_None);
		while (ck_inputFrame.yDirection != 0 || ck_inputFrame.xDirection != 0)
		{
			IN_PumpEvents();
			IN_ReadControls(0, &ck_inputFrame);
		}
		VL_Present();
	}
	// the level goes on with the song that is playing
	if (ff_playingSong >= 0)
		ff_map.music = ff_playingSong;
	IN_ClearKeysDown();
}
// "Snailas" (SNAILAS.C, from a BASIC program by Commander Spleen, made C by
// K1n9_Duk3): another arcade machine's waiting screen. Forty snails, each
// three letters of a DOS text screen ( _@" ), crawl about a green screen
// round a box that says INSERT COIN, turning at each other and now and then
// falling asleep. Any key leaves. The original switches the PC to its
// 80-column text screen; here the same 80 x 25 cells are drawn on the
// game's own screen, 4 x 8 pixels each, the snails' letters as small shapes
// and the words in the box in the game's own letters. The snails and the
// box start as the game's data has them (its SNAILAS_DATA chunk).
#define FF_SNAIL_COLS 80
#define FF_SNAIL_ROWS 25
#define FF_NUM_SNAILS 40

void FF_RunSnailas(void)
{
	enum { CELLS = FF_SNAIL_COLS * FF_SNAIL_ROWS, MAXX = FF_SNAIL_COLS - 3, MAXY = FF_SNAIL_ROWS - 1 };
	static uint8_t glyph[CELLS], colour[CELLS], blocked[CELLS];
	static struct { int8_t x, y, dir, sleep; uint8_t col; } snails[FF_NUM_SNAILS];
	const uint8_t defaultColours = 8 | (2 << 4); // dark grey on green

	CA_CacheGrChunk(FF_SNAILAS_DATA);
	const uint8_t *data = (const uint8_t *)ca_graphChunks[FF_SNAILAS_DATA];
	if (!data)
		return;
	// its data: the screen (a letter and its colours for every cell), which
	// cells are taken, the snails (five bytes each)
	for (int i = 0; i < CELLS; ++i)
	{
		glyph[i] = data[i * 2];
		colour[i] = data[i * 2 + 1];
		blocked[i] = data[CELLS * 2 + i];
	}
	for (int i = 0; i < FF_NUM_SNAILS; ++i)
	{
		const uint8_t *sn = data + CELLS * 3 + i * 5;
		snails[i].x = (int8_t)sn[0];
		snails[i].y = (int8_t)sn[1];
		snails[i].dir = (int8_t)sn[2];
		snails[i].sleep = (int8_t)sn[3];
		snails[i].col = sn[4];
	}

	// the snails' letters, three pixels wide in a cell of 4 x 8
	static const struct { char c; uint8_t rows[8]; } shapes[] = {
		{'_', {0, 0, 0, 0, 0, 0, 7, 0}},
		{'@', {0, 0, 2, 7, 7, 7, 2, 0}},
		{'"', {0, 0, 5, 5, 0, 0, 0, 0}},
		{'z', {0, 0, 0, 0, 7, 2, 7, 0}},
		{'Z', {0, 0, 7, 1, 2, 4, 7, 0}},
	};

	VL_SetScrollCoords(0, 0);
	VL_SetDefaultPalette();
	US_SetPrintRoutines(NULL, VH_DrawPropString);
	IN_ClearKeysDown();
	for (int frame = 0;; ++frame)
	{
		if (frame % 6 == 0)
		{
			for (int i = 0; i < FF_NUM_SNAILS; ++i)
			{
				int xmove = 0, ymove = 0;
				bool move = false;
				int pos = snails[i].x + snails[i].y * FF_SNAIL_COLS;
				if (snails[i].sleep > 0)
				{
					if (--snails[i].sleep != 0)
						glyph[pos + 1 + snails[i].dir] = (snails[i].sleep & 1) ? 'Z' : 'z';
					else
						move = true; // (it wakes: drawn again where it is)
				}
				else
				{
					if (rand() % 70 > 55)
						snails[i].dir = -snails[i].dir;
					else if (rand() % 65 > 50)
					{
						move = true;
						xmove = snails[i].dir;
						ymove = (rand() % 2) * 2 - 1;
					}
					else
					{
						move = true;
						xmove = snails[i].dir;
					}
				}
				if (move)
				{
					int newx = snails[i].x + xmove, newy = snails[i].y + ymove;
					newx = newx > MAXX ? 0 : newx < 0 ? MAXX : newx;
					newy = newy > MAXY ? 0 : newy < 0 ? MAXY : newy;
					int newpos = newy * FF_SNAIL_COLS + newx;
					for (int k = 0; k < 3; ++k)
						blocked[pos + k] = 0;
					if (blocked[newpos] || blocked[newpos + 1] || blocked[newpos + 2])
						snails[i].dir = -snails[i].dir; // something in the way: turn round
					else
					{
						for (int k = 0; k < 3; ++k)
							glyph[pos + k] = 0;
						colour[pos + 1] = defaultColours;
						glyph[newpos + 1] = '@';
						colour[newpos + 1] = snails[i].col;
						glyph[newpos + 1 + snails[i].dir] = '"';
						glyph[newpos + 1 - snails[i].dir] = '_';
						snails[i].x = (int8_t)newx;
						snails[i].y = (int8_t)newy;
						pos = newpos;
					}
					for (int k = 0; k < 3; ++k)
						blocked[pos + k] = 1;
				}
				if (snails[i].sleep == 0 && move && rand() % 50 == 45)
				{
					snails[i].sleep = 10;
					glyph[pos + 1 + snails[i].dir] = 'z';
					glyph[pos + 1 - snails[i].dir] = ' ';
				}
			}
		}
		// the screen
		for (int i = 0; i < CELLS; ++i)
		{
			int x = (i % FF_SNAIL_COLS) * 4, y = (i / FF_SNAIL_COLS) * 8;
			VL_ScreenRect(x, y, 4, 8, (colour[i] >> 4) & 7);
			for (int k = 0; k < (int)(sizeof(shapes) / sizeof(shapes[0])); ++k)
			{
				if (shapes[k].c != glyph[i])
					continue;
				for (int row = 0; row < 8; ++row)
					for (int bit = 0; bit < 3; ++bit)
						if (shapes[k].rows[row] & (4 >> bit))
							VL_ScreenRect(x + bit, y + row, 1, 1, colour[i] & 15);
			}
		}
		// the words in the box (cells 32-48 across, 8-16 down)
		US_SetWindowX(30 * 4 - 8);
		US_SetWindowW(21 * 4 + 16);
		US_SetPrintFont(0);
		US_SetPrintColour(14);
		US_SetPrintY(9 * 8 - 1);
		US_CPrint("SNAILAS");
		US_SetPrintColour(4);
		US_SetPrintY(11 * 8 - 1);
		US_CPrint("0 CREDIT");
		if ((frame / 24) % 2 == 0) // (the text screen's blinking letters)
		{
			US_SetPrintColour(12);
			US_SetPrintY(13 * 8 - 1);
			US_CPrint("INSERT COIN");
			US_SetPrintY(14 * 8 + 2);
			US_CPrint("TO PLAY!");
		}
		VL_Present();
		VL_DelayTics(1);

		IN_PumpEvents();
		IN_ReadControls(0, &ck_inputFrame);
		if (IN_GetLastScan() != IN_SC_None || ck_inputFrame.jump || ck_inputFrame.pogo || ck_inputFrame.button2)
			break;
		// (developer runs: a picture of the 60th frame, and on)
		if (frame == 60 && KW_DebugWait())
			break;
		if (frame > 60 && getenv("KW_AUTOSHOT"))
			break;
	}
	US_SetPrintColour(15);
	US_SetPrintRoutines(NULL, NULL);
	VL_ClearScreen(0);
	IN_ClearKeysDown();
}
// "Foobs in Space!" (FOOBS.C, by K1n9_Duk3): an arcade machine's waiting
// screen. Stars fly out of the middle, the word FOOBS is spelled with Foobs
// that bob up and down, and INSERT COIN flashes. Any key leaves. Written
// again for this engine (the original draws with the EGA card directly):
// every frame is drawn whole.
static int FF_FoobSin(int angle, int factor)
{
	static const uint8_t table[65] = {
		0, 3, 6, 9, 12, 15, 18, 21, 24, 28, 31, 34, 37, 40, 43, 46,
		48, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88,
		90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 109, 111, 112, 114, 115, 117,
		118, 119, 120, 121, 122, 123, 124, 124, 125, 126, 126, 127, 127, 127, 127, 127,
		128};
	angle &= 0xFF;
	int value = angle <= 64 ? table[angle] : angle <= 128 ? table[128 - angle] :
		angle <= 192 ? -(int)table[angle - 128] : -(int)table[256 - angle];
	return (factor * value) >> 7;
}

void FF_RunFoobsInSpace(void)
{
	enum { NUM_STARS = 100, W = 320, H = 200 };
	struct { int angle, moved, speed, x, y; } stars[NUM_STARS];
	// the picture: two Foobs side by side, 16 x 16 each, as four EGA planes
	// of 32 x 16. Each becomes a picture of its own; light magenta is shown
	// as white and light red as black, as the original sets the colours.
	static uint8_t frames[2][4 * 16 * 2];
	CA_CacheGrChunk(FF_FOOBSPIC);
	const uint8_t *pic = (const uint8_t *)ca_graphChunks[FF_FOOBSPIC];
	if (!pic)
		return;
	for (int n = 0; n < 2; ++n)
	{
		memset(frames[n], 0, sizeof(frames[n]));
		for (int y = 0; y < 16; ++y)
			for (int x = 0; x < 16; ++x)
			{
				int sx = n * 16 + x, colour = 0;
				for (int plane = 0; plane < 4; ++plane)
					if (pic[plane * 64 + y * 4 + sx / 8] & (0x80 >> (sx % 8)))
						colour |= 1 << plane;
				colour = colour == 13 ? 15 : colour == 12 ? 0 : colour;
				for (int plane = 0; plane < 4; ++plane)
					if (colour & (1 << plane))
						frames[n][plane * 32 + y * 2 + x / 8] |= 0x80 >> (x % 8);
			}
	}
	for (int i = 0; i < NUM_STARS; ++i)
		stars[i].moved = -1;

	static const uint8_t F[] = {0,16, 16,16, 32,16, 0,33, 0,50, 16,50, 0,67, 0,84};
	static const uint8_t O[] = {0,24, 0,41, 0,58, 0,75, 16,16, 16,84, 32,24, 32,41, 32,58, 32,75};
	static const uint8_t B[] = {0,16, 16,16, 0,33, 0,50, 16,50, 0,67, 0,84, 16,84, 32,21, 32,38, 32,62, 32,79};
	static const uint8_t S[] = {32,16, 16,16, 0,24, 0,41, 16,50, 32,58, 32,75, 16,84, 0,84};
	static const struct { const uint8_t *pattern; int size; } letters[5] = {
		{F, sizeof(F)}, {O, sizeof(O)}, {O, sizeof(O)}, {B, sizeof(B)}, {S, sizeof(S)}};

	VL_SetScrollCoords(0, 0);
	VL_SetDefaultPalette();
	CA_CacheGrChunk(5); // the story's font, for "IN SPACE!"
	US_SetPrintRoutines(NULL, VH_DrawPropString);
	IN_ClearKeysDown();
	int amplitude = 10, ampdelta = 1;
	for (int counter = 1;; ++counter)
	{
		VL_ClearScreen(0);
		// the stars: dark in the middle, brighter as they fly out
		for (int i = 0; i < NUM_STARS; ++i)
		{
			if (stars[i].moved >= 0)
				stars[i].moved += stars[i].speed;
			for (int tries = 0; tries < 50; ++tries)
			{
				if (stars[i].moved >= 0)
				{
					stars[i].x = FF_FoobSin(stars[i].angle, stars[i].moved / 2) + W / 2;
					stars[i].y = FF_FoobSin(stars[i].angle + 64, stars[i].moved / 2) + H / 2;
					if (stars[i].x >= 0 && stars[i].x < W && stars[i].y >= 0 && stars[i].y < H)
						break;
				}
				stars[i].angle = rand() % 256;
				stars[i].moved = rand() % 128;
				stars[i].speed = rand() % 4 + 1;
			}
			VL_ScreenRect(stars[i].x, stars[i].y, 1, 1, stars[i].moved < 75 ? 8 : stars[i].moved < 150 ? 7 : 15);
		}
		// FOOBS, in Foobs
		if ((counter & 0x1FF) == 0)
		{
			if (amplitude == 10 || amplitude == 0)
				ampdelta = -ampdelta;
			amplitude += ampdelta;
		}
		int x = (W - (5 * 48 + 4 * 8)) / 2, frame = 0;
		for (int l = 0; l < 5; ++l, x += 48 + 8)
		{
			int f = frame++;
			for (int k = 0; k < letters[l].size; k += 2, f += 8)
			{
				int fx = x + letters[l].pattern[k];
				int fy = letters[l].pattern[k + 1] + FF_FoobSin(counter + fx, amplitude);
				VL_UnmaskedToScreen(frames[((counter + f) >> 4) & 1], fx & ~7, fy, 16, 16);
			}
		}
		// the words
		US_SetWindowX(0);
		US_SetWindowW(W);
		US_SetPrintFont(2);
		US_SetPrintColour(14);
		US_SetPrintY(128);
		US_CPrint(FF_String(FF_S_INSPACE));
		US_SetPrintFont(0);
		US_SetPrintColour((counter & 8) ? 12 : 4);
		US_SetPrintY(184);
		US_CPrint(FF_String(FF_S_INSERTCOIN));
		VL_Present();
		VL_DelayTics(1);

		IN_PumpEvents();
		IN_ReadControls(0, &ck_inputFrame);
		if (IN_GetLastScan() != IN_SC_None || ck_inputFrame.jump || ck_inputFrame.pogo || ck_inputFrame.button2)
			break;
		// (developer runs: a picture of the 60th frame, and on)
		if (counter == 60 && KW_DebugWait())
			break;
		if (counter > 60 && getenv("KW_AUTOSHOT"))
			break;
	}
	US_SetPrintFont(0);
	US_SetPrintColour(15);
	US_SetPrintRoutines(NULL, NULL);
	VL_ClearScreen(0);
	IN_ClearKeysDown();
}

// The fade out and back in around a teleport (DelayedFade in CK_GAME.C):
// the screen goes dark now and comes back after two refreshes, when
// everything has drawn itself where it now is. The engine has the same for
// its level starts.
void CK_BeginFadeDrawing(void);

void FF_DelayedFade(void)
{
	CK_BeginFadeDrawing();
}

// Music. A level's song is a number in its map (ScanInfoPlane, music trigger
// tiles), kept in ff_map.music: the engine's StartMusic(level) plays that
// in this world. Its own StartMusic(song) (dialogues, triggers) names a song
// directly. (Its songs are numbered as the engine's "levels": song n is
// CK_LevelMusic[n].)
static bool ff_songDirect;

void FF_StartMusic(uint16_t song, bool repeat)
{
	(void)repeat;
	ff_songDirect = true;
	StartMusic(song);
	ff_songDirect = false;
}

int FF_SongFor(int level)
{
	if (!ff_songDirect)
		level = (ff_map.music >= 0 && ff_map.music < FF_LASTMUSIC) ? ff_map.music : 0;
	ff_playingSong = level; // (for its jukebox)
	if (level >= 0 && level < FF_LASTMUSIC)
		ff_songsHeard[level] = 1;
	return level;
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

// ---- What its PlayLoop (CK_PLAY.C) does each frame beyond the engine's.
// The engine's play loop calls these in this world. ----

void FF_CheckWorldInTiles(CK_object *keen); // kw_ff_map.c

// What was pressed, as its PollControls changes it: with a Nisasi on his
// head Keen's left and right are swapped and he can't look, jump or pogo;
// under the Brain Fucl he walks on by himself.
void FF_InputRead(void)
{
	IN_ControlFrame *c = &ck_inputFrame;
	if (ff_map.nisasi)
	{
		c->yDirection = IN_motion_None;
		c->y = 0;
		c->jump = c->pogo = false;
		c->x = -c->x;
		c->xDirection = (IN_Motion)(-(int)c->xDirection);
		c->dir = c->xDirection < 0 ? IN_dir_West : c->xDirection > 0 ? IN_dir_East : IN_dir_None;
	}
	else if (ff_map.brainfucl > 0)
	{
		bool onGround = ck_keenObj && ck_keenObj->currentAction && ck_keenObj->currentAction->stickToGround == 1;
		memset(c, 0, sizeof(*c));
		c->dir = IN_dir_None;
		if (onGround)
		{
			c->x = -1;
			c->xDirection = IN_motion_Left;
			c->dir = IN_dir_East; // (as in the source)
		}
	}
	if (c->yDirection != IN_motion_Up)
		ff_keenUpHeld = false;
}

// And what Keen may do with it: no shooting with a Nisasi on his head, no
// jumping in quicksand; in two levels the pogo key jumps.
void FF_InputRestrictions(void)
{
	// (the engine's debug input sets the key states after the frame is read)
	if (ff_map.nisasi)
		ck_keenState.shootIsPressed = ck_keenState.shootWasPressed = false;
	if (ff_map.quicksand)
		ck_keenState.jumpIsPressed = ck_keenState.jumpWasPressed = ck_keenState.pogoIsPressed =
			ck_keenState.pogoWasPressed = false;
	if (ca_mapOn == MAP_MUSHROOMLEVEL1 || ca_mapOn == MAP_TARDISLEVEL)
	{
		if (ck_keenState.pogoIsPressed || ck_keenState.jumpIsPressed)
		{
			ck_keenState.jumpIsPressed = true;
			ck_keenState.pogoIsPressed = false;
		}
		else
		{
			ck_keenState.jumpWasPressed = false;
			ck_keenState.jumpIsPressed = false;
		}
	}
}

// Keen rides platforms, and quicksand counts as one.
void FF_FrameRiding(void)
{
	if (!ck_keenState.platform)
		ck_keenState.platform = ff_map.quicksand;
	if (ck_keenState.platform)
		FF_HandleRiding(ck_keenObj);
}

// The tiles Keen is in: items, hazards, warps, water...
void FF_FrameInTiles(void)
{
	if (ca_mapOn == 0)
		FF_CheckWorldInTiles(ck_keenObj);
	else
		FF_CheckInTiles(ck_keenObj);
	if (getenv("KW_TRACE"))
	{
		CK_object *k = ck_keenObj;
		fprintf(stderr, "TRACE keen tiles x %d-%d (mid %d) y %d-%d misc:", k->clipRects.tileX1, k->clipRects.tileX2,
			k->clipRects.tileXmid, k->clipRects.tileY1, k->clipRects.tileY2);
		for (int y = k->clipRects.tileY1; y <= k->clipRects.tileY2; ++y)
			for (int x = k->clipRects.tileX1; x <= k->clipRects.tileX2; ++x)
				fprintf(stderr, " %d/%x", TI_ForeMisc(CA_TileAtPos(x, y, 1)) & 0x7F, CA_TileAtPos(x, y, 2));
		fprintf(stderr, " warp %d,%d info %x checkpoint %d lives %d | pos %u,%u sprite %d riding %d\n", ff_map.warpDeltaX,
			ff_map.warpDeltaY, ff_map.warpInfo, (int)ff_map.checkpointSaved, ck_gameState.numLives, k->posX, k->posY,
			k->gfxChunk, ck_keenState.platform ? ck_keenState.platform->type : 0);
	}
}

// The level camera follows Keen, or the creature he controls by mind.
void FF_ScrollScreen(CK_object *ob); // kw_ff_cam.c
void FF_Camera(CK_object *keen)
{
	FF_ScrollScreen(ff_map.mindcontrolled ? ff_map.mindcontrolled : keen);
}

// After the frame is drawn: wrap-around scrolling, warps, timers.
// TODO: its Nisasi music tempo, the bonus level's Lindsey.
void FF_WrapScroll(void); // kw_ff_cam.c

void FF_FrameEnd(void)
{
	FF_WrapScroll();
	if (ff_map.warpInfo)
	{
		if (!FF_KeenIsDead())
		{
			FF_TeleportKeen(ck_keenObj, ff_map.warpInfo);
			CK_SetAction(ck_keenObj, ck_keenObj->currentAction);
		}
		ff_map.warpInfo = 0;
	}
	if (ff_map.warpDeltaX || ff_map.warpDeltaY)
	{
		if (!FF_KeenIsDead())
		{
			ck_keenObj->posX += RF_TileToUnit(ff_map.warpDeltaX);
			ck_keenObj->posY += RF_TileToUnit(ff_map.warpDeltaY);
			CK_SetAction(ck_keenObj, ck_keenObj->currentAction);
			if (ca_mapOn != MAP_TARDISLEVEL)
				CK_CentreCamera(ck_keenObj);
		}
		ff_map.warpDeltaX = ff_map.warpDeltaY = 0;
	}
	if (ff_map.invincibilityTimer)
	{
		ff_map.invincibilityTimer -= SD_GetSpriteSync();
		if (ff_map.invincibilityTimer < 0)
			ff_map.invincibilityTimer = 0;
	}
	if (ff_map.stunCooldown)
	{
		ff_map.stunCooldown -= SD_GetSpriteSync();
		if (ff_map.stunCooldown <= 0)
			ff_map.stunCooldown = ff_map.stunCounter = 0;
	}
	// a checkpoint touched this frame is saved now, with everything settled
	if (ff_map.checkpointTouched)
		FF_CheckpointSave(ff_map.checkpointTouched);
}

// ---- How a level ends (the switch after PlayLoop in its GameLoop, and
// HandleLevelDone, CK_GAME.C). Called by the engine's game loop before and
// after its own handling of the level state. ----

void CK_ShowTextPages(int textChunk, int song, bool blackBackground);
void KW_AfterWorldEnding(void);

#define COLLECT_DONUT 0x0040
#define COLLECT_SODAS 0x0080
#define FF_LS_HANDLED 60 // a level state the engine's game loop does nothing for

static int ff_doneLevel = -1;
static uint16_t ff_doneFlags;

static void FF_HandleLevelDone(void)
{
	ff_state.collectiblesTotal |= ff_map.collectiblesNew;
	ff_state.oldscore = ck_gameState.keenScore;
	ff_state.slugcans += ff_map.slugcans;
	if (ff_state.slugcans >= SLUGCANS_TOTAL)
		ff_state.collectiblesTotal |= COLLECT_SODAS;
	ff_map.doneFlags |= DF_DONE;
	if (ff_state.cheated)
		ff_map.doneFlags |= DF_CHEATED;
	if (ff_map.itemsCollected && ff_map.itemsCollected == ff_map.itemsTotal)
		ff_map.doneFlags |= DF_PERFECT;
}

static void FF_Ending(int textChunk, int song)
{
	CK_ShowTextPages(textChunk, song, false);
	// The engine's own ending text leaves a cache level pushed, which Keen
	// Worlds pops afterwards (KW_AfterWorldEnding). Ours doesn't, so push one.
	CA_UpLevel();
	KW_AfterWorldEnding();
}

void FF_LevelEnded(void)
{
	int level = ca_mapOn;
	ff_doneLevel = -1;
	switch ((int)ck_gameState.levelState)
	{
	case ex_mushroomtrigger: // straight on into the second mushroom level
		ck_gameState.currentLevel = MAP_MUSHROOMLEVEL2;
		SD_PlayTwoSounds(FF_FOOTAPPEARSND, FF_YETICLOUDSND);
		FF_HandleLevelDone();
		ck_gameState.levelState = FF_LS_HANDLED;
		break;
	case ex_secrettrigger: // on into the secret level
		FF_HandleLevelDone();
		ck_gameState.levelsDone[MAP_MUSHROOMLEVEL1] = ff_map.doneFlags;
		ck_gameState.levelsDone[level] = ff_map.doneFlags;
		ck_gameState.currentLevel = MAP_TARDISLEVEL;
		SD_PlaySound(FF_ENTERLEVELSND);
		ck_gameState.levelState = FF_LS_HANDLED;
		break;
	case ex_bonuscomplete:
		FF_LindseyDialog(2);
		StopMusic();
		ff_state.collectiblesTotal |= COLLECT_DONUT;
		ck_gameState.levelState = LS_LevelComplete;
		// fall through
	case LS_LevelComplete:
		FF_HandleLevelDone();
		if (level == 0)
			break; // the map: the engine goes on into the level entered
		if (level == MAP_TEMPLELEVEL)
		{
			// beaten: straight on to the final level
			ck_gameState.levelsDone[MAP_TEMPLELEVEL] = ff_map.doneFlags;
			ck_gameState.currentLevel = MAP_ACMELABLEVEL;
			SD_PlaySound(FF_ENTERLEVELSND);
			ck_gameState.levelState = FF_LS_HANDLED;
			break;
		}
		if (level == MAP_MUSHROOMLEVEL2)
			ck_gameState.levelsDone[MAP_MUSHROOMLEVEL1] = ff_map.doneFlags;
		// the engine marks the level done and returns to the map; its
		// flags (done, perfect, cheated) go in afterwards
		ff_doneLevel = level;
		ff_doneFlags = ff_map.doneFlags;
		break;
	case ex_clonedjanitor: // the game is won: one of its endings
		FF_HandleLevelDone();
		if (ff_state.cheated)
			FF_Ending(FF_T_END_CHEAT, FF_JosephCollins_Slipping_Slug);
		else if (ck_gameState.keenScore >= 1000000)
			FF_Ending(FF_T_END_ALL, FF_Kohntarkosz__JoJackson);
		else if (ck_gameState.keenScore <= 1000)
			FF_Ending(ff_state.continuesUsed ? FF_T_END_TRICKERY : FF_T_END_MINIMUM, -1);
		else
			FF_Ending(FF_T_END_WIN, FF_IncredibleMachine_Fusion);
		ck_gameState.levelState = FF_LS_HANDLED;
		break;
	case ex_brainfucled: // the bad ending. (The source quits the game; here Keen is back on the map.)
		FF_HandleLevelDone();
		CK_ShowTextPages(FF_T_END_LOSE, FF_LeeJackson_dopejaws, false);
		ck_gameState.currentLevel = 0;
		ck_gameState.levelState = FF_LS_HANDLED;
		break;
	}
}

void FF_AfterLevelEnded(void)
{
	if (ff_doneLevel > 0 && ck_gameState.levelsDone[ff_doneLevel])
		ck_gameState.levelsDone[ff_doneLevel] = ff_doneFlags;
	ff_doneLevel = -1;
}

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
	US_CPrint(FF_String(FF_S_LOCATION));
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
	US_CPrint(FF_String(FF_S_EXTRA));
	VHB_Bar(79, 71, 66, 10, 0);
	CK_DrawLongRight(88, 72, 7, 6, ck_gameState.nextKeenAt);
	VHB_Bar(95, 80, 1, 1, 8);
	VHB_Bar(119, 80, 1, 1, 8);

	US_SetPrintY(85);
	US_SetWindowX(81);
	US_SetWindowW(64);
	US_CPrint(FF_String(FF_S_SCORE));
	VHB_Bar(79, 95, 66, 10, 0);
	CK_DrawLongRight(88, 96, 7, 6, ck_gameState.keenScore);
	VHB_Bar(95, 104, 1, 1, 8);
	VHB_Bar(119, 104, 1, 1, 8);

	US_SetPrintY(61);
	US_SetWindowX(177);
	US_SetWindowW(64);
	US_SetPrintColour(15);
	US_CPrint(FF_String(FF_S_LEVEL));
	VHB_Bar(175, 71, 66, 10, 0);
	US_SetPrintY(72);
	US_SetWindowX(176);
	US_SetWindowW(64);
	US_SetPrintColour(10);
	US_CPrint(FF_String(ff_state.cheated ? FF_S_CHEATER : ck_gameState.difficulty == D_Easy ? FF_S_EASY :
		ck_gameState.difficulty == D_Normal ? FF_S_NORMAL : ck_gameState.difficulty == D_Hard ? FF_S_HARD : FF_S_EXPERT));

	US_SetPrintX(194);
	US_SetPrintY(85);
	US_SetPrintColour(15);
	US_Print(FF_String(FF_S_KEYS));
	VHB_Bar(175, 95, 66, 10, 0);
	for (int i = 0, x = 184; i < 4; i++, x += 13)
		VHB_DrawTile8(x, 96, ck_gameState.keyGems[i] ? i : 4);

	US_SetPrintX(80);
	US_SetPrintY(144);
	US_Print(FF_String(FF_S_AMMO));
	VHB_Bar(127, 143, 18, 10, 0);
	CK_DrawLongRight(128, 144, 2, 6, ck_gameState.numShots);

	US_SetPrintX(80);
	US_SetPrintY(112);
	US_Print(FF_String(FF_S_KEENS));
	VHB_Bar(127, 111, 18, 10, 0);
	CK_DrawLongRight(128, 112, 2, 6, ck_gameState.numLives);

	US_SetPrintX(80);
	US_SetPrintY(128);
	US_Print(FF_String(FF_S_DROPS));
	VHB_Bar(127, 127, 18, 10, 0);
	CK_DrawLongRight(128, 128, 2, 6, ck_gameState.numCentilife);

	// The six collectibles. TODO: the source draws eight rows of each
	// sprite, dark grey while it is missing; here only those found, whole,
	// and only if the sprite is in memory.
	US_SetPrintY(117);
	US_SetWindowX(177);
	US_SetWindowW(64);
	US_CPrint(FF_String(FF_S_GOODIES));
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

// ---- The map's finished levels (PatchWorldMap in CK_GAME.C): entrances
// and fences go, flags stand (another flag for a perfect level, rising for
// the level just finished). Keen Worlds keeps the entrances: finished levels
// can be visited again. ----

extern int ck_lastLevelFinished;
bool KW_IsHabitableLevel(int level);
void FF_SpawnFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff);
void FF_SpawnRisingFlag(uint16_t tileX, uint16_t tileY, uint16_t spriteoff);

void FF_PatchWorldMap(void)
{
	for (int y = 0; y < CA_GetMapHeight(); y++)
		for (int x = 0; x < CA_GetMapWidth(); x++)
		{
			uint16_t w = CA_TileAtPos(x, y, 2);
			int level = w & 0xFF, flags = w >> 8;
			if (w == (0xC000 | MAP_TEMPLELEVEL) && ck_gameState.levelsDone[MAP_TEMPLELEVEL])
			{
				// the temple's entrance becomes the laboratory's
				CA_SetTileAtPos(x, y, 2, 0xC000 | MAP_ACMELABLEVEL);
				continue;
			}
			if (level < 1 || level >= FF_GAMELEVELS || !ck_gameState.levelsDone[level] || level == MAP_BWBMEGAROCKET)
				continue;
			if (flags == 0xC0 && KW_IsHabitableLevel(level))
				continue;
			CA_SetTileAtPos(x, y, 2, 0);
			if (flags == 0xD0)
				CA_SetTileAtPos(x, y, 1, 0); // the fences go
			else if (flags == 0xD1)
				CA_SetTileAtPos(x, y, 1, 35); // an invisible blocking tile
			else if (flags == 0xF0)
			{
				int flagoff = (ck_gameState.levelsDone[level] & DF_PERFECT) ? FF_FLAG2FLIP1SPR - FF_FLAGFLIP1SPR : 0;
				if (ck_lastLevelFinished == level)
					FF_SpawnRisingFlag(x, y, flagoff);
				else
					FF_SpawnFlag(x, y, flagoff);
			}
		}
}

// ---- Keen Worlds' peaceful levels: what leaves a level once it is
// completed, and what would replay the story. ----

bool FF_IsHostileType(int type)
{
	switch (type)
	{
	case CT5_EnemyShot: case CT5_StunnedCreature:
	case FF_CT_Slug: case FF_CT_Smirky: case FF_CT_Skypest: case FF_CT_Mimrock: case FF_CT_PatPat:
	case FF_CT_XkyLick: case FF_CT_FungusAmongus: case FF_CT_DungBeetle: case FF_CT_Chemolain:
	case FF_CT_Salamander: case FF_CT_Wormouth: case FF_CT_Shrubbery: case FF_CT_Tentacle:
	case FF_CT_Poleknawel: case FF_CT_Kitten: case FF_CT_GellyGhost: case FF_CT_Roots: case FF_CT_Yeti:
	case FF_CT_TinyYeti: case FF_CT_TinyYetiMaster: case FF_CT_YetiCloud: case FF_CT_Bird: case FF_CT_BirdEgg:
	case FF_CT_Egg: case FF_CT_SneakyTree: case FF_CT_Crocodent: case FF_CT_TentacleNest: case FF_CT_Warble:
	case FF_CT_Efree: case FF_CT_Nisasi: case FF_CT_NisasiCooldown: case FF_CT_CloneAnimation: case FF_CT_Clone:
	case FF_CT_CloneRising: case FF_CT_CoronaPlant: case FF_CT_CoronaBall: case FF_CT_Nipnap: case FF_CT_Crab:
	case FF_CT_BrainFucl: case FF_CT_Fungus: case FF_CT_Mummy1: case FF_CT_Mummy2:
		return true;
	}
	return false;
}

// The clone machine ends the game.
bool FF_IsStoryTrigger(const CK_object *obj)
{
	return obj->type == FF_CT_CloneMachineOff || obj->type == FF_CT_CloneMachineOn;
}

// ---- What a difficulty sets (ApplyDifficulty in CK_GAME.C) ----

// The limits always; lives and ammo only for a game begun in this world
// (Keen arriving from another world keeps what he has).
static void FF_ApplyDifficulty(bool newGame)
{
	static const struct { int restarts, ammo, maxammo, lives, clip; } table[] = {
		{2, 5, 25, 3, 5}, // (not playing: as Normal)
		{3, 10, 25, 5, 8}, // Easy
		{2, 5, 25, 3, 5}, // Normal
		{1, 3, 20, 3, 5}, // Hard
		{0, 3, 20, 3, 5}, // Expert
	};
	int d = ck_gameState.difficulty;
	if (d < 0 || d > FF_D_Expert)
		d = D_Normal;
	ff_state.freeRestartsPerCheckpoint = table[d].restarts;
	ff_state.maxammo = table[d].maxammo;
	ff_state.shotsinclip = table[d].clip;
	if (newGame)
	{
		ck_gameState.numShots = table[d].ammo;
		ck_gameState.numLives = table[d].lives;
	}
}

// A new game begun in this world (called by the engine's game loop once the
// difficulty is known).
void FF_NewGame(void)
{
	memset(&ff_state, 0, sizeof(ff_state));
	FF_ApplyDifficulty(true);
}

// ---- Keen's look. The game comes with two graphics sets: its own Keen
// (EGAGRAPH.CKS) and a "classic" one with the Keen of Keen 4-6
// (EGAGRAPH.CKC). Keen Worlds uses the classic one, so that Keen looks the
// same in every world, unless the player chooses the game's own (Options
// menu; setting kw_fitfOwnKeen) or the classic files aren't there. Called
// when the world's variables have been loaded, before its graphics are. ----

void FF_ChooseGraphics(void)
{
	if (CK_INT(ff_classicKeen, -1) == -1)
		return; // not this world's variables
	bool classic = !CFG_GetConfigBool("kw_fitfOwnKeen", false) && FS_IsKeenFilePresent("EGAGRAPH.CKC") &&
		FS_IsKeenFilePresent("EGAHEAD.CKC") && FS_IsKeenFilePresent("EGADICT.CKC");
	CK_VAR_SetString("ca_graphFile", classic ? "EGAGRAPH.CKC" : "EGAGRAPH.CKS");
	CK_VAR_SetString("ca_graphHead", classic ? "EGAHEAD.CKC" : "EGAHEAD.CKS");
	CK_VAR_SetString("ca_graphDict", classic ? "EGADICT.CKC" : "EGADICT.CKS");
	CK_VAR_SetInt("ff_classicKeen", classic);
}

// ---- Its colours (CK_LevelPalette, CK_DefaultPalette in CK_GAME.C): each
// level has its own sixteen. They are VGA colours; the fades between
// screens still step through the standard ones. ----

#include "kw_fitf_pal.h"

void FF_LevelPalette(void)
{
	int level = ca_mapOn;
	int n = (level >= 0 && level < (int)sizeof(ff_levelPalette)) ? ff_levelPalette[level] : 0;
	VL_SetCustomColors(ff_palettes[n]);
}

// Leaving this world: the standard colours again.
void FF_LeaveWorld(void)
{
	VL_SetCustomColors(NULL);
}

// Its level setup: kw_ff_scan.c (ScanInfoPlane in CK_SPEC.C).
void FF_ScanInfoPlane(void);
void FF_ScanInfoLayer(void)
{
	memset(&ff_map, 0, sizeof(ff_map));
	ff_keenKilled = false;
	FF_ClearCheckpoint(); // a checkpoint belongs to the level it was touched in
	if (!ff_state.maxammo)
		FF_ApplyDifficulty(false); // arrived from another world
	FF_ScanInfoPlane();
	FF_LevelPalette();
}

void kw_ff_map_SetupFunctions(void);

void FF_SetupFunctions(void)
{
	kw_ff_act_SetupFunctions();
	kw_ff_map_SetupFunctions();
	kw_ff_keen_SetupFunctions();
}

// A condition of its text pages (HandleCondition in its CK_TEXT.C): what an
// ending says depends on what Keen found and did.
#define COLLECT_ALLPLUSHIES 0x003F
bool FF_TextCondition(int number)
{
	uint16_t collected = ff_state.collectiblesTotal & COLLECT_ALLPLUSHIES;
	switch (number)
	{
	case 0: return true;
	case 1: return ff_state.slugcans >= SLUGCANS_TOTAL;
	case 2: return collected == COLLECT_ALLPLUSHIES;
	case 3: return collected != 0 && collected != COLLECT_ALLPLUSHIES;
	case 4: return collected == 0;
	case 5: return ck_gameState.levelsDone[MAP_BONUSLEVEL] != 0;
	case 6: return !ck_gameState.levelsDone[MAP_BONUSLEVEL];
	case 7: return ck_gameState.levelsDone[MAP_BONUSLEVEL] && ff_state.slugcans < SLUGCANS_TOTAL;
	case 8: return ck_gameState.levelsDone[MAP_BONUSLEVEL] && ff_state.slugcans >= SLUGCANS_TOTAL;
	case 9: return ck_gameState.levelsDone[MAP_TARDISLEVEL] != 0;
	case 10: return !ck_gameState.levelsDone[MAP_TARDISLEVEL];
	case 11: return ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL] != 0;
	case 12: return !ck_gameState.levelsDone[MAP_SLUGBUSTSLEVEL];
	case 13: return ff_map.machinesActive > 1;
	case 14: return !(ff_map.machinesActive > 1);
	case 15: return !(ck_gameState.difficulty >= FF_D_Expert);
	}
	return false;
}

// (scripted tests: what Keen has collected, to see an ending's other pages)
void FF_TestCollected(int bits, int cans)
{
	ff_state.collectiblesTotal = (uint16_t)bits;
	ff_state.slugcans = (uint16_t)cans;
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
