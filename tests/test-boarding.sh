#!/bin/sh
# Exercise actual boarding hooks and installed world data with isolated saves/config.
set -eu
binary=$(realpath "${1:-$(dirname "$0")/../../play/omnispeak}")
game_data=$(realpath "${2:-$(dirname "$0")/../../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout -k 2s 45s \
 "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /BOARDTEST > "$work/result.log" 2>&1; then
 tail -35 "$work/result.log"; exit 1
fi
rg '^PASS:' "$work/result.log"
if rg 'Boarding test|runtime error:|ERROR: AddressSanitizer' "$work/result.log"; then exit 1; fi
