/* Keen Worlds: story scenes of its own, between the games. GPL-2.0-or-later.
 * Calidune's arrival and ending, and Spot's rescue on Fribbulus Xax.
 * In the house style of the Alphamatic opening (doc/alphamatic-opening.md):
 * a staged, voiced scene with subtitles; a fresh tap advances a spoken
 * line, holding Escape (or the controller's menu button) skips it all, and
 * the game's state is not touched. The orbit and the desert are drawn here;
 * Keen's poses and the Megarocket are the Alphamatic opening's. The words
 * are Keen Worlds' own (Dead in the Desert sets Keen down on Calidune
 * "without his trusty Bean with Bacon Megarocket" and he leaves in a copy).
 */
#include "kw_story_scenes.h"
#include "kw_alphamatic.h"
#include "kw_music.h"
#include "kw_skip.h"
#include "ck_def.h"
#include "id_ca.h"
#include "id_vh.h"
#include "id_vl.h"
#include "id_sd.h"
#include "id_in.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CD_Image { int w,h; uint8_t *pixels,*mask; } CD_Image;
typedef struct CD_Scene { void *orbit,*xax,*sand,*street; CD_Image keen[3],spot,ship,copy; } CD_Scene;

typedef enum CD_Stage { CD_WARN, CD_AGAIN, CD_BEAM, CD_SAND, CD_RISE, CD_COPY, CD_OFF,
	SP_ORBIT, SP_LAND, SP_SPOT1, SP_LOCK, SP_HOME, SP_SPOT2, SP_DEPART } CD_Stage;
typedef struct CD_Part
{
	CD_Stage stage;
	const char *name,*text,*clip;
	int color;
	uint32_t tics; // unvoiced: how long it lasts
	uint32_t lead; // animation before the line is spoken
} CD_Part;

static const CD_Part cd_arrival[] = {
	{CD_WARN,"Megarocket","Warning. A transporter beam has locked onto you. It has Mortimer's signature.","CD_CUT_WARN.RAW",11,0,70},
	{CD_AGAIN,"Keen","Not again!","CD_CUT_AGAIN.RAW",14,0,0},
	{CD_BEAM,"Calidune","The beam pulls Keen down to the desert below. The Megarocket cannot follow.",NULL,7,4*70,0},
	{CD_SAND,"Keen","Sand, sun... and no ship. Thanks a lot, Mortimer.","CD_CUT_SAND.RAW",14,0,100},
};
static const CD_Part cd_ending[] = {
	{CD_RISE,"Calidune orbit","Keen flies up from Calidune in a strange copy of his ship. The Megarocket is waiting.",NULL,7,5*70,0},
	{CD_COPY,"Megarocket","You found a copy of me. It is not as good.","CD_CUT_COPY.RAW",11,0,0},
	{CD_OFF,"Keen","It got me off Calidune, didn't it?","CD_CUT_OFF.RAW",14,0,0},
};

// Spot's rescue, after the Shikadi Station: the third episode of the Spot
// story was never released, so Keen Worlds tells its end (STORY_SEAMS.md, 5).
static const CD_Part sp_rescue[] = {
	{SP_ORBIT,"Fribbulus Xax","With the Shikadi shield down, the Megarocket drops through the clouds of Fribbulus Xax. "
		"The Bloogs are holding Spot in Bloogton. Keen has been here before.","SP_CUT_ORBIT.RAW",7,0,0},
	{SP_LAND,"Megarocket","Landing in Bloogton. I am detecting one Yorp. It is complaining loudly.","SP_CUT_LAND.RAW",11,0,40},
	{SP_SPOT1,"Spot","Keen! I knew you would come! They kept trying to teach me to fetch.","SP_CUT_SPOT1.RAW",13,0,0},
	{SP_LOCK,"Keen","Nobody kidnaps my Yorp. Hold still, I'll get the lock.","SP_CUT_LOCK.RAW",14,0,0},
	{SP_HOME,"Keen","Come on, Spot. We're going home.","SP_CUT_HOME.RAW",14,0,50},
	{SP_SPOT2,"Spot","Can we stop for snacks on the way?","SP_CUT_SPOT2.RAW",13,0,0},
	{SP_DEPART,"Fribbulus Xax","The Bloogs watched them go. None of them tried to stop the boy with the pogo stick. Not again.",
		"SP_CUT_DEPART.RAW",7,0,0},
};

