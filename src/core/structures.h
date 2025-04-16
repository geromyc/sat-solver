#ifndef STRUCTURES_H
#define STRUCTURES_H

// A single literal: variable index and whether it is negated
struct Lit {
    int var;       // 1‑based variable index
    bool neg;      // true if this is ¬var
};

// A clause is just a list of literals
struct Clause {
    std::vector<Lit> lits;
};

// The whole formula
struct CNF {
    int       numVars;
    std::vector<Clause> clauses;

    // For fast propagation: for each literal (var·sign) a list
    // of all clauses where it appears
    std::vector<std::vector<int>> occur;
    
    // Build the occur‑list after you parse clauses:
    void buildOccur() {
        occur.assign(2*(numVars+1),{});
        for(int ci=0; ci<clauses.size(); ci++){
            for(auto &l : clauses[ci]){
                int idx = 2*l.var + (l.neg?1:0);
                occur[idx].push_back(ci);
            }
        }
    }
};

CNF readDimacs(std::istream &in){
    CNF F;
    std::string line;
    while(std::getline(in,line)){
        if(line.empty() || line[0]=='c') continue;
        if(line[0]=='p'){
            std::istringstream ss(line);
            std::string tmp; ss>>tmp>>tmp>>F.numVars;
        } else {
            std::istringstream ss(line);
            int lit; Clause C;
            while(ss>>lit && lit!=0){
                C.lits.push_back({abs(lit),(lit<0)});
            }
            F.clauses.push_back(std::move(C));
        }
    }
    return F;
}

#endif