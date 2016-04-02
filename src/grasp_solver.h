/* Copyright 2016 Authors */

#ifndef SRC_GRASP_SOLVER_H_
#define SRC_GRASP_SOLVER_H_

#include <algorithm>
#include <deque>
#include <random>
#include <vector>

#include "../src/clause.h"
#include "../src/solver.h"
#include "../src/unit_propagator.h"

class GraspSolver : public Solver {
  int variables;
  UnitPropagator up = UnitPropagator(0);
  std::vector<Clause> clauses;
  bool solved;

  // returns true if all clauses are satisfied.
  // otherwise returns false and sets one variable to some value
  bool decide(int dl);

 public:
  explicit GraspSolver(int _variables): up(_variables) {}

  bool success() override {
    return solved;
  }

  Model solution() override {
    return up.get_model();
  }

  void solve(std::vector<Clause> _clauses) override;
};

#endif  // SRC_GRASP_SOLVER_H_
