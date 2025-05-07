#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <optional>
#include <string>
#include <algorithm>

using Clause = std::vector<int>;
using CNF = std::vector<Clause>;
using Assignment = std::unordered_map<int, bool>;

bool is_clause_satisfied(const Clause& clause, const Assignment& assignment) {
    for (int literal : clause) {
        int var = abs(literal);
        if (assignment.find(var) != assignment.end()) {
            if (assignment.at(var) != (literal < 0)) return true;
        }
    }
    return false;
}

std::optional<Assignment> unit_propagate(const CNF& cnf, Assignment assignment) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const Clause& clause : cnf) {
            int unassigned_count = 0;
            int last_unassigned = 0;
            bool satisfied = false;

            for (int lit : clause) {
                int var = abs(lit);
                if (assignment.find(var) == assignment.end()) {
                    ++unassigned_count;
                    last_unassigned = lit;
                } else if (assignment[var] != (lit < 0)) {
                    satisfied = true;
                    break;
                }
            }

            if (satisfied) continue;
            if (unassigned_count == 0) return std::nullopt;

            if (unassigned_count == 1) {
                int var = abs(last_unassigned);
                bool value = last_unassigned > 0;
                assignment[var] = value;
                changed = true;
            }
        }
    }
    return assignment;
}

int choose_variable(const CNF& cnf, const Assignment& assignment) {
    for (const Clause& clause : cnf) {
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.find(var) == assignment.end()) return var;
        }
    }
    return 0;
}

std::optional<Assignment> dpll(const CNF& cnf, Assignment assignment = {}) {
    auto propagated = unit_propagate(cnf, assignment);
    if (!propagated) return std::nullopt;

    assignment = *propagated;

    bool all_satisfied = std::all_of(cnf.begin(), cnf.end(), [&](const Clause& clause) {
        return is_clause_satisfied(clause, assignment);
    });

    if (all_satisfied) return assignment;

    int var = choose_variable(cnf, assignment);
    if (var == 0) return std::nullopt;

    for (bool value : {true, false}) {
        Assignment new_assignment = assignment;
        new_assignment[var] = value;
        auto result = dpll(cnf, new_assignment);
        if (result) return result;
    }

    return std::nullopt;
}

CNF parse_dimacs(const std::string& filename) {
    CNF cnf;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << "\n";
        exit(1);
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') continue;

        std::istringstream iss(line);
        int lit;
        Clause clause;
        while (iss >> lit) {
            if (lit == 0) break;
            clause.push_back(lit);
        }
        if (!clause.empty()) cnf.push_back(clause);
    }

    return cnf;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./Group22Solver <input_file.cnf>\n";
        return 1;
    }

    CNF cnf = parse_dimacs(argv[1]);
    auto result = dpll(cnf);

    if (result) {
        std::cout << "RESULT:SAT\n";
        std::cout << "ASSIGNMENT:";
        std::vector<int> vars;
        for (const auto& [var, _] : *result) vars.push_back(var);
        std::sort(vars.begin(), vars.end());
        for (int var : vars) {
            std::cout << var << "=" << ((*result)[var] ? "1" : "0") << " ";
        }
        std::cout << "\n";
    } else {
        std::cout << "RESULT:UNSAT\n";
    }

    return 0;
}
