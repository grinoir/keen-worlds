"""One-time hand fixes on the converted CK_KEEN2.C (kw_ff_map.c)."""
import re, sys
p = sys.argv[1]; t = open(p).read()
def rep(a, b):
    global t
    if a not in t: print('no match:', a[:60])
    t = t.replace(a, b)
rep('#include "kw_fitf.h"\n', '''/*
Keen Worlds: Foray in the Forest's map Keen and flags, converted from
CK_KEEN2.C of its source (Nisaba and K1n9_Duk3; GPL v2 or later) by
tools/kw_fitf_port.py, with the hand fixes of tools/kw_fitf_map_fixes.py.
*/

#include "kw_fitf.h"

void FF_T_KeenWorldWalk(CK_object *ob);
#define max(a, b) ((a) > (b) ? (a) : (b))
''')
rep("""		px += ((py) * CA_GetMapWidth());
		mapsegs[1][px++] = 0;
		mapsegs[1][px] = 0;""", """		CA_SetTileAtPos(px, py, 1, 0);
		CA_SetTileAtPos(px + 1, py, 1, 0);""")
t = t.replace("""		px += mapbwidthtable[py] / 2;
		mapsegs[1][px++] = 0;
		mapsegs[1][px] = 0;""", """		CA_SetTileAtPos(px, py, 1, 0);
		CA_SetTileAtPos(px + 1, py, 1, 0);""")
rep("dir_South", "IN_dir_South")
rep("0xC000+GAMELEVELS", "0xC000+FF_GAMELEVELS")
rep("	tile = *((uint16_t _seg *)mapsegs[1] + mapbwidthtable[midTileY]/2 + midTileX);\n	info = tinf[INTILE+tile];", "	tile = CA_TileAtPos(midTileX, midTileY, 1);\n	info = TI_ForeMisc(tile);")
rep("if (!screenfaded)", "if (!vl_screenFaded)")
# KEEN WORLDS: its game loop reads ck_mapState.nextLevel when a level ends
# on the map; the engine's reads the game state's current level.
rep("""			ff_map.nextLevel = MAP_BWBMEGAROCKET;
			ck_gameState.levelState = ex_landed;""", """			ff_map.nextLevel = MAP_BWBMEGAROCKET;
			ck_gameState.currentLevel = MAP_BWBMEGAROCKET; // KEEN WORLDS: the engine's "next level"
			ck_gameState.levelState = LS_LevelComplete; // its ex_landed""")
rep("""				ff_map.nextLevel = infotile - 0xC000;
				ck_gameState.levelState = ex_completed;""", """				ff_map.nextLevel = infotile - 0xC000;
				ck_gameState.currentLevel = infotile - 0xC000; // KEEN WORLDS: the engine's "next level"
				ck_gameState.levelState = LS_LevelComplete;""")
rep("G_T_SHIFT", "8")
open(p, 'w').write(t)

# ---- Keen Worlds: arriving by Megarocket from another world ----
t = open(p).read()
def rep2(a, b):
    global t
    if a not in t: print('no match:', a[:60])
    t = t.replace(a, b)
# The fly-in belongs to a game begun here. Keen arriving from another world
# lands in the BwB level directly (it is then marked done, below); leaving it
# for the map he stands where the ship landed.
rep2("	if (ck_gameState.mapPosX == 0 && info != 0)\n", "	if (ck_gameState.mapPosX == 0 && info != 0 && !ck_gameState.levelsDone[MAP_BWBMEGAROCKET])\n")
rep2("""		if (ck_gameState.mapPosX == 0)
		{
			ck_keenObj->posX = RF_TileToUnit(tileX);
			ck_keenObj->posY = RF_TileToUnit(tileY);
		}""", """		if (ck_gameState.mapPosX == 0 && info != 0)
		{
			// KEEN WORLDS: no fly-in was played: beside the landed ship
			ck_keenObj->posX = RF_TileToUnit(info >> 8);
			ck_keenObj->posY = RF_TileToUnit((info & 0xFF) + 1);
		}
		else if (ck_gameState.mapPosX == 0)
		{
			ck_keenObj->posX = RF_TileToUnit(tileX);
			ck_keenObj->posY = RF_TileToUnit(tileY);
		}""")
open(p, 'w').write(t)
