/* Copyright 2016 Authors */

#ifndef SRC_GRASP_SOLVER_H_
#define SRC_GRASP_SOLVER_H_

#include <algorithm>
#include <deque>
#include <random>
#include <vector>

#include "../src/clause.h"
#include "../src/solver.h"
#include "../src/unit_propagator.h"

class GraspSolver : public Solver {
  int n;
  std::vector<int> decision_vars;
  std::vector<Clause> clauses;
  UnitPropagator up = UnitPropagator(0);
  std::vector<std::vector<int> > rev_graph;
  bool solved;
  
  //returns true if all clauses are satisfied. 
  //otherwise returns false and sets one variable to some value
  bool decide(){
    Model& model = up.get_model();
    if(std::all_of(clauses.begin(), clauses.end(), 
        [&model](Clause& c){return model.satisfied(c);})){
      return true;
    }
    auto cend = clauses.end(); 
    for(auto it = clauses.begin(); it != cend; ++it){
    int witness;
    if(model.ambivalent(*it, witness)){
      if((*it).has(witness)){
        model.set(witness, true);
      }
      else if((*it).has(-witness)){
        model.set(witness, false);
      }
      else{
        std::cerr << "cos poszlo nie tak"; 
        exit(-1);
      }
      return false;
    }
  }
//jesli jestesmy tutaj to znaczy, ze zdanie jest na pewno niespelnione
//przez biezace wartosciowanie
  return false;
}
  
  //dl - decision level, bac_dl - backtracking decision level 
  //when dl = bac_dl - we stop bactrack
  bool diagnose(int dl, int* bac_dl){
	  
  }
  
  void erase() {}
  
  //dl - decision level, bac_dl - backtracking decision level
  bool search(int dl, int* bac_dl){
    if(decide()){
      return true;
    }
    while(true){
      //no conflict if(up.propagate)
      up.propagate();
      if(true){
        if(search(dl+1, bac_dl)){
          return true;
        }
        else if(dl!=(*bac_dl)){
          erase();
          return false;
        }
      }
//it was conflict or we have bactracked a few levels
      if(!diagnose(dl, bac_dl)){
        erase();
        return false;
      }
      erase();
    }
  }

 public:
  GraspSolver(int _n): up(_n) {}

  void solve(int _n, std::vector<Clause> _clauses) override {
    n = _n;
    clauses = _clauses;
    rev_graph.resize(n);
    
    auto cend = clauses.end();
    for(auto it = clauses.begin(); it != cend; ++it){
		up.add_clause(*it);
	}
	solved = search(0,0);
  }

  bool success() override {
    return solved;
  }

  Model solution() override {
    return up.get_model();
  }
};

#endif  // SRC_GRASP_SOLVER_H_
