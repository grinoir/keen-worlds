"""Build Omnispeak support files for our (pre-v1.4, probably v1.0) Keen 6 EXE.

Omnispeak ships header tables only for Keen 6 v1.4/v1.5. Our copy has the same
chunk layout but a different Huffman dictionary, graphics header, map header
(levels 9+ differ) and 5 tile property bytes. Those tables live inside the EXE, so we pull them out of it and
reuse everything else from Omnispeak's keen6e14 data.

Usage: python3 tools/k6tables.py keen6/KEEN6C.EXE omnispeak/data/keen6e14 OUTDIR
"""
import os, shutil, sys

sys.path.insert(0, os.path.dirname(__file__))
from unlzexe import unlzexe

NUM_CHUNKS = 5561          # same graphics layout as v1.4
GRAPH_SIZE = 457309        # size of our EGAGRAPH.CK6; last EGAHEAD entry
MAPHEAD_LEN = 402          # RLEW tag + 100 level offsets
TILEINFO_LEN = 23904

exe, ref, out = sys.argv[1:4]
d = unlzexe(exe)
ref_audiodct = open(os.path.join(ref, 'AUDIODCT.CK6'), 'rb').read()
ref_maphead = open(os.path.join(ref, 'MAPHEAD.CK6'), 'rb').read()

# Graphics header: 3-byte offsets, the last one equals the EGAGRAPH size.
end = d.find(GRAPH_SIZE.to_bytes(3, 'little'))
egahead = d[end - (NUM_CHUNKS - 1) * 3:end + 3]
# Graphics Huffman dictionary is linked right after the (unchanged) audio one.
a = d.find(ref_audiodct)
egadict = d[a + 1024:a + 2048]
# Map header: levels 0-8 sit at the same offsets as v1.4, later ones differ.
m = d.find(ref_maphead[:2 + 9 * 4])
maphead = d[m:m + MAPHEAD_LEN]
# Tile properties follow the map header.
tileinfo = d[m + MAPHEAD_LEN:m + MAPHEAD_LEN + TILEINFO_LEN]
assert end > 0 and a > 0 and m > 0, 'EXE does not look like our Keen 6 version'

os.makedirs(out, exist_ok=True)
for f in os.listdir(ref):
    shutil.copy(os.path.join(ref, f), out)
for name, data in [('EGAHEAD.CK6', egahead), ('EGADICT.CK6', egadict),
                   ('MAPHEAD.CK6', maphead), ('TILEINFO.CK6', tileinfo)]:
    open(os.path.join(out, name), 'wb').write(data)
print('wrote', out)
