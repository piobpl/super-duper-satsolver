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

void UnitPropagator::add_clause(const Clause &clause, bool run_recheck) {
  int c = static_cast<int>(_clauses.size());
  _clauses.push_back(clause);
  _watchers.push_back(std::make_pair(-1, -1));
  _satisfied_at.push_back(std::numeric_limits<int>::max());

  calculate_watchers(c);

  if (run_recheck) recheck();
}

void UnitPropagator::assume(Literal lit) {
  _level[lit.variable().index()] = static_cast<int>(_deductions.size());
  _deductions.push_back(std::vector<Literal>{lit});
  _satisfied.push_back(std::vector<int>{});
  _model.set(lit);
  _propagation_queue.push(lit);
  recheck();
}

int UnitPropagator::diagnose() {
  Clause clause;
  for (Clause &clause_it : _clauses) {
    if (_model.spoiled(clause_it)) {
      bump_var_activity(clause_it);
      bump_clause_activity(&clause_it);
      clause = clause_it;
      break;
    }
  }
  int ret_to_lvl = -1;
  std::vector<bool> was_here;
  std::vector<Literal> blacks;
  std::vector<int> top_count;
  was_here.resize(_model.variable_count(), false);
  top_count.resize(_model.variable_count(), 0);
  for (Literal x : clause) {
    int i = x.variable().index();
    ret_to_lvl = std::max(ret_to_lvl, _level[i]);
  }

  for (Literal x : clause) {
      if (_level[x.variable().index()] == ret_to_lvl) {
      blacks.push_back(x);
      was_here[x.variable().index()] = true;
    }
  }
  // TOP SORT

  while (blacks.size() > 0) {
    Literal bl = blacks.back();
    blacks.pop_back();
    if (_reason[bl.variable().index()] == -1) {
      continue;
    }
    const Clause& temp_edges = _clauses[_reason[bl.variable().index()]];

    for (Literal x : temp_edges) {
      if (_level[x.variable().index()] == ret_to_lvl
          && bl.variable() != x.variable()) {
        int x_ind = x.variable().index();
        top_count[x_ind]++;
        if (!was_here[x_ind]) {
          was_here[x_ind] = true;
          blacks.push_back(x);
        }
      }
    }
  }

  int temp_index = 0;
  std::vector<int> top_index;
  std::vector<int> top_queue;
  top_index.resize(_model.variable_count(), -1);
  for (int i = 0; i < _model.variable_count(); ++i) {
    if (was_here[i]) {
      if (top_count[i] == 0) {
        top_index[i] = (temp_index++);
        top_queue.push_back(i);
      }
    }
  }

  while (top_queue.size() > 0) {
    int reso = _reason[top_queue.back()];
    top_queue.pop_back();
    if (reso == -1) {
      continue;
    }
    Clause& cl = _clauses[reso];
    for (Literal x : cl) {
      int x_ind = x.variable().index();
      top_count[x_ind]--;
      if (top_count[x_ind] == 0) {
        top_index[x_ind] = (temp_index++);
        top_queue.push_back(x_ind);
      }
    }
  }
  was_here.resize(0); was_here.resize(_model.variable_count(), false);
  for (Literal x : clause) {
    was_here[x.variable().index()] = true;
  }
  while (true) {
    int min_ind = -1, count = 0;
    int in_cl_index = -1, it = 0;
    for (Literal x : clause) {
      int x_ind = x.variable().index();
      if (_level[x_ind] == ret_to_lvl) {
        ++count;
        if ((min_ind == -1) || (top_index[min_ind] > top_index[x_ind])) {
          min_ind = x_ind;
          in_cl_index = it;
        }
      }
      it++;
    }
    if (count == 1) {
      break;
    }
    std::swap(clause.back(), clause.at(in_cl_index));
    Literal to_remove = clause.back();
    clause.pop_back();

    assert(_reason[to_remove.variable().index()] != -1);
    for (Literal x : _clauses[_reason[to_remove.variable().index()]]) {
      int x_ind = x.variable().index();
      if (!was_here[x_ind] && x.variable() != to_remove.variable()) {
        was_here[x_ind] = true;
        clause.push_back(x);
      }
    }
  }

  bump_var_activity(clause);

  decay_var_activity();
  decay_clause_activity();

  add_clause(clause, false);
  return ret_to_lvl;
}

Variable UnitPropagator::active_variable() {
  auto vars = _model.variables();
  return *std::max_element(vars.begin(), vars.end(),
      [this](Variable v1, Variable v2) {
        return (_model.defined(v1) && !_model.defined(v2)) ||
            (_model.defined(v1) ==_model.defined(v2)
            && _activity[v1.index()] < _activity[v2.index()]);
      });
}

void UnitPropagator::bump_var_activity(const Clause &clause) {
  for (const auto &l : clause) {
    if ((_activity[l.variable().index()] += _var_inc) > RESCALE_THRESHOLD) {
      for (int i = 0; i < static_cast<int>(_activity.size()); i++) {
        _activity[i] /= RESCALE_THRESHOLD;
      }
      _var_inc /= RESCALE_THRESHOLD;
    }
  }
}

