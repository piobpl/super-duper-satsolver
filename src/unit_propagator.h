/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATOR_H_
#define SRC_UNIT_PROPAGATOR_H_

#include <deque>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "../src/model.h"

class UnitPropagator {
 public:
  unsigned int MAX_CLAUSES_NUM = 1000;
  explicit UnitPropagator(int variables)
    : _model(variables),
      _clauses_with_literal(2*variables),
      _failed(false),
      _reason(variables, -1),
      _level(variables, -1),
      _deductions(1) {}

  void add_clause(const Clause &clause);

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }
  
  bool garbage_clauses ();

  void assume(Literal var);

  const std::vector<std::vector<Literal>>& deductions() {
    return _deductions;
  }

  bool failed() {
    return _failed;
  }

  const Model& model() {
    return _model;
  }

  int diagnose();
  
  void set_cl_num (int x) {clauses_num_at_start = x; }

  void revert(int decision_level);

  /**
   * Returns a vector of (clause, index) pairs, over all clauses that can be
   * forgotten, that is hasn't already bren forgotten and is not a reason of any
   * value at the moment. This member function returns a different vector after
   * each forgetting, but a clause assigned to an id doesn't change.
   */
  std::vector<std::pair<Clause, int>> available_clauses() const;

  /**
   * Makes the c-th clause forgotten. Behavior is undefined if ci-th clause
   * has already been forgotten or is a reason of some value.
   */
  void forget_clause(int c);

 private:
  std::pair<bool, Literal> extract_nonroot_literal(Clause *c);

  void recheck();

  void propagation_push(Literal var, int lause_pos);

  void calculate_watchers(int c);

  Model _model;
  std::vector<std::pair<Clause, int> > _clauses;
  std::vector<int> _clause_index;
  std::vector<std::pair<int, int> > _watchers;
  std::vector<int> _finished;
  std::vector<std::set<int> > _clauses_with_literal;
  std::queue<Literal> _propagation_queue;
  
  int clauses_num_at_start = -1; 
  bool _failed;
  std::vector<int> _reason;
  std::vector<int> _is_reason;
  std::vector<int> _level;

  // unused in brutal up std::queue<Literal> _propagation_queue;
  std::vector<std::vector<Literal>> _deductions;
};

#endif  // SRC_UNIT_PROPAGATOR_H_
