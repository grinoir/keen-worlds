"""Read-only compatibility inventory for The Alphamatic 3.0 (not a port).

Usage: python3 kwtools/kw_alphamatic_audit.py MOD_DIR KEEN5E.EXE KEEN5_DATA OUT_DIR

KEEN5_DATA supplies the original extracted EGADICT/GFXINFOE. No DOS code is
executed. OUT_DIR holds hashes, map/transport inventories, action changes,
code/data patches and decoded story/ending layouts for native port review.
"""
from collections import Counter
import contextlib
import hashlib
import io
import json
from pathlib import Path
import re
import shutil
import struct
import sys
import tempfile

import keenmap
from kw_accumulators import cstring
from kw_patconv import Game, parse_pat, strip_comments, cmd_actions, cmd_code, cmd_data


def audit(source, exe, reference, output):
    source, reference, output = map(Path, (source, reference, output))
    files = {p.name.lower(): p for p in source.iterdir() if p.is_file()}
    patch = files['patch5.pat']
    active = re.split(r'(?im)^\s*%end\b', strip_comments(patch.read_text(encoding='latin1')), maxsplit=1)[0]
    g = Game(str(exe), str(Path(__file__).resolve().parents[1] / 'data/keen5'))
    pats = parse_pat(patch)
    image = bytearray(g.patched(pats))
    embedded = []
    # This archive's four patchfiles do not overlap its byte patches.
    for m in re.finditer(r'%patchfile\s+\$([\dA-Fa-f]+)\s+"?([\w.]+)"?', active):
        at, name = int(m[1], 16), m[2]
        raw = files[name.lower()].read_bytes()
        assert not any(at < off+len(data) and off < at+len(raw) for off, data in pats)
        image[at:at+len(raw)] = raw
        embedded.append({'offset': hex(at), 'file': name, 'bytes': len(raw)})
    output.mkdir(parents=True, exist_ok=True)
    for label, command in [('actions', cmd_actions), ('code', cmd_code), ('data', cmd_data)]:
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            command(g, pats)
        (output / (label+'.txt')).write_text(buf.getvalue())
    dictionary = (reference/'EGADICT.CK5').read_bytes()
    dict_at = g.img.find(dictionary)
    assert dict_at >= 0 and image[dict_at:dict_at+len(dictionary)] == dictionary
    nodes = struct.unpack('<510H', dictionary[:1020])
    graph = files['egagraph.ck5'].read_bytes()
    head = files['egahead.ck5'].read_bytes()
    starts = [int.from_bytes(head[i:i+3], 'little') for i in range(0, len(head), 3)]

    def text_chunk(chunk):
        start = starts[chunk]
        end = next(x for x in starts[chunk+1:] if x != 0xFFFFFF)
        size = struct.unpack_from('<I', graph, start)[0]
        assert size < 1000000
        src = graph[start+4:end]
        out, node = bytearray(), 254
        for byte in src:
            for bit in range(8):
                value = nodes[2*node + ((byte >> bit) & 1)]
                if value < 256:
                    out.append(value)
                    node = 254
                    if len(out) == size:
                        return out.decode('latin1')
                else:
                    node = value-256
        raise ValueError('Truncated text chunk '+str(chunk))

    layouts = {}
    for name, chunk in [('help',4914), ('controls',4915), ('story',4916),
                        ('credits',4917), ('ending',4918), ('secret-ending',4919)]:
        text = text_chunk(chunk)
        (output/(name+'.txt')).write_text(text, encoding='latin1')
        layouts[name] = {'chunk': chunk, 'bytes': len(text),
                         'pages': len(re.findall(r'\^P', text, re.I))}
    gfx = struct.unpack('<23H', (reference/'GFXINFOE.CK5').read_bytes())
    back_count, fore_count = gfx[2:4]
    tiles = files['mytiles.tli'].read_bytes()
    assert len(tiles) == 2*back_count+7*fore_count
    misc = tiles[2*back_count+5*fore_count:2*back_count+6*fore_count]
    maps = []
    with tempfile.TemporaryDirectory(prefix='alphamatic-audit-') as tmp:
        tmp = Path(tmp)
        for name in ('MAPHEAD.CK5','GAMEMAPS.CK5'):
            shutil.copy2(files[name.lower()], tmp/name)
        _, offsets, _ = keenmap.load(tmp, 5)
        for level, offset in enumerate(offsets):
            if offset in (0, 0xFFFFFFFF):
                continue
            internal, width, height, planes = keenmap.level(tmp, 5, level)
            assert all(len(p) == width*height for p in planes)
            assert max(planes[0]) < back_count and max(planes[1]) < fore_count
            ptr = struct.unpack_from('<I', image, 0x31AB4+level*4)[0]
            name = cstring(image, (ptr >> 16)*16+(ptr & 65535)) if level < 50 else internal
            transports, markers = [], []
            for i, (tile, info) in enumerate(zip(planes[1], planes[2])):
                x, y = i % width, i // width
                if level == 0 and (misc[tile] & 0x7F) in (20,33,34):
                    dest = [info >> 8, info & 255]
                    transports.append({'at':[x,y], 'kind': misc[tile]&0x7F,
                                       'destination':dest,
                                       'in_bounds':dest[0]<width and dest[1]<height})
                if level == 0 and info >> 8 in (0xC0,0xD0,0xF0):
                    markers.append({'at':[x,y], 'kind':hex(info>>8), 'level':info&255})
            maps.append({'level':level, 'name':name, 'header_name':internal,
                         'campaign_slot':level <= 25,
                         'size':[width,height], 'plane_words':[len(p) for p in planes],
                         'info_counts':dict(sorted(Counter(planes[2]).items())),
                         'transports':transports, 'completion_markers':markers})
    audio = files['audio.ck5'].read_bytes()
    ah = files['audiohed.ck5'].read_bytes()
    audio_offsets = struct.unpack('<%dI' % (len(ah)//4), ah)
    present = 0
    for start, end in zip(audio_offsets, audio_offsets[1:]):
        assert 0 <= start <= end <= len(audio)
        if start != end:
            size = struct.unpack_from('<I', audio, start)[0]
            assert size == end-start-4
            present += 1
    result = {
        'status':'static audit only; not installed or playable',
        'files':{p.name:{'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()}
                 for p in sorted(files.values())},
        'exe_sha256':hashlib.sha256(Path(exe).read_bytes()).hexdigest(),
        'active_byte_patch_directives':len(pats), 'patchfiles':embedded,
        'ignored_after_first_end':True, 'graphics_dictionary_offset':hex(dict_at),
        'graphics_header_entries':len(starts), 'text_layouts':layouts,
        'audio':{'chunks':len(audio_offsets)-1, 'nonempty':present,
                 'music_start':173,'songs':33,'length_prefixed_uncompressed':True},
        'level_music':list(struct.unpack_from('<26H',image,0x3067A)), 'maps':maps,
    }
    (output/'inventory.json').write_text(json.dumps(result,indent=2)+'\n')
    print(f'{len(maps)} maps decoded; {len(pats)} active byte patches; {present} audio chunks verified')
    for row in maps:
        print(row['level'], row['name'], row['size'])
    print('World-map transports:',json.dumps(maps[0]['transports']))


if __name__ == '__main__':
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    audit(*sys.argv[1:])
