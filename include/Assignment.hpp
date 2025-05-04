#pragma once
#include <vector>

#include "Clause.hpp"
#include "CoreTypes.hpp"
class Assignment {
public:
  explicit Assignment(size_t nVars = 0) : _val(nVars + 1, UNK) {}

  void resize(size_t n) { _val.resize(n + 1, UNK); }

  /* query / update parcially assigned literals --------------------------------- */
  Val valueVar(int v) const { return _val[v]; }
  Val valueLit(Lit l) const;

  // alias expected by Heuristics.cpp
  inline Val value(int v) const { return valueVar(v); } // or `_val[v]` if you prefer

  void setLit(Lit l) { _val[var(l)] = l > 0 ? TRUE : FALSE; }

  /* ----- trail management (decision levels) ----------------------------------- */
  void pushDecision(Lit l);          // add decision literal
  void pushImplied(Lit l);           // add implied literal
  void backtrackTo(size_t newLevel); // pop until size == levelPositions[newLevel]
  size_t currLevel() const { return _levelPos.size() - 1; }

  const std::vector<Lit>& trail() const { return _trail; }
  const std::vector<Val>& raw() const { return _val; }

  Lit lastDecisionLit() const {
    if (_levelPos.size() <= 1) // no decisions
      return 0;
    size_t lastDecLevel = _levelPos[_levelPos.size() - 1];
    return _trail[lastDecLevel];
  }

  bool isComplete() const {
    for (size_t v = 1; v < _val.size(); ++v)
      if (_val[v] == UNK)
        return false;
    return true;
  }

  void backjump(size_t newLevel) { backtrackTo(newLevel); }

  void assign(Lit l, size_t dl, const Clause*) {
    if (dl > currLevel()) {
      // Add new level
      _levelPos.push_back(_trail.size());
    }
    _trail.push_back(l);
    setLit(l);
  }

  size_t decisionLevel() const { return currLevel(); }

private:
  std::vector<Val> _val;            // 1‑based index
  std::vector<Lit> _trail;          // signed literals (0 = level marker)
  std::vector<size_t> _levelPos{0}; // index in _trail where each level starts
};
