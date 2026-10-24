// The actual input handlers and skip policy, with a controlled clock/joystick.
#include "id_in.h"
#include "kw_skip.h"
#include "id_vl_private.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern IN_Backend *in_backend;
static uint32_t now;
static uint16_t joyButtons;
static int script, stopped, restored;
static uint32_t voiceEnds;
static void Pump(void)
{
	if (!script) return;
	++now;
	if (script == 2)
	{
		if (now == 20 || now == 110) IN_HandleKeyDown(IN_SC_Enter, false);
		if (now == 21 || now == 111) IN_HandleKeyUp(IN_SC_Enter, false);
	}
	if (script == 3)
	{
		if (now == 20) IN_HandleKeyDown(IN_SC_Escape, false);
		if (now == 100) IN_HandleKeyUp(IN_SC_Escape, false);
	}
}
static bool JoyPresent(int joystick) { return joystick == 0; }
static uint16_t JoyButtons(int joystick) { (void)joystick; return joyButtons; }
uint32_t SD_GetTimeCount(void) { return now; }
void VL_ToggleFullscreen(void) {}

// Static-wait integration: speech, input, timeout and release are real;
// graphics are inert because this test has no video device.
VL_EGAVGAAdapter vl_emuegavgaadapter;
uint8_t VL_EGARGBColorTable[16][3];
bool KW_DebugWait(void) { return false; }
bool KW_ClipPlaying(void) { return now < voiceEnds; }
void KW_LinePoll(void) {}
void KW_StopVoice(void) { ++stopped; voiceEnds = 0; }
int VL_GetScrollX(void) { return 0; }
int VL_GetScrollY(void) { return 0; }
void VL_Present(void) {}
void *VL_CreateSurface(int w, int h) { (void)w; (void)h; return malloc(1); }
void VL_DestroySurface(void *surface) { free(surface); }
void VL_UpdateRect(int x, int y, int w, int h) { (void)x; (void)y; (void)w; (void)h; }
void VL_ScreenRect(int x, int y, int w, int h, int c)
{ (void)x; (void)y; (void)w; (void)h; (void)c; }
void VL_SurfaceToSurface(void *src, void *dst, int x, int y, int sx, int sy, int w, int h)
{ (void)src; (void)dst; (void)x; (void)y; (void)sx; (void)sy; (void)w; (void)h; }
void VL_SurfaceToScreen(void *src, int x, int y, int sx, int sy, int w, int h)
{ (void)src; (void)x; (void)y; (void)sx; (void)sy; (void)w; (void)h; ++restored; }
void VH_MeasurePropString(const char *s, uint16_t *w, uint16_t *h, int16_t f)
{ (void)s; (void)f; *w = 120; *h = 8; }
void VH_DrawPropString(const char *s, int x, int y, int f, int c)
{ (void)s; (void)x; (void)y; (void)f; (void)c; }

static void Down(IN_ScanCode key) { IN_HandleKeyDown(key, false); }
static void Up(IN_ScanCode key) { IN_HandleKeyUp(key, false); }
static void Begin(KW_Presentation *p)
{
	KW_ClearPresentationKeys();
	joyButtons = 0;
	now = 0;
	KW_BeginPresentation(p);
}

