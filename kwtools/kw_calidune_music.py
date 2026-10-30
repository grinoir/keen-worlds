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
  10 raga       "Desert Raga": a tanpura drone, a sitar alaap, then the
                 tabla and a reedy tune, D Bhairav.
  11 filmi       "Calidune Filmi": a film-song groove on tabla, the tune
                 twice, a sitar break.
  12 bhangra     "Sand Bhangra": fast, a dhol roll and the bhangra beat.

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
    if candidate in INDIA:
        return build_india(candidate)
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


# ---- 10-12: Calidune in filmi style (Indian film song)
#
# Original music in the style the user asked for: a tanpura drone, a sitar,
# a shehnai-like reed for the tune and tabla/dhol on the chip's drums, in
# D Bhairav (D Eb F# G A Bb C#). Nothing is taken from any existing song.
#
# No clicks: a channel is never struck while it still rings. Just before a
# new note its envelope is cut to silence in a few milliseconds (a damp),
# the plucked parts take turns on two channels, and a tune moves its pitch
# without striking again.
INDIA = ('raga', 'filmi', 'bhangra')
RATE = 560                      # IMF ticks per second
DAMP = 6                        # ticks to silence a voice before striking it
# (modulator, carrier): 0x20 vib/sustain/mult, 0x40 level, 0x60 attack/decay, 0x80 sustain/release; then 0xC0
TANPURA = ((0x01, 0x01), (0x1C, 0x06), (0xF2, 0xF2), (0x33, 0x03), 0x0A)   # a buzzing drone string
SITAR = ((0x03, 0x01), (0x1A, 0x05), (0xF3, 0xF4), (0x24, 0x05), 0x0A)     # bright twang that fades
BASS = ((0x01, 0x01), (0x1C, 0x02), (0xF4, 0xF4), (0x05, 0x06), 0x04)      # a round pluck
SHEHNAI = ((0x21, 0x61), (0x16, 0x00), (0x86, 0x76), (0x14, 0x15), 0x08)   # a reedy, singing tune
TANPURA_CH, SITAR_CH, BASS_CH_I, TUNE_CH = (1,), (2, 3), (4,), (5,)
INDIA_DRUM_OPS = [   # operator, mult (no sustain: they ring and fade), level, attack/decay, sustain/release
    (0x10, 0x00, 0x16, 0xF6, 0x06),   # bass drum, modulator: the low "ge"
    (0x13, 0x00, 0x0C, 0xF6, 0x06),   # bass drum, carrier
    (0x11, 0x01, 0x3F, 0xF8, 0x78),   # hi-hat: silent
    (0x14, 0x01, 0x3F, 0xF8, 0x78),   # snare: silent (its noise hisses)
    (0x12, 0x01, 0x0E, 0xF7, 0x07),   # tom: the ringing "na" of a tabla
    (0x15, 0x01, 0x3F, 0xF3, 0x23),   # cymbal: silent
]
GE_PITCH = (6, 2, 0x1A0)
# Tabla/dhol: 16 sixteenths a bar. B ge (low drum), T na (high), t tun (lower), X ge and na.
TABLA = {
    'none':    '................',
    'theka':   'X...T.t.T...X.T.',
    'kehrwa':  'X..BT.tTX.B.T.TT',
    'fill':    'X.T.T.TTX.TTX.XX',
    'bhangra': 'B..TB.T.B..TB.TT',
    'roll':    'B.B.BTBTBTBTXXXX',
}

# The tune (16ths). '-' holds, '.' lets go; a note after a note glides on
# without being struck again, as a reed does.
HOOK = ['A4 - - - A#4 A4 G4 - F#4 - - - G4 - A4 -',
        'A#4 - A4 - G4 - F#4 - D#4 - - - D4 - - -',
        'D5 - - - C#5 D5 A#4 - A4 - - - A#4 A4 G4 -',
        'A4 - - - - - - - . . . . . . . .',
        'F#4 - G4 - A4 - A#4 - A4 - G4 - F#4 - G4 -',
        'A4 - - - A#4 - C#5 - D5 - - - - - - -',
        'D#5 - D5 - C#5 - D5 - A#4 - A4 - G4 - F#4 -',
        'G4 - F#4 - D#4 - - - D4 - - - . . . .']
