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

std::vector<Lit> Formula::gatherPureLiterals(const Assignment& a) const {
  std::unordered_map<int, int> mask; // var -> bitmask(positive=1, negative=2)

  for (const auto& c : _clauses) {
    // do NOT skip any literal based on assignment:
    for (Lit l : c.lits()) {
      const int v = var(l);
      // Mark that v has appeared with this sign
      int& m = mask[v];
      m |= (l > 0 ? 1 : 2);
    }
  }

  std::vector<Lit> out;
  for (auto& [v, bits] : mask) {
    if (bits == 1)
      out.push_back(+v); // only positive
    if (bits == 2)
      out.push_back(-v); // only negative
  }
  return out;
}

void Formula::initWatches(Assignment& a, bool useWatched) {
  for (auto& c : _clauses)
    c.watchInit(a, useWatched);
}