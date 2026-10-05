/*
Keen Worlds: the tavern's own tune in Border Village.

The tune is an IMF stream (AdLib register writes with delays, like the
games' own music) kept in the engine, so players' AUDIO.CK4 files are
untouched.
*/

#include <stdio.h>
#include <stdlib.h>

#include "id_ca.h"
#include "id_fs.h"
#include "id_vl.h"
#include "ck_cross.h"
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
#include "kw_title_music.h"

// Inside the ship its own tune plays instead of a level's song.
// ---- Voice clips (data/voice: signed 16-bit mono, low byte first). Music
// never plays under a voice: it fades out, stays off while the clip plays,
// and fades back in when the clip is over (KW_UpdateMusicZone). ----

#define KW_VOICE_RATE 22050
#define KW_MUSIC_FADE_OUT 500  // milliseconds
#define KW_MUSIC_FADE_IN 2500
#define KW_ZONE_FADE_IN 900    // through a door

static bool kw_musicHeld;          // off for a voice; comes back when it has finished
static uint32_t kw_voiceEnd = 0;   // time count at which the clip is over
static bool kw_voicePending;       // something is about to be said (see KW_VoiceComingUp)

bool KW_VoiceSpeaking(void)
{
	return kw_voicePending || SD_GetTimeCount() < kw_voiceEnd;
}

void KW_VoiceComingUp(bool pending)
{
	kw_voicePending = pending;
}

// Fade the music down; returns how many tics that takes. The caller waits,
// then calls KW_MusicQuiet() (or KW_Speak, which does).
int KW_FadeMusicOut(void)
{
	if (kw_musicHeld)
		return 0;
	SD_FadeAdLibVolume(0, KW_MUSIC_FADE_OUT);
	return KW_MUSIC_FADE_OUT * 70 / 1000 + 1;
}

void KW_MusicQuiet(void)
{
	SD_MusicOff();
	SD_FadeAdLibVolume(256, 0); // the games' sound effects stay at full volume
	kw_musicHeld = true;
}

// The clip is cut short (its card was put away); the music comes back.
void KW_StopVoice(void)
{
	SD_StopVoice();
	kw_voiceEnd = 0;
}

void KW_Speak(const char *file, bool waitForFade)
{
	static int16_t *clip = NULL;
	SD_StopVoice();
	free(clip);
	clip = NULL;
	kw_voiceEnd = 0;
	if (SD_GetSoundMode() == sdm_Off || !file)
		return;
	char path[512];
	snprintf(path, sizeof(path), "%s/voice/%s", fs_keenPath, file);
	FILE *f = fopen(path, "rb");
	if (!f)
		return;
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (size >= 2 && (clip = (int16_t *)malloc(size)) != NULL && fread(clip, 1, size, f) == (size_t)size)
	{
		for (long i = 0; i < size / 2; ++i)
			clip[i] = (int16_t)CK_Cross_SwapLE16((uint16_t)clip[i]);
		if (!kw_musicHeld && waitForFade)
			VL_DelayTics(KW_FadeMusicOut());
		KW_MusicQuiet();
		SD_PlayVoice(clip, (uint32_t)(size / 2), KW_VOICE_RATE);
		kw_voiceEnd = SD_GetTimeCount() + (uint32_t)(size / 2 * 70 / KW_VOICE_RATE) + 35;
	}
	fclose(f);
}

// The title tune of Commander Keen: Worlds (tools/kw_title_music.py): with
// the title picture when the game is launched, and under Keen Worlds' story.
// It plays on from one into the other rather than starting again.
static bool kw_titleTunePlaying;

void KW_StartTitleMusic(void)
{
	if (kw_titleTunePlaying)
		return;
	KW_StopVoice();
	SD_FadeAdLibVolume(256, 0);
	SD_StartMusicData(kw_titleMusic, sizeof(kw_titleMusic));
	kw_titleTunePlaying = true;
}

// Some other music was started, or the music was stopped (StartMusic and
// StopMusic in ck_play.c).
void KW_TitleMusicIsOver(void)
{
	kw_titleTunePlaying = false;
}

bool KW_StartShipMusic(void)
{
	if (KW_CurrentWorld() != KW_SHIP)
		return false;
	if (KW_VoiceSpeaking())
		KW_MusicQuiet();
	else
		SD_StartMusicData(kw_shipMusic, sizeof(kw_shipMusic));
	return true;
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
	bool inTavern = KW_IsInTavern();
	if (kw_musicHeld)
	{
		// The voice has finished: the music of where Keen is comes back in gently.
		kw_tavernThemePlaying = inTavern;
		if (KW_VoiceSpeaking())
			return;
		kw_musicHeld = false;
		SD_FadeAdLibVolume(0, 0);
		if (KW_CurrentWorld() == KW_SHIP)
			KW_StartShipMusic();
		else if (inTavern)
			SD_StartMusicData(kw_tavernMusic, sizeof(kw_tavernMusic));
		else if (ca_mapOn >= 0)
			StartMusic(ca_mapOn);
		SD_FadeAdLibVolume(256, KW_MUSIC_FADE_IN);
		return;
	}
	// Through the tavern's door: the music outside fades down and the
	// tavern's tune comes up, and the other way round.
	static uint32_t changeAt = 0; // time count at which the old music has faded out (0: no change under way)
	if (inTavern == kw_tavernThemePlaying)
	{
		if (changeAt)
		{
			// Keen turned round in the doorway: the music that was fading comes back up
			changeAt = 0;
			SD_FadeAdLibVolume(256, KW_MUSIC_FADE_OUT);
		}
		return;
	}
	if (!changeAt)
	{
		changeAt = SD_GetTimeCount() + KW_FadeMusicOut();
		return;
	}
	if (SD_GetTimeCount() < changeAt)
		return;
	changeAt = 0;
	kw_tavernThemePlaying = inTavern;
	SD_FadeAdLibVolume(0, 0);
	if (inTavern)
		SD_StartMusicData(kw_tavernMusic, sizeof(kw_tavernMusic));
	else if (ca_mapOn >= 0)
		StartMusic(ca_mapOn);
	SD_FadeAdLibVolume(256, KW_ZONE_FADE_IN);
}
