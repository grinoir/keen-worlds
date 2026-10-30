/* Keen Dreams Gold Edition Source Code
 * Copyright (C) 2026 Szemigi
 * This project contains taken and modified
 * codes/codings from these following sources:
 *
 * 1.
 * Reconstructed BioMenace 1
 * Copyright (C) 2024 lethal_guitar
 * 
 * 2.
 * Foray in the Forest
 * Copyright (C) 2022-2024 Nisaba
 * Copyright (C) 2019-2025 K1n9_Duk3
 *
 * 3.
 * Atroxian Realm
 * Copyright (C) 2020 Gridlock
 *
 * 4.
 * Reconstructed Commander Keen 4-6
 * Copyright (C) 2021 K1n9_Duk3
 *
 * 5.
 * Keen Dreams Unofficial Tech Demo
 * Copyright (C) 2015 NY00123
 *
 * 6.
 * Wolfenstein 3-D Source Code
 * Copyright (C) 1995 Id Software
 *
 * 7.
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * 8.
 * Catacomb 3-D Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// KDG_DEF.H

#include "id_heads.h"


/*
=============================================================================

						GLOBAL CONSTANTS

=============================================================================
*/

//#define CREDITS 0

#define	MAXACTORS	MAXSPRITES

#define ACCGRAVITY	3
#define SPDMAXY		80

#define BLOCKSIZE	(8*PIXGLOBAL)		// for positioning starting actors

//
// movement scrolling edges
//
#define SCROLLEAST (TILEGLOBAL*11)
#define SCROLLWEST (TILEGLOBAL*9)
#define SCROLLSOUTH (TILEGLOBAL*8)
#define SCROLLNORTH (TILEGLOBAL*4)

#define CLIPMOVE	24					// speed to push out of a solid wall

#define GAMELEVELS	48
#define GAMELEVELSWITHOUTTITLE	47

void CA_FreeGraphics(void);

#define CONVERT_GLOBAL_TO_TILE(x)  ((x)>>(G_T_SHIFT))
#define CONVERT_PIXEL_TO_GLOBAL(x) ((x)<<(G_P_SHIFT))
#define CONVERT_TILE_TO_GLOBAL(x)  ((x)<<(G_T_SHIFT))

#define SPAWN_ADJUST_Y(y, h) (CONVERT_TILE_TO_GLOBAL(y) + (CONVERT_PIXEL_TO_GLOBAL(16-(h))))
#define PLATFORMEDGE 23	//used as hitnorth or hitsouth value when hitting a blocking object

#define PURGE_FIRST 3
#define MINMEMORY 409l*1024

//
// Note (Szemigi): 1024 is only testing to make sure the 'outofmem' screen appears correctly as expected
//
//#define MINMEMORY 1024l*1024


/*
=============================================================================

							 TYPES

=============================================================================
*/

typedef	enum	{notdone,resetgame,endgame,levelcomplete,warptolevel,died,victorious,nightvictorious,mortendgame}
				exittype;

typedef	enum	{nothing,keenobj,mortobj,mortdeadobj,powerobj,doorobj,metaldoorobj,keenbombobj,gargoylefaceobj,gargoyleobj,pumpkinobj,mosquitoobj,beanobj,baberryobj,
	bonusobj,keyobj,cauliobj,rhubobj,parsobj,turnipobj,horseobj,broccoobj,tomatobj,carrotobj,asparobj,grapeobj,eggplantobj,pinkowl3obj,guardrobotobj,keycardobj,
	taterobj,cartobj,mcartobj,frenchyobj,melonobj,celeryobj,machineobj,mushroomobj,boobusbombobj,cameo1obj,cameo2obj,exitobj,roverobj,kumquatobj,cornobj,
	brusselobj,squashobj,apelobj,peapodobj,peabrainobj,boobusobj,platformobj,stunshotobj,stunnedobj,beetobj,pinkowlobj,pinkowl2obj,kidobj,steelcartobj,
	zeeobj,pooffromobj,shotobj,shot2obj,inertobj}	classtype;

typedef struct
{
  int16_t 		leftshapenum,rightshapenum;
  enum		{step,slide,think,stepthink,slidethink} progress;
  boolean	skippable;

  boolean	pushtofloor;
  int16_t tictime;
  int16_t xmove;
  int16_t ymove;
  void (*think) ();
//  void (*contact) ();
  void (*contact) (struct objstruct*, struct objstruct*);
  void (*react) ();
  void near *nextstate;
} statetype;


typedef	struct
{
	uint16_t	worldx,worldy;
	uint32_t	score,nextextra;				// K1n9_Duk3's fix
	// Note (ktx58): Try to preserve saved game compatibility after
	// "Hard" level modifications. The intro level can never be
	// completed so the space for it in "leveldone" can be re-used.
	byte		leveldone[GAMELEVELSWITHOUTTITLE];
	byte		damagetime;
	byte		flowerpowers;
	byte		ammo;
	byte		boobusbombs,bombsthislevel;
	byte		keys;
	byte		keycard;
	byte		text_keycard;
	byte		mapon;
	signed  char		lives;
	byte		difficulty;
	boolean	text_mort;
} gametype;


