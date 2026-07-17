#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Run like: $0 prog dir"
    exit
fi

prog="$1"
if [ "$(basename -- "$1")" = "$1" ]; then
    prog="./"$1
fi

for f in "$2"/*.in; do
    base="$(basename -- "$f" .in)"
    time_limit=2
    if [ "$(expr "$base" : ".*100.*")" -ne 0 ]; then
        time_limit=3
    fi

    out="$(mktemp)"
    err="$(mktemp)"

    timeout "$time_limit" valgrind --leak-check=full -q "$prog" <"$f" >"$out" 2>"$err"
    ret_val=$?
    if [ $ret_val -ne 0 ]; then
        echo "Test $base fail"
        rm "$out" "$err"
        continue
    fi

    if ! diff --color=auto "${2}/${base}.out" "$out"; then
        echo "Test $base fail: .out diff"
        rm "$out" "$err"
        continue
    fi
    rm "$out"

    if ! diff --color=auto "${2}/${base}.err" "$err"; then
        echo "Test $base fail: .err diff"
        rm "$err"
        continue
    fi
    rm "$err"

    echo "Test $base OK"
done
