/* Blaze Mk. 8: code-drawn EGA artwork for Keen Worlds. GPL-2.0-or-later.
 * The LCD retains the existing menu, dialog and Paddle War coordinates.
 */
#include "kw_watch.h"
#include "ck_def.h"
#include "kw_world.h"
#include "kw_suburb.h"
#include "id_vh.h"
#include "id_ca.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Keen Dreams has no second font. Keep the original compact watch font
// outside the episode cache, so its bitmap chunk is never read as a font.
static uint8_t *kw_sharedWatchFont;
void KW_CacheWatchFont(void)
{
 if(kw_sharedWatchFont || (KW_CurrentWorld()!=KW_KEEN4 && KW_CurrentWorld()!=KW_SHIP)) return;
 int chunk=CK_CHUNKNUM(FON_WATCHFONT);
 CA_CacheGrChunk(chunk);
	const uint8_t *font=(const uint8_t *)ca_graphChunks[chunk];
 int height=font[0]|font[1]<<8,size=770;
 for(int ch=0;ch<256;++ch) {
  int width=font[514+ch],offset=font[2+ch*2]|font[3+ch*2]<<8;
  int end=offset+((width+7)/8)*height;
  if(width && end>size) size=end;
 }
	kw_sharedWatchFont=(uint8_t *)malloc(size);
 if(!kw_sharedWatchFont) Quit("Keen Worlds: cannot retain the shared watch font");
 memcpy(kw_sharedWatchFont,font,size);
}
void KW_FreeWatchFont(void) {free(kw_sharedWatchFont);kw_sharedWatchFont=NULL;}
const uint8_t *KW_DreamWatchFont(void)
{
 return KW_CurrentWorld()==KW_DREAM?kw_sharedWatchFont:NULL;
}

// Small, fixed lettering for the case and Billy's paper reminder.
static const uint8_t kw_watchLetters[38][7] = {
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},
 {14,17,16,16,16,17,14},{30,17,17,17,17,17,30},
 {31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
 {14,4,4,4,4,4,14},{7,2,2,2,18,18,12},
 {17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,25,21,19,19,17},
 {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
 {14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},
 {17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},
 {14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
 {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},
 {14,17,17,14,17,17,14},{14,17,17,15,1,1,14},
 {0,0,0,0,0,6,6},{0,0,0,14,0,0,0}
};
typedef void (*KW_WatchBar)(int,int,int,int,int);

static void KW_WatchLabel(KW_WatchBar bar,const char *text,int x,int y,int colour)
{
 for(;*text;++text,x+=6) {
  int c=toupper((unsigned char)*text),index=c>='A'&&c<='Z'?c-'A':
   c>='0'&&c<='9'?26+c-'0':c=='.'?36:c=='-'?37:-1;
  if(index<0) continue;
  for(int row=0;row<7;++row) for(int col=0;col<5;++col)
   if(kw_watchLetters[index][row]&(16>>col)) bar(x+col,y+row,1,1,colour);
 }
}

static const char *KW_WatchTask(void)
{
 int world=KW_CurrentWorld();
 if(KW_IsWorldCompleted(world)) return "EXPLORE SOME MORE";
 switch(world) {
 case KW_KEEN4:return "FIND THE COUNCIL";
 case KW_KEEN5:case KW_CK55:return "STOP THE MACHINE";
 case KW_KEEN6:return "FIND MOLLY";
 case KW_KEEN7:return "FIND THE SEVEN KEYS";
 case KW_KEEN8:return "FIND A POWER SOURCE";
 case KW_KEEN9:return "FIND MORTIMER";
 case KW_AREALM:return "FOLLOW THE DISTRESS CALL";
 case KW_FITF:return "FIND MY PLUSH FRIENDS";
 case KW_OSI:return "DISABLE THE SHIELD";
 case KW_KEEN95:return "FIND THE ACCUMULATORS";
 case KW_ASYLUM:return "FIND A WAY OUT";
 case KW_BOUNTY:return "FIND RICK ULLER";
 case KW_ALPHAMATIC:return "INVESTIGATE VITACORP";
 case KW_SUBURB:return SS_BagState()==2?"CATCH THE SCHOOL BUS":"FIND MY SCHOOL BAG";
 case KW_MARS:return "GET THE SHIP PARTS";
 case KW_DREAM:return "FIND THE BOOBUS BOMBS";
 default:return "CHECK THE FLIGHT CHART";
 }
}

static void KW_WatchScrew(KW_WatchBar bar,int x,int y)
{
 bar(x,y,7,5,8);bar(x+1,y,5,1,15);bar(x+1,y+1,5,3,7);bar(x+2,y+2,3,1,0);
}

void KW_DrawWatchTitle(const char *title,bool buffered)
{
 KW_WatchBar bar=buffered?VHB_Bar:VH_Bar;
 bar(74,48,160,8,0);KW_WatchLabel(bar,title,80,48,10);bar(77,55,155,1,2);
}

void KW_DrawWatch(bool buffered)
{
 KW_WatchBar bar=buffered?VHB_Bar:VH_Bar;
 bar(0,0,320,200,0);
 // Purple ribbed strap behind a stepped, yellow calculator case.
 bar(119,0,76,200,1);bar(122,0,70,200,5);
 bar(122,0,2,200,13);bar(189,0,3,200,0);
 for(int y=0;y<200;y+=8) {bar(126,y,59,1,13);bar(126,y+5,59,2,1);}
 bar(151,3,12,5,0);bar(151,190,12,5,0);
 bar(52,21,225,165,0);bar(48,15,218,168,6);bar(43,21,229,156,6);
 bar(46,21,223,152,14);bar(49,23,217,148,14);
 bar(49,18,214,2,15);bar(46,24,2,145,14);bar(267,25,3,146,0);
 // Keep the LCD at the original menu/dialog/game coordinates.
 bar(62,41,184,116,0);bar(64,43,180,112,8);bar(68,47,172,104,0);
 bar(64,43,180,1,0);bar(64,43,1,111,0);bar(244,46,1,109,14);
 KW_WatchLabel(bar,"BLAZE MK.8",74,27,0);KW_WatchLabel(bar,"BY BILLY",180,27,0);
 KW_WatchScrew(bar,52,25);KW_WatchScrew(bar,255,25);
 KW_WatchScrew(bar,52,163);KW_WatchScrew(bar,255,163);
 // Crooked grey repair tape on the rim, clear of the display.
 for(int row=0;row<31;++row) {
  int x=253+row/11;bar(x,43+row,17,1,7);
  if(row==3 || row==24) bar(x+2,43+row,12,1,8);
  if(row%7==0) bar(x+16,43+row,2,1,8);
 }
 const int buttonColours[]={4,1,2};
 for(int i=0;i<3;++i) {
  int y=61+i*24;bar(36,y,8,17,0);bar(37,y,6,14,8);
  bar(38,y+1,4,11,buttonColours[i]);bar(38,y+1,4,1,7);
 }
 // Billy's paper to-do note stays outside the usable LCD.
 bar(73,159,173,14,0);bar(72,158,173,14,15);
 const char *task=KW_WatchTask();int width=(int)strlen(task)*6-1;
 KW_WatchLabel(bar,task,72+(173-width)/2,161,0);
 bar(77,169,156,1,6);bar(239,158,6,1,15);bar(244,158,1,6,6);
}
