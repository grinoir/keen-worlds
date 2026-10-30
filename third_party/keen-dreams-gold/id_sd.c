/* Keen Dreams Gold Edition Source Code
 * Copyright (C) 2026 Szemigi
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * Keen Worlds native companion: the sound manager on SDL2.
 * Copyright (C) 2026 Anders Christensen
 *
 * This is ID_SD.C from Gold (Jason Blochowiak's v1.0d1 with K1n9_Duk3's
 * fixes) with the hardware replaced: timer 0 is driven by the audio
 * callback, the PC speaker is a square wave, the AdLib is DBOPL (from
 * DOSBox, via Omnispeak), and TimeCount follows the wall clock. The Sound
 * Blaster code is gone; Keen Dreams has no digitized sounds.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <SDL.h>
#include "id_heads.h"
#include "opl/dbopl.h"

#undef TimeCount

#define	SDL_SoundFinished()	{SoundNumber = SoundPriority = 0;}

#define PC_PIT_RATE	1193182
#define OUT_RATE	44100

//	Global variables
	boolean		QuietFX,
			LeaveDriveOn,
			SoundBlasterPresent,AdLibPresent,
				NeedsDigitized,NeedsMusic;
	SDMode		SoundMode;
	SMMode		MusicMode;
	word		HackCount;
	word		*SoundTable;	// unused; SoundSegs points into audiosegs
static	byte	**SoundSegs;

//	Internal variables
static	boolean			SD_Started;
static	longword		TimerDivisor = 0x10000;
static	char			*ParmStrings[] = {"noal","nosb","nodr",nil};
static	void			(*SoundUserHook)(void);
static	word			SoundNumber,SoundPriority;
static	long			LocalTime;

//	PC Sound variables
static	byte			pcLastSample,far *pcSound;
static	longword		pcLengthLeft;
static	word			pcSoundLookup[255];
static	word			pcDivisor;		// 0 = speaker off
static	Uint64			pcPhase;	// PIT ticks * OUT_RATE

//	AdLib variables
static	boolean			alNoCheck;
static	byte			far *alSound;
static	word			alBlock;
static	longword		alLengthLeft;
static	longword		alTimeCount;
static	Instrument		alZeroInst;
static	Chip			oplChip;

// This table maps channel numbers to carrier and modulator op cells
static	byte			carriers[9] =  { 3, 4, 5,11,12,13,19,20,21},
						modifiers[9] = { 0, 1, 2, 8, 9,10,16,17,18};

//	Sequencer variables
static	boolean			sqActive;
static	word			alFXReg;
static	word			far *sqHack,far *sqHackPtr,sqHackLen,sqHackSeqLen;
static	long			sqHackTime;

//	Audio device
static	SDL_AudioDeviceID	sdDevice;
static	Uint64			sdServiceFrac;	// PIT ticks * OUT_RATE toward the next ISR

#define LOCK()		do { if (sdDevice) SDL_LockAudioDevice(sdDevice); } while (0)
#define UNLOCK()	do { if (sdDevice) SDL_UnlockAudioDevice(sdDevice); } while (0)

//	Timing (wall clock, 70 Hz)
static	longword		tcValue;
static	Uint64			tcBase;		// performance counter at tcValue's last tick
static	longword		tcStill;	// reads without a change

longword *SD_TimeCountRef(void)
{
	static Uint32 lastpump;
	Uint64 now = SDL_GetPerformanceCounter(), freq = SDL_GetPerformanceFrequency();

	// The DOS keyboard interrupt updated the keys while the game polled
	// TimeCount; many waiting loops read nothing else, so pump here.
	if (SDL_GetTicks() != lastpump)
	{
		lastpump = SDL_GetTicks();
		IN_PumpEvents();
	}
	Uint64 tick = freq / TickBase;
	if (!tcBase)
		tcBase = now;
	if (now - tcBase >= tick)
	{
		Uint64 n = (now - tcBase) / tick;
		tcValue += (longword)n;
		tcBase += n * tick;
		tcStill = 0;
		if (SoundUserHook)
			SoundUserHook();
	}
	else if (++tcStill > 64)
	{
		// The original busy-waits on TimeCount; don't spin a core.
		tcStill = 0;
		SDL_Delay(1);
	}
	return &tcValue;
}

uint32_t SD_GetTimeCount(void)
{
	return *SD_TimeCountRef();
}

void SD_PollDelay(void)
{
	SDL_Delay(1);
}

///////////////////////////////////////////////////////////////////////////
//
//	alOut(n,b) - Puts b in AdLib card register n
//
///////////////////////////////////////////////////////////////////////////
void
alOut(byte n,byte b)
{
	Chip__WriteReg(&oplChip, n, b);
}

//
//	PC Sound code
//

static void
SDL_PCPlaySound(PCSound far *sound)
{
	pcLastSample = -1;
	pcLengthLeft = sound->common.length;
	pcSound = sound->data;
}

static void
SDL_PCStopSound(void)
{
	pcSound = 0;
	pcDivisor = 0;
}

static void
SDL_PCService(void)
{
	byte	s;

	if (pcSound)
	{
		s = *pcSound++;
		if (s != pcLastSample)
		{
			pcLastSample = s;
			pcDivisor = s ? pcSoundLookup[s] : 0;
		}

		if (!(--pcLengthLeft))
		{
			SDL_PCStopSound();
			SDL_SoundFinished();
		}
	}
}

static void
SDL_ShutPC(void)
{
	pcSound = 0;
	pcDivisor = 0;
}

//
// 	AdLib Code
//

static void
SDL_ALStopSound(void)
{
	alSound = 0;
	alOut(alFreqH + 0,0);
}

static void
SDL_AlSetFXInst(Instrument far *inst)
{
	byte		c,m;
	byte		scale;	// added for "quiet AdLib" mode

	m = modifiers[0];
	c = carriers[0];
	alOut(m + alChar,inst->mChar);
	alOut(m + alScale,inst->mScale);
	alOut(m + alAttack,inst->mAttack);
	alOut(m + alSus,inst->mSus);
	alOut(m + alWave,inst->mWave);
	alOut(c + alChar,inst->cChar);
	scale = inst->cScale;
	if (QuietFX)
	{
		scale = 0x3F-scale;
		scale = (scale>>1) + (scale>>2);	// basically 'scale *= 0.75;'
		scale = 0x3F-scale;
	}
	alOut(c + alScale,scale);
	alOut(c + alAttack,inst->cAttack);
	alOut(c + alSus,inst->cSus);
	alOut(c + alWave,inst->cWave);
	alOut(alFeedCon,0);	// K1n9_Duk3 fix: always re-apply default value
}

static void
SDL_ALPlaySound(AdLibSound far *sound)
{
	Instrument	far *inst;

	SDL_ALStopSound();

	alLengthLeft = sound->common.length;
	alSound = sound->data;
	alBlock = ((sound->block & 7) << 2) | 0x20;
	inst = &sound->inst;

	if (!(inst->mSus | inst->cSus))
	{
		UNLOCK();
		Quit("SDL_ALPlaySound() - Bad instrument");
	}

	SDL_AlSetFXInst(inst);
}

static void
SDL_ALSoundService(void)
{
	byte	s;

	if (alSound)
	{
		s = *alSound++;
		if (!s)
			alOut(alFreqH + 0,0);
		else
		{
			alOut(alFreqL + 0,s);
			alOut(alFreqH + 0,alBlock);
		}

		if (!(--alLengthLeft))
		{
			alSound = 0;
			alOut(alFreqH + 0,0);
			SDL_SoundFinished();
		}
	}
}

static void
SDL_ALService(void)
{
	word	w;

	if (!sqActive)
		return;

	while (sqHackLen && (sqHackTime <= alTimeCount))
	{
		w = SDL_SwapLE16(*sqHackPtr++);
		sqHackTime = alTimeCount + SDL_SwapLE16(*sqHackPtr++);
		alOut(w & 0xff,w >> 8);
		sqHackLen -= 4;
	}
	alTimeCount++;
	if (!sqHackLen)
	{
		sqHackPtr = (word far *)sqHack;
		sqHackLen = sqHackSeqLen;
		alTimeCount = sqHackTime = 0;
	}
}

static void
SDL_ShutAL(void)
{
	alOut(alEffects,0);
	alOut(alFreqH + 0,0);
	SDL_AlSetFXInst(&alZeroInst);
	alSound = 0;
}

static void
SDL_CleanAL(void)
{
	int	i;

	alOut(alEffects,0);
	for (i = 1;i < 0xf5;i++)
		alOut(i,0);
}

static void
SDL_StartAL(void)
{
	alFXReg = 0;
	alOut(alEffects,alFXReg);
	SDL_AlSetFXInst(&alZeroInst);
}

///////////////////////////////////////////////////////////////////////////
//
//	SDL_t0Service() - the timer 0 ISR, now called from the audio callback.
//		TimeCount is kept by the wall clock (see SD_TimeCountRef).
//
///////////////////////////////////////////////////////////////////////////
static void
SDL_t0Service(void)
{
static	word	count = 1;

	HackCount++;

	if (MusicMode == smm_AdLib)
	{
		SDL_ALService();
		if (!(++count & 7))
			LocalTime++;
		if (!(count & 3))
		{
			switch (SoundMode)
			{
			case sdm_PC:
				SDL_PCService();
				break;
			case sdm_AdLib:
				SDL_ALSoundService();
				break;
			}
		}
	}
	else
	{
		if (!(++count & 1))
			LocalTime++;
		switch (SoundMode)
		{
		case sdm_PC:
			SDL_PCService();
			break;
		case sdm_AdLib:
			SDL_ALSoundService();
			break;
		}
	}
}

static void
SD_AudioCallback(void *userdata, Uint8 *stream, int len)
{
	Sint16	*out = (Sint16 *)stream;
	int	frames = len / sizeof(Sint16);
	Bit32s	opl[512];

	(void)userdata;
	while (frames > 0)
	{
		// Run until the next timer 0 interrupt, at most one block.
		Uint64	need = (Uint64)TimerDivisor * OUT_RATE;
		Uint64	untilService = sdServiceFrac >= need ? 1
			: (need - sdServiceFrac + PC_PIT_RATE - 1) / PC_PIT_RATE;
		int	n = frames, i;

		if (untilService < 1)
			untilService = 1;
		if ((Uint64)n > untilService)
			n = untilService;
		if (n > 512)
			n = 512;

		Chip__GenerateBlock2(&oplChip, n, opl);
		for (i = 0; i < n; i++)
		{
			Sint32	s = opl[i] * 2;

			if (pcDivisor)
			{
				// The speaker's square wave: pcDivisor PIT ticks per period.
				Uint64 period = (Uint64)pcDivisor * OUT_RATE;
				pcPhase = (pcPhase + PC_PIT_RATE) % period;
				s += (pcPhase < period / 2) ? 3000 : -3000;
			}
			if (s > 32767) s = 32767;
			if (s < -32768) s = -32768;
			*out++ = s;
		}
		frames -= n;

		sdServiceFrac += (Uint64)n * PC_PIT_RATE;
		while (sdServiceFrac >= need)
		{
			sdServiceFrac -= need;
			SDL_t0Service();
		}
	}
}

static void
SDL_SetIntsPerSec(word ints)
{
	LOCK();
	TimerDivisor = PC_PIT_RATE / ints;
	UNLOCK();
}

static void
SDL_ShutDevice(void)
{
	switch (SoundMode)
	{
	case sdm_PC:
		SDL_ShutPC();
		break;
	case sdm_AdLib:
		SDL_ShutAL();
		break;
	}
	SoundMode = sdm_Off;
}

static void
SDL_CleanDevice(void)
{
	if ((SoundMode == sdm_AdLib) || (MusicMode == smm_AdLib))
		SDL_CleanAL();
}

static void
SDL_StartDevice(void)
{
	switch (SoundMode)
	{
	case sdm_AdLib:
		SDL_StartAL();
		break;
	}
	SoundNumber = SoundPriority = 0;
}

static void
SDL_SetTimerSpeed(void)
{
	word	rate;

	if (MusicMode == smm_AdLib)
		rate = TickBase * 8;
	else
		rate = TickBase * 2;
	SDL_SetIntsPerSec(rate);
}

//	Public routines

boolean
SD_SetSoundMode(SDMode mode)
{
	boolean	result = false;
	word	tableoffset = 0;

	SD_StopSound();

	switch (mode)
	{
	case sdm_Off:
		result = true;
		break;
	case sdm_PC:
		tableoffset = STARTPCSOUNDS;
		result = true;
		break;
	case sdm_AdLib:
		if (AdLibPresent)
		{
			tableoffset = STARTADLIBSOUNDS;
			result = true;
		}
		break;
	default:
		result = false;
		break;
	}

	if (result && (mode != SoundMode))
	{
		LOCK();
		SDL_ShutDevice();
		SoundMode = mode;
		SoundSegs = &audiosegs[tableoffset];
		SDL_StartDevice();
		UNLOCK();
	}

	SDL_SetTimerSpeed();

	return(result);
}

boolean
SD_SetMusicMode(SMMode mode)
{
	boolean	result = false;

	SD_FadeOutMusic();

	switch (mode)
	{
	case smm_Off:
		NeedsMusic = false;
		result = true;
		break;
	case smm_AdLib:
		if (AdLibPresent)
		{
			NeedsMusic = true;
			result = true;
		}
		break;
	default:
		result = false;
		break;
	}
	if (result)
		MusicMode = mode;

	SDL_SetTimerSpeed();

	return(result);
}

void
SD_Startup(void)
{
	int	i;
	SDL_AudioSpec	want, have;

	if (SD_Started)
		return;

	alNoCheck = false;
	LeaveDriveOn = false;
	for (i = 1;i < _argc;i++)
	{
		switch (US_CheckParm(_argv[i],ParmStrings))
		{
		case 0:						// No AdLib
			alNoCheck = true;
			break;
		case 2:
			LeaveDriveOn = true;
			break;
		}
	}

	SoundUserHook = 0;
	DBOPL_InitTables();
	Chip__Chip(&oplChip);
	Chip__Setup(&oplChip, OUT_RATE);

	SDL_zero(want);
	want.freq = OUT_RATE;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = 512;
	want.callback = SD_AudioCallback;
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0)
		sdDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

	LocalTime = alTimeCount = 0;
	tcValue = 0;
	tcBase = 0;

	SD_SetSoundMode(sdm_Off);
	SD_SetMusicMode(smm_Off);

	// The emulated AdLib is always there when audio works.
	AdLibPresent = sdDevice && !alNoCheck;
	SoundBlasterPresent = false;
	if (AdLibPresent)
	{
		for (i = 1;i <= 0xf5;i++)	// Zero all the registers
			alOut(i,0);
		alOut(1,0x20);	// Set WSE=1
	}

	for (i = 0;i < 255;i++)
		pcSoundLookup[i] = i * 60;

	if (sdDevice)
		SDL_PauseAudioDevice(sdDevice, 0);

	SD_Started = true;
}

void
SD_Default(boolean gotit,SDMode sd,SMMode sm)
{
	boolean	gotsd,gotsm;

	gotsd = gotsm = gotit;

	if (gotsd)	// Make sure requested sound hardware is available
	{
		switch (sd)
		{
		case sdm_AdLib:
			gotsd = AdLibPresent;
			break;
		case sdm_SoundBlaster:
			gotsd = false;
			break;
		}
	}
	if (!gotsd)
	{
		if (AdLibPresent)
			sd = sdm_AdLib;
		else
			sd = sdm_PC;
	}
	if (sd != SoundMode)
		SD_SetSoundMode(sd);


	if (gotsm)	// Make sure requested music hardware is available
	{
		switch (sm)
		{
		case smm_AdLib:
			gotsm = AdLibPresent;
			break;
		}
	}
	if (!gotsm)
	{
		if (AdLibPresent)
			sm = smm_AdLib;
		else
			sm = smm_Off;
	}
	if (sm != MusicMode)
		SD_SetMusicMode(sm);
}

void
SD_Shutdown(void)
{
	if (!SD_Started)
		return;

	SD_MusicOff();
	LOCK();
	SDL_ShutDevice();
	SDL_CleanDevice();
	UNLOCK();
	if (sdDevice)
	{
		SDL_CloseAudioDevice(sdDevice);
		sdDevice = 0;
	}

	SD_Started = false;
}

void
SD_SetUserHook(void (* hook)(void))
{
	SoundUserHook = hook;
}

void
SD_PlaySound(soundnames sound)
{
	SoundCommon	far *s;

	if ((SoundMode == sdm_Off) || (sound == -1))
		return;

	s = (SoundCommon *)SoundSegs[sound];
	if (!s)
		Quit("SD_PlaySound() - Uncached sound");
	if (!s->length)
		Quit("SD_PlaySound() - Zero length sound");
	if (s->priority < SoundPriority)
		return;

	LOCK();
	switch (SoundMode)
	{
	case sdm_PC:
		SDL_PCPlaySound((void far *)s);
		break;
	case sdm_AdLib:
		SDL_ALPlaySound((void far *)s);
		break;
	}

	SoundNumber = sound;
	SoundPriority = s->priority;
	UNLOCK();
}

word
SD_SoundPlaying(void)
{
	boolean	result = false;

	switch (SoundMode)
	{
	case sdm_PC:
		result = pcSound? true : false;
		break;
	case sdm_AdLib:
		result = alSound? true : false;
		break;
	}

	if (result)
		return(SoundNumber);
	else
		return(false);
}

void
SD_StopSound(void)
{
	LOCK();
	switch (SoundMode)
	{
	case sdm_PC:
		SDL_PCStopSound();
		break;
	case sdm_AdLib:
		SDL_ALStopSound();
		break;
	}

	SDL_SoundFinished();
	UNLOCK();
}

void
SD_WaitSoundDone(void)
{
	while (SD_SoundPlaying())
		SDL_Delay(1);
}

void
SD_MusicOn(void)
{
	sqActive = true;
}

void
SD_MusicOff(void)
{
	word	i;

	LOCK();
	switch (MusicMode)
	{
	case smm_AdLib:
		alFXReg = 0;
		alOut(alEffects,0);
		for (i = 0;i < sqMaxTracks;i++)
			alOut(alFreqH + i + 1,0);
		break;
	}
	sqActive = false;
	UNLOCK();
}

void
SD_StartMusic(MusicGroup far *music)
{
	SD_MusicOff();
	LOCK();
	if (MusicMode == smm_AdLib)
	{
		sqHackPtr = sqHack = music->values;
		sqHackSeqLen = sqHackLen = SDL_SwapLE16(music->length);
		sqHackTime = 0;
		alTimeCount = 0;
		sqActive = true;
	}
	UNLOCK();
}

void
SD_FadeOutMusic(void)
{
	switch (MusicMode)
	{
	case smm_AdLib:
		// DEBUG - quick hack to turn the music off
		SD_MusicOff();
		break;
	}
}

boolean
SD_MusicPlaying(void)
{
	return(false);
}
