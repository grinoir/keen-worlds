#!/bin/sh
# Build the native Keen Dreams Gold Edition companion, without game data.
# Usage: kw_dreams_gold_build.sh OUTPUT [native|windows]
set -eu
repo=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
out=$1
platform=${2:-native}
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$(dirname "$out")"
if [ "$platform" = windows ]; then
 toolchain=${KW_TOOLCHAIN:-$(dirname "$repo")/toolchain}
 cc=$(find "$toolchain" -path '*/bin/x86_64-w64-mingw32-gcc' -print -quit)
 sdl=$(find "$toolchain" -path '*/x86_64-w64-mingw32/bin/sdl2-config' -print -quit)
 sdlprefix=$(dirname "$(dirname "$sdl")")
 cflags="-I$sdlprefix/include -I$sdlprefix/include/SDL2 -D_REENTRANT -DSDL_MAIN_HANDLED"
 libs="-L$sdlprefix/lib -static -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid"
else
 cc=${CC:-cc}
 cflags=$(sdl2-config --cflags)
 libs="$(sdl2-config --libs) -lm"
fi
src="$repo/third_party/keen-dreams-gold"
set --
for file in "$src"/*.c "$repo/src/opl/dbopl.c"; do
 obj="$work/$(basename "$file").o"
 # The DOS source relies on Borland's leniency; keep those diagnostics quiet.
 "$cc" -std=gnu99 -O2 -include string.h -fno-strict-aliasing -fwrapv $cflags \
  -I"$src" -I"$repo/src" -Wno-implicit-function-declaration -Wno-int-conversion \
  -Wno-incompatible-pointer-types -w -c "$file" -o "$obj"
 set -- "$@" "$obj"
done
"$cc" "$@" $libs -o "$out"
