#!/usr/bin/env bash
set -e        # fail fast on any command error

export SAT_USE_WATCHED=1        # comment out for naïve mode
pass=1

for f in benchmarks/uf20-91/*.cnf; do
    if ./mySAT "$f" | grep -q '^SAT'; then
        printf '.'               # progress indicator
    else
        echo -e "\nFAILED on $f"
        pass=0
        break
    fi
done

echo
((pass)) && echo "All tests passed!"
