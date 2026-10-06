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

// Is a clip sounding right now? (KW_VoiceSpeaking also says yes while one
// is only expected.)
bool KW_ClipPlaying(void)
{
	return SD_GetTimeCount() < kw_voiceEnd;
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

// Read a clip into memory (the one clip that can play at a time).
static int16_t *kw_clip = NULL;
static long kw_clipSamples = 0;

static bool KW_LoadClip(const char *file)
{
	SD_StopVoice();
	free(kw_clip);
	kw_clip = NULL;
	kw_voiceEnd = 0;
	if (SD_GetSoundMode() == sdm_Off || !file)
		return false;
	char path[512];
	snprintf(path, sizeof(path), "%s/voice/%s", fs_keenPath, file);
	FILE *f = fopen(path, "rb");
	if (!f)
		return false;
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	bool ok = size >= 2 && (kw_clip = (int16_t *)malloc(size)) != NULL && fread(kw_clip, 1, size, f) == (size_t)size;
	fclose(f);
	if (!ok)
	{
		free(kw_clip);
		kw_clip = NULL;
		return false;
	}
	kw_clipSamples = size / 2;
	for (long i = 0; i < kw_clipSamples; ++i)
		kw_clip[i] = (int16_t)CK_Cross_SwapLE16((uint16_t)kw_clip[i]);
	return true;
}

static void KW_StartClip(void)
{
	SD_PlayVoice(kw_clip, (uint32_t)kw_clipSamples, KW_VOICE_RATE);
	kw_voiceEnd = SD_GetTimeCount() + (uint32_t)(kw_clipSamples * 70 / KW_VOICE_RATE) + 35;
}

void KW_Speak(const char *file, bool waitForFade)
{
	if (!KW_LoadClip(file))
		return;
	if (!kw_musicHeld && waitForFade)
		VL_DelayTics(KW_FadeMusicOut());
	KW_MusicQuiet();
	KW_StartClip();
}

// ---- Lines of dialogue in a game's own windows (Keen 4's: clips made by
// tools/kw_keen4_voice.py). The music is turned down, not off, while a line
// is spoken, and comes back up where it was: KW_LinesOver when the windows
// are put away, or KW_LinePoll while one stays up. Only in the world the
// clips were made for (other games on the same engine have other words).

static bool kw_ducked;

bool KW_SayLine(int world, const char *file)
{
	KW_StopVoice();
	if (KW_CurrentWorld() != world || !KW_LoadClip(file))
		return false;
	if (!kw_ducked && !kw_musicHeld)
	{
		SD_FadeAdLibVolume(0, 300);
		VL_DelayTics(22);
		kw_ducked = true;
	}
	if (getenv("KW_AUTOTALK"))
		printf("KW_AUTOTALK: %s (%.1f s)\n", file, kw_clipSamples / (double)KW_VOICE_RATE);
	fflush(stdout);
	KW_StartClip();
	return true;
}

// A clip by itself, with no change to the music (the caller has seen to
// it): the narrator reading a story while it scrolls.
bool KW_Narrate(const char *file)
{
	KW_StopVoice();
	if (!KW_LoadClip(file))
		return false;
	if (getenv("KW_AUTOTALK"))
		printf("KW_AUTOTALK: %s (%.1f s)\n", file, kw_clipSamples / (double)KW_VOICE_RATE);
	KW_StartClip();
	return true;
}

// The line of the window called `name` in the world Keen is in: the clip
// <prefix>_<name>.RAW. Worlds on one engine share their windows but not
// their words (Keen 7 says other things in Keen 4's windows), so each has
// clips of its own, or none.
bool KW_SayWorldLine(const char *name)
{
	static const struct { int world; const char *prefix; } spoken[] = {
		{KW_KEEN4, "K4"}, {KW_KEEN5, "K5"}, {KW_KEEN6, "K6"}, {KW_KEEN7, "K7"}, {KW_KEEN8, "K8"}, {KW_KEEN9, "K9"},
		{KW_KEEN95, "K95"}, {KW_BOUNTY, "BH"}, {KW_CK55, "K55"}, {KW_DREAM, "KD"}, {KW_AREALM, "AR"}, {KW_FITF, "FF"},
		// (Keen Asylum and Operation Station Infiltration say Keen 5's words where they speak)
		{KW_ASYLUM, "K5"}, {KW_OSI, "K5"}};
	for (size_t i = 0; i < sizeof(spoken) / sizeof(spoken[0]); ++i)
		if (spoken[i].world == KW_CurrentWorld())
		{
			char file[48];
			snprintf(file, sizeof(file), "%s_%s.RAW", spoken[i].prefix, name);
			return KW_SayLine(spoken[i].world, file);
		}
	KW_StopVoice();
	return false;
}

// Several lines one after another as one clip (with a short gap between
// them): those of the names that have a clip in the world Keen is in.
bool KW_SayWorldLines(const char *const *names, int count)
{
	static const struct { int world; const char *prefix; } spoken[] = {{KW_FITF, "FF"}};
	const char *prefix = NULL;
	for (size_t i = 0; i < sizeof(spoken) / sizeof(spoken[0]); ++i)
		if (spoken[i].world == KW_CurrentWorld())
			prefix = spoken[i].prefix;
	KW_StopVoice();
	if (!prefix || SD_GetSoundMode() == sdm_Off)
		return false;
	int16_t *all = NULL;
	long total = 0;
	const long gap = KW_VOICE_RATE / 4;
	for (int n = 0; n < count; ++n)
	{
		char path[600];
		snprintf(path, sizeof(path), "%s/voice/%s_%s.RAW", fs_keenPath, prefix, names[n]);
		FILE *f = fopen(path, "rb");
		if (!f)
			continue;
		fseek(f, 0, SEEK_END);
		long samples = ftell(f) / 2;
		fseek(f, 0, SEEK_SET);
		int16_t *grown = (int16_t *)realloc(all, (total + gap + samples) * sizeof(int16_t));
		if (grown)
		{
			all = grown;
			if (total)
			{
				memset(all + total, 0, gap * sizeof(int16_t));
				total += gap;
			}
			long got = (long)fread(all + total, sizeof(int16_t), samples, f);
			for (long i = 0; i < got; ++i)
				all[total + i] = (int16_t)CK_Cross_SwapLE16((uint16_t)all[total + i]);
			total += got;
			if (getenv("KW_AUTOTALK"))
				printf("KW_AUTOTALK: %s_%s.RAW (%.1f s)\n", prefix, names[n], got / (double)KW_VOICE_RATE);
		}
		fclose(f);
	}
	fflush(stdout);
	if (!total)
	{
		free(all);
		return false;
	}
	SD_StopVoice();
	free(kw_clip);
	kw_clip = all;
	kw_clipSamples = total;
	if (!kw_ducked && !kw_musicHeld)
	{
		SD_FadeAdLibVolume(0, 300);
		VL_DelayTics(22);
		kw_ducked = true;
	}
	KW_StartClip();
	return true;
}

void KW_LinePoll(void)
{
	if (kw_ducked && !KW_VoiceSpeaking())
	{
		kw_ducked = false;
		SD_FadeAdLibVolume(256, 900);
	}
}

void KW_LinesOver(void)
{
	KW_StopVoice();
	KW_LinePoll();
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
	// Play goes on: a line of dialogue still being spoken is cut (its window
	// has been put away), and the music comes back up.
	if (kw_ducked)
		KW_LinesOver();
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
