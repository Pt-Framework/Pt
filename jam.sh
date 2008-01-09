#!/bin/sh

if ! test -f ./jam.bin; then
    echo "no jam.bin"
else
    echo "have jam."
fi

./jam.bin 