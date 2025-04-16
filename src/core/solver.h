#ifndef SOLVER_H
#define SOLVER_H

#include "structures.h"
#include <utility>
#include <vector>

struct Solver {
  // Reference to the formula we're going to solve
  CNF& F;

  // 0 = unassigned,  1 = true,  -1 = false
  std::vector<int> value;

  // The assignment trail: we record each literal we enqueue
  std::vector<int> trail;
  std::vector<int> trail_lim; // decision‐level markers

  // For each clause, which two positions are we “watching”?
  std::vector<std::pair<int, int>> watches;

  // ctor: initialize all those vectors to the right size
  Solver(CNF& f);

  // Enqueue a literal (assign it), return false if that causes an
  // immediate conflict (i.e. a clause becomes unsatisfiable).
  bool enqueue(Lit p);

  // Boolean‐Constraint‐Propagation: process everything on the trail
  // since the last decision.  Returns false if a conflict is found.
  bool propagate();

  // Make a new decision (increase decision level, pick an unassigned var)
  void decide();

  // Undo assignments back to the previous decision point
  void backtrack();

  // The top‐level DPLL driver
  bool solve();

private:
  // Helpers used inside propagate() and enqueue():
  Lit decode(int lit_code);
  bool tryWatch(int clause_index, int watched_pos);
  Lit getUnit(int clause_index, int other_watched);
  int varOf(int lit_code);
  bool allAssigned();
};

#endif // SOLVER_H
