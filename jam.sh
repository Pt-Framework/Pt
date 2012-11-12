#!/bin/sh

#TODO set JAMDIR to jam

JAM_SOURCES="./builtins.c ./class.c ./command.c ./compile.c ./constants.c ./debug.c ./execcmd.c ./execunix.c ./fileunix.c ./filesys.c ./function.c ./frames.c ./glob.c ./hash.c ./hcache.c ./hdrmacro.c ./headers.c ./jam.c ./jambase.c ./jamgram.c ./lists.c ./make.c ./make1.c ./md5.c ./mem.c ./modules.c ./native.c ./object.c ./option.c ./output.c ./parse.c ./pathunix.c ./pathsys.c ./cwd.c ./regexp.c ./rules.c ./scan.c ./search.c ./strings.c ./subst.c ./timestamp.c ./variable.c ./w32_getreg.c ./modules/set.c ./modules/path.c ./modules/regex.c ./modules/property-set.c ./modules/sequence.c ./modules/order.c"

#use -DPT_MCHECK for memory usage statistics

if test ! -f ./jam100.bin; then
    echo "Building jam executable version 1.00..."
    cd jam/src
    cc -o ../../jam100.bin $JAM_SOURCES
    cd ../..
    echo "Built jam100.bin"
fi

./jam100.bin $*
