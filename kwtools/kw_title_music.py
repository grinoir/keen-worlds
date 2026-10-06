"""Write the title tune of Commander Keen: Worlds as AdLib music:
"The Greatest Adventure".

Funk in A Dorian, 16th notes at 116 bpm, just under a minute, then round
again. It is made of the sounds of the Megarocket's tune (the same bass,
stab and lead voices, the same drum kit), played by a bigger band:

  bass     a syncopated line that lands hard on the one and walks up to it
  horns    three-note stabs (Am7 - D9, and E7 sharp nine to turn around)
  lead     the tune: a call that climbs and an answer that comes back down,
           with room between them; some notes are slid into (the pitch is
           bent, a Moog's glide)
  drums    bass drum, snare, hi-hat; tom fills, a cymbal where a part begins

  2 bars of bass and drums - A, the tune (8) - B, the lift (8) - A (8) -
  2 bars of break.

The output is an IMF stream (OPL2 register, value, 16-bit delay in 1/560 s
ticks), the format of the games' own music, compiled into the engine so
nobody's AUDIO file is touched.

Usage: python3 tools/kw_title_music.py csource > omnispeak/src/kw_title_music.h
       python3 tools/kw_title_music.py imf out.imf     (for tools/kw_imf2wav.c)
"""
import sys

STEP = 72     # ticks in a 16th: 116 bpm
GAP = 10      # silence before a note is struck again

SEMI = {'C': 0, 'C#': 1, 'D': 2, 'D#': 3, 'E': 4, 'F': 5, 'F#': 6, 'G': 7, 'G#': 8, 'A': 9, 'A#': 10, 'B': 11}


def midi(note):
    return 12 * (int(note[-1]) + 1) + SEMI[note[:-1]]


def opl_pitch(m):
    """(block, f-number) of a pitch given as a (fractional) MIDI note."""
    hz = 440.0 * 2 ** ((m - 69) / 12.0)
    for block in range(8):
        f = int(round(hz * (1 << (20 - block)) / 49716.0))
        if f < 1024:
            return block, f
    return 7, 1023


# ---- The tune: one bar per line, sixteen 16ths. '-' holds, '.' rests,
# '~' before a note slides into it from the note before.

VAMP = [
    ('Am7', 'A1 . . A1  . C2 . D2  D#2 E2 . G2  . A2 G2 E2'),
    ('D9',  'D2 . . D2  . F#2 . A2  C3 . A2 .  F#2 . E2 D2'),
    ('Am7', 'A1 . . A1  . C2 . D2  D#2 E2 . G2  . A2 . C3'),
    ('D9E', 'D2 . . D2  . F#2 . A2  E2 . . E2  . G#2 B2 E3'),
]
TUNE = [
    '. . . .    E4 . G4 A4   - - . G4    A4 . C5 .',
    'A4 - - .   . . F#4 .    E4 - D4 .   E4 - - .',
    '. . . .    E4 . G4 A4   - - . C5    D5 . E5 .',
    'D5 - C5 A4  - . G4 .    G#4 - - -   B4 - - .',
    '. . . .    E4 . G4 A4   - - . G4    A4 . C5 .',
    'D5 - C5 .  A4 . F#4 .   E4 - D4 .   C4 D4 E4 .',
    '. . . .    A4 . C5 E5   - - . D5    C5 . A4 .',
    'D5 - C5 A4  - . G4 .    ~B4 - - -   ~E5 - - .',
]
LIFT = [
    ('F',  'F2 . F2 .  . F2 A2 .  C3 . F2 .  . A2 C3 .',   'A4 - - -   - - C5 -    ~F5 - - -   E5 - C5 -'),
    ('G',  'G2 . G2 .  . G2 B2 .  D3 . G2 .  . B2 D3 .',   'B4 - - -   - - D5 -    ~G5 - - -   F5 - D5 -'),
    ('Am', 'A2 . A2 .  . A2 C3 .  E3 . A2 .  . G2 A2 .',   'E5 - - -   - - - -     . . C5 D5   E5 . G5 .'),
    ('Am', 'A2 . A2 G2  . E2 . D2  C2 . D2 .  D#2 E2 . G2', 'E5 - - D5  - - C5 -    A4 - - -    . . . .'),
    ('F',  'F2 . F2 .  . F2 A2 .  C3 . F2 .  . A2 C3 .',   'C5 . C5 A4  . C5 . F5   - - - .     E5 - C5 .'),
    ('G',  'G2 . G2 .  . G2 B2 .  D3 . G2 .  . B2 D3 .',   'D5 . D5 B4  . D5 . G5   - - - .     F5 - D5 .'),
    ('E7', 'E2 . . E2  . G#2 . B2  D3 . B2 .  G#2 . E2 .', 'E5 - - -   D5 - B4 -   G#4 - - -   B4 - D5 -'),
    ('E7', 'E2 . E3 .  E2 . E3 .  E2 E2 G#2 B2  D3 E3 . .', '~E5 - - -  - - - -     . . . .     . . . .'),
]
BREAK = [
    ('Am7', 'A1 . . A1  . C2 . D2  D#2 E2 . G2  . A2 G2 E2'),
    ('D9E', 'D2 . . D2  . F#2 . A2  E2 . E2 .  E2 G#2 B2 E3'),
]

