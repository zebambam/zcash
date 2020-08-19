#!/bin/bash
set -e

typefile="$1"

# You must run this script from the root of the repository.
cd src

# Find out where the types are declared.

# 0-argument constructors
while read type; do
    # Remove namespaces that won't be in the definition.
    type=$(echo "$type" | sed 's/libzcash:://g')

    >&2 echo "Looking for class declarations for [$type]..."
    # We only want the header that has the actual declaration.
    ack "class $type" -C50 | grep "$type()" || true

done < $typefile > ./intermediate-simple-classes.txt

# structs
while read type; do
    # Remove namespaces that won't be in the definition.
    type=$(echo "$type" | sed 's/libzcash:://g')

    >&2 echo "Looking for struct declarations for [$type]..."
    # We only want the header that has the actual declaration.
    ack "struct $type" -C5 | grep "struct $type\\( \\|$\\)" || true

done < "$typefile" > ./intermediate-structs.txt


# Now actually generate the fuzzers.
while read type; do
    fulltype="$type"
    # Remove namespaces that won't be in the definition.
    type=$(echo "$type" | sed 's/libzcash:://g')

    # These regexes must match the ones above (union of the two)
    line=$(cat ./intermediate-simple-classes.txt ./intermediate-structs.txt | grep "$type()\\|$type\\( \\|$\\)" | head -n 1)
    headerpath=$(echo "$line" | cut -d '-' -f 1 | cut -d ':' -f 1)
    if [ -z "$headerpath" ]; then
        >&2 echo "No 0-argument constructor for [$type] was found."
    else
        >&2 echo ""
        >&2 echo "Generating fuzzer for [$type]..."
        >&2 echo "    Found header [$headerpath] for type [$type]"
        >&2 echo "        Based on source code line: [$line]"
        >&2 echo ""
        
        fuzzname="gen-$type"

        mkdir "./fuzzing/$fuzzname"
        mkdir "./fuzzing/$fuzzname/input"
        touch "./fuzzing/$fuzzname/input/__empty__"

        cp "./fuzzing/templates/deserialize-template.cpp" "./fuzzing/$fuzzname/fuzz.cpp"
        sed -i "s#subst_headerfile#$headerpath#" "./fuzzing/$fuzzname/fuzz.cpp"
        sed -i "s#subst_typename#$fulltype#" "./fuzzing/$fuzzname/fuzz.cpp"
    fi
done < "$typefile"
