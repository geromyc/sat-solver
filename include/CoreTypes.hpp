#pragma once
#include <cassert>
#include <cstdlib>

/* ---------- helper types ---------- */
using Lit = int;                              // positive ⇒  x  , negative ⇒  ¬x
inline Lit neg(Lit l) { return -l; }          // returns negated value
inline int var(Lit l) { return std::abs(l); } // returns positive value

/*  sign(l)  – return 1 if the literal is positive, 0 if it is negative      */
inline int sign(Lit l) { return l > 0; }
/*  negLit(v) – build the negative literal for variable v (v > 0)            */
inline Lit negLit(int v) { return -v; }
/* 3‑valued logic - if process timesout*/
enum Val : int { UNK = -1, FALSE = 0, TRUE = 1 };

/* Convenience */
inline Val boolToVal(bool b) { return b ? TRUE : FALSE; }
inline bool isAssigned(Val v) { return v != UNK; }
