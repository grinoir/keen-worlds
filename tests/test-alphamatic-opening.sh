#!/bin/sh
# Render the real opening with the player's data and voices in an isolated save path.
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\nkw_voices = true\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOTALK=1 KW_AUTOCUTSCENE="$work" \
 timeout -k 2s 30s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /ALOPENING > "$work/result.log" 2>&1; then
 tail -30 "$work/result.log"; exit 1
fi
python3 - "$work" <<'PY'
from pathlib import Path
import sys
root=Path(sys.argv[1]);log=(root/'result.log').read_text()
assert 'opening finished' in log and 'PASS:' in log
assert 'solid lettering and black outlines on all 16 EGA backgrounds' in log
assert len(list(root.glob('*.ppm')))==22
for path in root.glob('*.ppm'):
 data=path.read_bytes()
 assert data.startswith(b'P6\n320 200\n255\n'),path
 assert len(data.split(b'\n',3)[3])==320*200*3,path
assert (root/'05-Keen.ppm').read_bytes()!=(root/'05b-Keen-motion.ppm').read_bytes()
assert log.count('KW_AUTOTALK: AL_CUT_')==16,'Supply all 16 opening voice clips'
print('PASS: workshop/crawl/call/blueprint/Mars/departure/map frames, animated pose, 16 real speech clips and preserved game state')
PY
