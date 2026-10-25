#!/bin/sh
# Native course gating and migration, with an isolated config/save directory.
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout -k 2s 30s \
 "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /CHRONICLETEST > "$work/result.log" 2>&1; then
 tail -20 "$work/result.log"; exit 1
fi
rg '^PASS:' "$work/result.log"
