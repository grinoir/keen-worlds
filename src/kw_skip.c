/* Keen Worlds: deliberate presentation controls. GPL v2 or later. */
#include "kw_skip.h"
#include "kw_music.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_vl_private.h"

#include <string.h>
#include <stdio.h>

static bool kw_physicalKeys[256];
static uint32_t kw_pressSerial, kw_inputSerial;
static int kw_lastController = -1;

void KW_PresentationKey(uint8_t key, bool down)
{
	if (down && !kw_physicalKeys[key])
	{
		++kw_inputSerial;
		kw_lastController = -1;
		if (key != IN_SC_Escape) ++kw_pressSerial;
	}
	kw_physicalKeys[key] = down;
}

void KW_ClearPresentationKeys(void)
{
	memset(kw_physicalKeys, 0, sizeof(kw_physicalKeys));
}

static uint32_t KW_PresentationJoyButtons(void)
{
	uint32_t buttons = 0;
	for (int i = 0; i < 2; ++i)
		if (IN_JoyPresent(i))
			buttons |= (uint32_t)IN_GetJoyButtonsDB(i) << (16 * i);
	return buttons;
}

static bool KW_PresentationKeysDown(void)
{
	for (int i = 0; i < 256; ++i)
		if (kw_physicalKeys[i])
			return true;
	return false;
}

void KW_BeginPresentation(KW_Presentation *state)
{
	IN_PumpEvents();
	memset(state, 0, sizeof(*state));
	state->pressSerial = kw_pressSerial;
	state->inputSerial = kw_inputSerial;
	state->controller = kw_lastController;
	state->holdSource = -2;
	state->joyButtons = KW_PresentationJoyButtons();
	state->armed = !KW_PresentationKeysDown() && !state->joyButtons;
	int menu = IN_GetJoyConf(IN_joy_menu);
	uint32_t skipButtons = menu >= 0 && menu < 16 ?
		((uint32_t)1 << menu) | ((uint32_t)1 << (menu + 16)) : 0;
	state->skipArmed = !kw_physicalKeys[IN_SC_Escape] && !(state->joyButtons & skipButtons);
	IN_ClearKeysDown();
}

bool KW_PollPresentation(KW_Presentation *state, bool allowAdvance)
{
	IN_PumpEvents();
	uint32_t now = SD_GetTimeCount();
	uint32_t buttons = KW_PresentationJoyButtons();
	bool keyPressed = kw_pressSerial != state->pressSerial;
	bool pressed = keyPressed || (buttons & ~state->joyButtons);
	bool input = kw_inputSerial != state->inputSerial;
	uint32_t joyPressed = buttons & ~state->joyButtons;
	if (input) state->controller = kw_lastController = -1;
	else if (joyPressed)
		state->controller = kw_lastController = (joyPressed & 0xffff) ? 0 : 1;
	int menu = IN_GetJoyConf(IN_joy_menu);
	uint32_t skipButtons = menu >= 0 && menu < 16 ?
		((uint32_t)1 << menu) | ((uint32_t)1 << (menu + 16)) : 0;
	// Help is requested by a fresh ordinary key/button, never by entering a
	// page, a narration finishing, Escape, or the controller's skip button.
	if (keyPressed || (joyPressed & ~skipButtons))
	{
		state->hintRequested = true;
		state->hintStarted = now;
	}
	state->allowAdvance = allowAdvance;
	state->inputSerial = kw_inputSerial;
	state->pressSerial = kw_pressSerial;
	state->joyButtons = buttons;
	// Discard stray keys, including auto-repeat, before legacy scene code
	// can interpret them as a skip or open a menu.
	IN_SetLastScan(IN_SC_None);
	if (!state->armed)
	{
		state->armed = !KW_PresentationKeysDown() && !buttons;
	}
	if (!state->skipArmed)
		state->skipArmed = !kw_physicalKeys[IN_SC_Escape] && !(buttons & skipButtons);
	int source = kw_physicalKeys[IN_SC_Escape] ? -1 : -2;
	if (source == -2 && menu >= 0 && menu < 16)
		for (int joy = 0; joy < 2; ++joy)
			if (buttons & ((uint32_t)1 << (menu + joy * 16)))
			{ source = joy; break; }
	if (!state->skipArmed) source = -2;
	if (source != -2)
	{
		if (!state->holding || state->holdSource != source)
		{
			state->holding = true;
			state->holdSource = source;
			state->holdStarted = now;
		}
		if ((uint32_t)(now - state->holdStarted) >= KW_SKIP_HOLD_TICS)
		{
			state->accepted = true;
			return true;
		}
	}
	else
		state->holding = false;
	// Presses during speech are consumed, so they cannot advance the page
	// when the voice ends. Escape always uses the hold, even on a silent page.
	if (state->armed && allowAdvance && pressed && !state->holding)
	{
		state->accepted = true;
		return true;
	}
	return false;
}

