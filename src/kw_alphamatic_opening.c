/* Keen Worlds: the backyard workshop opening for The Alphamatic.
 * Adapted from Gridlock's original story; GPL-2.0-or-later.
 * Backgrounds, actors and flight art are ours; callers/plans come from the mod.
 */
#include "kw_alphamatic.h"
#include "kw_music.h"
#include "kw_skip.h"
#include "ck_def.h"
#include "ck_play.h"
#include "id_ca.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_sd.h"
#include "id_in.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct AL_SceneLine { int speaker,view; const char *name,*text,*clip; } AL_SceneLine;
#include "kw_alphamatic_opening_data.h"

typedef struct AL_Image { int w,h; uint8_t *pixels,*mask; } AL_Image;
typedef struct AL_Opening
{
 void *workshop,*drive,*orbit;
 AL_Image keen[3],spot,portraits[3],machine,station,ship;
} AL_Opening;

static void AL_Box(int x,int y,int w,int h,int color)
{
 VH_HLine(x,x+w-1,y,color);VH_HLine(x,x+w-1,y+h-1,color);
 VH_VLine(y,y+h-1,x,color);VH_VLine(y,y+h-1,x+w-1,color);
}

static void *AL_Background(const uint8_t *packed)
{
 void *surface=VL_CreateSurface(320,200),*old=VL_SetScreen(surface);
 for(int y=0;y<200;++y) for(int x=0;x<320;x+=2) {
  int b=packed[y*160+x/2];VH_Plot(x,y,b>>4);VH_Plot(x+1,y,b&15);
 }
 VL_SetScreen(old);return surface;
}

static AL_Image AL_CaptureActor(const uint8_t *pixels,int w,int h)
{
	AL_Image image={w,h,(uint8_t *)malloc(w*h),(uint8_t *)malloc(w*h)};
 if(!image.pixels || !image.mask) Quit("Alphamatic scene: no actor memory");
 for(int i=0;i<w*h;++i) {
  image.pixels[i]=pixels[i]&15;
  image.mask[i]=pixels[i]<16;
 }
 return image;
}

// Regression: glyph colour and coverage must not depend on the artwork
// underneath. In particular, painting black must produce a real outline.
static void AL_CheckSolidText(void)
{
 void *reference=VL_CreateSurface(320,200),*sample=VL_CreateSurface(320,200);
 void *old=VL_SetScreen(reference);
 VL_ClearScreen(0);VH_DrawPropStringSolid("Keen and Spot",2,2,0,15);
 uint16_t width,height;VH_MeasurePropString("Keen and Spot",&width,&height,0);
 int ink=0;
 for(int bg=0;bg<16;++bg) for(int c=0;c<3;++c) {
  const int colors[]={0,14,15};int color=colors[c];
  VL_SetScreen(sample);VL_ClearScreen(bg);
  VH_DrawPropStringSolid("Keen and Spot",2,2,0,color);
  for(int y=0;y<height+4;++y) for(int x=0;x<width+4;++x) {
   bool glyph=VL_SurfacePGet(reference,x,y)==15;
   if(glyph) ++ink;
   if(VL_SurfacePGet(sample,x,y)!=(glyph?color:bg))
    Quit("Alphamatic opening: text colour depends on background");
  }
 }
 if(!ink) Quit("Alphamatic opening: empty text fixture");
 VL_SetScreen(old);VL_DestroySurface(reference);VL_DestroySurface(sample);
 puts("PASS: solid lettering and black outlines on all 16 EGA backgrounds");
}