typedef struct	objstruct
{
	classtype	obclass;
	enum		{no,yes,allways,removable} active;
	boolean		needtoreact;
	enum {cl_noclip, cl_midclip, cl_fullclip} needtoclip;
	uint16_t	nothink;
	uint16_t	x,y;

	int16_t			xdir,ydir;
	int16_t			xmove,ymove;
	int16_t			xspeed,yspeed;

	int16_t			ticcount,ticadjust;
	statetype	*state;

	uint16_t	shapenum;
	int16_t 		priority;

	uint16_t	left,top,right,bottom;	// hit rectangle
	uint16_t	midx;
	uint16_t	tileleft,tiletop,tileright,tilebottom;	// hit rect in tiles
	uint16_t	tilemidx;

	int16_t			hitnorth,hiteast,hitsouth,hitwest;	// wall numbers contacted

	int16_t			temp1,temp2,temp3,temp4;

	void		*sprite;

	struct	objstruct	*next,*prev;
} objtype;

typedef enum
{
	INTILE_FOREGROUND = 0x80
} intiletype;

#define INTILE_TYPEMASK (INTILE_FOREGROUND-1)


/*
=============================================================================

					 KDG_MAIN.C DEFINITIONS

=============================================================================
*/

extern	char	str[80],str2[20];
extern	int extravbls; // Imported from Catacomb 3-D
extern  boolean storedemo;
extern	boolean	singlestep,jumpcheat,godmode,tedlevel,nightmode,mortwindow,mortimerbedgfx,greenball;
extern	word	tedlevelnum;

void	DebugMemory (void);
void	TestSprites(void);
int		DebugKeys (void);
void	StartupId (void);
void	ShutdownId (void);
void	Quit (char *error);
void	InitGame (void);
int	main (int argc, char **argv);

// Taken from Foray in the Forest for the proper implementing of WallDebug

#define CA_UnmarkGrChunk(chunk)	grneeded[chunk] &= ~ca_levelbit
#define CA_UncacheGrChunk(chunk)	{CA_UnmarkGrChunk(chunk); if (grsegs[chunk]) MM_SetPurge(&grsegs[chunk], true);}

/*
=============================================================================

					  KDG_DEMO.C DEFINITIONS

=============================================================================
*/

extern boolean scorescreenkludge;

void	Finale (void);
void	GameOver (void);
void	AltDemoLoop (void);
void	DemoLoop (void);
void 	RunDemo(int num);
void 	CheckLastScan(void);
void 	ShowHighScores(void);
void	StatusWindow (void);
void	NewGame (void);
void	TEDDeath (void);
void 	StartMusic(int num);

// Rename these to names from Catacomb 3-D (fix conflict with SaveGame struct)
boolean	LoadTheGame/*LoadGame*/ (int file);
boolean	SaveTheGame/*SaveGame*/ (int file);
void	ResetGame (void);

// *** Music ***
void 	StartMusic (int num);
void 	StopMusic (void);

/*
=============================================================================

					  KDG_PLAY.C DEFINITIONS

=============================================================================
*/

// K1n9_Duk3 mod 2025-05-27: removed the extern declarations for xtry and ytry
extern	boolean keenkilled;
extern	gametype	gamestate;
extern	exittype	playstate;
extern	boolean		jumpheld,pogoheld,attackheld,jumpbutton,pogobutton,attackbutton,shootbutton,shootscan,debugok;
extern	unsigned	originxtilemax,originytilemax;
extern	objtype		*new,*check,*player,*scoreobj;
extern	objtype		*riding;
extern	objtype		*mortinlevel;
extern	boolean		onmushroom;
// Note (ktx58): On "Hard" difficulty, "plummet" is re-used  as a
// "level must be restarted" (e.g. player has hit the bottom of the level)
// variable.
extern	int			plummet;

extern	ControlInfo	c;

extern	objtype dummyobj;
extern boolean showscorebox;

extern boolean enternightmodecode;
extern boolean entermortimercode;

extern char far *levelnames[GAMELEVELS];
extern char far *levelentries[GAMELEVELS];

void	CheckKeys (void);
void	CalcInactivate (void);
void 	InitObjArray (void);
void	GetNewObj (boolean usedummy);
void	RemoveObj (objtype *gone);
void 	ScanInfoPlane (void);
void 	PatchWorldMap (void);
void 	MarkTileGraphics (void);
void 	FadeAndUnhook (void);
void 	SetupGameLevel (boolean loadnow);
void 	ScrollScreen (objtype *ob);
void 	MoveObjVert (objtype *ob, int ymove);
void 	MoveObjHoriz (objtype *ob, int xmove);
void 	GivePoints (unsigned points);
void 	ClipToEnds (objtype *ob);
void 	ClipToEastWalls (objtype *ob);
void 	ClipToWestWalls (objtype *ob);
void 	ClipToWalls (objtype *ob);
void	ClipToSprite (objtype *push, objtype *solid, boolean squish);
void	ClipToSpriteSide (objtype *push, objtype *solid);
void 	ClipToSpriteTop (objtype *push, objtype *solid);
void 	ClipPushSprite(objtype *push, objtype *solid, boolean squish);
int 	DoActor (objtype *ob,int tics);
void 	StateMachine (objtype *ob);
void 	NewState (objtype *ob,statetype near *state);
void 	PlayLoop (void);
void 	GameLoop (void);
void 	CenterActor(objtype *ob);
void 	NightGameFinale (void);
void 	ScoreThink (objtype *ob);