static uint32_t CD_Noise(int x,int y,int z)
{
	uint32_t h=(uint32_t)x*73856093u^(uint32_t)y*19349663u^(uint32_t)z*83492791u;
	h^=h>>16;h*=0x7feb352du;h^=h>>15;h*=0x846ca68bu;h^=h>>16;
	return h;
}

static CD_Image CD_Capture(const uint8_t *pixels,int w,int h,const uint8_t *recolor,bool flip)
{
	CD_Image image={w,h,(uint8_t *)malloc(w*h),(uint8_t *)malloc(w*h)};
	if(!image.pixels || !image.mask) Quit("Calidune scene: no actor memory");
	for(int y=0;y<h;++y) for(int x=0;x<w;++x) {
		uint8_t p=pixels[y*w+(flip?w-1-x:x)];
		image.pixels[y*w+x]=recolor?recolor[p&15]:p&15;
		image.mask[y*w+x]=p<16;
	}
	return image;
}

static void CD_Free(CD_Image *image) {free(image->pixels);free(image->mask);}

// Drawn at its own size; "reveal" (0-256) shows that share of its pixels,
// the rest are the transporter's sparkle.
static void CD_Draw(const CD_Image *image,int x,int y,int reveal,uint32_t t)
{
	for(int dy=0;dy<image->h;++dy) for(int dx=0;dx<image->w;++dx) {
		int i=dy*image->w+dx,px=x+dx,py=y+dy;
		if(!image->mask[i] || px<0 || px>=320 || py<0 || py>=144) continue;
		if((int)(CD_Noise(dx,dy,7)&255)<reveal) VH_Plot(px,py,image->pixels[i]);
		else if(reveal>0 && (CD_Noise(dx,dy,t/3)&7)==0) VH_Plot(px,py,(CD_Noise(dx,dy,t)&1)?15:11);
	}
}

// The transporter's shimmer: twinkling points in a box.
static void CD_Sparkle(int x,int y,int w,int h,int count,uint32_t t)
{
	static const int colors[]={15,11,3,9,15,11};
	for(int i=0;i<count;++i) {
		uint32_t n=CD_Noise(i,(int)(t/2),31);
		int px=x+(int)(n%(uint32_t)(w>0?w:1)),py=y+(int)((n>>12)%(uint32_t)(h>0?h:1));
		if(px>=0 && px<320 && py>=0 && py<144) {
			VH_Plot(px,py,colors[(n>>24)%6]);
			if((n>>28)==0 && px+1<320) VH_Plot(px+1,py,15);
		}
	}
}

// A planet from orbit: stars, and its curve below. Colours: lit, mid, dark,
// night, the bright rim of its air, and the air beyond the dark side.
static const uint8_t cd_calidune[6]={14,6,4,0,12,4},cd_xax[6]={10,2,3,1,11,9};
static void *CD_DrawOrbit(const uint8_t *c)
{
	void *surface=VL_CreateSurface(320,200),*old=VL_SetScreen(surface);
	VL_ClearScreen(0);
	const double cx=200,cy=330,r=235;
	for(int y=0;y<144;++y) for(int x=0;x<320;++x) {
		double dx=(x-cx)/r,dy=(y-cy)/r,d=dx*dx+dy*dy;
		int color=0;
		if(d<=1.0) {
			// lit from above and to the right; the night side is dark rust,
			// and wind-blown bands of darker sand curve across the dunes
			double light=dx*0.45-dy*0.9+0.03*sin(x/7.0+y/3.0);
			bool band=sin(dy*46.0+3.0*sin(dx*8.0)+1.5*sin(dx*23.0))>0.55;
			bool dither=(x+y)&1;
			if(light>0.95) color=band?c[1]:c[0];
			else if(light>0.85) color=dither?(band?c[1]:c[0]):c[1];
			else if(light>0.6) color=band?c[2]:c[1];
			else if(light>0.45) color=dither?c[1]:c[2];
			else if(light>0.25) color=c[2];
			else if(light>0.15) color=dither?c[2]:c[3];
			else color=c[3];
			if(d>0.985) color=light>0.6?c[0]:light>0.25?c[4]:c[2]; // the bright rim of its air
		} else if(d<=1.012) color=(x+y)&1?c[4]:c[5];
		else {
			uint32_t n=CD_Noise(x,y,1);
			if(n%173==0) color=15;
			else if(n%173==1) color=7;
			else if(n%311==2) color=8;
		}
		VH_Plot(x,y,color);
	}
	VL_SetScreen(old);
	return surface;
}

