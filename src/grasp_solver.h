/* Copyright 2016 Authors */

#ifndef SRC_GRASP_SOLVER_H_
#define SRC_GRASP_SOLVER_H_

#include <algorithm>
#include <deque>
#include <random>
#include <vector>

#include "../src/model.h"
#include "../src/solver.h"
#include "../src/unit_propagator.h"

class GraspSolver : public Solver {
  int variables;
  UnitPropagator up;
  std::vector<Clause> clauses;
  bool solved;

  void decide(int dl);

 public:
  explicit GraspSolver(int _variables): variables(_variables), up(_variables) {}

  bool success() override {
    return solved;
  }

  Model solution() override {
    return up.model();
  }

  const Model& solution_ref() {
    return up.model();
  }

  void solve(std::vector<Clause> _clauses) override;
};

#endif  // SRC_GRASP_SOLVER_H_
