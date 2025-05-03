#include "solver.h"

// Constructor
Solver::Solver(CNF& f) : F(f) {
  value.assign(f.numVars + 1, 0);
  watches.assign(f.clauses.size(), {0, 1});
}

// enqueue: assign literal p, push onto trail, return false on conflict
bool Solver::enqueue(Lit p) {
  int code = 2 * p.var + (p.neg ? 1 : 0);
  if (value[p.var] != 0) // already assigned?
    return value[p.var] == (p.neg ? -1 : +1);
  value[p.var] = (p.neg ? -1 : +1);
  trail.push_back(code);
  return true;
}

// decode an integer code back into a Lit
Lit Solver::decode(int lit_code) {
  int v  = lit_code / 2;
  bool n = lit_code % 2;
  return Lit{v, n};
}

// varOf: given the literal code, extract the variable index
int Solver::varOf(int lit_code) { return lit_code / 2; }

// allAssigned: check if every var[1..numVars] has a nonzero value
bool Solver::allAssigned() {
  for (int v = 1; v <= F.numVars; v++)
    if (value[v] == 0)
      return false;
  return true;
}

// tryWatch: try to move one of the two watches forward so that
// the clause remains “alive.”  Return true if we succeeded,
// false if the clause is now unit or conflicting.
bool Solver::tryWatch(int ci, int wpos) {
  auto& C   = F.clauses[ci].lits;
  int other = (wpos ^ 1);
  // If the other watched literal is already true, we’re done:
  Lit o = C[other];
  if ((value[o.var] != (o.neg ? 1 : -1)))
    return true;

  // Otherwise, scan for a new literal to watch:
  for (int i = 0; i < C.size(); i++) {
    if (i == other || i == wpos)
      continue;
    Lit l = C[i];
    if (value[l.var] == 0 || value[l.var] == (l.neg ? 1 : -1)) {
      watches[ci].first  = other;
      watches[ci].second = i;
      return true;
    }
  }
  // No replacement found ⇒ clause is unit or conflict
  return false;
}

// getUnit: we know clause ci has exactly one literal not set to false,
// return that Lit
Lit Solver::getUnit(int ci, int other_watched) {
  auto& C = F.clauses[ci].lits;
  for (auto& l : C) {
    if (value[l.var] == 0)
      return l;
  }
  // Shouldn’t get here unless it’s a conflict—but we’ll return the
  // watched one as a fallback:
  return C[other_watched];
}

// The propagate(), decide(), backtrack(), solve() you already
// wrote verbatim in solver.h
// … just copy their definitions here …
