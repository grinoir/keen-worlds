/*
Keen Worlds: Atroxian Realm (Gridlock, 2016) as a world.

Atroxian Realm is a game of its own with public GPL source, written from
Omnispeak's Keen 5 code. Here it runs on the engine's Keen 5 code with its
own data files (extension CKD), its own variable files (written by
tools/kw_arealm.py from its source) and its own level setup below. Its
creatures (AR_ACT1-3.C of its source) are not ported yet: levels load with
Keen, items and platforms only.

Licensed under the GNU GPL v2 or later, like Omnispeak and Atroxian Realm.
*/

#include "id_fs.h"
#include "ck_ep.h"
#include "kw_arealm.h"

void CK_SpawnAxisPlatform(int tileX, int tileY, int direction, bool purple);
void CK_SpawnFallPlat(int tileX, int tileY);


AR_State ar_state;

extern int ck_lastLevelFinished;
extern bool ck_scrollDisabled;
static void AR_SpawnBossKeen(int tileX, int tileY, int direction);
static void AR_SpawnTransporterKeen(int tileX, int tileY);

void AR_PhysAccelVert(CK_object *obj, int accY, int velLimit)
{
	bool isNegative = (obj->velY < 0);
	int32_t lastTimeCount = SD_GetLastTimeCount();
	for (int32_t tickCount = lastTimeCount - SD_GetSpriteSync(); tickCount < lastTimeCount; tickCount++)
	{
		// Every odd tic...
		if (tickCount & 1)
		{
			obj->velY += accY;
			if ((obj->velY < 0) != isNegative)
			{
				isNegative = (obj->velY < 0);
				obj->yDirection = isNegative ? -1 : 1;
			}
			if (obj->velY > velLimit)
				obj->velY = velLimit;
			else if (obj->velY < -velLimit)
				obj->velY = -velLimit;
		}
		ck_nextY += obj->velY;
	}
}

void CK_ShowTextPages(int textChunk, int song);
extern CK_EpisodeDef ar_episode;

bool AR_IsActive(void)
{
	return ck_currentEpisode == &ar_episode;
}

// Its story and dialogue screens (EndScreens in CK_TEXT.C): text chunk and
// song by number. (The source also picks a red or black background.)
void AR_EndScreens(int dialogue)
{
	static const int chunks[] = {T_ENDDIALOGUE, T_BWBKRILE, T_KRILEELDER, T_ATROXUS, T_SECRETKRILE,
		T_SECRETEND, T_END2, T_END3, T_NORMALEND, T_ATROXUSBOX1, T_ATROXUSBOX2, T_ATROXUSBOX3};
	static const int songs[] = {ENDING_MUS, KRILE_THEME_MUS, ELDER_TEXT_MUS, ATROXUS_TEXT_MUS, KRILE_THEME_MUS,
		ENDING_MUS, ENDING_MUS, ENDING_MUS, NORMAL_END_MUS, ATROXUS_BOX_MUS, ATROXUS_BOX_MUS, ATROXUS_BOX_MUS};
	if (dialogue < 0 || dialogue >= (int)(sizeof(chunks) / sizeof(chunks[0])))
		return;
	// Screen 6 follows screen 0 and keeps its music playing.
	CK_ShowTextPages(chunks[dialogue], dialogue == 6 ? -1 : songs[dialogue]);
}

void AR_WinGame(void)
{
	AR_EndScreens(0);
	AR_EndScreens(6);
	AR_EndScreens(ar_state.doSecretEnding ? 5 : 8);
	// The engine's own ending text leaves a cache level pushed, which Keen
	// Worlds pops afterwards (KW_AfterWorldEnding). Ours don't, so push one.
	CA_UpLevel();
}

