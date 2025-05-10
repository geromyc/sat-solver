#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "CNFParser.hpp"
#include "CoreTypes.hpp"
#include "Formula.hpp"

/** ------------------------------------------------------------------------- *
 * Name of Author(s): Geromy Cunningham                                       *
 *  parseDIMACS                                                               *
 * -------------------------------------------------------------------------- *
 * – tokenises every line with std::istringstream (=> true whitespace split)  *
 * – header “p cnf <vars> <clauses>” is read from *that same line*            *
 * – populates Formula with clauses and records #vars / #clauses inside F     *
 * Input:                                                                     *
 * @param path - Path to the DIMACS file to be parsed                         *
 * Output:                                                                    *
 * @return F - Formula containing the parsed clauses                          *
 * -------------------------------------------------------------------------- */
Formula parseDIMACS(const std::string& path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("cannot open file “" + path + "”");

  Formula F;
  std::string line;
  std::vector<Lit> curClause;

  while (std::getline(in, line)) {
    if (line.empty() || line[0] == 'c') // comment / blank
      continue;

    std::istringstream ls(line);
    std::string tok;
    ls >> tok;

    // --- Header line “p cnf <vars> <clauses>” ---
    if (tok == "p") {
      std::string fmt;
      size_t vars = 0, clauses = 0;
      ls >> fmt >> vars >> clauses;
      if (fmt != "cnf")
        throw std::runtime_error("expected “p cnf …”, got “p " + fmt + " …”");
      F.setVarCount(vars);
      F.reserveClauses(clauses);
      continue;
    }

    // --- EOF marker "%" or "0" in the line ---
    if (tok == "%")
      break;

    // --- Clause Handling ---
    int lit = std::stoi(tok); // first literal already read
    for (;;) {
      if (lit == 0) { // end–of–clause sentinel
        if (!curClause.empty()) {
          F.addClause(Clause(std::move(curClause)));
          curClause.clear();
        }
        break;
      }
      curClause.push_back(lit);
      if (!(ls >> lit)) // no more tokens on this line
        break;
    }
  }

  /* file may omit the final “0” — flush anything buffered                   */
  if (!curClause.empty())
    F.addClause(Clause(std::move(curClause)));

  return F;
}