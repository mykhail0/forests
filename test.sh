#!/bin/bash

for f in "$2"/*.in; do
    out=$(mktemp)
    err=$(mktemp)
    base="$(basename -- "$f" .in)"
    prog="$1"
    if [ "$(basename -- "$1")" = "$1" ]; then
        prog="./"$1
    fi

    if ! <"$f" valgrind --leak-check=full -q "$prog" >"$out" 2>"$err"; then
        echo "Test $base: FAIL"
        rm "$out" "$err"
        continue
    fi

    if ! diff --color=auto "${2}/${base}.out" "$out"; then
        echo "Test $base: FAIL"
        rm "$out" "$err"
        continue
    fi
    rm "$out"

    if ! diff --color=auto "${2}/${base}.err" "$err"; then
        echo "Test $base: FAIL"
        rm "$err"
        continue
    fi
done
