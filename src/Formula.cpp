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