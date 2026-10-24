#!/bin/sh
# Build only the audio code exercised by the regression test.
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
${CXX:-c++} -x c++ -std=c++11 -O2 -ffunction-sections -fdata-sections \
	$(sdl2-config --cflags) -DWITH_KEEN4 -Isrc tests/audio-output.cpp src/id_sd.c \
	src/opl/dbopl.c src/opl/nuked_opl3.c -Wl,--gc-sections \
	$(sdl2-config --libs) -o "$work/audio-output"
"$work/audio-output"
