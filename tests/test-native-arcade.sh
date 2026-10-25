#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$work/game data" "$work/user saves"
python3 - "$work/game data" <<'PY'
import sys
from pathlib import Path
root=Path(sys.argv[1])
for episode,size in enumerate([51190,58335,61599],1):
 folder=root/'arcade'/f'keen{episode}';folder.mkdir(parents=True)
 (folder/f'KEEN{episode}.EXE').write_bytes(bytes(size))
 required=['EGAHEAD','EGALATCH','EGASPRIT','LEVEL01','LEVEL80']
 if episode == 1: required.append('SOUNDS')
 for name in required:
  (folder/f'{name}.CK{episode}').write_bytes(b'fixture')

PY
${CC:-cc} -std=gnu99 -O2 -DWITH_KEEN4 -DWITH_SDL -Isrc $(sdl2-config --cflags) \
 tests/native-arcade.c src/kw_arcade.c $(sdl2-config --libs) -o "$work/arcade-test"
${CC:-cc} -std=c99 -O2 tests/arcade-companion.c -o "$work/keen-vorticons"
SDL_VIDEODRIVER=dummy "$work/arcade-test" "$work/game data" "$work/user saves"
