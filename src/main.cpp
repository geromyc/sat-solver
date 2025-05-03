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

#include "basic_solver.h"
#include "solver.h"
#include "structures.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
      std::cerr << "Usage: ./sat_solver input.cnf\n";
      return 1;
  }

  CNFParser parser(argv[1]);
  Formula formula = parser.parse();
  Solver solver(formula);

  bool result = solver.solve();

  if (result) {
      std::cout << "RESULT:SAT\n";
      std::cout << "ASSIGNMENT:" << solver.getAssignmentString() << "\n";
  } else {
      std::cout << "RESULT:UNSAT\n";
  }
}