void UnitPropagator::decay_var_activity() {
  _var_inc /= VAR_DECAY;
}

void UnitPropagator::rebuild() {
  _failed = false;

  while (!_propagation_queue.empty()) _propagation_queue.pop();

  for (Variable i : _model.variables()) {
    _clauses_with_literal[(+i).index()].clear();
    _clauses_with_literal[(-i).index()].clear();
  }

  std::vector<int> rc = _unsatisfied;
  _unsatisfied.clear();

  for (int c : rc) {
    if (_satisfied_at[c] >= static_cast<int>(_deductions.size())) {
      _satisfied_at[c] = std::numeric_limits<int>::max();
      calculate_watchers(c);
    } else {
      _satisfied[_satisfied_at[c]].push_back(c);
    }
  }
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

  while (decision_level < static_cast<int>(_satisfied.size())) {
    _unsatisfied.insert(_unsatisfied.end(),
      _satisfied.back().begin(),
      _satisfied.back().end());
    _satisfied.pop_back();
  }

  rebuild();

  recheck();
}

void UnitPropagator::recheck() {
  if (_failed) return;
  while (!_propagation_queue.empty()) {
    Literal lit = _propagation_queue.front();
    _propagation_queue.pop();
    int lit_index = lit.index();
    for (int c : _clauses_with_literal[lit_index]) {
      _satisfied_at[c] =
        std::min(_satisfied_at[c], _level[lit.variable().index()]);
    }
    lit = -lit;
    lit_index = lit.index();
    for (int c : _clauses_with_literal[lit_index]) {
      bool found = false;
      int size = static_cast<int>(_clauses[c].size());
      for (int i = _watchers[c].second + 1; i < size; i++) {
        Literal new_watcher = _clauses[c][i];
        if (!_model.defined(new_watcher) || _model.value(new_watcher)) {
          if (_model.defined(new_watcher)) {
            _satisfied_at[c] =
              std::min(_satisfied_at[c],
                _level[new_watcher.variable().index()]);
          } else {
            if (lit == _clauses[c][_watchers[c].first])
              _watchers[c].first = _watchers[c].second;
            _watchers[c].second = i;
            _clauses_with_literal[new_watcher.index()].push_back(c);
          }
          found = true;
          break;
        }
      }
      if (!found) {
        Literal deducted = _clauses[c][_watchers[c].first];
        if (lit == _clauses[c][_watchers[c].first])
          deducted = _clauses[c][_watchers[c].second];
        if (_model.defined(deducted) && !_model.value(deducted)) {
          _failed = true;
          return;
        } else if (!_model.defined(deducted)) {
          propagation_push(deducted, c);
        }
      }
    }
  }
}

void UnitPropagator::propagation_push(Literal var, int c) {
  if (_model.defined(var)) {
    if (!_model.value(var))
      _failed = true;
    return;
  }
  int max_level = 0;
  for (Literal lit : _clauses[c])
    max_level = std::max(max_level, _level[lit.variable().index()]);
  _level[var.variable().index()] = max_level;
  _deductions[max_level].push_back(var);
  _reason[var.variable().index()] = c;
  _model.set(var);
  _propagation_queue.push(var);
}

void UnitPropagator::calculate_watchers(int c) {
  int st = -1, nd = -1;

  bool sat = false;
  for (int i = 0; i < static_cast<int>(_clauses[c].size()); ++i) {
    Literal x = _clauses[c][i];
    if (!_model.defined(x) || _model.value(x)) {
      if (_model.defined(x)) {
        _satisfied_at[c] = _level[x.variable().index()];
        sat = true;
        break;
      }
      if (st == -1) {
        st = i;
      } else {
        nd = i;
        break;
      }
    }
  }
  if (sat) {
    _satisfied[_satisfied_at[c]].push_back(c);
    return;
  }
  _unsatisfied.push_back(c);

  if (nd != -1) {
    _watchers[c] = std:: make_pair(st, nd);
    _clauses_with_literal[_clauses[c][st].index()].push_back(c);
    _clauses_with_literal[_clauses[c][nd].index()].push_back(c);
  } else {
    if (st != -1)
      propagation_push(_clauses[c][st], c);
    else
      _failed = true;
  }
}

std::vector<int> UnitPropagator::redundant() {
  std::vector<bool> red(_clauses.size(), true);
  for (Variable i : _model.variables()) {
    if (_reason[i.index()] != -1) {
        red[_reason[i.index()]] = false;
    }
  }
  std::vector<int> res;
  for (int i = _base_clauses; i < static_cast<int>(_clauses.size()); ++i)
    if (red[i])
      res.push_back(i);
  return res;
}

