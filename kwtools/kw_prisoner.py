"""Write Keen Worlds' variable files for The Prisoner's Dilemma, both builds.

    python3 kwtools/kw_prisoner.py GAMEDIR KEEN6E.EXE omnispeak/data/keen6e14 play/mods

GAMEDIR: the player's copy of Commander Keen and Rick Uller in 'The
Prisoner's Dilemma' (Levellass/community, v1.0; the files of prisoner/game,
with extrastuff.zip unpacked so askeen.pat and asrick.pat are beside the
data). It is a Keen 6 v1.4 mod in two builds, chosen in the DOS game by
CK6start.EXE: play as Keen (its .ck6 files, askeen.pat) or as Rick Uller
(its .ru6 files, asrick.pat), over shared maps. Each becomes a Keen Worlds
world. KEEN6E.EXE is the unmodified Apogee v1.4 program the mod patches (it
is only read; the base game is commercial, so the user supplies it, as for
Keen 5). omnispeak/data/keen6e14 is Omnispeak's Keen 6 v1.4 data.

The mod's own data files (graphics, music, tiles, maps) are the player's
copy, dropped into play/mods/prisoner (Keen) and play/mods/prisonerr (Rick)
under Keen 6's standard names (see the world's "put its files" note);
EGADICT/GFXINFOE/AUDINFOE are Keen 6 v1.4's, copied here from Omnispeak's
data. Texts, level names and music are turned into settings as for The Eight
Accumulators (kw_accumulators.py, episode 6); each change to the program's
rules has a setting in the engine's Keen 6 code, listed in RULES; the one
routine the patches add (the rendezvous pickup: shows a window, sets the
flag, stays in the level) is CK6_RendezvousCol in src/kw_prisoner.c, named in
STAND_INS. Audit: research/ocflore-and-aurora-bliss.md in the workspace root.
"""
import contextlib, io, os, shutil, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert, escape
import kw_patconv

PAT = 'abiathar.pat'   # (overwritten per build below)
# Keen 6 v1.4 files from Omnispeak's data, the same for every Keen 6 mod.
STANDARD = ['EGADICT.CK6', 'GFXINFOE.CK6', 'AUDINFOE.CK6']
# Our routine for the program's (the rendezvous pickup's collide), as
# kw_patconv.py takes it (0x125A:1641 is the code the patch adds).
STAND_INS = ['CK6_RendezvousCol=125A:1641']

# Keen 6 strings the mod overwrites that convert() doesn't catch (its
# replacements are longer than the originals): offset -> Omnispeak's string
# name. They are read back from the patched program.
BANTER = {0x3301A: 'ck6_str_grabbiterHungry', 0x33071: 'ck6_str_grabbiterFed', 0x33139: 'ck6_str_cliffNoRope'}

