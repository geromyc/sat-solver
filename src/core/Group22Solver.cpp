#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>

using namespace std;
using namespace chrono;

// Runtime toggles
bool USE_VSIDS = true;
bool USE_LEARNING = true;
bool USE_NONCHRONO = true;
bool USE_WATCHED_LITS = true;
bool USE_ACTIVITY_DECAY = true;

struct Clause {
    vector<int> literals;
};

int numVars, numClauses;
vector<Clause> clauses;
vector<vector<int>> watches; // watched literals
vector<int> assignment; // -1 = unassigned, 0 = false, 1 = true
vector<int> decisionLevel;
vector<int> implicationVar;
vector<vector<int>> reasons;
vector<double> activity; // for VSIDS
double decay = 0.95;
int currentLevel = 0;
int conflictCount = 0;

const int UNASSIGNED = -1;
const int FALSE = 0;
const int TRUE = 1;

int pickBranchVar() {
    double maxScore = -1;
    int bestVar = -1;
    for (int i = 1; i <= numVars; i++) {
        if (assignment[i] == UNASSIGNED) {
            double score = USE_VSIDS ? activity[i] : 0;
            if (score > maxScore) {
                maxScore = score;
                bestVar = i;
            }
        }
    }
    return bestVar;
}


void bumpActivity(int var) {
    if (USE_VSIDS)
        activity[var] += 1.0;
}

void decayActivities() {
    if (USE_VSIDS && USE_ACTIVITY_DECAY) {
        for (int i = 1; i <= numVars; i++) {
            activity[i] *= decay;
        }
    }
}


bool addWatch(int lit, int clauseIndex) {
    if (USE_WATCHED_LITS) {
        watches[lit + numVars].push_back(clauseIndex);
    }
    return true;
}


void parseCNF(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Failed to open CNF file.\n";
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') {
            string tmp;
            stringstream ss(line);
            ss >> tmp >> tmp >> numVars >> numClauses;
            assignment.assign(numVars + 1, UNASSIGNED);
            decisionLevel.assign(numVars + 1, 0);
            implicationVar.assign(numVars + 1, 0);
            reasons.assign(numVars + 1, vector<int>());
            watches.assign(2 * numVars + 1, vector<int>());
            activity.assign(numVars + 1, 0.0);
        } else {
            stringstream ss(line);
            int lit;
            Clause clause;
            while (ss >> lit && lit != 0) {
                clause.literals.push_back(lit);
            }
            int clauseIndex = clauses.size();
            clauses.push_back(clause);
            if (clause.literals.size() >= 1) addWatch(clause.literals[0], clauseIndex);
            if (clause.literals.size() >= 2) addWatch(clause.literals[1], clauseIndex);
        }
    }
}

bool unitPropagate(int& conflictClauseIndex) {
    queue<int> unitQueue;
    for (int var = 1; var <= numVars; var++) {
        if (assignment[var] != UNASSIGNED) {
            int lit = assignment[var] == TRUE ? var : -var;
            unitQueue.push(lit);
        }
    }

    while (!unitQueue.empty()) {
        int lit = unitQueue.front();
        unitQueue.pop();
        int watchIdx = -lit + numVars;

        vector<int> tempWatch = watches[watchIdx];
        watches[watchIdx].clear();

        for (int clauseIndex : tempWatch) {
            Clause& clause = clauses[clauseIndex];
            bool foundNewWatch = false;

            for (int alt : clause.literals) {
                if (alt != lit && (assignment[abs(alt)] == UNASSIGNED || 
                    (assignment[abs(alt)] == TRUE && alt > 0) || 
                    (assignment[abs(alt)] == FALSE && alt < 0))) {
                    addWatch(alt, clauseIndex);
                    foundNewWatch = true;
                    break;
                }
            }

            if (!foundNewWatch) {
                int cntUnassigned = 0, lastUnassigned = 0;
                bool clauseSat = false;
                for (int l : clause.literals) {
                    if ((l > 0 && assignment[l] == TRUE) || (l < 0 && assignment[-l] == FALSE)) {
                        clauseSat = true;
                        break;
                    }
                    if (assignment[abs(l)] == UNASSIGNED) {
                        cntUnassigned++;
                        lastUnassigned = l;
                    }
                }

                if (clauseSat) {
                    addWatch(lit, clauseIndex);
                } else if (cntUnassigned == 1) {
                    assignment[abs(lastUnassigned)] = lastUnassigned > 0 ? TRUE : FALSE;
                    decisionLevel[abs(lastUnassigned)] = currentLevel;
                    unitQueue.push(lastUnassigned);
                    implicationVar[abs(lastUnassigned)] = clauseIndex;
                    addWatch(lit, clauseIndex);
                } else {
                    conflictClauseIndex = clauseIndex;
                    return false;
                }
            }
        }
    }

    return true;
}

