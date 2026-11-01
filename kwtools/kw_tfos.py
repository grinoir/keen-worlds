"""Write Keen Worlds' variable file for The Terror from Outer Space.

    python3 kwtools/kw_tfos.py GAMEDIR KEEN4E.EXE omnispeak/data/keen4 play/mods/tfos

GAMEDIR: the player's copy of The Terror from Outer Space (Dr. Colossus,
2013, v1.0; the files of its game folder). It is a Keen 4 v1.4 mod: its own
levels, graphics, music and sounds, and a CKPatch file (mymod.PAT) that
changes texts, many of the program's rules, and retimes/recolours its
sprites. KEEN4E.EXE is the Keen 4 v1.4 program the mod patches (it is only
read; the mod ships the build its patch targets, so that copy is used).
Texts, level names and the music table are turned into settings as for The
Eight Accumulators (kw_accumulators.py, episode 4); each change to the
program's rules has a setting in the engine's Keen 4 code, listed in RULES
with the patch it stands for. Audit: research/ (this is a work in progress).
"""
import contextlib, io, os, re, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert, escape
import kw_patconv

PAT = 'mymod.PAT'
FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'AUDIODCT.CK4': 'AUDIODCT.CK4', 'AUDIOHED.CK4': 'AUDIOHED.CK4',
         'EGAGRAPH.CK4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4', 'GAMEMAPS.CK4': 'GAMEMAPS.CK4',
         'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}

# Our routines for the creatures the mod repurposes (name=the program's
# function it stands in for), as kw_patconv.py takes them. The C is in
# src/kw_tfos.c; the %action lines name them so the engine calls ours.
STAND_INS = ['TFOS_MineCol=CK4_MineCol', 'TFOS_InchwormCol=CK4_InchwormCol',
             'TFOS_EggCol=CK4_EggCol', 'TFOS_DartGun=CK4_DartGun']

