"""Write Keen Worlds' variable file for "the last reunion", the hidden
epilogue of Revival of Bliss (the FUTURE game in its launcher).

    python3 kwtools/kw_future.py REVIVALDIR AURORADIR omnispeak/data/keen4 play/mods

REVIVALDIR: the player's copy of Revival of Bliss (the unpacked zip). Its DOS
launcher (MKLAUNCH.EXE by K1n9_Duk3) hides a bonus game, FUTURE, in
LAUNCHER.DAT, unlocked by a password. The game asks for "the two resonant
words combined", which its two builds' orbs repeat (LAST and REUNION) and its
endings' riddle tells you to combine: the password is "LAST REUNION". This
extracts FUTURE from LAUNCHER.DAT with it (the launcher's own format: a list
of [name offset, size] file records, then a running stream cipher described
below) into play/mods/future. FUTURE is a one-level Keen 4 mod set 32 years
on, Keen retiring and handing command to Craig. AURORADIR: the player's copy
of Aurora Bliss, whose graphics dictionary its graphics are packed with.
Converted as an epilogue (kw_canmore_epilogues.py) with Craig's rules
(kw_revival.py): the hero throws his shots, has no pogo, black help pages.
Audit: research/ocflore-and-aurora-bliss.md in the workspace root.

The cipher (read from MKLAUNCH.EXE): the key is the password's bytes with the
first xored by the length; each ciphertext byte is xored with the
bit-reversed current key byte to give the plaintext, then the key position
advances (wrapping) and that key byte is xored with the inverted plaintext.
One running key spans every file in order.
"""
import contextlib, io, os, shutil, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert
import kw_patconv

PASSWORD = 'LAST REUNION'   # the two resonant words, combined (its endings' riddle)
PAT = 'ABIATHAR.PAT'
FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'AUDIODCT.CK4': 'AUDIODCT.CK4', 'AUDIOHED.CK4': 'AUDIOHED.CK4',
         'EGAGRAPH.CK4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4', 'GAMEMAPS.CK4': 'GAMEMAPS.CK4',
         'MAPHEAD.CK4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}
REV = [int('{:08b}'.format(i)[::-1], 2) for i in range(256)]

