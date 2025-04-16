#ifndef BASIC_SOLVER_H
#define BASIC_SOLVER_H

#include "structures.h"
#include <vector>

class BasicSolver {
  CNF F;
  std::vector<int> value;     // 0=unassigned, +1=true, -1=false
  std::vector<Lit> trail;     // stack of assigned literals
  std::vector<int> trail_lim; // indices in trail where each decision starts

public:
  BasicSolver(const CNF& f) : F(f), value(f.numVars + 1, 0) {}

  // Top‐level DPLL
  bool solve() { return dfs(0); }

private:
  // 1) Naïve Boolean‐Constraint‐Propagation
  bool propagate() {
    bool again;
    do {
      again = false;
      // For each clause...
      for (auto& C : F.clauses) {
        bool sat       = false;
        int unassigned = 0;
        Lit lone{0, false};

        // Scan its literals
        for (auto& l : C.lits) {
          int v = l.var, val = value[v];
          if ((val == 1 && !l.neg) || (val == -1 && l.neg)) {
            sat = true;
            break; // clause already satisfied
          }
          if (val == 0) {
            unassigned++;
            lone = l; // remember last unassigned
          }
        }

        if (sat)
          continue;

        if (unassigned == 0)
          return false; // conflict: no way to satisfy

        if (unassigned == 1) {
          // unit clause → force assignment
          enqueue(lone);
          again = true;
        }
      }
    } while (again);

    return true;
  }

  // 2) Pure‐literal elimination
  void eliminatePureLiterals() {
    std::vector<int> polarity(F.numVars + 1, 0);
    // Mark which vars appear only as +lit or only as -lit
    for (auto& C : F.clauses) {
      for (auto& l : C.lits) {
        if (value[l.var] == 0)
          polarity[l.var] |= (l.neg ? -1 : +1);
      }
    }
    // Enqueue each pure literal
    for (int v = 1; v <= F.numVars; v++) {
      if (value[v] == 0 && polarity[v] != 0) {
        enqueue({v, polarity[v] == -1});
      }
    }
  }

  // Helper to assign a literal and push it on the trail
  void enqueue(Lit p) {
    value[p.var] = (p.neg ? -1 : +1);
    trail.push_back(p);
  }

  // Chronological backtrack to previous decision level
  void backtrack(int lvl) {
    int limit = trail_lim[lvl];
    for (int i = (int)trail.size() - 1; i >= limit; --i) {
      value[trail[i].var] = 0;
      trail.pop_back();
    }
    trail_lim.resize(lvl);
  }

  // Pick the next unassigned variable (first‐fit)
  int pickBranchVar() const {
    for (int v = 1; v <= F.numVars; v++)
      if (value[v] == 0)
        return v;
    return -1; // all assigned
  }

  // Recursive DPLL with chronological backtracking
  bool dfs(int level) {
    eliminatePureLiterals();

    if (!propagate())
      return false;

    // Check for completion
    if (pickBranchVar() < 0)
      return true;

    // Make a new decision
    int decisionVar = pickBranchVar();
    trail_lim.push_back(trail.size());

    // Try true first
    enqueue({decisionVar, false});
    if (dfs(level + 1))
      return true;

    // Backtrack & try false
    backtrack(level);
    trail_lim.push_back(trail.size());
    enqueue({decisionVar, true});
    if (dfs(level + 1))
      return true;

    // Backtrack again: neither branch worked
    backtrack(level);
    return false;
  }
};

#endif // BASIC_SOLVER_H
