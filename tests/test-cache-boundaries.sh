#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
${CC:-cc} -std=gnu99 -O2 -ffunction-sections -fdata-sections -DWITH_KEEN4 -Isrc \
 tests/cache-boundaries.c src/id_ca.c -Wl,--gc-sections -o "$work/cache-boundaries"
"$work/cache-boundaries"
