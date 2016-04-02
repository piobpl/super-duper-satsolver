/* Copyright 2016 Authors */

#ifndef SRC_RANDOM_SOLVER_H_
#define SRC_RANDOM_SOLVER_H_

#include <algorithm>
#include <deque>
#include <random>
#include <vector>

#include "../src/clause.h"
#include "../src/solver.h"

class RandomSolver : public Solver {
  std::random_device r;
  std::default_random_engine e;
  std::uniform_int_distribution<int> u;

  int variables;
  std::vector<Clause> clauses;
  Model model;
  bool solved;

  int limit;
  std::deque<int> order;

  bool boolean() {
    return u(e);
  }

 public:
  explicit RandomSolver(int _variables, int lim)
      : e(r()),
        u(0, 1),
        variables(_variables),
        model(variables),
        limit(lim) {}

  void solve(std::vector<Clause> _clauses) override {
    clauses = _clauses;
    int m = static_cast<int>(clauses.size());

    solved = 0;

    for (int i = 1; i <= variables; ++i)
      model.set(i, boolean());

    for (int i = 0; i < m; ++i)
      order.push_back(i);
    std::random_shuffle(order.begin(), order.end());

    int p = 0;
    int cnt = 0;
    while (p < m) {
      ++cnt;
      if (cnt == limit) return;

      int c = order[p];
      if (model.satisfied(clauses[c])) {
        ++p;
      } else {
        order.erase(order.begin() + p);
        order.push_front(c);
        p = 0;
        for (int i = 1; i <= variables; ++i)
          if (clauses[c].has(i) || clauses[c].has(-i))
            model.set(i, boolean());
      }
    }
    solved = 1;
  }

  bool success() override {
    return solved;
  }

  Model solution() override {
    return model;
  }
};

#endif  // SRC_RANDOM_SOLVER_H_
