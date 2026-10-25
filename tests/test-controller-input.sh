#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
${CC:-cc} -std=c17 -O2 -ffunction-sections -fdata-sections -DWITH_KEEN4 -Isrc \
 $(pkg-config --cflags sdl2) tests/controller-input.c src/id_in_sdl.c \
 -Wl,--gc-sections $(pkg-config --libs sdl2) -o "$work/controller-input"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy "$work/controller-input"
