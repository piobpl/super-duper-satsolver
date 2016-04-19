/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <set>
#include <utility>
#include <vector>

void UnitPropagator::add_clause(const Clause &clause) {
  _clauses.push_back(clause);
  recheck();
}

void UnitPropagator::assume(Literal var) {
  _level[var.variable().index()] = static_cast<int>(_deductions.size());
  _deductions.push_back(std::vector<Literal>{var});
  _model.set(var);
  recheck();
}

int UnitPropagator::diagnose() {
  Clause clause;
  for (Clause clause_it : _clauses) {
    if (_model.spoiled(clause_it)) {
      clause = clause_it;
      break;
    }
  }

  int ret_to_lvl = -1;
  std::vector<bool> was_here;
  was_here.resize(_model.variable_count(), 0);
  for (Literal x : clause) {
    int i = x.variable().index();
    was_here[i] = true;
    ret_to_lvl = std::max(ret_to_lvl, _level[i]);
  }

  bool got;
  Literal x;
  while (std::tie(got, x) = extract_nonroot_literal(&clause), got) {
    Clause& reso = _clauses[_reason[x.variable().index()]];
    for (Literal y : reso) {
      int i = y.variable().index();
      if (!was_here[i] && x.variable() != y.variable()) {
        was_here[i] = true;
        clause.push_back(y);
        assert(_model.defined(y) && !_model.value(y));
      }
    }
  }

  /* to powinno jeszcze zwracac -1 
   * gdy zmienna na poziomie 0 od razu daje sprzecznosc
   * prosty fix na jutro
   */

  add_clause(clause);
  return ret_to_lvl;
}

void UnitPropagator::revert(int decision_level) {
  for (Variable i : _model.variables())
    if (_level[i.index()] >= decision_level) {
      _model.unset(i);
      _level[i.index()] = -1;
      _reason[i.index()] = -1;
    }

  if (decision_level < static_cast<int>(_deductions.size()))
    _deductions.resize(decision_level);

  _failed = false;
  recheck();
}

std::pair<bool, Literal> UnitPropagator::extract_nonroot_literal(Clause *c) {
  int place = -1;
  for (int i = 0; i < static_cast<int>(c->size()); ++i) {
    if (_reason[c->at(i).variable().index()] != -1) {
      place = i;
      break;
    }
  }
  if (place == -1) return std::pair<bool, Literal>(false, Literal());
  std::swap(c->at(place), c->back());
  Literal x = c->back();
  c->pop_back();
  return std::pair<bool, Literal>(true, x);
}

void UnitPropagator::recheck() {
  bool change;
  do {
    change = false;
    int ind = -1;
    for (Clause c : _clauses) {
      ++ind;
      if (!_model.satisfied(c)) {
        int cnt = 0;
        Literal x;
        int lvl = -1;
        for (Literal v : c) {
          lvl = std::max(lvl, _level[v.variable().index()]);
          if (!_model.defined(v)) {
            ++cnt;
            x = v;
          }
        }
        if (cnt == 0) {
          _failed = true;
        }
        if (cnt == 1) {
          change = true;
          _reason[x.variable().index()] = ind;
          _level[x.variable().index()] = lvl;
          _deductions.back().push_back(x);
          _model.set(x);
        }
      }
    }
  } while (change);
}
