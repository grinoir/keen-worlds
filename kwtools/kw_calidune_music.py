"""Music for Calidune's arrival scene (Keen Worlds): AdLib, three candidates.

Keen is caught by Mortimer's transporter in orbit and set down alone on the
desert planet Calidune (Dead in the Desert's opening). The user chooses one
of these by ear:

  1 transporter  "Transporter Trap": D minor, tense; a pulsing bass, an
                 alarm-like lead on two notes, the kit building up.
  2 sands        "Dead Sands": E Phrygian dominant (the desert scale), slow;
                 a drone bass, a long lead that slides, toms and a cymbal.
  3 both         "Trap to Sand": the trap's alarm for the orbit, then a break
                 and the desert groove for the sand.

The voices, drums and IMF output are those of the title tune
(kw_title_music.py), so it sounds like the same band.

Usage: python3 kwtools/kw_calidune_music.py imf CANDIDATE out.imf
       python3 kwtools/kw_calidune_music.py csource CANDIDATE > omnispeak/src/kw_calidune_music.h
"""
import sys
from kw_title_music import (midi, opl_pitch, BASS_VOICE, STAB_VOICE, LEAD_VOICE, BASS_CH, STAB_CH, LEAD_CH,
                            OPS, DRUM_OPS, DRUM_PITCH, RHYTHM, BD, SD, TOM, CYM, HH)

GAP = 10
R = '.  .  .  .   .  .  .  .   .  .  .  .   .  .  .  .'   # a bar's rest

# ---- 1: Transporter Trap (D minor, 132 bpm)
TRAP_BASS = ['D2 . D2 D3  . D2 . D2   D3 . D2 .   D2 D3 . D2',
             'D2 . D2 D3  . D2 . D2   D#2 . D#2 .  D#2 D#3 . D#2']
TRAP_LEAD = ['A4 - . .   D5 - . .    A4 - . .    D5 - . .',
             'A4 - . .   D#5 - . .   A4 - . .    D#5 - - -',
             'F5 - - -   E5 - D5 -   ~C#5 - - -  A4 - - -',
             'F5 - - -   G5 - F5 -   E5 - D5 -   C#5 - - -',
             'D5 - - -   F5 - - -    A5 - - -    ~G#5 - - -',
             'A5 - - -   - - - -     ~D5 - - -   - - - -']
TRAP_CHORD = {'Dm': ('D4', 'F4', 'A4'), 'Eb': ('D#4', 'G4', 'A#4'), 'A7': ('C#4', 'G4', 'A4')}
TRAP_DRUMS = {
    'pulse': {HH: range(0, 16, 2)},
    'build': {BD: [0, 6, 8, 14], SD: [4, 12], HH: range(16)},
    'drive': {BD: [0, 3, 6, 8, 11, 14], SD: [4, 12], HH: range(16)},
    'fill':  {BD: [0, 6], SD: [8, 10, 11, 12, 13, 14, 15], TOM: [4, 6]},
}

# ---- 2: Dead Sands (E Phrygian dominant, 88 bpm)
SAND_BASS = ['E2 - - -   - - - -   E2 - - -   F2 - E2 -',
             'E2 - - -   - - - -   E2 - - -   D2 - C2 -']
SAND_LEAD = ['. . . .    E4 - F4 -   G#4 - - -  - - - .',
             'A4 - G#4 -  F4 - - -   E4 - - -   - - . .',
             '. . . .    B4 - C5 -   ~D5 - - -  C5 - B4 -',
             'C5 - B4 -   A4 - G#4 -  ~A4 - - -  G#4 - F4 -',
             'E4 - - -   G#4 - B4 -  ~E5 - - -  - - D5 -',
             'C5 - B4 -   A4 - G#4 -  F4 - - -   ~E4 - - -']
SAND_CHORD = {'E': ('E4', 'G#4', 'B4'), 'F': ('F4', 'A4', 'C5'), 'Dm': ('D4', 'F4', 'A4')}
SAND_DRUMS = {
    'still': {},
    'toms':  {TOM: [0, 6, 10], HH: [4, 12]},
    'walk':  {BD: [0, 10], TOM: [6, 13], SD: [8], HH: [4, 12, 14]},
}


def song(candidate):
    """(16th length in ticks, [(bass, lead, chord or None, stab 16ths, drums, cymbal)])"""
    trap, sand = [], []
    for i in range(2):
        trap.append((TRAP_BASS[0], '', None, [], 'pulse', False))
    for i, lead in enumerate(TRAP_LEAD):
        chord = 'Eb' if i % 2 else 'Dm'
        trap.append((TRAP_BASS[i % 2], lead, chord, [0, 8], 'build' if i < 2 else 'drive', i == 2))
    trap.append((TRAP_BASS[1], '', 'A7', [0], 'fill', False))
    for i in range(2):
        sand.append((SAND_BASS[i], '', None, [], 'still', False))
    for i, lead in enumerate(SAND_LEAD):
        chord = ('E', 'F', 'E', 'Dm', 'E', 'F')[i]
        sand.append((SAND_BASS[i % 2], lead, chord, [0], 'toms' if i < 4 else 'walk', i == 0))
    if candidate == 'transporter':
        return 57, trap
    if candidate == 'sands':
        return 86, sand
    # Trap to Sand: the trap's first half and its fill, then the desert
    # at the desert's own tempo (written as twice the 16ths in each bar)
    return 57, trap[:6] + [trap[-1]] + sand_at()


