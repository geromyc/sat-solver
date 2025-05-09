#include "Formula.hpp"
#include <algorithm>

size_t Formula::maxVar() const {
  return _numVars ? _numVars          // header told us
                  : std::max_element( // otherwise compute
                        _clauses.begin(), _clauses.end(),
                        [](const Clause& a, const Clause& b) {
                          return a.maxVar() < b.maxVar();
                        })
                        ->maxVar();
}

bool Formula::allSatisfied(const Assignment& a) const {
  for (const auto& c : _clauses)
    if (!c.isSatisfied(a))
      return false;
  return true;
}

bool Formula::hasEmptyClause(const Assignment& a) const {
  for (const auto& c : _clauses)
    if (c.hasEmpty(a))
      return true;
  return false;
}

const Clause* Formula::unitPropagate(Assignment& a) {
  bool changed;
  do {
    changed = false;
    for (const auto& c : _clauses) {
      if (c.isSatisfied(a))
        continue;
      if (c.hasEmpty(a))
        return &c; // Conflict
      if (c.isUnit(a)) {
        Lit l = c.unitLit(a);
        Val v = a.valueLit(l);
        if (v == FALSE)
          return &c; // Conflict
        if (v == UNK) {
          a.pushImplied(l);
          changed = true;
        }
      }
    }
  } while (changed);
  return nullptr; // No conflict
};

/* ------------------------------------------------------------------ *
 *  Pure‑literal collection                                           *
 *    – scans only the clauses that are still UNSAT under A           *
 *    – considers only UNASSIGNED variables                           *
 * ------------------------------------------------------------------ */
std::vector<Lit> Formula::gatherPureLiterals(const Assignment& A) const {
  const size_t n = varCount();
  std::vector<char> seenPos(n + 1, 0), seenNeg(n + 1, 0);

  for (const Clause& c : _clauses) {
    if (c.isSatisfied(A))
      continue; // clause already true → irrelevant
    for (Lit l : c.lits()) {
      const int v = var(l);
      if (A.valueVar(v) != UNK) // variable fixed ⇒ can’t be pure
        continue;
      (l > 0 ? seenPos[v] : seenNeg[v]) = 1;
    }
  }

  std::vector<Lit> pure;
  for (int v = 1; v <= (int)n; ++v) {
    if (A.valueVar(v) != UNK) // skip assigned variables
      continue;
    if (seenPos[v] && !seenNeg[v])
      pure.push_back(v); // only positive occurrences
    else if (seenNeg[v] && !seenPos[v])
      pure.push_back(-v); // only negative occurrences
  }
  return pure;
}

void Formula::initWatches(Assignment& a, bool useWatched) {
  _watchList.assign(varCount() * 2 + 2, {}); // +/‑ for every var
  for (auto& c : _clauses) {
    c.watchInit(a, useWatched);
    if (!useWatched)
      continue;
    _watchList[c.lits()[c.firstWatch()] + varCount()].push_back(&c);
    _watchList[c.lits()[c.secondWatch()] + varCount()].push_back(&c);
  }
}