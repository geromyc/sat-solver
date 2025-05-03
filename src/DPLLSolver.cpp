#include "DPLLSolver.hpp"
#include <cstdlib>

DPLLSolver::DPLLSolver(Formula f)
    : _F(std::move(f)), _useWatched(std::getenv("SAT_USE_WATCHED") ? true : false),
      _useCDCL(std::getenv("SAT_USE_CDCL") ? true : false) {
  _A.resize(_F.maxVar());
  _F.initWatches(_A, _useWatched);
}

/* ------------------------------------------------------------------ *
 *  Top Level Entrypoint                                              *
 * ------------------------------------------------------------------ */
bool DPLLSolver::solve() {
  /* initial pure‑literal elimination + unit propagation */
  pureLiteralElimination();
  if (!unitPropagate())
    return false;

  return dpll();
}

/* ------------------------------------------------------------------ *
 *  Recursive Core                                                    *
 * ------------------------------------------------------------------ */
bool DPLLSolver::dpll() {
  if (_F.allSatisfied(_A))
    return true;
  if (_F.hasEmptyClause(_A))
    return false;

  Lit decision = chooseBranchLiteral();
  size_t lvl   = _A.currLevel();

  _A.pushDecision(decision);
  if (unitPropagate() && dpll())
    return true;

  _A.backtrackTo(lvl);
  _A.pushDecision(neg(decision));
  if (unitPropagate() && dpll())
    return true;

  _A.backtrackTo(lvl);
  return false;
}

/* ------------------------------------------------------------------ *
 *  Utilities                                                         *
 * ------------------------------------------------------------------ */
void DPLLSolver::pureLiteralElimination() {
  bool changed;
  do {
    changed   = false;
    auto pure = _F.gatherPureLiterals(_A);
    for (Lit l : pure) {
      if (_A.valueLit(l) == UNK) {
        _A.pushImplied(l);
        changed = true;
      }
    }
  } while (changed);
}

/* ------------------------------------------------------------------ *
 *  Boolean Constraint Propagation                                    *
 * ------------------------------------------------------------------ */
bool DPLLSolver::unitPropagate() {
  if (_useWatched) { /* fast path with 2‑watched literals */
    /* collect initial units ------------------------------- */
    for (auto& c : _F.clauses())
      if (c.isUnit(_A))
        _unitQueue.push_back(c.unitLit(_A));

    while (!_unitQueue.empty()) {
      Lit l = _unitQueue.back();
      _unitQueue.pop_back();

      Val v = _A.valueLit(l);
      if (v == TRUE)
        continue;
      if (v == FALSE)
        return false; // conflict
      _A.pushImplied(l);

      /* notify all clauses that watched !l disappeared */
      for (auto& c : _F.clauses())
        if (!c.onLiteralFalse(neg(l), _A, _unitQueue, true))
          return false; // conflict
    }
    return true;
  }

  /* ----------------------------------------------------------------
   *  SIMPLE (O(n·m)) PROPAGATION when watched literals are OFF
   * ---------------------------------------------------------------- */
  while (true) {
    bool anyChange = false;

    for (const auto& c : _F.clauses()) {
      if (c.isSatisfied(_A))
        continue;

      /* conflict? */
      if (c.hasEmpty(_A))
        return false;

      /* unit clause? */
      if (c.isUnit(_A)) {
        Lit l = c.unitLit(_A);
        Val v = _A.valueLit(l);

        if (v == FALSE)
          return false; // conflict
        if (v == UNK) {
          _A.pushImplied(l);
          anyChange = true;
        }
      }
    }
    if (!anyChange)
      break; // fixed point reached
  }
  return true;
}

Lit DPLLSolver::chooseBranchLiteral() const {
  /* naive: first UNK variable, positive polarity */
  for (size_t v = 1; v < _A.raw().size(); ++v)
    if (_A.valueVar(v) == UNK)
      return Lit(v);
  return 0; // unreachable if called correctly
}
