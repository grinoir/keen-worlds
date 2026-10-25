// Run the real scorebox updater against Dreams' packaged layout. Tile bytes
// identify which digit was drawn, while canaries guard artwork and borders.
#include "ck_def.h"
#include "ck_act.h"
#include "id_rf.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CK_GameState ck_gameState;
bool ck_inHighScores, ck_scoreBoxEnabled = true;
int rf_scrollXUnit, rf_scrollYUnit;
ca_gfxinfo ca_gfxInfoE;
static VH_ShiftedSprite *sprite;
static uint8_t tiles[64 * 32];
static int ammo, icon = 28;
static struct { char key[64]; int value; } settings[16];
static int settingCount;
intptr_t CK_VAR_GetInt(const char *key, intptr_t fallback)
{
 if (!strcmp(key, "SPR_SCOREBOX")) return 100;
 for (int i = 0; i < settingCount; ++i)
  if (!strcmp(key, settings[i].key)) return settings[i].value;
 return fallback;
}
bool KD_IsActive(void) { return true; }
int KD_ScoreBoxShots(void) { return ammo; }
int KD_ScoreBoxIcon(void) { return icon; }
IN_DemoMode IN_DemoGetMode(void) { return IN_Demo_Off; }
VH_SpriteTableEntry VH_GetSpriteTableEntry(int n)
{ (void)n; VH_SpriteTableEntry box = {0}; box.width = 8; box.height = 32; box.shifts = 1; return box; }
mm_ptr_t CA_GetGrChunk(int base, int index, const char *type, bool required)
{ (void)index; (void)type; (void)required; return base == 100 ? (void *)sprite : (void *)tiles; }
void CAL_ShiftSprite(uint8_t *src, uint8_t *dst, int w, int h, int shift)
{ (void)src; (void)dst; (void)w; (void)h; (void)shift; assert(!"unexpected shift"); }
void RF_AddSpriteDraw(RF_SpriteDrawEntry **e, int x, int y, int chunk, bool white, int z)
{ (void)e; (void)x; (void)y; (void)chunk; (void)white; (void)z; }
void CK_UpdateScoreBox(CK_object *scorebox);

static uint8_t PixelByte(int plane, int row, int col)
{ return sprite->data[256 + plane * 256 + row * 8 + col]; }
static void CheckArtwork(void)
{
 for (int i = 0; i < 256; ++i) assert(sprite->data[i] == 0xa5); // mask
 for (int plane = 0; plane < 4; ++plane)
  for (int y = 0; y < 32; ++y)
   for (int x = 0; x < 8; ++x)
   {
    bool field = (y >= 4 && y < 12 && x >= 2) ||
     (y >= 16 && x >= 4 && x <= 5) ||
     (y >= 20 && y < 28 && (x == 2 || x >= 6));
    if (!field) assert(PixelByte(plane, y, x) == 0xa5);
   }
 for (int i = 1280; i < 1312; ++i) assert(sprite->data[i] == 0xa5);
}

int main(void)
{
 FILE *f = fopen("data/mods/dreams/DREAMS.KDR", "r"); assert(f);
 char line[256];
 while (fgets(line, sizeof(line), f))
 {
  char key[64]; int value;
  if (sscanf(line, "%%int %63s %d", key, &value) == 2 && !strncmp(key, "ck_scoreBox", 11))
  { assert(settingCount < 16); strcpy(settings[settingCount].key, key); settings[settingCount++].value = value; }
 }
 fclose(f);
 assert(settingCount == 7);
 sprite = malloc(sizeof(*sprite) + 1312); assert(sprite);
 memset(sprite, 0, sizeof(*sprite)); memset(sprite->data, 0xa5, 1312);
 sprite->sprShiftByteWidths[0] = 8;
 for (int tile = 0; tile < 64; ++tile) memset(tiles + tile * 32, tile, 32);
 CK_object box = {0}; box.user1 = box.user2 = box.user3 = box.user4 = -1;
 const int lives[] = {0, 1, 9, 10, 99, 3};
 const int shots[] = {0, 1, 9, 10, 99, 100};
 const int scores[] = {0, 1, 12345, 999999, 1000000, 42};
 for (unsigned i = 0; i < sizeof(lives) / sizeof(lives[0]); ++i)
 {
  ck_gameState.numLives = lives[i]; ck_gameState.keenScore = scores[i]; ammo = shots[i];
  CK_UpdateScoreBox(&box);
  assert(PixelByte(0, 20, 2) == (lives[i] > 9 ? 10 : lives[i] + 1));
  int shown = shots[i] > 99 ? 99 : shots[i];
  assert(PixelByte(0, 20, 6) == (shown < 10 ? 0 : shown / 10 + 1));
  assert(PixelByte(0, 20, 7) == shown % 10 + 1);
  int score = scores[i] > 999999 ? 999999 : scores[i];
  for (int col = 7; col >= 2; --col)
  { assert(PixelByte(0, 4, col) == (score || col == 7 ? score % 10 + 1 : 0)); score /= 10; }
  assert(ck_gameState.numLives == lives[i] && ck_gameState.keenScore == scores[i]);
  CheckArtwork();
 }
 // Boobus' icon survives a freshly loaded level, then changes back again.
 for (int mode = 0; mode < 2; ++mode)
 {
  icon = mode ? 28 : 20; box.user3 = -1;
  CK_UpdateScoreBox(&box);
  assert(PixelByte(0, 16, 4) == icon);
  assert(PixelByte(0, 16, 5) == icon + 1);
  assert(PixelByte(0, 24, 4) == icon + 2);
  assert(PixelByte(0, 24, 5) == icon + 3);
  rf_scrollXUnit += 100; CK_UpdateScoreBox(&box); CheckArtwork();
 }
 free(sprite);
 puts("PASS: Dreams score, lives cap, flower/bomb counts and icons, scrolling, reload invalidation, portrait/borders/mask preserved");
 return 0;
}
