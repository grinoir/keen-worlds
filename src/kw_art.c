/*
Keen Worlds: new art placed into the original maps at load time.

Art is drawn by scripts in the project's tools/ folder and generated into
kw_*_art.h headers. Each art tile is laid over whatever original foreground
tile is beneath it and stored in an empty, unused masked-tile slot, so the
player's EGAGRAPH/GAMEMAPS files stay untouched. Tile properties (collision,
animation) for those slots are set at runtime.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#include <string.h>

#include "id_ca.h"
#include "id_mm.h"
#include "id_rf.h"
#include "id_vh.h"
#include "id_vl.h"
#include "ck_def.h"
#include "kw_art.h"

extern uint8_t *ti_tileInfo;

static uint16_t KW_ArtSlot(const KW_Art *art, int frame, int row, int col)
{
	return art->slots[(frame * art->tilesH + row) * art->tilesW + col];
}

static const uint8_t *KW_ArtTile(const KW_Art *art, int frame, int row, int col)
{
	return art->tiles + ((frame * art->tilesH + row) * art->tilesW + col) * 160;
}

// Build one art tile in a masked-tile slot: the original tile underneath (if
// any), with the art's opaque pixels on top.
static void KW_ComposeArtTile(uint16_t slot, uint16_t under, const uint8_t *art)
{
	int chunk = ca_gfxInfoE.offTiles16m + slot;
	if (ca_graphChunks[chunk])
		MM_FreePtr(&ca_graphChunks[chunk]);
	MM_GetPtr(&ca_graphChunks[chunk], 160);
	// Art exists only in memory (its slot has no data in EGAGRAPH), so it
	// must never be purged when memory blocks run short (e.g. while a
	// dialogue caches its pictures).
	MM_SetLock(&ca_graphChunks[chunk], true);
	uint8_t *dst = (uint8_t *)ca_graphChunks[chunk];

	if (under)
	{
		int underChunk = ca_gfxInfoE.offTiles16m + under;
		CA_CacheGrChunk(underChunk);
		memcpy(dst, ca_graphChunks[underChunk], 160);
	}
	else
	{
		memset(dst, 0xFF, 32); // all transparent
		memset(dst + 32, 0, 128);
	}

	// Plane 0 is the mask (1 = transparent), then four colour planes.
	for (int i = 0; i < 32; ++i)
	{
		uint8_t opaque = (uint8_t)~art[i];
		dst[i] &= (uint8_t)~opaque;
		for (int plane = 1; plane <= 4; ++plane)
			dst[plane * 32 + i] = (uint8_t)((dst[plane * 32 + i] & ~opaque) | (art[plane * 32 + i] & opaque));
	}
}

// Tile properties for an art slot: collision and misc flags (doors, poles,
// drawing in front of sprites) copied from the original tile, none if
// walkable; and an animation link to the next frame.
// Foreground tile info layout (see id_ti.c): 0 top, 1 right, 2 bottom,
// 3 left, 4 animation offset, 5 misc, 6 animation time.
static void KW_SetArtTileInfo(const KW_Art *art, uint16_t slot, uint16_t under, bool walkable, bool solid, uint16_t nextSlot)
{
	int n16m = ca_gfxInfoE.numTiles16m;
	uint8_t *fore = ti_tileInfo + ca_gfxInfoE.numTiles16 * 2;
	for (int k = 0; k < 4; ++k)
		fore[k * n16m + slot] = solid ? 1 : walkable ? 0 : !under ? 0 : fore[k * n16m + under];
	fore[5 * n16m + slot] = (walkable || !under || art->clearMisc) ? 0 : fore[5 * n16m + under];
	bool animated = art->frames > 1;
	fore[4 * n16m + slot] = animated ? (uint8_t)(int8_t)(nextSlot - slot) : 0;
	fore[6 * n16m + slot] = animated ? art->animTics : 0;
}

void KW_PlaceArt(const KW_Art *art)
{
	uint16_t placed[KW_ART_MAX_TILES];
	if (art->tilesW * art->tilesH > KW_ART_MAX_TILES)
		Quit("KW_PlaceArt: art is larger than KW_ART_MAX_TILES");

	for (int r = 0; r < art->tilesH; ++r)
	{
		for (int c = 0; c < art->tilesW; ++c)
		{
			int x = art->originX + c, y = art->originY + r;
			uint16_t under = CA_TileAtPos(x, y, 1);
			if (under == KW_ArtSlot(art, 0, r, c))
				return; // Already placed on this copy of the map.
			if (art->dropsUnderlay && art->dropsUnderlay(x, y))
				under = 0;

			for (int f = 0; f < art->frames; ++f)
			{
				uint16_t slot = KW_ArtSlot(art, f, r, c);
				KW_ComposeArtTile(slot, art->ownPictureOnly ? 0 : under, KW_ArtTile(art, f, r, c));
				bool solid = art->solid ? r != art->walkableRow : (art->isSolidCell && art->isSolidCell(x, y));
				KW_SetArtTileInfo(art, slot, under, r == art->walkableRow, solid,
					KW_ArtSlot(art, (f + 1) % art->frames, r, c));
			}
			placed[r * art->tilesW + c] = KW_ArtSlot(art, 0, r, c);
		}
	}
	// Only the map: art is placed while the level is set up, before its
	// graphics are cached and its animations found, so nothing is drawn yet
	// (RF_ReplaceTiles would redraw at the previous map's scroll position,
	// with tiles not yet cached).
	for (int r = 0; r < art->tilesH; ++r)
		for (int c = 0; c < art->tilesW; ++c)
			CA_SetTileAtPos(art->originX + c, art->originY + r, 1, placed[r * art->tilesW + c]);
}

bool KW_IsArtPlaced(const KW_Art *art)
{
	for (int r = 0; r < art->tilesH; ++r)
		for (int c = 0; c < art->tilesW; ++c)
		{
			if (CA_TileAtPos(art->originX + c, art->originY + r, 1) != KW_ArtSlot(art, 0, r, c))
				return false;
			for (int f = 0; f < art->frames; ++f)
				if (!ca_graphChunks[ca_gfxInfoE.offTiles16m + KW_ArtSlot(art, f, r, c)])
					return false;
		}
	return true;
}

bool KW_IsArtCellPlaced(const KW_Art *art, int x, int y)
{
	int r = y - art->originY, c = x - art->originX;
	if (r < 0 || c < 0 || r >= art->tilesH || c >= art->tilesW)
		return false;
	return CA_TileAtPos(x, y, 1) == KW_ArtSlot(art, 0, r, c) &&
		ca_graphChunks[ca_gfxInfoE.offTiles16m + KW_ArtSlot(art, 0, r, c)];
}

void KW_BuildArtTiles(const KW_Art *art)
{
	for (int f = 0; f < art->frames; ++f)
		for (int r = 0; r < art->tilesH; ++r)
			for (int c = 0; c < art->tilesW; ++c)
				if (!ca_graphChunks[ca_gfxInfoE.offTiles16m + KW_ArtSlot(art, f, r, c)])
					KW_ComposeArtTile(KW_ArtSlot(art, f, r, c), 0, KW_ArtTile(art, f, r, c));
}

void KW_DrawArt(const KW_Art *art, int x, int y)
{
	// Like VHB_DrawTile16M (which draws an 8x8 tile by mistake), into the
	// back buffer at screen position x, y.
	int sx = VL_GetScrollX() & 8, sy = VL_GetScrollY();
	for (int r = 0; r < art->tilesH; ++r)
		for (int c = 0; c < art->tilesW; ++c)
		{
			int px = x + c * 16 + sx, py = y + r * 16 + sy;
			if (VH_MarkUpdateBlock(px, py, px + 15, py + 15))
				VH_DrawTile16M(px, py, KW_ArtSlot(art, 0, r, c));
		}
}
