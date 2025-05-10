#include "config.hpp"
#pragma once
#include <string>

#include "Formula.hpp"

/* Reads a DIMACS‑CNF file and returns a populated Formula object. */
Formula parseDIMACS(const std::string& path);