void UnitPropagator::forget(const std::vector<int>& ind) {
  std::vector<int> new_id(_clauses.size(), -1);

  int i = 0;
  int t = _base_clauses;
  for (int c = _base_clauses; c < static_cast<int>(_clauses.size()); ++c) {
    while (i < static_cast<int>(ind.size()) && ind[i] < c) ++i;
    if (i == static_cast<int>(ind.size()) || ind[i] > c) {
      new_id[c] = t;
      if (t != c) {
        _clauses[t] = _clauses[c];
        _watchers[t] = _watchers[c];
        _satisfied_at[t] = _satisfied_at[c];
      }
      ++t;
    }
  }

  _clauses.resize(t);
  _watchers.resize(t);
  _satisfied_at.resize(t);

  for (Variable v : _model.variables()) {
    if (_reason[v.index()] != -1 && _reason[v.index()] >= _base_clauses) {
      assert(new_id[_reason[v.index()]] != -1);
      _reason[v.index()] = new_id[_reason[v.index()]];
    }
  }

  i = 0;
  while (i < static_cast<int>(_unsatisfied.size())) {
    if (_unsatisfied[i] >= _base_clauses) {
      if (new_id[_unsatisfied[i]] == -1) {
        std::swap(_unsatisfied[i], _unsatisfied.back());
        _unsatisfied.pop_back();
      } else {
        _unsatisfied[i] = new_id[_unsatisfied[i]];
        ++i;
      }
    } else {
      ++i;
    }
  }

  for (int d = 0; d < static_cast<int>(_satisfied.size()); ++d) {
    i = 0;
    while (i < static_cast<int>(_satisfied[d].size())) {
      if (_satisfied[d][i] >= _base_clauses) {
        if (new_id[_satisfied[d][i]] == -1) {
          std::swap(_satisfied[d][i], _satisfied[d].back());
          _satisfied[d].pop_back();
        } else {
          _satisfied[d][i] = new_id[_satisfied[d][i]];
          ++i;
        }
      } else {
        ++i;
      }
    }
  }

  rebuild();
}

int UnitPropagator::glucose_factor(const Clause& cl) {
  std::set<int> ids;
  for (const Literal& l : cl) {
    if (_model.defined(l.variable())) {
      ids.insert(_reason[l.variable().index()]);
    }
  }
  return static_cast<int>(ids.size());
}

bool UnitPropagator::garbage_clauses_all() {
  forget(redundant());
  return true;
}

bool UnitPropagator::garbage_clauses_glucose() {
  if (static_cast<int>(_clauses.size()) < MAX_CLAUSES_NUM) {
    return true;
  }
  int to_remove = (static_cast<int>(_clauses.size()) - MAX_CLAUSES_NUM/2);
  auto av_cl = redundant();

  std::vector<std::pair<int, int> > arr_to_sort;
  for (int ind : av_cl) {
    Clause& cl = _clauses[ind];
    assert(ind >= _base_clauses);
    arr_to_sort.push_back(
    std::pair<int, int> (ind, glucose_factor(cl)));
  }
  std::sort(arr_to_sort.begin(), arr_to_sort.end(),
  [](std::pair<int, int> x, std::pair<int, int> y)
                      {return x.second > y.second;});
  int i;
  std::vector<int> to_rem;
  for (i=0; i < to_remove && i<static_cast<int>(arr_to_sort.size()); i++) {
    to_rem.push_back(arr_to_sort[i].first);
  }
  if ( i== static_cast<int> (arr_to_sort.size()) ) {
    MAX_CLAUSES_NUM += MAX_CLAUSES_GROW;
  }
  std::sort(to_rem.begin(), to_rem.end());
  forget(to_rem);
  return true;
}

bool UnitPropagator::garbage_clauses_grasp() {
  if (static_cast<int>(_clauses.size()) < MAX_CLAUSES_NUM) {
    return true;
  }
  auto avail_cl = redundant();
  std::vector<int> indices;
  std::vector<std::pair<int, int> > arr_to_sort;
  for (int ind : avail_cl) {
    Clause& cl = _clauses[ind];
    if (cl.size() > 3) {
      indices.push_back(ind);
    }
    forget(indices);
  }
  return true;
}

void UnitPropagator::garbage_clauses_minisat() {
  if (static_cast<int>(_clauses.size()) < MAX_CLAUSES_NUM) {
    return;
  }
  auto learnts = redundant();
  std::sort(learnts.begin(), learnts.end(), [this](int i, int j) {
    return _clauses[i].size() > 2 && (_clauses[j].size() == 2 ||
      _clauses[i].activity() < _clauses[j].activity());
  });
  learnts.resize(learnts.size() / 2);
  std::sort(learnts.begin(), learnts.end());
  forget(learnts);
}

void UnitPropagator::decay_clause_activity() {
  _cla_inc /= CLAUSE_DECAY;
}

void UnitPropagator::bump_clause_activity(Clause *c) {
  if ( (c->activity() += _cla_inc) > CLAUSE_RESCALE_THRESHOLD ) {
    for (Clause &d : _clauses) {
      d.activity() /= CLAUSE_RESCALE_THRESHOLD;
    }
    _cla_inc /= CLAUSE_RESCALE_THRESHOLD;
  }
}
