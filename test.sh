#!/bin/bash

# TODO remove breaks
# TODO change SECONDS usage to timeout coreutil
for f in "$2"/*.in; do
    base="$(basename -- "$f" .in)"
    time_limit=2
    if [ "$(expr "$base" : ".*100.*")" -ne 0 ]; then
        time_limit=3
    fi

    out="${2}/${base}.myout"
    err="${2}/${base}.myerr"
    # out="$(mktemp)"
    # err="$(mktemp)"
    prog="$1"
    if [ "$(basename -- "$1")" = "$1" ]; then
        prog="./"$1
    fi

    t0=$SECONDS
    <"$f" valgrind --leak-check=full -q "$prog" >"$out" 2>"$err"
    t1=$SECONDS
    ret_val=$?
    if [ $ret_val -ne 0 ]; then
        echo "Test $base fail: return code"
        break
    fi

    if ! diff --color=auto "${2}/${base}.out" "$out"; then
        echo "Test $base fail: .out diff"
        break
    fi

    if ! diff --color=auto "${2}/${base}.err" "$err"; then
        echo "Test $base fail: .err diff"
        break
    fi
    rm "$out" "$err"

    if [ $((t1 - t0)) -ge "$time_limit" ]; then
        echo "Test $base fail: TLE"
        break
    fi

    echo "Test $base OK"
done