# The sitar: every note is plucked; '^' moves to a note on the same pluck.
ALAAP = ['D4 - - - - - - - D#4 ^D4 - - - - - -',
         'F#4 - - - G4 - - - A4 - - - - - - -',
         'A#4 ^A4 - - G4 - F#4 - G4 - - - D#4 ^D4 - -',
         'D4 - - - - - - - . . . . . . . .']
RIFF = ['D4 . A4 . D5 . A4 . D4 . A4 . C#5 . D5 .',
        'D4 . A4 . D5 . A#4 . A4 . G4 . F#4 . G4 .']
TAAN = ['D5 C#5 A#4 A4 G4 F#4 D#4 D4 D#4 F#4 G4 A4 A#4 C#5 D5 .',
        'D5 A4 D5 A4 C#5 A4 A#4 A4 G4 F#4 G4 A4 D4 . . .']
BASSLINE = ['D2 . . D2 . . A1 . D2 . . D2 . C#2 D2 .',
            'D#2 . . D#2 . . A#1 . D2 . . D2 . A1 D2 .']
EMPTY = '. . . . . . . . . . . . . . . .'
TANPURA_BAR = 'A2 - - - D3 - - - D3 - - - D2 - - -'


def india_song(candidate):
    """(seconds per 16th, [(tanpura, sitar, bass, tune, tabla)])"""
    bars = []
    add = lambda n, **k: bars.extend([dict(dict(tanpura=TANPURA_BAR, sitar=EMPTY, bass=EMPTY, tune=EMPTY,
                                                    tabla='none'), **k)] * n)
    if candidate == 'raga':     # slow: the drone, the sitar's alaap, then the tabla and the tune
        add(2)
        for a in ALAAP:
            add(1, sitar=a)
        for i, h in enumerate(HOOK):
            add(1, tune=h, sitar=RIFF[i % 2] if i >= 4 else EMPTY, tabla='theka' if i < 7 else 'fill')
        for t in TAAN:
            add(1, sitar=t, tabla='theka')
        add(1, sitar='D4 - - - - - - - . . . . . . . .', tabla='none')
        return 0.15, bars
    if candidate == 'filmi':    # a film song: a tabla call, the tune twice, a sitar break
        add(1, tabla='fill')
        for rep in range(2):
            for i, h in enumerate(HOOK):
                add(1, tune=h, sitar=RIFF[i % 2], bass=BASSLINE[i % 2], tabla='kehrwa' if i != 7 else 'fill')
            if rep == 0:
                for i, t in enumerate(TAAN * 2):
                    add(1, sitar=t, bass=BASSLINE[i % 2], tabla='kehrwa')
        add(1, sitar='D4 - - - - - - - . . . . . . . .', bass='D2 . . . . . . . . . . . . . . .', tabla='none')
        return 0.142, bars
    # bhangra: fast; a dhol roll, the tune over the bhangra beat, a sitar break
    add(1, tabla='roll', tanpura=EMPTY)
    for rep in range(2):
        for i, h in enumerate(HOOK):
            add(1, tune=h, sitar=RIFF[i % 2], bass=BASSLINE[i % 2], tabla='bhangra' if i != 7 else 'roll')
        if rep == 0:
            for i, r in enumerate(RIFF * 2):
                add(1, sitar=r if i < 2 else TAAN[i % 2], bass=BASSLINE[i % 2], tabla='bhangra')
    add(1, tune='D5 - - - - - - - . . . . . . . .', bass='D2 . . . . . . . . . . . . . . .', tabla='roll')
    return 0.12, bars


