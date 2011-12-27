#!/bin/sh

JAM_SOURCES="builtins.c class.c command.c compile.c constants.c debug.c execunix.c expand.c fileunix.c filesys.c function.c glob.c hash.c hcache.c hdrmacro.c headers.c jam.c jambase.c jamgram.c lists.c make.c make1.c md5.c mem.c modules.c native.c object.c option.c output.c parse.c pathunix.c pwd.c regexp.c rules.c scan.c search.c strings.c subst.c timestamp.c variable.c w32_getreg.c modules/set.c modules/path.c modules/regex.c modules/property-set.c modules/sequence.c modules/order.c"

cc -o ../jam.bin -DPT_MCHECK $JAM_SOURCES
