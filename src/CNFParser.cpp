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
  std::string line;
  std::size_t expectedClauses = 0, parsedClauses = 0;

  while (std::getline(in, line)) {
    if (line.empty())
      continue; // blank line
    char lead = line[0];
    if (lead == 'c')
      continue; // comment
    if (lead == '%')
      break; // optional EOF marker

    if (lead == 'p') { // problem line
      std::stringstream ss(line);
      char p;
      std::string fmt;
      size_t vars;
      ss >> p >> fmt >> vars >> expectedClauses;
      continue;
    }

    /* a normal clause line -------------------------------------------------- */
    std::stringstream ss(line);
    int lit;
    std::vector<Lit> clauseLits;
    while (ss >> lit && lit != 0)
      clauseLits.push_back(lit);

    if (!clauseLits.empty()) {
      F.addClause(Clause(std::move(clauseLits)));
      ++parsedClauses;
    }
  }

  if (expectedClauses && expectedClauses != parsedClauses)
    std::cerr << "warning: header said " << expectedClauses << " clauses, parsed "
              << parsedClauses << "\n";

  return F;
}
