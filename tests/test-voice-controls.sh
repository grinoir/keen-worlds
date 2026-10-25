#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir "$work/voice"
# Tiny little-endian PCM fixtures, including the forest's combined-line path.
printf '\001\000\002\000' > "$work/voice/TEST.RAW"
cp "$work/voice/TEST.RAW" "$work/voice/FF_ONE.RAW"
cp "$work/voice/TEST.RAW" "$work/voice/FF_TWO.RAW"
${CC:-cc} -std=c17 -O2 -ffunction-sections -fdata-sections -DWITH_KEEN4 -Isrc \
 tests/voice-controls.c src/kw_music.c -Wl,--gc-sections -o "$work/voice-controls"
"$work/voice-controls" "$work"
