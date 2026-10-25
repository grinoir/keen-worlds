#!/bin/sh
# Real watch scrolling, all flight selections and the animated cabin console.
set -eu
binary=$(realpath "${1:-$(dirname "$0")/../../play/omnispeak}")
game_data=$(realpath "${2:-$(dirname "$0")/../../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOWATCH="$work" KW_AUTOMENUALL="$work" \
 timeout -k 2s 30s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /MENUTEST > "$work/result.log" 2>&1; then
 tail -30 "$work/result.log"; exit 1
fi
python3 - "$work" <<'PY'
from pathlib import Path
from PIL import Image
import sys
p=Path(sys.argv[1]);log=(p/'result.log').read_text()
assert log.count('PASS:')==2 and 'runtime error:' not in log and 'ERROR: AddressSanitizer' not in log
assert len(list(p.glob('watch-[0-9][0-9].ppm')))==18
for name in ('new','load','save','configure','options','sound','music','voices',
             'keyboard','movement','buttons','paddle','dialog','options-bottom'):
 assert Image.open(p/f'watch-{name}.ppm').size==(320,200),name
watch=Image.open(p/'watch-00.ppm')
for f in p.glob('watch-*.ppm'):
 im=Image.open(f)
 for bounds in ((0,0,68,200),(246,0,320,200),(0,0,320,41),(0,157,320,200)):
  assert im.crop(bounds).tobytes()==watch.crop(bounds).tobytes(),f'Watch case overwritten at {f}'
flights=sorted(p.glob('flight-*.ppm'));assert len(flights)==16
reference=Image.open(flights[0])
for f in flights:
 im=Image.open(f);assert im.size==(320,200)
 for bounds in ((0,0,8,200),(312,0,320,200)):
  assert im.crop(bounds).tobytes()==reference.crop(bounds).tobytes(),f'Frame damaged at {f}'
assert (p/'flight-00.ppm').read_bytes()!=(p/'flight-15.ppm').read_bytes()
assert Image.open(p/'flight-00.ppm').crop((164,48,304,120)).tobytes()==Image.open(p/'flight-08.ppm').crop((164,48,304,120)).tobytes(), 'Main-story scrolling moved the side trips'
assert Image.open(p/'flight-09.ppm').crop((16,48,156,120)).tobytes()==Image.open(p/'flight-15.ppm').crop((16,48,156,120)).tobytes(), 'Side-trip scrolling moved the main story'
print('PASS: 16 grouped flight entries, independent column scrolling, forward/reverse navigation, unchanged frame, and watch/submenu renders')
PY
rg '^PASS:' "$work/result.log"
