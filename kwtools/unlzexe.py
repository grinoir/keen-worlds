"""Unpack LZEXE 0.91 compressed DOS executables (the Keen 4-6 EXEs use it).

Returns the decompressed load image (code + data, no MZ header); good enough for
finding the data tables embedded in the game.

Usage: python3 tools/unlzexe.py KEEN6C.EXE OUT.BIN
"""
import struct, sys


def unlzexe(path):
    d = open(path, 'rb').read()
    h = struct.unpack('<16H', d[:32])
    hdr, cs = h[4] * 16, h[11]
    info = struct.unpack('<8H', d[hdr + cs * 16:hdr + cs * 16 + 16])
    p = hdr + (cs - info[4]) * 16
    out = bytearray()
    buf = d[p] | d[p + 1] << 8
    p += 2
    cnt = 16

    def bit():
        nonlocal buf, cnt, p
        b = buf & 1
        cnt -= 1
        if cnt == 0:
            buf = d[p] | d[p + 1] << 8
            p += 2
            cnt = 16
        else:
            buf >>= 1
        return b

    def byte():
        nonlocal p
        p += 1
        return d[p - 1]

    while True:
        if bit():
            out.append(byte())
            continue
        if not bit():
            ln = (bit() << 1 | bit()) + 2
            span = byte() - 256
        else:
            lo, hi = byte(), byte()
            span = (lo | ((hi & ~7) << 5) | 0xE000) - 0x10000
            ln = (hi & 7) + 2
            if ln == 2:
                ln = byte()
                if ln == 0:
                    break
                if ln == 1:
                    continue
                ln += 1
        for _ in range(ln):
            out.append(out[span])
    return bytes(out)


if __name__ == '__main__':
    open(sys.argv[2], 'wb').write(unlzexe(sys.argv[1]))
