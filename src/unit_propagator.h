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
  
  //reson of unit propagation - index of clause which is reason
  std::vector<int> reason;
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
  
  int find_nonroot_var(const Clause& c){
    for(int i=1; i<=max_var; i++){
      if(c.has(i) && level[i]!=-1){
        return i;
      }
    }
    return -1;
  }

 public:
   
  explicit UnitPropagator(int _max_var)
      : failed(0),
        max_var(_max_var),
        model(max_var),
        observed_data(2*max_var+1),
        reason(_max_var+1),
        level(_max_var+1) {
		  for(int i=0; i<=max_var; ++i){
			  level[i] = -1;
			  reason[i] = -1;
		  }	
		}

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }
  
  void notice(int var, bool val, int dec_level){
    model.set(var, val);
    reason[var] = -1;
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
    
    int ret_to_lvl = -1;
    auto bcend = bad_clauses.end();
    for(auto it = bad_clauses.begin(); it!=bcend; ++it){
      std::queue<int> mov_back;
      int k;
      while((k=find_nonroot_var(*it))!=-1){
        bool pos = it->ispos(k);
        it->remove(k);
        for(int i=1; i<=max_var; ++i){
          Clause& reso = clauses[reason[k]];
          if(reso.has(i)){
            bool pos_i = reso.ispos(i);
            if((pos && pos_i) || (!pos && !pos_i)){
              it->add(i);
            }
            else{
              it->add(-i);
            }
          }
        }
      }
      for(int i=1; i<=max_var; ++i){
        if(it->has(i)){
          ret_to_lvl = std::min(ret_to_lvl, level[i]);
        }
      }
    }
    /*to powinno jeszcze zwracac -1 
     * gdy zmienna na poziomie 0 od razu daje sprzecznosc
     * prosty fix na jutro
     */
    add_clauses(bad_clauses);
    return ret_to_lvl;
  }
  
  void backtrack(int lvl) {
    for(int i=1; i<=max_var; ++i){
      if(level[i]!=-1 && level[i]>=lvl){
        model.unset(i);
        level[i] = -1;
        reason[i] = -1;
      }
    }
  }

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
