"""Write Keen Worlds' variable files for Revival of Bliss, both builds.

    python3 kwtools/kw_revival.py REVIVALDIR omnispeak/data/keen4 play/mods

REVIVALDIR: the player's copy of Aurora Bliss III: Revival of Bliss v1.1
(Dave216, 2024; the files of m.dave216.revival_of_bliss_v.1.1.k4.2024-06-
08.zip, unpacked). It is two Keen 4 mods side by side, chosen by a DOS
launcher: KEEN/ (play as Keen) and CRAIG/ (play as Craig), each with its own
levels, graphics, music and a CKPatch file (abiathar.pat). This copies them
to play/mods/revival and play/mods/revivalc (the Keen build has no graphics
dictionary of its own: it is the Craig build's, Aurora Bliss's) and writes
each one's MOD.CK4. Texts, level names and the music table are turned into
settings as for The Eight Accumulators (kw_accumulators.py); each change to
the program's rules is a setting in the engine's Keen 4 code, listed in
RULES with the patch it stands for; the routines it replaces are in
src/kw_revival.c (and two of Operation Ocflore's, the same patches), named
in STAND_INS for the sprite actions that use them, written out as %action
lines (kw_patconv.py). Audit: research/ocflore-and-aurora-bliss.md in the
workspace root.
"""
import contextlib, io, os, shutil, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert
import kw_patconv

