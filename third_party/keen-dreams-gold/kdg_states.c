/* Keen Dreams Gold Edition: Keen Worlds native companion
 * Copyright (C) 2026 Anders Christensen
 * GPL v2 or later (see LICENSE).
 *
 * Gold keeps object and state pointers in 16-bit object fields and in saved
 * games (DOS near pointers). Here they become numbers: an object is its
 * place in objarray, a state its place in this table. Generated from the
 * statetype definitions in kdg_*.c; keep it sorted when adding states.
 */

#include "kdg_def.h"

extern statetype s_altsquasherdead;
extern statetype s_apelclimb1;
extern statetype s_apelclimb2;
extern statetype s_apeldead;
extern statetype s_apelfall;
extern statetype s_apelslide1;
extern statetype s_apelslide2;
extern statetype s_apelslide3;
extern statetype s_apelslide4;
extern statetype s_apelwalk1;
extern statetype s_apelwalk2;
extern statetype s_apelwalk3;
extern statetype s_aspardead;
extern statetype s_asparwalk1;
extern statetype s_asparwalk2;
extern statetype s_asparwalk3;
extern statetype s_asparwalk4;
extern statetype s_baberrycry1;
extern statetype s_baberrycry2;
extern statetype s_baberrysit;
extern statetype s_beanstunned;
extern statetype s_beanwalk1;
extern statetype s_beanwalk2;
extern statetype s_beanwalk3;
extern statetype s_beet;
extern statetype s_beetdead;
extern statetype s_blueflowerpower1;
extern statetype s_blueflowerpower2;
extern statetype s_bluevort1;
extern statetype s_bluevort2;
extern statetype s_bombexplode;
extern statetype s_bombexplode2;
extern statetype s_bombexplode3;
extern statetype s_bombexplode4;
extern statetype s_bombexplode5;
extern statetype s_bombexplode6;
extern statetype s_bonus;
extern statetype s_bonusrise;
extern statetype s_boobusbomb1;
extern statetype s_boobusbomb2;
extern statetype s_boobusbomb3;
extern statetype s_boobusbomb4;
extern statetype s_boobusbombthrown1;
extern statetype s_boobusbombthrown2;
extern statetype s_boobusdie;
extern statetype s_boobusdie2;
extern statetype s_boobusjump;
extern statetype s_boobusjumpdown;
extern statetype s_boobusjumpleft;
extern statetype s_boobusjumpright;
extern statetype s_boobusjumpup;
extern statetype s_boobuswalk1;
extern statetype s_boobuswalk2;
extern statetype s_boobuswalk3;
extern statetype s_boobuswalk4;
extern statetype s_broccodead;
extern statetype s_broccosmash1;
extern statetype s_broccosmash2;
extern statetype s_broccosmash3;
extern statetype s_broccosmash4;
extern statetype s_broccosmash5;
extern statetype s_broccosmash6;
extern statetype s_broccosmash7;
extern statetype s_broccosmash8;
extern statetype s_broccosmash9;
extern statetype s_broccowalk1;
extern statetype s_broccowalk2;
extern statetype s_broccowalk3;
extern statetype s_broccowalk4;
extern statetype s_brusselroll1;
extern statetype s_brusselroll2;
extern statetype s_brusselroll3;
extern statetype s_brusselroll4;
extern statetype s_btomatbounce;
extern statetype s_btomatbounce2;
extern statetype s_btomatdead;
extern statetype s_carrotdead;
extern statetype s_carrotleap;
extern statetype s_carrotwalk1;
extern statetype s_carrotwalk2;
extern statetype s_carrotwalk3;
extern statetype s_carrotwalk4;
extern statetype s_cartroll1;
extern statetype s_cartroll2;
extern statetype s_cauli1;
extern statetype s_cauli2;
extern statetype s_caulistop;
extern statetype s_celeryscare1;
extern statetype s_celeryscare2;
extern statetype s_celerystop1;
extern statetype s_celerystop2;
extern statetype s_cornattack1;
extern statetype s_cornattack2;
extern statetype s_cornattack3;
extern statetype s_corndead;
extern statetype s_cornwalk1;
extern statetype s_cornwalk2;
extern statetype s_cornwalk3;
extern statetype s_cornwalk4;
extern statetype s_dave;
extern statetype s_deathboom1;
extern statetype s_deathboom2;
extern statetype s_deathboom3;
extern statetype s_deathboom4;
extern statetype s_deathboom5;
extern statetype s_deathboom6;
extern statetype s_deathboom7;
extern statetype s_demo;
extern statetype s_door;
extern statetype s_doorraise;
extern statetype s_eggplantdead;
extern statetype s_eggplantwalk1;
extern statetype s_eggplantwalk2;
extern statetype s_flower1;
extern statetype s_flower2;
extern statetype s_flower3;
extern statetype s_flower4;
extern statetype s_flower5;
extern statetype s_flower6;
extern statetype s_flower7;
extern statetype s_flowerpower1;
extern statetype s_flowerpower2;
extern statetype s_flowerpower3;
extern statetype s_flowerpower4;
extern statetype s_flowerpower5;
extern statetype s_flowerpower6;
extern statetype s_frenchydead;
extern statetype s_frenchyrun1;
extern statetype s_frenchyrun2;
extern statetype s_frenchyrun3;
extern statetype s_frenchyrun4;
extern statetype s_frenchythrow1;
extern statetype s_frenchythrow2;
extern statetype s_frenchythrow3;
extern statetype s_frenchywalk1;
extern statetype s_frenchywalk2;
extern statetype s_frenchywalk3;
extern statetype s_frenchywalk4;
extern statetype s_fry1;
extern statetype s_fry2;
extern statetype s_gargoylebreakout;
extern statetype s_gargoyleexplode1;
extern statetype s_gargoyleexplode2;
extern statetype s_gargoyleexplode3;
extern statetype s_gargoyleface;
extern statetype s_gargoylefly1;
extern statetype s_gargoylefly2;
extern statetype s_gargoylefly3;
extern statetype s_gargoylefly4;
extern statetype s_gargoylestun;
extern statetype s_grapefall;
extern statetype s_grapefall2;
extern statetype s_graperise;
extern statetype s_graperise2;
extern statetype s_grapesit;
extern statetype s_grapesit2;
extern statetype s_grapewait;
extern statetype s_grapewait2;
extern statetype s_guardrobot1;
extern statetype s_guardrobot2;
extern statetype s_guardrobot3;
extern statetype s_guardrobot4;
extern statetype s_guardrobot5;
extern statetype s_guardrobot6;
extern statetype s_horseradgrow1;
extern statetype s_horseradgrow2;
extern statetype s_horseradgrow3;
extern statetype s_horseradgrow4;
extern statetype s_horseradgrow5;
extern statetype s_horseradhole;
extern statetype s_keenairshoot1;
extern statetype s_keenairshoot2;
extern statetype s_keenairshoot3;
extern statetype s_keenairshootdown1;
extern statetype s_keenairshootdown2;
extern statetype s_keenairshootdown3;
extern statetype s_keenairshootup1;
extern statetype s_keenairshootup2;
extern statetype s_keenairshootup3;
extern statetype s_keenairthrow1;
extern statetype s_keenairthrow2;
extern statetype s_keenairthrow3;
extern statetype s_keenairthrowdown1;
extern statetype s_keenairthrowdown2;
extern statetype s_keenairthrowdown3;
extern statetype s_keenairthrowup1;
extern statetype s_keenairthrowup2;
extern statetype s_keenairthrowup3;
extern statetype s_keenclimb1;
extern statetype s_keenclimb2;
extern statetype s_keenclimb3;
extern statetype s_keenclimbup;
extern statetype s_keenclimbup2;
extern statetype s_keenclimbup3;
extern statetype s_keenclimbup4;
extern statetype s_keenclimbup5;
extern statetype s_keendead;
extern statetype s_keendie1;
extern statetype s_keendie2;
extern statetype s_keendie3;
extern statetype s_keendrop;
extern statetype s_keenduck;
extern statetype s_keenexit1;
extern statetype s_keenexit2;
extern statetype s_keenexit3;
extern statetype s_keenexit4;
extern statetype s_keenexit5;
extern statetype s_keenfell;
extern statetype s_keengetup;
extern statetype s_keengosleep1;
extern statetype s_keengosleep2;
extern statetype s_keenholdon;
extern statetype s_keenholdon2;
extern statetype s_keenjump1;
extern statetype s_keenjump2;
extern statetype s_keenjump3;
extern statetype s_keenjumpup1;
extern statetype s_keenjumpup2;
extern statetype s_keenjumpup3;
extern statetype s_keenlookup;
extern statetype s_keenlosehealth;
extern statetype s_keenlowshoot1;
extern statetype s_keenlowshoot2;
extern statetype s_keenpauselook;
extern statetype s_keenpickup;
extern statetype s_keenplace;
extern statetype s_keenpogo;
extern statetype s_keenpogo2;
extern statetype s_keenpogodown;
extern statetype s_keenpole;
extern statetype s_keenpoleshoot1;
extern statetype s_keenpoleshoot2;
extern statetype s_keenpoleshootdown1;
extern statetype s_keenpoleshootdown2;
extern statetype s_keenpoleshootup1;
extern statetype s_keenpoleshootup2;
extern statetype s_keenpolethrow1;
extern statetype s_keenpolethrow2;
extern statetype s_keenpolethrow3;
extern statetype s_keenpolethrowdown1;
extern statetype s_keenpolethrowdown2;
extern statetype s_keenpolethrowdown3;
extern statetype s_keenpolethrowup1;
extern statetype s_keenpolethrowup2;
extern statetype s_keenpolethrowup3;
extern statetype s_keenshoot1;
extern statetype s_keenshoot2;
extern statetype s_keenshootup1;
extern statetype s_keenshootup2;
extern statetype s_keensleep1;
extern statetype s_keensleep2;
extern statetype s_keenslide1;
extern statetype s_keenslide2;
extern statetype s_keenslide3;
extern statetype s_keenslide4;
extern statetype s_keenstand;
extern statetype s_keenstand2;
extern statetype s_keenthrow1;
extern statetype s_keenthrow2;
extern statetype s_keenthrow3;
extern statetype s_keenthrow4;
extern statetype s_keenthrowup1;
extern statetype s_keenthrowup2;
extern statetype s_keenthrowup3;
extern statetype s_keenturn;
extern statetype s_keenwait1;
extern statetype s_keenwait2;
extern statetype s_keenwait3;
extern statetype s_keenwait4;
extern statetype s_keenwait5;
extern statetype s_keenwait6;
extern statetype s_keenwalk1;
extern statetype s_keenwalk2;
extern statetype s_keenwalk3;
extern statetype s_keenwalk4;
extern statetype s_keenyawn1;
extern statetype s_keenyawn2;
extern statetype s_keenyawn3;
extern statetype s_keenyawn4;
extern statetype s_keenzee1;
extern statetype s_keenzee2;
extern statetype s_keenzee3;
extern statetype s_keycard;
extern statetype s_kid;
extern statetype s_kumquat;
extern statetype s_kumquat2;
extern statetype s_kumquatsit;
extern statetype s_machine;
extern statetype s_mcartroll1;
extern statetype s_mcartroll2;
extern statetype s_melondown;
extern statetype s_melondownspit;
extern statetype s_melondownspit2;
extern statetype s_melonseed1;
extern statetype s_melonseed2;
extern statetype s_melonseedd1;
extern statetype s_melonseedd2;
extern statetype s_melonseedu1;
extern statetype s_melonseedu2;
extern statetype s_melonside;
extern statetype s_melonsidespit;
extern statetype s_melonsidespit2;
extern statetype s_melonup;
extern statetype s_melonupspit;
extern statetype s_melonupspit2;
extern statetype s_metaldoor;
extern statetype s_metaldoorraise;
extern statetype s_mortairthrow1;
extern statetype s_mortairthrow2;
extern statetype s_mortairthrow3;
extern statetype s_mortairthrow4;
extern statetype s_mortairthrowdown1;
extern statetype s_mortairthrowdown2;
extern statetype s_mortairthrowdown3;
extern statetype s_mortairthrowup1;
extern statetype s_mortairthrowup2;
extern statetype s_mortairthrowup3;
extern statetype s_mortdie1;
extern statetype s_mortdie2;
extern statetype s_mortdie3;
extern statetype s_mortduck;
extern statetype s_mortjump1;
extern statetype s_mortjump2;
extern statetype s_mortjumpup1;
extern statetype s_mortjumpup2;
extern statetype s_mortjumpup3;
extern statetype s_mortstand;
extern statetype s_mortthrow1;
extern statetype s_mortthrow2;
extern statetype s_mortthrow3;
extern statetype s_mortthrow4;
extern statetype s_mortthrowup1;
extern statetype s_mortthrowup2;
extern statetype s_mortthrowup3;
extern statetype s_mortwalk1;
extern statetype s_mortwalk2;
extern statetype s_mortwalk3;
extern statetype s_mortwalk4;
extern statetype s_mosquitofly1;
extern statetype s_mosquitofly2;
extern statetype s_mosquitoshot1;
extern statetype s_mosquitoshot2;
extern statetype s_mosquitoshot3;
extern statetype s_mushroomfall;
extern statetype s_mushroomrise;
extern statetype s_parsnipstand;
extern statetype s_parsnipwalk1;
extern statetype s_parsnipwalk2;
extern statetype s_peabraindead;
extern statetype s_peabrainfly;
extern statetype s_peabrainwalk1;
extern statetype s_peabrainwalk2;
extern statetype s_peabrainwalk3;
extern statetype s_peabrainwalk4;
extern statetype s_peapoddead;
extern statetype s_peapodspit1;
extern statetype s_peapodspit2;
extern statetype s_peapodwalk1;
extern statetype s_peapodwalk2;
extern statetype s_peapodwalk3;
extern statetype s_peapodwalk4;
extern statetype s_pearwalk1;
extern statetype s_pearwalk2;
extern statetype s_pinkowl;
extern statetype s_pinkowl2;
extern statetype s_pinkowl3;
extern statetype s_poof;
extern statetype s_pooffrom1;
extern statetype s_pooffrom2;
extern statetype s_pooffrom3;
extern statetype s_pooffrom4;
extern statetype s_pooffrom5;
extern statetype s_pooffrom6;
extern statetype s_pooffrom7;
extern statetype s_poofto1;
extern statetype s_poofto2;
extern statetype s_poofto3;
extern statetype s_poofto4;
extern statetype s_powerblink1;
extern statetype s_powerblink2;
extern statetype s_powerblink3;
extern statetype s_powerblink4;
extern statetype s_pumpkin;
extern statetype s_pumpkinbounce;
extern statetype s_pumpkindead;
extern statetype s_pumpkinjump1;
extern statetype s_pumpkinjump2;
extern statetype s_redvort1;
extern statetype s_redvort2;
extern statetype s_rhubarbwalk1;
extern statetype s_rhubarbwalk2;
extern statetype s_rhubarbwalk3;
extern statetype s_rhubarbwalk4;
extern statetype s_rhubarbwalk5;
extern statetype s_rover1;
extern statetype s_rover2;
extern statetype s_rover3;
extern statetype s_rover4;
extern statetype s_scart1;
extern statetype s_scart2;
extern statetype s_score;
extern statetype s_snake;
extern statetype s_spot;
extern statetype s_squasherdead;
extern statetype s_squasherjump1;
extern statetype s_squasherjump2;
extern statetype s_squasherwait;
extern statetype s_squasherwalk1;
extern statetype s_squasherwalk2;
extern statetype s_taterattack1;
extern statetype s_taterattack2;
extern statetype s_taterattack3;
extern statetype s_taterdead;
extern statetype s_taterfemaledead;
extern statetype s_taterscream1;
extern statetype s_taterscream2;
extern statetype s_taterscream3;
extern statetype s_taterwalk1;
extern statetype s_taterwalk2;
extern statetype s_taterwalk3;
extern statetype s_taterwalk4;
extern statetype s_tomatbounce;
extern statetype s_tomatbounce2;
extern statetype s_tomatdead;
extern statetype s_turnip;
extern statetype s_turnip2;
extern statetype s_vorthit1;
extern statetype s_vorthit2;
extern statetype s_vorthit3;
extern statetype s_vorthit4;
extern statetype s_vortray1;
extern statetype s_vortray2;
extern statetype s_vortray3;
extern statetype s_vortray4;
extern statetype s_vortray5;
extern statetype s_vortray6;
extern statetype s_vortray7;
extern statetype s_vortray8;
extern statetype s_worldkeen;
extern statetype s_worldkeensleep1;
extern statetype s_worldkeensleep2;
extern statetype s_worldkeenwait;
extern statetype s_worldkeenwave1;
extern statetype s_worldkeenwave2;
extern statetype s_worldkeenwave3;
extern statetype s_worldkeenwave4;
extern statetype s_worldkeenwave5;
extern statetype s_worldwalk;
extern statetype s_yellowvort1;
extern statetype s_yellowvort2;

