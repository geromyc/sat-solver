#ifndef SOLVER_H
#define SOLVER_H

#include <vector.h>

struct Solver {
  CNF &F;
  std::vector<int>  value;       // 0=unassigned, 1=true, -1=false
  std::vector<int>  trail;       // stack of assigned literals
  std::vector<int>  trail_lim;   // indices in trail where each decision starts

  // Watched‑literals: for each clause, two watched positions
  std::vector<std::pair<int,int>> watches;

  Solver(CNF &f): F(f) {
      value .assign(f.numVars+1, 0);
      watches.assign(f.clauses.size(), {0,1});
  }

  // Enqueue an assignment, return false on immediate conflict
  bool enqueue(Lit p);

  // Run BCP from the current trail head; return false if conflict
  bool propagate() {
      size_t qhead = trail_lim.empty() ? 0 : trail_lim.back();
      while(qhead < trail.size()){
          Lit p = decode(trail[qhead++]);
          int   opp = 2*p.var + (p.neg?0:1);
          for(int ci: F.occur[opp]){
              auto [w1,w2] = watches[ci];
              int other = (F.clauses[ci].lits[w1] == p ? w2 : w1);
              if (!tryWatch(ci, w1==other ? w2 : w1)) {
                  // clause is unit or conflicting
                  if (!enqueue(getUnit(ci, other))) return false;
              }
          }
      }
      return true;
  }

  // Decide next var (e.g. pick first unassigned) and push decision level
  void decide() {
      trail_lim.push_back(trail.size());
      // pick var and assign
  }

  // Backtrack to previous decision level
  void backtrack() {
      int lvl = trail_lim.back(); trail_lim.pop_back();
      for(int i=trail.size()-1; i>=lvl; --i)
          value[varOf(trail[i])] = 0;
      trail.resize(lvl);
  }

  // The main DPLL loop
  bool solve() {
      F.buildOccur();
      while(true) {
          if (!propagate()) {
              if (trail_lim.empty()) return false;
              backtrack();
          } else if (allAssigned()) {
              return true;
          } else {
              decide();
          }
      }
  }
};


#endif