// Its level setup (CK5_ScanInfoLayer in AR_MISC.C). Creature numbers are
// skipped until their code is ported.
void AR_ScanInfoLayer(void)
{
	CA_InitLumps();
	ar_state.axosisNum = 0;
	ar_state.krileGhost = NULL;
	int mapW = CA_GetMapWidth();
	int mapH = CA_GetMapHeight();

	for (int y = 0; y < mapH; ++y)
	{
		for (int x = 0; x < mapW; ++x)
		{
			int infoValue = CA_TileAtPos(x, y, 2);
			switch (infoValue)
			{
			case 1:
			case 2:
				CK_SpawnKeen(x, y, infoValue == 2 ? -1 : 1);
				CK_DemoSignSpawn();
				CA_MarkGrChunk(CK_CHUNKNUM(SPR_SCOREBOX));
				CA_MARKLUMP(KEENLUMP);
				if (ca_mapOn == 10)
					CA_MARKLUMP(KEENVINESLUMP);
				break;
			case 3:
				CK_DemoSignSpawn();
				CA_MarkGrChunk(CK_CHUNKNUM(SPR_SCOREBOX));
				CA_MARKLUMP(MAPKEENLUMP);
				CA_MARKLUMP(MAPKEEN2LUMP);
				// After the Great Ziggurat he arrives by transporter (37).
				if (ck_lastLevelFinished != 15)
					CK_SpawnMapKeen(x, y);
				break;
			case 37:
				if (ck_lastLevelFinished == 15)
				{
					AR_SpawnTransporterKeen(x, y);
					CK_DemoSignSpawn();
					CA_MarkGrChunk(CK_CHUNKNUM(SPR_SCOREBOX));
					CA_MARKLUMP(MAPKEENLUMP);
					CA_MARKLUMP(MAPKEEN2LUMP);
				}
				break;
			case 4:
				AR_SpawnGenerator(x, y);
				CA_MARKLUMP(GENERATORLUMP);
				break;
			case 6:
				AR_SpawnElder(x, y);
				CA_MarkGrChunk(ELDER2SPR);
				break;
			case 38:
				AR_SpawnBWBElder(x, y);
				CA_MarkGrChunk(ELDER1SPR);
				break;
			case 39:
				AR_SpawnAtroxusDialogue(x, y);
				CA_MarkGrChunk(ATROXUSDETECTSPR);
				break;
			case 40:
				AR_SpawnSecretKrile(x, y);
				CA_MarkGrChunk(ELDER1SPR);
				break;
			case 41:
				AR_SpawnBossKeen(x, y, IN_motion_Right);
				CA_MarkGrChunk(CK_CHUNKNUM(SPR_SCOREBOX));
				CA_MARKLUMP(KEENLUMP);
				break;
			// Krile Guardian
			case 50:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 49:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 5:
				AR_SpawnKrileGuardian(x, y);
				CA_MARKLUMP(KRILEGUARDIANLUMP);
				CA_MARKLUMP(KRILEGUARDIANSHOTLUMP);
				CA_MarkGrChunk(SPR_GUARDSHOTDIE1);
				CA_MarkGrChunk(SPR_GUARDSHOTDIE2);
				break;
			case 9:
				AR_SpawnAxoclaw(x, y);
				CA_MARKLUMP(AXOCLAW1LUMP);
				CA_MARKLUMP(AXOCLAW2LUMP);
				break;
			case 48:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 47:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 14:
				AR_SpawnAtroxite(x, y);
				CA_MARKLUMP(ATROXITELUMP);
				break;
			case 15:
				AR_SpawnAtroxus(x, y);
				CA_MARKLUMP(ATROXUSLUMP);
				CA_MARKLUMP(SMALLEXPOSIONLUMP);
				CA_MARKLUMP(MAGMINEEXPLOSIONLUMP);
				CA_MarkGrChunk(ABOMB1SPR);
				CA_MarkGrChunk(ABOMB2SPR);
				CA_MarkGrChunk(ATROXUSDROPBOMB1SPR);
				CA_MarkGrChunk(ATROXUSDROPBOMB2SPR);
				CA_MarkGrChunk(ATROXUSDEBRIS1SPR);
				CA_MarkGrChunk(ATROXUSDEBRIS2SPR);
				break;
			// Moxel
			case 24:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 23:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 17:
				AR_SpawnMoxel(x, y);
				CA_MARKLUMP(MOXELLUMP);
				CA_MARKLUMP(MOXELSHOTLUMP);
				break;
			case 18:
				AR_SpawnKrileGhost(x, y);
				CA_MARKLUMP(KRILEGHOSTLUMP);
				break;
			// Axosis
			case 74:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 73:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 20:
				AR_SpawnAxosis(x, y);
				CA_MARKLUMP(AXOSISLUMP);
				CA_MARKLUMP(AXOSIS2LUMP);
				break;
			case 34:
				if (ck_gameState.numShots < 5)
					CK_SpawnItem(x, y, 11);
				break;
			case 35:
				AR_SpawnBridge(x, y);
				CA_MARKLUMP(BRIDGELUMP);
				break;
			case 87:
				AR_SpawnAtroxusBox(x, y);
				CA_MarkGrChunk(ATROXUSDETECTSPR);
				break;
			case 78:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 77:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 76:
				AR_SpawnAtroxian(x, y);
				CA_MARKLUMP(ATROXIANLUMP);
				CA_MARKLUMP(SMALLEXPOSIONLUMP);
				CA_MARKLUMP(MAGMINEEXPLOSIONLUMP);
				CA_MarkGrChunk(SPR_ATROXIANDEAD);
				break;
			// Hornet
			case 52:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 51:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 7:
				AR_SpawnHornet(x, y);
				CA_MARKLUMP(HORNETLUMP);
				break;
			case 11:
				AR_SpawnMagmine(x, y);
				CA_MARKLUMP(MAGMINELUMP);
				break;
			case 12:
				AR_SpawnDartfrog(x, y);
				CA_MARKLUMP(DARTFROGLUMP);
				break;
			case 19:
				AR_SpawnSplider(x, y);
				CA_MARKLUMP(SPLIDERLUMP);
				break;
			case 21:
				AR_SpawnBeetle(x, y);
				CA_MARKLUMP(BEETLELUMP);
				break;
			case 44:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 43:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 22:
				AR_SpawnLarvac(x, y);
				CA_MARKLUMP(LARVACLUMP);
				break;
			case 46:
				if (ck_gameState.difficulty < D_Hard)
					break;
			case 45:
				if (ck_gameState.difficulty < D_Normal)
					break;
			case 8:
				AR_SpawnLarvbat(x, y);
				CA_MARKLUMP(LARVBATLUMP);
				// (as the source: its dying frames aren't in the lump)
				CA_MarkGrChunk(SPR_LARVBATDIE1);
				CA_MarkGrChunk(SPR_LARVBATDIE2);
				CA_MarkGrChunk(SPR_LARVBATDIE3);
				CA_MarkGrChunk(SPR_LARVBATDIE4);
				break;
			case 53: case 54: case 55: case 56:
				AR_SpawnDartgun(x, y, infoValue - 53);
				CA_MARKLUMP(DARTLUMP);
				break;
			case 79: case 80: case 81: case 82:
				if (ck_gameState.difficulty >= D_Normal)
				{
					AR_SpawnDartgun(x, y, infoValue - 79);
					CA_MARKLUMP(DARTLUMP);
				}
				break;
			case 83: case 84: case 85: case 86:
				if (ck_gameState.difficulty >= D_Hard)
				{
					AR_SpawnDartgun(x, y, infoValue - 83);
					CA_MARKLUMP(DARTLUMP);
				}
				break;
			case 70: case 71: case 72:
				AR_SpawnFireball(x, y, infoValue - 70);
				CA_MARKLUMP(FIREBALLLUMP);
				break;
			case 69: // its 3-up
				CA_MarkGrChunk(ITEM3UP1SPR);
				CA_MarkGrChunk(ITEM3UP2SPR);
				CA_MarkGrChunk(ITEM3UP3SPR);
				CK_SpawnItem(x, y, 12);
				break;
			case 42:
				AR_SpawnCheckPoint(x, y);
				CA_MARKLUMP(CHECKPOINTLUMP);
				break;
			case 25:
				RF_SetScrollBlock(x, y, true);
				break;
			case 26:
				RF_SetScrollBlock(x, y, false);
				break;
			case 27:
			case 28:
			case 29:
			case 30:
				CK_SpawnAxisPlatform(x, y, infoValue - 27, false);
				CA_MARKLUMP(PLATFORMLUMP);
				break;
			case 32:
				CK_SpawnFallPlat(x, y);
				CA_MARKLUMP(PLATFORMLUMP);
				break;
			case 57: case 58: case 59: case 60: case 61: case 62:
			case 63: case 64: case 65: case 66: case 67: case 68:
				CK_SpawnItem(x, y, infoValue - 57);
				break;
			}
		}
	}

	for (CK_object *obj = ck_keenObj; obj != NULL; obj = obj->next)
	{
		if (obj->active != OBJ_ALWAYS_ACTIVE)
			obj->active = OBJ_INACTIVE;
		// The Krile ghost starts out after the first Axosis.
		if (ar_state.krileGhost && obj->type == AR_CT_Axosis && obj->user1 == 0)
			ar_state.krileGhost->user4 = AR_ObjToUser(obj);
	}
	CA_MarkAllLumps();
}