# (places, what, setting) -- the epilogue's rules, as for Saint of Aurora and
# the Craig build of Revival of Bliss (the settings are already in the engine)
RULES = [
    ((0x8CEF, 0x8C91, 0x8C75, 0x8C83, 0x8D83, 0x8CA4, 0x20DF0, 0x354F6), 'its own sound and music: 12 songs from chunk 150',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"\n%int ca_audStartMusic 150\n%int ca_audNumSongs 12'),
    ((0x2F206,), 'its one level plays song 0', '%intarray CK_LevelMusic 0'),
    ((0x1CC6F, 0x1CC6B), None, None),   # menu music (Keen Worlds has its own)
    ((0x9CD7,), 'the ending plays song 7', '%int CK_EndingMusicLevel 7'),
    ((0x904F,), 'map 0 is its level (it scrolls and ends like one)', '%int ck_worldMapNumber 99'),
    ((0x6AE6,), 'one council member (Craig, whom Keen meets) ends it', '%int CK4_NumCouncilsToRescue 1'),
    ((0xF52E, 0xF544, 0xF534), "no thanks window: only Keen's line", '%int CK4_CouncilNoThanks 1'),
    ((0x6ABF, 0xCDAB), 'no sounds when Craig is found', '%int CK4_CouncilQuiet 1'),
    ((0xFD54,), 'Craig stands still', '%int CK4_CouncilNeverPauses 1'),
    ((0x668C,), 'a lost life starts the level again at once', '%int CK_DeathRestartsLevel 1'),
    ((0x2FD5E,), None, None),   # the air-door fix (handled in Omnispeak)
    ((0xA21E,), "K1n9_Duk3's fix: the floor check ends at the map's top", '%int CK_ClipToEndsSigned 1'),
    # Craig: his gun throws its shots, he has no pogo, and the help is black.
    ((0x8B43, 0x8B3C, 0x8B75, 0x8318, 0x7A99), 'the hero (Craig) has no pogo stick', '%int CK_PogoNeedsWetsuit 1'),
    ((0xC9AA, 0xC9B1, 0xC9B8, 0xC9C6, 0xC9CD, 0xC9D4, 0xC9ED, 0xC9F4, 0xC9FB, 0xCA09, 0xCA10, 0xD664, 0xD66D, 0xD73B, 0xD744, 0xB833),
     'his gun throws its shots (up, right, down, left)',
     '%int CK_ShotArc 1\n%intarray CK_ShotArcVel 0, -0x4C, 0x30, -0x18, 0, 0x20, -0x40, -0x18'),
    ((0xDA4F,), 'his shots never run out', '%int CK_ShotsFree 1\n%int ck_scoreBoxHideShots 1'),
    ((0x95FB, 0x95FF, 0x9603, 0x92C6), 'black help pages', '%int CK_HelpBarColour 0'),
    # Not settings: the difficulty menu, the demo loop and end screens, the
    # title/menu text ("A..."/"Trivial"/"???"/high scores), the DOS memory,
    # screen update and loading screen, the status labels.
    ((0x1FD80, 0x3C51, 0x3C7D, 0x50ED, 0x51BC, 0x1C241, 0x61DD, 0x5C4B, 0x157C2, 0x18793, 0x9BA8, 0x9CBC,
      0x13F4B, 0x354EA, 0x1D383, 0x1E7F9, 0x1E87A, 0x3F09, 0x51AB, 0x7930, 0x1CED9, 0x2F49C, 0x2F4CB, 0x2F4D1,
      0x2F4D9, 0x2F4AF, 0x5A36, 0x30E34, 0x30E52, 0x3375F, 0x3376A, 0x6478, 0x647F, 0x640D, 0x6414,
      0x581B, 0x5814, 0x2F05E, 0x3D5E, 0x3CE5, 0x1D130, 0x3CF6,
      0x92A0, 0x9296, 0x929C, 0x92A7, 0x9487, 0x9495, 0x946B, 0x98B0, 0x9475,
      0x2FF7C, 0x2FF9A, 0x2FFB8), None, None),  # (the last three: the thrown shot's own sprite; the default stunner sprite is kept)
]


def extract(launcher_dat, out):
    """FUTURE's files from LAUNCHER.DAT, decrypted with the clue password."""
    d = open(launcher_dat, 'rb').read()
    # The file names (one per record), near the end, as FUTURE\NAME strings.
    import re
    # The file records' name table: the longest run of back-to-back FUTURE\\*
    # strings (stray FUTURE\\ references elsewhere are not part of it).
    runs, cur, at = [], [], None
    for m in re.finditer(rb'FUTURE\\[\w.]+\x00', d):
        if at is not None and m.start() != at:
            runs.append(cur); cur = []
        cur.append(m.group(0).decode('latin1').split('\\')[1].rstrip('\x00'))
        at = m.end()
    runs.append(cur)
    names = max(runs, key=len)
    n = len(PASSWORD)
    # The encrypted file stream follows the launcher's fixed blocks. Find its
    # start by decrypting a little at each offset until the first file (its
    # Abiathar patch) appears.
    def peek(pos, length):
        key = bytearray(PASSWORD.encode('latin1'))
        key[0] ^= n & 0xFF
        kp, out = 0, bytearray()
        for c in d[pos + 6:pos + 6 + length]:
            q = c ^ REV[key[kp]]
            out.append(q)
            kp += 1
            if kp == n:
                kp = 0
            key[kp] ^= (~q) & 0xFF
        return bytes(out)
    start = next(pos for pos in range(0x100, 0x800) if b'Abiathar' in peek(pos, 48))
    key = bytearray(PASSWORD.encode('latin1'))
    key[0] ^= n & 0xFF
    kp = 0
    pos = start
    os.makedirs(out, exist_ok=True)
    got = []
    for name in names:
        size = struct.unpack_from('<I', d, pos + 2)[0]
        data = d[pos + 6:pos + 6 + size]
        plain = bytearray(size)
        for i, c in enumerate(data):
            p = c ^ REV[key[kp]]
            plain[i] = p
            kp += 1
            if kp == n:
                kp = 0
            key[kp] ^= (~p) & 0xFF
        open(os.path.join(out, name), 'wb').write(plain)
        got.append(name)
        pos += 6 + size + 4   # a 4-byte checksum follows each file
    return got


def main(revival, aurora, keen4, mods):
    tmp = os.path.join(mods, 'future_src')
    extract(os.path.join(revival, 'LAUNCHER.DAT'), tmp)
    out = os.path.join(mods, 'future')
    os.makedirs(out, exist_ok=True)
    for src, dst in FILES.items():
        # (the launcher stores maphead.ck4 and Keen4.tli under those names)
        names = {f.lower(): f for f in os.listdir(tmp)}
        shutil.copyfile(os.path.join(tmp, names[src.lower()]), os.path.join(out, dst))
    shutil.copyfile(os.path.join(aurora, 'EGADICT.CK4'), os.path.join(out, 'EGADICT.CK4'))
    extra, handled = [], []
    for places, what, setting in RULES:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    convert(tmp, keen4, out, pat=PAT, files={}, extra=extra, handled=handled,
            skip=('keen4.tli', 'ck4load.bin', 'audiohed.ck4', 'audiodct.ck4'),
            title='The Last Reunion (hidden epilogue of Revival of Bliss: Dave216, 2024)', tool='kwtools/kw_future.py')
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', os.path.join(tmp, 'KEEN4E.EXE'), os.path.join(tmp, PAT), keen4]
        kw_patconv.main()
    acts = [l for l in buf.getvalue().splitlines() if l.startswith('%action')]
    # The mod wrote its one level's entry text with a stray "\\ " where it
    # meant a line break ("Thirty two years\\ later..."); mend it so it fits.
    modpath = os.path.join(out, 'MOD.CK4')
    text = open(modpath, encoding='latin1').read().replace('Thirty two years\\\\ later...', 'Thirty two years\\nlater...')
    open(modpath, 'w', encoding='latin1').write(text)
    with open(modpath, 'a', encoding='latin1') as f:
        f.write('\n# The sprite actions the patches change (Craig\'s thrown shots, Craig stands)\n' + '\n'.join(acts) + '\n')
    shutil.rmtree(tmp)
    print('%s/MOD.CK4: %d actions' % (out, len(acts)))


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
