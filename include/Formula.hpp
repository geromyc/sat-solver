#include "config.hpp"
#pragma once
#include <optional>
#include <unordered_map>
#include <vector>

#include "Assignment.hpp"
#include "Clause.hpp"
#include "CoreTypes.hpp"
class Formula {
public:
  /* –– construction helpers –– */
  void setVarCount(size_t n) { _numVars = n; }
  void reserveClauses(size_t n) { _clauses.reserve(n); }

  size_t maxVar() const;
  size_t numClauses() const { return _clauses.size(); }
  size_t varCount() const { return _numVars ? _numVars : maxVar(); }
  bool allSatisfied(const Assignment&) const;
  bool hasEmptyClause(const Assignment&) const;

  /* –– clause management –– */
  void addClause(Clause c) {
    if (_numVars < c.maxVar())
      _numVars = c.maxVar(); // auto‑grow
    _clauses.emplace_back(std::move(c));
  }

  /* --- Unit propagation - returns clause if conflict, else null --- */
  const Clause* unitPropagate(Assignment& a);

  /* --- Pure literal elimination – returns the vector of pure literals --- */
  std::vector<Lit> gatherPureLiterals(const Assignment&) const;
  std::vector<Clause>& clauses() { return _clauses; }
  Clause& operator[](size_t i) { return _clauses[i]; }
  const Clause& operator[](size_t i) const { return _clauses[i]; }
  const std::vector<Clause>& clauses() const { return _clauses; }

  /* --- Watched Literals --- */
  void initWatches(Assignment&, bool useWatched);

private:
  size_t _numVars = 0;          // total #variables in instance
  std::vector<Clause> _clauses; // owned clauses
};
