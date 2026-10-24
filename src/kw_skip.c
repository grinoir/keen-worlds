/* Keen Worlds: deliberate presentation controls. GPL v2 or later. */
#include "kw_skip.h"
#include "kw_music.h"
#include "id_in.h"
#include "id_sd.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_vl_private.h"

#include <string.h>

static bool kw_physicalKeys[256];
static uint32_t kw_pressSerial;

void KW_PresentationKey(uint8_t key, bool down)
{
	if (down && !kw_physicalKeys[key] && key != IN_SC_Escape)
		++kw_pressSerial;
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
	state->joyButtons = KW_PresentationJoyButtons();
	state->armed = !KW_PresentationKeysDown() && !state->joyButtons;
	IN_ClearKeysDown();
}

bool KW_PollPresentation(KW_Presentation *state, bool allowAdvance)
{
	IN_PumpEvents();
	uint32_t now = SD_GetTimeCount();
	uint32_t buttons = KW_PresentationJoyButtons();
	bool pressed = kw_pressSerial != state->pressSerial || (buttons & ~state->joyButtons);
	state->pressSerial = kw_pressSerial;
	state->joyButtons = buttons;
	// Discard stray keys, including auto-repeat, before legacy scene code
	// can interpret them as a skip or open a menu.
	IN_SetLastScan(IN_SC_None);
	if (!state->armed)
	{
		state->armed = !KW_PresentationKeysDown() && !buttons;
		return false;
	}
	if (kw_physicalKeys[IN_SC_Escape])
	{
		if (!state->holding)
		{
			state->holding = true;
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
	if (allowAdvance && pressed && !state->holding)
	{
		state->accepted = true;
		return true;
	}
	return false;
}

// A small footer, drawn directly so it also works over the story scroll's
// unusual plane mask. Static callers save and restore the covered pixels.
void KW_PresentWithSkipHint(const KW_Presentation *state, bool allowAdvance)
{
	const char *text = state->accepted ? "Release to continue" :
		allowAdvance && !state->holding ? "Press a key to continue" : "Hold Esc to skip";
	uint16_t w, h;
	VH_MeasurePropString(text, &w, &h, 0);
	int x = (320 - w) / 2 + VL_GetScrollX();
	int y = 181 + VL_GetScrollY();
	// Worlds can remap every palette entry; choose contrasting colours in
	// the palette actually on screen, rather than assuming 0/15 are black/white.
	int dark = 0, light = 0, luminance[16];
	for (int i = 0; i < 16; ++i)
	{
		const uint8_t *rgb = VL_EGARGBColorTable[vl_emuegavgaadapter.palette[i]];
		luminance[i] = 299 * rgb[0] + 587 * rgb[1] + 114 * rgb[2];
		if (luminance[i] < luminance[dark]) dark = i;
		if (luminance[i] > luminance[light]) light = i;
	}
	VL_ScreenRect(72 + VL_GetScrollX(), y, 176, 19, dark);
	// The proportional font renderer XORs its colour with the background.
	VH_DrawPropString(text, x, y + 1, 0, dark ^ light);
	VL_ScreenRect(96 + VL_GetScrollX(), y + 15, 128, 2, light);
	VL_ScreenRect(96 + VL_GetScrollX(), y + 15, 128, 1, dark);
	if (state->holding)
	{
		uint32_t held = SD_GetTimeCount() - state->holdStarted;
		if (held > KW_SKIP_HOLD_TICS)
			held = KW_SKIP_HOLD_TICS;
		VL_ScreenRect(96 + VL_GetScrollX(), y + 15, 128 * held / KW_SKIP_HOLD_TICS, 2, light);
	}
	VL_UpdateRect(72 + VL_GetScrollX(), y, 176, 19);
	VL_Present();
}

void KW_EndPresentation(KW_Presentation *state)
{
	// Consume the release too, so a skip cannot dismiss the next card or
	// become Escape in the play loop's menu.
	if (state->accepted)
		while (KW_PresentationKeysDown() || KW_PresentationJoyButtons())
		{
			IN_PumpEvents();
			KW_PresentWithSkipHint(state, false);
		}
	IN_ClearKeysDown();
}

void *KW_SavePresentationFooter(void)
{
	void *background = VL_CreateSurface(176, 19);
	VL_SurfaceToSurface(vl_emuegavgaadapter.screen, background, 0, 0,
		72 + VL_GetScrollX(), 181 + VL_GetScrollY(), 176, 19);
	return background;
}

void KW_RestorePresentationFooter(void *background)
{
	int x = 72 + VL_GetScrollX(), y = 181 + VL_GetScrollY();
	VL_SurfaceToScreen(background, x, y, 0, 0, 176, 19);
	VL_UpdateRect(x, y, 176, 19);
}

bool KW_DebugWait(void);

bool KW_WaitPresentation(int tics, bool dialogue)
{
	if (KW_DebugWait())
		return true;
	KW_Presentation state;
	KW_BeginPresentation(&state);
	uint32_t started = SD_GetTimeCount();
	// Preserve the page/scene under the footer, including when it times out.
	void *background = KW_SavePresentationFooter();
	do
	{
		KW_LinePoll();
		bool advance = dialogue && !KW_ClipPlaying();
		if (KW_PollPresentation(&state, advance))
		{
			if (state.holding)
				KW_StopVoice();
			break;
		}
		KW_PresentWithSkipHint(&state, advance);
	} while (!tics || (uint32_t)(SD_GetTimeCount() - started) < (uint32_t)tics ||
		(dialogue && KW_ClipPlaying()));
	KW_EndPresentation(&state);
	KW_RestorePresentationFooter(background);
	VL_DestroySurface(background);
	VL_Present();
	return state.accepted;
}
