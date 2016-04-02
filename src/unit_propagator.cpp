/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

#include <algorithm>
#include <queue>
#include <vector>

void UnitPropagator::push(int literal, int current_reason, int decision_level) {
  _model.set(literal, true);
  int variable = std::abs(literal);
  _reason[variable] = current_reason;
  _level[variable] = decision_level;
  propagation_queue.push(literal);
}

int UnitPropagator::find_nonroot_var(const Clause& c) {
  for (int i = 1; i <= variables; i++) {
    if (c.has(i) && _level[i] != -1) {
      return i;
    }
  }
  return -1;
}

int UnitPropagator::max_level(const Clause& c) {
  int lvl = -1;
  for (int lit : c)
    lvl = std::max(lvl, std::abs(lit));
  return lvl;
}

void UnitPropagator::assume(int var, bool val, int decision_level) {
  if (_model.is_set(var)) {
    if (_model.value(var) != val)
      failed = true;
    return;
  }
  push(val ? var : -var, -1, decision_level);
}

void UnitPropagator::backtrack(int decision_level) {
  for (int i = 1; i <= variables; ++i) {
    if (_level[i] != -1 && _level[i] >= decision_level) {
      _model.unset(i);
      _level[i] = -1;
      _reason[i] = -1;
    }
  }
  failed = false;
  while (!propagation_queue.empty()) propagation_queue.pop();
  while (!observators_events.empty()
      && observators_events.back().decision_level >= decision_level) {
    ObservatorsEvent e = observators_events.back();
    observators_events.pop_back();
    if (!observators[e.clause][1].end())
      observed(*observators[e.clause][1]).pop_back();
    observators[e.clause] = e.observators;
  }
}

void UnitPropagator::add_clause(const Clause &clause) {
  int c = static_cast<int>(clauses.size());
  clauses.push_back(clause);

  Clause::Iterator st = clauses[c].begin();
  while (!st.end()) {
    if (!_model.is_set(*st) || _model.value(*st)) break;
    ++st;
  }
  if (st.end()) {
    failed = true;
    clauses.pop_back();
    return;
  }

  Clause::Iterator nd = st;
  ++nd;
  while (!nd.end()) {
    if (!_model.is_set(*nd) || _model.value(*nd)) break;
    ++nd;
  }
  if (nd.end()) {
    if (!_model.is_set(*st))
      push(*st, c, max_level(clause));
    clauses.pop_back();
    return;
  }

  observators.push_back(Observators{st, nd});
  observed(*st).push_back(c);
  observed(*nd).push_back(c);
}

bool UnitPropagator::propagate(int decision_level) {
  if (failed) return false;
  while (!propagation_queue.empty()) {
    int literal = propagation_queue.front();
    propagation_queue.pop();
    for (int c : observed(-literal)) {
      observators_events.push_back(ObservatorsEvent{
        decision_level,
        c,
        observators[c]
      });

      if (*observators[c][0] == -literal) {
        observators[c][0] = observators[c][1];
      }
      ++observators[c][1];
      while (!observators[c][1].end()) {
        if (!_model.is_set(*observators[c][1])
            || _model.value(*observators[c][1])) break;
        ++observators[c][1];
      }
      int new_literal = *observators[c][0];
      if (!observators[c][1].end()) {
        observed(*observators[c][1]).push_back(c);
      } else if (_model.is_set(new_literal) && !_model.value(new_literal)) {
        failed = true;
        return false;
      } else if (!_model.is_set(new_literal)) {
        push(new_literal, c, max_level(clauses[c]));
      }
    }
  }
  return true;
}

int UnitPropagator::diagnose() {
  std::vector<Clause> bad_clauses;
  for (Clause clause : clauses) {
    if (_model.spoiled(clause)) {
      bad_clauses.push_back(clause);
    }
  }

  int ret_to_lvl = -1;
  for (Clause &clause : bad_clauses) {
    std::queue<int> mov_back;
    int k;
    while ((k=find_nonroot_var(clause)) != -1) {
      bool pos = clause.is_pos(k);
      clause.remove(k);
      for (int i = 1; i <= variables; ++i) {
        Clause& reso = clauses[_reason[k]];
        if (reso.has(i)) {
          bool pos_i = reso.is_pos(i);
          if ((pos && pos_i) || (!pos && !pos_i)) {
            clause.add(i);
          } else {
            clause.add(-i);
          }
        }
      }
    }
    for (int i = 1; i <= variables; ++i) {
      if (clause.has(i)) {
        ret_to_lvl = std::min(ret_to_lvl, _level[i]);
      }
    }
  }
  /* to powinno jeszcze zwracac -1 
   * gdy zmienna na poziomie 0 od razu daje sprzecznosc
   * prosty fix na jutro
   */
  add_clauses(bad_clauses);
  return ret_to_lvl;
}
