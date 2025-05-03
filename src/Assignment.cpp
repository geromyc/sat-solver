#include "Assignment.hpp"

/* ---------- trail helpers ------------------------------------------------------ */
void Assignment::pushDecision(Lit l) {
  _levelPos.push_back(_trail.size());
  _trail.push_back(l);
  setLit(l);
  _trail.push_back(0); // level marker
}

void Assignment::pushImplied(Lit l) {
  _trail.push_back(l);
  setLit(l);
}

void Assignment::backtrackTo(size_t lvl) {
  assert(lvl < _levelPos.size());
  size_t newTrailSz = _levelPos[lvl];
  for (size_t i = _trail.size(); i > newTrailSz; --i) {
    Lit lit = _trail[i - 1];
    if (lit != 0)
      _val[var(lit)] = UNK;
  }
  _trail.resize(newTrailSz);
  _levelPos.resize(lvl + 1);
}
