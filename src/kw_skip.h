/* Keen Worlds: deliberate presentation controls. GPL v2 or later. */
#ifndef KW_SKIP_H
#define KW_SKIP_H

#include <stdbool.h>
#include <stdint.h>

#define KW_SKIP_HOLD_TICS 70

typedef struct KW_Presentation
{
	uint32_t holdStarted, pressSerial, inputSerial, joyButtons, hintStarted;
	int controller, holdSource;
	bool armed, skipArmed, holding, accepted, allowAdvance, hintDrawn, hintRequested;
	void *hintBackground;
} KW_Presentation;

// Physical key state survives IN_ClearKeysDown; repeated key-down events
// cannot count as fresh presses. Called only by the input event handlers.
void KW_PresentationKey(uint8_t key, bool down);
void KW_ClearPresentationKeys(void);

void KW_BeginPresentation(KW_Presentation *state);
// Poll consumes input. allowAdvance is true only for a silent/finished
// story page; a scene or playing narrator always needs a held Escape.
bool KW_PollPresentation(KW_Presentation *state, bool allowAdvance);
void KW_PresentWithSkipHint(KW_Presentation *state, bool allowAdvance);
// Call before redrawing an animated scene, so its next frame never contains
// pixels from the previous hint. EndPresentation restores and frees it too.
void KW_ClearPresentationHint(KW_Presentation *state);
// Hidden while idle. A skip hold shows progress immediately; another fresh
// key/button requests two seconds of help.
bool KW_PresentationHintVisible(const KW_Presentation *state);
void KW_EndPresentation(KW_Presentation *state);
// Static story pages: tics == 0 waits indefinitely. allowPageAdvance allows
// an ordinary fresh press after narration; Escape retains the story hold.
bool KW_WaitPresentation(int tics, bool allowPageAdvance);
// Distinguishes a held story-skip from an ordinary next-page press.
bool KW_LastPresentationWasSkipped(void);
// Popup dialogue: a fresh key/controller press advances immediately,
// including Escape. No hold gesture or story skip hint is displayed.
bool KW_WaitDialogue(int tics);

#endif
