#!/bin/bash
set -e

typefile="$1"
datafile="$2"

# You must run this script from the root of the repository.
cd src

while read type; do
    fulltype="$type"
    # Remove namespaces that won't be in the definition.
    type=$(echo "$type" | sed 's/libzcash:://g')
    fuzzname="gen-$type"


    if [ -d "./fuzzing/$fuzzname/" ]; then
        echo "Distributing inputs for [$type]..."

        mkdir -p "./fuzzing/$fuzzname/tmp_all_inputs"

        cat "$datafile" | grep "\\[$fulltype\\]" | cut -d "[" -f 3 | cut -d "]" -f 1 > "./fuzzing/$fuzzname/tmp_base64"
        while read base64; do
            filename=$(echo "$base64" | base64 -d | sha256sum)
            echo "$base64" | base64 -d > "./fuzzing/$fuzzname/tmp_all_inputs/$filename"
        done < "./fuzzing/$fuzzname/tmp_base64"
    else
        echo "Type [$fulltype] has no directory under ./src/fuzzing"
    fi

done < "$typefile"