int main(void)
{
	static IN_Backend backend;
	backend.pumpEvents = Pump;
	backend.joyPresent = JoyPresent;
	backend.joyGetButtons = JoyButtons;
	in_backend = &backend;
	KW_Presentation p;
	const IN_ScanCode gameplay[] = {IN_SC_UpArrow, IN_SC_DownArrow,
		IN_SC_LeftArrow, IN_SC_RightArrow, IN_SC_Control, IN_SC_Alt, IN_SC_Space};
	for (unsigned i = 0; i < sizeof(gameplay) / sizeof(gameplay[0]); ++i)
	{
		Begin(&p);
		Down(gameplay[i]);
		assert(!KW_PollPresentation(&p, false));
		now = 200;
		Down(gameplay[i]); // auto-repeat after legacy code cleared input
		IN_ClearKeysDown();
		assert(!KW_PollPresentation(&p, false));
	}
	Begin(&p);
	Down(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	now = 69;
	assert(!KW_PollPresentation(&p, false));
	Up(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	now = 100;
	Down(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	now = 170;
	assert(KW_PollPresentation(&p, false));
	// Still holding Escape cannot skip the next scene, even after clearing.
	IN_ClearKeysDown();
	KW_BeginPresentation(&p);
	now = 1000;
	Down(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	Up(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	Down(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	now += 70;
	assert(KW_PollPresentation(&p, false));
	// A press during speech must be released and pressed again afterwards.
	Begin(&p);
	Down(IN_SC_Enter);
	assert(!KW_PollPresentation(&p, false));
	IN_ClearKeysDown();
	Down(IN_SC_Enter);
	assert(!KW_PollPresentation(&p, true));
	Up(IN_SC_Enter);
	assert(!KW_PollPresentation(&p, true));
	Down(IN_SC_Enter);
	Up(IN_SC_Enter); // a quick tap between polls still counts
	assert(KW_PollPresentation(&p, true));
	// Escape needs the full hold on silent pages too.
	Begin(&p);
	Down(IN_SC_Escape);
	Up(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, true));
	// A gameplay button already down at scene entry must first be released.
	Begin(&p);
	Down(IN_SC_Space);
	IN_ClearKeysDown();
	KW_BeginPresentation(&p);
	Down(IN_SC_Space);
	assert(!KW_PollPresentation(&p, true));
	Up(IN_SC_Space);
	assert(!KW_PollPresentation(&p, true));
	Down(IN_SC_Space);
	assert(KW_PollPresentation(&p, true));
	// Controller presses during narration are also consumed.
	Begin(&p);
	joyButtons = 1;
	assert(!KW_PollPresentation(&p, false));
	assert(!KW_PollPresentation(&p, true));
	joyButtons = 0;
	assert(!KW_PollPresentation(&p, true));
	joyButtons = 1;
	assert(KW_PollPresentation(&p, true));
	KW_BeginPresentation(&p);
	assert(!KW_PollPresentation(&p, true));
	// Focus loss cancels a partial hold instead of leaving a stuck Escape.
	Begin(&p);
	Down(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	KW_ClearPresentationKeys();
	now = 1000;
	assert(!KW_PollPresentation(&p, false));
	assert(!p.holding);
	// Elapsed holds still work across the unsigned clock's wraparound.
	Begin(&p);
	now = UINT32_MAX - 35;
	Down(IN_SC_Escape);
	assert(!KW_PollPresentation(&p, false));
	now += 70;
	assert(KW_PollPresentation(&p, false));
	// Ordinary menu input remains available outside presentation polling.
	Begin(&p);
	Down(IN_SC_Space);
	assert(IN_GetLastScan() == IN_SC_Space);
	// Timed dialogue waits for speech that outlasts its nominal display time.
	Begin(&p);
	voiceEnds = 100;
	script = 1;
	assert(!KW_WaitPresentation(8, true));
	assert(now >= 100 && stopped == 0 && restored == 1);
	script = 0;
	Begin(&p);
	voiceEnds = 100;
	script = 2;
	assert(KW_WaitPresentation(0, true));
	assert(now == 111 && stopped == 0 && restored == 2);
	script = 0;
	Begin(&p);
	voiceEnds = 1000;
	script = 3;
	assert(KW_WaitPresentation(0, true));
	assert(now == 100 && stopped == 1 && restored == 3);
	puts("PASS: deliberate skip, fresh presses, repeat/held-key guards, voice gating, joystick, focus loss and clock wrap");
	puts("PASS: timed narration finishes; early key is discarded; held Escape stops speech and consumes its release; footer is restored");
	return 0;
}
