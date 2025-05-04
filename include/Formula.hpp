#pragma once
#include "Assignment.hpp"
#include "Clause.hpp"
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
  void collectClausesWithLit(Lit l, std::vector<size_t>& idxOut) const;

  /* pure literal elimination – returns the vector of pure literals ------------- */
  std::vector<Lit> gatherPureLiterals(const Assignment&) const;
  std::vector<Clause>& clauses() { return _clauses; }

  Clause& operator[](size_t i) { return _clauses[i]; }
  const Clause& operator[](size_t i) const { return _clauses[i]; }

  const std::vector<Clause>& clauses() const { return _clauses; }
  // used by DLIS/VSIDS
  inline size_t varCount() const { return maxVar(); }
  
private:
  std::vector<Clause> _clauses;
};