// ---- Keen's own differences (KD_KEEN.C) ----

// His think function while shooting in the air.
void CK_KeenFall(CK_object *obj)
{
	CK_PhysGravityHigh(obj);
	ck_nextX = obj->velX * SD_GetSpriteSync();
}

// Its switches ("Fleexy's code" in KD_KEEN.C): one switch can stand for a
// stack of them. Each either flips a bridge (foreground tiles to their next
// animation tile) or changes an info value (a platform arrow or blocker).
//   type: 0 bridge, 1 info value, 2 by the switch's own tile property
//   animSwitch: 0 not a real switch, 1 a normal one, 2 only animate it
static bool AR_SwitchFlipWorker(int xpos, int ypos, int type, int animSwitch)
{
	static const int infoPlaneInverses[8] = {2, 3, 0, 1, 6, 7, 4, 5};
	uint16_t switchTarget = CA_TileAtPos(xpos, ypos, 2);
	int switchTargetX = switchTarget >> 8, switchTargetY = switchTarget & 0xFF;
	int switchMisc = 0;
	if (animSwitch > 0)
	{
		uint16_t switchTile = CA_TileAtPos(xpos, ypos, 1);
		switchMisc = TI_ForeMisc(switchTile);
		if (switchMisc != 15 && switchMisc != 5 && switchMisc != 6) // bridge, platform on, platform off
			return false;
		uint16_t switchNextTile = switchTile + TI_ForeAnimTile(switchTile);
		RF_ReplaceTiles(&switchNextTile, 1, xpos, ypos, 1, 1);
	}
	else if (switchTarget < 0x100)
		return false;

	if ((animSwitch == 0 && type == 0) || (animSwitch == 1 && switchMisc == 15))
	{
		for (int tileY = switchTargetY; tileY < switchTargetY + 2; ++tileY)
			for (int tileX = switchTargetX - ((tileY == switchTargetY) ? 0 : 1); tileX < CA_GetMapWidth(); ++tileX)
			{
				uint16_t currentTile = CA_TileAtPos(tileX, tileY, 1);
				if (!TI_ForeAnimTile(currentTile))
					break;
				uint16_t newTile = currentTile + TI_ForeAnimTile(currentTile);
				RF_ReplaceTiles(&newTile, 1, tileX, tileY, 1, 1);
			}
	}
	else if ((animSwitch == 0 && type == 1) || animSwitch == 1)
	{
		uint16_t infoPlaneValue = CA_TileAtPos(switchTargetX, switchTargetY, 2);
		if (infoPlaneValue >= 91 && infoPlaneValue < 99)
			infoPlaneValue = infoPlaneInverses[infoPlaneValue - 91] + 91; // turn a platform arrow round
		else
			infoPlaneValue ^= 0x1F; // put in or take out a blocker
		if (switchTargetX < CA_GetMapWidth() && switchTargetY < CA_GetMapHeight())
			CA_mapPlanes[2][switchTargetY * CA_GetMapWidth() + switchTargetX] = infoPlaneValue;
	}
	return true;
}

