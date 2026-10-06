#!/bin/sh
# Build Keen Worlds for Linux and Windows and pack each as one archive.
#
#   kwtools/kw_package.sh                  both archives, in ../dist
#   kwtools/kw_package.sh --with-my-games  also copies the games found in
#                                          ../play (Keen 4-6 and mods/) in
#
# An archive holds the game program, Keen Worlds' own files (the Megarocket,
# the voices, Omnispeak's tables for Keen 4-6), the guide and the source.
# It holds nobody's game: the player adds those. --with-my-games is for
# trying the archives on your own machines; such an archive contains other
# people's games and is not for handing out.
#
# Needs, in ../toolchain (or $KW_TOOLCHAIN):
#   zig-x86_64-linux-*      https://ziglang.org/download/  (Linux build that
#                           runs on older distributions: glibc 2.28)
#   llvm-mingw-*            https://github.com/mstorsjo/llvm-mingw/releases
#   SDL2-2.*                SDL2-devel-<version>-mingw from
#                           https://github.com/libsdl-org/SDL/releases
# and SDL2's headers and library on this machine for the Linux build.
set -e

REPO=$(cd "$(dirname "$(readlink -f "$0")")/.." && pwd)
PROJECT=$(dirname "$REPO")
TOOLCHAIN=${KW_TOOLCHAIN:-$PROJECT/toolchain}
DIST=${KW_DIST:-$PROJECT/dist}
PLAY=$PROJECT/play
WITH_GAMES=0
[ "$1" = "--with-my-games" ] && WITH_GAMES=1

ZIG=$(ls -d "$TOOLCHAIN"/zig-x86_64-linux-*/zig 2>/dev/null | tail -1)
MINGW=$(ls -d "$TOOLCHAIN"/llvm-mingw-*/bin 2>/dev/null | tail -1)
SDLWIN=$(ls -d "$TOOLCHAIN"/SDL2-2.* 2>/dev/null | tail -1)
for need in "$ZIG" "$MINGW" "$SDLWIN"; do
	if [ -z "$need" ]; then
		echo "kw_package: a build tool is missing from $TOOLCHAIN (see the top of this script)." >&2
		exit 1
	fi
done

# The version is written in src/kw_version.h; the commit says which build of it.
NUMBER=$(sed -n 's/^#define KW_VERSION "\(.*\)"/\1/p' "$REPO/src/kw_version.h")
NAMED=$(sed -n 's/^#define KW_VERSION_NAME "\(.*\)"/\1/p' "$REPO/src/kw_version.h")
VERSION=$NUMBER-$(git -C "$REPO" log -1 --format=%h)
if [ -n "$(git -C "$REPO" status --porcelain)" ]; then
	VERSION=$VERSION-changed
fi
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
JOBS=$(nproc 2>/dev/null || echo 4)

# ---- the Linux program
# The sources are C built as C++, which clang must be told when compiling
# (and must not be told when linking).
cat > "$WORK/cxx" <<EOF
#!/bin/sh
for a in "\$@"; do
	if [ "\$a" = "-c" ]; then
		exec "$ZIG" c++ -target x86_64-linux-gnu.2.28 -x c++ -Wno-deprecated "\$@"
	fi
done
exec "$ZIG" c++ -target x86_64-linux-gnu.2.28 "\$@"
EOF
chmod +x "$WORK/cxx"
SDLSO=$(ls /usr/lib/libSDL2-2.0.so.0 /usr/lib64/libSDL2-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0 2>/dev/null | head -1)
GLSO=$(ls /usr/lib/libGL.so.1 /usr/lib64/libGL.so.1 /usr/lib/x86_64-linux-gnu/libGL.so.1 2>/dev/null | head -1)
echo "Building for Linux..."
make -s -C "$REPO/src" -j"$JOBS" BINDIR="$WORK/bin-linux" OBJDIR="$WORK/obj-linux" \
	COMPILER="$WORK/cxx" \
	SDL_CFLAGS="-isystem /usr/include/SDL2 -D_REENTRANT -DWITH_SDL" \
	SDL_LIBS="$SDLSO" LIBGL="$GLSO" LDFLAGS="-Wl,--allow-shlib-undefined" \
	"$WORK/bin-linux/omnispeak" >"$WORK/linux.log" 2>&1 || { tail -30 "$WORK/linux.log"; exit 1; }

