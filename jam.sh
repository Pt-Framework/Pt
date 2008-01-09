#!/bin/sh

JAM_SOURCES="./jam/src/command.c ./jam/src/compile.c ./jam/src/debug.c ./jam/src/execunix.c ./jam/src/expand.c ./jam/src/fileunix.c ./jam/src/glob.c ./jam/src/hash.c ./jam/src/hdrmacro.c ./jam/src/headers.c ./jam/src/jam.c ./jam/src/jambase.c ./jam/src/jamgram.c ./jam/src/lists.c ./jam/src/make.c ./jam/src/make1.c ./jam/src/newstr.c ./jam/src/option.c ./jam/src/parse.c ./jam/src/pathunix.c ./jam/src/pathvms.c ./jam/src/regexp.c ./jam/src/rules.c ./jam/src/scan.c ./jam/src/search.c ./jam/src/subst.c ./jam/src/timestamp.c ./jam/src/variable.c ./jam/src/modules.c ./jam/src/strings.c ./jam/src/filesys.c ./jam/src/builtins.c ./jam/src/pwd.c ./jam/src/class.c ./jam/src/w32_getreg.c ./jam/src/native.c ./jam/src/modules/set.c ./jam/src/modules/path.c ./jam/src/modules/regex.c ./jam/src/modules/property-set.c ./jam/src/modules/sequence.c ./jam/src/modules/order.c"

if ! test -f ./jam.bin; then
    echo "Building jam executable..."
    cc -o ./jam.bin $JAM_SOURCES
fi

./jam.bin $*