"""Write the ending tune of Commander Keen: Worlds as AdLib music: the calm
one, for when every world is saved (under the last story and with the
picture of Keen on the beach).

Slow, no drums: held chords, a harp-like figure going up and down them, a
long bass note, and (in two of the three) a tune on top with room in it.
Three candidates, to be picked by ear:

  1  "Hammock"    C major, 66 bpm: C - Am - F - G, with a tune
  2  "Low tide"   D major, 60 bpm: two chords rocking, a few long notes
  3  "Starlight"  A minor, 72 bpm: the figure and the bass alone

The output is an IMF stream, as tools/kw_title_music.py writes (the format
of the games' own music), compiled into the engine.

Usage: python3 tools/kw_ending_music.py N csource > omnispeak/src/kw_ending_music.h
       python3 tools/kw_ending_music.py N imf out.imf    (for tools/kw_imf2wav.c)
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kw_title_music import midi, opl_pitch

# (modulator, carrier): tremolo/sustain/mult, level, attack/decay, sustain/release; feedback
PAD_VOICE = ((0x21, 0x21), (0x20, 0x0C), (0x42, 0x42), (0x23, 0x24), 0x02)    # slow to rise, slow to go
HARP_VOICE = ((0x01, 0x01), (0x1C, 0x0A), (0xF3, 0xF3), (0x44, 0x35), 0x04)   # struck, rings a while
TUNE_VOICE = ((0x61, 0x21), (0x1C, 0x08), (0x72, 0x72), (0x24, 0x15), 0x08)   # a soft flute with vibrato
BASS_VOICE = ((0x01, 0x01), (0x14, 0x08), (0xA3, 0xA3), (0x35, 0x36), 0x06)
# Channel 0 belongs to the games' AdLib sound effects.
BASS_CH, PAD_CH, HARP_CH, TUNE_CH = 1, 2, 5, 6
OPS = {1: (0x01, 0x04), 2: (0x02, 0x05), 3: (0x08, 0x0B), 4: (0x09, 0x0C), 5: (0x0A, 0x0D), 6: (0x10, 0x13)}

CHORDS = {'C': ('C2', ('E3', 'G3', 'C4')), 'Am': ('A1', ('E3', 'A3', 'C4')), 'F': ('F1', ('F3', 'A3', 'C4')),
          'G': ('G1', ('D3', 'G3', 'B3')), 'Dmaj7': ('D2', ('F#3', 'A3', 'C#4')), 'Gmaj7': ('G1', ('G3', 'B3', 'F#4')),
          'Em': ('E2', ('E3', 'G3', 'B3')), 'A7': ('A1', ('E3', 'G3', 'C#4')), 'Dm': ('D2', ('F3', 'A3', 'D4'))}
# The figure: which notes of the chord (0-2, an octave up with +) on each 8th of the bar.
UPDOWN = ['0', '1', '2', '0+', '2', '1', '0', '1']
ROCK = ['0', '2', '1', '2', '0+', '2', '1', '2']

# candidate: (ticks in an 8th, the figure, [(chord, the tune's bar: eight 8ths; '-' holds, '.' rests)])
SONGS = {
    1: (254, UPDOWN, [
        ('C', '. . . .  . . . .'), ('Am', '. . . .  . . . .'),
        ('F', 'E5 - - -  D5 - C5 -'), ('G', 'D5 - - -  - - . .'),
        ('C', 'E5 - - G5  - - E5 -'), ('Am', 'C5 - - -  - - . .'),
        ('F', 'A4 - C5 -  F5 - E5 -'), ('G', 'D5 - - -  - - - .'),
        ('C', 'G5 - - E5  - - C5 -'), ('Am', 'E5 - - -  - - . .'),
        ('F', 'F5 - E5 -  D5 - C5 -'), ('G', 'B4 - - -  D5 - - -'),
        ('C', 'C5 - - -  - - - -'), ('Am', '. . . .  . . . .'),
        ('F', '. . . .  . . . .'), ('G', '. . . .  . . . .'),
    ]),
    2: (280, ROCK, [
        ('Dmaj7', '. . . .  . . . .'), ('Gmaj7', '. . . .  . . . .'),
        ('Dmaj7', 'F#5 - - -  - - - -'), ('Gmaj7', 'D5 - - -  - - . .'),
        ('Dmaj7', 'A5 - - -  - - F#5 -'), ('Gmaj7', 'B5 - - -  - - - .'),
        ('Em', 'G5 - - -  E5 - - -'), ('A7', 'C#5 - - -  - - . .'),
        ('Dmaj7', 'F#5 - - -  - - A5 -'), ('Gmaj7', 'B5 - - -  D6 - - -'),
        ('Em', 'B5 - - -  G5 - - -'), ('A7', 'E5 - - -  - - . .'),
        ('Dmaj7', 'D5 - - -  - - - -'), ('Gmaj7', '. . . .  . . . .'),
    ]),
    3: (233, UPDOWN, [
        ('Am', ''), ('F', ''), ('C', ''), ('G', ''),
        ('Am', ''), ('Dm', ''), ('F', ''), ('G', ''),
        ('Am', ''), ('F', ''), ('C', ''), ('Em', ''),
        ('F', ''), ('Dm', ''), ('G', ''), ('G', ''),
    ]),
}
GAP = 12


def build(which):
    step, figure, song = SONGS[which]
    ev = []

    def setpitch(t, ch, m):
        block, f = opl_pitch(m)
        ev.append((t, 2, 0xA0 + ch, f & 0xFF))
        ev.append((t, 3, 0xB0 + ch, 0x20 | block << 2 | f >> 8))

    def off(t, ch, m):
        block, f = opl_pitch(m)
        ev.append((t, 0, 0xB0 + ch, block << 2 | f >> 8))

    voices = [(BASS_CH, BASS_VOICE), (PAD_CH, PAD_VOICE), (PAD_CH + 1, PAD_VOICE), (PAD_CH + 2, PAD_VOICE),
              (HARP_CH, HARP_VOICE), (TUNE_CH, TUNE_VOICE)]
    ev.append((0, -3, 0xBD, 0))     # no rhythm mode: channel 6 is a voice like the others
    for ch, (mult, level, ad, sr, fb) in voices:
        for base, vals in ((0x20, mult), (0x40, level), (0x60, ad), (0x80, sr)):
            for op, val in zip(OPS[ch], vals):
                ev.append((0, -2, base + op, val))
        ev.append((0, -2, 0xC0 + ch, fb))
        ev.append((0, -1, 0xB0 + ch, 0))

    for bar, (chord, tune) in enumerate(song):
        t0 = bar * 8 * step
        bass, notes = CHORDS[chord]
        setpitch(t0, BASS_CH, midi(bass))
        off(t0 + 8 * step - GAP, BASS_CH, midi(bass))
        for j, n in enumerate(notes):
            setpitch(t0, PAD_CH + j, midi(n))
            off(t0 + 8 * step - GAP * 3, PAD_CH + j, midi(n))
        for i, f in enumerate(figure):
            m = midi(notes[int(f[0])]) + 12 + (12 if f.endswith('+') else 0)
            setpitch(t0 + i * step, HARP_CH, m)
            off(t0 + (i + 1) * step - GAP, HARP_CH, m)
        cells = tune.split()
        assert len(cells) in (0, 8), tune
        for i, n in enumerate(cells):
            if n in '-.':
                continue
            length = 1
            while i + length < 8 and cells[i + length] == '-':
                length += 1
            setpitch(t0 + i * step, TUNE_CH, midi(n))
            off(t0 + (i + length) * step - GAP, TUNE_CH, midi(n))

    end = len(song) * 8 * step
    ev.append((end, 9, 0xB0 + BASS_CH, 0))  # the loop point: the player restarts after its last event
    ev.sort()
    out = bytearray()
    for i, (t, _, reg, val) in enumerate(ev):
        delay = ev[i + 1][0] - t if i + 1 < len(ev) else 0
        out += bytes((reg, val, delay & 0xFF, delay >> 8))
    assert len(out) < 65536, 'too long for the player: %d bytes' % len(out)
    return bytes(out), end / 560.0


if __name__ == '__main__':
    if len(sys.argv) < 3 or sys.argv[1] not in ('1', '2', '3'):
        sys.exit(__doc__)
    data, seconds = build(int(sys.argv[1]))
    if sys.argv[2] == 'imf' and len(sys.argv) == 4:
        open(sys.argv[3], 'wb').write(data)
        print('%.0f seconds' % seconds, file=sys.stderr)
        sys.exit(0)
    if sys.argv[2] != 'csource':
        sys.exit(__doc__)
    print('// The ending tune of Commander Keen: Worlds (candidate %s).' % sys.argv[1])
    print('// Generated by tools/kw_ending_music.py - do not edit by hand.')
    print('static const uint8_t kw_endingMusic[%d] = {' % len(data))
    for i in range(0, len(data), 16):
        print('\t' + ' '.join('0x%02x,' % b for b in data[i:i + 16]))
    print('};')
