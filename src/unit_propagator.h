/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATOR_H_
#define SRC_UNIT_PROPAGATOR_H_

#include <queue>
#include <vector>
#include <deque>

#include "../src/clause.h"
#include "../src/model.h"

class UnitPropagator {
  typedef std::array<Clause::Iterator, 2> Observators;

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
        observed_data(2*max_var+1) {}

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }

  bool has_failed() {
    return failed;
  }

  Model& get_model() {
    return model;
  }

  void add_clause(const Clause &clause);

  void propagate();
};

#endif  // SRC_UNIT_PROPAGATOR_H_
