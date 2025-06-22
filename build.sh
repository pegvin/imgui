#!/bin/sh

set -eu

CC=${CC:-gcc}
LD=${LD:-gcc}
BUILD="build"
SOURCES="src/main.c src/imgui.c src/os/os.c src/base/math.c src/base/arena.c src/base/string.c"
OBJECTS=$(echo "$SOURCES" | sed "s|\([^ ]*\)\.c|$BUILD/\1.c.o|g")
CFLAGS="-std=c99 -Isrc/ -march=native -MMD -MP -Wall -Wextra -Wpadded -pedantic -D_DEFAULT_SOURCE=1 -ffast-math -fvisibility=hidden -fdata-sections -ffunction-sections"
LFLAGS="-fvisibility=hidden -Wl,--gc-sections -lX11"
CMD=${1:-}

mkdir -p $BUILD "$BUILD/.ccache"

if [ "$CMD" = "clean" ]; then
	rm -rf $BUILD
	exit 0
elif [ "$CMD" = "bear" ]; then
	bear --append --output "$BUILD/compile_commands.json" -- $0 # github.com/rizsotto/Bear
	exit 0
elif [ "$CMD" = "release" ]; then
	CFLAGS="$CFLAGS -O3"
elif [ "$CMD" = "" ]; then
	CFLAGS="$CFLAGS -O0 -g -fsanitize=address,undefined"
	LFLAGS="$LFLAGS -fsanitize=address,undefined"
elif [ "$CMD" ]; then
	echo "Invalid command '$CMD', Available commands are: clean/bear/release or none to just build."
	exit 1
fi

echo "$SOURCES 0" | tr ' ' '\n' | while read source; do
	if [ "$source" = "0" ]; then wait; exit 0; fi
	echo "CC $source"
	mkdir -p "$(dirname "$BUILD/$source.o")"
	CCACHE_DIR="$BUILD/.ccache" ccache $CC $CFLAGS -c $source -o "$BUILD/$source.o" &
done

$LD $OBJECTS $LFLAGS -o "$BUILD/imgui"
echo "LD $BUILD/imgui"
