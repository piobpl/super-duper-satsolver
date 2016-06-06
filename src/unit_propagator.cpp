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
  int c = static_cast<int>(_clauses.size());
  _clauses.push_back(clause);
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
  for (Clause clause_it : _clauses) {
    if (_model.spoiled(clause_it)) {
      clause = clause_it;
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
  _clauses.push_back(clause);
  _watchers.push_back(std::make_pair(-1, -1));
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

  for (Variable i : _model.variables()) {
    _clauses_with_literal[(+i).index()].clear();
    _clauses_with_literal[(-i).index()].clear();
  }

  for (int c = 0; c < static_cast<int>(_clauses.size()); ++c) {
    if (_watchers[c].first != -1) {
      Literal a = _clauses[c][_watchers[c].first];
      Literal b = _clauses[c][_watchers[c].second];
      if ((!_model.defined(a) || _model.value(a)) &&
          (!_model.defined(b) || _model.value(b))) {
        _clauses_with_literal[a.index()].push_back(c);
        _clauses_with_literal[b.index()].push_back(c);
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
      int size = static_cast<int>(_clauses[c].size());
      for (int i = 0; i < size - 1; i++) {
        if (from + i == size) from -= size;
        if (from + i == _watchers[c].first) continue;
        Literal new_watcher = _clauses[c][from+i];
        if (!_model.defined(new_watcher) || _model.value(new_watcher)) {
          if (lit == _clauses[c][_watchers[c].first])
            _watchers[c].first = _watchers[c].second;
          _watchers[c].second = from+i;
          _clauses_with_literal[new_watcher.index()].push_back(c);
          found = true;
          break;
        }
      }
      if (!found) {
        Literal deducted = _clauses[c][_watchers[c].first];
        if (lit == _clauses[c][_watchers[c].first])
          deducted = _clauses[c][_watchers[c].second];
        if (_model.defined(deducted) && !_model.value(deducted))
          _failed = true;
        else if (!_model.defined(deducted))
          propagation_push(deducted, c);
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

  for (int i = 0; i < static_cast<int>(_clauses[c].size()); ++i) {
    Literal x = _clauses[c][i];
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
    _clauses_with_literal[_clauses[c][st].index()].push_back(c);
    _clauses_with_literal[_clauses[c][nd].index()].push_back(c);
  } else {
    if (st != -1)
      propagation_push(_clauses[c][st], c);
    else
      _failed = true;
  }
}

std::vector<int> UnitPropagator::redundant(){
  std::vector<bool> red(_clauses.size(), true);
  std::cerr << "iterating over vars" << std::endl << std::flush;
  for (Variable i : _model.variables()){
    std::cerr << "index = " << i.index() << " reason=" << _reason[i.index()] << std::endl << std::flush;
	if(_reason[i.index()] != -1){
      red[_reason[i.index()]] = false;
	}
  }
  std::vector<int> res;
  std::cerr << "iterating over _base cl " << _base_clauses << std::endl << std::flush;
  for (int i = _base_clauses; i < static_cast<int>(_clauses.size()); ++i)
    if (red[i])
      res.push_back(i);
  return res;
}

void UnitPropagator::forget(const std::vector<int>& ind) {
  for (int i : ind) {
	for (Variable v : _model.variables())
	  assert(_reason[v.index()] != i);
	assert(i >= _base_clauses);
  }
  
  int i = 0;
  int t = _base_clauses;
  for (int c = _base_clauses; c < static_cast<int>(_clauses.size()); ++c){
    while (i < static_cast<int>(ind.size()) && ind[i] < c) ++i;
    if (i == static_cast<int>(ind.size()) || ind[i] > c) {
      if (t != c) {
        _clauses[t] = _clauses[c];
        _watchers[t] = _watchers[c];
        ++t;
      }
    }
  }

  _watchers.resize(t);
  _clauses.resize(t);

  for (Variable v : _model.variables()) {
    _clauses_with_literal[(+v).index()].clear();
    _clauses_with_literal[(-v).index()].clear();
  }

  for (int c = 0; c < static_cast<int>(_clauses.size()); ++c) {
    /*if (_watchers[c].first != -1) {
      Literal a = _clauses[c][_watchers[c].first];
      Literal b = _clauses[c][_watchers[c].second];
      if ((!_model.defined(a) || _model.value(a)) &&
          (!_model.defined(b) || _model.value(b))) {
        _clauses_with_literal[a.index()].push_back(c);
        _clauses_with_literal[b.index()].push_back(c);
      } else {
        calculate_watchers(c);
      }
    } else {
      calculate_watchers(c);
    }*/
	calculate_watchers(c);
  }
}

int UnitPropagator::glucose_factor(const Clause& cl) {
  std::set<int> ids;
  for (const Literal& l : cl) {
    if (_model.defined(l.variable())) {
      ids.insert(_reason[l.index()]);
    }
  }
  return static_cast<int>(ids.size());
};

bool UnitPropagator::garbage_clauses_glucose() {
  if(static_cast<int>(_clauses.size() < MAX_CLAUSES_NUM)){
	  return true;
  }
  int to_remove = (_clauses.size() - MAX_CLAUSES_NUM/2);
  std::cerr << "BEFORE redundant()" << std::endl << std::flush;
  auto av_cl = redundant();
  
  std::cerr << "after redundant()" << std::endl << std::flush;
  std::vector<std::pair<int, int> > arr_to_sort;
  std::cerr << "is_set" << std::endl << std::flush;
  std::cerr << "iterating over indices" << std::endl << std::flush;
  for (int ind : av_cl) {
    Clause& cl = _clauses[ind];
    assert(ind >= _base_clauses);
    arr_to_sort.push_back(
    std::pair<int, int> (ind, glucose_factor(cl)));     
  } 
  std::cerr << "BEFORE sort()" << std::endl << std::flush;
  std::sort(arr_to_sort.begin(), arr_to_sort.end(),
  [](std::pair<int, int> x, std::pair<int, int> y)
                      {return x.second > y.second;});
  int i;
  std::vector<int> to_rem;
  for (i=0; i < to_remove && i<static_cast<int>(arr_to_sort.size()); i++) {
    to_rem.push_back(arr_to_sort[i].first);
  }
  std::sort(to_rem.begin(), to_rem.end());
  forget(to_rem);
  std::cerr << "AFTER forget()" << std::endl << std::flush;
  return true;
  /*
  if (static_cast<int>(_clauses.size()) < MAX_CLAUSES_NUM) {
    return true;
  }

  int to_remove = (static_cast<int>(_clauses.size()) - MAX_CLAUSES_NUM/2);
  auto avail_cl = available_clauses();
  std::vector<std::pair<int, int> > arr_to_sort;
  assert(clauses_num_at_start != -1);
  for (int ind : avail_cl) {
    Clause& cl = _clauses[ind];
    assert(ind >= clauses_num_at_start);
    arr_to_sort.push_back(
    std::pair<int, int> (ind, glucose_factor(cl)));
  }
  std::sort(arr_to_sort.begin(), arr_to_sort.end(),
  [](std::pair<int, int> x, std::pair<int, int> y)
                      {return x.second > y.second;});
  int i;
  for (i=0; i < to_remove && i<static_cast<int>(arr_to_sort.size()); i++) {
    forget_clause(arr_to_sort[i].first);
  }
  if ( i == static_cast<int>(arr_to_sort.size()) ) {
      std::cerr << "RESIZING: " << MAX_CLAUSES_NUM << std::endl;
	  MAX_CLAUSES_NUM *= 2;
  }
  return true;*/
}

/*bool UnitPropagator::garbage_clauses_grasp() {
  if (static_cast<int>(_clauses.size()) < MAX_CLAUSES_NUM) {
    return true;
  }
  auto avail_cl = available_clauses();
  std::vector<std::pair<int, int> > arr_to_sort;
  assert(clauses_num_at_start != -1);
  for (int ind : avail_cl) {
    Clause& cl = _clauses[ind];
    assert(ind >= clauses_num_at_start);
    if (cl.size() > 8) {
      forget_clause(ind);
    }
  }
  return true;
}*/