# Both builds: (the patches' places, what they do, the setting)
COMMON = [
    ((0x11ED3,), 'Bobbas (here, caged aliens) do not shoot', '%int CK6_BobbaNoShoot 1'),
    ((0x1216A, 0x1218D), 'Babobbas (big caged aliens) do not nap or shoot',
     '%int CK6_BabobbaNapChance 0\n%int CK6_BabobbaHopsPerShot 0'),
    ((0x10738,), 'go-platforms have no accessory animation', '%int CK6_GoPlatNoAccessory 1'),
    ((0x8927, 0x8920, 0x8959), 'the hero has no pogo until it is found (the "Hello, old friend!" pickup)',
     '%int CK_PogoNeedsWetsuit 1'),
    ((0xF6EE,), 'the rocket (A-Danac Express) flies without the passcard', '%int CK6_RocketNoPasscard 1'),
    ((0x32A86, 0x13BE1), 'the rendezvous pickup shows its window and stays in the level (CK6_RendezvousCol)', None),
    # Captured by the %action lines (field changes within Keen 6 actions the
    # patches recolour and retime: the dropping platform, Bloogguard/Snai-Danac,
    # Blooglets, Gik, Flect, and the dead Snai-Danac that becomes a platform).
    ((0x32B26, 0x32B30, 0x32B44, 0x32B4E, 0x32B62, 0x32B6C,
      0x32DC6, 0x32DE4, 0x32E02, 0x32E20, 0x32E5C, 0x32E7A, 0x32E98, 0x32EB6,
      0x32CAC, 0x32CCA, 0x32CE8, 0x32D06, 0x32D24, 0x32D42, 0x32D60, 0x32D7E, 0x32DB0,
      0x333AC, 0x333CA, 0x333E8, 0x32F90, 0x32FAE, 0x32FCC, 0x32FEA,
      0x32FF4, 0x32F88, 0x32FA6, 0x32FC4, 0x32FE2), None, None),
    # Not settings: the trivia check and the demo loop (Keen Worlds has its
    # own menu); the high-score/status labels; the sprite-cache range markers
    # (Omnispeak caches each level's sprites itself); the recoloured creatures'
    # spawn/draw heights (Omnispeak uses its own, a few pixels' difference);
    # the Grabbiter's and cliff's rendezvous-level numbers (Omnispeak shows the
    # message anyway); the removed map NPC (Jethro); the HUD coin/Rick counter;
    # and the Blooglet gem's spawn height.
    ((0x1CC7B, 0x1CC88, 0x3C9C, 0x3C9E, 0x3CA0, 0x3CA4, 0x3CA6, 0x35613, 0x35604,
      0x32440, 0x32488, 0x3248E, 0x32484, 0x10B54, 0x11D2F,
      0xF54E, 0xF57B, 0xEF52, 0xEF6E, 0xEE36, 0xEE52, 0xEEC4, 0xEEE0, 0xFBB3, 0xFBD4,
      0x563F, 0xE33C, 0x7CAC, 0x7CC2, 0x7CCD, 0x10CF5,
      # Rick only: the save-file name (Keen Worlds saves its own way), his
      # loading screen, the "RICKS" counter, and small jump/gib tweaks.
      0x34CF4, 0x6248, 0x624F, 0x61DD, 0x61E4, 0xAA49, 0xB8AC, 0xB8B7,
      0x316BA, 0x316D8, 0x316F6, 0x31714, 0x31732, 0x31750, 0x3176E, 0x3178C, 0x317AA, 0x317C8,
      0x3173C, 0x31796, 0x317B4, 0x317D2), None, None),
]
BUILDS = {
    'prisoner': dict(pat='askeen.pat', hero='Keen', files='ck6',
                     title="The Prisoner's Dilemma, as Keen (Levellass and the community, 2021, v1.0)"),
    'prisonerr': dict(pat='asrick.pat', hero='Rick', files='ru6',
                      title="The Prisoner's Dilemma, as Rick Uller (Levellass and the community, 2021, v1.0)"),
}
# The mod's files for each build -> the standard Keen 6 names a Keen Worlds
# world uses. The player copies these (renaming) into the world's folder.
DATA_FILES = {
    'ck6': {'Egagraph.ck6': 'EGAGRAPH.CK6', 'EGAHEAD.CK6': 'EGAHEAD.CK6', 'Audio.ck6': 'AUDIO.CK6',
            'AUDIOHED.CK6': 'AUDIOHED.CK6', 'AUDIODCT.CK6': 'AUDIODCT.CK6', 'tileinfo.ck6': 'TILEINFO.CK6',
            'Gamemaps.ck6': 'GAMEMAPS.CK6', 'maphead.ck6': 'MAPHEAD.CK6'},
    'ru6': {'EGAGRAPH.ru6': 'EGAGRAPH.CK6', 'EGAHEAD.ru6': 'EGAHEAD.CK6', 'AUDIO.RU6': 'AUDIO.CK6',
            'AUDIOHED.RU6': 'AUDIOHED.CK6', 'AUDIODCT.CK6': 'AUDIODCT.CK6', 'tileinfo.ru6': 'TILEINFO.CK6',
            'Gamemaps.ck6': 'GAMEMAPS.CK6', 'maphead.ck6': 'MAPHEAD.CK6'},
}
# The mod ships AUDIOHED (not the base's AUDIOHHD); ca_tileInfo is TILEINFO.CK6.
EXTRA_CA = ['%string ca_audioHead "AUDIOHED.CK6"']

