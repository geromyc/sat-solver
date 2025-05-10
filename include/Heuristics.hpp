#pragma once
#include <vector>

#include "Assignment.hpp"
#include "Formula.hpp"

// global flag initialiser called from solver
void initHeuristicFlags();

// --- Primitive Branch Heuristics ---
Lit chooseLiteral_DPLL(const Formula&, const Assignment&);
// --- optional heuristics (activated via env var) ---
Lit chooseLiteral_DLIS(const Formula&, const Assignment&);
Lit chooseLiteral_VSIDS(const Formula&, const Assignment&, bool lastConflict);

/* called by CDCL or plain DPLL on every learned / conflict clause */
void vsidsBump(const std::vector<Lit>& clauseLits);