void StartDemoRecord(void);
void EndDemoRecord(void);

// For a workaround that fixes issues related to object pointers 
// in saved games.

unsigned IndexOfObjectInLinkedList(objtype *objptr);

// Keen Worlds companion: object and state pointers kept in 16-bit fields
int16_t		ObjToInt (objtype *ob);
objtype		*IntToObj (int16_t n);
int16_t		StateToInt (statetype *state);
statetype	*IntToState (int16_t n);
objtype * ObjectAtIndexInLinkedList(unsigned index);
objtype * GetFirstObjectWithClass(classtype obclass);


extern statetype near s_keenduck;
extern statetype near s_keenlookup;

extern statetype near s_keenstand;

//
// curent user input
//

extern	boolean		demorecord,demoplayback;
extern	char		far *demoptr, far *lastdemoptr;
extern	memptr		demobuffer;

extern	boolean	nameenteredforsecret;

void SpawnBomb (int tilex, int tiley);



/*
=============================================================================

					  KDG_KEEN.C DEFINITIONS

=============================================================================
*/

extern  ScanCode        statusscan, attackscan;	// K1n9_Duk3 mod 2025-05-26: replace placingscan with statusscan

extern int bonuspoints[];

extern statetype near s_score;
extern statetype near s_demo;

extern int invincible;

void WalkSound(objtype *ob);

void	CalcSingleGravity (void);

void	ProjectileThink		(objtype *ob);
void	DrawReact			(objtype *ob);

void	SpawnScore (void);
void	SpawnDemo (void);
void	FixScoreBox (void);
void	SpawnWorldKeen (int tilex, int tiley);
void	SpawnKeen (int tilex, int tiley, int dir);

void 	KillKeen (void);

void KeenHoldThink(objtype *ob);
void KeenPullUpThink1(objtype *ob);
void KeenPullUpThink2(objtype *ob);
void KeenPullUpThink3(objtype *ob);
void KeenPulledUpThink(objtype *ob);
void SpawnShot(unsigned x, unsigned y, int dir);
void ExplodeShot(objtype *ob);
void ShotThink(objtype *ob);
void ShotReact(objtype *ob);
void StunnedReact(objtype *ob);
void StunObj(objtype *ob, objtype *shot, statetype near *stunstate);
void ResetScoreBox(void);
void ShotContact (objtype *ob, objtype *hit);

extern	int	singlegravity;
extern	unsigned	bounceangle[8][8];

extern statetype near s_keenholdon;
extern statetype near s_keenholdon2;
extern statetype near s_keenclimbup;
extern statetype near s_keenclimbup2;
extern statetype near s_keenclimbup3;
extern statetype near s_keenclimbup4;
extern statetype near s_keenclimbup5;
extern statetype near s_keenlookup;

extern statetype near s_turnip;
extern statetype near s_turnip2;

extern statetype near s_poofto1;

extern statetype near s_cauli1;
extern statetype near s_cauli2;

extern statetype near s_caulistop;

extern statetype near s_parsnipwalk1;
extern statetype near s_parsnipwalk2;

extern statetype near s_rhubarbwalk3;

extern statetype near s_keenplace;
extern statetype near s_keenlowshoot1;
extern statetype near s_keenlowshoot2;

extern statetype near s_blueflowerpower1;
extern statetype near s_blueflowerpower2;

extern statetype near s_vorthit1;
extern statetype near s_vorthit2;

extern statetype near s_taterdead;
extern statetype near s_broccodead;
extern statetype near s_squasherdead;
extern statetype near s_altsquasherdead;
extern statetype near s_tomatdead;
extern statetype near s_btomatdead;
extern statetype near s_apeldead;
extern statetype near s_frenchydead;
extern statetype near s_peapoddead;
extern statetype near s_peabraindead;
extern statetype near s_taterfemaledead;
extern statetype near s_carrotdead;
extern statetype near s_aspardead;

extern statetype near s_flowerpower1;
extern statetype near s_flowerpower2;
extern statetype near s_flowerpower3;
extern statetype near s_flowerpower4;
extern statetype near s_flowerpower5;
extern statetype near s_flowerpower6;

extern statetype near s_boobusbomb1;
extern statetype near s_boobusbomb2;

extern statetype near s_boobusbombthrown1;
extern statetype near s_boobusbombthrown2;

