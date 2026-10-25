#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
${CC:-cc} -std=gnu99 -O2 -ffunction-sections -fdata-sections -include string.h \
 $(sdl2-config --cflags) -Ithird_party/chocolate-keen/src \
 -DCHOCOLATE_KEEN_IS_EPISODE1_ENABLED -DCHOCOLATE_KEEN_IS_EPISODE2_ENABLED \
 -DCHOCOLATE_KEEN_IS_EPISODE3_ENABLED \
 tests/native-arcade-exit.c third_party/chocolate-keen/src/game/gameplay.c \
 -Wl,--gc-sections -o "$work/exit-test"
"$work/exit-test"
status=0
"$work/exit-test" error > "$work/error.log" 2>&1 || status=$?
test "$status" -eq 1
rg 'test error|PASS:' "$work/error.log"