SONG = []   # (section, chord, bass, lead)
for i in range(2):
    SONG.append(('intro', VAMP[i][0], VAMP[i][1], ''))
for part in ('A', 'B', 'A'):
    if part == 'A':
        for i in range(8):
            SONG.append(('Afill' if i % 4 == 3 else 'A', VAMP[i % 4][0], VAMP[i % 4][1], TUNE[i]))
    else:
        for i, (chord, bass, lead) in enumerate(LIFT):
            SONG.append(('Bfill' if i % 4 == 3 else 'B', chord, bass, lead))
for i, (chord, bass) in enumerate(BREAK):
    SONG.append(('break' if i == 0 else 'breakfill', chord, bass, ''))

# The horns: three notes each
CHORD = {'Am7': ('G3', 'C4', 'E4'), 'D9': ('F#3', 'C4', 'E4'), 'E7': ('G#3', 'D4', 'G4'),
         'F': ('A3', 'C4', 'F4'), 'G': ('B3', 'D4', 'G4'), 'Am': ('C4', 'E4', 'A4')}
STABS = {   # section -> [(16th, length in 16ths)]
    'intro': [],
    'A': [(4, 1), (7, 1), (10, 1), (13, 1), (15, 1)],
    'B': [(0, 3), (6, 2), (10, 3)],
    'break': [(2, 1), (5, 1), (7, 1), (10, 1), (13, 1)],
}
# Drums: 16ths of the bar on which each is hit.
BD, SD, TOM, CYM, HH = 0x10, 0x08, 0x04, 0x02, 0x01
DRUMS = {
    'intro': {BD: [0, 3, 6, 10], SD: [4, 12], HH: [0, 2, 4, 6, 8, 10, 12, 14]},
    'A': {BD: [0, 3, 6, 10], SD: [4, 12], HH: [0, 2, 4, 6, 7, 8, 10, 12, 14]},
    'Afill': {BD: [0, 3, 6, 10, 11], SD: [4, 12, 14, 15], HH: [0, 2, 4, 6, 8, 10]},
    'B': {BD: [0, 8, 10], SD: [4, 12], HH: range(16)},
    'Bfill': {BD: [0, 8], SD: [4, 10, 11, 13, 15], HH: range(8), TOM: [12, 14]},
    'break': {BD: [0, 3, 6, 10], SD: [4, 12], HH: [0, 2, 4, 6, 8, 10, 12, 14]},
    'breakfill': {BD: [0, 3, 8], SD: [4, 10, 11, 14, 15], TOM: [12, 13], HH: [0, 2, 4, 6]},
}
STAB_AS = {'Afill': 'A', 'Bfill': 'B', 'breakfill': 'break'}
CRASH = {'A', 'B'}   # a cymbal on the first beat where one of these begins

