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

// The Megarocket's tune: space funk, written out by tools/kw_ship_music.py.
#include "kw_ship_music.h"

// Inside the ship its own tune plays instead of a level's song.
// The tune waits while the ship computer speaks (a landing announcement),
// so the words can be heard.
static bool kw_shipMusicHeld;

void KW_HoldShipMusic(void)
{
	kw_shipMusicHeld = true;
}

bool KW_StartShipMusic(void)
{
	if (KW_CurrentWorld() != KW_SHIP)
		return false;
	if (KW_ShipComputerSpeaking())
	{
		SD_MusicOff();
		kw_shipMusicHeld = true;
	}
	else
		SD_StartMusicData(kw_shipMusic, sizeof(kw_shipMusic));
	return true;
}

#define KW_SHIP_MUSIC_FADE_IN 2500  // milliseconds
#define KW_SHIP_MUSIC_FADE_OUT 700

// The tune goes quiet (for the ship computer): it fades down, the caller
// waits that long, then KW_ShipMusicQuiet() turns it off.
int KW_FadeShipMusicOut(void)
{
	SD_FadeAdLibVolume(0, KW_SHIP_MUSIC_FADE_OUT);
	return KW_SHIP_MUSIC_FADE_OUT * 70 / 1000 + 1; // in tics
}

void KW_ShipMusicQuiet(void)
{
	SD_MusicOff();
	SD_FadeAdLibVolume(256, 0); // the games' sound effects stay at full volume
	kw_shipMusicHeld = true;
}

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
	if (kw_shipMusicHeld && (KW_CurrentWorld() != KW_SHIP || !KW_ShipComputerSpeaking()))
	{
		kw_shipMusicHeld = false;
		// ... and comes back in gently
		if (KW_StartShipMusic())
		{
			SD_FadeAdLibVolume(0, 0);
			SD_FadeAdLibVolume(256, KW_SHIP_MUSIC_FADE_IN);
		}
	}
	bool inTavern = KW_IsInTavern();
	if (inTavern == kw_tavernThemePlaying)
		return;

	kw_tavernThemePlaying = inTavern;
	if (inTavern)
		SD_StartMusicData(kw_tavernMusic, sizeof(kw_tavernMusic));
	else if (ca_mapOn >= 0)
		StartMusic(ca_mapOn);
}
