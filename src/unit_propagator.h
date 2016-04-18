/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATOR_H_
#define SRC_UNIT_PROPAGATOR_H_

#include <deque>
#include <queue>
#include <set>
#include <vector>

#include "../src/model.h"

class UnitPropagator {
  typedef std::array<Clause::iterator, 2> Observators;

  struct Event {
    int clause;
    Observators observators;
  };

  std::deque<Clause> clauses;
  std::vector<Observators> observators;
  std::vector<std::vector<Event>> events;
  bool _failed;
  int variables;
  Model _model;
  std::queue<int> propagation_queue;
  std::vector<std::set<int>> _observed;

  // reason of unit propagation - index of clause which is reason
  std::vector<int> _reason;
  // level on which unit propagation happened
  std::vector<int> _level;

  std::set<int>& observed(int v) {
    return _observed[v + variables];
  }

  void push(Literal literal, int current_reason, int decision_level);

  int find_nonroot_var(const Clause& c);

  int max_level(const Clause& c);

  /* Analyzes satisfiablity of clause no c.
   * Clause observers can be in three *stable* states:
   * - both looking at some different literals, both unset or satisfied
   * - nd at end, st at satifisfied literal
   * - both at end, clause spoiled
   * For unpropagated literals some observers may still look at them as unset
   * or satisfied, causing the clause observers to be in *temporary* state.
   * Only first *stable* state can become *temporary* state!
   * Repair transitions clause observers from *temporary* to *stable* state.
   */
  void repair(int c);

 public:
  explicit UnitPropagator(int _variables)
      : events(_variables),
        _failed(0),
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

  bool failed() {
    return _failed;
  }

  const Model& model() {
    return _model;
  }

  const std::deque<Clause>& get_clauses() {
    return clauses;
  }

  void assume(Literal var, int decision_level);

  void backtrack(int decision_level);

  void add_clause(const Clause &clause);

  void propagate();

  int diagnose();

  size_t clauses_size() {
    return clauses.size();
  }
};

#endif  // SRC_UNIT_PROPAGATOR_H_
