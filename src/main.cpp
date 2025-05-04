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
    const bool sat    = solver.solve();
    const auto& model = solver.model().raw(); // 0‑index dummy entry

    if (sat) {
      std::cout << "RESULT:SAT\nASSIGNMENT:";
      for (size_t v = 1; v < model.size(); ++v) {
        int bit = (model[v] == TRUE ? 1 : 0);
        std::cout << v << '=' << bit;
        if (v + 1 < model.size())
          std::cout << ' ';
      }
      std::cout << '\n';
    }
    else {
      std::cout << "RESULT:UNSAT\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 2;
  }
  return 0;
}