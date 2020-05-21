#!/usr/bin/env bash
#   ./zcutil/libfuzzer/libfuzzer-build.sh <fuzz case>

set -xeu -o pipefail

FUZZ_CASE="$1"
shift 
export FUZZER_LOG_DIR="$(pwd)"
export ZCUTIL=$(realpath "./zcutil")

# overwrite the Linux make profile to use clang instead of GCC:

cat $ZCUTIL/../depends/hosts/linux.mk | sed -e 's/=gcc/=clang-7/g' | sed -e 's/=g++/=clang++-7/g' > x
mv x $ZCUTIL/../depends/hosts/linux.mk

# don't modify the filesystem if we're only building dependencies
# this is to optimize docker layers

if [ "$FUZZ_CASE" = "dependsonly" ]
then
  # make an empty fuzz file just so we can build dependencies 
  > src/fuzz.cpp
  export BUILD_STAGE=dependsonly
else
  cp "./src/fuzzing/$FUZZ_CASE/fuzz.cpp" src/fuzz.cpp
fi

CONFIGURE_FLAGS="--enable-tests=no --disable-bench" "$ZCUTIL/build.sh" -j$(nproc) "CC=$ZCUTIL/libfuzzer/zcash-wrapper-clang" "CXX=$ZCUTIL/libfuzzer/zcash-wrapper-clang++" "$@"
