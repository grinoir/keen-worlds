#!/bin/sh
# The bunk's Keen Dreams Gold launcher: checks and native launch, no game data.
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$work/game data" "$work/user saves"
${CC:-cc} -std=gnu99 -O2 -DWITH_KEEN4 -DWITH_SDL -Isrc $(sdl2-config --cflags) \
 tests/dreams-gold-launch.c src/kw_arcade.c $(sdl2-config --libs) -o "$work/gold-test"
${CC:-cc} -std=c99 -O2 tests/dreams-gold-companion.c -o "$work/keen-dreams-gold"
SDL_VIDEODRIVER=dummy "$work/gold-test" "$work/game data" "$work/user saves"
