"""Write Keen Worlds' variable file for Operation Ocflore.

    python3 kwtools/kw_ocflore.py GAMEDIR omnispeak/data/keen4 play/mods/ocflore

GAMEDIR: the player's copy of Operation Ocflore v1.3 (Bubbatom, 2020; the
files of m.bubbatom.operation_ocflore_v1.3.k4.2020-02-18.zip, unpacked). It
is a Keen 4 mod: its own levels, graphics, music and sounds, and a CKPatch
file (Ocflore.PAT) that changes texts, many of the program's rules, and
replaces some of its routines. Texts, level names and the music table are
turned into settings as for The Eight Accumulators (kw_accumulators.py);
each change to the program's rules has a setting in the engine's Keen 4
code, listed in RULES with the patch it stands for; the routines it replaces
are in src/kw_ocflore.c, named in STAND_INS for the sprite actions that use
them, which are written out as %action lines (kw_patconv.py). Audit:
research/ocflore-and-aurora-bliss.md in the workspace root.
"""
import contextlib, io, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert
import kw_patconv

FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'AUDIODCT.CK4': 'AUDIODCT.CK4', 'AUDIOHED.CK4': 'AUDIOHED.CK4',
         'EGAGRAPH.CK4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4', 'GAMEMAPS.CK4': 'GAMEMAPS.CK4',
         'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}
PAT = 'Ocflore.PAT'

# Our routines for the program's (name=its function, or name=SEG:OFF of code
# the patches add), as kw_patconv.py takes them.
STAND_INS = ['OC_BounderCol=CK4_BounderCheckShot', 'OC_SlugCol=CK4_SlugCol', 'OC_LickCol=CK4_LickCol',
             'OC_LickMove=CK4_LickMove', 'OC_InchwormAway=CK4_InchwormMove', 'OC_FireballDraw=CK4_FireballDraw',
             'OC_SpriteCol=2EE7:0146', 'OC_SkypestCol=037D:055E', 'OC_InchwormMeet=037D:0526',
             'OC_WormRevert=1080:0793', 'OC_InchwormCol=1080:07B5', 'OC_KeenSquishCol=0B80:15CD']

