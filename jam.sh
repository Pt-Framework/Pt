#!/bin/sh
set -e

ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
JAM="$ROOT/jam/jam"
SRC="$ROOT/jam/src"

SOURCES="
builtins.c class.c command.c compile.c constants.c debug.c debugger.c execcmd.c execunix.c
fileunix.c filesys.c function.c frames.c glob.c hash.c hcache.c hdrmacro.c
headers.c jam.c jambase.c jamgram.c lists.c make.c make1.c md5.c mem.c modules.c
native.c object.c option.c output.c parse.c pathunix.c pathsys.c cwd.c regexp.c rules.c scan.c
search.c strings.c subst.c timestamp.c variable.c w32_getreg.c
modules/set.c modules/path.c modules/regex.c modules/property-set.c modules/sequence.c
modules/order.c
"

if test ! -x "$JAM"; then
    echo "Building jam..."
    CC="${CC:-cc}"
    case `uname -s` in
    Darwin)
        FLAGS="-O2 -arch x86_64 -arch arm64 -mmacosx-version-min=11.0"
        ;;
    *)
        FLAGS="-O2"
        ;;
    esac
    (
        cd "$SRC"
        $CC $FLAGS -o "$JAM" $SOURCES
        rm -f *.o
    )
fi

exec "$JAM" "$@"