static AL_Image AL_CapturePicture(int chunk,uint16_t backgroundColors)
{
 CA_CacheGrChunk(chunk);
 AL_Image image={0};
 VH_BitmapTableEntry dimensions=VH_GetBitmapTableEntry(chunk-ca_gfxInfoE.offBitmaps);
 image.w=dimensions.width*8;image.h=dimensions.height;
 if(image.w<=0 || image.h<=0 || image.w>320 || image.h>200) Quit("Alphamatic scene: invalid art dimensions");
 int size=image.w*image.h;
	image.pixels=(uint8_t *)malloc(size);image.mask=(uint8_t *)calloc(size,1);
 if(!image.pixels || !image.mask) Quit("Alphamatic scene: no art memory");
 void *surface=VL_CreateSurface(336,208),*old=VL_SetScreen(surface);
 VL_ClearScreen(0);
 VH_DrawBitmap(0,0,chunk);
 for(int y=0;y<image.h;++y) for(int x=0;x<image.w;++x)
  image.pixels[y*image.w+x]=VL_SurfacePGet(surface,x,y)&15;
 {
  // Pictures use different backing colours. Remove only edge-connected
  // backing, so enclosed details such as Spot's eye remain intact.
		int *queue=(int *)malloc(size*sizeof(int)),head=0,tail=0;
  if(!queue) Quit("Alphamatic scene: no mask memory");
  for(int y=0;y<image.h;++y) for(int x=0;x<image.w;++x) {
   int i=y*image.w+x;
   if((x==0 || y==0 || x==image.w-1 || y==image.h-1) &&
      (backgroundColors & (1u<<image.pixels[i])) && !image.mask[i]) {
    image.mask[i]=1;queue[tail++]=i;
   }
  }
  while(head<tail) {
   int i=queue[head++],x=i%image.w,y=i/image.w;
   const int neighbours[4]={x?i-1:-1,x+1<image.w?i+1:-1,y?i-image.w:-1,y+1<image.h?i+image.w:-1};
   for(int n=0;n<4;++n) {
    int at=neighbours[n];
    if(at>=0 && (backgroundColors & (1u<<image.pixels[at])) && !image.mask[at]) {
     image.mask[at]=1;queue[tail++]=at;
    }
   }
  }
  for(int i=0;i<size;++i) image.mask[i]=!image.mask[i];
  // Preserve the one-pixel black outline against the removed backing.
  // Mark it separately before applying it, so it cannot grow into a margin.
  if(backgroundColors & (1u<<8)) {
   int outline=0;
   for(int y=0;y<image.h;++y) for(int x=0;x<image.w;++x) {
    int i=y*image.w+x;
    if(image.pixels[i] || image.mask[i]) continue;
    bool outlined=false;
    for(int dy=-1;dy<=1 && !outlined;++dy) for(int dx=-1;dx<=1;++dx) {
     int nx=x+dx,ny=y+dy;
     if(nx>=0 && nx<image.w && ny>=0 && ny<image.h) {
      int n=ny*image.w+nx;
      if(image.mask[n] && !(backgroundColors & (1u<<image.pixels[n]))) {
       outlined=true;break;
      }
     }
    }
    if(outlined) queue[outline++]=i;
   }
   for(int i=0;i<outline;++i) image.mask[queue[i]]=1;
  }
  free(queue);
 }
 VL_SetScreen(old);VL_DestroySurface(surface);return image;
}

static void AL_DrawImage(const AL_Image *image,int x,int y,int w,int h)
{
 for(int dy=0;dy<h;++dy) for(int dx=0;dx<w;++dx) {
  int i=(dy*image->h/h)*image->w+dx*image->w/w;
  if(image->mask[i] && x+dx>=0 && x+dx<320 && y+dy>=0 && y+dy<200)
   VH_Plot(x+dx,y+dy,image->pixels[i]);
 }
}

// Pixel-measured wrapping keeps every subtitle inside the 320x200 page.
static int AL_Wrap(const char *text,char lines[][80],int maxLines,int width)
{
 int count=0;
 while(*text && count<maxLines) {
  char *line=lines[count];int used=0,lastSpace=-1;
  while(text[used] && used<78) {
   line[used]=text[used];line[used+1]=0;
   uint16_t w,h;VH_MeasurePropString(line,&w,&h,0);
   if(w>width) break;
   if(text[used]==' ') lastSpace=used;
   ++used;
  }
  if(text[used] && lastSpace>0) used=lastSpace;
  if(!used) used=1;
  memcpy(line,text,used);line[used]=0;
  text+=used;while(*text==' ') ++text;
  ++count;
 }
 if(*text) Quit("Alphamatic scene: text does not fit");
 return count;
}

static void AL_Subtitle(const char *name,const char *text,int color)
{
 char lines[4][80];int count=AL_Wrap(text,lines,4,296);
 VH_Bar(0,144,320,56,0);VH_HLine(10,309,145,8);
 VH_DrawPropStringSolid(name,12,150,0,color);
 for(int i=0;i<count;++i) VH_DrawPropStringSolid(lines[i],12,162+i*9,0,15);
}

