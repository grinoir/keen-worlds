#!/bin/sh
# Uses the player's converted remake, real collision code and save loader.
set -eu
cd "$(dirname "$0")/.."
binary=$(realpath "${1:-../play/omnispeak}")
game_data=$(realpath "${2:-../play}")
if [ ! -f "$game_data/arcade/mars/MARS.CK5" ]; then
 echo "Install the converted Mars remake first (kwtools/kw_mars.py)." >&2
 exit 1
fi
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOWAIT="$work/statue-" KW_AUTOFINALE=1 timeout -k 2s 30s \
 "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /MARSTEST > "$work/rules.log" 2>&1; then
 head -40 "$work/rules.log"
 exit 1
fi
rg '^PASS: Mars ' "$work/rules.log"