# (the patches' places in the program, what they do, the setting)
RULES = [
    ((0x8CEF, 0x8C91, 0x8C75, 0x8C83, 0x8D83, 0x8CA4, 0x8CCD, 0x20DF0, 0x354F6), 'its own sound and music: 12 songs',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"\n'
     '%int ca_audStartMusic 150\n%int ca_audNumSongs 12'),
    ((0x6AE4,), 'three relics (council members) win the game', '%int CK4_NumCouncilsToRescue 3'),
    ((0x5836, 0x9CD7, 0xF107, 0xF4CA), 'music: none for the story, the ending plays level 13\'s, level 19\'s while the Ancient talks',
     '%int CK_StarWarsMusicLevel 0\n%int CK_EndingMusicLevel 13\n%int CK4_CouncilTalkMusicLevel 19'),
    ((0x7E4A, 0x7E42, 0x7E6A), "the relics' box in the status window",
     '%int CK4_StatusCouncilBoxX 95\n%int CK4_StatusCouncilBoxW 32\n%int CK4_StatusCouncilX 96'),
    ((0x60FD,), 'levels 18 and up are its ship levels', '%int ck_maxDoneLevel 17'),
    ((0xDE36, 0xDE43, 0xDE58, 0xDE78, 0xDE8C), 'the foot: where it flies, chosen by level 14 being done',
     '%int CK4_FootByLevelDone 14\n%int CK4_FootThereX 0x3300\n%int CK4_FootThereY 0x5000\n'
     '%int CK4_FootBackX 0x3000\n%int CK4_FootBackY 0x1700'),
    ((0x862B,), 'the underwater level ends at its left or right side', '%int CK4_UnderwaterExitsAtSides 1'),
    ((0xF622,), 'the swimming Keen clips normally', '%int CK4_SwimKeenClipNormal 1'),
    ((0xC1F2,), 'bridges four tiles thick', '%int CK_BridgeThickness 4'),
    ((0xA7B2, 0xA7B7), "the schoolfish's hitbox for walls", '%int CK4_SchoolfishClipX 0x180\n%int CK4_SchoolfishClipY 0x100'),
    ((0x10A36,), 'clouds move faster', '%int CK4_CloudXAccel 20'),
    ((0x12177, 0x1218B), 'the Dopefish seeks Keen upside down', '%int CK4_DopefishSeekReversed 1'),
    ((0xFF90, 0xFF97, 0xFFBB, 0xFF9C, 0xFF7C), "the Mad Mushroom's hops",
     '%int CK4_MushroomNumJumps 2\n%int CK4_MushroomHopYVel -0x18\n%int CK4_MushroomJumpYVel -0x4A'),
    ((0x111BC,), "the bounder's jump", '%int CK4_BounderBounceYVelocity -0x28'),
    ((0x111FE,), "the bounder's ride", '%int CK4_BounderRideXVel 16'),
    ((0x111C2,), 'bounders chase Keen', '%int CK4_BounderFollowsKeen 1'),
    ((0x126A6,), "the Sprite's shot starts on its second frame", '%int CK4_SpriteShotSkipFirst 1'),
    ((0x10797,), 'skypests never land; they slide along the ground', '%int CK4_SkypestNeverLands 1'),
    ((0x11F65, 0x11F5B), "the Mimrock's attack jump", '%int CK4_MimrockJumpYVelocity -0x20'),
    ((0x103D0, 0x1048B), 'flying birds never land', '%int CK4_BirdNeverLands 1'),
    ((0x10135,), 'eggs hatch into flying birds', '%action CK4_EggHatchAction = CK4_ACT_BirdFly0'),
    ((0x1006A,), 'the walking bird stands on the ground', '%int CK4_BirdSpawnYOffset 0xE1'),
    ((0x1027D, 0x10299, 0x102AD), "the flying bird's acceleration", '%int CK4_BirdFlyXAccel 32'),
    ((0xEECB, 0xEECD, 0xEECF, 0x10366, 0x10321), 'markers 36-38: flying birds on easy, normal, hard',
     '%int OC_FlyingBirdMarkers 1'),
    ((0x112E6, 0x11E85, 0x10ED0, 0x11A4C, 0xFD9C, 0xFFFB), 'which creatures Keen is drawn in front of',
     '%int CK4_LickZLayer 0\n%int CK4_MimrockZLayer 0\n%int CK4_InchwormZLayer 0\n%int CK4_SmirkyZLayer 1\n'
     '%int CK4_SlugZLayer 0\n%int CK4_EggZLayer 1'),
    ((0xB7EE, 0xB768, 0xB7FC, 0xB772, 0xB7E2, 0xB784, 0xB7F6, 0xB764, 0xB767), 'where the stun stars are',
     '%int CK4_SkypestStarsX 0x40\n%int CK4_SkypestStarsY -0x80\n%int CK4_LickStarsX 0x40\n%int CK4_LickStarsY -0x15E\n'
     '%int CK4_SlugStarsX -0x80\n%int CK4_SlugStarsY -0x80\n%int CK4_InchwormStarsX -0x80\n'
     '%int CK4_BounderStarsX 0xC0\n%int CK4_BounderStarsY -0x80\n%int CK4_SmirkyStarsX 0xC0\n%int CK4_SmirkyStarsY -0x80'),
    ((0xB55C, 0xB60E, 0x10E17), "walkers turning round undo their step's height too (no sticking at slope edges)",
     '%int CK_WalkTurnUndoesY 1'),
    # Its routines, ours in kw_ocflore.c (the actions name them): the
    # bounder's, slug's, Sprite's, skypest's, lick's collisions and the
    # lick's move, the inchworms' (they keep away from Keen; two that meet
    # become an Arachnut), Keen squashing them on his pogo stick, flames
    # bursting on walls.
    ((0x11138, 0xFE6A, 0x2EFB6, 0x3D2E, 0x11423, 0x1140D, 0x11342, 0x10F70, 0x3CF6, 0x3D13, 0x10FB5,
      0x11064, 0x10F93, 0xCDCD, 0x10D5E), None, None),
    # Not settings: the loading screen; the title without its story; the
    # DOS program's memory (lumps, caches, freeing), video timing, direct
    # audio loading; its palette is set in kw_world.c; the intro's text
    # palette; the compressed copy of Keen's collision routine (the same
    # rules, made smaller to free space); the wetsuit string moved.
    ((0x3CBD, 0x3CE5, 0x61DD, 0x5C4B, 0xEAEA, 0xEF19, 0xEA9A, 0xE9E0, 0xEA1E, 0x1E7F9, 0x1E87A, 0x1E84C,
      0x39DB, 0x7A69, 0x2EFF0, 0x13466, 0x13373, 0x2F078, 0x2F089, 0xCCA9, 0xCD54, 0x2F036, 0x808E,
      0x5A09, 0x5B58, 0x5B63, 0x5B89, 0x5C06, 0x194FF, 0x196A9, 0x6B6C, 0x6B70, 0x6B81, 0x1F040, 0x1F0D0), None, None),
]


def actions(game, keen4):
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', os.path.join(game, 'KEEN4E.EXE'), os.path.join(game, PAT), keen4] + STAND_INS
        kw_patconv.main()
    return [l.split('  #')[0] for l in buf.getvalue().splitlines() if l.startswith('%action')]


def main(game, keen4, out):
    extra, handled = [], []
    for places, what, setting in RULES:
        handled += places
        if setting and not setting.startswith('%action'):
            extra += ['# ' + what] + setting.split('\n')
    convert(game, keen4, out, pat=PAT, files=FILES, extra=extra, handled=handled,
            skip=('keen4.tli', 'ck4load.bin', 'audiohed.ck4', 'audiodct.ck4', 'egahead.ck4', 'maphead.ck4'),
            title='Operation Ocflore (Bubbatom, 2020, v1.3)', tool='kwtools/kw_ocflore.py',
            level_names={7: 'Pelagic Pillars'})   # (Omnispeak's own spelling of level 7 differs)
    acts = actions(game, keen4)
    with open(os.path.join(out, 'MOD.CK4'), 'a', encoding='latin1') as f:
        f.write('\n# Its sprite actions, as the patches leave them (our routines named as in\n'
                '# kwtools/kw_ocflore.py STAND_INS)\n' + '\n'.join(acts) + '\n')
        for places, what, setting in RULES:
            if setting and setting.startswith('%action'):
                f.write('# %s\n%s\n' % (what, setting))
    print('%d actions' % len(acts))


if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
