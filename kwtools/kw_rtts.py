"""Return to the Shadowlands v1.8 (Ceilick, 2022-12-21) as a Keen Worlds world.

python3 kwtools/kw_rtts.py RTTS_GAME_DIR OUTPUT_DIR

RTTS is a source mod of K1n9_Duk3's reconstructed Keen 4 source, so in Keen
Worlds it runs on Omnispeak's Keen 4 code. This writes its variable file
(RTTS.CK4) and audio table (AUDINFOE.RTT) from the audit of its v1.8 source
(research/return-to-the-shadowlands.md). The player's own files are checked,
never changed or copied: the game's data stay in mods/rtts as released.

Graphics: RTTS keeps Keen 4's pictures where they were. Its sprites are
Keen 4's with the gem-door sprite (241) taken out, so those after it are one
lower, and 34 new ones after them; every tile and later chunk is 34 higher.
Its level names, texts and dialogue are the game's own, read from RTTS.EXE.
"""
from pathlib import Path
import hashlib
import re
import struct
import sys

HERE = Path(__file__).resolve().parent
KEEN4 = HERE.parent / 'data' / 'keen4'

FILES = {  # the v1.8 release (KeenWiki, l.ceilick.return_to_the_shadowlands_v1.8)
    'RTTS.EXE': '17df5ec106daf3edda9ca0383fe976a99a3f1d62791172ae2a49f34f7c2fc2b6',
    'EGAGRAPH.CK4': '9eb0091a64abcca6653b56f4cbb79fcc6a8d35eab6754318b19fd2be9ea09719',
    'GAMEMAPS.CK4': '28dc9550fec4755f97aca2456fd49da5fce9bb385d1fc9d6bc1b2b52bd101607',
    'AUDIOT.CK4': '12380f009e0f1a03774e4637565e0cf6a28407b60bae73f51b5c9249e8443d6f',
}
SPRITES_END = 520     # Keen 4's last sprite chunk
DOOR_SPRITE = 241     # Keen 4's gem door sprite: RTTS opens doors with tiles
SHIFT = 34            # tiles and externs: RTTS has 431 sprites to Keen 4's 397

# RTTS's sprites after Keen 4's (GFXE_CK4.H of its source, checked against
# GFXINFOE.CK4 of the release: sprites 124..554, tiles from 555).
NEW_SPRITES = {
    'SPR_BADOINGHIDDEN': 520, 'SPR_BADOINGRISE1': 521, 'SPR_BADOINGRISE2': 522, 'SPR_BADOINGRISE3': 523,
    'SPR_BADOINGRISE4': 524, 'SPR_BADOINGTURNL': 525, 'SPR_BADOINGLOOKL': 526, 'SPR_BADOINGTURNR': 527,
    'SPR_BADOINGLOOKR': 528, 'SPR_BADOINGSTUNNED': 529,
    'SPR_WITCHDOCWALKL1': 530, 'SPR_WITCHDOCWALKL2': 531, 'SPR_WITCHDOCWALKR1': 532, 'SPR_WITCHDOCWALKR2': 533,
    'SPR_JANITORWALK1R': 534, 'SPR_JANITORWALK2R': 535, 'SPR_JANITORWALK1L': 536, 'SPR_JANITORWALK2L': 537,
    'SPR_JANITORTHINKL': 538, 'SPR_JANITORTHINKR': 539,
    'SPR_STINGARUWALKL1': 540, 'SPR_STINGARUWALKL2': 541, 'SPR_STINGARUWALKL3': 542, 'SPR_STINGARUWALKL4': 543,
    'SPR_STINGARUJUMPL1': 544, 'SPR_STINGARUJUMPL2': 545, 'SPR_STINGARUJUMPL3': 546, 'SPR_STINGARUSTUNNED': 547,
    'SPR_STINGARUWALKR1': 548, 'SPR_STINGARUWALKR2': 549, 'SPR_STINGARUWALKR3': 550, 'SPR_STINGARUWALKR4': 551,
    'SPR_STINGARUJUMPR1': 552, 'SPR_STINGARUJUMPR2': 553, 'SPR_STINGARUJUMPR3': 554,
}
NEW_LUMPS = [('LUMP_BADOING', 'SPR_BADOINGHIDDEN', 'SPR_BADOINGSTUNNED'),
             ('LUMP_WITCHDOC', 'SPR_WITCHDOCWALKL1', 'SPR_WITCHDOCWALKR2'),
             ('LUMP_JANITOR', 'SPR_JANITORWALK1R', 'SPR_JANITORTHINKR'),
             ('LUMP_STINGARU', 'SPR_STINGARUWALKL1', 'SPR_STINGARUJUMPR3')]