// The desert: pale sky, the sun, red mesas, dunes in sunlight.
static double CD_Dune(int x,double base,double height,double width,double phase)
{
	return base+height*sin(x/width+phase)+height*0.35*sin(x/(width*0.43)+phase*2.1);
}

static void *CD_DrawSand(void)
{
	void *surface=VL_CreateSurface(320,200),*old=VL_SetScreen(surface);
	VL_ClearScreen(0);
	for(int y=0;y<144;++y) for(int x=0;x<320;++x) {
		bool dither=(x+y)&1;
		int color=y<22?(dither&&y>14?9:1):y<44?(y<30&&dither?1:9):y<66?(y<52&&dither?9:11):y<88?(y<74&&dither?11:7):7;
		// the sun, white hot, high on the right
		int sx=x-252,sy=y-28,sd=sx*sx+sy*sy;
		if(sd<=12*12) color=15;
		else if(sd<=16*16) color=dither?15:14;
		else if(sd<=22*22 && dither && (x/2+y)%2) color=14;
		// red mesas on the horizon
		double mesa=86-((x>18&&x<74)?14-abs(x-46)/6:0)-((x>150&&x<232)?10+((x>170&&x<214)?8:0):0)
		 -((x>280)?6:0);
		if(y>=mesa && y<96) color=(y<mesa+2)?12:(dither&&y>mesa+6?6:4);
		// dunes: far, middle, near; the slope facing the sun is lit
		const double base[3]={94,108,126},height[3]={3,6,8},width[3]={23,41,57},phase[3]={0.4,2.0,4.1};
		for(int d=0;d<3;++d) {
			double top=CD_Dune(x,base[d],height[d],width[d],phase[d]);
			if(y>=top) {
				double slope=CD_Dune(x+1,base[d],height[d],width[d],phase[d])-top;
				bool lit=slope<0.05;
				color=lit?14:6;
				if(fabs(slope)<0.12 && dither) color=lit?6:14;
				// wind ripples
				if(((int)(y+3*sin(x/7.0+d))%6)==0 && (x+d)%3) color=lit?6:4;
				if(y<top+1) color=lit?15:14;
			}
		}
		VH_Plot(x,y,color);
	}
	VL_SetScreen(old);
	return surface;
}

// Bloogton: a green sky with a ringed moon, Bloog domes, a paved street.
static void *CD_DrawStreet(void)
{
	void *surface=VL_CreateSurface(320,200),*old=VL_SetScreen(surface);
	VL_ClearScreen(0);
	for(int y=0;y<144;++y) for(int x=0;x<320;++x) {
		bool dither=(x+y)&1;
		int color=y<36?2:y<62?(dither?2:10):10;
		// the moon, with its ring
		int mx=x-62,my=y-26,md=mx*mx+my*my;
		if(md<=11*11) color=(mx+my<-4)?15:7;
		double ring=(mx*0.35-my)*(mx*0.35-my)/9.0+mx*mx/600.0;
		if(ring<1.0 && ring>0.55 && !(md<=11*11 && my<0)) color=14;
		// domes of the town on the horizon, with lit windows
		static const int domes[][3]={{18,26,14},{70,34,20},{128,22,12},{196,40,24},{262,28,16},{306,24,13}};
		for(int i=0;i<6;++i) {
			int dx=x-domes[i][0],dy=y-100,r=domes[i][1],h=domes[i][2];
			if(dy<=0 && dx*dx*h*h+dy*dy*r*r<=r*r*h*h) {
				color=dx>r/3?(dither?5:13):dx<-r/3?5:13;
				if(dy==-h/2 && (dx+40)%7<2) color=14;
			}
		}
		if(y>=100 && y<104) color=y==100?7:8;          // the kerb
		if(y>=104) {
			color=dither&&y<110?8:7;                     // the paving
			if((y-104)%9==0 || ((x+(y-104)/9*11)%22==0)) color=8;
		}
		VH_Plot(x,y,color);
	}
	VL_SetScreen(old);
	return surface;
}

