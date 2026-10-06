"""Write Keen Worlds' variable file for Bounty Hunting Brouhaha.

    python3 tools/kw_bounty.py bounty/game omnispeak/data/keen4 play/mods/bounty

Bounty Hunting Brouhaha (Kohntarkosz_, 2019) is a Keen 4 mod: its own
levels, graphics and music, and a CKPatch file that changes texts and a
number of the program's rules. The texts, level names and music table are
turned into settings as for The Eight Accumulators (tools/kw_accumulators.py
does the work). Each change to the program's rules has a setting in the
engine's Keen 4 code, listed in RULES below with the patch it stands for.
"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert

FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'AUDIODCT.CK4': 'AUDIODCT.CK4', 'AUDIOHED.CK4': 'AUDIOHED.CK4',
         'EGAGRAPH.CK4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4', 'GAMEMAPS.CK4': 'GAMEMAPS.CK4',
         'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}
# (the patches' places in the program, what they do, the setting)
RULES = [
    ((0x60FD,), 'levels 16 and 17 can be entered again', '%int ck_maxDoneLevel 15'),
    ((0x9058, 0xCC2A), 'the water level is 13', '%int CK4_UnderwaterCameraLevel 13\n%int CK4_UnderwaterDeathLevel 13'),
    ((0xD1FA,), 'the moon tile ends the level', '%int CK4_MoonTileEndsLevel 1'),
    ((0x11792,), 'platforms without flames', '%int CK4_PlatformNoFlames 1'),
    ((0x105B0,), 'the Arachnut does not kill', '%int CK4_ArachnutHarmless 1'),
    ((0x123F9,), 'the Dopefish does not eat Keen', '%int CK4_DopefishIgnoresKeen 1'),
    ((0xE208,), 'Keen always swims on the map', '%int CK4_MapAlwaysSwim 1'),
    ((0x8058, 0x80BE, 0x80DC), 'no wetsuit in the status window', '%int CK4_StatusNoWetsuit 1'),
    ((0x7E32, 0x7E4D, 0x7E6D), 'no Council in the status window', '%int CK4_StatusNoCouncil 1'),
    ((0x6BD9, 0x12A62), 'what is found in place of the wetsuit wins the game', '%int CK4_WetsuitWinsGame 1'),
    ((0xDE36, 0xDE43, 0xDE58, 0xDE78, 0xDE8C), 'where the foot flies',
     '%int CK4_FootThresholdX 0x3000\n%int CK4_FootThereX 0x2900\n%int CK4_FootThereY 0x2600\n'
     '%int CK4_FootBackX 0x3100\n%int CK4_FootBackY 0x4300'),
    ((0xFAF8, 0xFB21, 0xFB49, 0xFB71, 0xFB99, 0xFBC1, 0xFBE9, 0xFC11), 'where the mirage\'s tiles are in the map',
     '%int CK4_MiragiaSourceY 80'),
    ((0xFD54,), 'the witch doctor does not stop to think', '%int CK4_CouncilNeverPauses 1'),
    ((0x7893,), 'the item cheat gives no keys', '%int CK_ItemCheatNoKeys 1'),
    ((0x8CEF, 0x8C75, 0x8C83, 0x8D83), 'its own sound and music headers',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"'),
    # Not turned into settings: the title sequence without demos and story
    # (a traveller never sees it), and two of the main menu's lines.
    ((0x3CBB, 0x3CBD, 0x3CBF, 0x3CC3, 0x3CC5, 0x33CCA), None, None),
]


def main(game, keen4, out):
    extra, handled = [], [0x1F0D0]   # (level 7's name is given outright below)
    for places, what, setting in RULES:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    convert(game, keen4, out, pat='KEEN4a.PAT', files=FILES, extra=extra, handled=handled,
            skip=('keen4.tli', 'audiohed.ck4', 'audiodct.ck4'),
            level_names={7: "King's Canyon"}, 
            title='Bounty Hunting Brouhaha (Kohntarkosz_, 2019)', tool='tools/kw_bounty.py')


if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