# (the patches' places in the program, what they do, the setting). A work in
# progress: the creatures' rules are mapped here one by one. These first
# entries are field tweaks whose old value is the engine setting's own
# default (so the new value is the setting), found by matching defaults.
RULES = [
    # Its own sound: the mod ships AUDIO.CK4 with its own header and dictionary.
    # Omnispeak's compressed-audio path looks for the header as AUDIOHHD by
    # default (its reconstruction's name); point it at the mod's AUDIOHED.CK4 so
    # the chunk offsets match the mod's AUDIO (otherwise it reads stock Keen 4's
    # offsets and allocates a garbage-sized block).
    ((), 'read the mod\'s own audio header and dictionary',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"'),
    ((0xFF9C,), 'Mushrooms jump less high', '%int CK4_MushroomJumpYVel -80'),
    ((0xFFBB,), 'Mushrooms hop less high', '%int CK4_MushroomHopYVel -21'),
    ((0x10931,), "Wormmouths reach further right to bite", '%int CK4_WormmouthBiteRightMaxX 0x580'),
    ((0x10942,), "Wormmouths reach further left to bite", '%int CK4_WormmouthBiteLeftMinX -1280'),
    ((0x10A36, 0x10A80), 'the storm cloud drifts faster', '%int CK4_CloudXAccel 0xf'),
    ((0x10CC8,), "the Berkeloid's fireball starts higher", '%int CK4_BerkeloidFireballYOffset -1'),
    ((0x10CD0,), "the Berkeloid's fireball rises faster", '%int CK4_BerkeloidFireballYVelocity -28'),
    ((0x10CE1,), "the Berkeloid's fireball starts further right", '%int CK4_BerkeloidFireballXOffsetRight 0x100'),
    ((0x10FD7,), "the inchworm's foot sits lower", '%int CK4_InchwormFootYOffset 0xf00'),
    ((0x111BC,), 'Bounders bounce less high', '%int CK4_BounderBounceYVelocity -70'),
    ((0x12829,), "Princess Lindsey doesn't drift up and down", '%int CK4_LindseyFloatYAccel 0'),
    ((0x1283F, 0x1284F), "Princess Lindsey floats in a tight band", '%int CK4_LindseyFloatYRadius 1'),
    ((0x1253C,), 'schoolfish swim slower sideways', '%int CK4_SchoolfishXVelocity 4'),
    ((0x12558, 0x1256C), 'schoolfish swim slower up and down', '%int CK4_SchoolfishYVelocity 4'),
    ((0x10B53,), "the storm cloud's bolt drops straight down", '%int CK4_CloudBoltXOffset 0'),
    ((0x10B5C,), "the storm cloud's bolt starts higher", '%int CK4_CloudBoltYOffset -1280'),
    ((0x113DA,), 'Licks leap higher on a long hop', '%int CK4_LickLongHopYVelocity -48'),
    # Wholesale creature rewrites, each the engine's own behaviour behind a flag
    # (the same flags Aurora Bliss and Revival of Bliss turned on). Verified
    # against src/ck4_obj2.c / ck4_obj3.c: the mod's new routine is what the
    # engine does with the flag set.
    ((0x11138,), "a shot only makes a Bounder hop, it can't be stunned (CK4_BounderCheckShot rewrite)",
     '%int CK4_BounderUnstunnable 1'),
    ((0x11C99, 0x11C4A), "Smirkies never teleport (CK4_SmirkyCol / CK4_SmirkyTeleport rewrites)",
     '%int CK4_SmirkyNoTeleport 1'),
    ((0x11EEB, 0x11F05), "Mimrocks follow Keen whichever way he faces (CK4_MimrockWait facing checks removed)",
     '%int CK4_MimrockIgnoresFacing 1'),
    ((0x11F3B,), "Mimrocks never sit back down once chasing (CK4_MimrockCheckJump rewrite)",
     '%int CK4_MimrockAlwaysChases 1'),
    # The bird family, the same shared infrastructure Operation Ocflore uses
    # (verified: same offsets / same feature). Map markers 36-38 spawn flying
    # birds by difficulty (OC_SpawnFlyingBird, hooked in ck4_misc.c); flying
    # birds never land; the ground bird (what the egg hatches) stands on the
    # floor.
    ((0x10321, 0x10366, 0xEED1, 0xEED3, 0xEED5), "map markers 36-38 spawn flying birds by difficulty",
     '%int OC_FlyingBirdMarkers 1'),
    ((0x103D0, 0x1048B), "flying birds never land", '%int CK4_BirdNeverLands 1'),
    ((0x1006A,), "the walking bird stands on the ground", '%int CK4_BirdSpawnYOffset 0xE1'),
    # Non-creature settings, shared with Operation Ocflore (same offsets).
    ((0x6AE4,), "six of the Council win the game", '%int CK4_NumCouncilsToRescue 6'),
    ((0x60FD,), "levels 21 and up are its ship levels", '%int ck_maxDoneLevel 20'),
    # Its map has level doors up to level 21 (0xC015); Keen 4's map only lets you
    # enter levels 1-18, so the doors by the ship (level 20, the Bridge) wouldn't open.
    ((), "its map has level doors up to level 21", '%int ck_maxEnterLevel 21'),
    ((0xF622,), "the swimming Keen clips normally", '%int CK4_SwimKeenClipNormal 1'),
    # Rewrites that are the engine's own behaviour recompiled, with only a
    # cosmetic delta: Omnispeak's own function is equivalent, so they are
    # acknowledged, not reimplemented. (Decoded against src/ck4_obj1.c.)
    ((0xFE6A,), "CK4_SlugCol: identical but for dropping the 50/50 random stun frame "
                "(Omnispeak picks one at random, same creature)", None),
    ((0x12A30,), "CK4_WetsuitCol: identical pickup but for dropping the redundant "
                 "already-have-it guard (the wetsuit is met once)", None),
    # Creatures the mod repurposes into its own aliens; our routines in
    # src/kw_tfos.c (the %action lines name them via STAND_INS). The stun
    # frames reuse Keen 4 action slots with the mod's graphics.
    ((0x127A6,), "the 'mine' is a stun-only floating alien (TFOS_MineCol)", None),
    ((0x10F93,), "the 'inchworm' kills Keen and a shot stuns it (TFOS_InchwormCol)", None),
    ((0x100CF, 0x10135), "eggs hatch a walking alien; the shell hops off as a Lick, "
                         "no shell bits (TFOS_EggCol)", None),
    # The wall dart gun fires Bounder-classed darts; our TFOS_DartGun folds in the
    # mod's one-frame prefix action (whose think, the repurposed CK4_Miragia2 at
    # $FB33, only sets the dart's type to Bounder) and so accounts for the dart-
    # action repoints and the two appended prefix actions. (The engine's
    # CK4_Miragia2 still serves the Miragia map mirage, which TFOS does not use.)
    ((0x12950, 0x1298A, 0x129A7, 0x129B4, 0xFB33, 0x3D740, 0x3D77C),
     "the dart gun fires Bounder-classed darts (TFOS_DartGun)", None),
    # The repurposed inchworm (TFOS_InchwormCol) moves straight ahead: the mod
    # nops out CK4_InchwormMove's two direction-flip writes. (It is no longer
    # Keen 4's inchworm-into-Foot creature, so the flips are moot.)
    ((0x10F83, 0x10F8B), "the repurposed inchworm doesn't flip direction", None),
    # The per-level music table (read as CK_LevelMusic above). The mod writes a
    # few entries past its levels and over the \"Active Object\" debug string that
    # follows; both are unused by Omnispeak.
    ((0x2F206,), "the music table's tail and the debug string after it", None),
    # The planet name placed in spare space; it is already carried by the level
    # names, entry texts and ck_str_exitToMap that convert() reads.
    ((0x1F040,), "a spare copy of the planet name \"Phaelox\"", None),
    # The sprite lumps: the mod widens three cache lumps so a creature's moved
    # frames (its stunned frames among them) load with it. Written as the whole
    # lumpStarts/lumpEnds arrays below (lumps()); these four offsets are those
    # same edits in the program (lumpStarts[22]/[32]/[34], lumpEnds[32]). The
    # two sprite-shape pointer tables ($30764/$307C8) are the DOS program's own
    # (Omnispeak builds its sprites from the graphics file), so nothing to do.
    ((0x306F0, 0x30700, 0x30714, 0x3075E, 0x30764, 0x307C8), None, None),
]

# The mod's edits to Keen 4's sprite-cache lumps, by index into lumpStarts /
# lumpEnds (found from the four %patch words at dseg 0x1880-0x18EE): lump 32
# grows from 440-442 to 436-451 to take a creature's moved frames, and lumps
# 22 and 34 start one or two sprites earlier.
LUMP_STARTS = {22: 337, 32: 436, 34: 455}
LUMP_ENDS = {32: 451}


def _lump_arrays(keen4):
    """Keen 4's lumpStarts/lumpEnds (data/keen4/GFXCHUNK.CK4) with the mod's
    edits applied by index. Returns (defs, lumpStarts, lumpEnds)."""
    text = open(os.path.join(keen4, 'GFXCHUNK.CK4'), encoding='latin1').read()
    defs = {m.group(1): int(m.group(2)) for m in re.finditer(r'%int\s+(SPR_\w+|PIC_\w+|FON_\w+)\s+(\d+)', text)}
    arrs = {}
    for name, edits in (('lumpStarts', LUMP_STARTS), ('lumpEnds', LUMP_ENDS)):
        block = re.search('%intarray ' + name + r'\n((?:\s+@?\w+,?\n)+)', text).group(1)
        nums = [defs[t] if t in defs else int(t, 0)
                for t in (ln.strip().rstrip(',').lstrip('@') for ln in block.strip().splitlines())]
        for i, v in edits.items():
            nums[i] = v
        arrs[name] = nums
    return defs, arrs['lumpStarts'], arrs['lumpEnds']


def lumps(keen4):
    """The two %intarray overrides (lumpStarts/lumpEnds, mod edits applied)."""
    _, ls, le = _lump_arrays(keen4)
    return '\n'.join('%%intarray %s\n%s' % (name, ',\n'.join('\t%d' % n for n in nums))
                     for name, nums in (('lumpStarts', ls), ('lumpEnds', le)))


def extra_lumps(keen4, acts):
    """The sprite-cache lumps every level must mark: the mod shuffles creatures'
    frames across lumps (its Berkeloid stuns into Mimrock frames), so a creature
    can't just mark its own one lump. Every lump that any sprite named in the
    mod's actions falls in is marked, so a borrowed frame is always loaded."""
    defs, ls, le = _lump_arrays(keen4)

    def lumpof(c):
        for i, (s, e) in enumerate(zip(ls, le)):
            if s <= c <= e:
                return i
        return None
    want = set()
    for line in acts:
        tok = line.split()
        for t in tok[3:5]:   # chunkLeft, chunkRight
            t = t.lstrip('@')
            v = defs.get(t, int(t) if t.lstrip('-').isdigit() else None)
            if v is not None and (l := lumpof(v)) is not None:
                want.add(l)
    nums = sorted(want)
    return ('%%int CK4_NumExtraSpriteLumps %d\n%%intarray CK4_ExtraSpriteLumps\n%s'
            % (len(nums), ',\n'.join('\t%d' % n for n in nums)))


def action_offsets(game, exe, keen4):
    """File offsets of the patches that edit a sprite's action struct or build a
    new one: these are captured by the %action lines appended below, so they are
    not left over. (Keen 4's actions retimed/recoloured, plus the mod's new ones.)"""
    g = kw_patconv.Game(exe, keen4)
    pats = list(kw_patconv.parse_pat(os.path.join(game, PAT)))
    new = g.new_actions(pats)   # {dseg offset: name} of whole new action structs
    struct_offs = sorted(g.byoff) + sorted(new)   # existing + new action structs (dseg offsets)
    hit = []
    for off, b in pats:
        d = off - g.dseg
        if d < 0:
            continue
        if any(o < d + len(b) and d < o + kw_patconv.ACTION_SIZE for o in struct_offs):
            hit.append(off)
    return hit


def level_count(game):
    """How many levels the mod has (highest map index + 1), from its MAPHEAD:
    a 2-byte RLEW tag then 100 int32 level offsets, nonzero where a level is.
    The Terror has ship/finale levels past Keen 4's 18, so its level-name and
    entry-text arrays must reach them (otherwise loading one reads off the end)."""
    import struct
    mh = [f for f in os.listdir(game) if f.lower() == 'maphead.ck4'][0]
    head = open(os.path.join(game, mh), 'rb').read()
    offs = struct.unpack_from('<100i', head, 2)
    return max(i for i, o in enumerate(offs) if o > 0) + 1


def main(game, exe, keen4, out):
    nlevels = level_count(game)
    extra, handled = [], list(action_offsets(game, exe, keen4))
    for places, what, setting in RULES:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    # Its ship/finale levels are past Keen 4's 20 songs, so raise the limit (the
    # music table and CK_LevelMusic are read out to its level count above).
    extra += ['# play music on its extra levels too', '%%int CK_NumMusicLevels %d' % nlevels]
    convert(game, keen4, out, pat=PAT, files=FILES, extra=extra, handled=handled, episode=4, exe=exe,
            skip=('keen4.tli', 'ck4load.bin', 'audiohed.ck4', 'audiodct.ck4'), num_levels=nlevels,
            title="The Terror from Outer Space (Dr. Colossus, 2013, v1.0)", tool='kwtools/kw_tfos.py')
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', exe, os.path.join(game, PAT), keen4] + STAND_INS
        kw_patconv.main()
    acts = [l.split('  #')[0] for l in buf.getvalue().splitlines() if l.startswith('%action')]
    with open(os.path.join(out, 'MOD.CK4'), 'a', encoding='latin1') as f:
        f.write('\n# Its sprite actions, as the patches leave them (Keen 4\'s own, frames changed)\n'
                + '\n'.join(acts) + '\n')
        f.write('\n# The sprite-cache lumps, widened so moved frames load with their creature\n'
                + lumps(keen4) + '\n')
        f.write('\n# The lumps every level marks (its creatures borrow each other\'s frames)\n'
                + extra_lumps(keen4, acts) + '\n')
    print('%d actions' % len(acts))


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
