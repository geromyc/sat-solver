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
## Putting it all together
structures.h defines your CNF, Lit, Clause, and readDimacs.

solver.h declares your Solver type and its entire public+private interface—importantly including all the little helpers you invoke.

solver.cpp actually implements each method you declared in the header.

main.cpp then does:

CNF F = readDimacs(in);
Solver solver(F);
bool sat = solver.solve();
With that split, each file compiles cleanly, and your build command (for example) becomes:


# How-To-Run
##### Update your local files
```text
git checkout main
git pull
```
git checkout *branch name you are working on*

##### If you don't have the benchmark files in your local directory
```text
mkdir uf20-91
mkdir uf50-218
```
```text
cd src/SATLIB/
tar xzf uf20-91.tar.gz -C uf20-91
```
===OR===
```text
cd src/SATLIB/
tar xzf uf50-218.tar.gz -C uf50-218
```
## Build
# from the project root
```text
g++ -std=c++17 -O3 -Wall -Iinclude src/*.cpp -o mySAT
```

# Run (plain)
./mySAT ../../benchmarks/uf20-01.cnf

# Run with watched literals
SAT_USE_WATCHED=1 ./mySAT ../../benchmarks/uf50-218.cnf

# Run with both watched + CDCL (once you implement CDCL)
SAT_USE_WATCHED=1 SAT_USE_CDCL=1 ./mySAT hard.cnf
