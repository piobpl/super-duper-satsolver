/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <set>
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

void UnitPropagator::diagnose() {
}

void UnitPropagator::revert() {
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
          _reason[x.variable().index()] = cnt;
          _level[x.variable().index()] = lvl;
          _deductions.back().push_back(x);
          _model.set(x);
        }
      }
    }
  } while (change);
}
