#include "Clause.hpp"
#include "Assignment.hpp"
#include "Logger.hpp"
#include <algorithm>

bool Clause::isSatisfied(const Assignment& a) const {
  for (Lit l : _lits)
    if (a.valueLit(l) == TRUE)
      return true;
  return false;
}

bool Clause::hasEmpty(const Assignment& a) const {
  for (Lit l : _lits)
    if (a.valueLit(l) != FALSE)
      return false;
  return true;
}

bool Clause::isUnit(const Assignment& a) const {
  Lit unassigned = 0;
  for (Lit l : _lits) {
    Val v = a.valueLit(l);
    if (v == TRUE)
      return false; // satisfied
    if (v == UNK) {
      if (unassigned)
        return false; // >1 unassigned
      unassigned = l;
    }
  }
  return unassigned != 0;
}

Lit Clause::unitLit(const Assignment& a) const {
  for (Lit l : _lits)
    if (a.valueLit(l) == UNK)
      return l;
  return 0; // should not happen
}

/* ---------- watched‑literal helpers (light‑weight) ----------------------------- */
void Clause::watchInit(const Assignment& a, bool useWatched) {
  if (!useWatched || _lits.size() < 2)
    return;
  _wa = 0;
  _wb = 1; // first two literals
  /* ensure at least one watch is currently not FALSE to avoid immediate enqueue */
  if (a.valueLit(_lits[_wa]) == FALSE)
    std::swap(_wa, _wb);
}

bool Clause::onLiteralFalse(Lit falsed,
                            Assignment& asgn,
                            std::vector<Lit>& unitQ,
                            bool useWatched) {
  if (!useWatched || _lits.size() < 2)
    return true;

  /* if we are **not** watching this literal, nothing to do */
  if (_lits[_wa] != falsed && _lits[_wb] != falsed)
    return true;

  /* identify which watch hit */
  int& watch = (_lits[_wa] == falsed ? _wa : _wb);

  /* try to find replacement literal */
  for (size_t k = 0; k < _lits.size(); ++k) {
    if (_wa == -1 || _wb == -1)
      return true;
    if (k == static_cast<size_t>(_wa) || k == static_cast<size_t>(_wb))
      continue;
    if (asgn.valueLit(_lits[k]) != FALSE) {
      watch = static_cast<int>(k); // move watch
      return true;
    }
  }
  /* no replacement: clause now unit or conflict */
  Lit other = _lits[_wa == watch ? _wb : _wa];
  Val ov    = asgn.valueLit(other);
  if (ov == FALSE) {
    return false; // conflict
    Logger::instance().log("Clause::onLiteralFalse: Conflict detected on literal " +
                           std::to_string(other));
  }
  if (ov == UNK) {
    Logger::instance().log("Clause::onLiteralFalse: Clause became unit with literal " +
                           std::to_string(other));
    unitQ.push_back(other); // enqueue
  }
  return true;
}
