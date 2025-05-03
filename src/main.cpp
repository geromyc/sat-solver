#include "CNFParser.hpp"
#include "DPLLSolver.hpp"
#include <iostream>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << "  <file.cnf>\n";
    return 1;
  }
  try {
    Formula F = parseDIMACS(argv[1]);
    DPLLSolver solver(std::move(F));

    bool sat = solver.solve();
    std::cout << (sat ? "SAT\n" : "UNSAT\n");
    if (sat) {
      const auto& M = solver.model().raw();
      for (size_t v = 1; v < M.size(); ++v)
        std::cout << (M[v] == TRUE ? v : -int(v)) << " ";
      std::cout << "0\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 2;
  }
  return 0;
}
