#!/bin/sh
# Real watch confirmation, End Game, shared scores and the title, in each world.
# Native arcade End Game/return is covered by test-native-arcade-exit.sh.
set -eu
binary=$(realpath "${1:-$(dirname "$0")/../../play/omnispeak}")
game_data=$(realpath "${2:-$(dirname "$0")/../../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
for world in megarocket keen4 keen5 keen6 keen7 keen8 keen9 arealm fitf osi keen95 ck55 asylum bounty dreams alpham suburb; do
 if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOENDGAME=1 KW_AUTOCONFIRM=1 \
  KW_WATCHWORLD="$world" KW_AUTOWATCH="$work" KW_AUTOTITLE="$work/title.ppm" \
  timeout -k 2s 20s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /MENUTEST > "$work/result.log" 2>&1; then
  printf 'FAIL: End Game in %s\n' "$world"
  tail -25 "$work/result.log"; exit 1
 fi
 python3 - "$work" <<'PY'
from pathlib import Path
from PIL import Image
import sys
p=Path(sys.argv[1]);log=(p/'result.log').read_text(errors='replace')
assert 'PASS: End Game and the shared high-score panel' in log
assert 'runtime error:' not in log and 'ERROR: AddressSanitizer' not in log
for name in ('highscores','title'):
 assert Image.open(p/f'{name}.ppm').size==(320,200)
PY
 printf 'PASS: %s confirms End Game, renders shared scores, returns to title and shuts down cleanly\n' "$world"
done
