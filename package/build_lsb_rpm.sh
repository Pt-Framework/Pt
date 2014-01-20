#!/bin/bash

# Location of the executable for making LSB RPMs
EXEC_FK_ROOT='/opt/lsb/bin/fakeroot'
EXEC_GEN_RPM='/opt/lsb/bin/makelsbpkg'

# Increment this if for some reason the we need to rebuild the RPM
# for the same version of Pt; otherwise, it should be set to '1'
RELEASE='1'

# Check if the tree has been configure for LSB build
LSBCC=`cat Jamrules | grep lsbcc`
LSBCP=`cat Jamrules | grep lsbc++`
if [[ "x$LSBCC" == 'x' && "x$LSBCP" == 'x' ]]; then
    echo 'The tree was not configured for LSB build!'
    exit 1
fi;

# Get the output directory
# Modified from http://www.shelldorado.com/goodcoding/cmdargs.html
OUTDIR='package'
while [ $# -gt 0 ]
do
    case "$1" in
        -o) OUTDIR="$2";
            shift;;
        -*) echo >&2 "usage: $0 [-o output_directory]"
            exit 1;;
         *) break;;
    esac
    shift
done

# Go up to the parent directory if needed
if [[ `pwd` == *package ]]; then
    cd ..
fi

# Determine the installation directory (under 'deploy') and the additional package name
ADDPN=''
RSDIR='deploy/release/'
DEBUG=`cat Jamrules | grep build/debug`
if [[ "x$DEBUG" != 'x' ]]; then
    RSDIR='deploy/debug/'
    ADDPN='-debug'
fi

# Determine the architecture
ARCH=`cat Jamrules | grep TARGET_OSPLAT | awk '{print $3}' | awk -F'"' '{print $2}'`

# Get the version of Pt
V1=`cat include/Pt/Api.h | grep PT_VERSION_MAJOR | awk '{print $3}'`
V2=`cat include/Pt/Api.h | grep PT_VERSION_MINOR | awk '{print $3}'`
V3=`cat include/Pt/Api.h | grep PT_VERSION_REVISION | awk '{print $3}'`
VERSION="$V1.$V2.$V3"

# Invoke the install command
rm -rf $RSDIR
mkdir -p $RSDIR
./jam.sh install -sPT_INSTALL_LIBDIR=$RSDIR/lib -sPT_INSTALL_INCLUDEDIR=$RSDIR/include

cd $RSDIR

# Prepare the package directory structure
INST_INC='/opt/Pt-'$VERSION'/include/'
INST_LIB='/opt/Pt-'$VERSION'/lib/'

chmod a+x lib/*

TMP_BIN=tmpb
TMP_DEV=tmpd

mkdir -p $TMP_BIN$INST_LIB
mv lib/* $TMP_BIN$INST_LIB

mkdir -p $TMP_DEV$INST_INC
mv include/* $TMP_DEV$INST_INC

# Create the RPM packages
SBIN='Platinum (Pt) C++ Framework Binary Package'
SDEV='Platinum (Pt) C++ Framework Development Package'
DESC=$'Platinum (Pt) is a comprehensive C++ framework, which allows developers to\nwrite high-performance applications for many platforms with only one codebase.\nIt provides a large amount of features and is still very easy to use. It\nintergrates well into existing toolkits and frameworks.'

$EXEC_FK_ROOT $EXEC_GEN_RPM             \
    pt-1.0.0$ADDPN-$ARCH $TMP_BIN       \
    --verbose                           \
    --license     'extended LGPL'       \
    --version     "$VERSION"            \
    --release     "$RELEASE"            \
    --group       'C++ Framework'       \
    --package     'Pt Development Team' \
    --vendor      'Pt Development Team' \
    --summary     "$SBIN"               \
    --description "$DESC"

$EXEC_FK_ROOT $EXEC_GEN_RPM             \
    pt-1.0.0$ADDPN-devel-$ARCH $TMP_DEV \
    --verbose                           \
    --license     'extended LGPL'       \
    --version     "$VERSION"            \
    --release     "$RELEASE"            \
    --group       'C++ Framework'       \
    --package     'Pt Development Team' \
    --vendor      'Pt Development Team' \
    --summary     "$SDEV"               \
    --description "$DESC"

cd -

# Move RPM packages to output directoy
mv $RSDIR/*.rpm $OUTDIR

# Clean up
rm -rf $RSDIR
