#include <iostream>
#include <sstream>

#include "CNFParser.hpp"
#include "DPLLSolver.hpp"
#include "Formula.hpp"
#include "Logger.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << "  <file.cnf>\n";
    return 1;
  }
  try {
    Formula F = parseDIMACS(argv[1]);
    DPLLSolver solver(std::move(F));
    const bool sat = solver.solve();
    if (!sat) {
      std::cout << "RESULT:UNSAT\n";
      return 0;
    }
    std::cout << "RESULT:SAT\nASSIGNMENT:";
    const auto& M = solver.model().raw(); // 1‑based, M[0] is dummy
    for (size_t v = 1; v < M.size(); ++v)
      std::cout << v << '=' << (M[v] == TRUE ? 1 : 0) << ' ';
    std::cout << '\n';
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 2;
  }
  return 0;
}
