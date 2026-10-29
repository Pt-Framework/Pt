#!/bin/sh
set -e

ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
JAM_ROOT=${JAM_ROOT:-"$ROOT/jam"}
CORE="$JAM_ROOT/core"
VERSION="$CORE/meta/version"

invalid_core()
{
    echo "Jam core is missing or invalid at \"$JAM_ROOT\"." >&2
    echo "Provide a prepared JAM_ROOT with supported snapshot format revision 1." >&2
    exit 1
}

compiler_missing()
{
    echo "The host binary \"$JAM\" is missing." >&2
    echo "A C compiler is required to build it." >&2
    exit 1
}

if [ ! -r "$VERSION" ]; then
    invalid_core
fi

if ! awk '
    NR != 1 { exit 1 }
    $1 !~ /^[1-9][0-9]*$/ { exit 1 }
    $2 !~ /^[0-9][0-9]*$/ { exit 1 }
    $3 !~ /^[0-9][0-9]*$/ { exit 1 }
    $4 != "1" || NF != 4 { exit 1 }
    END { if (NR != 1) exit 1 }
' "$VERSION"; then
    invalid_core
fi

INIT=
for arg do
    if [ "$arg" = "--init" ]; then
        INIT=1
    fi
done

case `uname -s` in
Darwin)
    HOST_ID=macos-universal
    ;;
Linux)
    HOST_ID=linux-x64
    ;;
*)
    echo "Unsupported host: `uname -s`" >&2
    exit 1
    ;;
esac
JAM_BINARY_DIR="$CORE/bin/$HOST_ID"
JAM="$JAM_BINARY_DIR/jam"
SRC="$CORE/src"

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
    if [ ! -d "$SRC" ]; then
        invalid_core
    fi

    echo "Building jam..."
    CC="${CC:-cc}"
    if ! command -v "$CC" > /dev/null 2>&1; then
        compiler_missing
    fi

    if ! mkdir -p "$JAM_BINARY_DIR"; then
        echo "Could not create or access the host binary directory \"$JAM_BINARY_DIR\"." >&2
        exit 1
    fi

    case `uname -s` in
    Darwin)
        FLAGS="-O2 -arch x86_64 -arch arm64 -mmacosx-version-min=11.0"
        ;;
    *)
        FLAGS="-O2"
        ;;
    esac

    if ! (
        cd "$SRC"
        $CC $FLAGS -o "$JAM" $SOURCES
    ); then
        echo "Could not build the host binary \"$JAM\"." >&2
        exit 1
    fi

    (
        cd "$SRC"
        rm -f *.o
    )
fi

if [ -n "$INIT" ]; then
    exit 0
fi

export JAM_ROOT
exec "$JAM" -f "$CORE/Jambase" "$@"
