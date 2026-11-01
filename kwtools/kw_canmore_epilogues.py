"""Write Keen Worlds' variable files for the two epilogues of Canmore's
Redemption: Saint of Aurora and Initiation Party.

    python3 kwtools/kw_canmore_epilogues.py CANMOREDIR AURORADIR omnispeak/data/keen4 play/mods

CANMOREDIR: the player's copy of Canmore's Redemption v1.0.2 (Dave216; the
zip unpacked). Its endings give the passwords of two archives in it, "Saint
of Aurora.rar" (the sad ending: SuchIsLife) and "Initiation Party.rar" (the
body-armour ending: CaptainCanmoreForever); this unpacks them (7z) into
play/mods/saint and play/mods/party. Each is a small Keen 4 mod: its own
levels, graphics, music and a CKPatch file (abiathar.pat). AURORADIR: the
player's copy of Aurora Bliss, whose graphics dictionary (EGADICT.CK4) their
graphics are packed with (the archives don't have one). Texts, level names
and the music table are turned into settings as for The Eight Accumulators
(kw_accumulators.py); each change to the program's rules is a setting in the
engine's Keen 4 code, listed in RULES with the patch it stands for. Audit:
research/ocflore-and-aurora-bliss.md in the workspace root.
"""
import contextlib, io, os, shutil, subprocess, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert
import kw_patconv

FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'AUDIODCT.CK4': 'AUDIODCT.CK4', 'AUDIOHED.CK4': 'AUDIOHED.CK4',
         'EGAGRAPH.CK4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4', 'GAMEMAPS.CK4': 'GAMEMAPS.CK4',
         'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}
PAT = 'abiathar.pat'

# Both: (the patches' places, what they do, the setting)
COMMON = [
    ((0x8CEF, 0x8C91, 0x8C75, 0x8C83, 0x8D83, 0x8CA4, 0x20DF0, 0x354F6), 'its own sound and music',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"'),
    ((0x9CD7,), 'the ending plays song 7', '%int CK_EndingMusicLevel 7'),
    ((0x6AE6,), 'one council member (the one Keen comes to see) ends it', '%int CK4_NumCouncilsToRescue 1'),
    ((0xF52E, 0xF544, 0xF534), "no thanks window: only Keen's line", '%int CK4_CouncilNoThanks 1'),
    ((0x6ABF, 0xCDAB), 'no sounds when he is found', '%int CK4_CouncilQuiet 1'),
    ((0xFD54,), 'the one Keen comes to see stands still', '%int CK4_CouncilNeverPauses 1'),
    ((0x30E34, 0x30E52), None, None),   # (the same, in its actions: written by kw_patconv.py)
    # Not settings: the loading screen, the help (Keen Worlds has its own),
    # the status window's labels and the high scores' rescued count (it
    # shows none of them), the main menu's "QUIT".
    ((0x3F09, 0x51AB, 0x7930, 0x1CED9, 0x2F49C, 0x2F4CB, 0x2F4D1, 0x5A36, 0x3376A, 0x1F040), None, None),
]
GAMES = {
    'saint': dict(archive='Saint of Aurora.rar', password='SuchIsLife', folder='Saint of Aurora',
                  title='Saint of Aurora (an epilogue of Canmore\'s Redemption: Dave216, 2022)', rules=[
        ((), 'six songs from chunk 156', '%int ca_audStartMusic 156\n%int ca_audNumSongs 6'),
        ((0x5836,), 'the story scrolls to song 7', '%int CK_StarWarsMusicLevel 7'),
        ((0x904F,), 'map 0 is its level (it scrolls and ends like one)', '%int ck_worldMapNumber 99'),
        ((0xBBEB, 0xBBF0, 0xBBF5, 0xBBB7), 'Keen just stands (no bored animations)', '%int CK_KeenNoBored 1'),
        ((0x668C,), 'a lost life starts the level again at once', '%int CK_DeathRestartsLevel 1'),
        ((0x11B90, 0x11C13), 'the Treasure Eater never teleports', '%int CK4_SmirkyNoTeleport 1'),
        # Not settings: the difficulty menu (Keen Worlds asks its own way),
        # the demo loop and end screens (DOS), the loading box's thumbs.
        ((0x1FD80, 0x3C51, 0x3C7D, 0x50ED, 0x51BC, 0x19B2E, 0x642A), None, None),
    ]),
    'party': dict(archive='Initiation Party.rar', password='CaptainCanmoreForever', folder='Initiation Party',
                  title='Initiation Party (an epilogue of Canmore\'s Redemption: Dave216, 2023)', rules=[
        ((), 'ten songs from chunk 152', '%int ca_audStartMusic 152\n%int ca_audNumSongs 10'),
        ((0xF4CD,), "the level's music plays on while Keen talks", '%int CK4_CouncilTalkKeepsMusic 1'),
    ]),
}


def unpack(canmore, game, out):
    with tempfile.TemporaryDirectory() as tmp:
        subprocess.run(['7z', 'x', '-y', '-p' + game['password'], os.path.join(canmore, game['archive']), '-o' + tmp],
                       check=True, stdout=subprocess.DEVNULL)
        src = os.path.join(tmp, game['folder'])
        os.makedirs(out, exist_ok=True)
        for name in os.listdir(src):
            shutil.copyfile(os.path.join(src, name), os.path.join(out, name))


def build(gid, canmore, aurora, keen4, mods):
    game = GAMES[gid]
    out = os.path.join(mods, gid)
    unpack(canmore, game, out)
    shutil.copyfile(os.path.join(aurora, 'EGADICT.CK4'), os.path.join(out, 'EGADICT.CK4'))
    rules = COMMON + game['rules']
    extra, handled = [], []
    for places, what, setting in rules:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    convert(out, keen4, out, pat=PAT, files={k: v for k, v in FILES.items() if k != v}, extra=extra, handled=handled,
            skip=('keen4.tli', 'ck4load.bin', 'audiohed.ck4', 'audiodct.ck4'),
            title=game['title'], tool='kwtools/kw_canmore_epilogues.py')
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', os.path.join(out, 'KEEN4E.EXE'), os.path.join(out, PAT), keen4]
        kw_patconv.main()
    actions = [l for l in buf.getvalue().splitlines() if l.startswith('%action')]
    with open(os.path.join(out, 'MOD.CK4'), 'a', encoding='latin1') as f:
        f.write('\n# The sprite actions the patches change (the one Keen comes to see stands)\n' + '\n'.join(actions) + '\n')
    # (only what the game reads stays: not the DOS program and patcher)
    for name in ('KEEN4E.EXE', 'CK4PATCH.EXE', 'CK4LOAD.BIN', 'play.bat', 'SAINT.bat', 'PARTY.bat', 'CONFIG.CK4', 'CONFIG.BIN',
                 'Keen4.tli', 'maphead.ck4', 'STORY.CK4'):
        if os.path.exists(os.path.join(out, name)) and name not in FILES.values():
            os.remove(os.path.join(out, name))
    print('%s: %d actions' % (gid, len(actions)))


def main(canmore, aurora, keen4, mods):
    for gid in GAMES:
        build(gid, canmore, aurora, keen4, mods)


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