SOUNDS, SONGS = 54, 7   # AUDIOCK4.H: two new sounds, seven songs
# CK_PLAY.C's songs[] (WORLD, GOODTIME, SLACK, K4S4C, SLOWBURN, DOPEY, WONDER
# are 6, 3, 1, 2, 0, 4, 5 in AUDIOCK4.H's order SLOWBURN, SLACK, K4S4C,
# GOODTIME, DOPEY, WONDER, WORLD).
# Its new creatures (K4_ACT4.C), as actions for src/kw_rtts.c. Offsets from
# 0x7000 are past Keen 4's, so saved games tell them apart.
ACTIONS = '''
%action RT_ACT_Badoing 0x7000 0 0 Frame 0 0 0 0 0 RT_BadoingThink RT_BadoingCol CK_BasicDrawFunc1 NULL
%action RT_ACT_BadoingStunned 0x701E @SPR_BADOINGSTUNNED @SPR_BADOINGSTUNNED Frame 0 0 0 0 0 CK_Fall NULL CK_BasicDrawFunc4 NULL
%action RT_ACT_WitchDoctor 0x703C 0 0 Frame 0 1 0 0 0 RT_WitchDoctorThink RT_WitchDoctorCol RT_WitchDoctorDraw NULL
%action RT_ACT_WitchDoctorJump 0x705A @SPR_WITCHDOCWALKL1 @SPR_WITCHDOCWALKR1 Frame 0 0 0 0 0 CK_Fall RT_WitchDoctorCol RT_WitchDoctorJumpDraw NULL
%action RT_ACT_JanitorWalk0 0x7078 @SPR_JANITORWALK1L @SPR_JANITORWALK1R UnscaledOnce 0 1 10 64 0 RT_JanitorWalk RT_JanitorCol CK_BasicDrawFunc2 RT_ACT_JanitorWalk1
%action RT_ACT_JanitorWalk1 0x7096 @SPR_JANITORWALK2L @SPR_JANITORWALK2R UnscaledOnce 0 1 10 64 0 RT_JanitorWalk RT_JanitorCol CK_BasicDrawFunc2 RT_ACT_JanitorWalk0
%action RT_ACT_JanitorStand 0x70B4 @SPR_JANITORTHINKL @SPR_JANITORTHINKR UnscaledOnce 0 0 120 128 0 NULL RT_JanitorCol CK_BasicDrawFunc1 RT_ACT_JanitorWalk0
%action RT_ACT_Stingaru 0x70D2 0 0 Frame 0 1 0 0 0 RT_StingaruThink RT_StingaruCol RT_StingaruDraw NULL
%action RT_ACT_StingaruJump 0x70F0 @SPR_STINGARUJUMPL3 @SPR_STINGARUJUMPR3 Frame 0 0 0 0 0 CK_Fall RT_StingaruCol RT_StingaruJumpDraw NULL
%action RT_ACT_StingaruStunned 0x710E @SPR_STINGARUSTUNNED @SPR_STINGARUSTUNNED Frame 0 0 0 0 0 CK_Fall NULL CK_BasicDrawFunc4 NULL
'''

LEVEL_MUSIC = [6, 3, 1, 2, 1, 1, 2, 3, 1, 0, 0, 0, 3, 0, 0, 2, 1, 4, 1, 3, 5]


def chunk(n):
    """Keen 4's chunk number to RTTS's."""
    if n < DOOR_SPRITE or n > 0xFFFF:
        return n
    if n <= SPRITES_END:
        return n if n == DOOR_SPRITE else n - 1
    return n + SHIFT


def exe_strings(exe):
    """The game's own texts, from its data segment (C strings, in order)."""
    data = exe.read_bytes()
    found = {}
    for m in re.finditer(rb'[\x09\x0a\x20-\x7e]{2,}\x00', data):
        found.setdefault(m.group()[:-1].decode('latin-1'), m.start())
    return found


def strings_between(exe, first, count):
    """count C strings in a row, from the one equal to first."""
    data = exe.read_bytes()
    at = data.find(first.encode('latin-1') + b'\0')
    if at < 0:
        sys.exit(f'RTTS.EXE: "{first}" not found; not the v1.8 release?')
    out = []
    while len(out) < count:
        end = data.index(b'\0', at)
        text = data[at:end].decode('latin-1')
        if text:
            out.append(text)
        at = end + 1
    return out


