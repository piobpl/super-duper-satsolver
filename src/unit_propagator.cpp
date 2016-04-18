/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

void UnitPropagator::push(int literal, int current_reason, int decision_level) {
  _model.set(literal, true);
  int variable = std::abs(literal);
  _reason[variable] = current_reason;
  _level[variable] = decision_level;
  propagation_queue.push(literal);
  std::cerr << "\tinferred " << literal << std::endl;
}

int UnitPropagator::find_nonroot_var(const Clause& c) {
  for (int i = 1; i <= variables; i++) {
    std::cerr << "reason[" << i << "]=" << _reason[i] << std::endl;
    if ((c.has(i) || c.has(-i)) && _reason[i] != -1) {
      return i;
    }
  }
  return -1;
}

int UnitPropagator::max_level(const Clause& c) {
  int lvl = -1;
  for (int lit : c)
    lvl = std::max(lvl, _level[std::abs(lit)]);
  return lvl;
}

void UnitPropagator::assume(int var, bool val, int decision_level) {
  std::cerr << "assuming " << var << " = " << val
    << " @ " << decision_level << std::endl;
  if (_model.is_set(var)) {
    if (_model.value(var) != val)
      failed = true;
    return;
  }
  push(val ? var : -var, -1, decision_level);
}

void UnitPropagator::backtrack(int decision_level) {
  std::cerr << "backtracking to " << decision_level << std::endl;
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
  Clause& c = clauses.back();

  int my_max = -1;
  int to_set = -1;
  for (auto it = c.begin(); it != c.end(); ++it) {
      if (_model.is_set(std::abs(*it))) {
        my_max = std::max(my_max, _level[std::abs(*it)]);
      } else {
        to_set = *it;
      }
  }
  assume(std::abs(to_set), to_set > 0 , my_max);
}

void UnitPropagator::add_clause(const Clause &clause) {
  std::cerr << "adding clause " << clause << std::endl;

  int c = static_cast<int>(clauses.size());
  clauses.push_back(clause);

  Clause::Iterator st = clauses[c].begin();
  while (!st.end()) {
    if (!_model.is_set(*st) || _model.value(*st)) break;
    ++st;
  }
  Clause::Iterator nd = st;

  if (st.end()) {
    failed = true;
  } else {
    ++nd;
    while (!nd.end()) {
      if (!_model.is_set(*nd) || _model.value(*nd)) break;
      ++nd;
    }
    if (nd.end()) {
      if (!_model.is_set(*st)) {
        push(*st, c, max_level(clause));
      }
    }
  }

  observators.push_back(Observators{st, nd});
  observed(*st).push_back(c);
  observed(*nd).push_back(c);

  std::cerr << "\t" << observators[c][0]
    << " " << observators[c][1] << std::endl;
}

bool UnitPropagator::propagate(int decision_level) {
  std::cerr << "propagating " << decision_level << std::endl;
  if (failed) return false;
  while (!propagation_queue.empty()) {
    int literal = propagation_queue.front();
    std::cerr << "proping " << literal << std::endl;
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
        std::cerr << "\tfailed " << clauses[c] << std::endl;
        failed = true;
        return false;
      } else if (!_model.is_set(new_literal)) {
        std::cerr << "\tclosed " << clauses[c] << std::endl;
        push(new_literal, c, max_level(clauses[c]));
      }
    }
  }
  return true;
}

int UnitPropagator::diagnose() {
  std::cerr << "diagnosing..." << std::endl << std::flush;
  Clause clause(variables);
  for (Clause clause_it : clauses) {
    if (_model.spoiled(clause_it)) {
      clause = clause_it;
      break;
    }
  }

  int ret_to_lvl = -1;
  std::vector<bool> was_here;
  was_here.resize(variables+1);
  std::queue<int> mov_back;
  int k;
  for (int i=1; i <= variables; ++i) {
    was_here[i] = clause.has(i) || clause.has(-i);
    if (was_here[i]) {
      ret_to_lvl = std::max(ret_to_lvl, _level[i]);
    }
  }

  while ((k=find_nonroot_var(clause)) != -1) {
    if (clause.has(k)) {
        clause.remove(k);
    } else {
        clause.remove(-k);
    }

    Clause& reso = clauses[_reason[k]];
    for (int i = 1; i <= variables; ++i) {
      if (!was_here[i] && (reso.has(i) || reso.has(-i)) && i != k) {
        was_here[i] = true;
        clause.add(i);
      }
    }
  }

  for (int i = 1; i <= variables; ++i) {
    if (clause.has(i)) {
      if (_model.value(i) == true) {
        clause.remove(i);
        clause.add(-i);
      }
    }
  }

  /* to powinno jeszcze zwracac -1 
   * gdy zmienna na poziomie 0 od razu daje sprzecznosc
   * prosty fix na jutro
   */

  add_clause(clause);
  std::cerr << "\tproblem @ " << ret_to_lvl << std::endl;
  return ret_to_lvl;
}