extern statetype near s_bombexplode;
extern statetype near s_bombexplode2;
extern statetype near s_bombexplode3;
extern statetype near s_bombexplode4;
extern statetype near s_bombexplode5;
extern statetype near s_bombexplode6;

extern statetype near s_powerblink1;
extern statetype near s_powerblink2;
extern statetype near s_powerblink3;
extern statetype near s_powerblink4;

extern statetype near s_worldkeen;

extern statetype near s_worldkeenwave1;
extern statetype near s_worldkeenwave2;
extern statetype near s_worldkeenwave3;
extern statetype near s_worldkeenwave4;
extern statetype near s_worldkeenwave5;

extern statetype near s_worldkeenwait;

extern statetype near s_worldkeensleep1;
extern statetype near s_worldkeensleep2;

extern statetype near s_worldwalk;

extern statetype near s_keenpogodown;
extern statetype near s_keenpogo;
extern statetype near s_keenpogo2;

extern statetype near s_keenlosehealth;

extern statetype near s_keenzee1;
extern statetype near s_keenzee2;
extern statetype near s_keenzee3;

extern statetype near s_keenstand2;

extern statetype near s_keenstand;
extern statetype near s_keenpauselook;
extern statetype near s_keenyawn1;
extern statetype near s_keenyawn2;
extern statetype near s_keenyawn3;
extern statetype near s_keenyawn4;

extern statetype near s_keenwait1;
extern statetype near s_keenwait2;
extern statetype near s_keenwait3;
extern statetype near s_keenwait4;
extern statetype near s_keenwait5;
extern statetype near s_keenwait6;

extern statetype near s_keengosleep1;
extern statetype near s_keengosleep2;
extern statetype near s_keensleep1;
extern statetype near s_keensleep2;

extern statetype near s_keendie1;
extern statetype near s_keendie2;
extern statetype near s_keendie3;

extern statetype near s_keendead;

extern statetype near s_keenturn;

extern statetype near s_keengetup2;

extern statetype near s_keenlookup;
extern statetype near s_keenduck;
extern statetype near s_keenpickup;
extern statetype near s_keendrop;

extern statetype near s_keenreach;

extern statetype near s_keenpole;

extern statetype near s_keenclimb1;
extern statetype near s_keenclimb2;
extern statetype near s_keenclimb3;

extern statetype near s_keenslide1;
extern statetype near s_keenslide2;
extern statetype near s_keenslide3;
extern statetype near s_keenslide4;

extern statetype near s_keenpolethrow1;
extern statetype near s_keenpolethrow2;
extern statetype near s_keenpolethrow3;

extern statetype near s_keenpolethrowup1;
extern statetype near s_keenpolethrowup2;
extern statetype near s_keenpolethrowup3;

extern statetype near s_keenpolethrowdown1;
extern statetype near s_keenpolethrowdown2;
extern statetype near s_keenpolethrowdown3;

extern statetype near s_keenwalk1;
extern statetype near s_keenwalk2;
extern statetype near s_keenwalk3;
extern statetype near s_keenwalk4;

extern statetype near s_keenthrow1;
extern statetype near s_keenthrow2;
extern statetype near s_keenthrow3;
extern statetype near s_keenthrow4;

extern statetype near s_keenthrowup1;
extern statetype near s_keenthrowup2;
extern statetype near s_keenthrowup3;

extern statetype near s_keenfell;

extern statetype near s_keenjumpup1;
extern statetype near s_keenjumpup2;
extern statetype near s_keenjumpup3;

extern statetype near s_keenjump1;
extern statetype near s_keenjump2;
extern statetype near s_keenjump3;

extern statetype near s_keenairthrow1;
extern statetype near s_keenairthrow2;
extern statetype near s_keenairthrow3;

extern statetype near s_keenairthrowup1;
extern statetype near s_keenairthrowup2;
extern statetype near s_keenairthrowup3;

extern statetype near s_keenairthrowdown1;
extern statetype near s_keenairthrowdown2;
extern statetype near s_keenairthrowdown3;

extern statetype near s_keenholdon;
extern statetype near s_keenholdon2;
extern statetype near s_keenclimbup;
extern statetype near s_keenclimbup2;
extern statetype near s_keenclimbup3;
extern statetype near s_keenclimbup4;
extern statetype near s_keenclimbup5;

extern statetype near s_keenshoot1;
extern statetype near s_keenshoot2;
extern statetype near s_keenshootup1;
extern statetype near s_keenshootup2;
extern statetype near s_keenpoleshoot1;
extern statetype near s_keenpoleshoot2;
extern statetype near s_keenpoleshootup1;
extern statetype near s_keenpoleshootup2;
extern statetype near s_keenpoleshootdown1;
extern statetype near s_keenpoleshootdown2;
extern statetype near s_keenairshoot1;
extern statetype near s_keenairshoot2;
extern statetype near s_keenairshoot3;
extern statetype near s_keenairshootup1;
extern statetype near s_keenairshootup2;
extern statetype near s_keenairshootup3;
extern statetype near s_keenairshootdown1;
extern statetype near s_keenairshootdown2;
extern statetype near s_keenairshootdown3;

