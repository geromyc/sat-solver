#!/usr/bin/env bash

# Bulk-run mySAT on one or more benchmark folders.
# Usage:  ./bulktest.sh benchmarks/uf20-91 benchmarks/uf50-218
# If no arguments are given, it defaults to every *.cnf under ./benchmarks.

#set -e                     # exit on script error

# NOTE: DLIS > VSIDS if both are enabled
# comment out for base DPLL
# export USE_VSIDS=1
# export USE_LEARNING=1
# export USE_WATCHED_LITS = 1;
# export USE_NONCHRONO=1
# export USE_ACTIVITY_DECAY=1

dirs=("$@")
[[ ${#dirs[@]} -eq 0 ]] && dirs=(benchmarks)

lineWidth=100
count=0  sat=0  unsat=0  err=0

print_result() {
  local cnf="$1"
  if ! out=$(./mySAT "$cnf" 2>&1); then
    printf '?'
    ((err++))
  else
    read -r firstline <<< "$out"
    case "$firstline" in
      RESULT:SAT*  ) printf '.'; ((sat++)) ;;
      RESULT:UNSAT*) printf 'x'; ((unsat++)) ;;
      *            ) printf '?'; ((err++)) ;;
    esac
  fi

  ((count++))
  (( count % lineWidth == 0 )) && echo
}

for dir in "${dirs[@]}"; do
  echo "debug: files found under $dir:"
  mapfile -t cnf_files < <(find "$dir" -type f -name '*.cnf' | sort)
  printf '%s\n' "${cnf_files[@]:0:10}"  # print first 10
  echo "starting solver..."

  for cnf in "${cnf_files[@]}"; do
    print_result "$cnf"
  done
done

echo
echo "Processed $count files  |  SAT=$sat  UNSAT=$unsat  OTHER=$err"