def exe_string(exe, anchor):
    """The whole C string in RTTS.EXE that starts with anchor."""
    data = exe.read_bytes()
    at = data.find(anchor.encode('latin-1'))
    if at < 0:
        sys.exit(f'RTTS.EXE: "{anchor}" not found; not the v1.8 release?')
    return data[at:data.index(b'\0', at)].decode('latin-1')


def quote(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def main(game, output):
    game, output = Path(game), Path(output)
    files = {p.name.upper(): p for p in game.iterdir()}
    for name, digest in FILES.items():
        if name not in files:
            sys.exit(f'{name} is missing from {game}')
        if hashlib.sha256(files[name].read_bytes()).hexdigest() != digest:
            sys.exit(f'{name} is not the v1.8 release (2022-12-21)')
    exe = files['RTTS.EXE']
    names = strings_between(exe, 'Far Shadowlands', 20)
    messages = strings_between(exe, 'Keen returns to the\nShadowlands', 20)

    out = ['# Return to the Shadowlands v1.8, by Ceilick; programming K1n9_Duk3.',
           '# Written by kwtools/kw_rtts.py. Native code: src/kw_rtts.c. It runs on Keen 4.',
           '', '%string ca_graphHead "EGAHEAD.CK4"',
           '%string ca_audioInfoFile "AUDINFOE.RTT"', '%int ca_audioUncompressed 1',
           '%string ca_audioHead "AUDIOHED.CK4"', '%string ca_audioFile "AUDIOT.CK4"',
           '%string ca_mapHead "MAPHEAD.CK4"',
           '# (no such file: the tile info is in MAPHEAD.CK4, after the map offsets)',
           '%string ca_tileInfo "TILEINFO.RTT"',
           '%int ck_maxLevel 20', '%int ck_maxEnterLevel 20', '%int ck_maxDoneLevel 20',
           '%int CK_highScoreLevel 19', '%int CK4_NumCouncilsToRescue 8',
           '%string ck_str_levelWarp "  Warp to which level(1-20):"',
           '%string ck_str_exitToMap "Exit to Far Shadowlands"',
           '%string ck4_str_statusRescued "SCOUTS"', '',
           '%intarray CK_LevelMusic ' + ', '.join(map(str, LEVEL_MUSIC)),
           '%int ck4_specialMusic 5', '%int CK_NumMusicLevels 21', '']
    # Level 19 is the high score map; its name is empty in the game.
    names = names[:19] + [''] + names[19:]
    messages = messages[:19] + [''] + messages[19:]
    # Its story, from the opening crawl (K4_LEVEL.C crawltext), after the title
    crawl = exe_string(exe, 'While working in his')
    out.append('%string ck_str_storyText\n\t' + quote('Return to the Shadowlands\n\n' + crawl))
    out.append('%stringarray ck_str_levelNames')
    out.append(',\n'.join('\t' + quote(n) for n in names))
    out.append('%stringarray ck_str_levelEntryText')
    out.append(',\n'.join('\t' + quote(m) for m in messages))
    out.append('')

    # Its dialogue (K4_LEVEL.C), in Keen 4's own dialogue windows
    say = lambda anchor: exe_string(exe, anchor)
    out += ['%int CK4_LindseyMessage1Level 12', '%int CK4_JanitorLevel -1', '%int CK4_JanitorPic 87',
            '%string ck4_str_lindsaySays ""',
            '%string ck4_str_lindsayMessage1 ' + quote(say('Commander Keen!\nI\'m afraid')),
            '%string ck4_str_lindsayMessage2 ' + quote(say('The Oracle Janitor has')),
            '%string ck4_str_lindsayThanks1 ' + quote(say('Any time, your highness!')),
            '%string ck4_str_lindsayThanks2 ' + quote(say('Don\'t worry, princess,')),
            '%string ck4_str_janitor1 ' + quote(say('I\'ve searched for those')),
            '%string ck4_str_janitor2 ' + quote(say('I\'ll leave the rest')),
            '%string ck4_str_janitor3 ' + quote(say('Now wait a minute,')),
            '%string ck4_str_janitor4 ' + quote(say('Here\'s my mop.')),
            '%string ck4_str_cantSwim ' + quote(say('I don\'t have\nmy wetsuit!')),
            '%string ck4_str_gotWetsuit ' + quote(say('I wonder who leaves'))]
    # Each scout's words and Keen's answer, by level (OracleDialog: 17 and
    # any other level have the default's)
    scout = {3: 'Captain Keen!\nRemember', 5: 'Oh Comrade Keen!', 8: 'You don\'t happen to',
             10: 'Help! I\'m afraid', 13: 'Did you see it, Captain?', 14: 'Aw schucks',
             16: 'Oh my, Corporal Keen', 17: 'Commodore Keen!'}
    reply = {3: 'You again?', 5: 'You\'re even more', 8: 'Meet the others', 10: 'How did you get',
             13: 'You should be more', 14: 'I could just', 16: 'I can\'t disagree', 17: 'It\'s Comman-'}
    out.append('%stringarray rtts_str_scout')
    out.append(',\n'.join('\t' + quote(say(scout.get(level, scout[17]))) for level in range(21)))
    out.append('%stringarray rtts_str_keenScout')
    out.append(',\n'.join('\t' + quote(say(reply.get(level, reply[17]))) for level in range(21)))
    out.append('')

    # Every graphics chunk, renumbered; the rest of GFXCHUNK.CK4 as it is.
    gfx = (KEEN4 / 'GFXCHUNK.CK4').read_text()
    lump_names = []
    for line in gfx.splitlines():
        m = re.match(r'%int\s+(\w+)\s+(-?\d+)', line)
        if m and m.group(1) not in ('NUMLUMPS',) and not m.group(1).startswith('LUMP_'):
            out.append(f'%int {m.group(1)} {chunk(int(m.group(2)))}')
        elif m and m.group(1).startswith('LUMP_'):
            lump_names.append((m.group(1), int(m.group(2))))
    for name, number in NEW_SPRITES.items():
        out.append(f'%int {name} {number}')
    starts = re.search(r'%intarray lumpStarts(.*?)%intarray', gfx, re.S).group(1).strip()
    ends = re.search(r'%intarray lumpEnds(.*?)#', gfx, re.S).group(1).strip()
    count = len(lump_names)
    out.append(f'%int NUMLUMPS {count + len(NEW_LUMPS)}')
    out.append('%intarray lumpStarts\n\t' + starts + ',\n\t' + ',\n\t'.join('@' + s for _, s, _ in NEW_LUMPS))
    out.append('%intarray lumpEnds\n\t' + ends + ',\n\t' + ',\n\t'.join('@' + e for _, _, e in NEW_LUMPS))
    for name, number in lump_names:
        out.append(f'%int {name} {number}')
    for i, (name, _, _) in enumerate(NEW_LUMPS):
        out.append(f'%int {name} {count + i}')
    # Its sounds that Keen 4 hasn't (AUDIOCK4.H)
    out += ['', '%int SOUND_RTWITCHDOCSEARCH 11', '%int SOUND_RTWITCHDOCCHARGE 24',
            '%int SOUND_RTSHOWSTATUS 34', '%int SOUND_RTBADOINGPOPUP 52', '%int SOUND_RTSTINGARUJUMP 53', '']
    out += ACTIONS.strip('\n').split('\n')
    # The Treasure Eater's actions, with its RTTS rules (kw_rtts.c)
    for line in (KEEN4 / 'ACTION.CK4').read_text().splitlines():
        if line.startswith('%action CK4_ACT_Smirky'):
            f = line.split()
            name = f[1]
            if not name.startswith('CK4_ACT_SmirkyStunned'):
                f[12] = 'RT_EaterCol'
            if name == 'CK4_ACT_Smirky1':
                f[11] = 'RT_EaterSearch'
            if name == 'CK4_ACT_SmirkyTele3':
                f[11] = 'RT_EaterVanish'
            if name.startswith('CK4_ACT_SmirkyJump'):
                f[13] = 'RT_EaterDraw'
            out.append(' '.join(f))
    out += ['', '%override "EPISODE.CK4"', '']

    output.mkdir(parents=True, exist_ok=True)
    (output / 'RTTS.CK4').write_text('\n'.join(out))
    # numSongs, numSounds, numSndChunks, startPC, startAdLib, startDigi, startMusic
    (output / 'AUDINFOE.RTT').write_bytes(struct.pack('<7H', SONGS, SOUNDS, 3 * SOUNDS + SONGS,
                                                      0, SOUNDS, 2 * SOUNDS, 3 * SOUNDS))
    print(f'RTTS.CK4 and AUDINFOE.RTT written to {output} ({len(names)} levels)')


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
