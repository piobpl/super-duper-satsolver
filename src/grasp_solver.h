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
  UnitPropagator up = UnitPropagator(0);
  std::vector<Clause> clauses;
  bool solved;
  
  //returns true if all clauses are satisfied. 
  //otherwise returns false and sets one variable to some value
  bool decide(int dl){
    Model& model = up.get_model();
    if(std::all_of(clauses.begin(), clauses.end(), 
        [&model](Clause& c){return model.satisfied(c);})){
      return true;
    }
    auto cend = clauses.end(); 
    for(auto it = clauses.begin(); it != cend; ++it){
      int witness;
      if(model.ambivalent(*it, &witness)){
        if((*it).has(witness)){
		  up.notice(witness, true, dl);
        }
        else if((*it).has(-witness)){
		  up.notice(witness, false, dl);
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
  
  

 public:
 
  GraspSolver(int _n): up(_n) {}

  void solve(int _n, std::vector<Clause> _clauses) override {
    n = _n;
    clauses = _clauses;  
    up.add_clauses(clauses);
    Model& model = up.get_model();
    
    if(!up.propagate()){
		solved = false;
		return;
	}
	//decision level
	int dl = 0;
	/*due to Marques Silva - for future features 
	 * check should be done in this way*/
	while(!model.all_assigned()){
	    decide(dl);
	    dl++;
	    if(up.propagate()){
	      int beta = up.diagnose();
	      if( beta < 0 ){
	        solved = false;
	        return;
        }
        else{
          up.backtrack();
          dl = beta;
          }
      }
    }
    solved = true;
  }

  bool success() override {
    return solved;
  }

  Model solution() override {
    return up.get_model();
  }
};

#endif  // SRC_GRASP_SOLVER_H_