static statetype *const kdg_states[] =
{
	&s_altsquasherdead,
	&s_apelclimb1,
	&s_apelclimb2,
	&s_apeldead,
	&s_apelfall,
	&s_apelslide1,
	&s_apelslide2,
	&s_apelslide3,
	&s_apelslide4,
	&s_apelwalk1,
	&s_apelwalk2,
	&s_apelwalk3,
	&s_aspardead,
	&s_asparwalk1,
	&s_asparwalk2,
	&s_asparwalk3,
	&s_asparwalk4,
	&s_baberrycry1,
	&s_baberrycry2,
	&s_baberrysit,
	&s_beanstunned,
	&s_beanwalk1,
	&s_beanwalk2,
	&s_beanwalk3,
	&s_beet,
	&s_beetdead,
	&s_blueflowerpower1,
	&s_blueflowerpower2,
	&s_bluevort1,
	&s_bluevort2,
	&s_bombexplode,
	&s_bombexplode2,
	&s_bombexplode3,
	&s_bombexplode4,
	&s_bombexplode5,
	&s_bombexplode6,
	&s_bonus,
	&s_bonusrise,
	&s_boobusbomb1,
	&s_boobusbomb2,
	&s_boobusbomb3,
	&s_boobusbomb4,
	&s_boobusbombthrown1,
	&s_boobusbombthrown2,
	&s_boobusdie,
	&s_boobusdie2,
	&s_boobusjump,
	&s_boobusjumpdown,
	&s_boobusjumpleft,
	&s_boobusjumpright,
	&s_boobusjumpup,
	&s_boobuswalk1,
	&s_boobuswalk2,
	&s_boobuswalk3,
	&s_boobuswalk4,
	&s_broccodead,
	&s_broccosmash1,
	&s_broccosmash2,
	&s_broccosmash3,
	&s_broccosmash4,
	&s_broccosmash5,
	&s_broccosmash6,
	&s_broccosmash7,
	&s_broccosmash8,
	&s_broccosmash9,
	&s_broccowalk1,
	&s_broccowalk2,
	&s_broccowalk3,
	&s_broccowalk4,
	&s_brusselroll1,
	&s_brusselroll2,
	&s_brusselroll3,
	&s_brusselroll4,
	&s_btomatbounce,
	&s_btomatbounce2,
	&s_btomatdead,
	&s_carrotdead,
	&s_carrotleap,
	&s_carrotwalk1,
	&s_carrotwalk2,
	&s_carrotwalk3,
	&s_carrotwalk4,
	&s_cartroll1,
	&s_cartroll2,
	&s_cauli1,
	&s_cauli2,
	&s_caulistop,
	&s_celeryscare1,
	&s_celeryscare2,
	&s_celerystop1,
	&s_celerystop2,
	&s_cornattack1,
	&s_cornattack2,
	&s_cornattack3,
	&s_corndead,
	&s_cornwalk1,
	&s_cornwalk2,
	&s_cornwalk3,
	&s_cornwalk4,
	&s_dave,
	&s_deathboom1,
	&s_deathboom2,
	&s_deathboom3,
	&s_deathboom4,
	&s_deathboom5,
	&s_deathboom6,
	&s_deathboom7,
	&s_demo,
	&s_door,
	&s_doorraise,
	&s_eggplantdead,
	&s_eggplantwalk1,
	&s_eggplantwalk2,
	&s_flower1,
	&s_flower2,
	&s_flower3,
	&s_flower4,
	&s_flower5,
	&s_flower6,
	&s_flower7,
	&s_flowerpower1,
	&s_flowerpower2,
	&s_flowerpower3,
	&s_flowerpower4,
	&s_flowerpower5,
	&s_flowerpower6,
	&s_frenchydead,
	&s_frenchyrun1,
	&s_frenchyrun2,
	&s_frenchyrun3,
	&s_frenchyrun4,
	&s_frenchythrow1,
	&s_frenchythrow2,
	&s_frenchythrow3,
	&s_frenchywalk1,
	&s_frenchywalk2,
	&s_frenchywalk3,
	&s_frenchywalk4,
	&s_fry1,
	&s_fry2,
	&s_gargoylebreakout,
	&s_gargoyleexplode1,
	&s_gargoyleexplode2,
	&s_gargoyleexplode3,
	&s_gargoyleface,
	&s_gargoylefly1,
	&s_gargoylefly2,
	&s_gargoylefly3,
	&s_gargoylefly4,
	&s_gargoylestun,
	&s_grapefall,
	&s_grapefall2,
	&s_graperise,
	&s_graperise2,
	&s_grapesit,
	&s_grapesit2,
	&s_grapewait,
	&s_grapewait2,
	&s_guardrobot1,
	&s_guardrobot2,
	&s_guardrobot3,
	&s_guardrobot4,
	&s_guardrobot5,
	&s_guardrobot6,
	&s_horseradgrow1,
	&s_horseradgrow2,
	&s_horseradgrow3,
	&s_horseradgrow4,
	&s_horseradgrow5,
	&s_horseradhole,
	&s_keenairshoot1,
	&s_keenairshoot2,
	&s_keenairshoot3,
	&s_keenairshootdown1,
	&s_keenairshootdown2,
	&s_keenairshootdown3,
	&s_keenairshootup1,
	&s_keenairshootup2,
	&s_keenairshootup3,
	&s_keenairthrow1,
	&s_keenairthrow2,
	&s_keenairthrow3,
	&s_keenairthrowdown1,
	&s_keenairthrowdown2,
	&s_keenairthrowdown3,
	&s_keenairthrowup1,
	&s_keenairthrowup2,
	&s_keenairthrowup3,
	&s_keenclimb1,
	&s_keenclimb2,
	&s_keenclimb3,
	&s_keenclimbup,
	&s_keenclimbup2,
	&s_keenclimbup3,
	&s_keenclimbup4,
	&s_keenclimbup5,
	&s_keendead,
	&s_keendie1,
	&s_keendie2,
	&s_keendie3,
	&s_keendrop,
	&s_keenduck,
	&s_keenexit1,
	&s_keenexit2,
	&s_keenexit3,
	&s_keenexit4,
	&s_keenexit5,
	&s_keenfell,
	&s_keengetup,
	&s_keengosleep1,
	&s_keengosleep2,
	&s_keenholdon,
	&s_keenholdon2,
	&s_keenjump1,
	&s_keenjump2,
	&s_keenjump3,
	&s_keenjumpup1,
	&s_keenjumpup2,
	&s_keenjumpup3,
	&s_keenlookup,
	&s_keenlosehealth,
	&s_keenlowshoot1,
	&s_keenlowshoot2,
	&s_keenpauselook,
	&s_keenpickup,
	&s_keenplace,
	&s_keenpogo,
	&s_keenpogo2,
	&s_keenpogodown,
	&s_keenpole,
	&s_keenpoleshoot1,
	&s_keenpoleshoot2,
	&s_keenpoleshootdown1,
	&s_keenpoleshootdown2,
	&s_keenpoleshootup1,
	&s_keenpoleshootup2,
	&s_keenpolethrow1,
	&s_keenpolethrow2,
	&s_keenpolethrow3,
	&s_keenpolethrowdown1,
	&s_keenpolethrowdown2,
	&s_keenpolethrowdown3,
	&s_keenpolethrowup1,
	&s_keenpolethrowup2,
	&s_keenpolethrowup3,
	&s_keenshoot1,
	&s_keenshoot2,
	&s_keenshootup1,
	&s_keenshootup2,
	&s_keensleep1,
	&s_keensleep2,
	&s_keenslide1,
	&s_keenslide2,
	&s_keenslide3,
	&s_keenslide4,
	&s_keenstand,
	&s_keenstand2,
	&s_keenthrow1,
	&s_keenthrow2,
	&s_keenthrow3,
	&s_keenthrow4,
	&s_keenthrowup1,
	&s_keenthrowup2,
	&s_keenthrowup3,
	&s_keenturn,
	&s_keenwait1,
	&s_keenwait2,
	&s_keenwait3,
	&s_keenwait4,
	&s_keenwait5,
	&s_keenwait6,
	&s_keenwalk1,
	&s_keenwalk2,
	&s_keenwalk3,
	&s_keenwalk4,
	&s_keenyawn1,
	&s_keenyawn2,
	&s_keenyawn3,
	&s_keenyawn4,
	&s_keenzee1,
	&s_keenzee2,
	&s_keenzee3,
	&s_keycard,
	&s_kid,
	&s_kumquat,
	&s_kumquat2,
	&s_kumquatsit,
	&s_machine,
	&s_mcartroll1,
	&s_mcartroll2,
	&s_melondown,
	&s_melondownspit,
	&s_melondownspit2,
	&s_melonseed1,
	&s_melonseed2,
	&s_melonseedd1,
	&s_melonseedd2,
	&s_melonseedu1,
	&s_melonseedu2,
	&s_melonside,
	&s_melonsidespit,
	&s_melonsidespit2,
	&s_melonup,
	&s_melonupspit,
	&s_melonupspit2,
	&s_metaldoor,
	&s_metaldoorraise,
	&s_mortairthrow1,
	&s_mortairthrow2,
	&s_mortairthrow3,
	&s_mortairthrow4,
	&s_mortairthrowdown1,
	&s_mortairthrowdown2,
	&s_mortairthrowdown3,
	&s_mortairthrowup1,
	&s_mortairthrowup2,
	&s_mortairthrowup3,
	&s_mortdie1,
	&s_mortdie2,
	&s_mortdie3,
	&s_mortduck,
	&s_mortjump1,
	&s_mortjump2,
	&s_mortjumpup1,
	&s_mortjumpup2,
	&s_mortjumpup3,
	&s_mortstand,
	&s_mortthrow1,
	&s_mortthrow2,
	&s_mortthrow3,
	&s_mortthrow4,
	&s_mortthrowup1,
	&s_mortthrowup2,
	&s_mortthrowup3,
	&s_mortwalk1,
	&s_mortwalk2,
	&s_mortwalk3,
	&s_mortwalk4,
	&s_mosquitofly1,
	&s_mosquitofly2,
	&s_mosquitoshot1,
	&s_mosquitoshot2,
	&s_mosquitoshot3,
	&s_mushroomfall,
	&s_mushroomrise,
	&s_parsnipstand,
	&s_parsnipwalk1,
	&s_parsnipwalk2,
	&s_peabraindead,
	&s_peabrainfly,
	&s_peabrainwalk1,
	&s_peabrainwalk2,
	&s_peabrainwalk3,
	&s_peabrainwalk4,
	&s_peapoddead,
	&s_peapodspit1,
	&s_peapodspit2,
	&s_peapodwalk1,
	&s_peapodwalk2,
	&s_peapodwalk3,
	&s_peapodwalk4,
	&s_pearwalk1,
	&s_pearwalk2,
	&s_pinkowl,
	&s_pinkowl2,
	&s_pinkowl3,
	&s_poof,
	&s_pooffrom1,
	&s_pooffrom2,
	&s_pooffrom3,
	&s_pooffrom4,
	&s_pooffrom5,
	&s_pooffrom6,
	&s_pooffrom7,
	&s_poofto1,
	&s_poofto2,
	&s_poofto3,
	&s_poofto4,
	&s_powerblink1,
	&s_powerblink2,
	&s_powerblink3,
	&s_powerblink4,
	&s_pumpkin,
	&s_pumpkinbounce,
	&s_pumpkindead,
	&s_pumpkinjump1,
	&s_pumpkinjump2,
	&s_redvort1,
	&s_redvort2,
	&s_rhubarbwalk1,
	&s_rhubarbwalk2,
	&s_rhubarbwalk3,
	&s_rhubarbwalk4,
	&s_rhubarbwalk5,
	&s_rover1,
	&s_rover2,
	&s_rover3,
	&s_rover4,
	&s_scart1,
	&s_scart2,
	&s_score,
	&s_snake,
	&s_spot,
	&s_squasherdead,
	&s_squasherjump1,
	&s_squasherjump2,
	&s_squasherwait,
	&s_squasherwalk1,
	&s_squasherwalk2,
	&s_taterattack1,
	&s_taterattack2,
	&s_taterattack3,
	&s_taterdead,
	&s_taterfemaledead,
	&s_taterscream1,
	&s_taterscream2,
	&s_taterscream3,
	&s_taterwalk1,
	&s_taterwalk2,
	&s_taterwalk3,
	&s_taterwalk4,
	&s_tomatbounce,
	&s_tomatbounce2,
	&s_tomatdead,
	&s_turnip,
	&s_turnip2,
	&s_vorthit1,
	&s_vorthit2,
	&s_vorthit3,
	&s_vorthit4,
	&s_vortray1,
	&s_vortray2,
	&s_vortray3,
	&s_vortray4,
	&s_vortray5,
	&s_vortray6,
	&s_vortray7,
	&s_vortray8,
	&s_worldkeen,
	&s_worldkeensleep1,
	&s_worldkeensleep2,
	&s_worldkeenwait,
	&s_worldkeenwave1,
	&s_worldkeenwave2,
	&s_worldkeenwave3,
	&s_worldkeenwave4,
	&s_worldkeenwave5,
	&s_worldwalk,
	&s_yellowvort1,
	&s_yellowvort2,
};

extern objtype objarray[MAXACTORS];

int16_t ObjToInt (objtype *ob)
{
	if (!ob)
		return 0;
	return (int16_t)(ob - objarray) + 1;
}

objtype *IntToObj (int16_t n)
{
	if (n <= 0 || n > MAXACTORS)
		return NULL;
	return &objarray[n-1];
}

int16_t StateToInt (statetype *state)
{
	unsigned i;

	if (!state)
		return 0;
	for (i = 0; i < sizeof(kdg_states)/sizeof(kdg_states[0]); i++)
		if (kdg_states[i] == state)
			return (int16_t)(i + 1);
	Quit ("StateToInt: unknown state");
	return 0;
}

statetype *IntToState (int16_t n)
{
	if (n <= 0 || n > (int16_t)(sizeof(kdg_states)/sizeof(kdg_states[0])))
		return NULL;
	return kdg_states[n-1];
}