def build_india(candidate):
    step, bars = india_song(candidate)
    tick = lambda s: int(round(s * RATE))
    ev = []
    voice_of = {}

    def setv(t, ch, voice, damp=False):
        mult, level, ad, sr, fb = voice
        if t == 0:
            for base, vals in ((0x20, mult), (0x40, level), (0x60, ad)):
                for op, val in zip(OPS[ch], vals):
                    ev.append((0, -2, base + op, val))
            ev.append((0, -2, 0xC0 + ch, fb))
        for op, val in zip(OPS[ch], sr):
            ev.append((t, 1 if damp else 2, 0x80 + op, (val | 0x0F) if damp else val))

    def strike(t, ch, note):
        block, f = opl_pitch(midi(note))
        if (ch, 'note') in voice_of:     # damp whatever still rings (at its own pitch), then strike
            setv(t - DAMP, ch, voice_of[ch], damp=True)
            release(t - DAMP, ch, voice_of[(ch, 'note')])
            setv(t, ch, voice_of[ch])
        voice_of[(ch, 'note')] = note
        ev.append((t, 3, 0xA0 + ch, f & 0xFF))
        ev.append((t, 4, 0xB0 + ch, 0x20 | block << 2 | f >> 8))
        return note

    def glide(t, ch, note):
        voice_of[(ch, 'note')] = note
        block, f = opl_pitch(midi(note))
        ev.append((t, 3, 0xA0 + ch, f & 0xFF))
        ev.append((t, 4, 0xB0 + ch, 0x20 | block << 2 | f >> 8))

    def release(t, ch, note):
        block, f = opl_pitch(midi(note))
        ev.append((t, 0, 0xB0 + ch, block << 2 | f >> 8))

    def part(key, chans, voice, legato):
        for ch in chans:
            voice_of[ch] = voice
            setv(0, ch, voice)
            ev.append((0, -2, 0xB0 + ch, 0))
        turn, sounding = 0, None    # (channel, note)
        for b, bar in enumerate(bars):
            for i, tok in enumerate(bar[key].split()):
                t = tick((b * 16 + i) * step) + DAMP + 1
                if tok == '-':
                    continue
                if tok == '.':
                    if sounding and legato:
                        release(t, *sounding)
                    sounding = None if legato else sounding
                    continue
                if tok.startswith('^'):
                    if sounding:
                        glide(t, sounding[0], tok[1:])
                        sounding = (sounding[0], tok[1:])
                    continue
                if legato and sounding:
                    glide(t, sounding[0], tok)
                    sounding = (sounding[0], tok)
                    continue
                ch = chans[turn % len(chans)]
                turn += 1
                sounding = (ch, strike(t, ch, tok))
        if sounding and legato:
            release(tick(len(bars) * 16 * step) + DAMP, *sounding)

    part('tanpura', TANPURA_CH, TANPURA, False)
    part('sitar', SITAR_CH, SITAR, False)
    part('bass', BASS_CH_I, BASS, False)
    part('tune', TUNE_CH, SHEHNAI, True)

    for op, mult, level, ad, sr in INDIA_DRUM_OPS:
        for base, v in ((0x20, mult), (0x40, level), (0x60, ad), (0x80, sr)):
            ev.append((0, -2, base + op, v))
    for ch in (6, 7, 8):
        ev.append((0, -2, 0xC0 + ch, 0x00))
    ch, block, f = GE_PITCH
    ev.append((0, -2, 0xA0 + ch, f & 0xFF))
    ev.append((0, -2, 0xB0 + ch, block << 2 | f >> 8))
    mode = 0x40 | RHYTHM
    ev.append((0, -1, 0xBD, mode))
    drum_ops = {BD: ((0x10, 0x06), (0x13, 0x06)), TOM: ((0x12, 0x07),)}
    for b, bar in enumerate(bars):
        for i, c in enumerate(TABLA[bar['tabla']]):
            hits = {'B': BD, 'T': TOM, 't': TOM, 'X': BD | TOM}.get(c, 0)
            if not hits:
                continue
            t = tick((b * 16 + i) * step) + DAMP + 1
            for drum, ops in drum_ops.items():
                if hits & drum:
                    for op, sr in ops:
                        ev.append((t - DAMP, 1, 0x80 + op, sr | 0x0F))
                        ev.append((t, 2, 0x80 + op, sr))
            if hits & TOM:
                block, f = opl_pitch(midi('D4' if c in 'TX' else 'A3'))
                ev.append((t - 2, 2, 0xA8, f & 0xFF))
                ev.append((t - 2, 2, 0xB8, block << 2 | f >> 8))
            ev.append((t, 5, 0xBD, mode | hits))
            ev.append((t + tick(step) // 2, 0, 0xBD, mode))
    end = tick(len(bars) * 16 * step + 2.5)
    ev.append((end, 9, 0xBD, mode))
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
