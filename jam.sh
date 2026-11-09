#!/bin/sh
set -e

JAM_PIN=4.0.0
JAM_DIST_ROOT_DEFAULT=https://github.com/Pt-Framework/Pt/releases/download
JAM_DIST_ROOT=${JAM_DIST_ROOT:-"$JAM_DIST_ROOT_DEFAULT"}

ROOT=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
JAM_ROOT=${JAM_ROOT:-"$ROOT/jam"}
CORE="$JAM_ROOT/core"
INCOMING="$JAM_ROOT/core.incoming"
OUTGOING="$JAM_ROOT/core.outgoing"
SUPPORTED_FORMAT=1

INIT=
if [ "$1" = "init" ]; then
    INIT=1
    shift
fi

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

need_init()
{
    echo "Jam core is missing or does not match snapshot $JAM_PIN at \"$JAM_ROOT\"." >&2
    echo "Run: \"$0\" init" >&2
    exit 1
}

invalid_core()
{
    echo "Jam core is invalid at \"$JAM_ROOT\"." >&2
    echo "Remove core/ and run init to install snapshot $JAM_PIN." >&2
    exit 1
}

compiler_missing()
{
    echo "The host binary \"$JAM\" is missing." >&2
    echo "A C compiler is required to build it." >&2
    exit 1
}

tool_missing()
{
    echo "Required tool \"$1\" was not found." >&2
    exit 1
}

first_line()
{
    sed -n '1p' "$1" | tr -d '\r'
}

has_extra_line()
{
    _extra=`sed -n '2p' "$1" | tr -d '\r'`
    if [ -n "$_extra" ]; then
        return 0
    fi
    return 1
}

format_ok()
{
    _file=$1/meta/format
    if [ ! -r "$_file" ]; then
        return 1
    fi
    if has_extra_line "$_file"; then
        return 1
    fi
    _line=`first_line "$_file"`
    if [ "$_line" = "$SUPPORTED_FORMAT" ]; then
        return 0
    fi
    return 1
}

version_ok()
{
    _file=$1/meta/version
    if [ ! -r "$_file" ]; then
        return 1
    fi
    if has_extra_line "$_file"; then
        return 1
    fi
    _line=`first_line "$_file"`
    if echo "$_line" | awk '$0 !~ /^[1-9][0-9]*\.[0-9]+\.[0-9]+$/ { exit 1 }' >/dev/null
    then
        return 0
    fi
    return 1
}

core_state()
{
    _core=$1
    if [ -e "$_core" ] && [ ! -d "$_core" ]; then
        echo invalid
        return
    fi
    if [ ! -d "$_core" ]; then
        echo absent
        return
    fi
    if ! format_ok "$_core"; then
        echo invalid
        return
    fi
    if ! version_ok "$_core"; then
        echo invalid
        return
    fi
    _line=`first_line "$_core/meta/version"`
    if [ "$_line" = "$JAM_PIN" ]; then
        echo match
    else
        echo mismatch
    fi
}

contents_ok()
{
    _core=$1
    _contents=$_core/meta/contents
    if [ ! -r "$_contents" ]; then
        return 1
    fi
    while IFS= read -r _line || [ -n "$_line" ]; do
        _line=`printf '%s' "$_line" | tr -d '\r'`
        _size=`printf '%s\n' "$_line" | awk '{ print $1 }'`
        _path=`printf '%s\n' "$_line" | awk '{ print $2 }'`
        if [ -z "$_size" ] || [ -z "$_path" ]; then
            return 1
        fi
        if ! echo "$_size" | awk '$0 !~ /^[0-9]+$/ { exit 1 }' >/dev/null; then
            return 1
        fi
        _file=$_core/$_path
        if [ ! -f "$_file" ]; then
            echo "Snapshot file \"$_path\" is missing." >&2
            return 1
        fi
        _actual=`wc -c < "$_file" | tr -d ' \t'`
        if [ "$_actual" != "$_size" ]; then
            echo "Snapshot file \"$_path\" has size $_actual, expected $_size." >&2
            return 1
        fi
    done < "$_contents"
}

