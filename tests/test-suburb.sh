#!/bin/sh
# Real Suburb maps/actions, connected progress, complete save headers and actors.
set -eu
binary=$(realpath "${1:-$(dirname "$0")/../../play/omnispeak}")
game_data=$(realpath "${2:-$(dirname "$0")/../../play}")
test -f "$game_data/mods/suburb/SUBURB.CK5" || {
 echo 'Convert the audited Suburb v1.2 data into mods/suburb first.' >&2; exit 1;
}
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy KW_AUTOTALK=1 KW_AUTOFINALE=1 \
 timeout -k 2s 60s "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /SUBURBTEST > "$work/result.log" 2>&1; then
 tail -40 "$work/result.log"; exit 1
fi
rg '^PASS:' "$work/result.log"
test "$(rg -c '^PASS:' "$work/result.log")" -eq 8
if rg -i 'uncached|AddressSanitizer|runtime error:|Suburb test line' "$work/result.log"; then exit 1; fi
