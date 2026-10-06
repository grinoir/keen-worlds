"""Write the Megarocket's tune as AdLib music for Keen Worlds: space funk.

An original groove in E Dorian, 16th notes at 108 bpm: a rubbery synth bass,
three-note "chicken scratch" stabs on the off-beats (Em7 - A7 - B7), the
OPL2's own drum kit (bass drum, snare, hi-hat), and from the fifth bar a
wobbly lead synth. Sixteen bars, then round again. The output is an IMF
stream (OPL2 register, value, 16-bit delay in 1/560 s ticks), the format of
the games' own music, compiled into the engine so nobody's AUDIO file is
touched.

Usage: python3 tools/kw_ship_music.py csource > omnispeak/src/kw_ship_music.h
"""
import sys

STEP = 78     # ticks in a 16th: 108 bpm
GAP = 10      # silence before a note is struck again

FNUM = {'C': 0x157, 'C#': 0x16B, 'D': 0x181, 'D#': 0x198, 'E': 0x1B0, 'F': 0x1CA,
        'F#': 0x1E5, 'G': 0x202, 'G#': 0x220, 'A': 0x241, 'A#': 0x263, 'B': 0x287}

# One bar per line, sixteen 16ths; '-' holds the note, '.' is a rest.
BASS = """
E2 . E3 .   . E2 . G2   A2 . A#2 B2  . D3 E3 .
E2 . E3 .   . E2 . G2   A2 . A#2 B2  . D3 B2 A2
A2 . A3 .   . A2 . C#3  D3 . D#3 E3  . G3 A3 .
B2 . B3 .   . B2 A2 .   G2 . F#2 .   E2 . D2 D#2
"""
LEAD = """
. . . .     . . . .     . . . .      . . . .
. . . .     . . . .     . . . .      . . . .
. . . .     . . . .     . . . .      . . . .
. . . .     . . . .     . . . .      . . B4 D5
B4 - . E5   - . D5 E5   . G5 - -     E5 . D5 .
B4 - . A4   - . G4 A4   . B4 - -     - . . .
C#5 - . E5  - . G5 E5   . A5 - -     G5 . E5 .
F#5 - . D#5 - . B4 -    . A4 B4 .    D5 D#5 E5 -
E5 . E5 G5  . E5 . D5   B4 - . D5    E5 - . .
G5 . G5 A5  . G5 . E5   D5 - . B4    D5 - . .
A5 - G5 E5  . C#5 E5 .  G5 - E5 C#5  . A4 . .
B4 . D#5 .  F#5 . A5 .  B5 - - -     . . . .
. . . .     . . . .     . . E5 G5    B5 - A5 G5
E5 - - -    . . . .     . . D5 E5    G5 - E5 D5
C#5 - - -   . . E5 .    A5 - G5 -    E5 - C#5 .
D#5 - F#5 - A5 - F#5 -  B5 . B5 .    A5 G5 F#5 D#5
"""
# The chord of each bar of the four-bar round, and where its stabs fall.
CHORDS = ['Em7', 'Em7', 'A7', 'B7']
CHORD = {'Em7': ('G3', 'B3', 'D4'), 'A7': ('G3', 'C#4', 'E4'), 'B7': ('A3', 'D#4', 'F#4')}
STABS = [2, 5, 7, 10, 13]          # 16ths of the bar
# The drum kit: 16ths of the bar on which each drum is hit.
KICK = [0, 3, 8, 10]
SNARE = [4, 12]
HAT = [0, 2, 4, 6, 7, 8, 10, 12, 14]
FILL = {'kick': [0, 3, 8, 10, 11], 'snare': [4, 12, 14, 15], 'hat': [0, 2, 4, 6, 8, 10]}   # every fourth bar

# OPL2 voices: (modulator, carrier) register values
#            tremolo/sustain/mult  level        attack/decay  sustain/release  feedback
BASS_VOICE = ((0x01, 0x01), (0x0C, 0x02), (0xF4, 0xF5), (0x68, 0x78), 0x0C)
STAB_VOICE = ((0x02, 0x01), (0x14, 0x0E), (0xF5, 0xF6), (0x77, 0x88), 0x08)
LEAD_VOICE = ((0x61, 0x21), (0x18, 0x08), (0xA2, 0x92), (0x24, 0x16), 0x0C)
VOICES = [BASS_VOICE, STAB_VOICE, STAB_VOICE, STAB_VOICE, LEAD_VOICE]
# Channel 0 belongs to the games' AdLib sound effects, so the voices use 1-5.
FIRST = 1
OPS = [(0x01, 0x04), (0x02, 0x05), (0x08, 0x0B), (0x09, 0x0C), (0x0A, 0x0D)]  # operator offsets of channels 1-5
# The drums are the OPL2's rhythm mode (register 0xBD), which takes channels 6-8.
DRUM_OPS = [   # operator, mult, level, attack/decay, sustain/release
    (0x10, 0x00, 0x08, 0xF8, 0x66),   # bass drum, modulator
    (0x13, 0x00, 0x00, 0xF6, 0x58),   # bass drum, carrier
    (0x11, 0x01, 0x12, 0xF8, 0x78),   # hi-hat
    (0x14, 0x01, 0x04, 0xF7, 0xB7),   # snare
    (0x12, 0x02, 0x3F, 0xF5, 0x55),   # tom (silent)
    (0x15, 0x01, 0x3F, 0xF5, 0x55),   # cymbal (silent)
]
DRUM_PITCH = [(6, 2, 0x1C0), (7, 3, 0x202), (8, 2, 0x157)]   # channel, block, f-number
RHYTHM, BD, SD, HH = 0x20, 0x10, 0x08, 0x01