check_extracted()
{
    _core=$1
    if ! format_ok "$_core"; then
        echo "Extracted snapshot has an unsupported or missing format revision." >&2
        return 1
    fi
    if ! version_ok "$_core"; then
        echo "Extracted snapshot has an invalid version." >&2
        return 1
    fi
    _line=`first_line "$_core/meta/version"`
    if [ "$_line" != "$JAM_PIN" ]; then
        echo "Extracted snapshot version $_line does not match pin $JAM_PIN." >&2
        return 1
    fi
    if ! contents_ok "$_core"; then
        return 1
    fi
    return 0
}

download_archive()
{
    _url=$1
    _dest=$2
    if command -v curl >/dev/null 2>&1; then
        curl -L -f -o "$_dest" "$_url"
    elif command -v wget >/dev/null 2>&1; then
        wget -O "$_dest" "$_url"
    else
        echo "Required tool \"curl\" or \"wget\" was not found." >&2
        exit 1
    fi
}

install_snapshot()
{
    if [ ! -d "$JAM_ROOT" ]; then
        mkdir -p "$JAM_ROOT" || {
            echo "Could not create \"$JAM_ROOT\"." >&2
            exit 1
        }
    fi

    if [ ! -d "$CORE" ] && [ -d "$OUTGOING" ]; then
        mv "$OUTGOING" "$CORE"
    fi

    STATE=`core_state "$CORE"`
    if [ "$STATE" = "match" ]; then
        return
    fi
    if [ "$STATE" = "invalid" ]; then
        invalid_core
    fi

    echo "Installing Jam snapshot $JAM_PIN..."
    rm -rf "$INCOMING"
    mkdir -p "$INCOMING" || {
        echo "Could not create \"$INCOMING\"." >&2
        exit 1
    }

    ARCHIVE="$INCOMING/jam-$JAM_PIN.tar.gz"
    URL="$JAM_DIST_ROOT/jam-$JAM_PIN/jam-$JAM_PIN.tar.gz"
    if ! download_archive "$URL" "$ARCHIVE"; then
        echo "Could not download \"$URL\"." >&2
        rm -rf "$INCOMING"
        exit 1
    fi

    if ! command -v tar >/dev/null 2>&1; then
        tool_missing tar
    fi
    if ! tar -xzf "$ARCHIVE" -C "$INCOMING"; then
        echo "Could not extract \"$ARCHIVE\"." >&2
        rm -rf "$INCOMING"
        exit 1
    fi

    STAGED="$INCOMING/core"
    if ! check_extracted "$STAGED"; then
        echo "Downloaded snapshot failed checks." >&2
        rm -rf "$INCOMING"
        exit 1
    fi

    if [ -d "$CORE" ]; then
        rm -rf "$OUTGOING"
        if ! mv "$CORE" "$OUTGOING"; then
            echo "Could not move \"$CORE\" aside." >&2
            rm -rf "$INCOMING"
            exit 1
        fi
        if ! mv "$STAGED" "$CORE"; then
            echo "Could not install \"$CORE\"." >&2
            mv "$OUTGOING" "$CORE"
            rm -rf "$INCOMING"
            exit 1
        fi
        rm -rf "$OUTGOING"
    else
        if ! mv "$STAGED" "$CORE"; then
            echo "Could not install \"$CORE\"." >&2
            rm -rf "$INCOMING"
            exit 1
        fi
    fi
    rm -rf "$INCOMING"
}

build_host_binary()
{
    if test -x "$JAM"; then
        return
    fi
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
}

if [ -n "$INIT" ]; then
    install_snapshot
    STATE=`core_state "$CORE"`
    if [ "$STATE" != "match" ]; then
        invalid_core
    fi
    build_host_binary
    exit 0
fi

STATE=`core_state "$CORE"`
if [ "$STATE" = "invalid" ]; then
    invalid_core
fi
if [ "$STATE" != "match" ]; then
    need_init
fi
build_host_binary

export JAM_ROOT
exec "$JAM" -f "$CORE/Jambase" "$@"
