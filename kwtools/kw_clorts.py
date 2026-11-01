"""Write Keen Worlds' variable file for Send in the Clorts.

    python3 kwtools/kw_clorts.py GAMEDIR KEEN5E.EXE omnispeak/data/keen5 play/mods/clorts

GAMEDIR: the player's copy of Send in the Clorts (the files of its game
folder, with sc.zip unpacked so KEEN5a.PAT is beside the data). It is a Keen
5 v1.4 mod: its own levels, graphics, music and sounds, and one CKPatch file
(KEEN5a.PAT) that changes texts, the music table and some of the program's
rules. It doesn't ship the program it patches: KEEN5E.EXE is Keen 5 v1.4 (the
player's own; it is only read, like Keen 5 and Keen 6's base games). Texts,
level names and the music table are turned into settings as for The Eight
Accumulators (kw_accumulators.py, episode 5); each change to the program's
rules has a setting in the engine's Keen 5 code, listed in RULES with the
patch it stands for. The mod adds no routines of its own (its one code patch
only makes Sparkies ignore the stunner, which is CK5_SparkyShotProof), so it
has no .c file. The "boats" reuse the purple-platform lump, moved onto four
of the Shikadi Master's unused frames; that and two tightened caches are
written out as the lumpStarts/lumpEnds arrays. Audit:
research/ocflore-and-aurora-bliss.md in the workspace root.
"""
import os, re, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert, escape
import kw_patconv

FILES = {'AUDIO.CK5': 'AUDIO.CK5', 'AUDIODCT.CK5': 'AUDIODCT.CK5', 'AUDIOHED.CK5': 'AUDIOHED.CK5',
         'EGAGRAPH.CK5': 'EGAGRAPH.CK5', 'EGAHEAD.CK5': 'EGAHEAD.CK5', 'GAMEMAPS.CK5': 'GAMEMAPS.CK5',
         'maphead.ck5': 'MAPHEAD.CK5', 'Keen5.tli': 'KEEN5.TLI'}
PAT = 'KEEN5a.PAT'

# The Chronicle scroll (ck_str_storyText): the game tells its story as a
# picture sequence (its TEXT_STORY chunk); this is that story, cut to a scroll
# in the game's own words. It follows straight on from The Prisoner's Dilemma
# (the A-Danac system) and chases Mortimer's Re-Synapsizer to Aleph Zilch.
STORY = (
    "Acting on a hot tip\nfrom the A-Danac system,\nCommander Keen arrives\non planet Aleph Zilch,\n"
    "hunting Mortimer McMire\nand his latest\ninvention, the\nRe-Synapsizer. The\ndevice is about to\n"
    "re-wire the brains of\nevery sentient being in\nthe universe, killing\nninety percent of them\n"
    "in the process. Keen\nsteps out of the Bean-\nwith-Bacon Megarocket,\ntakes a look around,\n"
    "and is joined by his\npart-time sidekick Rick\nUller, who stopped for\na banana milkshake on\n"
    "the way. \"This planet\nlooks empty,\" says\nKeen. \"Not as empty as\nthis cup,\" says Rick.\n"
    "Below them, the\nRe-Synapsizer is\nwarming up.")

