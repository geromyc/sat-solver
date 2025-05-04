#pragma once
#include "Assignment.hpp"
#include "Formula.hpp"
#include <vector>

class DPLLSolver {
public:
  std::vector<Lit> _unitQ; // propagation queue (watched mode)
  explicit DPLLSolver(Formula f);
  bool solve();
  const Assignment& model() const { return _A; }

private:
  /* Recursive DPLL ------------------------------------------------------------- */
  bool dpll();

  /* BCP & helper routines ------------------------------------------------------ */
  bool unitPropagate();
  void pureLiteralElimination();

  Lit chooseBranchLiteral() const;

  /* data ----------------------------------------------------------------------- */
  Formula _F;
  Assignment _A;

  std::vector<Lit> _unitQueue;

  /* run‑time heuristics toggled by env vars ------------------------------------ */
  bool _useWatched;
  bool _useCDCL;
  bool _useDLIS;
  bool _useVSIDS;
  bool _lastConflict; // needed by VSIDS bumping
};
