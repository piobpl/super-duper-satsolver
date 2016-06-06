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
  int MAX_CLAUSES_NUM = 500;
  explicit UnitPropagator(int variables, int base_clauses)
    : _base_clauses(base_clauses),
      _model(variables),
      _clauses_with_literal(2*variables),
      _failed(false),
      _reason(variables, -1),
      _level(variables, -1),
      _activity(variables),
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

  int diagnose();

  void revert(int decision_level);

  Variable active_variable();

  const std::vector<Clause>& clauses() { return _clauses; }

  std::vector<int> redundant();

  void forget(const std::vector<int>& ind);

  bool garbage_clauses_glucose();

  bool garbage_clauses_grasp();

  bool garbage_clauses_all();

 private:
  std::pair<bool, Literal> extract_nonroot_literal(Clause *c);

  bool recheck_clause(Literal, int, int);

  void recheck();

  void propagation_push(Literal var, int c);

  int glucose_factor(const Clause& cl);

  void calculate_watchers(int c);

  void bump_activity(const Clause &);

  void decay_activity();

  void restart();

  int _base_clauses;

  Model _model;
  std::vector<Clause> _clauses;
  std::vector<std::pair<int, int> > _watchers;
  std::vector<std::vector<int> > _clauses_with_literal;
  std::queue<Literal> _propagation_queue;
  bool _failed;
  std::vector<int> _reason;
  std::vector<int> _level;

  const double VAR_DECAY = 0.95;
  const double RESCALE_THRESHOLD = 1e100;
  double _var_inc = 1;
  std::vector<double> _activity;

  // unused in brutal up std::queue<Literal> _propagation_queue;
  std::vector<std::vector<Literal>> _deductions;
};

#endif  // SRC_UNIT_PROPAGATOR_H_
