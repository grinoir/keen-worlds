/* Keen Worlds: Billy's homemade wrist computer. GPL-2.0-or-later. */
#ifndef KW_WATCH_H
#define KW_WATCH_H
#include <stdbool.h>
#include <stdint.h>
void KW_CacheWatchFont(void);
void KW_FreeWatchFont(void);
const uint8_t *KW_DreamWatchFont(void);
void KW_DrawWatch(bool buffered);
void KW_DrawWatchTitle(const char *title, bool buffered);
#endif
