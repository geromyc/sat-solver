#include <optional>

#include "Assignment.hpp"
#include "CDCL.hpp"
#include "Clause.hpp"
#include "Formula.hpp"
#include "Heuristics.hpp"
#include "Logger.hpp"

// VERY reduced CDCL skeleton: learns unit clause on first conflict
std::optional<bool> cdclStep(Formula& F, Assignment& A) {
  // conflict!   confl -> ptr to conflicting clause
  while (F.unitPropagate(A)) {
    if (A.decisionLevel() == 0)
      return false; // UNSAT at root level

    // learn negation of last decision as a unit clause
    Lit decLit = A.lastDecisionLit();
    Clause learnt({neg(decLit)});
    F.addClause(std::move(learnt));
    Logger::instance().log("CDCL::cdclStep: Learnt clause with literal " +
                           std::to_string(neg(decLit)) + ", backjumping to level 0");

    vsidsBump(learnt.lits());

    // non-chronological backjump to level-0 and assert learnt unit
    A.backjump(0);
    A.assign(neg(decLit), 0, nullptr);
  }

  // if all vars assigned, SAT; else continue with decision
  return A.isComplete() ? std::optional<bool>{true} // SAT
                        : std::nullopt;             // continue search
}
