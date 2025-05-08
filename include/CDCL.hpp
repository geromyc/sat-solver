#ifdef SAT_USE_CDCL // only expose when we ask for it
#pragma once
#include <optional>

#include "Assignment.hpp"
#include "Formula.hpp"

// Perform one CDCL‐style “search step”.
// Returns true  if the instance is proven SAT,
//         false if UNSAT,
//         std::nullopt (or throws) if search must continue.
std::optional<bool> cdclStep(Formula& F, Assignment& A);
#endif
