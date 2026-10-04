/*
Keen Worlds: Foray in the Forest's level setup, ported from ScanInfoPlane in
CK_SPEC.C of its source (Nisaba and K1n9_Duk3; GPL v2 or later).
*/

#include "kw_fitf.h"
#include "kw_ff_act.h"

// A sprite cache of its GRAPHCKS.H (START_LUMP/END_LUMP there)
static void FF_NeedChunks(int first, int last)
{
	for (int i = first; i <= last; ++i)
		CA_MarkGrChunk(i);
}
#define NeedLump(name) FF_NeedChunks(FF_##name##_LUMP_START, FF_##name##_LUMP_END)

extern CK_object *ck_lastObject;
#define DEFAULT_SHOTS 5

// If the info value is a number icon naming a song, the level plays it
// (SetMusicNum in CK_SPEC.C).
static void FF_SetMusicNum(int infoValue)
{
	if (infoValue >= INFONUMBER_0 && infoValue <= INFONUMBER_MAX)
	{
		infoValue -= INFONUMBER_0;
		if (infoValue < FF_LASTMUSIC)
			ff_map.music = infoValue;
	}
}


void FF_ScanInfoPlane(void)
{

	CK_object *ob;
	int x, y, infoValue, i,j;


	// K1n9_Duk3 addition:
	// Music number must now be placed as an InfoPlane number icon in
	// the lop left corner of the map (at position 0, 0).
	
	FF_SetMusicNum(FF_MapAt(2, 0));
	// Note: The music will be set to song number 0 by default!
	

	for (y = 0, i = 0; y < CA_GetMapHeight(); y++)
	{
		for (x = 0; x < CA_GetMapWidth(); x++, i++)
		{
			infoValue = FF_MapAt(2, i);

			switch (infoValue)
			{
				case 1:
					FF_SpawnKeen(x, y, 1);
					FF_SetMusicNum(FF_MapAt(2, i-CA_GetMapWidth()));	// use info number directly above the Keen icon as music number
					NeedLump(KEEN);
					CK_DemoSignSpawn();
					CA_MarkGrChunk(FF_SCOREBOXSPR);
					break;

				case 81:
					if (ck_gameState.levelsDone[MAP_BWBMEGAROCKET])
						break;
				case 2:
					FF_SpawnKeen(x, y, -1);
					FF_SetMusicNum(FF_MapAt(2, i-CA_GetMapWidth()));	// use info number directly above the Keen icon as music number
					NeedLump(KEEN);
					CK_DemoSignSpawn();
					CA_MarkGrChunk(FF_SCOREBOXSPR);
					break;

				case 3:
					CK_SpawnMapKeen(x, y); // TODO: its own map Keen (SpawnWorldKeen in CK_KEEN2.C)
					NeedLump(WORLDKEEN);

					CK_DemoSignSpawn();
					CA_MarkGrChunk(FF_SCOREBOXSPR);
					break;

				case 4:
					ob = FF_SpawnBonus(x, y, 6);	// spawn cotton candy object
					ob->gfxChunk++;	// and change sprite number into lollypop sprite
					ob->user2++;
					ob->user3++;
					CK_ResetClipRects(ob);	// hitboxes for the lollypop and the cotton candy sprites should be mostly identical, but just in case...
					break;

				case 5:
					FF_SpawnTentacle(x, y);
					NeedLump(TENTACLE);
					break;

				case 6:
					FF_SpawnLindsey(x, y, 0);
					NeedLump(LINDSEY);
					NeedLump(SMIRKYPOOF);
					break;

				case 7:
					FF_SpawnWormouth(x, y);
					NeedLump(WORMOUTH);
					break;

				case 8:
					FF_SpawnSkypest(x, y);
					NeedLump(SKYPEST);
					break;

				case 9:
					FF_SpawnPoleknawel(x, y);
					NeedLump(POLEKNAWEL);
					break;

				case 10:
					FF_SpawnKitten(x, y);
					NeedLump(KITTEN);
					break;

				case 11:
					FF_SpawnGellyGhost(x, y);
					NeedLump(GELLYGHOST);
					NeedLump(KEENKNOCKBACK);
					break;

				case 12:
					FF_SpawnBounder(x, y);
					NeedLump(BOUNDER);
					break;

				case 13:
					FF_SpawnBrainFucl(x, y);
					NeedLump(BRAINFUCL);
					break;

				case 14:
					FF_SpawnXkyLick(x, y);
					NeedLump(XKYLICK);
					break;

				case 15:
					FF_SpawnRoots(x, y);
					NeedLump(ROOTS);
					break;

				case 16:
					FF_SpawnFungusAmongus(x, y);
					NeedLump(FUNGUSAMONGUS);
					break;

				case 17:
					FF_SpawnYeti(x, y);
					NeedLump(YETI);
					break;

				case 18:
					FF_SpawnBalloonSmirky(x, y);
					NeedLump(SMIRKY);
					break;

				case 19:
					FF_SpawnCloud(x, y);
					CA_MarkGrChunk(FF_BIGCLOUDSPR);
					break;

				case 20:
					FF_SpawnMolet(x, y);
					NeedLump(MOLET);
#ifdef EXTRA_INTERACTIONS
					NeedLump(TREASUREPOOF);
#endif
					break;

				case 21:
					FF_SpawnSneakyTree(x, y);
					NeedLump(SNEAKYTREE);
					break;

				case 22:
					FF_SpawnSlug(x, y);
					NeedLump(SLUG);
					break;

				case 23:
					FF_SpawnBird(x, y);
					NeedLump(BIRD);
					break;

				case 24:
					FF_SpawnCroc(x, y);
					NeedLump(CROCO);
					break;

				case 25:
					RF_SetScrollBlock(x,y,true);
					break;
				case 26:
					RF_SetScrollBlock(x,y,false);
					break;

				case 27:
				case 28:
				case 29:
				case 30:
					FF_SpawnPlatform(x,y,infoValue-27);
					NeedLump(PLATFORM);
					break;

				// 31 is the platform blocker icon!

				case 32:
					FF_SpawnDropPlat(x,y);
					NeedLump(PLATFORM);
					break;

				case 33:
					FF_SpawnTentacleNest(x, y);
					NeedLump(GRETENTA);
					CA_MarkGrChunk(FF_EFREEKEEN1SPR);
					CA_MarkGrChunk(FF_EFREEKEEN2SPR);
					break;

				case 34:
					if (ck_gameState.numShots < DEFAULT_SHOTS)
						FF_SpawnBonus(x, y, 11)->user4 = -1;	// spawn it and mark it as an IF-Stunner
					break;

				case 35:
					FF_SpawnCheckpoint(x, y);
					NeedLump(CHECKPOINT);
					NeedLump(SMIRKYPOOF);
					CA_MarkGrChunk(FF_CHECKPOINTOVERLAYSPR);
					break;

				case 36:
					FF_SpawnLeaveaTrip(x, y);
					NeedLump(LEAVEA);
					break;

				case 37:
				case 38:
					FF_SpawnPatPat(x,y,infoValue-37);
					NeedLump(PATPAT);
					break;

				case 39:
					FF_SpawnWarble(x, y);
					NeedLump(WARBLE);
					break;

				case 40:
					// K1n9_Duk3 note: FOREGROUNDPRIORITY is 3, so both cases do the
					// same anyway and there is no need for an if-check here.
					/*
					if (ck_gameState.currentLevel == MAP_BWBMEGAROCKET)
					{
						SpawnStaticSprite(x, y, BWBSPR, 3);
						SpawnStaticSprite(x, y, BWBBACKSPR, 0);
					}
					else
					*/
					{
						FF_SpawnStaticSprite(x, y, FF_BWBSPR, FOREGROUNDPRIORITY);
						FF_SpawnStaticSprite(x, y, FF_BWBBACKSPR, 0);
					}
					// Note: SpawnStaticSprite marks the required chunk!
					break;

				case 41:
					// just cache the graphics - Keen's idle code spawns the object when needed
					NeedLump(EFREE);
					break;

				case 42:
					FF_SpawnNisasi(x, y);
					NeedLump(NISASI);
					NeedLump(YETICLOUD);	// for the smoke cloud sprites
					break;

				case 43:
					// this is the cloud start marker, search for the cloud end marker in the same row
					for (j=1; j+x < CA_GetMapWidth(); j++)
					{
						if (FF_MapAt(2, j+i) == 44)
						{
							// found the end marker, so remove it from the map to avoid the error message
							CA_SetTileAtPos((j + i) % CA_GetMapWidth(), (j + i) / CA_GetMapWidth(), 2, 0);
							// spawn an invisible cloud object with the given hitbox (everything is in "global" units!)
							FF_SpawnInvisibleCloud(RF_TileToUnit(x), RF_TileToUnit(y), RF_TileToUnit(j+1), 11*16);
							goto found_cloud_end;
						}
					}
					// couldn't find a cloud end marker, indicating an error in the level design
					Quit("Foray in the Forest: BADCLOUDSTART");
found_cloud_end:
					break;
				case 44:
					// found a cloud end marker that hasn't been used, indicating an error in the level design
					Quit("Foray in the Forest: BADCLOUDEND");

				// 45 is handled below (along with 80)

				case 46:
					FF_SpawnWorldNessie(x, y);
					CA_MarkGrChunk(FF_WORLDNESSIE);
					break;

				case 47:
					FF_SpawnFrog(x, y);
					CA_MarkGrChunk(FF_FROGSPR);
					break;

				case 48:
					FF_SpawnCouncilPage(x, y, 1);
					NeedLump(PAGE);	
				/*
					SpawnJediBoot(x, y);
				*/
					CA_MarkGrChunk(FF_JEDIBOOTSPR);
					break;

				case 49:
					// maps with this scrolling type don't need the invisible 2-tile border
					if (!ff_map.wrapScroll)
					{
						// TODO KEEN WORLDS: its wrap-around scrolling (the map's
						// border is opened up and the scroll blocks cleared)
						ff_map.wrapScroll = true;
					}
					break;

				case 50:
					FF_SpawnCouncilPage(x, y, 0);
					FF_SpawnSlideDoor(x, y, 0);
					NeedLump(PAGE);
					NeedLump(PAGEDOORS);
					break;

				case 51:
					NeedLump(POLEKNAWEL);
					FF_SpawnCloneMachine(x, y, 0, IN_motion_Right);
					NeedLump(SCANNER);
					break;

				case 52:
					NeedLump(MOLET);
					FF_SpawnCloneMachine(x, y, 1, IN_motion_Right);
					NeedLump(SCANNER);
					break;

				case 53:
					NeedLump(CHEMOLAIN);
					FF_SpawnCloneMachine(x, y, 2, IN_motion_Right);
					NeedLump(SCANNER);
					break;

				case 54:
					NeedLump(NISASI);
					FF_SpawnCloneMachine(x, y, 3, IN_motion_Right);
					NeedLump(SCANNER);
					break;

				case 55:
					NeedLump(COUNCIL);
					FF_SpawnCloneMachine(x, y, 4, IN_motion_Right);
					NeedLump(SCANNER);
					break;

				case 56:
					//SpawnCouncilMember(x, y, 0, true);	
					FF_SpawnCouncilMember(x, y, 0);	// ALWAYS spawn!   // Nisaba patch
					NeedLump(COUNCIL);
					break;

					// Items
				case 57:
				case 58:
				case 59:
				case 60:
				case 61:
				case 62:
				case 63:
				case 64:
				case 65:
				case 66:
				case 67:
				case 68:
					FF_SpawnBonus(x,y,infoValue-57);	
					break;

					// new object types added by K1n9_Duk3:
				case 69:
					FF_SpawnDungBeetle(x, y);
					NeedLump(DUNGBEETLE);
					break;

				case 70:
					FF_SpawnChemolain(x, y);
					NeedLump(CHEMOLAIN);
					NeedLump(KEENKNOCKBACK);
					break;

				case 71:
				case 72:
				case 73:
					FF_SpawnLogPlatform(x, y, infoValue-72);
					CA_MarkGrChunk(FF_LOGSPR);	// don't really need to create a lump for this
					break;

				case 74:
					FF_SpawnSalamander(x, y);
					NeedLump(SALAMANDER);
					break;

				case 75:
					FF_SpawnShrubbery(x, y);
					NeedLump(SHRUBBERY);
					NeedLump(KEENKNOCKBACK);
					break;

				case 76:
				case 77:
				case 78:
					FF_SpawnCoronaPlant(x, y, infoValue-77);
					NeedLump(CORONA);
					break;

				case 79:
					FF_SpawnNipnap(x, y);
					NeedLump(NIPNAP);
					break;

				case 45:
				case 80:
					FF_SpawnCrab(x, y, infoValue & 1);
					NeedLump(CRAB);
					break;

				// 81 was handled above (along with 2)

				case 82:
				case 83:
				case 84:
				case 85:
				case 86:
				case 87:
					FF_SpawnCollectible(x, y, infoValue-82);
					// Note: SpawnCollectible marks the required chunks!
					break;
				
				case 88:
					FF_SpawnObelisk(x, y);
					CA_MarkGrChunk(FF_OBELISKSPR);
					break;

				case 89:
					FF_SpawnLindsey(x, y, 1);
					NeedLump(LINDSEY);
					NeedLump(SMIRKYPOOF);
					break;

				case 90:
					FF_SpawnDonut(x, y);
					NeedLump(FLYDONUT);
					break;

				// 91 to 98 are direction arrows

				case 99:
				case 100:
				case 101:
				case 102:
				case 103:
				case 104:
				case 105:
				case 106:
				case 107:
				case 108:
				case 109:
				case 110:
				case 111:
				case 112:
				case 113:
				case 114:
					//SpawnCouncilMember(x, y, infoValue-99, false);	
					FF_SpawnCouncilMember(x, y, infoValue-99);  // Nisaba patch
					NeedLump(COUNCIL);
					break;

				case 115:
					FF_SpawnFallBlock(x, y);
					NeedLump(SANDSTONE);
					break;

				case 116:
					FF_SpawnMummySpawner(x, y);
					NeedLump(MUMMY);
					break;
					
				case 117:
					FF_SpawnLemonShark(x, y);
					NeedLump(LEMONSHARK);
					break;
					
				case 118:
					FF_SpawnDonutCloud(x, y);
					CA_MarkGrChunk(FF_DONUTCLOUDSPR);
					CA_MarkGrChunk(FF_DONUTCLOUDSHADOWSPR);
					break;
			/*	
				case 119:
					SpawnDonutCloudShadow(x, y);
					CA_MarkGrChunk(DONUTCLOUDSHADOWSPR);
					break;
			*/
				case 120:
					FF_SpawnJediBoot(x, y);
					CA_MarkGrChunk(FF_JEDIBOOTSPR);
					break;
					
				case 121:
					FF_SpawnSlugCans(x, y);
					NeedLump(SLUGCANS);
					break;
					
				case 122:
					FF_SpawnBigShroom(x, y);
					NeedLump(SMIRKYPOOF);
					break;
					
				// Nisaba patch: Donut-Slug
				case 123:
					FF_SpawnSlug(x, y);
					ck_lastObject->user1 = 1;	// slug carries a donut!
					NeedLump(SLUG);
					ff_map.itemsTotal++;
					break;
			}

			// K1n9_Duk3 addition (for level statistics):
			infoValue = TI_ForeMisc(FF_MapAt(1, i)) & INTILE_TYPEMASK;
			switch (infoValue)
			{
			case MF_Points100:
			case MF_Points200:
			case MF_Points500:
			case MF_Points1000:
			case MF_Points2000:
			case MF_Points5000:
				//ck_mapState.pointsTotal += bonuspoints[infoValue+4-MF_Points100];
				ff_map.itemsTotal++;
				break;
		/*
			case MF_1UP:
				ck_mapState.livesTotal++;
				break;
			case MF_Stunner:
				ck_mapState.ammoTotal++;
				break;
			case MF_Centilife:
				ck_mapState.dropsTotal++;
				break;
		*/
			}
			// end of addition
		}
	}

	// Inactivate all objects
	for (ob = ck_keenObj; ob != NULL; ob = ob->next)
	{
		if (ob->active != OBJ_ALWAYS_ACTIVE)
			ob->active = OBJ_INACTIVE;
	}

}