static void AL_Mars(int cx,int cy,int radius)
{
 for(int y=-radius;y<=radius;++y) for(int x=-radius;x<=radius;++x)
  if(x*x+y*y<=radius*radius) {
   int color=x<-radius/3 ? 4 : 6;
   if(x>radius/2 && (x+y)%2) color=12;
   int dx=x-radius/5,dy=y+radius/3;
   if(dx*dx+2*dy*dy<radius*radius/16) color=4;
   if(y>radius/8 && y<radius/4 && x>-radius/4 && x<radius*3/4) color=4;
   if(cx+x>=0 && cx+x<320 && cy+y>=0 && cy+y<144) VH_Plot(cx+x,cy+y,color);
  }
}

static void AL_Workshop(AL_Opening *scene,int speaker,int view,uint32_t t,bool connecting)
{
 VL_SurfaceToScreen(scene->workshop,0,0,0,0,320,200);
 // Keep all three remote friends visible, even when plans occupy the screen.
 for(int i=0;i<3;++i) {
  int x=178+i*36,active=speaker==i+2;
  VH_Bar(x-1,21,34,60,0);
  AL_Box(x-1,21,34,60,active ? 14 : 8);
  if(connecting && t<(uint32_t)(i+1)*30) {
   VH_DrawPropStringSolid("...",x+10,44,0,(t/12)%2?10:2);
   continue;
  }
  AL_DrawImage(&scene->portraits[i],x,24+(active && (t/10)%2),32,44);
  const char *name=i==0 ? "VIA" : i==1 ? "Lind." : "Elder";
  VH_DrawPropStringSolid(name,x+1,71,0,active?14:7);
 }
 // The machine insert is on a second display, with the call still connected.
 if(view) {
  VH_Bar(18,27,124,58,0);AL_Box(18,27,124,58,11);
  if(view==1) {
   AL_DrawImage(&scene->machine,27,32,104,42);
   VH_DrawPropStringSolid("ALPHAMATIC",37,75,0,10);
  } else {
   AL_Mars(82,54,20);
   VH_DrawPropStringSolid("MARS ORBIT",38,75,0,14);
   VH_Plot(109+(t/12)%4,44,15);
  }
 }
 int pose=speaker==0 ? 1+(t/18)%2 : (t%280>266 ? 1 : 0);
 int bob=speaker==0 ? (t/12)%2 : 0;
 AL_DrawImage(&scene->keen[pose],76,78-bob,AL_KEEN_W,AL_KEEN_H);
 int spotX=connecting ? 20+98*(int)(t>140?140:t)/140 : 118;
 int spotY=98-(speaker==1 ? (t/12)%3 : connecting ? (t/8)%2 : 0);
 AL_DrawImage(&scene->spot,spotX,spotY,AL_SPOT_W,AL_SPOT_H);
 // The CRT's status light blinks; the screen border stays still.
 VH_Bar(287,39,3,3,(t/35)%2?10:2);
}

static void AL_Crawl(AL_Opening *scene,uint32_t elapsed,uint32_t duration)
{
 int third=elapsed<duration/3 ? 0 : elapsed<2*duration/3 ? 1 : 2;
 VL_SurfaceToScreen(third==1?scene->drive:scene->workshop,0,0,0,0,320,200);
 if(third==2) {
  VH_Bar(177,21,108,61,0);
  VH_DrawPropStringSolid("VITACRYPT",192,36,0,10);
  VH_DrawPropStringSolid("ACCESS DENIED",184,50,0,12);
  VH_DrawPropStringSolid("KEY REQUIRED",187,66,0,7);
 }
 // A half-screen upward credit roll; the illustration remains visible at right.
 VH_Bar(0,24,146,176,0);
 VH_VLine(145,24,199,8);
 char lines[32][80];int count=AL_Wrap(al_introText,lines,32,128);
 uint32_t travel=190+count*11;
 int offset=190-(int)((uint64_t)(elapsed>duration?duration:elapsed)*travel/duration);
 for(int i=0;i<count;++i) {
  int y=offset+i*11;
  if(y>=26 && y<=184) {
   VH_DrawPropStringSolid(lines[i],9,y,0,15);
  }
 }
 VH_Bar(0,0,146,24,0);
 VH_DrawPropStringSolid("BACKYARD WORKSHOP",9,8,0,14);
}

