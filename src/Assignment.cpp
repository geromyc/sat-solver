#include "Assignment.hpp"
#include "Logger.hpp"

Val Assignment::valueLit(Lit l) const {
  Val v = _val[std::abs(l)];
  if (v == UNK) // still unassigned? → UNK
    return UNK;
  // otherwise return v (if literal is positive) or its negation
  return (l > 0) ? v : (v == TRUE ? FALSE : TRUE);
}

void Assignment::pushDecision(Lit l) {
  _levelPos.push_back(_trail.size());
  _trail.push_back(l);
  setLit(l);
  Logger::instance().log("pushDecision(" + std::to_string(l) + ")");
}

void Assignment::pushImplied(Lit l) {
  _trail.push_back(l);
  setLit(l);
  Logger::instance().log("pushImplied(" + std::to_string(l) + ")");
}

/* Assignment.hpp / .cpp  ------------------------------------------- */
void Assignment::fillUnassignedFalse() {
  for (size_t v = 1; v < _val.size(); ++v)
    if (_val[v] == UNK)
      _val[v] = FALSE;
}

void Assignment::backtrackTo(size_t lvl) {
  if (lvl >= _levelPos.size()) {
    Logger::instance().warn("backtrackTo(" + std::to_string(lvl) +
                            ")  ‑‑ clipping to deepest level " +
                            std::to_string(currLevel()));
    lvl = _levelPos.empty() ? 0 : _levelPos.size() - 1; // clamp
  }

  size_t newTrailSz = _levelPos[lvl]; // first lit *of* lvl

  /* undo assignments */
  for (size_t i = _trail.size(); i > newTrailSz; --i)
    _val[var(_trail[i - 1])] = UNK;

  _trail.resize(newTrailSz);
  _levelPos.resize(lvl + 1);
}

Lit Assignment::lastDecisionLit() const {
  return (_levelPos.size() <= 1)
             ? 0
             : _trail[_levelPos.back()]; // first literal of last level
}
