#pragma once
#include "CoreTypes.hpp"
#include <vector>

class Assignment; /* forward */

class Clause {
public:
  explicit Clause(std::vector<Lit> lits);
  Lit unitLit(const Assignment& a) const; // pre‑condition: isUnit()==true
  bool isSatisfied(const Assignment& a) const;
  bool isUnit(const Assignment& a) const;
  bool hasEmpty(const Assignment& a) const;
  bool deleted() const { return _deleted; }
  void setDeleted(bool b = true) { _deleted = b; }

  /* watched‑literal helpers – a no‑op if feature disabled at run‑time            */
  void watchInit(const Assignment& a, bool useWatched);
  /* returns true  -> clause is satisfied or watch moved
   *         false -> clause became unit or conflict (pushed to unitQ)            */
  bool onLiteralFalse(Lit falsed,
                      Assignment& a,
                      std::vector<Lit>& unitQ,
                      bool useWatched);

  /* low‑level access                                                             */
  const std::vector<Lit>& lits() const { return _lits; }

private:
  bool _deleted = false;
  std::vector<Lit> _lits;
  /* indices into _lits for the two watches (-1 if not used)                      */
  int _wa = -1, _wb = -1;
};
