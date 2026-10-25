#!/bin/sh
# Real converted game data, native collision/physics, transports and saves.
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
test -f "$game_data/mods/alphamatic/ALPHAM.CK5" || {
 echo 'Supply The Alphamatic 3.0 in mods/alphamatic first.' >&2; exit 1;
}
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout -k 2s 45s \
 "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /ALPHATEST > "$work/result.log" 2>&1; then
 tail -40 "$work/result.log"; exit 1
fi
rg '^PASS:' "$work/result.log"
