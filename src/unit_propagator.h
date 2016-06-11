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
  explicit UnitPropagator(int variables, int base_clauses)
    : _base_clauses(base_clauses),
      _model(variables),
      _failed(false),
      _clauses_with_literal(2*variables),
      _satisfied(1),
      _reason(variables, -1),
      _level(variables, -1),
      _deductions(1),
      _activity(variables) {}

  void add_clause(const Clause &clause, bool run_recheck);

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause, false);
    }
    recheck();
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

  /**
   * Forgets the clauses with given indices. Indices in the ind vector must be
   * in increasing order. Time complexity: linear wrt. the
   * size of the whole formula
   */
  void forget(const std::vector<int>& ind);

  bool garbage_clauses_glucose();

  bool garbage_clauses_grasp();

  bool garbage_clauses_all();

  void garbage_clauses_minisat();

 private:
  std::pair<bool, Literal> extract_nonroot_literal(Clause *c);

  void rebuild();

  void recheck();

  void propagation_push(Literal var, int c);

  int glucose_factor(const Clause& cl);

  void calculate_watchers(int c);

  void bump_var_activity(const Clause &);

  void decay_var_activity();

  void decay_clause_activity();

  void bump_clause_activity(Clause *c);

  void restart();

  int _base_clauses;
  Model _model;
  bool _failed;

  std::vector<Clause> _clauses;
  std::vector<std::pair<int, int>> _watchers;
  std::vector<std::vector<int>> _clauses_with_literal;

  std::vector<int> _unsatisfied;
  std::vector<std::vector<int>> _satisfied;
  std::vector<int> _satisfied_at;

  std::queue<Literal> _propagation_queue;

  std::vector<int> _reason;
  std::vector<int> _level;
  std::vector<std::vector<Literal>> _deductions;

  const double VAR_DECAY = 0.95;
  const double RESCALE_THRESHOLD = 1e100;
  double _var_inc = 1;
  std::vector<double> _activity;

  const double CLAUSE_DECAY = 0.999;
  const double CLAUSE_RESCALE_THRESHOLD = 1e20;
  double _cla_inc = 1;

  const int MAX_CLAUSES_GROW = 100;
  int MAX_CLAUSES_NUM = 3000;
};

#endif  // SRC_UNIT_PROPAGATOR_H_