vector<int> analyzeConflict(int conflictClauseIndex, int& backtrackLevel) {
       if (!USE_LEARNING) {
        backtrackLevel = 0;
        return {};
    }
    Clause& conflictClause = clauses[conflictClauseIndex];
    unordered_set<int> seen;
    vector<int> learned;
    stack<int> stk;
    int counter = 0;

    for (int lit : conflictClause.literals) {
        int var = abs(lit);
        if (decisionLevel[var] == currentLevel) {
            counter++;
        }
        if (!seen.count(var)) {
            seen.insert(var);
            stk.push(var);
        }
    }

    while (!stk.empty()) {
        int var = stk.top();
        stk.pop();

        int clauseIndex = implicationVar[var];
        if (clauseIndex == 0) continue;

        Clause& reasonClause = clauses[clauseIndex];
        for (int lit : reasonClause.literals) {
            int v = abs(lit);
            if (!seen.count(v)) {
                seen.insert(v);
                stk.push(v);
                if (decisionLevel[v] == currentLevel)
                    counter++;
            }
        }
        counter--;
        if (counter <= 0) break;
    }

    for (int v : seen) {
        learned.push_back((assignment[v] == TRUE ? v : -v));
        bumpActivity(v);
    }

    decayActivities();

    backtrackLevel = 0;
    for (int lit : learned) {
        int var = abs(lit);
        if (decisionLevel[var] < currentLevel) {
            backtrackLevel = max(backtrackLevel, decisionLevel[var]);
        }
    }

    return learned;
}

void backtrack(int level) {
    if (!USE_NONCHRONO)
        level = currentLevel - 1;

    for (int i = 1; i <= numVars; ++i) {
        if (decisionLevel[i] > level) {
            assignment[i] = UNASSIGNED;
            decisionLevel[i] = 0;
            implicationVar[i] = 0;
        }
    }
    currentLevel = level;
}

bool solve() {
    int conflictClauseIndex;
    while (true) {
        if (!unitPropagate(conflictClauseIndex)) {
            if (currentLevel == 0) return false;

            int backtrackLevel;
            vector<int> learned = analyzeConflict(conflictClauseIndex, backtrackLevel);
            Clause newClause{ learned };
            int newClauseIndex = clauses.size();
            clauses.push_back(newClause);

            for (int l : learned) addWatch(l, newClauseIndex);

            backtrack(backtrackLevel);
            currentLevel++;

            int lit = learned[0];
            assignment[abs(lit)] = lit > 0 ? TRUE : FALSE;
            decisionLevel[abs(lit)] = currentLevel;
            implicationVar[abs(lit)] = newClauseIndex;
        } else {
            int var = pickBranchVar();
            if (var == -1) return true;
            currentLevel++;
            assignment[var] = TRUE;
            decisionLevel[var] = currentLevel;
            implicationVar[var] = 0;
        }
    }
}

void printResult() {
    if (solve()) {
        cout << "RESULT:SAT ASSIGNMENT:";
        for (int i = 1; i <= numVars; i++) {
            cout << i << "=" << assignment[i];
            if (i != numVars) cout << " ";
        }
        cout << "\n";
    } else {
        cout << "RESULT:UNSAT\n";
    }
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./solver <input.cnf> [--no-vsids] [--no-learning] [--no-ncb] [--no-watched] [--no-decay]\n";
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--no-vsids") USE_VSIDS = false;
        else if (arg == "--no-learning") USE_LEARNING = false;
        else if (arg == "--no-ncb") USE_NONCHRONO = false;
        else if (arg == "--no-watched") USE_WATCHED_LITS = false;
        else if (arg == "--no-decay") USE_ACTIVITY_DECAY = false;
    }

    parseCNF(argv[1]);
    printResult();
    return 0;
}
