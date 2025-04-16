/**
 * @author Geromy Cunningham
 * @date 07 APR 2025
 * @file main.cpp
 * @brief Main function for a SAT solver.
 * @details This file contains the main function that initializes the SAT solver, parses
 * the input file, and handles command-line arguments. It also includes signal handling
 * for interruptions and provides statistics about the solving process.
 */
#include <fstream>
#include <iostream>

#include "solver.h"
#include "structures.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <cnf‑file>\n";
    return 1;
  }

  std::ifstream in(argv[1]);
  if (!in) {
    std::cerr << "Cannot open " << argv[1] << "\n";
    return 1;
  }

  // 1) Parse DIMACS into your CNF
  CNF F = readDimacs(in);

  // 2) Construct the solver, passing it the formula
  Solver solver(F);

  // 3) Call your DPLL routine
  bool sat = solver.solve();

  std::cout << (sat ? "SAT\n" : "UNSAT\n");
  return 0;
}