# ---- the Windows program (one .exe, nothing to install beside it)
echo "Building for Windows..."
make -s -C "$REPO/src" -j"$JOBS" PLATFORM=win64 TOOLDIR="$MINGW" BUILDASCPP=1 \
	BINDIR="$WORK/bin-win64" OBJDIR="$WORK/obj-win64" SDL_DIR="$SDLWIN/x86_64-w64-mingw32" \
	SDL_CONFIG="$SDLWIN/x86_64-w64-mingw32/bin/sdl2-config --prefix=$SDLWIN/x86_64-w64-mingw32" \
	"$WORK/bin-win64/omnispeak.exe" >"$WORK/win.log" 2>&1 || { tail -30 "$WORK/win.log"; exit 1; }

# ---- what both archives share
fill() {
	d=$1
	mkdir -p "$d/megarocket" "$d/voice" "$d/mods" "$d/keen6-v1.5"
	cp "$REPO"/data/keen4/*.CK4 "$REPO"/data/keen5/*.CK5 "$d/"
	# Keen 6 v1.4 is the one Keen Worlds is tested with; v1.5's tables are beside it.
	cp "$REPO"/data/keen6e14/*.CK6 "$d/"
	cp "$REPO"/data/keen6e15/*.CK6 "$d/keen6-v1.5/"
	cp "$REPO"/data/megarocket/* "$d/megarocket/"
	cp "$REPO"/data/voice/*.OGG "$d/voice/"
	sed "s/@VERSION@/$NUMBER ($NAMED)/" "$REPO/kwtools/package/README.txt" > "$d/README.txt"
	# Each world's folder, with the files that are Keen Worlds' own in it.
	cp -r "$REPO"/data/mods/. "$d/mods/"
	find "$d/mods" -name .keep -delete
	cp "$REPO/kwtools/package/MODS.txt" "$d/mods/README.txt"
	cp "$REPO/LICENSE" "$d/LICENSE.txt"
	cp "$REPO/AUTHORS" "$d/AUTHORS.txt"
	echo "Keen Worlds $NUMBER ($NAMED), build $VERSION, $(date +%Y-%m-%d)" > "$d/VERSION.txt"
	# The program is under the GPL: its source goes with it.
	git -C "$REPO" archive --format=tar.gz --prefix=keen-worlds-source/ -o "$d/source.tar.gz" HEAD \
		':!data/voice' ':!data/megarocket' ':!data/mods'
	if [ "$WITH_GAMES" = 1 ]; then
		for e in 4 5 6; do
			for f in GAMEMAPS EGAGRAPH AUDIO; do
				[ -f "$PLAY/$f.CK$e" ] && cp "$PLAY/$f.CK$e" "$d/"
			done
		done
		# The tables in play fit the games in play (an older Keen 6 has its own).
		cp "$PLAY"/*.CK4 "$PLAY"/*.CK5 "$PLAY"/*.CK6 "$d/"
		rm -f "$d"/CONFIG.CK?
		[ -d "$PLAY/mods" ] && cp -r "$PLAY/mods/." "$d/mods/"
		echo "This copy contains other people's games. It is for your own machines, not for handing out." > "$d/PRIVATE-COPY.txt"
	fi
}

NAME=KeenWorlds-$VERSION
[ "$WITH_GAMES" = 1 ] && NAME=$NAME-private
mkdir -p "$DIST"

L=$WORK/pack/$NAME-linux
fill "$L"
cp "$WORK/bin-linux/omnispeak" "$L/omnispeak"
cp "$REPO/kwtools/package/keenworlds.sh" "$L/keenworlds.sh"
chmod +x "$L/omnispeak" "$L/keenworlds.sh"
rm -f "$DIST/$NAME-linux.tar.gz"
tar -C "$WORK/pack" -czf "$DIST/$NAME-linux.tar.gz" "$NAME-linux"

W=$WORK/pack/$NAME-windows
fill "$W"
cp "$WORK/bin-win64/omnispeak.exe" "$W/omnispeak.exe"
cp "$REPO/kwtools/package/KeenWorlds.cmd" "$W/KeenWorlds.cmd"
# Windows' Notepad wants its own line ends.
for f in "$W/README.txt" "$W/mods/README.txt" "$W/VERSION.txt" "$W/LICENSE.txt" "$W/AUTHORS.txt"; do
	sed -i 's/\r$//; s/$/\r/' "$f"
done
rm -f "$DIST/$NAME-windows.zip"
(cd "$WORK/pack" && zip -q -r "$DIST/$NAME-windows.zip" "$NAME-windows")

ls -lh "$DIST/$NAME-linux.tar.gz" "$DIST/$NAME-windows.zip"
