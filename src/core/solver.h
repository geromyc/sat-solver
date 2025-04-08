#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

class Clause {
public:
    std::vector<int> literals;
    bool is_satisfied(const std::vector<int>& assignment);
};
    
class Formula {
public:
    std::vector<Clause> clauses;
    std::vector<std::vector<Clause*>> watch_list;
    
    bool unit_propagation(std::vector<int>& assignment);
    bool is_satisfied(const std::vector<int>& assignment);
};

class Solver {
public:
    Formula formula;
    std::vector<int> assignment;
    std::vector<int> decision_levels;

    bool solve();
    bool dpll();
    void backtrack();
};
    