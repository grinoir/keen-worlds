"""Write Keen Worlds' variable file for Aurora Bliss.

    python3 kwtools/kw_aurora.py GAMEDIR omnispeak/data/keen4 play/mods/aurora [CRAIGDIR]

GAMEDIR: the player's copy of Aurora Bliss v1.2 (Dave216, 2022-23; the
files of m.dave216.aurora_bliss_v.1.2.k4.2023-08-23.zip, unpacked). It is a
Keen 4 mod: its own levels, graphics (with its own graphics dictionary),
music and sounds, and a CKPatch file (abiathar.pat) that changes texts and a
number of the program's rules. Texts, level names and the music table are
turned into settings as for The Eight Accumulators (kw_accumulators.py does
that work); each change to the program's rules has a setting in the engine's
Keen 4 code, listed in RULES with the patch it stands for, and the patched
sprite actions are written out as %action lines (kw_patconv.py). Audit:
research/ocflore-and-aurora-bliss.md in the workspace root.

Aurora Bliss ships no Craig player sprite -- you walk as Keen -- but it is
Craig's story, part one, so after the conversion this tool splices Craig's
player sprite in (kw_craig_splice), taken from the already-converted Revival
of Bliss Craig build. CRAIGDIR is that build's output (default: a sibling
'revivalc' of the output dir); build revivalc first (kw_revival.py). If it is
missing the splice is skipped with a warning and Aurora stays as Keen.
"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_accumulators import convert
import kw_craig_splice

FILES = {'AUDIO.CK4': 'AUDIO.CK4', 'AUDIODCT.CK4': 'AUDIODCT.CK4', 'AUDIOHED.CK4': 'AUDIOHED.CK4',
         'EGADICT.CK4': 'EGADICT.CK4', 'EGAGRAPH.CK4': 'EGAGRAPH.CK4', 'EGAHEAD.CK4': 'EGAHEAD.CK4',
         'GAMEMAPS.CK4': 'GAMEMAPS.CK4', 'maphead.ck4': 'MAPHEAD.CK4', 'Keen4.tli': 'KEEN4.TLI'}
PAT = 'abiathar.pat'

# (the patches' places in the program, what they do, the setting)
RULES = [
    # sound and music: its own header and dictionary, 13 songs from chunk 149
    # (the patch file sets them twice; the second set counts)
    ((0x8CEF, 0x8C91, 0x8C75, 0x8C83, 0x8D83, 0x8CA4, 0x20DF0, 0x354F6), 'its own sound and music',
     '%string ca_audioHead "AUDIOHED.CK4"\n%string ca_audioDict "AUDIODCT.CK4"\n'
     '%int ca_audStartMusic 149\n%int ca_audNumSongs 13'),
    ((0xCC2A, 0x905B), 'no underwater level (the Well of Wishes exits and scrolls normally)',
     '%int CK4_UnderwaterDeathLevel 99\n%int CK4_UnderwaterCameraLevel 99'),
    ((0x6AE6,), 'seven council members (key carriers) to rescue', '%int CK4_NumCouncilsToRescue 7'),
    ((0xEFB6, 0xF033), "Lindsey's Miragia hint is in level 6", '%int CK4_LindseyMessage1Level 6'),
    ((0xCD9E,), 'finding the janitor ends the level', '%int CK4_JanitorEndsLevel 1'),
    ((0x123FB, 0x12404), 'a hungry Dopefish kills Keen at once', '%int CK4_DopefishKillsAtOnce 1'),
    ((0x12385,), 'the Dopefish does not burp aloud', '%int CK4_DopefishQuiet 1'),
    ((0x10D42, 0xCEB5), 'the Berkeloid does not harm Keen (on poles either)', '%int CK4_BerkeloidHarmless 1'),
    ((0x10D89,), 'its flame goes out when it lands', '%int CK4_FireballVanishes 1'),
    ((0x10DB5,), 'the Berkeloid walks instead of floating', '%int CK4_BerkeloidNoHover 1'),
    ((0x11B90, 0x11C13), 'the Treasure Eater never teleports', '%int CK4_SmirkyNoTeleport 1'),
    ((0x11166, 0x11172), 'a shot only makes a bounder hop', '%int CK4_BounderUnstunnable 1'),
    ((0x111AE,), 'bounders make no sound', '%int CK4_BounderQuiet 1'),
    ((0x111BC,), 'bounders slide (a hop of 1)', '%int CK4_BounderBounceYVelocity -1'),
    ((0x111C2,), 'bounders go where Keen is', '%int CK4_BounderFollowsKeen 1'),
    ((0x111FE, 0x11260), 'bounders slide at 16', '%int CK4_BounderBounceXVel 16\n%int CK4_BounderRideXVel 16'),
    ((0x10219,), 'the blue bird never flies', '%int CK4_BirdNeverFlies 1'),
    ((0x100BA, 0x100BF), 'a shot pops on the egg; only Keen hatches it', '%int CK4_EggHatchesOnlyForKeen 1'),
    ((0x1030A,), 'a shot just pops on the hatched bird', '%int CK4_BirdShotProof 1'),
    ((0xFF92,), 'the Mad Mushroom always makes its big jump', '%int CK4_MushroomNumJumps 1'),
    ((0xA7B2, 0xA7B7), "the schoolfish's hitbox for walls", '%int CK4_SchoolfishClipX 0x200\n%int CK4_SchoolfishClipY 0x150'),
    ((0x5836,), 'the story scrolls to song 7', '%int CK_StarWarsMusicLevel 7'),
    ((0x9CD7,), 'the ending plays song 2', '%int CK_EndingMusicLevel 2'),
    ((0xF107, 0xF4CA), "the world map's song while Lindsey and the Ancients talk", '%int CK4_TalkMusicLevel 0'),
    # Not turned into settings: the loading screen (ck4load.bin), K1n9_Duk3's
    # screen update code and the direct audio loading (the DOS program's
    # memory and video timing), and $115A (meant for the bounder, it lands in
    # unrelated start-up code: a typing error in the mod, no effect).
    ((0x1E7F9, 0x1E87A, 0x13466, 0x13373, 0x115A, 0x1F040), None, None),
]


def main(game, keen4, out, craigdir=None):
    extra, handled = [], []
    for places, what, setting in RULES:
        handled += places
        if setting:
            extra += ['# ' + what] + setting.split('\n')
    convert(game, keen4, out, pat=PAT, files=FILES, extra=extra, handled=handled,
            skip=('keen4.tli', 'ck4load.bin', 'audiohed.ck4', 'audiodct.ck4'),
            title='Aurora Bliss (Dave216, 2022-23, v1.2)', tool='kwtools/kw_aurora.py')
    # The sprite actions the patches change, after the %override line.
    import io, contextlib, kw_patconv
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        sys.argv = ['kw_patconv.py', 'actions', os.path.join(game, 'KEEN4E.EXE'), os.path.join(game, PAT), keen4]
        kw_patconv.main()
    actions = [l for l in buf.getvalue().splitlines() if l.startswith('%action')]
    # The mod's patch for the second walking frame of the blue bird missed by
    # two bytes: it wrote the new speed (256) over the action's type, which
    # no engine knows. What it meant is what the other three frames got.
    actions = [l.replace(' 256 0 1 7 0x80 0 ', ' UnscaledOnce 0 1 7 256 0 ')
               if l.startswith('%action CK4_ACT_BirdWalk1 ') else l for l in actions]
    assert not any(' 256 0 1 ' in l for l in actions), 'an action type the engine does not know'
    with open(os.path.join(out, 'MOD.CK4'), 'a', encoding='latin1') as f:
        f.write('\n# The sprite actions the patches change (the blue bird walks and kills like\n'
                '# the Mad Mushroom, the schoolfish swim, the Dopefish, the stunned bounder\n'
                '# recovers, the air-door fix)\n' + '\n'.join(actions) + '\n')
    print('%d actions' % len(actions))
    # Craig's story begins as Craig: splice his player sprite in from the
    # Revival of Bliss Craig build (its graphics share Aurora's dictionary).
    craig = craigdir or os.path.join(os.path.dirname(os.path.abspath(out)), 'revivalc')
    if os.path.exists(os.path.join(craig, 'EGAGRAPH.CK4')):
        kw_craig_splice.main(out, craig)
    else:
        print('NOTE: %s has no EGAGRAPH.CK4; Craig not spliced (build revivalc '
              'first, or pass CRAIGDIR). Aurora stays as Keen.' % craig)


if __name__ == '__main__':
    if len(sys.argv) not in (4, 5):
        sys.exit(__doc__)
    main(*sys.argv[1:])
