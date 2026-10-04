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

#include "id_ca.h"
#include "id_fs.h"
#include "id_rf.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_play.h"
#include "ck_ep.h"
#include "ck5_ep.h"

void CK_SpawnAxisPlatform(int tileX, int tileY, int direction, bool purple);
void CK_SpawnFallPlat(int tileX, int tileY);


// Its level setup (CK5_ScanInfoLayer in AR_MISC.C). Creature numbers are
// skipped until their code is ported.
void AR_ScanInfoLayer(void)
{
	CA_InitLumps();
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
		if (obj->active != OBJ_ALWAYS_ACTIVE)
			obj->active = OBJ_INACTIVE;
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
