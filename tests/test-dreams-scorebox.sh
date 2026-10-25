#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
${CC:-cc} -std=c17 -O2 -ffunction-sections -fdata-sections -DWITH_KEEN4 -Isrc \
 tests/dreams-scorebox.c src/ck_map.c -Wl,--gc-sections -o "$work/dreams-scorebox"
"$work/dreams-scorebox"
