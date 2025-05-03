#include "CNFParser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Formula parseDIMACS(const std::string& path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("cannot open file " + path);

  Formula F;
  std::string tok;
  size_t expectedClauses = 0, parsedClauses = 0;

  while (in >> tok) {
    if (tok == "c") { // comment
      std::getline(in, tok);
    }
    else if (tok == "p") {
      std::string fmt;
      size_t vars;
      in >> fmt >> vars >> expectedClauses;
    }
    else { // first literal
      std::vector<Lit> lits;
      Lit lit = std::stoi(tok);
      while (lit != 0) {
        lits.push_back(lit);
        in >> lit;
      }
      F.addClause(Clause(std::move(lits)));
      ++parsedClauses;
    }
  }
  if (expectedClauses && expectedClauses != parsedClauses) {
    std::cerr << "warning: header says " << expectedClauses << " clauses, but parsed "
              << parsedClauses << "\n";
  }
  return F;
}
