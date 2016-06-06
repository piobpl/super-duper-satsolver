/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

void UnitPropagator::add_clause(const Clause &clause) {
  int c = static_cast<int>(_clause_index.size());
  _clauses.emplace_back(clause, c);
  _clause_index.push_back(c);
  _is_reason.push_back(false);
  _watchers.push_back(std::make_pair(-1, -1));

  calculate_watchers(c);

  recheck();
}

void UnitPropagator::assume(Literal lit) {
  _level[lit.variable().index()] = static_cast<int>(_deductions.size());
  _deductions.push_back(std::vector<Literal>{lit});
  _model.set(lit);
  _propagation_queue.push(lit);
  recheck();
}

int UnitPropagator::diagnose() {
  Clause clause;
  for (const std::pair<Clause, int> &clause_it : _clauses) {
    if (_model.spoiled(clause_it.first)) {
      clause = clause_it.first;
      break;
    }
  }

  int ret_to_lvl = -1;
  std::vector<bool> was_here;
  was_here.resize(_model.variable_count(), false);
  for (Literal x : clause) {
    int i = x.variable().index();
    was_here[i] = true;
    ret_to_lvl = std::max(ret_to_lvl, _level[i]);
  }

  bool got;
  Literal x;
  while (std::tie(got, x) = extract_nonroot_literal(&clause), got) {
    Clause& reso = _clauses[_clause_index[_reason[x.variable().index()]]].first;
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
  int c = static_cast<int>(_clause_index.size());
  _clauses.emplace_back(clause, c);
  _clause_index.push_back(c);
  _is_reason.push_back(false);
  _watchers.push_back(std::make_pair(-1, -1));
  _finished.push_back(c);
  return ret_to_lvl;
}

void UnitPropagator::revert(int decision_level) {
  for (Variable i : _model.variables())
    if (_level[i.index()] >= decision_level) {
      _model.unset(i);
      _level[i.index()] = -1;
      _is_reason[_reason[i.index()]] = false;
      _reason[i.index()] = -1;
    }

  if (decision_level < static_cast<int>(_deductions.size()))
    _deductions.resize(decision_level);

  _failed = false;

  std::vector<int> recalculate = _finished;
  _finished.clear();

  for (int c : recalculate) {
    const Clause &clause = _clauses[_clause_index[c]].first;
    if (_watchers[c].first != -1) {
      Literal a = clause[_watchers[c].first];
      Literal b = clause[_watchers[c].second];
      if ((!_model.defined(a) || _model.value(a)) &&
          (!_model.defined(b) || _model.value(b))) {
        _clauses_with_literal[a.index()].insert(c);
        _clauses_with_literal[b.index()].insert(c);
      } else {
        calculate_watchers(c);
      }
    } else {
      calculate_watchers(c);
    }
  }

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
  while (!_propagation_queue.empty()) {
    Literal lit = -_propagation_queue.front();
    _propagation_queue.pop();
    int lit_index = lit.index();
    for (int c : _clauses_with_literal[lit_index]) {
      bool found = false;
      int from = _watchers[c].second + 1;
      const Clause &clause = _clauses[_clause_index[c]].first;
      int size = static_cast<int>(clause.size());
      for (int i = 0; i < size - 1; i++) {
        if (from + i == size) from -= size;
        if (from + i == _watchers[c].first) continue;
        Literal new_watcher = clause[from+i];
        if (!_model.defined(new_watcher) || _model.value(new_watcher)) {
          if (lit == clause[_watchers[c].first])
            _watchers[c].first = _watchers[c].second;
          _watchers[c].second = from+i;
          _clauses_with_literal[new_watcher.index()].insert(c);
          found = true;
          break;
        }
      }
      if (!found) {
        Literal deducted = clause[_watchers[c].first];
        if (lit == clause[_watchers[c].first])
          deducted = clause[_watchers[c].second];
        _clauses_with_literal[deducted.index()].erase(c);
        _finished.push_back(c);
        if (_model.defined(deducted) && !_model.value(deducted))
          _failed = true;
        else if (!_model.defined(deducted))
          propagation_push(deducted, c);
      }
    }
    _clauses_with_literal[lit_index].clear();
  }
}

void UnitPropagator::propagation_push(Literal var, int c) {
  if (_model.defined(var)) {
    if (!_model.value(var))
      _failed = true;
    return;
  }
  int max_level = 0;
  for (Literal lit : _clauses[_clause_index[c]].first)
    max_level = std::max(max_level, _level[lit.variable().index()]);
  _level[var.variable().index()] = max_level;
  _deductions[max_level].push_back(var);
  _is_reason[c] = true;
  _reason[var.variable().index()] = c;
  _model.set(var);
  _propagation_queue.push(var);
}

void UnitPropagator::calculate_watchers(int c) {
  int st = -1, nd = -1;
  const Clause &clause = _clauses[_clause_index[c]].first;

  for (int i = 0; i < static_cast<int>(clause.size()); ++i) {
    Literal x = clause[i];
    if (!_model.defined(x) || _model.value(x)) {
      if (st == -1) {
        st = i;
      } else {
        nd = i;
        break;
      }
    }
  }

  if (nd != -1) {
    _watchers[c] = std:: make_pair(st, nd);
    _clauses_with_literal[clause[st].index()].insert(c);
    _clauses_with_literal[clause[nd].index()].insert(c);
  } else {
    _finished.push_back(c);
    if (st != -1)
      propagation_push(clause[st], c);
    else
      _failed = true;
  }
}

std::vector<std::pair<Clause, int>> UnitPropagator::available_clauses() const {
  std::vector<std::pair<Clause, int>> clauses;
  for (const std::pair<Clause, int> &clause_it : _clauses) {
    if (!_is_reason[clause_it.second]) {
      clauses.push_back(clause_it);
    }
  }
  return clauses;
}

void UnitPropagator::forget_clause(int c) {
  assert(_clause_index.at(c) != -1 && !_is_reason.at(c));
  int ci = _clause_index[c];

  for (Literal lit : _clauses[ci].first) {
    _clauses_with_literal[lit.index()].erase(c);
  }

  int cl = _clauses.back().second;
  _clauses[ci] = _clauses.back();
  _clauses.pop_back();
  _clause_index[c] = -1;
  _clause_index[cl] = ci;
}
