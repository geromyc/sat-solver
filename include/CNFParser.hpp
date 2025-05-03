#pragma once
#include "Formula.hpp"
#include <string>

/* Reads a DIMACS‑CNF file and returns a populated Formula object. */
Formula parseDIMACS(const std::string& path);
