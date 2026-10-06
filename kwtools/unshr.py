"""Extract Apogee .SHR installer archives (PKWARE DCL implode, port of zlib's blast.c)."""
import sys, os, struct

LITLEN = [11,124,8,7,28,7,188,13,76,4,10,8,12,10,12,10,8,23,8,9,7,6,7,8,7,6,55,8,23,24,12,11,7,9,11,12,6,7,22,5,7,24,6,11,9,6,7,22,7,11,38,7,9,8,25,11,8,11,9,12,8,12,5,38,5,38,5,11,7,5,6,21,6,10,53,8,7,24,10,27,44,253,253,253,252,252,252,13,12,45,12,45,12,61,12,45,44,173]
LENLEN = [2,35,36,53,38,23]
DISTLEN = [2,20,53,230,247,151,248]
BASE = [3,2,4,5,6,7,8,9,10,12,16,24,40,72,136,264]
EXTRA = [0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8]

def construct(rep):
    lengths = []
    for b in rep:
        lengths += [b & 15] * ((b >> 4) + 1)
    count = [0] * 14
    for l in lengths: count[l] += 1
    offs = [0] * 14
    for l in range(1, 13): offs[l + 1] = offs[l] + count[l]
    sym = [0] * len(lengths)
    for s, l in enumerate(lengths):
        if l: sym[offs[l]] = s; offs[l] += 1
    return count, sym

class Bits:
    def __init__(s, data): s.d = data; s.p = 0; s.buf = 0; s.cnt = 0
    def bit(s):
        if s.cnt == 0:
            s.buf = s.d[s.p]; s.p += 1; s.cnt = 8
        b = s.buf & 1; s.buf >>= 1; s.cnt -= 1
        return b
    def bits(s, n):
        v = 0
        for i in range(n): v |= s.bit() << i
        return v
    def decode(s, h):
        count, sym = h
        code = first = index = 0
        for l in range(1, 14):
            code |= s.bit() ^ 1
            c = count[l]
            if code < first + c: return sym[index + code - first]
            index += c; first += c; first <<= 1; code <<= 1
        raise ValueError('bad code')

LIT, LEN, DIST = construct(LITLEN), construct(LENLEN), construct(DISTLEN)

def blast(data):
    b = Bits(data); lit = b.bits(8); dic = b.bits(8)
    if lit > 1 or not 4 <= dic <= 6: raise ValueError('not DCL data')
    out = bytearray()
    while True:
        if b.bit():
            sy = b.decode(LEN); ln = BASE[sy] + b.bits(EXTRA[sy])
            if ln == 519: break
            sh = 2 if ln == 2 else dic
            dist = (b.decode(DIST) << sh) + b.bits(sh) + 1
            for _ in range(ln): out.append(out[-dist])
        else:
            out.append(b.decode(LIT) if lit else b.bits(8))
    return bytes(out)

def entries(d, start=0x3A):
    p = start
    while p + 0xA8 <= len(d):
        name = d[p:p+128].split(b'\0')[0].decode('ascii')
        size = struct.unpack_from('<I', d, p + 136)[0]
        data = d[p + 0xA8:p + 0xA8 + size]
        yield name, data
        p += 0xA8 + size

if __name__ == '__main__':
    src, dst = sys.argv[1], sys.argv[2]
    os.makedirs(dst, exist_ok=True)
    for name, data in entries(open(src, 'rb').read()):
        out = blast(data)
        open(os.path.join(dst, name), 'wb').write(out)
        print(f'{name:14} {len(data):8} -> {len(out):8}')
