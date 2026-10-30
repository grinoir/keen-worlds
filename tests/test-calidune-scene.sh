#!/bin/sh
# Render Calidune's arrival and ending scenes with the player's data and voices in an isolated save path.
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\nkw_voices = true\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOTALK=1 KW_AUTOCUTSCENE="$work" \
 timeout -k 2s 60s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /CDSCENE > "$work/result.log" 2>&1; then
 tail -30 "$work/result.log"; exit 1
fi
python3 - "$work" <<'PY'
from pathlib import Path
import sys
root=Path(sys.argv[1]);log=(root/'result.log').read_text(errors='replace')
assert log.count('Calidune: scene finished')==2 and 'PASS:' in log
shots=sorted(root.glob('*.ppm'))
assert len(shots)==14,len(shots)
for path in shots:
 data=path.read_bytes()
 assert data.startswith(b'P6\n320 200\n255\n'),path
 assert len(data.split(b'\n',3)[3])==320*200*3,path
assert (root/'4-sanda.ppm').read_bytes()!=(root/'4-sandb.ppm').read_bytes()  # Keen materialises
assert log.count('KW_AUTOTALK: CD_CUT_')==5,'Supply all 5 Calidune scene clips'
print('PASS: Calidune orbit, beam, sand and ending frames, 5 real speech clips and preserved game state')
PY