extern statetype near s_keenexit1;
extern statetype near s_keenexit2;
extern statetype near s_keenexit3;
extern statetype near s_keenexit4;
extern statetype near s_keenexit5;

extern statetype near s_vortray1;
extern statetype near s_vortray2;
extern statetype near s_vortray3;
extern statetype near s_vortray4;
extern statetype near s_vortray5;
extern statetype near s_vortray6;
extern statetype near s_vortray7;
extern statetype near s_vortray8;

extern statetype near s_vorthit3;
extern statetype near s_vorthit4;

extern statetype near s_boobusbomb3;
extern statetype near s_boobusbomb4;

extern statetype near s_pumpkin;

/*
=============================================================================

					  KDG_ACT1.C DEFINITIONS

=============================================================================
*/

// Szemigi: DoHighGravity, DoWeakGravity, DoMidGravity, and DoLowGravity
// routines were taken from Nisaba's FITF.

void WalkReact (objtype *ob);
void KeycardThink (objtype *ob);

void 	DoGravity (objtype *ob);
void    DoHighGravity(objtype *ob);
void    DoWeakGravity(objtype *ob);
void    DoMidGravity(objtype *ob);
void    DoLowGravity(objtype *ob);
void	AccelerateX (objtype *ob,int dir,int max);
void 	AccelerateXv(objtype *ob, int dir, int maxspeed);
void 	AccelerateY(objtype *ob, int dir, int maxspeed);
void 	FrictionX (objtype *ob);
void 	PushObj(objtype *ob);

void	ProjectileThink		(objtype *ob);
void	ProjectileReact 	(objtype *ob);
void    WeakProjectileThink	(objtype *ob);
void	VelocityThink		(objtype *ob);
void	DrawReact			(objtype *ob);
void	DrawReact2			(objtype *ob);
void	DrawReact3			(objtype *ob);
void	DrawReact4			(objtype *ob);
void	DrawReact5			(objtype *ob);
void	DrawReact6			(objtype *ob);
void	DrawReact7			(objtype *ob);
void	DrawReact8			(objtype *ob);
void	DrawReact9			(objtype *ob);
void	DrawReact10			(objtype *ob);
void	ChangeState (objtype *ob, statetype near *state);

void 	PumpkinThink (objtype *ob);
void 	PumpkinSitThink (objtype *ob);
void 	PumpkinAirReact (objtype *ob);
void 	PumpkinBounceReact (objtype *ob);

void 	PearThink(objtype *ob);

void	ChangeToFlower (objtype *ob);

void 	MosquitoFlyThink(objtype *ob);
void 	MosquitoFlyContact(objtype *ob, objtype *hit);
void 	MosquitoReact(objtype *ob);

void 	BeanContact(objtype *ob, objtype *hit);

void 	BaberryContact(objtype *ob, objtype *hit);
void 	BaberryReact(objtype *ob);

void 	GrapeThink (objtype *ob);
void 	GrapeRiseReact (objtype *ob);
void 	GrapeFallReact (objtype *ob);

void 	AsparThink (objtype *ob);

void 	CarrotThink (objtype *ob);
void 	CarrotReact (objtype *ob);
void 	CarrotAirReact (objtype *ob);

void 	TomatBounceThink (objtype *ob);
void 	TomatReact (objtype *ob);

void 	BroccoThink (objtype *ob);
void 	BroccoGetUp (objtype *ob);

void 	ParsnipReact (objtype *ob);

void 	MortThink (objtype *ob);
void 	MortJumpReact (objtype *ob);
void 	MortStopThink (objtype *ob);
void 	MortDuckThink (objtype *ob);
void 	MortDeathThink (objtype *ob);
void 	MortWalkReact (objtype *ob);
void 	MortThrow (objtype *ob);
void 	MortAirThrow (objtype *ob);
void 	ThrowBlueFlowerPower (unsigned x, unsigned y, int dir);

void	DoorContact (objtype *ob, objtype *hit);
void 	DrawDoor(objtype *ob);
void 	DrawMetalDoor(objtype *ob);

void	SpawnMort (int tilex, int tiley);
void	SpawnBonus (int tilex, int tiley, int type);
void	SpawnDoor (int tilex, int tiley);
void	SpawnHorseradish (int tilex, int tiley);
void	SpawnParsnip (int tilex, int tiley);
void	SpawnRhubarb (int tilex, int tiley);
void	SpawnCauliflower (int tilex, int tiley);
void	SpawnTurnip (int tilex, int tiley);
void	SpawnBrocco (int tilex, int tiley);
void	SpawnTomat (int tilex, int tiley);
void	SpawnCarrot (int tilex, int tiley);
void	SpawnAspar (int tilex, int tiley);
void	SpawnGrape (int tilex, int tiley);
void	SpawnVorticon (int tilex, int tiley);
void	SpawnEggplant (int tilex, int tiley);
void	SpawnBeet (int tilex, int tiley);
void	SpawnCameo1 (int tilex, int tiley);
void	SpawnCameo2 (int tilex, int tiley);
void	SpawnExit (int tilex, int tiley);
void	SpawnMosquito (int tilex, int tiley);
void	SpawnBean (int tilex, int tiley);
void	SpawnBaberry (int tilex, int tiley);
void	SpawnKumquat (int tilex, int tiley);
void	SpawnSteelCart (int tilex, int tiley);
void	SpawnCorn (int tilex, int tiley);
void	SpawnMetalDoor (int tilex, int tiley);

