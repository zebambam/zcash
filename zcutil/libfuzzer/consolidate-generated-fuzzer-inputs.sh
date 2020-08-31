#!/bin/bash
set -e

typefile="$1"

# You must run this script from the root of the repository.
cd src

while read type; do
    fulltype="$type"
    # Remove namespaces that won't be in the definition.
    type=$(echo "$type" | sed 's/libzcash:://g')
    fuzzname="gen-$type"


    if [ -d "./fuzzing/$fuzzname/" ]; then
        cd ..

        echo ""
        echo ""
        echo ""
        echo ""
        echo ""
        echo "###########################################################"
        echo "###########################################################"
        echo "###########################################################"
        echo "###########################################################"
        echo "Building [$fuzzname]..."
        ./zcutil/libfuzzer/libfuzzer-build.sh fuzzer -s address,undefined -f "$fuzzname"

        echo ""
        echo ""
        echo ""
        echo ""
        echo ""
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        echo "Testing and consolidating inputs for [$fuzzname]..."
        ./src/zcashd -merge=1 -rss_limit_mb=30720 "./src/fuzzing/$fuzzname/input" "./src/fuzzing/$fuzzname/tmp_all_inputs/"

        cd src
    else
        echo "Type [$fulltype] has no directory under ./src/fuzzing"
    fi

done < "$typefile"
