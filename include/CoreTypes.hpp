#pragma once
#include <cassert>
#include <cstdlib>

/* ---------- tiny, POD‑style helper types ---------- */
using Lit = int; // positive ⇒  x  , negative ⇒  ¬x
inline Lit neg(Lit l) { return -l; }
inline int var(Lit l) { return std::abs(l); }

/* 3‑valued logic */
enum Val : int { UNK = -1, FALSE = 0, TRUE = 1 };

/* Convenience */
inline Val boolToVal(bool b) { return b ? TRUE : FALSE; }
inline bool isAssigned(Val v) { return v != UNK; }
