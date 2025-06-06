# sat-solver
ECE51216 Group Project - Satisfiability solver with heuristic implementation of watched literals and non-chronological backtracking (CDCL optional).

# Team Members
* Geromy Cunningham
* Jonathan Lloyd
* Adam Stefanyk

# How-To-Run
##### Update your local files (from remote/GitHub)
```text
git pull origin
git checkout *branch name you are working on*
```
##### If you don't have the benchmark files unzipped
```text
chmod +x unzip.sh
chmod +x bulklog.sh
chmod +x bulktest.sh
./unzip.sh
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

##### baseline + VSIDS
SAT_USE_VSIDS=1 ./mySAT example.cnf

##### enable watched literals only
SAT_USE_WATCHED=1 ./mySAT example.cnf

##### watched literals + VSIDS + CDCL
SAT_USE_WATCHED=1 SAT_USE_VSIDS=1 SAT_USE_CDCL=1 ./mySAT example.cnf

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
