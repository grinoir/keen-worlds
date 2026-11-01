"""Write Keen Worlds' variable file for Canmore's Redemption.

    python3 kwtools/kw_canmore.py GAMEDIR KEEN5E.EXE omnispeak/data/keen5 play/mods/canmore

GAMEDIR: the player's copy of Aurora Bliss II: Canmore's Redemption v1.0.2
(Dave216, 2023; the files of m.dave216.canmores_redemption_v.1.0.2.k5.
2023-07-30.zip, unpacked). It is a Keen 5 mod: its own levels, graphics,
music and sounds, and a CKPatch file (abiathar.pat) that changes texts, many
of the program's rules, and replaces some of its routines. It doesn't ship
the program it patches: KEEN5E.EXE is Keen 5 v1.4 (the player's own; it is
only read). Texts, level names and the music table are turned into settings
as for The Eight Accumulators (kw_accumulators.py, episode 5); each change
to the program's rules has a setting in the engine's Keen 5 code, listed in
RULES with the patch it stands for; the routines it replaces are in
src/kw_canmore.c, named in STAND_INS for the sprite actions that use them,
which are written out as %action lines (kw_patconv.py). Audit:
research/ocflore-and-aurora-bliss.md in the workspace root.
"""
import contextlib, io, os, re, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert, cstring, escape
import kw_patconv

FILES = {'AUDIO.CK5': 'AUDIO.CK5', 'AUDIODCT.CK5': 'AUDIODCT.CK5', 'AUDIOHED.CK5': 'AUDIOHED.CK5',
         'EGAGRAPH.CK5': 'EGAGRAPH.CK5', 'EGAHEAD.CK5': 'EGAHEAD.CK5', 'GAMEMAPS.CK5': 'GAMEMAPS.CK5',
         'MAPHEAD.CK5': 'MAPHEAD.CK5', 'Keen5.tli': 'KEEN5.TLI'}
PAT = 'abiathar.pat'

# Our routines for the program's (name=its function, or name=SEG:OFF of code
# the patches add), as kw_patconv.py takes them.
STAND_INS = ['CR_SparkyCol=CK5_SparkyCol', 'CR_AmptonCol=CK5_AmptonCol', 'CR_KorathCol=CK5_KorathColFunc',
             'CR_KorathWalkCol=06B9:0E68', 'CR_QEDCol=0AAF:1FB7', 'CR_ClipSideCol=0F92:0066']