def sand_at():
    # The desert bars at half speed in the trap's tempo: every 16th lasts two.
    bars = []
    for bass, lead, chord, stabs, drums, cym in [(SAND_BASS[i % 2], l, ('E', 'F', 'E', 'Dm', 'E', 'F')[i], [0],
                                                  'toms' if i < 4 else 'walk', i == 0)
                                                 for i, l in enumerate(SAND_LEAD)]:
        for half in (0, 1):
            b = double(bass, half)
            l = double(lead, half)
            bars.append((b, l, chord if half == 0 else None, [0] if half == 0 else [],
                         drums + '/2' + str(half), cym and half == 0))
    return bars


def double(text, half):
    """Half of a bar played at half speed: each 16th becomes two."""
    notes = text.split()
    if not notes:
        return ''
    part = notes[half * 8:half * 8 + 8]
    out = []
    for n in part:
        out += [n, '-' if n not in '.' else '.']
    return ' '.join(out)


def drums_for(name):
    table = dict(TRAP_DRUMS)
    table.update(SAND_DRUMS)
    if '/2' in name:
        base, half = name.split('/2')
        pattern = table[base]
        half = int(half)
        return {d: [(s - half * 8) * 2 for s in where if half * 8 <= s < half * 8 + 8] for d, where in pattern.items()}
    return table[name]


def build(candidate):
    step, bars = song(candidate)
    chords = dict(TRAP_CHORD)
    chords.update(SAND_CHORD)
    ev = []

    def setpitch(t, ch, m, order=2):
        block, f = opl_pitch(m)
        ev.append((t, order, 0xA0 + ch, f & 0xFF))
        ev.append((t, order + 1, 0xB0 + ch, 0x20 | block << 2 | f >> 8))

    def off(t, ch, m=None):
        if m is None:
            ev.append((t, 0, 0xB0 + ch, 0))
        else:
            block, f = opl_pitch(m)
            ev.append((t, 0, 0xB0 + ch, block << 2 | f >> 8))

    for ch, v in ((BASS_CH, BASS_VOICE), (STAB_CH, STAB_VOICE), (STAB_CH + 1, STAB_VOICE),
                  (STAB_CH + 2, STAB_VOICE), (LEAD_CH, LEAD_VOICE)):
        mult, level, ad, sr, fb = v
        for base, vals in ((0x20, mult), (0x40, level), (0x60, ad), (0x80, sr)):
            for op, val in zip(OPS[ch], vals):
                ev.append((0, -2, base + op, val))
        ev.append((0, -2, 0xC0 + ch, fb))
        off(0, ch)
    for op, mult, level, ad, sr in DRUM_OPS:
        for base, v in ((0x20, mult), (0x40, level), (0x60, ad), (0x80, sr)):
            ev.append((0, -2, base + op, v))
    for ch, block, f in DRUM_PITCH:
        ev.append((0, -2, 0xC0 + ch, 0x08 if ch == 6 else 0x01))
        ev.append((0, -2, 0xA0 + ch, f & 0xFF))
        ev.append((0, -2, 0xB0 + ch, block << 2 | f >> 8))
    ev.append((0, -1, 0xBD, RHYTHM))

    def line(text, ch, t0, prev):
        notes = text.split()
        assert len(notes) in (0, 16), (len(notes), text)
        for i, n in enumerate(notes):
            if n in '-.':
                continue
            slide = n.startswith('~')
            m = midi(n.lstrip('~'))
            length = 1
            while i + length < 16 and notes[i + length] == '-':
                length += 1
            t = t0 + i * step
            if slide and prev is not None and prev != m:
                for s in range(9):
                    setpitch(t + s * step // 8, ch, prev + (m - prev) * s / 8, order=2 if s == 0 else 4)
            else:
                setpitch(t, ch, m)
            off(t + length * step - GAP, ch, m)
            prev = m
        return prev

    last = None
    for bar, (bass, lead, chord, stabs, drums, cym) in enumerate(bars):
        t0 = bar * 16 * step
        line(bass, BASS_CH, t0, None)
        last = line(lead, LEAD_CH, t0, last)
        if chord:
            for s16 in stabs:
                t = t0 + s16 * step
                length = 8 if len(stabs) > 1 else 16
                for j, n in enumerate(chords[chord]):
                    setpitch(t, STAB_CH + j, midi(n))
                    off(t + length * step - GAP, STAB_CH + j, midi(n))
        pattern = drums_for(drums)
        for s16 in range(16):
            hits = 0
            for drum, where in pattern.items():
                if s16 in where:
                    hits |= drum
            if s16 == 0 and cym:
                hits |= CYM
            if hits:
                t = t0 + s16 * step
                ev.append((t, 5, 0xBD, RHYTHM | hits))
                ev.append((t + step - GAP, 0, 0xBD, RHYTHM))
    end = len(bars) * 16 * step
    ev.append((end, 9, 0xB0 + BASS_CH, 0))
    ev.sort()
    out = bytearray()
    for i, (t, _, reg, val) in enumerate(ev):
        delay = ev[i + 1][0] - t if i + 1 < len(ev) else 0
        out += bytes((reg, val, delay & 0xFF, delay >> 8))
    assert len(out) < 65536
    return bytes(out)


if __name__ == '__main__':
    if len(sys.argv) == 4 and sys.argv[1] == 'imf':
        open(sys.argv[3], 'wb').write(build(sys.argv[2]))
    elif len(sys.argv) == 3 and sys.argv[1] == 'csource':
        data = build(sys.argv[2])
        print('// Calidune arrival music ("%s"). Generated by kwtools/kw_calidune_music.py.' % sys.argv[2])
        print('static const uint8_t kw_caliduneMusic[%d] = {' % len(data))
        for i in range(0, len(data), 16):
            print('\t' + ' '.join('0x%02x,' % b for b in data[i:i + 16]))
        print('};')
    else:
        sys.exit(__doc__)
