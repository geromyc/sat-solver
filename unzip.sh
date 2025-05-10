#!/usr/bin/env bash
set -euo pipefail

DEST="./benchmarks/all_cnf"        # final collection folder
mkdir -p "$DEST"

# find every *.tar or *.tar.gz archived under ~/benchmarks
find "$HOME/benchmarks" -maxdepth 1 -type f \( -name "*.tar" -o -name "*.tar.gz" \) |
while read -r archive; do
    base=$(basename "$archive")        # e.g. uf20-91.tar.gz
    sub="${base%%.*}"                  # e.g. uf20-91
    work="$DEST/$sub"                  # extract here
    mkdir -p "$work"
    echo "‣ extracting $base → $work"
    tar -xf "$archive" -C "$work"
done

echo "✔  All archives extracted into: $DEST"