void AR_KeenPressSwitch(CK_object *obj)
{
	int switchX = obj->clipRects.tileXmid, switchY = obj->clipRects.tileY1;
	uint16_t switchTarget = CA_TileAtPos(switchX, switchY, 2);
	int type = TI_ForeMisc(CA_TileAtPos(switchX, switchY, 1)) == 15 ? 0 : 1;

	SD_PlaySound(KEENOUTOFAMMOSND);
	if (switchTarget == 0xF1EE)
	{
		// A stack of info values above it, all of this switch's kind
		AR_SwitchFlipWorker(switchX, switchY, 0, 2);
		while (--switchY >= 0 && AR_SwitchFlipWorker(switchX, switchY, type, 0))
			;
	}
	else
	{
		// A stack of switches, each of its own kind
		while (switchY >= 0 && AR_SwitchFlipWorker(switchX, switchY, 2, 1))
			switchY--;
	}
}

// The last frame of being pulled under by the vines ends the level.
void AR_KeenVines(CK_object *obj)
{
	ck_gameState.levelState = LS_ARVines;
}

// A vines tile (tile property 16) under a standing Keen pulls him under,
// once per game. The tile's info value says where on the map he comes out.
bool AR_KeenVinesTile(CK_object *obj, int tileX, int tileY)
{
	if (ar_state.vines != 0 || !ck_keenObj->topTI)
		return false;
	uint16_t info = CA_TileAtPos(tileX, tileY, 2);
	ck_gameState.mapPosX = RF_TileToUnit(info >> 8);
	ck_gameState.mapPosY = RF_TileToUnit(info & 0xFF);
	ck_scrollDisabled = true;
	obj->currentAction = CK_ACTION(AR_ACT_KeenPulledByVines1);
	obj->clipped = CLIP_not;
	ar_state.vines = 2;
	return true;
}

