/* Keen Worlds: story scenes of its own, between the games. GPL-2.0-or-later.
 * Calidune's arrival and ending, Spot's rescue on Fribbulus Xax, Krisle's
 * new ship, the bounty on Meurglys III, the messages after the Alphamatic
 * and the Shikadi signal before Gnosticus IV.
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
// The pictures behind the scenes, drawn when a scene needs them.
typedef enum CD_Background { BG_CALIDUNE, BG_XAX, BG_MARS, BG_SAND, BG_STREET, BG_KRISLE, BG_COCKPIT, BG_COUNT } CD_Background;
typedef struct CD_Scene { void *bg[BG_COUNT]; CD_Image keen[3],spot,ship,copy,knockoff; } CD_Scene;

typedef enum CD_Stage { CD_WARN, CD_AGAIN, CD_BEAM, CD_SAND, CD_RISE, CD_COPY, CD_OFF,
	SP_ORBIT, SP_LAND, SP_SPOT1, SP_LOCK, SP_HOME, SP_SPOT2, SP_DEPART,
	KR_ELDER, KR_SAME, KR_SEATS, KR_FLY, KR_MOSTLY,
	BH_NOTICE, BH_IN, BH_KNOW, BH_MYSTERY,
	AB_MESSAGES, AB_WORTH, AB_SELL, AB_HOME,
	K4_SIGNAL, K4_COUNCIL, K4_WIRES, CD_NUM_STAGES } CD_Stage;
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

// Krisle, after Atroxian Realm's ending: the Krile could not mend the
// Megarocket and bought a knock-off (the game's own ending says so).
static const CD_Part kr_ending[] = {
	{KR_ELDER,"Elder Krislen","Your ship could not be mended. So we ordered you a new one, from Sklargifax Space Industries.",
		"KR_CUT_ELDER.RAW",13,0,0},
	{KR_SAME,"Keen","It's... almost the same.","KR_CUT_SAME.RAW",14,0,200},
	{KR_SEATS,"Megarocket","I have been moved into a cheaper ship. The seats are plastic.","KR_CUT_SEATS.RAW",11,0,0},
	{KR_FLY,"Keen","Does it fly?","KR_CUT_FLY.RAW",14,0,0},
	{KR_MOSTLY,"Megarocket","Mostly.","KR_CUT_MOSTLY.RAW",11,0,0},
};
// Meurglys III, the first time: why Keen is hunting a bounty. The reward
// stays a mystery; the game's ending gives it.
static const CD_Part bh_opening[] = {
	{BH_NOTICE,"Megarocket","Bounty notice from Meurglys III. Wanted: Rick Uller. Reward: a mystery prize.","BH_CUT_NOTICE.RAW",11,0,90},
	{BH_IN,"Keen","A mystery prize? I'm in.","BH_CUT_IN.RAW",14,0,0},
	{BH_KNOW,"Megarocket","You do not even know what it is.","BH_CUT_KNOW.RAW",11,0,0},
	{BH_MYSTERY,"Keen","That's what makes it a mystery!","BH_CUT_MYSTERY.RAW",14,0,0},
};
// After the Alphamatic's cliffhanger: the summons, the bounty, and home
// first (where Spot is taken: Far Shadowlands opens there).
static const CD_Part ab_bridge[] = {
	{AB_MESSAGES,"Megarocket","Two messages. The council of Gnosticus IV want a word with you. "
		"And VitaCorp has put a price on your head. One billion vitas.","AB_CUT_MESSAGES.RAW",11,0,0},
	{AB_WORTH,"Keen","A billion? I'm worth way more than that.","AB_CUT_WORTH.RAW",14,0,0},
	{AB_SELL,"Megarocket","I will not be selling you.","AB_CUT_SELL.RAW",11,0,0},
	{AB_HOME,"Keen","Let's stop at home first. Spot's waiting in the yard.","AB_CUT_HOME.RAW",14,0,0},
};
// Gnosticus IV, the first time: Keen 4 opens at home with his radio; in
// Keen Worlds he is already in the ship, so the signal comes in there.
static const CD_Part k4_opening[] = {
	{K4_SIGNAL,"Megarocket","Incoming signal. Shikadi frequency. Very rude.","K4_CUT_SIGNAL.RAW",11,0,50},
	{K4_COUNCIL,"Keen","They're going after the Gnosticene Council! Set a course for Gnosticus IV.","K4_CUT_COUNCIL.RAW",14,0,0},
	{K4_WIRES,"Megarocket","Course set. Please stop poking my wires.","K4_CUT_WIRES.RAW",11,0,0},
};

static CD_Background CD_BackgroundOf(CD_Stage stage)
{
	switch(stage) {
	case CD_SAND: return BG_SAND;
	case SP_ORBIT: case SP_DEPART: return BG_XAX;
	case SP_LAND: case SP_SPOT1: case SP_LOCK: case SP_HOME: case SP_SPOT2: return BG_STREET;
	case KR_ELDER: case KR_SAME: case KR_SEATS: case KR_FLY: case KR_MOSTLY: return BG_KRISLE;
	case AB_MESSAGES: case AB_WORTH: case AB_SELL: case AB_HOME: return BG_MARS;
	case BH_NOTICE: case BH_IN: case BH_KNOW: case BH_MYSTERY: case K4_SIGNAL: case K4_COUNCIL: case K4_WIRES: return BG_COCKPIT;
	default: return BG_CALIDUNE;
	}
}

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
static const uint8_t cd_calidune[6]={14,6,4,0,12,4},cd_xax[6]={10,2,3,1,11,9},cd_mars[6]={12,4,6,0,12,4};
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

// Krisle: a violet sky with two suns, the Great Ziggurat, blue-grey rock.
static void *CD_DrawKrisle(void)
{
	void *surface=VL_CreateSurface(320,200),*old=VL_SetScreen(surface);
	VL_ClearScreen(0);
	for(int y=0;y<144;++y) for(int x=0;x<320;++x) {
		bool dither=(x+y)&1;
		int color=y<30?(y>20&&dither?5:0):y<60?(y<40&&dither?0:5):y<92?(y<70&&dither?5:13):13;
		int ax=x-58,ay=y-30,bx=x-96,by=y-48;
		if(ax*ax+ay*ay<=11*11) color=15;
		else if(ax*ax+ay*ay<=15*15 && dither) color=14;
		if(bx*bx+by*by<=6*6) color=12;
		// the Great Ziggurat, in steps
		int zx=abs(x-236);
		for(int step=0;step<5;++step)
			if(zx<=52-step*10 && y>=96-step*9 && y<105-step*9) color=(x<236)?(dither?6:4):6;
		if(zx<=3 && y>=48 && y<60) color=14; // its light
		// rock, with stones
		if(y>=100) {
			color=y<104?(dither?3:8):(dither&&y<116?8:3);
			uint32_t n=CD_Noise(x/6,y/4,11);
			if(n%23==0) color=8;
			if(n%41==1) color=11;
		}
		VH_Plot(x,y,color);
	}
	VL_SetScreen(old);
	return surface;
}

// Inside the Megarocket: stars through the window, a console, a big screen.
static void *CD_DrawCockpit(void)
{
	void *surface=VL_CreateSurface(320,200),*old=VL_SetScreen(surface);
	VL_ClearScreen(0);
	for(int y=0;y<144;++y) for(int x=0;x<320;++x) {
		int color=(y%24==0 || x%48==0)?8:0;              // hull plates
		if((y%24==4 || y%24==20) && (x%48==4 || x%48==44)) color=7; // rivets
		if(y>=6 && y<30 && x>=8 && x<312) {            // the window
			uint32_t n=CD_Noise(x,y,21);
			color=n%61==0?15:n%61==1?7:0;
		}
		if((y==5||y==30) && x>=7 && x<313) color=7;
		if(y>=36 && y<114 && x>=136 && x<310) color=(y==36||y==113||x==136||x==309)?7:(y<38||x<138)?15:0; // the screen
		if(y>=114) color=y==114?7:(y<120?8:(((x/4)&1)&&y>124?8:0)); // the console
		VH_Plot(x,y,color);
	}
	static const int lights[]={12,10,14,9,11,13,10,12};
	for(int i=0;i<8;++i) VH_Bar(150+i*18,124,6,4,lights[i]);
	VL_SetScreen(old);
	return surface;
}

// The knock-off: the Megarocket in Sklargifax brown and yellow, a little
// lopsided (each column sits a bit lower than the one before).
static void CD_DrawLopsided(const CD_Image *image,int x,int y)
{
	for(int dx=0;dx<image->w;++dx) {
		int shift=dx/16;
		for(int dy=0;dy<image->h;++dy) {
			int i=dy*image->w+dx,px=x+dx,py=y+dy+shift;
			if(image->mask[i] && px>=0 && px<320 && py>=0 && py<144) VH_Plot(px,py,image->pixels[i]);
		}
	}
}

// A line of the screen's lettering, centred on it.
static void CD_ScreenText(const char *text,int y,int color)
{
	uint16_t w,h;VH_MeasurePropString(text,&w,&h,0);
	VH_DrawPropStringSolid(text,223-w/2,y,0,color);
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
		VL_SurfaceToScreen(scene->bg[BG_CALIDUNE],0,0,0,0,320,200);
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
		VL_SurfaceToScreen(scene->bg[BG_SAND],0,0,0,0,320,200);
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
		VL_SurfaceToScreen(scene->bg[BG_CALIDUNE],0,0,0,0,320,200);
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
		VL_SurfaceToScreen(scene->bg[BG_XAX],0,0,0,0,320,200);
		int x,y;
		if(part->stage==SP_ORBIT) {x=CD_Ease(t,500,-70,150);y=CD_Ease(t,500,8,92);}
		else {x=CD_Ease(t,500,150,330);y=CD_Ease(t,500,92,-30);}
		int length=5+(t/4)%5;
		VH_HLine(x-length,x-1,y+17,part->stage==SP_ORBIT?9:12);VH_HLine(x-length/2,x-1,y+18,part->stage==SP_ORBIT?11:14);
		CD_Draw(&scene->ship,x,y,256,t);
		break;
	}
	case SP_LAND: case SP_SPOT1: case SP_LOCK: case SP_HOME: case SP_SPOT2: { // the street in Bloogton
		VL_SurfaceToScreen(scene->bg[BG_STREET],0,0,0,0,320,200);
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
	case KR_ELDER: case KR_SAME: case KR_SEATS: case KR_FLY: case KR_MOSTLY: {
		VL_SurfaceToScreen(scene->bg[BG_KRISLE],0,0,0,0,320,200);
		// the knock-off comes down and lands, wobbling, beside Keen
		if(part->stage!=KR_ELDER) {
			int y=part->stage==KR_SAME?CD_Ease(t,part->lead,-40,90):90;
			int wobble=part->stage==KR_SAME && t<part->lead?(int)((t/7)%3)-1:0;
			if(part->stage==KR_SAME && t<part->lead) {
				int length=4+(t/3)%5;
				VH_VLine(y+28,y+28+length,180+wobble+12,12);VH_VLine(y+28,y+28+length/2,180+wobble+48,14);
			}
			CD_DrawLopsided(&scene->knockoff,180+wobble,y);
			if((part->stage==KR_SEATS || part->stage==KR_MOSTLY) && talking && (t/16)%2) VH_Bar(224,y+12,2,2,10);
		}
		bool keenTalks=talking && part->color==14;
		int pose=keenTalks?1+(t/14)%2:(t%300>285?1:0);
		CD_Draw(&scene->keen[pose],96,76,256,t);
		break;
	}
	case BH_NOTICE: case BH_IN: case BH_KNOW: case BH_MYSTERY: case K4_SIGNAL: case K4_COUNCIL: case K4_WIRES: {
		VL_SurfaceToScreen(scene->bg[BG_COCKPIT],0,0,0,0,320,200);
		bool bounty=part->stage<=BH_MYSTERY;
		if(bounty) {
			// the wanted poster, scrolling onto the screen from the top
			int shown=part->stage==BH_NOTICE?CD_Ease(t,part->lead,0,74):74;
			VH_Bar(140,39,166,shown,14);
			if(shown>=74) {
				VH_Bar(142,41,162,2,6);VH_Bar(142,109,162,2,6);
				CD_ScreenText("WANTED",44,4);
				for(int dy=-12;dy<=12;++dy) for(int dx=-10;dx<=10;++dx)
					if(dx*dx*144+dy*dy*100<=14400) VH_Plot(223+dx,73+dy,dy>6&&abs(dx)>6?14:8);
				CD_ScreenText("?",68,15);
				CD_ScreenText("RICK ULLER",88,0);
				CD_ScreenText("REWARD: ???",98,(t/20)%2?4:6);
			}
		} else {
			// the Shikadi signal: static, then a wave
			for(int x=140;x<306;++x) {
				int wave=(int)(18*sin(x/7.0+t/5.0)*sin(x/31.0)+(int)(CD_Noise(x,(int)t,13)%7)-3);
				for(int dy=-1;dy<=1;++dy) VH_Plot(x,76+wave+dy,dy?2:10);
			}
			if((t/24)%2) CD_ScreenText("SHIKADI FREQUENCY",42,12);
			CD_ScreenText("PHOTACHYON TRANSCEIVER",100,7);
			// the transceiver on the console, and its sparks
			VH_Bar(92,104,30,12,8);VH_HLine(92,121,104,7);VH_VLine(84,104,118,7);VH_Plot(118,83,12);
			VH_Bar(96,108,4,4,(t/10)%2?10:2);VH_Bar(104,108,4,4,14);
			if(part->stage==K4_WIRES && (t/6)%3==0) CD_Sparkle(88,98,38,16,8,t);
		}
		bool keenTalks=talking && part->color==14;
		int pose=keenTalks?1+(t/14)%2:(t%300>285?1:0);
		CD_Draw(&scene->keen[pose],34,78,256,t);
		break;
	}
	case AB_MESSAGES: case AB_WORTH: case AB_SELL: case AB_HOME: {
		VL_SurfaceToScreen(scene->bg[BG_MARS],0,0,0,0,320,200);
		int x=part->stage==AB_HOME?CD_Ease(t,400,124,340):124,y=48+(int)((t/24)%2);
		if(part->stage==AB_HOME) {
			int length=5+(t/4)%5;
			VH_HLine(x-length,x-1,y+17,9);VH_HLine(x-length/2,x-1,y+18,11);
		}
		CD_Draw(&scene->ship,x,y,256,t);
		if(part->stage==AB_MESSAGES) {
			// the two messages, one after the other
			VH_Bar(8,6,124,46,0);VH_HLine(8,131,6,11);VH_HLine(8,131,51,11);VH_VLine(6,51,8,11);VH_VLine(6,51,131,11);
			VH_DrawPropStringSolid("1: GNOSTICUS IV",14,10,0,10);
			VH_DrawPropStringSolid("The council calls",14,20,0,7);
			if(t>220) {
				VH_DrawPropStringSolid("2: WANTED - KEEN",14,31,0,(t/18)%2?12:4);
				VH_DrawPropStringSolid("1,000,000,000 vitas",14,41,0,14);
			}
		}
		if(part->color==14) CD_Inset(scene,260,talking?1+(t/12)%2:0,256,talking,t);
		break;
	}
	default:
		break;
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
			"s1-orbit","s2-land","s3-spot","s4-lock","s5-home","s6-snacks","s7-depart",
			"k1-elder","k2-same","k3-seats","k4-fly","k5-mostly",
			"b1-notice","b2-in","b3-know","b4-mystery",
			"a1-messages","a2-worth","a3-sell","a4-home",
			"g1-signal","g2-council","g3-wires"};
		typedef char cd_everyStageNamed[sizeof(names)/sizeof(names[0])==CD_NUM_STAGES?1:-1];
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
	for(int i=0;i<count;++i) {
		CD_Background bg=CD_BackgroundOf(parts[i].stage);
		if(scene.bg[bg]) continue;
		switch(bg) {
		case BG_CALIDUNE: scene.bg[bg]=CD_DrawOrbit(cd_calidune); break;
		case BG_XAX: scene.bg[bg]=CD_DrawOrbit(cd_xax); break;
		case BG_MARS: scene.bg[bg]=CD_DrawOrbit(cd_mars); break;
		case BG_SAND: scene.bg[bg]=CD_DrawSand(); break;
		case BG_STREET: scene.bg[bg]=CD_DrawStreet(); break;
		case BG_KRISLE: scene.bg[bg]=CD_DrawKrisle(); break;
		default: scene.bg[bg]=CD_DrawCockpit(); break;
		}
	}
	for(int i=0;i<3;++i) scene.keen[i]=CD_Capture(AL_KeenPose(i),AL_SCENE_KEEN_W,AL_SCENE_KEEN_H,NULL,false);
	scene.spot=CD_Capture(AL_SpotPicture(),AL_SCENE_SPOT_W,AL_SCENE_SPOT_H,NULL,false);
	// The copy: the same ship in strange colours (green for red, purple for blue)
	static const uint8_t strange[16]={0,5,2,3,2,13,6,7,8,13,10,11,10,13,14,15};
	scene.ship=CD_Capture(AL_ShipPicture(),AL_SCENE_SHIP_W,AL_SCENE_SHIP_H,NULL,false);
	scene.copy=CD_Capture(AL_ShipPicture(),AL_SCENE_SHIP_W,AL_SCENE_SHIP_H,strange,true);
	// The knock-off: Sklargifax yellow and brown for the white, blue and red
	static const uint8_t sklargifax[16]={0,6,2,3,6,6,6,6,8,6,10,11,6,13,14,14};
	scene.knockoff=CD_Capture(AL_ShipPicture(),AL_SCENE_SHIP_W,AL_SCENE_SHIP_H,sklargifax,false);
	bool skipped=false;
	for(int i=0;i<count && !skipped;++i) {
		CD_Stage st=parts[i].stage;
		bool cut=i==0 || CD_BackgroundOf(st)!=CD_BackgroundOf(parts[i-1].stage);
		if(cut) {
			if(i) VL_FadeToBlack();
			CD_Frame(&scene,&parts[i],0,false);VH_UpdateScreen();VL_FadeFromBlack();
		}
		skipped=CD_PlayPart(&scene,&parts[i]);
	}
	VL_FadeToBlack();KW_LinesOver();
	for(int i=0;i<BG_COUNT;++i) if(scene.bg[i]) VL_DestroySurface(scene.bg[i]);
	for(int i=0;i<3;++i) CD_Free(&scene.keen[i]);
	CD_Free(&scene.spot);CD_Free(&scene.ship);CD_Free(&scene.copy);CD_Free(&scene.knockoff);
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

// The calm tune of Keen Worlds' ending, quietly under the voices.
static void CD_PlayWithTune(const char *what,const CD_Part *parts,int count)
{
	printf("%s: scene\n",what);fflush(stdout);
	KW_StopVoice();
	KW_StartEndingMusic(KW_VoicesEnabled() && KW_VoiceVolume()?48:256);
	CD_Play(parts,count);
	SD_MusicOff();
	SD_FadeAdLibVolume(256,0);
}

void KR_PlayNewShip(void) {CD_PlayWithTune("Krisle's new ship",kr_ending,(int)(sizeof(kr_ending)/sizeof(kr_ending[0])));}
void BH_PlayBountyNotice(void) {CD_PlayWithTune("The bounty notice",bh_opening,(int)(sizeof(bh_opening)/sizeof(bh_opening[0])));}
void AB_PlayMessages(void) {CD_PlayWithTune("After the Alphamatic",ab_bridge,(int)(sizeof(ab_bridge)/sizeof(ab_bridge[0])));}
void K4_PlayShikadiSignal(void) {CD_PlayWithTune("The Shikadi signal",k4_opening,(int)(sizeof(k4_opening)/sizeof(k4_opening[0])));}
