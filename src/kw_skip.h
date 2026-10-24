/* Keen Worlds: deliberate presentation controls. GPL v2 or later. */
#ifndef KW_SKIP_H
#define KW_SKIP_H

#include <stdbool.h>
#include <stdint.h>

#define KW_SKIP_HOLD_TICS 70

typedef struct KW_Presentation
{
	uint32_t holdStarted, pressSerial, joyButtons;
	bool armed, holding, accepted;
} KW_Presentation;

// Physical key state survives IN_ClearKeysDown; repeated key-down events
// cannot count as fresh presses. Called only by the input event handlers.
void KW_PresentationKey(uint8_t key, bool down);
void KW_ClearPresentationKeys(void);

void KW_BeginPresentation(KW_Presentation *state);
// Poll consumes input. allowAdvance is true only for a silent/finished
// dialogue page; a scene or playing voice always needs a held Escape.
bool KW_PollPresentation(KW_Presentation *state, bool allowAdvance);
void KW_PresentWithSkipHint(const KW_Presentation *state, bool allowAdvance);
void KW_EndPresentation(KW_Presentation *state);
// Story scrolls redraw the footer's original pixels before each frame,
// keeping its text from changing the backdrop's lower EGA planes.
void *KW_SavePresentationFooter(void);
void KW_RestorePresentationFooter(void *background);

// Static pictures/cards: tics == 0 waits indefinitely. Dialogue allows a
// fresh ordinary press after its voice finishes. Returns true if advanced.
bool KW_WaitPresentation(int tics, bool dialogue);

#endif