// Info value 41 in the Dark Temple (level 16): where Keen stands once the
// talk with Atroxus is over.
static void AR_SpawnBossKeen(int tileX, int tileY, int direction)
{
	if (ca_mapOn != 16 || !ck_gameState.levelsDone[16])
		return;
	ck_keenObj->type = CT_Player;
	ck_keenObj->active = OBJ_ALWAYS_ACTIVE;
	ck_keenObj->visible = true;
	ck_keenObj->zLayer = 1;
	ck_keenObj->clipped = CLIP_normal;
	ck_keenObj->posX = RF_TileToUnit(tileX);
	ck_keenObj->posY = RF_TileToUnit(tileY) - 241;
	ck_keenObj->xDirection = direction;
	ck_keenObj->yDirection = 1;
	CK_SetAction(ck_keenObj, CK_ACTION(CK_ACT_keenStanding));
}

// The map's flag doesn't flip for level 10 when the vines ended it.
bool AR_NoFlagFlip(int level)
{
	return level == 10 && ar_state.vines == 2;
}

int AR_MapSongAfterZiggurat(void)
{
	return SKY_SANCTUM_MAP_MUS;
}

// ---- The map transporter (AR_MAP.C) ----
// After the Great Ziggurat (level 15) Keen arrives on the map in a saucer
// that flies to the tile its info value names, where he steps out.

static void AR_SpawnTransporterKeen(int tileX, int tileY)
{
	ck_keenObj->type = CT_Player;
	ck_keenObj->clipped = CLIP_not;
	ck_keenObj->active = OBJ_ALWAYS_ACTIVE;
	ck_keenObj->posX = RF_TileToUnit(tileX);
	ck_keenObj->posY = RF_TileToUnit(tileY);
	ck_keenObj->zLayer = 2;

	uint16_t info = CA_TileAtPos(tileX - 1, tileY, 2);
	int32_t destX = RF_TileToUnit(ck_keenObj->user1 = info >> 8);
	int32_t destY = RF_TileToUnit(ck_keenObj->user2 = info & 0xFF);
	int32_t delX = destX - ck_keenObj->posX, delY = destY - ck_keenObj->posY;
	int32_t velX = 0, velY = 0;
	if (abs(delX) > abs(delY))
	{
		velX = delX > 0 ? 0x2000 : -0x2000;
		velY = delY * velX / delX;
	}
	else if (delY)
	{
		velY = delY > 0 ? 0x2000 : -0x2000;
		velX = delX * velY / delY;
	}
	ck_keenObj->velX = velX;
	ck_keenObj->velY = velY;
	CK_SetAction(ck_keenObj, CK_ACTION(AR_ACT_TransporterKeen1));
}