// A compact overlay, inset at the upper right. Its original pixels are
// restored before every frame and on exit, including static story pages.
#define HINT_X 140
#define HINT_Y 4
#define HINT_W 176
#define HINT_H 19

bool KW_PresentationHintVisible(const KW_Presentation *state)
{
	return state->holding || (state->hintRequested &&
		(uint32_t)(SD_GetTimeCount() - state->hintStarted) < 2 * 70);
}

void KW_ClearPresentationHint(KW_Presentation *state)
{
	if (!state->hintDrawn) return;
	int x = HINT_X + VL_GetScrollX(), y = HINT_Y + VL_GetScrollY();
	VL_SurfaceToScreen(state->hintBackground, x, y, 0, 0, HINT_W, HINT_H);
	VL_UpdateRect(x, y, HINT_W, HINT_H);
	state->hintDrawn = false;
}

void KW_PresentWithSkipHint(KW_Presentation *state, bool allowAdvance)
{
	KW_ClearPresentationHint(state);
	if (!KW_PresentationHintVisible(state))
	{
		VL_Present();
		return;
	}
	char controllerText[64];
	const char *text = state->accepted ? "Release to continue" :
		allowAdvance && !state->holding ? "Press to continue" : "Hold Esc to skip";
	if (!state->accepted && !(allowAdvance && !state->holding) && state->controller >= 0)
	{
		int menu = IN_GetJoyConf(IN_joy_menu);
		if (menu >= 0 && menu < 16)
		{
			const char *name = IN_GetJoyButtonName(state->controller, menu);
			if (name && *name) snprintf(controllerText, sizeof(controllerText), "Hold %.12s to skip", name);
			else snprintf(controllerText, sizeof(controllerText), "Hold B%d to skip", menu + 1);
			text = controllerText;
		}
	}
	uint16_t w, h;
	VH_MeasurePropString(text, &w, &h, 0);
	if (w > HINT_W - 4)
	{
		if (state->controller >= 0)
		{
			snprintf(controllerText, sizeof(controllerText), "Hold B%d to skip", IN_GetJoyConf(IN_joy_menu) + 1);
			text = controllerText;
		}
		else text = "Hold Esc to skip";
		VH_MeasurePropString(text, &w, &h, 0);
	}
	int x = 316 - w + VL_GetScrollX(), y = HINT_Y + VL_GetScrollY();
	if (!state->hintBackground) state->hintBackground = VL_CreateSurface(HINT_W, HINT_H);
	VL_SurfaceToSurface(vl_emuegavgaadapter.screen, state->hintBackground, 0, 0,
		HINT_X + VL_GetScrollX(), y, HINT_W, HINT_H);
	state->hintDrawn = true;
	int dark = 0, light = 0, luminance[16], chroma[16];
	for (int i = 0; i < 16; ++i)
	{
		const uint8_t *rgb = VL_EGARGBColorTable[vl_emuegavgaadapter.palette[i]];
		luminance[i] = 299 * rgb[0] + 587 * rgb[1] + 114 * rgb[2];
		int hi = rgb[0], lo = rgb[0];
		for (int c = 1; c < 3; ++c)
		{
			if (rgb[c] > hi) hi = rgb[c];
			if (rgb[c] < lo) lo = rgb[c];
		}
		chroma[i] = hi - lo;
		if (luminance[i] < luminance[dark]) dark = i;
		if (luminance[i] > luminance[light]) light = i;
	}
	int ink = light;
	int threshold = luminance[dark] + (luminance[light] - luminance[dark]) * 3 / 5;
	for (int i = 0; i < 16; ++i)
		if (luminance[i] >= threshold &&
			(chroma[i] < chroma[ink] || (chroma[i] == chroma[ink] && luminance[i] < luminance[ink])))
			ink = i;
	VL_ScreenRect(x - 2, y, w + 4, h + 2, dark);
	VH_DrawPropString(text, x, y + 1, 0, dark ^ ink);
	if (state->holding)
	{
		uint32_t held = SD_GetTimeCount() - state->holdStarted;
		if (held > KW_SKIP_HOLD_TICS) held = KW_SKIP_HOLD_TICS;
		VL_ScreenRect(x, y + h + 3, w * held / KW_SKIP_HOLD_TICS, 1, light);
	}
	VL_UpdateRect(HINT_X + VL_GetScrollX(), y, HINT_W, HINT_H);
	VL_Present();
}

