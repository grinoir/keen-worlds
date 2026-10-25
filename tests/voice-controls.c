// Exercise real clip loading and speech entry points with deterministic audio.
#include "kw_music.h"
#include "ck_def.h"
#include "kw_world.h"
#include "id_sd.h"
#include "id_cfg.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *fs_keenPath;
CK_object *ck_keenObj;
int16_t ca_mapOn = -1;
static int enabled = 1, volume = 100, played, fades, waits, mixedVolume, world = KW_FITF;
static uint32_t now = 100;
static bool playing;
bool CFG_GetConfigBool(const char *key, bool fallback) { (void)key; (void)fallback; return enabled; }
int CFG_GetConfigInt(const char *key, int fallback) { (void)key; (void)fallback; return volume; }
void CFG_SetConfigBool(const char *key, bool value) { assert(!strcmp(key, "kw_voices")); enabled = value; }
void CFG_SetConfigInt(const char *key, int value) { assert(!strcmp(key, "kw_voiceVolume")); volume = value; }
uint32_t SD_GetTimeCount(void) { return now; }
void SD_StopVoice(void) { playing = false; }
void SD_SetVoiceVolume(int value) { mixedVolume = value; }
void SD_PlayVoice(const int16_t *data, uint32_t samples, uint32_t rate)
{ assert(data && samples && rate == 22050); ++played; playing = true; }
void SD_FadeAdLibVolume(int target, int ms) { (void)target; (void)ms; ++fades; }
void VL_DelayTics(int tics) { waits += tics; }
void SD_MusicOff(void) {}
void SD_StartMusicData(const uint8_t *data, uint16_t length) { (void)data; (void)length; }
void StartMusic(int16_t level) { (void)level; }
int KW_CurrentWorld(void) { return world; }
bool KW_IsLevelPeaceful(void) { return false; }
int16_t *KW_DecodeVorbis(const uint8_t *data, int length, long *samples, int *rate)
{ (void)data; (void)length; (void)samples; (void)rate; assert(!"Unexpected decoder"); return NULL; }

int main(int argc, char **argv)
{
 assert(argc == 2); fs_keenPath = argv[1];
 const char *const lines[] = {"ONE", "TWO"};
 KW_SetVoiceVolume(40);
 assert(KW_VoiceVolume() == 40);
 // Effects mode has no role in this path; speech plays independently.
 assert(KW_SayLine(world, "TEST.RAW"));
 assert(playing && mixedVolume == 40 && KW_ClipPlaying());
 KW_SetVoicesEnabled(false);
 assert(!playing && !KW_ClipPlaying() && !KW_VoiceSpeaking());
 int beforePlayed = played, beforeFades = fades, beforeWaits = waits;
 KW_VoiceComingUp(true);
 KW_Speak("TEST.RAW", true);
 assert(!KW_Narrate("TEST.RAW"));
 assert(!KW_NarrateOverMusic("TEST.RAW"));
 assert(!KW_SayLine(world, "TEST.RAW"));
 assert(!KW_SayWorldLine("ONE"));
 assert(!KW_SayWorldLines(lines, 2));
 assert(!KW_VoiceSpeaking());
 assert(played == beforePlayed && fades == beforeFades && waits == beforeWaits);
 KW_SetVoicesEnabled(true);
 assert(KW_VoiceVolume() == 40 && !playing);
 assert(KW_SayWorldLines(lines, 2));
 assert(played == beforePlayed + 1 && playing && mixedVolume == 40);
 KW_SetVoiceVolume(0);
 assert(!playing && !KW_VoiceSpeaking());
 beforeFades = fades; beforeWaits = waits;
 assert(!KW_NarrateOverMusic("TEST.RAW"));
 assert(!KW_SayWorldLines(lines, 2));
 assert(fades == beforeFades && waits == beforeWaits);
 KW_SetVoiceVolume(500); assert(KW_VoiceVolume() == 100);
 KW_SetVoiceVolume(-1); assert(KW_VoiceVolume() == 0);
 KW_SetVoiceVolume(60); KW_SetVoicesEnabled(false); KW_SetVoicesEnabled(true);
 assert(KW_VoiceVolume() == 60 && !playing);
 assert(KW_Narrate("TEST.RAW"));
 assert(mixedVolume == 60);
 KW_LinesOver();
 puts("PASS: independent voices, volume, single/combined clips, mute stops speech, no silent waits/fades, retained volume and clamping");
 return 0;
}