def pitch(note):
    return int(note[-1]), FNUM[note[:-1]]


def build():
    ev = []  # (time, order, reg, value)

    def on(t, ch, note):
        block, f = pitch(note)
        ch += FIRST
        ev.append((t, 1, 0xA0 + ch, f & 0xFF))
        ev.append((t, 2, 0xB0 + ch, 0x20 | block << 2 | f >> 8))

    def off(t, ch):
        ev.append((t, 0, 0xB0 + FIRST + ch, 0))

    for ch, (mult, level, ad, sr, fb) in enumerate(VOICES):
        for base, vals in ((0x20, mult), (0x40, level), (0x60, ad), (0x80, sr)):
            for op, v in zip(OPS[ch], vals):
                ev.append((0, -2, base + op, v))
        ev.append((0, -2, 0xC0 + FIRST + ch, fb))
        off(0, ch)
    for op, mult, level, ad, sr in DRUM_OPS:
        for base, v in ((0x20, mult), (0x40, level), (0x60, ad), (0x80, sr)):
            ev.append((0, -2, base + op, v))
    for ch, block, f in DRUM_PITCH:
        ev.append((0, -2, 0xC0 + ch, 0x08 if ch == 6 else 0x01))
        ev.append((0, -2, 0xA0 + ch, f & 0xFF))
        ev.append((0, -2, 0xB0 + ch, block << 2 | f >> 8))
    ev.append((0, -1, 0xBD, RHYTHM))

    def line(text, ch, bars):
        notes = text.split()
        assert len(notes) % 16 == 0, len(notes)
        for rep in range(bars * 16 // len(notes)):
            for i, n in enumerate(notes):
                if n in '-.':
                    continue
                length = 1
                while i + length < len(notes) and notes[i + length] == '-':
                    length += 1
                t = (rep * len(notes) + i) * STEP
                on(t, ch, n)
                off(t + length * STEP - GAP, ch)

    bars = 16
    line(BASS, 0, bars)
    line(LEAD, 4, bars)
    for bar in range(bars):
        fill = bar % 4 == 3
        # stabs: none in the first two bars (bass and drums set the groove)
        if bar >= 2:
            for s in STABS:
                t = (bar * 16 + s) * STEP
                for k, n in enumerate(CHORD[CHORDS[bar % 4]]):
                    on(t, 1 + k, n)
                    off(t + STEP - GAP, 1 + k)
        for s in range(16):
            hits = 0
            if s in (FILL['kick'] if fill else KICK):
                hits |= BD
            if s in (FILL['snare'] if fill else SNARE):
                hits |= SD
            if s in (FILL['hat'] if fill else HAT):
                hits |= HH
            if hits:
                t = (bar * 16 + s) * STEP
                ev.append((t, 3, 0xBD, RHYTHM | hits))
                ev.append((t + STEP - GAP, 0, 0xBD, RHYTHM))    # let go, so the drum can be hit again
    end = bars * 16 * STEP
    ev.append((end, 9, 0xB0 + FIRST, 0))  # the loop point: the player restarts after its last event
    ev.sort()
    out = bytearray()
    for k, (t, _, reg, val) in enumerate(ev):
        delay = ev[k + 1][0] - t if k + 1 < len(ev) else 0
        out += bytes((reg, val, delay & 0xFF, delay >> 8))
    return bytes(out)


if __name__ == '__main__':
    data = build()
    if sys.argv[1:] != ['csource']:
        sys.exit(__doc__)
    print('// Generated by tools/kw_ship_music.py - do not edit by hand.')
    print('static const uint8_t kw_shipMusic[%d] = {' % len(data))
    for i in range(0, len(data), 16):
        print('\t' + ' '.join('0x%02x,' % b for b in data[i:i + 16]))
    print('};')
