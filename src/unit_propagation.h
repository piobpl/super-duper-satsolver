/* Copyright 2016 Authors */

#ifndef SRC_UNIT_PROPAGATION_H_
#define SRC_UNIT_PROPAGATION_H_

#include <queue>
#include <vector>

#include "../src/clause.h"
#include "../src/model.h"

class UnitPropagation {
 private:
  class ObservedClause {
   public:
    Clause clause;
    std::array<Clause::Iterator, 2> observer;
    explicit ObservedClause(Clause _clause)
      : clause(_clause), observer{clause.begin(), ++clause.begin()} {}
  };

  std::vector<ObservedClause> clauses;
  bool failed;
  int max_var;
  Model model;
  std::queue<int> set_literals;
  std::vector<std::vector<int>> _literals_occur;

  std::vector<int>& literals_occur(int v) {
    return _literals_occur[v + max_var];
  }

 public:
  explicit UnitPropagation(int _max_var) : max_var(_max_var) {}

  void add_clauses(const std::vector<Clause> &new_clauses) {
    for (Clause clause : new_clauses) {
      add_clause(clause);
    }
  }

  void add_clause(const Clause &clause) {
    if (clause.size() == 0) {
      failed = true;
      return;
    }
    if (clause.size() == 1) {
      int L = *clause.begin();
      if (model.isset(L)) {
        if (!model.value(L)) failed = true;
        return;
      }
      model.set(L, true);
      set_literals.push(L);
      return;
    }
    ObservedClause c(clause);
    clauses.push_back(c);
    literals_occur(
      *c.observer[0]).push_back(static_cast<int>(clauses.size()) - 1);
    literals_occur(
      *c.observer[1]).push_back(static_cast<int>(clauses.size()) - 1);
  }

  bool has_failed() {
    return failed;
  }

  Model get_model() {
    return model;
  }

  bool propagate() {
    if (has_failed()) return false;
    while (!set_literals.empty()) {
      int L = set_literals.front();
      set_literals.pop();
      for (int clause_no : literals_occur(-L)) {
        if (*clauses[clause_no].observer[0] == -L) {
          clauses[clause_no].observer[0] = clauses[clause_no].observer[1];
        }
        Clause::Iterator iter = ++clauses[clause_no].observer[1];
        while (iter != clauses[clause_no].clause.end()) {
          if (!model.isset(*iter) || model.value(*iter)) break;
          ++iter;
        }
        int H = *clauses[clause_no].observer[0];
        if (iter != clauses[clause_no].clause.end()) {
          clauses[clause_no].observer[1] = iter;
          literals_occur(*iter).push_back(clause_no);
        } else if (model.isset(H) && !model.value(H)) {
          failed = true;
          return false;
        } else if (!model.isset(H)) {
          model.set(H, true);
          set_literals.push(H);
        }
      }
    }
    return true;
  }
};

#endif  // SRC_UNIT_PROPAGATION_H_
