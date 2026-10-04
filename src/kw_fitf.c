/*
Keen Worlds: Foray in the Forest (Nisaba and K1n9_Duk3) as a world.

Foray in the Forest is a game of its own with public GPL source. That source
descends from Atroxian Realm's (so from Omnispeak's Keen 5 code), rewritten
with Keen Dreams names. Here it runs on the engine's Keen 5 code with its own
data files (extension CKS), its own variable files (written by
tools/kw_fitf.py from its source) and its own level setup below.

So far: its maps, graphics, sounds and sprite states load, and Keen walks
its map and levels. Its creatures, Keen's changes and its own game rules are
not ported yet (see ../../research/foray-in-the-forest.md).

Licensed under the GNU GPL v2 or later, like Omnispeak and Foray in the
Forest.
*/

#include "id_ca.h"
#include "id_fs.h"
#include "ck_act.h"
#include "ck_def.h"
#include "ck_ep.h"
#include "ck_play.h"
#include "kw_fitf_defs.h"

extern CK_EpisodeDef ff_episode;

bool FF_IsActive(void)
{
	return ck_currentEpisode == &ff_episode;
}

// A sprite cache of its GRAPHCKS.H (START_LUMP/END_LUMP there)
static void FF_NeedChunks(int first, int last)
{
	for (int i = first; i <= last; ++i)
		CA_MarkGrChunk(i);
}
#define FF_NEEDLUMP(name) FF_NeedChunks(FF_##name##_LUMP_START, FF_##name##_LUMP_END)

// Its level setup (ScanInfoPlane in CK_SPEC.C). Creature numbers are skipped
// until their code is ported.
void FF_ScanInfoLayer(void)
{
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
				FF_NEEDLUMP(KEEN);
				break;
			case 3:
				CK_SpawnMapKeen(x, y);
				CK_DemoSignSpawn();
				CA_MarkGrChunk(CK_CHUNKNUM(SPR_SCOREBOX));
				FF_NEEDLUMP(WORLDKEEN);
				break;
			}
		}
	}

	for (CK_object *obj = ck_keenObj; obj; obj = obj->next)
	{
		if (obj->active != OBJ_ALWAYS_ACTIVE)
			obj->active = OBJ_INACTIVE;
	}
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
