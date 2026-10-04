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
				CK_SpawnMapKeen(x, y);
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
			// Keen for the boss fight (the source has its own spawn for him;
			// not ported yet)
			case 41:
				CK_SpawnKeen(x, y, 1);
				CK_DemoSignSpawn();
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