// A Bloog, drawn: a round green body, one big eye, two feet.
static void CD_Bloog(int x,int y,uint32_t t)
{
	int step=(int)((t/6)%2);
	for(int dy=-10;dy<=10;++dy) for(int dx=-9;dx<=9;++dx) {
		int d=dx*dx*100+dy*dy*81;
		if(d>8100) continue;
		int color=d>6400?2:(dx>3&&dy<-2?11:10);
		int ex=dx-1,ey=dy+3;
		if(ex*ex+ey*ey<=10) color=ex*ex+ey*ey<=2?0:15;
		if(x+dx>=0 && x+dx<320) VH_Plot(x+dx,y+dy-step,color);
	}
	VH_Bar(x-6+step*2,y+9,4,3,2);VH_Bar(x+2-step*2,y+9,4,3,2);
}

// Spot's cage, the door shut or swung open.
static void CD_Cage(bool open)
{
	VH_Bar(212,58,54,4,8);VH_HLine(212,265,58,7);VH_Bar(212,118,54,3,8);
	for(int x=214;x<=262;x+=6) {
		bool door=x>=226 && x<=250;
		if(door && open) continue;
		VH_VLine(62,117,x,7);VH_VLine(62,117,x+1,8);
	}
	if(open) for(int i=0;i<5;++i) {VH_VLine(64+i,115-i,268+i*4,7);VH_VLine(64+i,115-i,269+i*4,8);}
	else VH_Bar(236,86,6,8,14); // the lock
}

static int CD_Wrap(const char *text,char lines[][80],int maxLines,int width)
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
	if(*text) Quit("Calidune scene: text does not fit");
	return count;
}

static void CD_Subtitle(const char *name,const char *text,int color)
{
	VH_Bar(0,144,320,56,0);VH_HLine(10,309,145,8);
	if(!name) return;
	char lines[4][80];int count=CD_Wrap(text,lines,4,296);
	VH_DrawPropStringSolid(name,12,150,0,color);
	for(int i=0;i<count;++i) VH_DrawPropStringSolid(lines[i],12,162+i*9,0,15);
}

static void CD_Hint(const KW_Presentation *state,bool allowAdvance)
{
	if(!KW_PresentationHintVisible(state)) {VL_Present();return;}
	const char *text=state->accepted?"Release to continue":
		allowAdvance && !state->holding?"Press to continue":"Hold Esc to skip";
	uint16_t width,height;VH_MeasurePropString(text,&width,&height,0);
	int x=308-width,y=150;
	VH_DrawPropStringSolid(text,x,y,0,14);
	if(state->holding) {
		uint32_t held=SD_GetTimeCount()-state->holdStarted;
		if(held>KW_SKIP_HOLD_TICS) held=KW_SKIP_HOLD_TICS;
		VL_ScreenRect(x,y+height+1,width,1,8);
		VL_ScreenRect(x,y+height+1,width*held/KW_SKIP_HOLD_TICS,1,14);
	}
	VL_Present();
}

// Keen in a framed picture, in the corner of an orbit shot.
static void CD_Inset(const CD_Scene *scene,int x,int pose,int reveal,bool speaking,uint32_t t)
{
	VH_Bar(x,6,52,82,0);
	VH_HLine(x,x+51,6,speaking?14:8);VH_HLine(x,x+51,87,speaking?14:8);
	VH_VLine(6,87,x,speaking?14:8);VH_VLine(6,87,x+51,speaking?14:8);
	CD_Draw(&scene->keen[pose],x+4,10,reveal,t);
	if(reveal<256) CD_Sparkle(x+4,10,44,64,40+(256-reveal)/3,t);
	VH_DrawPropStringSolid("Keen",x+14,76,0,speaking?14:7);
}

static int CD_Ease(uint32_t t,uint32_t duration,int from,int to)
{
	if(t>=duration) return to;
	int p=(int)(t*1024/duration),eased=p*(2048-p)/1024;
	return from+(to-from)*eased/1024;
}

