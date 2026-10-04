/*
Keen Worlds: the tavern's own tune in Border Village.

The tune is an IMF stream (AdLib register writes with delays, like the
games' own music) kept in the engine, so players' AUDIO.CK4 files are
untouched.
*/

#include "id_ca.h"
#include "id_sd.h"
#include "ck_def.h"
#include "ck_play.h"
#include "kw_habitat.h"
#include "kw_music.h"
#include "kw_world.h"

// The tune: an original jig, written out by tools/kw_tavern_music.py.
#include "kw_tavern_music.h"

static bool kw_tavernThemePlaying;

static bool KW_IsInTavern(void)
{
	if (KW_CurrentWorld() != KW_KEEN4 || ca_mapOn != 1 || !KW_IsLevelPeaceful() || !ck_keenObj)
		return false;

	// The room is carved below the inn. The doorway is at (63, 31); starting
	// one tile inside avoids switching while Keen is still on the street.
	uint16_t x = ck_keenObj->clipRects.tileXmid;
	uint16_t y = ck_keenObj->clipRects.tileY2;
	return x >= 64 && x <= 82 && y >= 26 && y <= 34;
}

void KW_UpdateMusicZone(void)
{
	bool inTavern = KW_IsInTavern();
	if (inTavern == kw_tavernThemePlaying)
		return;

	kw_tavernThemePlaying = inTavern;
	if (inTavern)
		SD_StartMusicData(kw_tavernMusic, sizeof(kw_tavernMusic));
	else if (ca_mapOn >= 0)
		StartMusic(ca_mapOn);
}