void 	CartReact (objtype *ob);
void 	CornThink (objtype *ob);
void 	CornContact (objtype *ob, objtype *hit);

extern statetype near s_cornwalk1;
extern statetype near s_cornwalk2;
extern statetype near s_cornwalk3;
extern statetype near s_cornwalk4;
extern statetype near s_cornattack1;
extern statetype near s_cornattack2;
extern statetype near s_cornattack3;
extern statetype near s_corndead;

extern statetype near s_scart1;
extern statetype near s_scart2;

extern statetype near s_kumquat;
extern statetype near s_kumquat2;
extern statetype near s_kumquatsit;

extern statetype near s_baberrycry1;
extern statetype near s_baberrycry2;
extern statetype near s_baberrysit;

extern statetype near s_beanwalk1;
extern statetype near s_beanwalk2;
extern statetype near s_beanwalk3;
extern statetype near s_beanstunned;

extern statetype near s_mosquitofly1;
extern statetype near s_mosquitofly2;
extern statetype near s_mosquitoshot1;
extern statetype near s_mosquitoshot2;
extern statetype near s_mosquitoshot3;

extern statetype near s_exit;

extern statetype near s_beet;
extern statetype near s_beetdead;

extern statetype near s_redvort1;
extern statetype near s_redvort2;
extern statetype near s_yellowvort1;
extern statetype near s_yellowvort2;
extern statetype near s_bluevort1;
extern statetype near s_bluevort2;

extern statetype near s_snake;
extern statetype near s_dave;
extern statetype near s_rover1;
extern statetype near s_rover2;

extern statetype near s_eggplantwalk1;
extern statetype near s_eggplantwalk2;
extern statetype near s_eggplantdead;

extern statetype near s_vort;

extern statetype near s_mortstand;

extern statetype near s_mortjump1;
extern statetype near s_mortjump2;
extern statetype near s_mortduck;
extern statetype near s_mortjumpup1;
extern statetype near s_mortjumpup2;
extern statetype near s_mortjumpup3;

extern statetype near s_mortwalk1;
extern statetype near s_mortwalk2;
extern statetype near s_mortwalk3;
extern statetype near s_mortwalk4;

extern statetype near s_mortthrow1;
extern statetype near s_mortthrow2;
extern statetype near s_mortthrow3;
extern statetype near s_mortthrow4;

extern statetype near s_mortthrowup1;
extern statetype near s_mortthrowup2;
extern statetype near s_mortthrowup3;

extern statetype near s_mortairthrow1;
extern statetype near s_mortairthrow2;
extern statetype near s_mortairthrow3;
extern statetype near s_mortairthrow4;

extern statetype near s_mortairthrowup1;
extern statetype near s_mortairthrowup2;
extern statetype near s_mortairthrowup3;

extern statetype near s_mortairthrowdown1;
extern statetype near s_mortairthrowdown2;
extern statetype near s_mortairthrowdown3;

extern statetype near s_mortdie1;
extern statetype near s_mortdie2;
extern statetype near s_mortdie3;

extern statetype near s_door;
extern statetype near s_doorraise;
extern statetype near s_metaldoor;
extern statetype near s_metaldoorraise;

extern statetype near s_bonus;
extern statetype near s_bonusrise;

extern statetype near s_broccowalk1;
extern statetype near s_broccowalk2;
extern statetype near s_broccowalk3;
extern statetype near s_broccowalk4;

extern statetype near s_broccosmash1;
extern statetype near s_broccosmash2;
extern statetype near s_broccosmash3;
extern statetype near s_broccosmash4;
extern statetype near s_broccosmash5;
extern statetype near s_broccosmash6;
extern statetype near s_broccosmash7;
extern statetype near s_broccosmash8;
extern statetype near s_broccosmash9;

extern statetype near s_grapefall;
extern statetype near s_grapefall2;

extern statetype near s_flower1;
extern statetype near s_flower2;
extern statetype near s_flower3;
extern statetype near s_flower4;
extern statetype near s_flower5;
extern statetype near s_flower6;

extern statetype near s_flower7;

extern statetype near s_poofto1;
extern statetype near s_poofto2;
extern statetype near s_poofto3;
extern statetype near s_poofto4;

extern statetype near s_poof;

extern statetype near s_pooffrom1;
extern statetype near s_pooffrom2;
extern statetype near s_pooffrom3;
extern statetype near s_pooffrom4;
extern statetype near s_pooffrom5;
extern statetype near s_pooffrom6;
extern statetype near s_pooffrom7;