// One frame. t: tics since this part began; spoken: the line is being said.
static void CD_Frame(CD_Scene *scene,const CD_Part *part,uint32_t t,bool talking)
{
	switch(part->stage) {
	case CD_WARN: case CD_AGAIN: case CD_BEAM: {
		VL_SurfaceToScreen(scene->orbit,0,0,0,0,320,200);
		int x=part->stage==CD_WARN?CD_Ease(t,220,-70,124):124,y=48+(int)((t/24)%2);
		if(part->stage==CD_WARN && x<120) { // the engine, while it still flies in
			int length=4+(t/5)%4;
			VH_HLine(x-length,x-1,y+17,9);VH_HLine(x-length/2,x-1,y+18,11);
		}
		if(part->stage!=CD_BEAM || t<40) {
			// the beam, from the sand up to the ship
			int strength=part->stage==CD_WARN?(t<70?0:(int)(t-70)):200;
			if(strength>0) {
				int beam=x+30;
				for(int by=y+26;by<104;++by) if((CD_Noise(by,(int)(t/2),3)&1)==0) VH_Plot(beam+(int)(CD_Noise(by,(int)t,4)%6),by,(CD_Noise(by,(int)t,5)&3)?11:15);
				CD_Sparkle(x-4,y-6,72,40,strength>120?120:strength,t);
			}
		}
		CD_Draw(&scene->ship,x,y,256,t);
		if(part->stage==CD_AGAIN)
			CD_Inset(scene,8,talking?1+(t/12)%2:0,256-(int)(t>90?160:t*160/90),talking,t);
		if(part->stage==CD_BEAM) {
			// Keen, a knot of sparkle, falls to the sand; a flash where he lands
			uint32_t fall=150;
			if(t<fall) {
				int kx=x+30+(int)(t*24/fall),ky=y+20+(int)(t*82/fall);
				CD_Sparkle(kx-5,ky-8,10,16,30,t);
			} else if(t<fall+30) {
				int r=(int)(t-fall)/3;
				for(int i=-r;i<=r;++i) {VH_Plot(178+i,102,15);if(i>-r/2 && i<r/2) VH_Plot(178+i,101,11);}
			}
		}
		break;
	}
	case CD_SAND: {
		VL_SurfaceToScreen(scene->sand,0,0,0,0,320,200);
		// heat shimmer: a few bright grains drifting over the near dune
		for(int i=0;i<12;++i) {
			uint32_t n=CD_Noise(i,0,9);
			int gx=(int)((n%320+t*(1+(n>>20)%2))%320),gy=118+(int)((n>>8)%22);
			VH_Plot(gx,gy,15);
		}
		int reveal=t<part->lead?(int)(t*256/part->lead):256;
		int pose=talking?1+(t/14)%2:(t%300>285?1:0);
		CD_Draw(&scene->keen[pose],140,74,reveal,t);
		if(reveal<256) CD_Sparkle(136,60+(256-reveal)/16,52,78,(256-reveal)/2,t);
		break;
	}
	case CD_RISE: case CD_COPY: case CD_OFF: {
		VL_SurfaceToScreen(scene->orbit,0,0,0,0,320,200);
		uint32_t all=part->stage==CD_RISE?t:400+t;
		int mx=70,my=46+(int)((all/26)%2);
		CD_Draw(&scene->ship,mx,my,256,all);
		// the copy faces the Megarocket and comes up beside it
		int cx=CD_Ease(all,300,224,150),cy=CD_Ease(all,300,112,52)+(int)((all/22)%2);
		if(all<300) {
			int length=5+(all/4)%5;
			VH_HLine(cx+64,cx+63+length,cy+17,12);VH_HLine(cx+64,cx+63+length/2,cy+18,14);
		}
		CD_Draw(&scene->copy,cx,cy,256,all);
		if(part->stage==CD_COPY && (all/18)%2) VH_Bar(mx+44,my+9,2,2,10); // the computer talks
		if(part->stage==CD_OFF) CD_Inset(scene,260,talking?1+(t/12)%2:0,256,talking,t);
		break;
	}
	case SP_ORBIT: case SP_DEPART: {
		VL_SurfaceToScreen(scene->xax,0,0,0,0,320,200);
		int x,y;
		if(part->stage==SP_ORBIT) {x=CD_Ease(t,500,-70,150);y=CD_Ease(t,500,8,92);}
		else {x=CD_Ease(t,500,150,330);y=CD_Ease(t,500,92,-30);}
		int length=5+(t/4)%5;
		VH_HLine(x-length,x-1,y+17,part->stage==SP_ORBIT?9:12);VH_HLine(x-length/2,x-1,y+18,part->stage==SP_ORBIT?11:14);
		CD_Draw(&scene->ship,x,y,256,t);
		break;
	}
	default: { // SP_LAND to SP_SPOT2: the street in Bloogton
		VL_SurfaceToScreen(scene->street,0,0,0,0,320,200);
		CD_Draw(&scene->ship,6,92,256,t);
		if(part->stage==SP_LAND)
			for(int i=0;i<3;++i) CD_Bloog(150+i*34+(int)(t*(2+i)/3),110-(i%2)*4,t+i*5);
		bool open=part->stage>SP_HOME || (part->stage==SP_HOME && t>=part->lead/2);
		// Spot: in the cage, then out beside Keen
		int sx=222,sy=74;
		if(part->stage==SP_HOME) sx=t<part->lead/2?222:CD_Ease(t-part->lead/2,140,222,176);
		if(part->stage==SP_SPOT2) sx=176;
		bool spotTalks=talking && (part->stage==SP_SPOT1 || part->stage==SP_SPOT2);
		sy-=spotTalks?(int)((t/12)%3):(part->stage==SP_HOME && t>part->lead?(int)((t/9)%2)*2:0);
		CD_Draw(&scene->spot,sx,sy,256,t);
		CD_Cage(open);
		int kx=part->stage==SP_LAND?CD_Ease(t,120,60,118):part->stage==SP_SPOT1?118:
			part->stage==SP_LOCK?CD_Ease(t,120,118,160):part->stage==SP_HOME?CD_Ease(t,120,160,132):132;
		bool keenTalks=talking && (part->stage==SP_LOCK || part->stage==SP_HOME);
		int pose=keenTalks?1+(t/14)%2:(t%300>285?1:0);
		CD_Draw(&scene->keen[pose],kx,56,256,t);
		break;
	}
	}
	if(part->lead && t<part->lead) CD_Subtitle(NULL,NULL,0);
	else CD_Subtitle(part->name,part->text,part->color);
}

