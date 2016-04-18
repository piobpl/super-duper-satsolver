/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATOR_H_
#define SRC_UNIT_PROPAGATOR_H_

#include <deque>
#include <queue>
#include <set>
#include <vector>

#include "../src/model.h"

class UnitPropagator {
 public:
  explicit UnitPropagator(int variables)
    : _model(variables),
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

  void diagnose();

  void revert();

 private:
  void recheck();

  std::vector<Clause> _clauses;
  Model _model;

  bool _failed;
  std::vector<int> _reason;
  std::vector<int> _level;

  // unused in brutal up std::queue<Literal> _propagation_queue;
  std::vector<std::vector<Literal>> _deductions;
};

#endif  // SRC_UNIT_PROPAGATOR_H_
