/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <set>
#include <vector>

void UnitPropagator::push(Literal literal, int creason, int decision_level) {
  _model.set(literal, true);
  variable variable = literal.variable();
  _reason[variable] = creason;
  _level[variable] = decision_level;
  propagation_queue.push(literal);
  std::cerr << "\tinferred " << literal << " @ " << decision_level << std::endl;
}

Variable UnitPropagator::find_nonroot_var(const Clause& c) {
  for (Literal x : c)
    if (_reason[x.variable().index()] != -1)
      return x.variable();
  assert(false);
}

int UnitPropagator::max_level(const Clause& c) {
  int lvl = -1;
  for (int lit : c)
    lvl = std::max(lvl, _level[std::abs(lit)]);
  return lvl;
}

void UnitPropagator::assume(Literal var, int decision_level) {
  assert(decision_level >= 0);
  std::cerr << "assuming " << var << " @ " << decision_level << std::endl;
  if (_model.defined(var)) {
    if (_model.value(var) != val)
      _failed = true;
    return;
  }
  push(val ? var.pos() : var.neg(), -1, decision_level);
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
  _failed = false;
  while (!propagation_queue.empty()) propagation_queue.pop();
  while (decision_level <= static_cast<int>(events.size())) {
    for (Event &e : events.back()) {
      int c = e.clause;
      std::cerr << " back " << c << std::endl;

      if (!observators[e.clause][0].end())
        observed(*observators[e.clause][0]).erase(c);
      if (!observators[e.clause][1].end())
        observed(*observators[e.clause][1]).erase(c);

      if (!e.observators[0].end())
        observed(*e.observators[0]).insert(c);
      if (!e.observators[1].end())
        observed(*e.observators[1]).insert(c);

      observators[e.clause] = e.observators;
    }
    events.pop_back();
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

void UnitPropagator::repair(int c) {
  std::cerr << "repairing " << c << " = " << clauses[c] << std::endl;

  Clause::Iterator &st = observators[c][0], &nd = observators[c][1];
  int dl = max_level(clauses[c]);

  std::cerr << "\t" << dl << " " << st << " " << nd << std::endl;

  if (st.end()) return;

  Event current{c, observators[c]};
  if (!st.end()) observed(*st).erase(c);
  if (!nd.end()) observed(*nd).erase(c);

  while (!nd.end()) {
    if (!_model.is_set(*nd) || _model.value(*nd)) break;
    ++nd;
  }

  if (_model.is_set(*st) && !_model.value(*st)) {
    if (nd.end()) {
      st = nd;
    } else {
      st = nd;
      ++nd;
      while (!nd.end()) {
        if (!_model.is_set(*nd) || _model.value(*nd)) break;
        ++nd;
      }
    }
  }

  if (!st.end()) observed(*st).insert(c);
  if (!nd.end()) observed(*nd).insert(c);

  if (st != current.observators[0] || nd != current.observators[1]) {
    assert(dl < static_cast<int>(events.size()));
    std::cerr << " TO BACK " << current.clause << " " << dl << std::endl;
    events[dl].push_back(current);
    for (Event &e : events[dl]) std::cerr << e.clause << std::endl;
  }

  if (st.end()) {
    _failed = true;
  } else if (nd.end()) {
    push(*st, c, dl);
  }

  std::cerr << "\t" << st << " " << nd << std::endl;
}

void UnitPropagator::add_clause(const Clause &clause) {
  std::cerr << "adding clause " << clause << std::endl;
  assert(clause.count() > 1);

  int c = static_cast<int>(clauses.size());
  clauses.push_back(clause);

  Clause::Iterator st = clauses[c].begin(), nd = st;
  ++nd;

  observators.push_back(Observators{st, nd});
  observed(*st).insert(c);
  observed(*nd).insert(c);

  repair(c);

  std::cerr << "\t" << observators[c][0]
    << " " << observators[c][1] << std::endl;
}

void UnitPropagator::propagate() {
  while (!propagation_queue.empty()) {
    Literal literal = propagation_queue.front();
    std::cerr << "propagating " << literal << std::endl;
    propagation_queue.pop();
    std::set<int> to_repair = observed(-literal);
    for (int c : to_repair) repair(c);
  }
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
