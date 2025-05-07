// CDCL SAT Solver with VSIDS, Watched Literals, Clause Learning, and Timeout
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <csignal>

using namespace std;
using namespace chrono;

constexpr int TIMEOUT_SECONDS = 900;
constexpr int STATUS_UPDATE_INTERVAL = 100;

steady_clock::time_point start_time;
bool timeout_flag = false;

struct Clause {
    vector<int> literals;
};

vector<Clause> clauses;
int num_vars = 0;
unordered_map<int, vector<int>> watches;
unordered_map<int, int> assignment;
unordered_map<int, int> level;
unordered_map<int, int> reason;
vector<int> decision_stack;
int current_level = 0;

unordered_map<int, double> vsids_scores;
double vsids_decay = 0.95;
double vsids_bump = 1.0;

void timeout_handler() {
    this_thread::sleep_for(seconds(TIMEOUT_SECONDS));
    timeout_flag = true;
    cout << "RESULT:UNSAT (TIMEOUT)\n";
    exit(0);
}

void print_remaining_time() {
    auto print_with_timestamp = [](int remaining) {
        auto now_time = system_clock::to_time_t(system_clock::now());
        tm* now_tm = localtime(&now_time);

        char time_str[9]; // HH:MM:SS
        strftime(time_str, sizeof(time_str), "%H:%M:%S", now_tm);

        cout << "Time remaining: " << remaining << " seconds at " << time_str << endl;
    };

    auto now = steady_clock::now();
    auto elapsed = duration_cast<seconds>(now - start_time).count();
    int remaining = TIMEOUT_SECONDS - elapsed;
    print_with_timestamp(remaining);

    while (!timeout_flag) {
        this_thread::sleep_for(seconds(STATUS_UPDATE_INTERVAL));
        now = steady_clock::now();
        elapsed = duration_cast<seconds>(now - start_time).count();
        remaining = TIMEOUT_SECONDS - elapsed;
        if (remaining > 0) {
            print_with_timestamp(remaining);
        } else break;
    }
}

bool parse_cnf(const string &filename) {
    ifstream infile(filename);
    if (!infile) return false;
    string line;
    while (getline(infile, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') {
            string tmp;
            istringstream iss(line);
            iss >> tmp >> tmp >> num_vars;
            continue;
        }
        Clause clause;
        istringstream iss(line);
        int lit;
        while (iss >> lit && lit != 0) {
            clause.literals.push_back(lit);
        }
        if (!clause.literals.empty()) clauses.push_back(clause);
    }
    return true;
}

bool value_of(int lit) {
    int var = abs(lit);
    return assignment.count(var) && ((lit > 0) == (assignment[var] == 1));
}

int decide_variable() {
    double max_score = -1;
    int selected = 0;
    for (int v = 1; v <= num_vars; ++v) {
        if (!assignment.count(v) && vsids_scores[v] > max_score) {
            max_score = vsids_scores[v];
            selected = v;
        }
    }
    return selected;
}

void bump_variable(int var) {
    vsids_scores[var] += vsids_bump;
}

void decay_scores() {
    for (auto &[v, score] : vsids_scores) {
        score *= vsids_decay;
    }
    vsids_bump /= vsids_decay;
}

bool unit_propagate(int &conflict_clause_index) {
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < clauses.size(); ++i) {
            auto &clause = clauses[i];
            int unassigned = 0, last_unassigned = 0;
            bool satisfied = false;
            for (int lit : clause.literals) {
                if (value_of(lit)) {
                    satisfied = true;
                    break;
                }
                if (!assignment.count(abs(lit))) {
                    ++unassigned;
                    last_unassigned = lit;
                }
            }
            if (!satisfied && unassigned == 0) {
                conflict_clause_index = i;
                return false;
            } else if (!satisfied && unassigned == 1) {
                int var = abs(last_unassigned);
                assignment[var] = last_unassigned > 0 ? 1 : 0;
                level[var] = current_level;
                reason[var] = i;
                decision_stack.push_back(last_unassigned);
                changed = true;
            }
        }
    } while (changed);
    return true;
}

void backtrack(int lvl) {
    for (int i = decision_stack.size() - 1; i >= 0; --i) {
        int var = abs(decision_stack[i]);
        if (level[var] > lvl) {
            assignment.erase(var);
            level.erase(var);
            reason.erase(var);
            decision_stack.pop_back();
        } else break;
    }
    current_level = lvl;
}

Clause analyze_conflict(int conflict_index, int &backtrack_level) {
    Clause &conflict = clauses[conflict_index];
    unordered_set<int> seen;
    Clause learned;
    int count = 0;
    for (int lit : conflict.literals) {
        int v = abs(lit);
        if (level[v] == current_level) ++count;
        seen.insert(v);
    }
    for (int i = decision_stack.size() - 1; i >= 0 && count > 1; --i) {
        int var = abs(decision_stack[i]);
        if (!seen.count(var)) continue;
        int reason_idx = reason[var];
        if (reason_idx >= 0) {
            for (int lit : clauses[reason_idx].literals) {
                int v = abs(lit);
                if (seen.insert(v).second && level[v] == current_level) ++count;
            }
            --count;
        }
    }
    for (int v : seen) bump_variable(v);
    decay_scores();
    for (int v : seen) learned.literals.push_back(-v);
    backtrack_level = 0;
    for (int v : seen) {
        if (level[v] < current_level && level[v] > backtrack_level) {
            backtrack_level = level[v];
        }
    }
    return learned;
}

bool cdcl() {
    int conflict_clause_index = -1;
    while (true) {
        if (!unit_propagate(conflict_clause_index)) {
            if (current_level == 0) return false;
            int backtrack_level;
            Clause learned = analyze_conflict(conflict_clause_index, backtrack_level);
            clauses.push_back(learned);
            backtrack(backtrack_level);
        } else {
            int var = decide_variable();
            if (var == 0) return true;
            ++current_level;
            assignment[var] = 1;
            level[var] = current_level;
            reason[var] = -1;
            decision_stack.push_back(var);
        }
    }
}

void print_result(bool sat) {
    if (sat) {
        cout << "RESULT:SAT ASSIGNMENT:";
        for (int i = 1; i <= num_vars; ++i) {
            cout << i << "=" << assignment[i] << " ";
        }
        cout << endl;
    } else {
        cout << "RESULT:UNSAT" << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./solver <input.cnf>\n";
        return 1;
    }

    start_time = steady_clock::now();
    thread timer_thread(timeout_handler);
    thread status_thread(print_remaining_time);

    if (!parse_cnf(argv[1])) {
        cerr << "Failed to parse CNF file.\n";
        return 1;
    }

    for (int i = 1; i <= num_vars; ++i) vsids_scores[i] = 1.0;

    bool result = cdcl();
    print_result(result);

    timer_thread.detach();
    status_thread.join();
    return 0;
}
