#include <cstdlib>

#include "DPLLSolver.hpp"
#include "Heuristics.hpp"
#include "Logger.hpp"
#ifdef SAT_USE_CDCL
#include "CDCL.hpp"
#endif

DPLLSolver::DPLLSolver(Formula f)
    : _F(std::move(f)), _useWatched(std::getenv("SAT_USE_WATCHED") != nullptr),
      _useCDCL(std::getenv("SAT_USE_CDCL") != nullptr),
      _useDLIS(std::getenv("SAT_USE_DLIS") != nullptr),
      _useVSIDS(std::getenv("SAT_USE_VSIDS") != nullptr), _lastConflict(false) {
  initHeuristicFlags(); // tell heuristics which flags are on
  _A.resize(_F.maxVar());
  _F.initWatches(_A, _useWatched);
}

/* ------------------------------------------------------------------ *
 *  Top Level Entrypoint                                              *
 * ------------------------------------------------------------------ */
bool DPLLSolver::solve() {
  pureLiteralElimination();
  if (!unitPropagate())
    return false;

  const bool sat = dpll();
  if (sat) // fill any “don’t‑care” holes deterministically
    _A.fillUnassignedFalse();
  return sat;
}

/* ------------------------------------------------------------------ *
 *  Recursive Core                                                    *
 * ------------------------------------------------------------------ */
bool DPLLSolver::dpll() {
  // Logger::instance().log("DPLLSolver::dpll entry: recursion depth = " +
  //                        std::to_string(_A.currLevel()));
  /* ------------------- CDCL single‑step hook -------------------- */
  if (_useCDCL) {
    while (true) {                      // loop until SAT / UNSAT / need decision
      if (auto done = cdclStep(_F, _A)) // returns optional<bool>
        return *done;                   // finished
      if (!unitPropagate()) {           // propagate learned unit(s)
        if (_lastConflict)
          Logger::instance().log("VSIDS bump after conflict at DL=" +
                                 std::to_string(_A.currLevel()));
        continue; // another CDCL round
      }
      break; // no conflict, need decision
    }
  }

  /* ------------------- terminal checks -------------------------- */
  if (_F.allSatisfied(_A)) {
    return true; // No need to check all variables - pure literal elimination is part of
                 // base DPLL
  }
  if (_F.hasEmptyClause(_A))
    return false;

  /* ------------------- normal DPLL split ------------------------ */
  Lit decision = chooseBranchLiteral();
  size_t lvl   = _A.currLevel();

  _A.pushDecision(decision);
  if (unitPropagate() && dpll())
    return true;

  _A.backtrackTo(lvl);
  _A.pushDecision(neg(decision));
  if (unitPropagate() && dpll()) {
    Logger::instance().log("DPLLSolver::dpll: Backtracking to level " +
                           std::to_string(lvl) + " after decision " +
                           std::to_string(decision));
    return true;
  }
  _A.backtrackTo(lvl);
  return false;
}

/* ------------------------------------------------------------------ *
 *  Pure-Literal Elimination                                          *
 * ------------------------------------------------------------------ */
void DPLLSolver::pureLiteralElimination() {
  bool changed;
  do {
    changed   = false;
    auto pure = _F.gatherPureLiterals(_A);
    for (Lit l : pure) {
      if (_A.valueLit(l) == UNK) {
        Logger::instance().log(
            "DPLLSolver::pureLiteralElimination: Forcing pure literal " +
            std::to_string(l));
        _A.pushImplied(l);
        changed = true;
      }
    }
  } while (changed);
}

/* ------------------------------------------------------------------ *
 *  Boolean Constraint Propagation                                    *
 *  sets _lastConflict so VSIDS can bump scores on conflict           *
 * ------------------------------------------------------------------ */
bool DPLLSolver::unitPropagate() {
  _lastConflict = false;

  if (_useWatched) { /* --- fast watched‑literal loop --- */
    for (auto& c : _F.clauses())
      if (c.isUnit(_A))
        _unitQ.push_back(c.unitLit(_A));

    while (!_unitQ.empty()) {
      Lit l = _unitQ.back();
      _unitQ.pop_back();
      Val v = _A.valueLit(l);
      if (v == TRUE)
        continue;
      else if (v == FALSE) {
        _lastConflict = true;
        // Logger::instance().log(
        //     "DPLLSolver::unitPropagate: Conflict detected, _lastConflict set to TRUE");
        return false;
      }

      _A.pushImplied(l);
      for (auto& c : _F.clauses())
        if (!c.onLiteralFalse(neg(l), _A, _unitQ, SAT_USE_WATCHED)) {
          _lastConflict = true;
          // Logger::instance().log("DPLLSolver::unitPropagate: Conflict detected while "
          //                        "processing clause, _lastConflict set to TRUE");
          return false;
        }
    }
    return true;
  }

  /* -------- O(n*m) fallback when watched literals disabled -------- */
  while (true) {
    bool any = false;
    for (const Clause& c : _F.clauses()) {
      if (c.isSatisfied(_A))
        continue;
      if (c.hasEmpty(_A)) {
        _lastConflict = true;
        Logger::instance().log("DPLLSolver::unitPropagate: Conflict detected in fallback "
                               "loop, _lastConflict set to TRUE");
        return false;
      }
      if (c.isUnit(_A)) {
        Lit l = c.unitLit(_A);
        Val v = _A.valueLit(l);
        if (v == FALSE) {
          _lastConflict = true;
          Logger::instance().log("DPLLSolver::unitPropagate: Conflict detected on unit "
                                 "literal in fallback, _lastConflict set to TRUE");
          return false;
        }
        if (v == UNK) {
          _A.pushImplied(l);
          any = true;
        }
      }
    }
    if (!any)
      return true;
  }
}

Lit DPLLSolver::chooseBranchLiteral() const {
  Lit chosen = 0;
  if (_useDLIS)
    chosen = chooseLiteral_DLIS(_F, _A);
  else if (_useVSIDS)
    chosen = chooseLiteral_VSIDS(_F, _A, _lastConflict);
  else
    chosen = chooseLiteral_DPLL(_F, _A);
  // Logger::instance().log("DPLLSolver::chooseBranchLiteral: Chosen literal " +
  //                        std::to_string(chosen));
  return chosen;
}