void AR_TransporterKeen(CK_object *obj)
{
	int tileX = RF_UnitToTile(obj->posX), tileY = RF_UnitToTile(obj->posY);
	if (obj->user1 - 1 <= tileX && tileX <= obj->user1 + 1 && obj->user2 - 1 <= tileY && tileY <= obj->user2 + 1)
	{
		obj->zLayer = 1;
		obj->xDirection = obj->yDirection = IN_motion_None;
		obj->user1 = 6;
		obj->user2 = 3;
		obj->user3 = 0;
		obj->gfxChunk = WORLDKEENDR1SPR;
		obj->clipped = CLIP_normal;
		CK_SetAction2(obj, CK_ACTION(CK5_ACT_MapKeenStart));
		return;
	}
	ck_nextX = obj->velX / 0x100 * SD_GetSpriteSync();
	ck_nextY = obj->velY / 0x100 * SD_GetSpriteSync();
}

void AR_Misc_SetupFunctions(void)
{
	CK_ACT_AddFunction("CK_KeenFall", &CK_KeenFall);
	CK_ACT_AddFunction("AR_KeenVines", &AR_KeenVines);
	CK_ACT_AddFunction("AR_TransporterKeen", &AR_TransporterKeen);
	CK_ACT_AddColFunction("AR_CheckPointCol", &AR_CheckPointCol);
}

// ---- Keen Worlds' peaceful levels ----
// Its object types reuse Keen 5's numbers for other things, so it has its
// own lists. Hostile: every creature, shot and hazard. Story triggers: what
// ends a level or starts a dialogue when touched.

bool AR_IsHostileType(int type)
{
	return type == CT5_EnemyShot || type == CT5_StunnedCreature ||
		(type >= AR_CT_Larvac && type <= AR_CT_Dartgun) || type == AR_CT_Atroxus;
}

bool AR_IsStoryTrigger(const CK_object *obj)
{
	if (obj->type == AR_CT_Generator || obj->type == AR_CT_Elder || obj->type == AR_CT_Bridge)
		return true;
	if (obj->type != CT_Friendly || !obj->currentAction || !obj->currentAction->collide)
		return false;
	static const char *talkers[] = {"AR_ElderBWBCollide", "AR_AtroxusDialogueCollide", "AR_SecretKrileCollide",
		"AR_AtroxusBoxCollide"};
	for (size_t i = 0; i < sizeof(talkers) / sizeof(talkers[0]); ++i)
		if (obj->currentAction->collide == CK_ACT_GetColFunction(talkers[i]))
			return true;
	return false;
}

// Saved games (Keen Worlds' save block): what must outlive a level. The
// ghost is found again among the loaded objects; a checkpoint isn't saved.
static bool ar_relinkGhost;
void AR_ClearCheckpoint(void);

bool AR_WriteState(FS_File fp)
{
	int16_t v[14] = {ar_state.vines, ar_state.generatorsDestroyed, ar_state.endscreen_played[0],
		ar_state.endscreen_played[1], ar_state.endscreen_played[2], ar_state.endscreen_played[3],
		ar_state.endscreen_played[4], ar_state.doSecretEnding, ar_state.atroxusBoxesPlayed,
		ar_state.atroxusBoxesLevels[0], ar_state.atroxusBoxesLevels[1], ar_state.atroxusBoxesLevels[2],
		(int16_t)ar_state.axosisNum, 0};
	return FS_WriteInt16LE(v, 14, fp) == 14;
}

bool AR_ReadState(FS_File fp)
{
	int16_t v[14];
	if (FS_ReadInt16LE(v, 14, fp) != 14)
		return false;
	memset(&ar_state, 0, sizeof(ar_state));
	ar_state.vines = v[0];
	ar_state.generatorsDestroyed = v[1];
	for (int i = 0; i < 5; ++i)
		ar_state.endscreen_played[i] = v[2 + i] != 0;
	ar_state.doSecretEnding = v[7] != 0;
	ar_state.atroxusBoxesPlayed = v[8];
	for (int i = 0; i < 3; ++i)
		ar_state.atroxusBoxesLevels[i] = v[9 + i];
	ar_state.axosisNum = v[12];
	ar_relinkGhost = true;
	AR_ClearCheckpoint();
	return true;
}

// For the debug ending: its state as after a real play-through (both
// generators destroyed, the vines already met).
void AR_DebugCompleteState(void)
{
	if (!AR_IsActive())
		return;
	ar_state.generatorsDestroyed = 2;
	ar_state.vines = 2;
}

// A new game: nothing of its extra state carries over.
void AR_ResetState(void)
{
	memset(&ar_state, 0, sizeof(ar_state));
	AR_ClearCheckpoint();
}