# ---- OPL2 voices, as in the Megarocket's tune (tools/kw_ship_music.py):
# (modulator, carrier) register values
#            tremolo/sustain/mult  level        attack/decay  sustain/release  feedback
BASS_VOICE = ((0x01, 0x01), (0x0C, 0x02), (0xF4, 0xF5), (0x68, 0x78), 0x0C)
STAB_VOICE = ((0x02, 0x01), (0x14, 0x10), (0xF5, 0xF6), (0x77, 0x88), 0x08)
LEAD_VOICE = ((0x61, 0x21), (0x18, 0x06), (0xA2, 0x92), (0x24, 0x16), 0x0C)
# Channel 0 belongs to the games' AdLib sound effects: the voices use 1-5.
BASS_CH, STAB_CH, LEAD_CH = 1, 2, 5
OPS = {1: (0x01, 0x04), 2: (0x02, 0x05), 3: (0x08, 0x0B), 4: (0x09, 0x0C), 5: (0x0A, 0x0D)}
# The drums are the chip's rhythm mode (register 0xBD), which takes channels 6-8.
DRUM_OPS = [   # operator, mult, level, attack/decay, sustain/release
    (0x10, 0x00, 0x08, 0xF8, 0x66),   # bass drum, modulator
    (0x13, 0x00, 0x00, 0xF6, 0x58),   # bass drum, carrier
    (0x11, 0x01, 0x12, 0xF8, 0x78),   # hi-hat
    (0x14, 0x01, 0x04, 0xF7, 0xB7),   # snare
    (0x12, 0x02, 0x0A, 0xF6, 0x57),   # tom
    (0x15, 0x01, 0x16, 0xF5, 0x44),   # cymbal
]
DRUM_PITCH = [(6, 2, 0x1C0), (7, 3, 0x202), (8, 2, 0x157)]   # channel, block, f-number
RHYTHM = 0x20


def build():
    ev = []  # (time, order, reg, value)

    def setpitch(t, ch, m, order=2):
        block, f = opl_pitch(m)
        ev.append((t, order, 0xA0 + ch, f & 0xFF))
        ev.append((t, order + 1, 0xB0 + ch, 0x20 | block << 2 | f >> 8))

    def off(t, ch, m=None):
        if m is None:
            ev.append((t, 0, 0xB0 + ch, 0))
        else:   # let go without the pitch jumping: the release rings at the note's own pitch
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
            t = t0 + i * STEP
            if slide and prev is not None and prev != m:
                steps, span = 8, STEP
                for s in range(steps + 1):
                    setpitch(t + s * span // steps, ch, prev + (m - prev) * s / steps, order=2 if s == 0 else 4)
            else:
                setpitch(t, ch, m)
            off(t + length * STEP - GAP, ch, m)
            prev = m
        return prev

    last = None
    for bar, (section, chord, bass, lead) in enumerate(SONG):
        t0 = bar * 16 * STEP
        line(bass, BASS_CH, t0, None)
        last = line(lead, LEAD_CH, t0, last)

        # the horns ('D9E': D9, then E7 for the second half of the bar)
        for s16, length in STABS[STAB_AS.get(section, section)]:
            name = chord if chord != 'D9E' else ('D9' if s16 < 8 else 'E7')
            t = t0 + s16 * STEP
            for j, n in enumerate(CHORD[name]):
                setpitch(t, STAB_CH + j, midi(n))
                off(t + length * STEP - GAP, STAB_CH + j, midi(n))

        pattern = DRUMS[section]
        for s16 in range(16):
            hits = 0
            for drum, where in pattern.items():
                if s16 in where:
                    hits |= drum
            if s16 == 0 and section in CRASH and SONG[bar - 1][0] not in (section, section + 'fill'):
                hits |= CYM
            if hits:
                t = t0 + s16 * STEP
                ev.append((t, 5, 0xBD, RHYTHM | hits))
                ev.append((t + STEP - GAP, 0, 0xBD, RHYTHM))    # let go, so the drum can be hit again

    end = len(SONG) * 16 * STEP
    ev.append((end, 9, 0xB0 + BASS_CH, 0))  # the loop point: the player restarts after its last event
    ev.sort()
    out = bytearray()
    for i, (t, _, reg, val) in enumerate(ev):
        delay = ev[i + 1][0] - t if i + 1 < len(ev) else 0
        out += bytes((reg, val, delay & 0xFF, delay >> 8))
    assert len(out) < 65536, 'too long for the player: %d bytes' % len(out)
    return bytes(out)


if __name__ == '__main__':
    data = build()
    if sys.argv[1:2] == ['imf'] and len(sys.argv) == 3:
        open(sys.argv[2], 'wb').write(data)
        sys.exit(0)
    if sys.argv[1:] != ['csource']:
        sys.exit(__doc__)
    print('// "The Greatest Adventure", the title tune of Commander Keen: Worlds.')
    print('// Generated by tools/kw_title_music.py - do not edit by hand.')
    print('static const uint8_t kw_titleMusic[%d] = {' % len(data))
    for i in range(0, len(data), 16):
        print('\t' + ' '.join('0x%02x,' % b for b in data[i:i + 16]))
    print('};')
