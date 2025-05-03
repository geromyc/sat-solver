#pragma once
#include "Assignment.hpp"
#include "Formula.hpp"
#include <vector>

class DPLLSolver {
public:
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
  const bool _useWatched;
  const bool _useCDCL; // stub – ready for later extension
};