FILES = ['AUDIO.CK4', 'AUDIODCT.CK4', 'AUDIOHED.CK4', 'EGAGRAPH.CK4', 'EGAHEAD.CK4', 'GAMEMAPS.CK4']
RENAMED = {'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}
PAT = 'abiathar.pat'

STAND_INS = ['OC_SpriteCol=2EE7:0146', 'OC_SkypestCol=037D:054D', 'OC_FireballDraw=CK4_FireballDraw',
             'RV_BossCol=CK4_DopefishCol', 'RV_BossMove=CK4_DopefishMove', 'RV_BossDefeated=11A2:0A31',
             'RV_SmirkyJump=CK4_SmirkySearch', 'RV_SmirkyCol=CK4_SmirkyCol', 'RV_SmirkyDraw=CK4_SmirkyDraw',
             'RV_InchwormCol=CK4_InchwormCol', 'RV_SchoolfishCol=1080:07C1', 'RV_SchoolfishStunnedCol=1080:07F5']
CRAIG_STAND_INS = ['RV_CraigShotDraw=0D8F:0E32']

# Both builds: (the patches' places, what they do, the setting)
COMMON = [
    ((0x8CEF, 0x8C91, 0x8C75, 0x8C83, 0x8D83, 0x8CA4, 0x20DF0, 0x354F6), 'its own sound and music: 25 songs from chunk 137',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"\n%int ca_audStartMusic 137\n%int ca_audNumSongs 25'),
    ((0xF4CA,), 'the Ancients talk over level 0\'s song', '%int CK4_CouncilTalkMusicLevel 0'),
    ((0xA21E,), "K1n9_Duk3's fix: the floor check ends at the map's top", '%int CK_ClipToEndsSigned 1'),
    ((0xF107,), "the Janitor talks over song 19", '%int CK4_JanitorTalkMusicLevel 19'),
    ((0x9CD7,), 'the ending plays song 19', '%int CK_EndingMusicLevel 19'),
    ((0xCC2A,), 'no underwater level (its death check is for a level 22 there is none of)', '%int CK4_UnderwaterDeathLevel 22'),
    ((0x905B,), 'the Well of Wishes exits and scrolls normally', '%int CK4_UnderwaterCameraLevel 99'),
    ((0x60FD,), 'levels 17 and up are its ship levels', '%int ck_maxDoneLevel 16'),
    ((0xEFB6, 0xF033), "Lindsey's Miragia hint is in level 17", '%int CK4_LindseyMessage1Level 17'),
    ((0xBC29,), 'Keen moons as often as he likes (its mooning: actions)', '%int CK4_MoonAgain 1'),
    ((0xD2FB, 0xD3B9, 0xD324, 0xD3DD), 'right-hand conveyor belts no longer carry Keen', '%int CK_ConveyorRightSpeed 0'),
    ((0x10B5C, 0x10B53), "the cloud's lightning starts at the cloud", '%int CK4_CloudBoltXOffset 0\n%int CK4_CloudBoltYOffset 0'),
    ((0x10A80,), 'the cloud stands still while it waits to strike', '%int CK4_CloudStrikeXAccel 0'),
    ((0x10B7D,), 'touching the cloud makes it strike', '%action CK4_ACT_CloudTouched = CK4_ACT_CloudStrike0'),
    ((0xDE43, 0xDE58, 0xDE78, 0xDE8C, 0xDE4A, 0xDE5F), "the foot's flight (signed speeds, as Omnispeak's are)",
     None),   # (per build, below)
    ((0xF0E6, 0xF0ED, 0xF0F4, 0xCD91, 0xF113, 0xF117, 0xF16E, 0xF123, 0xF146, 0xF14A, 0xF13A, 0xF13F, 0xF144,
      0xF184, 0xF188, 0xF1DF, 0xF194, 0xF1B7, 0xF1BB, 0xF1AB, 0xF1B0, 0xF1B5, 0xF1F5, 0xF1F9, 0xF252, 0xF205, 0xF22A,
      0xF22E, 0xF228, 0xF223, 0xF214, 0xF278, 0xF2D8, 0x1F580, 0x1F5E0, 0x1F630, 0x1F650),
     "the Janitor: in level 17, four windows (the orb, Craig, Keen's answer, Craig)",
     '%int CK4_JanitorLevel 17\n%int CK4_JanitorWindows 4\n%int CK4_JanitorPic 77\n%int CK4_JanitorPic2 111\n'
     '%int CK4_JanitorKeenPic 113'),
    ((0xFD54,), 'the Ancients stand still', '%int CK4_CouncilNeverPauses 1'),
    ((0x126A6,), "the Sprite's shot starts on its second frame (the first is its stunned picture)", '%int CK4_SpriteShotSkipFirst 1'),
    ((0x125D1,), 'Sprites float slower and less far up', '%int CK4_SpritePatrolYAccel 4\n%int CK4_SpritePatrolYRadiusTop 0x10'),
    ((0x12690,), 'a Sprite shoots with the sound of Keen\'s stunner', '%int CK4_SpriteShootSound 0x2A'),
    ((0x11EEB, 0x11F05), 'Mimrocks follow Keen whichever way he faces', '%int CK4_MimrockIgnoresFacing 1'),
    ((0x11F3B, 0x11F2F), 'once walking a Mimrock never sits down again', '%int CK4_MimrockAlwaysChases 1'),
    ((0x11F65, 0x11F5B, 0x11F53), "the Mimrock's jump", '%int CK4_MimrockJumpYVelocity -0x27\n%int CK4_MimrockJumpXVelocity 8\n'
     '%int CK4_MimrockJumpXRadius 0x200'),
    ((0x11EA6,), 'Mimrocks start out walking', '%action CK4_ACT_MimrockStart = CK4_ACT_MimrockWalk0'),
    ((0x5836,), None, None),   # (the story's song, per build)
    ((0x581B, 0x5814, 0x2F05E), "the story's colours", None),   # (ck_starWarsPalette, written below)
    ((0x107B4,), 'Skypests bounce off the ground instead of landing', '%int CK4_SkypestBounces 1'),
    ((0xF50D,), 'the last Ancient gives the wetsuit (the ID chip; Craig: his pogo stick), with its own words',
     '%int CK4_LastCouncilGivesWetsuit 1'),
    ((0xFF8E, 0xFF92), 'the Mad Mushroom always makes its big jump', '%int CK4_MushroomNumJumps 1'),
    ((0xFEE3,), 'Keen is drawn behind the Mad Mushroom', '%int CK4_MushroomZLayer 3'),
    ((0x3D5E, 0x3CE5, 0x1D130, 0x3CF6), 'its own palette (kw_world.c)', None),
    ((0x10219,), 'the blue bird never flies', '%int CK4_BirdNeverFlies 1'),
    ((0x1030A,), 'a shot just pops on the hatched bird', '%int CK4_BirdShotProof 1'),
    ((0x100B5, 0x100BF), "shots fly through the Madness's egg; it hatches with a sound", '%int CK4_EggIgnoresShots 1\n%int CK4_EggHatchSound 0x24'),
    ((0x11A4C,), 'Keen is drawn in front of the Treasure Eater', '%int CK4_SmirkyZLayer 0'),
    ((0x11A42,), 'the Treasure Eater is ridden like a bounder', '%int CK4_SmirkyType 15'),
    ((0x11166, 0x11172), 'a shot only makes a bounder hop', '%int CK4_BounderUnstunnable 1'),
    ((0x111AE,), 'bounders make no sound', '%int CK4_BounderQuiet 1'),
    ((0x111BC,), 'bounders slide (a hop of 1)', '%int CK4_BounderBounceYVelocity -1'),
    ((0x111C2,), 'bounders go where Keen is', '%int CK4_BounderFollowsKeen 1'),
    ((0xB784, 0xB787), "where a stunned Devil Foot's stars are", '%int CK4_InchwormStarsX 0x40\n%int CK4_InchwormStarsY -0x80'),
    ((0x124E5,), 'Muha (the schoolfish) clips normally', '%int CK4_SchoolfishClip 1'),
    ((0xEB81,), None, None),   # (it marks the old smoke's sprites: Omnispeak caches what a level shows anyway)
    ((0x3DA8, 0x120EF), 'Allain, the boss: health by difficulty', '%int CK4_DopefishIsBoss 1'),
    ((0xDA3F, 0xDB2A, 0xDBD6, 0xDC61, 0x1241C, 0x12197), 'the score box shows his health in level 16', '%int CK4_BossBarLevel 16'),
    ((0xB802,), 'no stars over the beaten boss', '%int CK4_DopefishStarsOff 1'),
    ((0x6ACB, 0x6BD1, 0x12451), 'beating him wins the game; rescued Ancients only end their level',
     '%int CK4_CouncilNeverWins 1'),
    ((0x105A9, 0x105D5), "a shot stuns an Arachnut", '%action CK4_ACT_ArachnutStunned = CK4_ACT_ArachnutRecover2'),
    ((0x3D8F, 0x10544), 'how long an Arachnut stays stunned, by difficulty', '%int CK4_ArachnutStunTimed 1\n%intarray CK4_ArachnutStunTimes 0x75, 0x28, 0x14'),
    ((0x10824,), 'Keen is drawn behind the Wormmouth', '%int CK4_WormmouthZLayer 3'),
    ((0x10DB5,), 'the Berkeloid floats no more (its own float is its walking animation)', '%int CK4_BerkeloidNoHover 1'),
    ((0x10E7D,), "it doesn't turn at ledges", '%int CK4_BerkeloidNoEdgeTurn 1'),
    ((0xEB4B, 0xEB55), "the Dopefish's normal and hard markers are Muha's", '%int CK4_DopefishIconsSpawnSchoolfish 1'),
    ((0xCEC1,), "bounders (and what rides like them) don't push Keen off poles", '%int CK4_BounderNoPolePush 1'),
    ((0xEDF0, 0xEDF9), "the blue bird's markers are Devil Feet", '%int CK4_BirdIconsSpawnInchworm 1'),
    # Their routines (kw_revival.c, and Operation Ocflore's Sprite, Skypest,
    # flame) and actions (kw_patconv.py writes them).
    ((0x2EFB6, 0x3D1D, 0x11A9C, 0x11AD5, 0x11CA4, 0x11D63, 0x11E2C, 0x10F9C, 0x10FC1, 0x123DD, 0x10D5E), None, None),
    # Not settings: the DOS program's memory, screen update, audio loading
    # and text locking; the font fix; help, menu and terminator music and
    # colours (Keen Worlds has its own); its E-N-D cheat; the loading screen.
    ((0x61DD, 0x5C4B, 0x157C2, 0x18793, 0x9BA8, 0x9CBC, 0x13F4B, 0x354EA, 0x1D383, 0x1E7F9, 0x1E87A, 0x13466,
      0x13373, 0x992D, 0x9A76, 0x9A72, 0x9A8B, 0x9A87, 0x9B8C, 0x9B88, 0x9B5F, 0x9B5B, 0x1CC6F, 0x1CC6B, 0x2F078,
      0x9139, 0x1FE47), None, None),
]
BUILDS = {
    'revival': dict(folder='KEEN', title="Revival of Bliss, as Keen (Aurora Bliss III: Dave216, 2024, v1.1)", rules=[
        ((0x5836,), 'the story scrolls to song 16', '%int CK_StarWarsMusicLevel 16'),
        ((), "Allain's health on easy, normal, hard ($3DBF)", '%intarray CK4_DopefishHealth 0x19, 0x1E, 0x28'),
        ((0xDE43, 0xDE58, 0xDE78, 0xDE8C), "the foot's flight",
         '%int CK4_FootThereX 0x0200\n%int CK4_FootThereY 0x3700\n%int CK4_FootBackX 0x7200\n%int CK4_FootBackY 0x3100'),
        ((0xBC32,), None, None),
    ]),
    'revivalc': dict(folder='CRAIG', title="Revival of Bliss, as Craig (Aurora Bliss III: Dave216, 2024, v1.1)", rules=[
        ((0x5836,), 'the story scrolls to song 8', '%int CK_StarWarsMusicLevel 8'),
        ((), "Allain's health on easy, normal, hard ($3DBF)", '%intarray CK4_DopefishHealth 0x10, 0x14, 0x25'),
        ((0xDE43, 0xDE58, 0xDE78, 0xDE8C), "the foot's flight",
         '%int CK4_FootThereX 0x0200\n%int CK4_FootThereY 0x3700\n%int CK4_FootBackX 0x3900\n%int CK4_FootBackY 0x3700'),
        ((0xE206,), "Craig can't swim on the map (his wetsuit is the pogo stick)", '%int CK4_MapNeverSwim 1'),
        ((0x8B43, 0x8B3C, 0x8B7F, 0x8B4A), 'no pogo stick until the eight repairs are done', '%int CK_PogoNeedsWetsuit 1'),
        ((0xE59E, 0xE5A7, 0xE5D0, 0xE5CB, 0xE5E0, 0xE5DB, 0xE5F0, 0xE5EB, 0xE600, 0xE5FB, 0xE607),
         'his gun throws its shots (up, right, down, left)',
         '%int CK_ShotArc 1\n%intarray CK_ShotArcVel 0, -0x4C, 0x30, -0x18, 0, 0x20, -0x40, -0x18'),
        ((0xE57E,), 'and never runs out', '%int CK_ShotsFree 1\n%int ck_scoreBoxHideShots 1'),
        ((0xE722,), 'his shots burst on walls, floors and ceilings (RV_CraigShotDraw)', None),
        ((0x98F5, 0x98F9, 0x98FD, 0x99F8, 0x95FB, 0x95FF, 0x9603), 'black help pages', '%int CK_HelpBarColour 0'),
        ((0x11179,), 'a shot bounder flies a little higher', '%int CK4_BounderDeathHopVelocity -0x65'),
        ((0x111FE, 0x11260), 'bounders stand still', '%int CK4_BounderBounceXVel 0\n%int CK4_BounderRideXVel 0'),
        # Not settings: the loading screen's thumbs, the help window's box.
        ((0x6478, 0x647F, 0x640D, 0x6414, 0x92C6), None, None),
    ]),
}
# (the Keen build's bounders slide at 16, as Aurora Bliss's)
BUILDS['revival']['rules'].append(((0x111FE, 0x11260), 'bounders slide at 16', '%int CK4_BounderBounceXVel 16\n%int CK4_BounderRideXVel 16'))

PALETTE = 0x3D5E   # K1n9_Duk3's palette patch: 16 colours, r g b 0-63 (in the Keen build: kw_world.c has both)
STORY_PALETTE = 0x2F05E


def late(setting):
    return setting.startswith('%action') or '@' in setting


def build(gid, src, keen4, mods):
    b = BUILDS[gid]
    game = os.path.join(src, b['folder'])
    out = os.path.join(mods, gid)
    os.makedirs(out, exist_ok=True)
    for name in FILES:
        shutil.copyfile(os.path.join(game, name), os.path.join(out, name))
    for name, to in RENAMED.items():
        shutil.copyfile(os.path.join(game, name), os.path.join(out, to))
    shutil.copyfile(os.path.join(src, 'CRAIG', 'EGADICT.CK4'), os.path.join(out, 'EGADICT.CK4'))
    rules = COMMON + b['rules']
    exe, pat = os.path.join(game, 'KEEN4E.EXE'), os.path.join(game, PAT)
    g = kw_patconv.Game(exe, keen4)
    pats = kw_patconv.parse_pat(pat)
    acts, _, _ = kw_patconv.classify(g, pats)
    handled = [off for off, data in pats if any(g.dseg + o <= off < g.dseg + o + kw_patconv.ACTION_SIZE for o in acts)]
    extra = []
    for places, what, setting in rules:
        handled += places
        if setting and not late(setting):
            extra += ['# ' + what] + setting.split('\n')
    patched = g.patched(pats)
    story = ', '.join('0x%02X' % c for c in patched[STORY_PALETTE:STORY_PALETTE + 17])
    extra += ["# the story's colours ($2F05E)", '%intarray ck_starWarsPalette ' + story]
    convert(game, keen4, out, pat=PAT, files={}, extra=extra, handled=handled,
            skip=('keen4.tli', 'ck4load.bin', 'audiohed.ck4', 'audiodct.ck4'),
            title=b['title'], tool='kwtools/kw_revival.py')
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', exe, pat, keen4] + STAND_INS + (CRAIG_STAND_INS if gid == 'revivalc' else [])
        kw_patconv.main()
    actions = [l.split('  #')[0] for l in buf.getvalue().splitlines() if l.startswith('%action')]
    with open(os.path.join(out, 'MOD.CK4'), 'a', encoding='latin1') as f:
        f.write('\n# Its sprite actions, as the patches leave them (our routines named as in\n'
                '# kwtools/kw_revival.py STAND_INS)\n' + '\n'.join(actions) + '\n')
        f.write('\n# Settings that name Keen 4\'s own (so after its file)\n')
        for places, what, setting in rules:
            if setting and late(setting):
                f.write('# %s\n%s\n' % (what, setting))
    print('%s: %d actions' % (gid, len(actions)))


def main(src, keen4, mods):
    for gid in BUILDS:
        build(gid, src, keen4, mods)


if __name__ == '__main__':
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    main(*sys.argv[1:])