static void AL_AttachLine(int x,int y,int x2,int y2)
{
 int dx=abs(x2-x),dy=-abs(y2-y),sx=x<x2?1:-1,sy=y<y2?1:-1,error=dx+dy;
 for(;;) {
  if(x>=0 && x<320 && y>=0 && y<144) VH_Plot(x,y,8);
  if(x==x2 && y==y2) break;
  int twice=2*error;
  if(twice>=dy) {error+=dy;x+=sx;}
  if(twice<=dx) {error+=dx;y+=sy;}
 }
}

static void AL_Departure(AL_Opening *scene,uint32_t elapsed,uint32_t duration)
{
 VL_SurfaceToScreen(scene->orbit,0,0,0,0,320,200);
 AL_DrawImage(&scene->station,186,36,AL_STATION_W,AL_STATION_H);
 // Decelerate into a stealth approach, then secure the Attach-O-Ray.
 int progress=(int)((uint64_t)(elapsed>duration?duration:elapsed)*1024/duration);
 int eased=progress*(2048-progress)/1024;
 int x=-64+204*eased/1024,y=94+7*progress/1024;
 if(progress<720 && x>6) {
  int length=4+(elapsed/5)%4;
  VH_HLine(x-length,x-1,y+17,9);
  VH_HLine(x-length/2,x-1,y+18,11);
 }
 AL_DrawImage(&scene->ship,x,y,AL_SHIP_W,AL_SHIP_H);
 if(progress>=768) {
  AL_AttachLine(x+63,y+24,218,123);
  VH_Plot(218,123,11);
 }
 AL_Subtitle("Mars orbit",al_departureText,11);
}

static void AL_TestShot(const char *name)
{
 const char *folder=getenv("KW_AUTOCUTSCENE");if(!folder) return;
 char path[600];snprintf(path,sizeof(path),"%s/%s.ppm",folder,name);
 void KW_DumpScreenTo(const char *path);
 KW_DumpScreenTo(path);
}

// A fresh tap advances a spoken turn. Holding Escape/menu skips the whole film.
static bool AL_PlayPart(AL_Opening *scene,int part)
{
 const int count=(int)(sizeof(al_sceneLines)/sizeof(al_sceneLines[0]));
 bool crawl=part==-1,connecting=part==-2,departure=part==count,automatic=getenv("KW_AUTOCUTSCENE")!=NULL;
 const AL_SceneLine *line=crawl||connecting||departure?NULL:&al_sceneLines[part];
 const char *clip=connecting?NULL:crawl?"AL_CUT_INTRO.RAW":departure?"AL_CUT_DEPART.RAW":line->clip;
 const char *words=connecting?"Connecting...":crawl?al_introText:departure?al_departureText:line->text;
 if(departure) {
  VL_FadeToBlack();AL_Departure(scene,0,12*70);
  VH_UpdateScreen();VL_FadeFromBlack();
 }
 bool voiced=KW_Narrate(clip);
 KW_Presentation controls;KW_BeginPresentation(&controls);
 uint32_t start=SD_GetTimeCount(),duration=connecting?140:crawl?30*70:departure?12*70:70+(uint32_t)strlen(words)*3;
 for(;;) {
  uint32_t elapsed=SD_GetTimeCount()-start;
  KW_ClearPresentationHint(&controls);
  if(crawl) AL_Crawl(scene,elapsed,duration);
  else if(connecting) {
   AL_Workshop(scene,-1,0,elapsed,true);
   AL_Subtitle("Video transmitter","Connecting to friends...",10);
  }
  else if(departure) AL_Departure(scene,elapsed,duration);
  else {
   AL_Workshop(scene,line->speaker,line->view,elapsed,false);
   AL_Subtitle(line->name,line->text,line->speaker==0?14:11);
  }
  VH_UpdateScreen();
  if(automatic) {
   if(crawl) {
    const char *names[]={"01-workshop-crawl","02-drive-crawl","03-vitacrypt-crawl"};
    for(int i=0;i<3;++i) {AL_Crawl(scene,duration*(2*i+1)/6,duration);AL_TestShot(names[i]);}
   } else if(connecting) {
    AL_Workshop(scene,-1,0,70,true);AL_Subtitle("Video transmitter","Connecting to friends...",10);
    AL_TestShot("03b-call-connect");
   } else if(departure) {
    AL_Departure(scene,duration/3,duration);AL_TestShot("18-flight-approach");
    AL_Departure(scene,duration*7/8,duration);AL_TestShot("19-departure");
   } else {
    char name[64];snprintf(name,sizeof(name),"%02d-%s",part+4,line->name);AL_TestShot(name);
    if(part==1) {
     AL_Workshop(scene,line->speaker,line->view,24,false);AL_Subtitle(line->name,line->text,14);
     AL_TestShot("05b-Keen-motion");
    }
   }
   break;
  }
  if(KW_PollPresentation(&controls,!crawl && !connecting)) break;
  KW_PresentWithSkipHint(&controls,!crawl && !connecting);
  if(crawl || connecting || departure) {
   if(elapsed>=duration && !KW_ClipPlaying()) break;
  } else if(voiced ? !KW_ClipPlaying() && elapsed>35 : elapsed>=duration) break;
  VL_DelayTics(2);
 }
 bool skipped=controls.accepted && controls.holding;
 KW_StopVoice();KW_EndPresentation(&controls);
 return skipped;
}