extern statetype near s_bonus1;

extern statetype near s_horseradhole;
extern statetype near s_horseradgrow1;
extern statetype near s_horseradgrow2;
extern statetype near s_horseradgrow3;
extern statetype near s_horseradgrow4;
extern statetype near s_horseradgrow5;

extern statetype near s_rhubarbwalk1;
extern statetype near s_rhubarbwalk2;
extern statetype near s_rhubarbwalk3;
extern statetype near s_rhubarbwalk4;
extern statetype near s_rhubarbwalk5;

extern statetype near s_parsnipstand;
extern statetype near s_parsnipwalk1;
extern statetype near s_parsnipwalk2;

extern statetype near s_turnip;
extern statetype near s_turnip2;

extern statetype near s_tomatbounce;
extern statetype near s_tomatbounce2;

extern statetype near s_btomatbounce;
extern statetype near s_btomatbounce2;

extern statetype near s_carrotwalk1;
extern statetype near s_carrotwalk2;
extern statetype near s_carrotwalk3;
extern statetype near s_carrotwalk4;

extern statetype near s_carrotleap;

extern statetype near s_asparwalk1;
extern statetype near s_asparwalk2;
extern statetype near s_asparwalk3;
extern statetype near s_asparwalk4;

extern statetype near s_grapewait;
extern statetype near s_grapefall;
extern statetype near s_grapesit;
extern statetype near s_graperise;

extern statetype near s_grapewait2;
extern statetype near s_grapefall2;
extern statetype near s_grapesit2;
extern statetype near s_graperise2;

extern statetype near s_cauli1;
extern statetype near s_cauli2;

extern statetype near s_caulistop;

extern statetype near s_gargoyleface;
extern statetype near s_gargoylebreakout;
extern statetype near s_gargoyleexplode1;
extern statetype near s_gargoyleexplode2;
extern statetype near s_gargoyleexplode3;
extern statetype near s_gargoylefly1;
extern statetype near s_gargoylefly2;
extern statetype near s_gargoylefly3;
extern statetype near s_gargoylefly4;
extern statetype near s_gargoylestun;

void SpawnGargoyle(int x, int y);
void FaceContact(objtype *ob, objtype *hit);
void StonePieceReaction(objtype *ob);
void GargoyleStunContact(objtype *ob, objtype *hit);
void GargoyleContact(objtype *ob, objtype *hit);
void GargoyleReact(objtype *ob);

void KumquatThink(objtype *ob);

/*
=============================================================================

					  KDG_ACT2.C DEFINITIONS

=============================================================================
*/

void SpawnTater (int tilex, int tiley);
void SpawnCart (int tilex, int tiley);
void SpawnMCart (int tilex, int tiley);
void SpawnFrenchy (int tilex, int tiley);
void SpawnMelon (int tilex, int tiley, int dir);
void SpawnCelery (int tilex, int tiley);
void SpawnMachine (int tilex, int tiley);
void SpawnMushroom (int tilex, int tiley);
void SpawnBrussel (int tilex, int tiley);
void SpawnSquasher (int tilex, int tiley);
void SpawnApel (int tilex, int tiley);
void SpawnPeaPod (int tilex, int tiley);
void SpawnPeaBrain (int tilex, int tiley);
void SpawnBoobus (int tilex, int tiley);
void SpawnPumpkin (int tilex, int tiley);
void SpawnPear (int tilex, int tiley);
void SpawnPinkOwl (int tilex, int tiley);
void SpawnPinkOwl2 (int tilex, int tiley);
void SpawnPinkOwl3 (int tilex, int tiley);
void SpawnRover (int tilex, int tiley);
void SpawnGuardRobot (int tilex, int tiley);
void SpawnKid (int tilex, int tiley);
void SpawnKeycard (int tilex, int tiley);

void CeleryThink (objtype *ob);
void CeleryStopThink (objtype *ob);

void MelonSpitThink (objtype *ob);
void ProjectileReact (objtype *ob);
void DrawMelon (objtype *ob);
void ProjectileReact2 (objtype *ob);
void ProjectileReact3 (objtype *ob);

void FrenchyThink (objtype *ob);
void FrenchyRunThink (objtype *ob);
void FrenchyThrow (objtype *ob);

void CartReact (objtype *ob);

void TaterThink (objtype *ob);
void TaterWalkReact (objtype *ob);
void BackupReact (objtype *ob);
void DrawTater (objtype *ob);

void GuardRobotThink (void);
void GuardRobotContact (objtype *ob, objtype *hit);

extern statetype near s_keycard;

extern statetype near s_kid;

extern statetype near s_guardrobot1;
extern statetype near s_guardrobot2;
extern statetype near s_guardrobot3;
extern statetype near s_guardrobot4;
extern statetype near s_guardrobot5;
extern statetype near s_guardrobot6;

extern statetype near s_rover3;
extern statetype near s_rover4;

extern statetype near s_pinkowl;
extern statetype near s_pinkowl2;
extern statetype near s_pinkowl3;

