/**
 * @file main.cpp
 * @brief Main function for a SAT solver.
 * @details This file contains the main function that initializes the SAT solver, parses the input
 *          file, and handles command-line arguments. It also includes signal handling for
 *          interruptions and provides statistics about the solving process.
 * @author Geromy Cunningham
 * @date 07 APR 2025
 */
#include <fstream>
#include <iostream>

#include "Dimacs.h"
#include "solver.h"

int main(int argc, char* argv[]) {
    std::ifstream input_file(argv[1]);
    parse_DIMACS(input_file, Solver &S, (bool)strictp);

    Solver solver;
    solver.load(dimacs);  // method to convert parsed input into internal structures

    bool result = solver.solve();
    std::cout << (result ? "SAT" : "UNSAT") << std::endl;

    return 0;
}
