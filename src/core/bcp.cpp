#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>

using namespace std;

// Function to evaluate a single clause
bool evaluateClause(const vector<pair<char, bool>>& clause, const map<char, bool>& assignment) {
    for (const auto& literal : clause) {
        char variable = literal.first;
        bool isNegated = literal.second;

        // Check if the literal is satisfied
        if (assignment.at(variable) == !isNegated) {
            return true; // Clause is satisfied
        }
    }
    return false; // Clause is not satisfied
}

// Function to evaluate the entire formula
bool evaluateFormula(const vector<vector<pair<char, bool>>>& formula, const map<char, bool>& assignment) {
    for (const auto& clause : formula) {
        if (!evaluateClause(clause, assignment)) {
            return false; // If any clause is false, the formula is unsatisfied
        }
    }
    return true; // All clauses are satisfied
}

// Function to format and display the formula
string formatFormula(const vector<vector<pair<char, bool>>>& formula) {
    string result = "";
    for (size_t i = 0; i < formula.size(); ++i) {
        result += "(";
        for (size_t j = 0; j < formula[i].size(); ++j) {
            if (formula[i][j].second) {
                result += string(1, formula[i][j].first) + "'";
            } else {
                result += string(1, formula[i][j].first);
            }
            if (j < formula[i].size() - 1) {
                result += " + ";
            }
        }
        result += ")";
        if (i < formula.size() - 1) {
            result += " ∧ ";
        }
    }
    return result;
}

// Recursive SAT solver with debug output
bool solveSAT(const vector<vector<pair<char, bool>>>& formula, map<char, bool>& assignment, set<char>& variables, map<char, bool>::iterator it) {
    if (it == assignment.end()) {
        // Base case: All variables have been assigned
        bool result = evaluateFormula(formula, assignment);
        cout << "Testing assignment: ";
        for (const auto& [var, value] : assignment) {
            cout << var << "=" << (value ? "true" : "false") << " ";
        }
        cout << "-> " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << endl;
        return result;
    }

    // Assign the current variable to true and recurse
    it->second = true;
    cout << "Assigning " << it->first << " = true" << endl;
    if (solveSAT(formula, assignment, variables, next(it))) {
        return true;
    }

    // Assign the current variable to false and recurse
    it->second = false;
    cout << "Assigning " << it->first << " = false" << endl;
    if (solveSAT(formula, assignment, variables, next(it))) {
        return true;
    }

    // Output the current state if unsatisfiable
    cout << "Backtracking on " << it->first << endl;
    return false; // No satisfying assignment found
}

int main() {
    // Full Boolean formula in CNF from the image
    vector<vector<pair<char, bool>>> formula = {
        {{'a', false}, {'b', false}, {'c', false}},  // (a + b + c)
        {{'a', false}, {'b', false}, {'c', true}},   // (a + b + c')
        {{'a', true}, {'b', false}, {'c', true}},    // (a' + b + c')
        {{'a', true}, {'c', false}, {'d', false}},   // (a' + c + d)
        {{'a', true}, {'c', false}, {'d', true}},    // (a' + c + d')
        {{'b', true}, {'c', true}, {'d', false}},    // (b' + c' + d)
        {{'b', true}, {'c', true}, {'d', true}}      // (b' + c' + d')
    };

    // Display the formula
    cout << "Formula: " << formatFormula(formula) << endl;

    // Extract variables for the formula
    set<char> variables;
    for (const auto& clause : formula) {
        for (const auto& literal : clause) {
            variables.insert(literal.first);
        }
    }

    // Initialize variable assignments
    map<char, bool> assignment;
    for (char var : variables) {
        assignment[var] = false; // Default to false
    }

    // Solve the SAT problem for the formula
    if (solveSAT(formula, assignment, variables, assignment.begin())) {
        cout << "SAT: The formula is satisfiable.\n";
        cout << "Satisfying assignment:\n";
        for (const auto& [var, value] : assignment) {
            cout << var << " = " << (value ? "true" : "false") << endl;
        }
    } else {
        cout << "UNSAT: The formula is not satisfiable.\n";
    }

    return 0;
}