static void KW_FinishPresentation(KW_Presentation *state, bool showHint)
{
	if (state->accepted)
		while (KW_PresentationKeysDown() || KW_PresentationJoyButtons())
		{
			IN_PumpEvents();
			if (showHint) KW_PresentWithSkipHint(state, false);
			else VL_Present();
		}
	KW_ClearPresentationHint(state);
	if (state->hintBackground) VL_DestroySurface(state->hintBackground);
	state->hintBackground = NULL;
	IN_ClearKeysDown();
}

void KW_EndPresentation(KW_Presentation *state)
{
	KW_FinishPresentation(state, true);
}

bool KW_DebugWait(void);

bool KW_WaitDialogue(int tics)
{
	if (KW_DebugWait()) return true;
	KW_Presentation state;
	KW_BeginPresentation(&state);
	uint32_t started=SD_GetTimeCount();
	do
	{
		KW_LinePoll();
		IN_PumpEvents();
		uint32_t buttons=KW_PresentationJoyButtons();
		bool pressed=kw_inputSerial!=state.inputSerial || (buttons & ~state.joyButtons);
		state.inputSerial=kw_inputSerial;
		state.joyButtons=buttons;
		IN_SetLastScan(IN_SC_None);
		if (!state.armed)
			state.armed=!KW_PresentationKeysDown() && !buttons;
		else if (pressed)
		{
			state.accepted=true;
			if (KW_ClipPlaying()) KW_StopVoice();
			break;
		}
		VL_Present();
	} while (!tics || (uint32_t)(SD_GetTimeCount()-started)<(uint32_t)tics || KW_ClipPlaying());
	KW_FinishPresentation(&state,false);
	VL_Present();
	return state.accepted;
}

static bool kw_lastPresentationSkipped;
bool KW_LastPresentationWasSkipped(void) { return kw_lastPresentationSkipped; }

bool KW_WaitPresentation(int tics, bool allowPageAdvance)
{
	kw_lastPresentationSkipped=false;
	if (KW_DebugWait())
		return true;
	KW_Presentation state;
	KW_BeginPresentation(&state);
	uint32_t started = SD_GetTimeCount();
	do
	{
		KW_LinePoll();
		bool advance = allowPageAdvance && !KW_ClipPlaying();
		if (KW_PollPresentation(&state, advance))
		{
			if (state.holding)
				KW_StopVoice();
			break;
		}
		KW_PresentWithSkipHint(&state, advance);
	} while (!tics || (uint32_t)(SD_GetTimeCount() - started) < (uint32_t)tics ||
		(allowPageAdvance && KW_ClipPlaying()));
	kw_lastPresentationSkipped=state.accepted && state.holding;
	KW_EndPresentation(&state);
	VL_Present();
	return state.accepted;
}
