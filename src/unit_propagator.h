/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATOR_H_
#define SRC_UNIT_PROPAGATOR_H_

#include <queue>
#include <vector>
#include <deque>
#include <queue>

#include "../src/clause.h"
#include "../src/model.h"

class UnitPropagator {
  typedef std::array<Clause::Iterator, 2> Observators;
  
  //reson of unit propagation
  std::vector<Clause* > reason;
  //level on which unit propagation happened
  std::vector<int> level;
  std::deque<Clause> clauses;
  std::vector<Observators> obs;
  bool failed;
  int max_var;
  Model model;
  std::queue<int> propagation_queue;
  std::vector<std::vector<int>> observed_data;

  std::vector<int>& observed(int v) {
    return observed_data[v + max_var];
  }

 public:
   
  explicit UnitPropagator(int _max_var)
      : failed(0),
        max_var(_max_var),
        model(max_var),
        observed_data(2*max_var+1),
        reason(_max_var),
        level(_max_var) {
		  for(int i=0; i<_max_var; ++i){
			  level[i] = -1;
			  reason[i] = NULL;
		  }	
		}

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }
  
  void notice(int var, bool val, int dec_level){
    model.set(var, val);
    reason[var] = NULL;
    level[var] = dec_level;
  }
  
  int diagnose(){
    std::vector<Clause> bad_clauses;
    auto cend = clauses.end();
    for(auto it = clauses.begin(); it!= cend; ++it){
      if(model.spoiled(*it)){
        bad_clauses.push_back(*it);
      }
    }
    
    auto bcend = bad_clauses.end();
    for(auto it = bad_clauses.begin(); it!=bcend; ++it){
      std::queue<int> mov_back;
      for(int i=0; i<
    }
  }
  
  void backtrack() {}

  bool has_failed() {
    return failed;
  }

  Model& get_model() {
    return model;
  }

  void add_clause(const Clause &clause);

  bool propagate();
};

#endif  // SRC_UNIT_PROPAGATOR_H_