extern statetype near s_pearwalk1;
extern statetype near s_pearwalk2;

extern statetype near s_pumpkinsit;
extern statetype near s_pumpkinjump1;
extern statetype near s_pumpkinjump2;
extern statetype near s_pumpkinbounce;
extern statetype near s_pumpkindead;

extern statetype near s_taterwalk1;
extern statetype near s_taterwalk2;
extern statetype near s_taterwalk3;
extern statetype near s_taterwalk4;

extern statetype near s_taterattack1;
extern statetype near s_taterattack2;
extern statetype near s_taterattack3;

extern statetype near s_taterscream1;
extern statetype near s_taterscream2;
extern statetype near s_taterscream3;

extern statetype near s_keenwalk1;
extern statetype near s_keenwalk2;
extern statetype near s_keenwalk3;
extern statetype near s_keenwalk4;

extern statetype near s_cartroll1;
extern statetype near s_cartroll2;

extern statetype near s_mcartroll1;
extern statetype near s_mcartroll2;

extern statetype near s_frenchywalk1;
extern statetype near s_frenchywalk2;
extern statetype near s_frenchywalk3;
extern statetype near s_frenchywalk4;

extern statetype near s_frenchyrun1;
extern statetype near s_frenchyrun2;
extern statetype near s_frenchyrun3;
extern statetype near s_frenchyrun4;

extern statetype near s_frenchythrow1;
extern statetype near s_frenchythrow2;
extern statetype near s_frenchythrow3;

extern statetype near s_fry1;
extern statetype near s_fry2;

extern statetype near s_melonside;
extern statetype near s_melonsidespit;
extern statetype near s_melonsidespit2;

extern statetype near s_melondown;
extern statetype near s_melondownspit;
extern statetype near s_melondownspit2;

extern statetype near s_melonseed1;
extern statetype near s_melonseed2;

extern statetype near s_melonseedd1;
extern statetype near s_melonseedd2;

extern statetype near s_melonup;
extern statetype near s_melonupspit;
extern statetype near s_melonupspit2;

extern statetype near s_melonseedu1;
extern statetype near s_melonseedu2;

extern statetype near s_celerystop1;
extern statetype near s_celerystop2;

extern statetype near s_celeryscare1;
extern statetype near s_celeryscare2;

extern statetype near s_machine;

extern statetype near s_mushroomfall;
extern statetype near s_mushroomrise;

extern statetype near s_brusselroll1;
extern statetype near s_brusselroll2;
extern statetype near s_brusselroll3;
extern statetype near s_brusselroll4;

extern statetype near s_squasherwalk1;
extern statetype near s_squasherwalk2;

extern statetype near s_squasherjump1;
extern statetype near s_squasherjump2;

extern statetype near s_squasherwait;

extern statetype near s_apelwalk1;
extern statetype near s_apelwalk2;
extern statetype near s_apelwalk3;

extern statetype near s_apelclimb1;
extern statetype near s_apelclimb2;

extern statetype near s_apelslide1;
extern statetype near s_apelslide2;
extern statetype near s_apelslide3;
extern statetype near s_apelslide4;

extern statetype near s_apelfall;

extern statetype near s_peabrainfly;

extern statetype near s_peabrainwalk1;
extern statetype near s_peabrainwalk2;
extern statetype near s_peabrainwalk3;
extern statetype near s_peabrainwalk4;

extern statetype near s_peapodwalk1;
extern statetype near s_peapodwalk2;
extern statetype near s_peapodwalk3;
extern statetype near s_peapodwalk4;

extern statetype near s_peapodspit1;
extern statetype near s_peapodspit2;

extern statetype near s_boobuswalk1;
extern statetype near s_boobuswalk2;
extern statetype near s_boobuswalk3;
extern statetype near s_boobuswalk4;

extern statetype near s_boobusjump;
extern statetype near s_boobusjumpup;
extern statetype near s_boobusjumpdown;
extern statetype near s_boobusjumpleft;
extern statetype near s_boobusjumpright;
extern statetype near s_boobusdie;
extern statetype near s_boobusdie2;

extern statetype near s_deathboom1;
extern statetype near s_deathboom2;
extern statetype near s_deathboom3;
extern statetype near s_deathboom4;
extern statetype near s_deathboom5;
extern statetype near s_deathboom6;
extern statetype near s_deathboom7;

/*
=============================================================================

					  KDG_TEXT.C DEFINITIONS

=============================================================================
*/

void HelpScreens(void);
void FinaleLayout(void);

// *** DEMO DEBUG CODE **

// Comment out #define DEMO_DEBUG in ID_HEADS.H to disable

#ifdef DEMO_DEBUG

#define DEMO_DEBUG_BUF_SIZE 0x4000

extern memptr demodebugseg; 
extern unsigned far * demodebugbuf;
extern char debugnameprefix;
extern long	LocalTime;

void StartDemoDataCollection(char nameprefix);
void WriteDemoDebugData(void);
void SaveDataForOneStepInDemo(void);

#endif

