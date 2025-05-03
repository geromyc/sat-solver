#include "CNFParser.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

/* Helper converts a token to int if it is numeric ---------------------------- */
static bool toInt(const std::string& tok, int& out) {
  if (tok.empty())
    return false;
  int sign = 1, idx = 0;
  if (tok[0] == '-' || tok[0] == '+') {
    sign = (tok[0] == '-') ? -1 : 1;
    idx  = 1;
  }
  for (; idx < (int)tok.size(); ++idx)
    if (!std::isdigit(tok[idx]))
      return false;
  out = sign * std::stoi(tok);
  return true;
}

Formula parseDIMACS(const std::string& path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("cannot open file " + path);

  Formula F;
  std::string tok;
  std::vector<Lit> clause;

  size_t expectedClauses = 0, parsedClauses = 0;

  while (in >> tok) {
    if (tok[0] == 'c') { /* comment – skip rest of line */
      std::getline(in, tok);
      continue;
    }
    if (tok[0] == 'p') { /* problem line */
      std::string fmt;
      size_t vars;
      in >> fmt >> vars >> expectedClauses;
      continue;
    }
    if (tok[0] == '%') /* optional EOF marker */
      break;

    int lit;
    if (!toInt(tok, lit)) /* ignore stray words */
      continue;

    if (lit == 0) { /* clause finished */
      if (!clause.empty()) {
        F.addClause(Clause(std::move(clause)));
        clause.clear();
        ++parsedClauses;
      }
    }
    else {
      clause.push_back(lit);
    }
  }

  if (!clause.empty()) { /* last clause if file omits '%' */
    F.addClause(Clause(std::move(clause)));
    ++parsedClauses;
  }

  if (expectedClauses && expectedClauses != parsedClauses)
    std::cerr << "warning: header said " << expectedClauses << " clauses, parsed "
              << parsedClauses << "\n";

  return F;
}