// ---- In-level checkpoints (AR_MISC.C) ----
// Touching a checkpoint saves the level as it is (map, objects, game
// state); dying after that restarts the level from there. The source writes
// a CHKPOINT file; here it is kept in memory. It is saved at the end of the
// frame, not inside the collision, so every object is in a settled state.

static CK_object *ar_touchedCheckpoint;
static struct
{
	bool valid;
	int level;
	CK_GameState gameState;
	AR_State state;
	uint16_t *planes[3];
	size_t planeBytes;
	CK_object *objects;
	int numObjects;
} ar_checkpoint;

void AR_ClearCheckpoint(void)
{
	for (int i = 0; i < 3; ++i)
	{
		free(ar_checkpoint.planes[i]);
		ar_checkpoint.planes[i] = NULL;
	}
	free(ar_checkpoint.objects);
	ar_checkpoint.objects = NULL;
	ar_checkpoint.valid = false;
	ar_touchedCheckpoint = NULL;
}

bool AR_HasCheckpoint(void)
{
	return ar_checkpoint.valid;
}

void AR_SpawnCheckPoint(int tileX, int tileY)
{
	CK_object *obj = CK_GetNewObj(false);
	obj->type = AR_CT_CheckPoint;
	obj->active = OBJ_ACTIVE;
	obj->zLayer = 0;
	obj->posX = RF_TileToUnit(tileX);
	obj->posY = RF_TileToUnit(tileY);
	obj->clipped = CLIP_not;
	CK_SetAction(obj, CK_ACTION(AR_ACT_CheckPointIdle));
}

void AR_CheckPointCol(CK_object *obj1, CK_object *obj2)
{
	if (obj2->type == CT_Player && obj2->currentAction != CK_ACTION(CK_ACT_keenDie0) &&
		obj2->currentAction != CK_ACTION(CK_ACT_keenDie1))
		ar_touchedCheckpoint = obj1;
}

static void AR_SaveCheckpoint(CK_object *checkpoint)
{
	AR_ClearCheckpoint();
	ar_checkpoint.level = ca_mapOn;
	ck_keenState.platform = NULL;
	ar_checkpoint.gameState = ck_gameState;
	ar_checkpoint.state = ar_state;
	ar_checkpoint.planeBytes = (size_t)CA_GetMapWidth() * CA_GetMapHeight() * 2;
	for (int i = 0; i < 3; ++i)
	{
		ar_checkpoint.planes[i] = (uint16_t *)malloc(ar_checkpoint.planeBytes);
		memcpy(ar_checkpoint.planes[i], CA_mapPlanes[i], ar_checkpoint.planeBytes);
	}
	int n = 0;
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
		++n;
	ar_checkpoint.objects = (CK_object *)malloc(n * sizeof(CK_object));
	ar_checkpoint.numObjects = n;
	n = 0;
	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
		ar_checkpoint.objects[n++] = *obj;
	// Keen restarts standing at the checkpoint.
	ar_checkpoint.objects[0].posX = checkpoint->posX;
	ar_checkpoint.objects[0].posY = checkpoint->posY;
	ar_checkpoint.valid = true;
}

// Once a frame, after the objects have moved and collided.
void AR_CheckpointFrame(void)
{
	// After a saved game was loaded: find the Krile ghost again.
	if (ar_relinkGhost)
	{
		ar_relinkGhost = false;
		ar_state.krileGhost = NULL;
		if (AR_IsActive())
			for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
				if (obj->type == AR_CT_KrileGhost)
					ar_state.krileGhost = obj;
	}
	if (!ar_touchedCheckpoint)
		return;
	CK_object *checkpoint = ar_touchedCheckpoint;
	ar_touchedCheckpoint = NULL;
	CK_SetAction2(checkpoint, CK_ACTION(AR_ACT_CheckPointSpin1));
	AR_SaveCheckpoint(checkpoint);

	CK_object *shadow = CK_GetNewObj(true);
	shadow->type = CT_Friendly;
	shadow->active = OBJ_EXISTS_ONLY_ONSCREEN;
	shadow->clipped = CLIP_not;
	shadow->posX = checkpoint->posX - 0x80;
	shadow->posY = checkpoint->posY;
	shadow->yDirection = IN_motion_Up;
	shadow->zLayer = PRIORITIES - 1;
	CK_SetAction(shadow, CK_ACTION(AR_ACT_CheckPointShadow));
	SD_PlaySound(CHECKPOINTSND);
}

