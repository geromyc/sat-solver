#pragma once
#include "Assignment.hpp"
#include "Clause.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

class Formula {
public:
  void addClause(const Clause& c) { _clauses.push_back(c); }

  size_t numClauses() const { return _clauses.size(); }
  size_t maxVar() const;

  bool allSatisfied(const Assignment&) const;
  bool hasEmptyClause(const Assignment&) const;

  /* ---- helpers for algorithms ------------------------------------------------ */
  void initWatches(Assignment&, bool useWatched);

  /* pure literal elimination – returns the vector of pure literals ------------- */
  std::vector<Lit> gatherPureLiterals(const Assignment&) const;
  std::vector<Clause>& clauses() { return _clauses; }

  Clause& operator[](size_t i) { return _clauses[i]; }
  const Clause& operator[](size_t i) const { return _clauses[i]; }

  const std::vector<Clause>& clauses() const { return _clauses; }

  // used by DLIS/VSIDS
  inline size_t varCount() const { return maxVar(); }

  std::optional<const Clause*> unitPropagate(Assignment& a) {
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
    return std::nullopt; // No conflict
  }

private:
  std::vector<Clause> _clauses;
};
