# sat-solver
ECE51216 Group Project - Satisfiability solver with heuristic implementation of watched literals and non-chronological backtracking (CDCL optional).

# Team Members
* Geromy Cunningham
* Jonathan Lloyd
* Adam Stefanyk

# Developer Notes
#### Function Comment Format (javadoc)
```text
/**
* Name of Author(s):
* Detailed description of what function's purpose is
*
* Input:
* @param Name of Input 1 Description of Input 1
* @param Name of Input 2 Description of Input 2
* @return Description of return value */
```
# How-To-Run
##### Update your local files
```text
git checkout origin/main
git pull
git checkout *branch name you are working on*
```

##### If you don't have the benchmark files unzipped
```text
mkdir -p sat-solver/benchmarks/{uf20-91,uf50-218}
cd benchmarks/
```
```text
tar xzf uf20-91.tar.gz -C uf20-91
```
```text
tar xzf uf50-218.tar.gz -C uf50-218
```

## Build
### From the project root
```text
g++ -std=c++17 -O3 -Wall -Iinclude src/*.cpp -o mySAT
```
### Run (plain)
```text
./mySAT benchmarks/uf20-91/uf20-01.cnf
```
### watched literals (export before run)
```text
export SAT_USE_WATCHED=1
./mySAT benchmarks/uf50-218/uf50-01.cnf
```
### Run bulk script
```text
./bulktest.sh
```
### Bulk script w/time
```text
time ./bulktest.sh
```
