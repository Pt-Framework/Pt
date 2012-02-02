#!/bin/sh

JAM_SOURCES="./command.c ./compile.c ./debug.c ./execunix.c ./expand.c ./fileunix.c ./glob.c ./hash.c ./hdrmacro.c ./headers.c ./jam.c ./jambase.c ./jamgram.c ./lists.c ./make.c ./make1.c ./newstr.c ./option.c ./parse.c ./pathunix.c ./pathvms.c ./regexp.c ./rules.c ./scan.c ./search.c ./subst.c ./timestamp.c ./variable.c ./modules.c ./strings.c ./filesys.c ./builtins.c ./pwd.c ./class.c ./w32_getreg.c ./native.c ./modules/set.c ./modules/path.c ./modules/regex.c ./modules/property-set.c ./modules/sequence.c ./modules/order.c"

if test ! -f ./jam2.bin; then
    echo "Building jam executable..."
    cd jam2/src
    cc -o ../../jam2.bin $JAM_SOURCES
    cd ../..
    echo "Built jam2.bin"
fi

./jam2.bin $*


