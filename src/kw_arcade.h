/* Keen Worlds native arcade bridge. GPL v2 or later. */
#ifndef KW_ARCADE_H
#define KW_ARCADE_H
#include <stdbool.h>
typedef enum KW_ArcadeStatus {
 KW_ARCADE_READY, KW_ARCADE_NO_ENGINE, KW_ARCADE_NO_DATA,
 KW_ARCADE_WRONG_VERSION, KW_ARCADE_FAILED
} KW_ArcadeStatus;
KW_ArcadeStatus KW_ArcadeCheck(int episode);
KW_ArcadeStatus KW_PlayArcade(int episode);
// Keen Dreams Gold Edition (Szemigi, 2026-05-31 release): its own native
// companion, offered at the bed beside the classic dream. Its files are the
// player's, unpacked from KDGOLD.ZIP into mods/dreams-gold.
KW_ArcadeStatus KW_DreamsGoldCheck(void);
KW_ArcadeStatus KW_PlayDreamsGold(void);
#endif