# (the patches' places in the program, what they do, the setting)
RULES = [
    ((0x21C80, 0x35EC4), 'its own sound and music (20 songs from chunk 186)',
     '%string ca_audioHead "AUDIOHED.CK5"\n%string ca_audioDict "AUDIODCT.CK5"\n'
     '%int ca_audStartMusic 186\n%int ca_audNumSongs 20'),
    ((0xFFC2,), 'the game ends when the fuse on level 10 (Clortopolis) is destroyed, not Keen 5\'s level 12',
     '%int CK5_QEDLevel 10'),
    ((0xB5E3,), 'switches lay down thicker bridges', '%int CK_BridgeThickness 6'),
    ((0x12EFA,), 'the Shikadi takes two shots to kill', '%int CK5_ShikadiHealth 2'),
    ((0x137AC,), 'Spherefuls scatter no particles', '%int CK5_SpherefulDiamonds 0'),
    ((0x1117C,), 'Sparkies ignore the stunner (the mod: killed only by each other)', '%int CK5_SparkyShotProof 1'),
    # The sprite lumps: the Shikadi's starts two frames later, the Sphereful's
    # cache ends four sooner, and the purple-platform lump (the mod's boats)
    # moves onto the Master's unused frames 0x13C-0x13F. Written as the whole
    # lumpStarts/lumpEnds arrays below (lumps()); the four offsets here are the
    # same four edits in the program.
    ((0x31A30, 0x31A88, 0x31A44, 0x31A98), None, None),
    # Not settings. Omnispeak: the no-cache-no-crash guard (DOS only); the
    # high-score columns and the game-over sequence (Omnispeak draws its own,
    # and a world is finished by completing it, not by the game-over screen);
    # the ending-sequence song (the world's ending is shown on its own); the
    # map's "out" elevators and its re-enter-14-and-15 (Keen Worlds re-enters
    # every finished level anyway). A few pixels of creature spawn height
    # (Omnispeak uses its own), the quiet fuse smash (no tile swap), and the
    # faster conveyors are left as Keen 5 has them.
    ((0x18C22, 0x3CB5, 0x3CB7, 0x3CB9, 0x3CBD, 0x3CBF, 0xEC67,
      0xF6E2, 0xF664, 0xF6B1, 0xF714, 0xF725,
      0xF451, 0xF455, 0xF460, 0xF464, 0xF497, 0xF49B, 0xF4B2, 0xF4B6, 0x60B3,
      0xCAE2, 0xCADE, 0xC60F, 0xC638, 0xC6CD, 0xC6F1,
      0x125C7, 0x13282, 0x11290, 0x11054,
      # the pause menu's "RETURN TO GAME" / "HIGH SCORES" labels (Omnispeak's own)
      0x3469A, 0x346A9), None, None),
    # Captured by the %action lines: the frames the patches recolour or retime
    # (the boats stolen from the Master, the Shocksund projectile, the Shikadi
    # cache, Shelley dissolving, the Sphereful's animation and the Sparky's one
    # front frame).
    ((0x31EBC, 0x31EC6, 0x31EDA, 0x31EE4,
      0x332E4, 0x33302, 0x33050, 0x3306E,
      0x3288C, 0x328AA, 0x328C8, 0x328E6,
      0x33384, 0x333A2, 0x333C0, 0x333DE,
      0x322E2, 0x32300, 0x3233C, 0x3235A, 0x32378, 0x324A4, 0x324C2, 0x324E0), None, None),
]
# (convert() reads the mod's texts itself, the cloaking-device line among them:
# Keen 5's machine-destroyed message ck5_str_fuseDestroyed, at $31C20.)
# The purple-platform lump (the boats) and the two tightened caches, matched by
# the value Keen 5 has there (unique): new start/end.
LUMP_STARTS = {0x140: 0x142, 0x1A6: 0x13C}   # Shikadi later; boats onto the Master's frames
LUMP_ENDS = {0x16D: 0x169, 0x1A7: 0x13F}     # Sphereful cache shorter; boats' four frames


def lumps(keen5):
    """Keen 5's lumpStarts/lumpEnds, with the mod's four edits (matched by the
    value Keen 5 has, so index order doesn't matter)."""
    text = open(os.path.join(keen5, 'GFXCHUNK.CK5'), encoding='latin1').read()
    defs = {m.group(1): int(m.group(2)) for m in re.finditer(r'%int\s+(SPR_\w+|PIC_\w+|FON_\w+)\s+(\d+)', text)}
    out = []
    for name, edits in (('lumpStarts', LUMP_STARTS), ('lumpEnds', LUMP_ENDS)):
        block = re.search('%intarray ' + name + r'\n((?:\s+@?\w+,?\n)+)', text).group(1)
        nums = []
        for ln in block.strip().splitlines():
            tok = ln.strip().rstrip(',').lstrip('@')
            val = defs[tok] if tok in defs else int(tok, 0)
            nums.append(edits.get(val, val))
        for old in edits:
            assert sum(1 for ln in block.strip().splitlines()
                       if defs.get(ln.strip().rstrip(',').lstrip('@')) == old) == 1, (name, hex(old))
        out.append('%%intarray %s\n%s' % (name, ',\n'.join('\t%d' % n for n in nums)))
    return '\n'.join(out)


def main(game, exe, keen5, out):
    extra = ['# the Chronicle scroll (the game\'s own intro story, cut to a scroll)',
             '%%string ck_str_storyText "%s"' % escape(STORY)]
    handled = []
    for places, what, setting in RULES:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    convert(game, keen5, out, pat=PAT, files=FILES, extra=extra, handled=handled, episode=5, exe=exe,
            skip=('keen5.tli', 'ck5load.bin', 'audiohed.ck5', 'audiodct.ck5'),
            title="Send in the Clorts (a Keen 5 v1.4 mod)", tool='kwtools/kw_clorts.py')
    import contextlib, io
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', exe, os.path.join(game, PAT), keen5]
        kw_patconv.main()
    acts = [l.split('  #')[0] for l in buf.getvalue().splitlines() if l.startswith('%action')]
    with open(os.path.join(out, 'MOD.CK5'), 'a', encoding='latin1') as f:
        f.write('\n# Its sprite actions, as the patches leave them (Keen 5\'s own, frames changed)\n'
                + '\n'.join(acts) + '\n')
        f.write("\n# The sprite lumps: the boats onto the Master's frames, two caches tightened\n" + lumps(keen5) + '\n')
    print('%d actions' % len(acts))


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    main(*sys.argv[1:])