# (the patches' places in the program, what they do, the setting)
RULES = [
    ((0x21C80, 0x35EC4, 0x8C61, 0x8C0D, 0x8BF1, 0x8BFF, 0x8CF5, 0x8C20), 'its own sound and music: 20 songs from chunk 186',
     '%string ca_audioHead "AUDIOHED.CK5"\n%string ca_audioDict "AUDIODCT.CK5"\n'
     '%int ca_audStartMusic 186\n%int ca_audNumSongs 20'),
    ((0xEC67, 0x5830, 0xEABD, 0xF808), 'music of the ending, the story, the help and the game over',
     '%int CK_EndingMusicLevel 15\n%int CK_StarWarsMusicLevel 9\n%int CK_HelpMusicLevel 2\n%int CK_GameOverMusicLevel 15'),
    ((0x7CE8,), "no \"Keen steps out onto Korath III\" on the map", '%int CK5_MapMarsMinY 200'),
    ((0xF431,), 'no elevators on the map', '%int ck5_mapElevators 0'),
    ((0x60C1, 0x60D3, 0x60EA), "finished levels leave the map's other markers alone", '%int CK_MapPreserveCoordinates 1'),
    ((0xB97D,), 'no higher jump on Easy', '%int ck_easyJumpGravity 0'),
    ((0xE85D, 0xE861, 0xE865, 0xE960, 0xE563, 0xE567, 0xE56B), 'black help pages (the text keeps its colours for red)',
     '%int CK_HelpBarColour 0'),
    ((0xBF8E, 0x8F71, 0xBFFE), 'dying in level 12 wins the game (its last level)', '%int CK_DiedWinsLevel 12'),
    ((0xEC31, 0xEC33), 'the secret ending: level 5 done (Myah\'s body armour)',
     '%int CK5_SecretFuseLevel 5\n%int CK5_SecretEndingAnyExit 1'),
    ((0xF10C, 0x334C6, 0xCAA7), 'every fuse marker is an unseen QED: touching it, a friend speaks (CR_QEDCol)',
     '%int CK5_QEDEveryLevel 1'),
    ((0xCBBD,), "the pogo stick doesn't break fuses", '%int CK5_PogoBreaksFuse 0'),
    ((0x6D1B, 0x6DEB, 0x33AA6, 0x33B01, 0x33B38, 0x31BFB), "the friends' messages: Keen 9's window (KW9_FriendMessage)", None),
    ((0x11762, 0x11806), 'slicestars take two shots', '%int CK5_SliceHealth 2\n%int CK5_SliceDiagHealth 2'),
    ((0x119A6,), "Shelleys don't jump off ledges", '%int CK5_ShelleyNoDive 1'),
    ((0x12643, 0x12680, 0x126CE, 0x126BD, 0x126EE), "Robo Red: 30 shots, from lower down, straight ahead",
     '%int CK5_RoboNumShots 30\n%int CK5_RoboShotYOffset 0x295\n%int CK5_RoboShotRightXOffset 0x200\n'
     '%int CK5_RoboShotYVelocity 0'),
    ((0x1102F, 0x11053, 0x11186, 0x1118D, 0x111C1), 'Sparkies take three shots (CR_SparkyCol)', '%int CK5_SparkyHealth 3'),
    ((0x11276, 0x1129C, 0x11643, 0x1164A), "Amptons: harmless, pushing, Keen's shots bounce off; an enemy shot stuns one and "
     'it drops a keycard (CR_AmptonCol)', '%int CK5_AmptonExtraLump @LUMP_KEYCARD'),
    ((0x137AC,), 'Spherefuls have no diamonds', '%int CK5_SpherefulDiamonds 0'),
    ((0x1373D, 0x13783), 'Spherefuls bounce silently on ceilings and floors',
     '%int CK5_SpherefulCeilingSound 0\n%int CK5_SpherefulFloorSound -1'),
    ((0x1257A,), "the mine's eye", '%int CK5_MineEyeChunk -1'),
    ((0x11D1B,), "mines don't move", '%int CK5_MineStill 1'),
    ((0x1244F,), "a mine's second piece is a Shockshund", '%action CK5_ACT_MineShrapB = CK5_ACT_ShocksundJump0'),
    ((0xEF7D,), "a mine needs the Shockshund's sprites", '%int CK5_MineExtraLump @LUMP_SHOCKSHUND'),
    ((0x133C8, 0x1328A, 0x133AA, 0x1330E, 0x1339B), 'Mortimer (the Shockshund): 30 hits, then stunned by every shot; '
     "no flashing, barks less, harmless to touch",
     '%int CK5_ShocksundHealth 30\n%int CK5_ShocksundStunsAgain 1\n%int CK5_ShocksundNoFlash 1\n'
     '%int CK5_ShocksundShootChance 0x60\n%int CK5_ShocksundHarmless 1'),
    ((0x133B7,), 'a stunned Shockshund jumps', '%action CK5_ACT_ShocksundStunned = CK5_ACT_ShocksundJump0'),
    ((0x138F1, 0x79F6, 0x79F8, 0x13956, 0x138D5), 'Korath: seven shots, starts walking right, a Shikadi to the program, '
     'deadly (CR_KorathWalkCol, CR_KorathCol), shoots like a turret when it stops',
     '%int CK5_KorathHealth 7\n%int CK5_KorathStartsRight 1\n%int CK5_KorathType 20\n%int CK5_KorathExtraLump @LUMP_LASER'),
    ((0x13011,), 'Shikadi zap without a pole', '%int CK5_ShikadiZapsAnywhere 1'),
    ((0x1307E, 0x12EFA), "Shikadi don't flash when shot (four shots, as before)", '%int CK5_ShikadiNoFlash 1'),
    ((0x12B1A,), "the Shikadi throws the Master's ball, from lower down", '%int CK5_MasterShotYOffset 0x120'),
    ((0x130F5, 0x130FC, 0x13105, 0x13110), "the Master's sparks fly sideways at Keen", '%int CK5_PoleZapHorizontal 1'),
    ((0xF996,), "the sparks push Keen without pushing him off ledges (CR_ClipSideCol)", None),
    ((0xF32E,), "the Master needs the Shikadi's sprites", '%int CK5_MasterExtraLump @LUMP_SHIKADI'),
    ((0x31A30,), "the Shikadi's sprites begin at the Master's ball", None),   # (lumpStarts, written below)
    ((0xF65F, 0xF6D8, 0xF6E2, 0xF664, 0xF6B1, 0xF714, 0xF725), "game over: the galaxy fades to and from white and doesn't explode",
     '%int CK5_GameOverWhite 1'),
    ((0x9518,), "K1n9_Duk3's fix: the floor check ends at the map's top", '%int CK_ClipToEndsSigned 1'),
    ((0x9DAB,), "K1n9_Duk3's fix: pushing sideways leaves no height over", '%int CK_PushXResetsY 1'),
    # Its actions (kw_patconv.py writes them): Robo Red, its shots, the
    # mine's explosion (three blasts, no warning), the Shockshund's shot and
    # stun, the Sphereful's float, Korath, the QED, the Master (no teleport;
    # it casts the sparks, the Shikadi on a pole throws its ball).
    ((0x32A80, 0x32A9E, 0x32ADA, 0x32AF8, 0x32B16, 0x3337A, 0x33384, 0x33398, 0x333A2, 0x333B6, 0x333C0, 0x333D4,
      0x333DE, 0x329B2, 0x329D0, 0x329EE, 0x32A0C, 0x3323A, 0x3335C, 0x33426, 0x33444, 0x33462, 0x33480, 0x3349E,
      0x3349A, 0x33150, 0x32F16, 0x32EBC, 0x331A4, 0x331C2, 0x331AA, 0x331C8, 0x331A6, 0x331C4, 0x331AE, 0x331CC),
     None, None),
    # Not settings: the menu's colours and title ("LOADING EMULATOR..."),
    # the loading screen, the sprite test, the screen update, memory and
    # direct audio loading (DOS), the font fix, a shorter but equal piece of
    # Keen's code (KeenPullThink2), the no-free-sprite-slot message.
    ((0x1B9A4, 0x1BE0A, 0x1C2D9, 0x1C4E1, 0x1C60E, 0x1CB29, 0x1CB3A, 0x1CC11, 0x1CF4D, 0x1D14F, 0x1B9AB, 0x1B9DE,
      0x1BE66, 0x1C294, 0x1C4E5, 0x1CC15, 0x1BA5F, 0x1BAB2, 0x1BC13, 0x1BC2E, 0x1BC4C, 0x1BC67, 0x1BE41, 0x1D694,
      0x1D6AC, 0x1B964, 0x1BBF9, 0x1C31F, 0x1C4AC, 0x1C533, 0x1C5ED, 0x1C925, 0x1CE45, 0x1CF4F, 0x1D160, 0x1D1A2,
      0x1D2EE, 0x1D598, 0x1D5D3, 0x1D60B, 0x1C2FC, 0x1C510, 0x1CC1C, 0x1C7CC, 0x1DBD4, 0x346B8, 0x6433, 0x643A,
      0x63C8, 0x63CF, 0x767E, 0x1F795, 0x1F816, 0x18BB7, 0x14403, 0x14310, 0x6179, 0x5C1F, 0x1675F, 0x19731,
      0xEB21, 0xEC3B, 0xEC4D, 0xEC58, 0xED11, 0xED1F, 0x14EE8, 0x35EB8, 0x1E31F), None, None),
]
SHIKADI_LUMP_START = 312   # ($31A30: the first of the Master's ball sprites)
FRIENDS = (('kw9_str_lindsey', 0x33AA6), ('kw9_str_barker', 0x33B01), ('kw9_str_spot', 0x33B38), ('kw9_str_janitor', 0x31BFB))


