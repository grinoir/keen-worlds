// Exercise the real SDL output paths and voice mixer without an audio device.
// Queue interception lets us check the samples Windows sends to SDL.
#include <SDL.h>
#include <cassert>
#include <cstdio>
#include <cstring>

static int lockDepth;
static int16_t captured[4096];
static unsigned capturedBytes;

static void TestLock() { assert(lockDepth++ == 0); }
static void TestUnlock() { assert(--lockDepth == 0); }
static int TestQueue(SDL_AudioDeviceID, const void *data, Uint32 size)
{
	assert(lockDepth == 1);
	assert(capturedBytes + size <= sizeof(captured));
	memcpy((Uint8 *)captured + capturedBytes, data, size);
	capturedBytes += size;
	return 0;
}
static Uint32 TestQueueSize(SDL_AudioDeviceID) { return 0; }

#define SDL_LockAudio TestLock
#define SDL_UnlockAudio TestUnlock
#define SDL_QueueAudio TestQueue
#define SDL_GetQueuedAudioSize TestQueueSize
#define SDL_t0Service TestTimerService
#include "../src/id_sd_sdl.c"
#undef SDL_t0Service

extern SD_Backend *sd_backend;

// Run exactly one timer iteration; no game state or waiting is needed.
void TestTimerService() { SD_SDL_useTimerFallback = false; }

static void CheckPath(int path, int volume)
{
	static int16_t voice[64];
	for (int i = 0; i < 64; ++i)
		voice[i] = (i & 1) ? -1000 : 1000;
	capturedBytes = 0;
	memset(captured, 0, sizeof(captured));
	SD_ALOut_SamplesStart = SD_ALOut_SamplesEnd = 0;
	SD_SDL_SampleOffsetInSound = 0;
	SD_SDL_ScaledSamplesPartNum = 0;
	SD_SDL_SamplesInCurrentPart = 128;
	SD_SDL_ScaledSamplesPerPartsTimesPITRate = (uint64_t)128 * PC_PIT_RATE;
	sd_sdl_bonusSamplesQueued = 0;
	sd_sdl_queueAudioMinBufSize = path == 2 ? 128 : 0;
	SD_SDL_useTimerFallback = true;
	SD_SetVoiceVolume(volume);
	SD_PlayVoice(voice, 64, SD_SDL_AudioSpec.freq);
	SD_FadeAdLibVolume(0, 0);
	if (path == 0)
	{
		TestLock();
		SD_SDL_CallBack(NULL, (Uint8 *)captured, 128 * numChannels * sizeof(int16_t));
		TestUnlock();
		capturedBytes = 128 * numChannels * sizeof(int16_t);
	}
	else if (path == 3)
	{
		sd_oplDelaySamples = 128;
		TestLock();
		SD_SDL_alOut(0, 0);
		TestUnlock();
	}
	else
	{
		SD_SDL_nextTickAt = 0;
		SD_SDL_t0InterruptThread(NULL);
	}
	assert(capturedBytes >= 128 * numChannels * sizeof(int16_t));
	for (unsigned i = 0; i < capturedBytes / sizeof(int16_t); ++i)
	{
		unsigned frame = i / numChannels;
		int expected = frame < 64 ? ((frame & 1) ? -750 : 750) * volume / 100 : 0;
		assert(captured[i] == expected);
	}
	assert(!SD_VoicePlaying());
	assert(lockDepth == 0);
}

int main()
{
	static SD_Backend backend = {};
	backend.lock = TestLock;
	backend.unlock = TestUnlock;
	sd_backend = &backend;
	SD_SDL_AudioSubsystem_Up = true;
	SD_SDL_AudioSpec.freq = 22050;
	sd_sdl_queueAudio = true;
	for (int channels = 1; channels <= 2; ++channels)
	{
		numChannels = channels;
		for (int emulator = SD_OPL_EMULATOR_DBOPL; emulator <= SD_OPL_EMULATOR_NUKED; ++emulator)
		{
			sd_oplEmulator = (SD_OPLEmulator)emulator;
			YM3812Init(1, 3579545, SD_SDL_AudioSpec.freq);
			for (int path = 0; path < 4; ++path)
				for (int volume = 0; volume <= 100; volume += 25)
					CheckPath(path, volume);
		}
	}
	puts("PASS: callback, queued timer, queue top-up and register-delay audio; mono/stereo, both OPL emulators");
	return 0;
}
