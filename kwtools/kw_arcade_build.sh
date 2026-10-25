#!/bin/sh
# Build the vendored native Vorticons engine, without copying game data.
# Usage: kw_arcade_build.sh OUTPUT [native|windows]
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
 cflags="-I$sdlprefix/include/SDL2 -D_REENTRANT -DSDL_MAIN_HANDLED"
 libs="-L$sdlprefix/lib -static -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid"
else
 cc=${CC:-cc}
 cflags=$(sdl2-config --cflags)
 libs="$(sdl2-config --libs) -lm"
fi
set --
for file in "$repo"/third_party/chocolate-keen/src/*/*.c "$repo"/third_party/chocolate-keen/src/third_party/cgenius/fileio/lz.c "$repo"/third_party/chocolate-keen/src/third_party/cgenius/fileio/compression/*.c; do
 case "$file" in */platform/input_vita.c|*/platform/input_windows.c|*/platform/sleep_vita.c|*/platform/sleep_emscripten.c) continue ;; esac
 obj="$work/$(basename "$(dirname "$file")")-$(basename "$file").o"
 # Compiler and SDL flags follow the same override convention as the engine.
 "$cc" -std=gnu99 -O2 -include string.h $cflags -I"$repo/third_party/chocolate-keen/src" \
  -DCHOCOLATE_KEEN_IS_EPISODE1_ENABLED -DCHOCOLATE_KEEN_IS_EPISODE2_ENABLED \
  -DCHOCOLATE_KEEN_IS_EPISODE3_ENABLED -c "$file" -o "$obj"
 set -- "$@" "$obj"
done
"$cc" "$@" $libs -o "$out"
