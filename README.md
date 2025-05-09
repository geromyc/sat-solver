# sat-solver
ECE51216 Group Project - Satisfiability solver with heuristic implementation of watched literals and non-chronological backtracking (CDCL optional).

# Team Members
* Geromy Cunningham
* Jonathan Lloyd
* Adam Stefanyk

# How-To-Run
##### Update your local files (from remote/GitHub)
```text
git checkout main
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
./mySAT benchmarks/foldername/filename
```
Within the bulk script, you can add/remove the # in front of the export environment variables.
Alternatively you can highlight them all and press ctrl+/ to comment/uncomment them all.
### Run bulk script
```text
./bulktest.sh benchmarks/foldername
```
### Bulk script w/time
The time pre-environment variable can be used for single cases as well, but will likely show up better within bulk runs.
```text
time ./bulktest.sh benchmarks/foldername
```

# Heuristics (export before run)
0 = default/off   ;   1 = on

##### enable watched literals only
SAT_USE_WATCHED=1 ./mySAT example.cnf

##### baseline + DLIS decision heuristic
SAT_USE_DLIS=1 ./mySAT example.cnf

##### baseline + VSIDS
SAT_USE_VSIDS=1 ./mySAT example.cnf

##### watched literals + VSIDS + CDCL
SAT_USE_WATCHED=1 SAT_USE_VSIDS=1 SAT_USE_CDCL=1 ./mySAT example.cnf

# Developer Notes
### Logger
Each run gets its own logfile — filename includes date + time down
to seconds, so consecutive bulk runs never overwrite each other.

2. You can route different solver modes (base / watched / CDCL / VSIDS) to
separate log files just by exporting SAT_LOG_TAG=<tag> before you run.
tag becomes part of the filename.

##### base DPLL
SAT_LOG_TAG=base  ./bulktest.sh benchmarks/all_cnf/uf20-91

##### watched literals only
SAT_USE_WATCHED=1 SAT_LOG_TAG=watched ./bulktest.sh …

##### watched + CDCL + VSIDS
SAT_USE_WATCHED=1 SAT_USE_CDCL=1 SAT_USE_VSIDS=1 \
SAT_LOG_TAG=cdcl_vsids ./bulktest.sh …


### Function Comment Format (javadoc)
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
