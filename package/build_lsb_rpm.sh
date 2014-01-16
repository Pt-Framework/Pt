#!/bin/bash

# Executable for making LSB RPM
EXEC_GEN_RPM='/opt/lsb/bin/makelsbpkg'

# Final installation directories
INST_INC='/opt/lsbpt/include/'
INST_LIB='/opt/lsbpt/lib/'

# Go up to the parent directory
cd ..

# Determine the installation directory under ('deploy') and the additional package name
ADDPN=''
SSDIR='release'
RSDIR='deploy/release/'
DEBUG=`cat Jamrules | grep build/debug`
if [[ $DEBUG != 'x' ]]; then
    SSDIR='debug'
    RSDIR='deploy/debug/'
    ADDPN='-debug'
fi

# Determine the architecture
ARCH=`cat Jamrules | grep TARGET_OSPLAT | awk '{print $3}' | awk -F'"' '{print $2}'`

# Invoke the install command
./jam.sh install

# Ensure all shared libraries have executable permission
cd $RSDIR
chmod a+x lib/*

# Mimic the final installation directory structure
TMP_BIN='tmpb'
TMP_DEV='tmpd'

rm -rvf $TMP_BIN
mkdir -p $TMP_BIN$INST_LIB
mv lib/* $TMP_BIN$INST_LIB

rm -rvf $TMP_DEV
mkdir -p $TMP_DEV$INST_INC
mv include/* $TMP_DEV$INST_INC

# Create the RPM packages
SBIN='Platinum (Pt) C++ Framework Binary Package'
SDEV='Platinum (Pt) C++ Framework Development Package'
DESC=$'Platinum (Pt) is a comprehensive C++ framework, which allows developers to\nwrite high-performance applications for many platforms with only one codebase.\nIt provides a large amount of features and is still very easy to use. It\nintergrates well into existing toolkits and frameworks.'

rm -f *.rpm

$EXEC_GEN_RPM                           \
    pt-1.0.0$ADDPN-$ARCH $TMP_BIN       \
    --verbose                           \
    --license     'LGPL'                \
    --version     '1.0.0'               \
    --release     '1'                   \
    --group       'C++ Framework'       \
    --package     'Pt Development Team' \
    --vendor      'Pt Development Team' \
    --summary     "$SBIN"               \
    --description "$DESC"

$EXEC_GEN_RPM                           \
    pt-1.0.0$ADDPN-devel-$ARCH $TMP_DEV \
    --verbose                           \
    --license     'LGPL'                \
    --version     '1.0.0'               \
    --release     '1'                   \
    --group       'C++ Framework'       \
    --package     'Pt Development Team' \
    --vendor      'Pt Development Team' \
    --summary     "$SDEV"               \
    --description "$DESC"

mv *.rpm ../../package

# Clean-ups
cd ..
rm -rvf $SSDIR
