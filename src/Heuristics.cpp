#include <cstdlib>
#include <vector>

#include "CoreTypes.hpp"
#include "Heuristics.hpp"

/* ------------------------------------------------------------ *
 *  global flags (set once at start‑up)                          *
 * ------------------------------------------------------------ */
static bool g_useDLIS  = false;
static bool g_useVSIDS = false;

void initHeuristicFlags() {
  g_useDLIS  = std::getenv("SAT_USE_DLIS") != nullptr;
  g_useVSIDS = std::getenv("SAT_USE_VSIDS") != nullptr;
}

/* ------------------------------------------------------------ *
 *  Plain DPLL fallback : first unassigned literal               *
 * ------------------------------------------------------------ */
Lit chooseLiteral_DPLL(const Formula& F, const Assignment& A) {
  for (const auto& c : F.clauses())
    for (Lit l : c.lits())
      if (A.valueVar(var(l)) == UNK)
        return l;
  return 0; // should never happen
}

/* ------------------------------------------------------------ *
 *  DLIS  (count positive / negative occurrences)                *
 * ------------------------------------------------------------ */
Lit chooseLiteral_DLIS(const Formula& F, const Assignment& A) {
  std::vector<int> pos(F.varCount() + 1, 0), neg(pos);

  for (const Clause& c : F.clauses()) {
    if (c.isSatisfied(A))
      continue;
    for (Lit l : c.lits()) {
      if (A.valueVar(var(l)) == UNK)
        (sign(l) ? pos : neg)[var(l)]++;
    }
  }
  int bestVar = 0, bestCnt = -1, bestPol = 1;
  for (int v = 1; v <= static_cast<int>(F.varCount()); ++v) {
    if (A.valueVar(v) != UNK)
      continue;
    if (pos[v] > bestCnt) {
      bestCnt = pos[v];
      bestVar = v;
      bestPol = 1;
    }
    if (neg[v] > bestCnt) {
      bestCnt = neg[v];
      bestVar = v;
      bestPol = 0;
    }
  }
  return bestPol ? Lit(bestVar) : negLit(bestVar);
}

/* ------------------------------------------------------------ *
 *  VSIDS  (two‑score arrays, exponential decay)                 *
 * ------------------------------------------------------------ */
static std::vector<double> vsScore;
static double decay = 0.95, bump = 1.0;

static void bumpVSIDS(Lit l) {
  vsScore[var(l)] += bump;
  bump /= decay; // grow bump over time
}

Lit chooseLiteral_VSIDS(const Formula& F, const Assignment& A, bool lastConflict) {
  if (vsScore.empty())
    vsScore.assign(F.varCount() + 1, 0.0);

  if (lastConflict) {
    for (int v = 1; v <= static_cast<int>(F.varCount()); ++v)
      if (A.valueVar(v) == FALSE)
        bumpVSIDS(Lit(v));
  }

  int best = -1;
  for (int v = 1; v <= static_cast<int>(F.varCount()); ++v)
    if (A.valueVar(v) == UNK && (best == -1 || vsScore[v] > vsScore[best]))
      best = v;

  return Lit(best); // may be 0 if something went wrong
}
