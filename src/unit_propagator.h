/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATOR_H_
#define SRC_UNIT_PROPAGATOR_H_

#include <deque>
#include <queue>
#include <vector>

#include "../src/clause.h"
#include "../src/model.h"

class UnitPropagator {
  typedef std::array<Clause::Iterator, 2> Observators;
  struct ObservatorsEvent {
    int decision_level;
    int clause;
    Observators observators;
  };

  std::deque<Clause> clauses;
  std::vector<Observators> observators;
  std::vector<ObservatorsEvent> observators_events;
  bool failed;
  int variables;
  Model _model;
  std::queue<int> propagation_queue;
  std::vector<std::vector<int>> _observed;

  // reason of unit propagation - index of clause which is reason
  std::vector<int> _reason;
  // level on which unit propagation happened
  std::vector<int> _level;

  std::vector<int>& observed(int v) {
    return _observed[v + variables];
  }

  void push(int literal, int current_reason, int decision_level);

  int find_nonroot_var(const Clause& c);

  int max_level(const Clause& c);

 public:
  explicit UnitPropagator(int _variables)
      : failed(0),
        variables(_variables),
        _model(variables),
        _observed(2*variables+1),
        _reason(_variables+1, -1),
        _level(_variables+1, -1) {}

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }

  int reason(int var) {
    return _reason[var];
  }

  bool has_failed() {
    return failed;
  }

  const Model& model() {
    return _model;
  }

  void assume(int var, bool val, int decision_level);

  void backtrack(int decision_level);

  void add_clause(const Clause &clause);

  int diagnose();

  bool propagate(int decision_level);
};

#endif  // SRC_UNIT_PROPAGATOR_H_
