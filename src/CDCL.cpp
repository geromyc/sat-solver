#include <optional>

#include "Assignment.hpp"
#include "CDCL.hpp"
#include "Clause.hpp"
#include "Formula.hpp"
#include "Heuristics.hpp"

// VERY reduced CDCL skeleton: learns unit clause on first conflict
std::optional<bool> cdclStep(Formula& F, Assignment& A) {
  // conflict!   confl -> ptr to conflicting clause
  while (auto confl = F.unitPropagate(A)) {
    if (A.decisionLevel() == 0)
      return false; // UNSAT at root level

    // learn negation of last decision as a unit clause
    Lit p = neg(A.trail().back()); // literal that just falsified clause
    Clause learnt({p});
    F.addClause(Clause({p}));
    vsidsBump({p});

    // non-chronological backjump to level-0 and assert learnt unit
    A.backjump(0);
    A.assign(p, /*dl=*/0, /*antecedent*/ nullptr);
  }

  // if all vars assigned, SAT; else continue with decision
  return A.isComplete() ? std::optional<bool>{true} // SAT
                        : std::nullopt;             // continue search
}
