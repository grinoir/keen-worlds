/*
Keen Worlds: new art placed into the original maps at load time.
Licensed under the GNU GPL v2 or later, like the rest of Omnispeak.
*/

#ifndef KW_ART_H
#define KW_ART_H

#include <stdbool.h>
#include <stdint.h>

#define KW_ART_MAX_TILES 256

typedef struct KW_Art
{
	int originX, originY;   // Top-left map tile.
	int tilesW, tilesH;
	int frames;             // 1 for still art.
	const uint16_t *slots;  // [frames][tilesH][tilesW] masked-tile slots.
	const uint8_t *tiles;   // [frames][tilesH][tilesW][160] mask + 4 planes.
	int walkableRow;        // Row (0-based) Keen can walk through, or -1.
	int animTics;           // Per frame, at 70 tics per second.
	// Optional: cells whose original tile is replaced, not drawn over.
	bool (*dropsUnderlay)(int x, int y);
	// Drop the original tiles' misc flags (e.g. boards over a door, so it
	// can't be entered).
	bool clearMisc;
} KW_Art;

// Place art on the loaded map. Call while the level is being set up (before
// its graphics are cached), e.g. from the info-layer scan.
void KW_PlaceArt(const KW_Art *art);

// For /KWTEST: whether the art is on the map and all its tiles exist.
bool KW_IsArtPlaced(const KW_Art *art);

// For /KWTEST: whether one cell of the art is on the map (other art may
// cover the rest).
bool KW_IsArtCellPlaced(const KW_Art *art, int x, int y);

// Build the art's tiles in memory without putting it on the map (for art
// drawn elsewhere, e.g. a dialogue picture).
void KW_BuildArtTiles(const KW_Art *art);

// Draw built art (first frame) into the back buffer at screen position x, y.
void KW_DrawArt(const KW_Art *art, int x, int y);

#endif
