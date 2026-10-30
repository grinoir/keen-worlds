#!/bin/sh
# Return to the Shadowlands: every map loads and runs on all difficulties with
# its new creatures; dying takes back the level's score. Needs the player's
# v1.8 files and our RTTS.CK4 in mods/rtts. Use a RENDERER=null build.
set -eu
binary=$(realpath "${1:-$(dirname "$0")/../../play/omnispeak}")
game_data=$(realpath "${2:-$(dirname "$0")/../../play}")
test -f "$game_data/mods/rtts/RTTS.CK4" || {
 echo 'Unpack Return to the Shadowlands v1.8 in mods/rtts and copy data/mods/rtts there first.' >&2; exit 1;
}
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
printf 'oplEmulator = "nukedopl3"\nloadingDelay = 0\n' > "$work/OMNISPK.CFG"
cd "$game_data"
if ! SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout -k 2s 300s \
 "$binary" /GAMEPATH "$game_data" /USERPATH "$work" /RTTSTEST > "$work/result.log" 2>&1; then
 tail -40 "$work/result.log"; exit 1
fi
grep -a '^PASS:' "$work/result.log"
test "$(grep -ac '^PASS:' "$work/result.log")" -eq 2
if grep -ai 'uncached\|AddressSanitizer\|runtime error:\|Shadowlands test line' "$work/result.log"; then exit 1; fi
