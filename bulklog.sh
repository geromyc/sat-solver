#!/usr/bin/env bash
# ------------------------------------------------------------
# bulklog.sh  –  run mySAT on every CNF below benchmarks/all_cnf
# one combined log per benchmark folder.
# ------------------------------------------------------------
set -uo pipefail            # keep -u and pipefail, but drop -e

ROOT="benchmarks/all_cnf"

# ---- solver feature flags -----------------------------------
export SAT_USE_WATCHED=1
export SAT_USE_CDCL=1
export SAT_USE_VSIDS=1
# export SAT_USE_DLIS=1
# -------------------------------------------------------------

find "$ROOT" -mindepth 1 -maxdepth 1 -type d | sort |
while read -r folder; do
    SECONDS=0
    echo "$folder"
    echo "starting solver..."

    logFile="$folder/$(basename "$folder").log"
    : > "$logFile"                       # truncate previous run

    count=0 sat=0 unsat=0 err=0

    find "$folder" -type f -name '*.cnf' | sort |
    while read -r cnf; do
        # run solver, capture *all* output, even on crash
        out=$(./mySAT "$cnf" 2>&1 || true)

        {
          echo "===== $(basename "$cnf") ====="
          printf '%s\n\n' "$out"
        } >> "$logFile"

        if   grep -q '^RESULT:SAT'   <<< "$out"; then ((sat++))
        elif grep -q '^RESULT:UNSAT' <<< "$out"; then ((unsat++))
        else                                  ((err++))
        fi
        ((count++))
    done
    elapsed=$SECONDS
    echo "Processed $count files  |  SAT=$sat  UNSAT=$unsat  OTHER=$err |  time=$elapsed"
done