static void AR_RestoreObject(CK_object *obj, const CK_object *saved)
{
	CK_object *prev = obj->prev, *next = obj->next;
	*obj = *saved;
	obj->prev = prev;
	obj->next = next;
	obj->visible = true;
	obj->sde = NULL;
}

// After a level has been loaded: restarting the level a checkpoint was
// touched in puts everything back as it was then. Lives, score, ammo (at
// least 10 shots) and vitalin stay as they are now. Any other level load
// drops the checkpoint.
void AR_AfterLevelLoad(void)
{
	if (!ar_checkpoint.valid)
		return;
	if (!AR_IsActive() || ca_mapOn != ar_checkpoint.level || ca_mapOn == 0 ||
		ar_checkpoint.planeBytes != (size_t)CA_GetMapWidth() * CA_GetMapHeight() * 2)
	{
		AR_ClearCheckpoint();
		return;
	}

	CK_GameState now = ck_gameState;
	AR_State stateNow = ar_state;
	ck_gameState = ar_checkpoint.gameState;
	ck_gameState.numShots = now.numShots < 10 ? 10 : now.numShots;
	ck_gameState.numLives = now.numLives;
	ck_gameState.numCentilife = now.numCentilife;
	ck_gameState.keenScore = now.keenScore;
	ck_gameState.nextKeenAt = now.nextKeenAt;
	ck_gameState.levelState = now.levelState;
	ar_state = ar_checkpoint.state;
	memcpy(ar_state.endscreen_played, stateNow.endscreen_played, sizeof(ar_state.endscreen_played));
	for (int i = 0; i < 3; ++i)
		memcpy(CA_mapPlanes[i], ar_checkpoint.planes[i], ar_checkpoint.planeBytes);

	CK_SetupObjArray();
	ar_state.krileGhost = NULL;
	AR_RestoreObject(ck_keenObj, &ar_checkpoint.objects[0]);
	CK_SpawnKeen(RF_UnitToTile(ck_keenObj->posX), RF_UnitToTile(ck_keenObj->posY), ck_keenObj->xDirection);
	for (int i = 1; i < ar_checkpoint.numObjects; ++i)
	{
		CK_object *obj = i == 1 ? ck_scoreBoxObj : CK_GetNewObj(false);
		AR_RestoreObject(obj, &ar_checkpoint.objects[i]);
		if (obj->type == CT5_StunnedCreature)
			obj->user3 = 0;
		else if (obj->type == CT5_Platform)
			obj->user2 = obj->user3 = 0;
		else if (obj->type == AR_CT_KrileGhost)
		{
			ar_state.krileGhost = obj;
			obj->user4 = 0;
		}
	}
	ck_scoreBoxObj->user1 = ck_scoreBoxObj->user2 = ck_scoreBoxObj->user3 = ck_scoreBoxObj->user4 = -1;
	// The ghost goes on hunting the Axosis it was after.
	if (ar_state.krileGhost)
		for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
			if (obj->type == AR_CT_Axosis && obj->user1 == ar_state.krileGhost->user3)
				ar_state.krileGhost->user4 = AR_ObjToUser(obj);
}

// The player's game files and our generated support files, all in the
// world's own folder.
bool AR_IsPresent(void)
{
	static const char *files[] = {"EGAGRAPH.CKD", "EGAHEAD.CKD", "EGADICT.CKD", "maptemp.ckd", "maphead.ckd",
		"AUDIOT.CKD", "AUDIOHED.CKD", "AUDIODCT.CKD", "GFXINFOE.CKD", "AUDINFOE.CKD", "ARACTION.CKD", "ARGFX.CKD"};
	for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
		if (!FS_IsKeenFilePresent(files[i]))
			return false;
	return true;
}

// The engine treats it as Keen 5 (its source's ancestor); only the file
// extension and the level setup differ.
CK_EpisodeDef ar_episode = {
	EP_CK5,
	"CKD",
	"AREALM.CKD",
	&AR_ScanInfoLayer,
	&AR_IsPresent,
};
