#include "Formula.hpp"
#include <algorithm>

size_t Formula::maxVar() const {
  size_t m = 0;
  for (const auto& c : _clauses)
    for (Lit l : c.lits())
      m = std::max(m, (size_t)var(l));
  return m;
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

void Formula::initWatches(Assignment& a, bool useWatched) {
  for (auto& c : _clauses)
    c.watchInit(a, useWatched);
}

std::vector<Lit> Formula::gatherPureLiterals(const Assignment& a) const {
  std::unordered_map<int, int> mask; // var -> bitmask(positive=1, negative=2)
  for (const auto& c : _clauses) {
    if (c.isSatisfied(a))
      continue;
    for (Lit l : c.lits()) {
      int v = var(l);
      if (a.valueVar(v) != UNK)
        continue;
      int& m = mask[v];
      m |= (l > 0 ? 1 : 2);
    }
  }
  std::vector<Lit> out;
  for (auto& [v, m] : mask) {
    if (m == 1)
      out.push_back(Lit(v)); // only positive
    if (m == 2)
      out.push_back(Lit(-v)); // only negative
  }
  return out;
}