# The Chronicle scroll (ck_str_storyText), both builds: the game tells its own
# story as a comic in HELPTEXT.CK6; this is that story, cut to a scroll in the
# game's voice. (Meurglys III is Bounty Hunting Brouhaha's world; the
# Re-Synapsizer is the machine Keen chases on to Aleph Zilch in Send in the
# Clorts.) Keen 6's intro is shown this way in Keen Worlds, like every world's.
STORY = (
    "Above Meurglys III,\nbounty hunter Commander\nKeen and his former\nquarry, Rick Uller,\n"
    "shake hands: together\nthey will stop Mortimer\nMcMire once and for\nall. But the Union man\n"
    "who comes to arrest\nthem is Mort himself,\nin disguise. He gloats\nof his latest machine,\n"
    "the Re-Synapsizer,\nwhich will triple every\ncreature's wits and\nkill most of them, then\n"
    "drops the pair down a\ncriminal deposit slot\nonto the Twin Prison\nPlanets, A-Danac 2 and\n"
    "3. Stripped of their\nships and Keen's pogo,\nlocked in adjoining\ncells, they have one\n"
    "hope: break out, reach\nthe Chief Magistrate,\nand play Rick's secret\nrecording of Mort's\n"
    "confession. Now are you\nglad we teamed up?")


def build(gid, game, exe, keen6, mods, stage):
    b = BUILDS[gid]
    out = os.path.join(mods, gid)
    os.makedirs(out, exist_ok=True)
    for name in STANDARD:
        shutil.copyfile(os.path.join(keen6, name), os.path.join(out, name))
    extra = list(EXTRA_CA) + ['# the Chronicle scroll (the game\'s HELPTEXT story, cut to a scroll)',
                              '%%string ck_str_storyText "%s"' % escape(STORY)]
    handled = []
    for places, what, setting in COMMON:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    pat = os.path.join(game, b['pat'])
    # The grabbiter/cliff banter the mod rewrote (read from the patched program)
    patched = bytearray(kw_patconv.load_image(exe))
    for off, data in kw_patconv.parse_pat(pat):
        patched[off:off + len(data)] = data
    extra.append('# the rendezvous banter (Keen 6\'s Grabbiter and cliff messages, rewritten)')
    for off, name in BANTER.items():
        end = patched.index(b'\0', off)
        extra.append('%%string %s "%s"' % (name, escape(patched[off:end].decode('latin1'))))
        handled.append(off)
    convert(game, keen6, out, pat=b['pat'], files={}, extra=extra, handled=handled, episode=6, exe=exe,
            skip=('tileinfo.ck6', 'tileinfo.ru6', 'ck6load.bin', 'audiohed.ck6', 'audiohed.ru6', 'audiodct.ck6'),
            title=b['title'], tool='kwtools/kw_prisoner.py')
    acts_buf = io.StringIO()
    with contextlib.redirect_stdout(acts_buf):
        sys.argv = ['kw_patconv.py', 'actions', exe, pat, keen6] + STAND_INS
        kw_patconv.main()
    acts = [l.split('  #')[0] for l in acts_buf.getvalue().splitlines() if l.startswith('%action')]
    with open(os.path.join(out, 'MOD.CK6'), 'a', encoding='latin1') as f:
        f.write('\n# Its sprite actions, as the patches leave them (CK6_RendezvousCol is ours,\n'
                '# kwtools/kw_prisoner.py; the rest are Keen 6\'s own, graphics changed)\n' + '\n'.join(acts) + '\n')
    # For local play/testing: the mod's own data files, renamed, in play/mods.
    if stage:
        pout = os.path.join(stage, gid)
        os.makedirs(pout, exist_ok=True)
        for name in STANDARD:
            shutil.copyfile(os.path.join(out, name), os.path.join(pout, name))
        shutil.copyfile(os.path.join(out, 'MOD.CK6'), os.path.join(pout, 'MOD.CK6'))
        for src, dst in DATA_FILES[b['files']].items():
            shutil.copyfile(os.path.join(game, src), os.path.join(pout, dst))
    print('%s (%s): %d actions' % (gid, b['hero'], len(acts)))


def main(game, exe, keen6, mods):
    stage = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), '..', 'play', 'mods')
    stage = stage if os.path.isdir(stage) else None
    for gid in BUILDS:
        build(gid, game, exe, keen6, mods, stage)


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
