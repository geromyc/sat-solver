#include <cstdlib>
#include <vector>

#include "CoreTypes.hpp"
#include "Heuristics.hpp"
#include "Logger.hpp"

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
  for (const Clause& c : F.clauses()) {
    if (c.isSatisfied(A))
      continue;
    for (Lit l : c.lits()) {
      if (A.valueVar(var(l)) == UNK) {
        return l; // split on first open literal
      }
    }
  }
  // F is satisfied ⇢ caller should already have returned SAT
  // defensive fallback:
  for (int v = 1; v <= (int)F.varCount(); ++v)
    if (A.valueVar(v) == UNK)
      return Lit(v); // arbitrary
  return 0;
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
 *  VSIDS  (two‑score arrays, exponential decay)                *
 * ------------------------------------------------------------ */
static struct {
  std::vector<double> pos, neg;
  double bump  = 1.0;  // current bump‑magnitude (grows 1/decay each conflict)
  double decay = 0.95; // global decay factor
  int cnt      = 0;    // how many times bumpClause() was called

  void ensure(size_t n) { // enlarge on demand
    if (pos.size() <= n) {
      pos.resize(n + 1, 0);
      neg.resize(n + 1, 0);
    }
  }

  /* bump all literals in the learnt/conflict clause */
  void bumpClause(const std::vector<Lit>& cls) {
    for (Lit l : cls)
      (l > 0 ? pos[l] : neg[-l]) += bump;

    bump /= decay; // next bump will be a bit larger   (MiniSAT style)

    /* decay all variable scores every 100 bumps */
    if (++cnt % 100 == 0) {
      for (double& s : pos)
        s *= decay;
      for (double& s : neg)
        s *= decay;
    }
  }

  Lit pick(const Assignment& A) const { // choose best unassigned literal
    int bestVar = 0, bestPol = 1;
    double best = -1;
    for (int v = 1; v < (int)pos.size(); ++v) {
      if (A.valueVar(v) != UNK)
        continue;
      if (pos[v] > best) {
        best    = pos[v];
        bestVar = v;
        bestPol = 1;
      }
      if (neg[v] > best) {
        best    = neg[v];
        bestVar = v;
        bestPol = 0;
      }
    }
    return bestPol ? Lit(bestVar) : negLit(bestVar);
  }
} VS;

/* called by solver whenever it *learns* a clause (CDCL) or hits a plain
   conflict in chronological DPLL                                          */
void vsidsBump(const std::vector<Lit>& learnt) { VS.bumpClause(learnt); }

Lit chooseLiteral_VSIDS(const Formula& F, const Assignment& A, bool /*unused*/) {
  VS.ensure(F.varCount());
  return VS.pick(A);
}