static void CD_Shot(const char *name)
{
	const char *folder=getenv("KW_AUTOCUTSCENE");if(!folder) return;
	char path[600];snprintf(path,sizeof(path),"%s/%s.ppm",folder,name);
	void KW_DumpScreenTo(const char *path);
	KW_DumpScreenTo(path);
}

// true: the whole scene is to be skipped
static bool CD_PlayPart(CD_Scene *scene,const CD_Part *part)
{
	bool automatic=getenv("KW_AUTOCUTSCENE")!=NULL;
	KW_Presentation controls;KW_BeginPresentation(&controls);
	uint32_t start=SD_GetTimeCount();
	bool voiced=false,said=part->clip==NULL;
	uint32_t spokeAt=0,reading=70+(uint32_t)strlen(part->text)*3;
	if(automatic) {
		if(part->clip) KW_Narrate(part->clip);
		static const char *names[]={"1-warn","2-again","3-beam","4-sand","5-rise","6-copy","7-off",
			"s1-orbit","s2-land","s3-spot","s4-lock","s5-home","s6-snacks","s7-depart"};
		char name[32];
		CD_Frame(scene,part,part->lead/2+20,false);VH_UpdateScreen();
		snprintf(name,sizeof(name),"%sa",names[part->stage]);CD_Shot(name);
		CD_Frame(scene,part,part->lead+(part->tics?part->tics*2/3:60),part->clip!=NULL);VH_UpdateScreen();
		snprintf(name,sizeof(name),"%sb",names[part->stage]);CD_Shot(name);
		KW_EndPresentation(&controls);
		return false;
	}
	for(;;) {
		uint32_t elapsed=SD_GetTimeCount()-start;
		if(!said && elapsed>=part->lead) {
			voiced=KW_Narrate(part->clip);said=true;spokeAt=elapsed;
		}
		bool talking=said && part->clip && (voiced?KW_ClipPlaying():elapsed-spokeAt<reading/2);
		CD_Frame(scene,part,elapsed,talking);
		VH_UpdateScreen();
		if(KW_PollPresentation(&controls,part->clip!=NULL && said)) break;
		CD_Hint(&controls,part->clip!=NULL && said);
		if(!part->clip) {if(elapsed>=part->tics) break;}
		else if(said && (voiced?!KW_ClipPlaying() && elapsed-spokeAt>35:elapsed-spokeAt>=reading)) break;
		VL_DelayTics(2);
	}
	bool skipped=controls.accepted && controls.holding;
	KW_StopVoice();KW_EndPresentation(&controls);
	return skipped;
}

