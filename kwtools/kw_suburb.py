"""Convert the player's audited Suburb Shenanigans v1.2 for native Keen Worlds.

python3 kwtools/kw_suburb.py SOURCE KEEN5E.EXE OUTPUT
The DOS executable is read as data only. Original maps are copied unchanged.
"""
from pathlib import Path
import hashlib
import shutil
import re
import struct
import sys
from kw_accumulators import cstring, escape
from kw_patconv import Game, parse_pat, cmd_check

PATCH_HASH = 'f796595f0226e6188dd41d1405fc3f06b7031de2c775658cbc016e39e7b42ddb'
STAND_INS = [
    'SS_DoorOpen=037D:0520', 'SS_MomCol=06B9:01A6', 'SS_BagCol=06B9:0E68',
    'SS_BlockCol=06B9:0E83', 'SS_CheckpointCol=06B9:015E', 'SS_FinishCol=06B9:017E',
    'SS_CarHorizontal=0ED5:09DA', 'SS_CarVertical=0F92:12B5', 'SS_OldManShoot=1102:02C4',
    'SS_ChildMove=CK5_VolteMove', 'SS_ChildCol=CK5_VolteCol',
    'SS_HiveCol=CK5_ShellyCol', 'SS_SpawnBees=CK5_SpawnShellyBits',
    'SS_BeeDraw=CK5_SpherefulTileCol', 'SS_GardenerThink=CK5_KorathWalk',
    'SS_GardenerCol=CK5_KorathColFunc', 'SS_OldManThink=CK5_AmptonWalk',
    'SS_OldManCol=CK5_AmptonCol', 'SS_VacuumCol=CK5_RoboCol',
    'SS_VacuumThink=CK5_RoboMove', 'SS_VacuumShoot=CK5_RoboShoot',
    'SS_BullyShoot=CK5_MasterShoot', 'SS_MortimerShoot=CK5_ShocksundShoot',
    'SS_BalloonCol=CK5_BarkShotCol', 'SS_StunFallDraw=CK5_MasterTeleTileCol',
    'SS_StunStars=CK_BasicDrawFunc4',
]
EXTRA_ACTIONS = {0xF2:'SS_ACT_Checkpoint', 0x110:'SS_ACT_Finish', 0x12E:'SS_ACT_VacuumDead',
                 0x14D:'SS_ACT_Stunned', 0x1902:'SS_ACT_CarVertical'}
SETTINGS = r'''
%string ca_mapHead "maphead.ck5"
%string ca_tileInfo "Keen5.tli"
%string ca_audioHead "AUDIOHED.CK5"
%int ca_audioUncompressed 2
%int ca_audStartMusic 190
%int ca_audNumSongs 16
%int ck5_mapElevators 0
%int CK_MapPreserveCoordinates 1
%int ck_maxLevel 23
%int ck_maxEnterLevel 23
%int ck_maxDoneLevel 23
%int CK_NumMusicLevels 28
%int CK_EndingMusicLevel 26
%int CK_HelpMusicLevel 25
%int CK_GameOverMusicLevel 27
%int CK_StarWarsMusicLevel 27
%int CK_MaxObjects 124
%int CK5_ShikadiHealth 3
%int CK5_KorathSpawnYOffset -576
%int CK5_AmptonSpawnYOffset -496
%int CK5_ShocksundJumpYVel -80
%int CK5_ShocksundStandChance 8
%int CK5_SpindredFloorBounces 65535
%int CK5_SpindredFloorSlamVel 56
%int CK5_SpirogripLaunchChance 40
%int CK5_RoboNumShots 1
%int CK5_RoboShotXVelocity 30
%int CK5_RoboShotYVelocity 0
%int CK5_RoboShotRightXOffset 293
%int CK5_ShelleyBits0XVel 7
%int CK5_SecretFuseLevel -1
%string ck_str_exitToMap "Exit to Suburbs"
%string ck_str_levelWarp " Warp to which level(1-23):"
%string ck5_str_statusKeycard "BAG"
%string ck_str_statusCentilives "ENERGY"
'''


