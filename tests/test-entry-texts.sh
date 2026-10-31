#!/bin/sh
# Every installed Galaxy world's "Keen enters..." texts fit the box shown
# while a level loads (a line too wide stops the game with "String exceeds
# width"; /KWTEST loads levels silently and checks this the same way).
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout -k 2s 60s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /ENTRYTEXTS > "$work/result.log" 2>&1 || true
grep -a "^PASS:\|too wide" "$work/result.log"
grep -q "^PASS:" "$work/result.log"
