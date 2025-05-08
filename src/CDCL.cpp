#include "CDCL.hpp"
#ifdef SAT_USE_CDCL // same guard
#include "Clause.hpp"

// VERY reduced CDCL skeleton: learns unit clause on first conflict
std::optional<bool> cdclStep(Formula& F, Assignment& A) {
  // 1. unit‑propagate until fix‑point -------------------------
  while (auto confl = F.unitPropagate(A)) {
    // conflict!   confl -> ptr to conflicting clause
    if (A.decisionLevel() == 0)
      return false; // UNSAT at root level

    // 2. learn a clause (here: naïve 1‑UIP = neg(decLit))
    Lit decLit = A.lastDecisionLit();
    Clause learnt({neg(decLit)});
    F.addClause(std::move(learnt));

    // 3. back‑jump to level 0 and assert the learnt unit
    A.backjump(0);
    A.assign(neg(decLit), /*dl=*/0, /*antecedent*/ nullptr);
    return {}; // continue search
  }

  // 4. if all vars assigned, SAT; else continue with decision
  if (A.isComplete())
    return true;
  return {}; // search continues
}

#endif
