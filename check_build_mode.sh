#!/usr/bin/env bash

TARGET=$1
DEBUG=$2

BUILD_MODE=$([ $DEBUG -eq 0 ] && echo release || echo debug)

if [[ ! -f $TARGET ]]; then
    echo $TARGET
    exit
fi

# Exit code is 0 if asan is found, which means its not in release mode, and vice versa
RELEASE_MODE=$(grep __asan_init $TARGET > /dev/null; echo $?)

if [ $RELEASE_MODE -eq $DEBUG ]; then
    echo build_mode_mismatch_${BUILD_MODE}
else
    echo $TARGET
fi