static void CD_Play(const CD_Part *parts,int count)
{
	CD_Scene scene={0};
	KW_LinesOver();
	VL_FadeToBlack();VL_SetScrollCoords(0,0);VL_SetDefaultPalette();
	CA_UpLevel();
	CA_CacheGrChunk(CK_CHUNKNUM(FON_MAINFONT));
	for(int i=0;i<count;++i) switch(parts[i].stage) {
	case CD_SAND: if(!scene.sand) scene.sand=CD_DrawSand(); break;
	case SP_ORBIT: case SP_DEPART: if(!scene.xax) scene.xax=CD_DrawOrbit(cd_xax); break;
	case CD_WARN: case CD_AGAIN: case CD_BEAM: case CD_RISE: case CD_COPY: case CD_OFF:
		if(!scene.orbit) scene.orbit=CD_DrawOrbit(cd_calidune);
		break;
	default: if(!scene.street) scene.street=CD_DrawStreet(); break;
	}
	for(int i=0;i<3;++i) scene.keen[i]=CD_Capture(AL_KeenPose(i),AL_SCENE_KEEN_W,AL_SCENE_KEEN_H,NULL,false);
	scene.spot=CD_Capture(AL_SpotPicture(),AL_SCENE_SPOT_W,AL_SCENE_SPOT_H,NULL,false);
	// The copy: the same ship in strange colours (green for red, purple for blue)
	static const uint8_t strange[16]={0,5,2,3,2,13,6,7,8,13,10,11,10,13,14,15};
	scene.ship=CD_Capture(AL_ShipPicture(),AL_SCENE_SHIP_W,AL_SCENE_SHIP_H,NULL,false);
	scene.copy=CD_Capture(AL_ShipPicture(),AL_SCENE_SHIP_W,AL_SCENE_SHIP_H,strange,true);
	bool skipped=false;
	for(int i=0;i<count && !skipped;++i) {
		CD_Stage st=parts[i].stage;
		bool cut=i==0 || st==CD_SAND || st==SP_LAND || st==SP_DEPART;
		if(cut) {
			if(i) VL_FadeToBlack();
			CD_Frame(&scene,&parts[i],0,false);VH_UpdateScreen();VL_FadeFromBlack();
		}
		skipped=CD_PlayPart(&scene,&parts[i]);
	}
	VL_FadeToBlack();KW_LinesOver();
	void *surfaces[]={scene.orbit,scene.xax,scene.sand,scene.street};
	for(int i=0;i<4;++i) if(surfaces[i]) VL_DestroySurface(surfaces[i]);
	for(int i=0;i<3;++i) CD_Free(&scene.keen[i]);
	CD_Free(&scene.spot);CD_Free(&scene.ship);CD_Free(&scene.copy);
	CA_DownLevel();
	IN_ClearKeysDown();
	printf("Story scene %s\n",skipped?"skipped":"finished");fflush(stdout);
}

void CD_PlayArrival(void)
{
	printf("Calidune: arrival scene\n");fflush(stdout);
	CD_Play(cd_arrival,(int)(sizeof(cd_arrival)/sizeof(cd_arrival[0])));
}

void CD_PlayEnding(void)
{
	printf("Calidune: ending scene\n");fflush(stdout);
	KW_StopVoice();
	KW_StartCaliduneMusic(KW_VoicesEnabled() && KW_VoiceVolume()?64:256);
	CD_Play(cd_ending,(int)(sizeof(cd_ending)/sizeof(cd_ending[0])));
	SD_MusicOff();
	SD_FadeAdLibVolume(256,0);
}

void SP_PlayRescue(void)
{
	printf("Spot's rescue: scene\n");fflush(stdout);
	KW_StopVoice();
	KW_StartEndingMusic(KW_VoicesEnabled() && KW_VoiceVolume()?48:256);
	CD_Play(sp_rescue,(int)(sizeof(sp_rescue)/sizeof(sp_rescue[0])));
	SD_MusicOff();
	SD_FadeAdLibVolume(256,0);
}