void AL_PlayOpening(void)
{
 if(!AL_IsActive()) return;
 printf("Alphamatic: workshop opening\n");fflush(stdout);
 AL_Opening scene={0};
 KW_LinesOver();SD_MusicOff();
 VL_FadeToBlack();VL_SetScrollCoords(0,0);VL_SetDefaultPalette();
 CA_UpLevel();
 CA_CacheGrChunk(CK_CHUNKNUM(FON_MAINFONT));
 if(getenv("KW_AUTOCUTSCENE")) AL_CheckSolidText();
 scene.workshop=AL_Background(al_workshopPicture);scene.drive=AL_Background(al_drivePicture);
 scene.orbit=AL_Background(al_orbitPicture);
 for(int i=0;i<3;++i) scene.keen[i]=AL_CaptureActor(al_keenPosePixels[i],AL_KEEN_W,AL_KEEN_H);
 scene.spot=AL_CaptureActor(al_spotPixels,AL_SPOT_W,AL_SPOT_H);
 const int portraits[]={14,16,15};
 for(int i=0;i<3;++i) scene.portraits[i]=AL_CapturePicture(portraits[i],1u<<0);
 // The original machine picture includes a blue schematic grid and gray
 // backing outside the circular platform; neither belongs to the station.
 scene.machine=AL_CapturePicture(53,(1u<<0)|(1u<<1)|(1u<<8)|(1u<<9));
 scene.station=AL_CaptureActor(al_stationPixels,AL_STATION_W,AL_STATION_H);
 scene.ship=AL_CaptureActor(al_shipPixels,AL_SHIP_W,AL_SHIP_H);
 StartMusic(27); // The mod's own opening tune, quietly under the voices.
 SD_FadeAdLibVolume(KW_VoicesEnabled() && KW_VoiceVolume()?40:128,0);
 AL_Crawl(&scene,0,30*70);VH_UpdateScreen();VL_FadeFromBlack();
 bool skipped=AL_PlayPart(&scene,-1);
 if(!skipped) skipped=AL_PlayPart(&scene,-2);
 const int count=(int)(sizeof(al_sceneLines)/sizeof(al_sceneLines[0]));
 for(int i=0;i<=count && !skipped;++i) skipped=AL_PlayPart(&scene,i);
 VL_FadeToBlack();KW_LinesOver();SD_MusicOff();
 SD_FadeAdLibVolume(256,0);
 VL_DestroySurface(scene.workshop);VL_DestroySurface(scene.drive);VL_DestroySurface(scene.orbit);
 AL_Image *images[]={&scene.keen[0],&scene.keen[1],&scene.keen[2],&scene.spot,
                    &scene.portraits[0],&scene.portraits[1],&scene.portraits[2],&scene.machine,&scene.station,&scene.ship};
 for(unsigned i=0;i<sizeof(images)/sizeof(images[0]);++i) {free(images[i]->pixels);free(images[i]->mask);}
 CA_DownLevel();
 IN_ClearKeysDown();
 printf("Alphamatic: workshop opening %s\n",skipped?"skipped":"finished");fflush(stdout);
}
