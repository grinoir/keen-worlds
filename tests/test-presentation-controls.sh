#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
${CC:-cc} -std=c17 -O2 -ffunction-sections -fdata-sections -DWITH_KEEN4 -Isrc \
	tests/presentation-controls.c src/kw_skip.c src/id_in.c \
	-Wl,--gc-sections -o "$work/presentation-controls"
"$work/presentation-controls"