def convert(source, exe, output):
    source,output=Path(source),Path(output)
    files={p.name.lower():p for p in source.iterdir() if p.is_file()}
    if hashlib.sha256(files['pat.pat'].read_bytes()).hexdigest()!=PATCH_HASH:
        raise ValueError('Requires audited Suburb v1.2, 2020-11-25.')
    base=Path(__file__).resolve().parents[1]/'data/keen5'
    game=Game(str(exe),str(base));game.stand_ins(STAND_INS)
    patches=parse_pat(files['pat.pat']);image=game.patched(patches)
    names={**game.byoff,**EXTRA_ACTIONS}
    lines=['# Suburb Shenanigans v1.2, by Bubbatom; patches Levellass/K1n9_Duk3.',
           '# Native implementation: src/kw_suburb.c. Original maps unchanged.',SETTINGS]
    for name,table in [('ck_str_levelNames',0x31AB4),('ck_str_levelEntryText',0x31B18)]:
        words=[]
        for i in range(24):
            ptr=struct.unpack_from('<I',image,table+i*4)[0]
            words.append(cstring(image,(ptr>>16)*16+(ptr&65535)))
        lines.append('%stringarray '+name+'\n'+',\n'.join('"'+escape(t)+'"' for t in words))
    lines.append('%string ck_str_storyText "'+escape(files['story.ck5'].read_bytes().split(b'\0')[0].decode('latin1'))+'"')
    lines.append('%intarray CK_LevelMusic '+', '.join(map(str,struct.unpack_from('<28H',image,0x3067A))))
    for name,at,n in [('ck_starWarsPalette',0x304DE,16),('ck_terminatorPalette1',0x304F8,17),('ck_terminatorPalette2',0x30509,17)]:
        lines.append('%intarray '+name+' '+', '.join(map(str,image[at:at+n])))
    # The DOS cache ranges have changed independently of the picture names.
    graphics=(base/'GFXCHUNK.CK5').read_text()
    for table in ('lumpStarts','lumpEnds'):
        text=re.search(r'%intarray '+table+r'\s+([^%]+)',graphics)[1].split('#')[0]
        values=[game.ints[name] for name in re.findall(r'@(\w+)',text)]
        # DOS orders these tables differently from Omnispeak. Read the five
        # patched boundaries, retaining native ordering for all unchanged ones.
        changed={26:0x31A34,22:0x31A50,23:0x31A2E} if table=='lumpStarts' else {
            18:0x31A80,26:0x31A88,22:0x31AA4}
        for index,at in changed.items():values[index]=struct.unpack_from('<H',image,at)[0]
        lines.append('%intarray '+table+' '+', '.join(map(str,values)))
    lines.append('%override "EPISODE.CK5"')
    for off,name in sorted(names.items()):
        line=game.action_line(name,off,game.unpack(image,off),names)
        if '?' in line:raise ValueError('Unmapped action: '+line)
        lines.append(line)
    output.mkdir(parents=True,exist_ok=True)
    metadata=output/'SUBURB.CK5';metadata.write_text('\n\n'.join(lines)+'\n',encoding='latin1')
    assert cmd_check(game,patches,str(metadata))==0
    for name in ('AUDIO.CK5','AUDIOHED.CK5','EGAGRAPH.CK5','EGAHEAD.CK5','EGADICT.CK5',
                 'GAMEMAPS.CK5','maphead.ck5','Keen5.tli','PAT.PAT','changelog.txt','STORY.CK5'):
        shutil.copy2(files[name.lower()],output/name)
    for name in ('GFXINFOE.CK5','AUDINFOE.CK5'):
        shutil.copy2(base/name,output/name)
    print('Suburb v1.2 native data:',output)


if __name__=='__main__':
    if len(sys.argv)!=4:sys.exit(__doc__)
    convert(*sys.argv[1:])
