/*
Keen Worlds: the project card, shown when the game quits (in place of the
original order screens). Edit the text here.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <stdint.h>
#include <string.h>

#include "id_us.h"
#include "kw_card.h"
#include "kw_version.h"

// The card's text. Keep lines under 54 characters so they fit the quit
// screen's box. In the body, an empty string is a blank line.
const char *kw_cardByline = "GriNoir brings you";
const char *kw_cardTitle = "COMMANDER KEEN: WORLDS";
const char *kw_cardBody[] = {
	"One Megarocket. A galaxy that remembers.",
	"",
	"Keen 4, 5 and 6 and the fan games, joined into one",
	"galaxy. Every world is its makers' work.",
	"",
	"Version " KW_VERSION " (" KW_VERSION_NAME ")",
	NULL,
};

// ---- Quit screen (80x25 text mode, like the original order screens) ----

#define KW_COLS 80
#define KW_ROWS 24 // The original skips the last row; DOS overwrote it.

// Text-mode colour attributes: background << 4 | foreground.
#define KW_ATTR(bg, fg) ((uint8_t)(((bg) << 4) | (fg)))
enum
{
	KW_BLACK, KW_BLUE, KW_GREEN, KW_CYAN, KW_RED, KW_MAGENTA, KW_BROWN, KW_LGREY,
	KW_DGREY, KW_LBLUE, KW_LGREEN, KW_LCYAN, KW_LRED, KW_LMAGENTA, KW_YELLOW, KW_WHITE
};

// Codepage 437 box and shade characters.
#define CP_SHADE_MED 0xB1
#define CP_DBL_H 0xCD
#define CP_DBL_V 0xBA
#define CP_DBL_TL 0xC9
#define CP_DBL_TR 0xBB
#define CP_DBL_BL 0xC8
#define CP_DBL_BR 0xBC
#define CP_DBL_LT 0xCC
#define CP_DBL_RT 0xB9

static uint8_t kw_screen[KW_COLS * 25 * 2];

static void KW_Put(int x, int y, uint8_t ch, uint8_t attr)
{
	if (x < 0 || x >= KW_COLS || y < 0 || y >= KW_ROWS)
		return;
	kw_screen[(y * KW_COLS + x) * 2] = ch;
	kw_screen[(y * KW_COLS + x) * 2 + 1] = attr;
}

static void KW_PutCentred(int y, const char *str, uint8_t attr)
{
	int x = (KW_COLS - (int)strlen(str)) / 2;
	for (int i = 0; str[i]; ++i)
		KW_Put(x + i, y, (uint8_t)str[i], attr);
}

void KW_PrintQuitCard(void)
{
	// A starfield backdrop, then a blue double-lined box.
	for (int y = 0; y < KW_ROWS; ++y)
		for (int x = 0; x < KW_COLS; ++x)
		{
			// Scatter stars with an integer hash (fixed, so it never flickers).
			uint32_t h = (uint32_t)x * 374761393u + (uint32_t)y * 668265263u;
			h = (h ^ (h >> 13)) * 1274126177u;
			unsigned hash = (h ^ (h >> 16)) % 97;
			uint8_t ch = (hash == 0) ? '*' : (hash < 3) ? '.' : ' ';
			KW_Put(x, y, ch, KW_ATTR(KW_BLACK, hash == 0 ? KW_WHITE : KW_DGREY));
		}

	const int boxL = 11, boxR = 68, boxT = 5, boxB = 19, ruleY = 9;
	const uint8_t frame = KW_ATTR(KW_BLUE, KW_LCYAN), fill = KW_ATTR(KW_BLUE, KW_WHITE);
	for (int y = boxT; y <= boxB; ++y)
		for (int x = boxL; x <= boxR; ++x)
			KW_Put(x, y, ' ', fill);
	for (int x = boxL + 1; x < boxR; ++x)
	{
		KW_Put(x, boxT, CP_DBL_H, frame);
		KW_Put(x, ruleY, CP_DBL_H, frame);
		KW_Put(x, boxB, CP_DBL_H, frame);
	}
	for (int y = boxT + 1; y < boxB; ++y)
	{
		KW_Put(boxL, y, CP_DBL_V, frame);
		KW_Put(boxR, y, CP_DBL_V, frame);
	}
	KW_Put(boxL, boxT, CP_DBL_TL, frame);
	KW_Put(boxR, boxT, CP_DBL_TR, frame);
	KW_Put(boxL, boxB, CP_DBL_BL, frame);
	KW_Put(boxR, boxB, CP_DBL_BR, frame);
	KW_Put(boxL, ruleY, CP_DBL_LT, frame);
	KW_Put(boxR, ruleY, CP_DBL_RT, frame);
	// Drop shadow.
	for (int y = boxT + 1; y <= boxB + 1; ++y)
		KW_Put(boxR + 1, y, CP_SHADE_MED, KW_ATTR(KW_BLACK, KW_DGREY));
	for (int x = boxL + 1; x <= boxR + 1; ++x)
		KW_Put(x, boxB + 1, CP_SHADE_MED, KW_ATTR(KW_BLACK, KW_DGREY));

	// Heading above the rule, the rest below it.
	KW_PutCentred(boxT + 1, kw_cardByline, KW_ATTR(KW_BLUE, KW_LCYAN));
	KW_PutCentred(boxT + 3, kw_cardTitle, KW_ATTR(KW_BLUE, KW_YELLOW));
	int y = ruleY + 2;
	for (int i = 0; kw_cardBody[i] && y < boxB - 1; ++i, ++y)
		KW_PutCentred(y, kw_cardBody[i], fill);
	KW_PutCentred(boxB - 1, "Thanks for playing!", KW_ATTR(KW_BLUE, KW_LGREEN));

	US_PrintB8000Text(kw_screen, KW_COLS * KW_ROWS);
}