def actions(game, exe, keen5):
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', exe, os.path.join(game, PAT), keen5] + STAND_INS
        kw_patconv.main()
    return [l.split('  #')[0] for l in buf.getvalue().splitlines() if l.startswith('%action')]


def friends(game, exe):
    """The friends' messages, as the patches write them ($6D1B shows them in
    Keen 5's fuse window, by level: 5, 11, 2, any other)."""
    patched = bytearray(kw_patconv.load_image(exe))
    for off, data in kw_patconv.parse_pat(os.path.join(game, PAT)):
        patched[off:off + len(data)] = data
    return ['%%string %s "%s"' % (name, escape(cstring(patched, at))) for name, at in FRIENDS]


def lump_starts(keen5):
    """Keen 5's sprite groups, the Shikadi's beginning earlier."""
    text = open(os.path.join(keen5, 'GFXCHUNK.CK5'), encoding='latin1').read()
    block = re.search(r'%intarray lumpStarts\n((?:\s+@?\w+,?\n)+)', text).group(1)
    assert block.count('@SPR_SHIKADI1') == 1
    return '%intarray lumpStarts\n' + block.replace('@SPR_SHIKADI1', str(SHIKADI_LUMP_START))


def late(setting):
    """Whether a setting names Keen 5's own actions or numbers, so that it
    goes after the %override that reads them."""
    return setting.startswith('%action') or '@' in setting


def main(game, exe, keen5, out):
    extra, handled = [], []
    for places, what, setting in RULES:
        handled += places
        if setting and not late(setting):
            extra += ['# ' + what] + setting.split('\n')
    extra += ["# the friends' messages (KW9_FriendMessage)"] + friends(game, exe)
    convert(game, keen5, out, pat=PAT, files=FILES, extra=extra, handled=handled, episode=5, exe=exe,
            drop_strings=('ck5_str_fuseDestroyed', 'ck5_str_fuseDestroyedKorath'),
            skip=('keen5.tli', 'ck5load.bin', 'audiohed.ck5', 'audiodct.ck5'),
            title="Canmore's Redemption (Aurora Bliss II: Dave216, 2023, v1.0.2)", tool='kwtools/kw_canmore.py')
    acts = actions(game, exe, keen5)
    with open(os.path.join(out, 'MOD.CK5'), 'a', encoding='latin1') as f:
        f.write('\n# Its sprite actions, as the patches leave them (our routines named as in\n'
                '# kwtools/kw_canmore.py STAND_INS)\n' + '\n'.join(acts) + '\n')
        f.write('\n# Settings that name Keen 5\'s own (so after its file)\n')
        for places, what, setting in RULES:
            if setting and late(setting):
                f.write('# %s\n%s\n' % (what, setting))
        f.write("# the Shikadi's sprites begin at the Master's ball ($31A30)\n" + lump_starts(keen5))
    print('%d actions' % len(acts))


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
