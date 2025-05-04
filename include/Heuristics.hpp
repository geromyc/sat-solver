#pragma once
#include "Assignment.hpp"
#include "Formula.hpp"

// Return the literal to branch on.
// `lastConflict` is true  if the previous propagation produced a conflict
//                     (needed by VSIDS bumping).
Lit chooseLiteral_DPLL(const Formula&, const Assignment&);

// -- optional heuristics (activated via env var) --
Lit chooseLiteral_DLIS(const Formula&, const Assignment&);
Lit chooseLiteral_VSIDS(const Formula&, const Assignment&, bool lastConflict);

// global initialiser called once from main()
void initHeuristicFlags();
