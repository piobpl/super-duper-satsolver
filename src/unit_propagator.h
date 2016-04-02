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

  std::deque<Clause> clauses;
  std::vector<Observators> obs;
  bool failed;
  int variables;
  Model model;
  std::queue<int> propagation_queue;
  std::vector<std::vector<int>> observed_data;

  // reason of unit propagation - index of clause which is reason
  std::vector<int> reason;
  // level on which unit propagation happened
  std::vector<int> level;

  std::vector<int>& observed(int v) {
    return observed_data[v + variables];
  }

  int find_nonroot_var(const Clause& c) {
    for (int i = 1; i <= variables; i++) {
      if (c.has(i) && level[i] != -1) {
        return i;
      }
    }
    return -1;
  }

 public:
  explicit UnitPropagator(int _variables)
      : failed(0),
        variables(_variables),
        model(variables),
        observed_data(2*variables+1),
        reason(_variables+1, -1),
        level(_variables+1, -1) {}

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }

  void notice(int var, bool val, int dec_level) {
    model.set(var, val);
    reason[var] = -1;
    level[var] = dec_level;
  }

  void backtrack(int lvl) {
    for (int i = 1; i <= variables; ++i) {
      if (level[i] != -1 && level[i] >= lvl) {
        model.unset(i);
        level[i] = -1;
        reason[i] = -1;
      }
    }
  }

  bool has_failed() {
    return failed;
  }

  const Model& get_model() {
    return model;
  }

  void add_clause(const Clause &clause);

  int diagnose();

  bool propagate();
};

#endif  // SRC_UNIT_PROPAGATOR_H_
