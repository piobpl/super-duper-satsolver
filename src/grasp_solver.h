/* Copyright 2016 Authors */

#ifndef SRC_GRASP_SOLVER_H_
#define SRC_GRASP_SOLVER_H_

#include <algorithm>
#include <deque>
#include <random>
#include <vector>

#include "../src/bohm.h"
#include "../src/model.h"
#include "../src/solver.h"
#include "../src/unit_propagator.h"

class GraspSolver : public Solver {
 public:
  explicit GraspSolver(int _variables, int _base_clauese)
    : variables(_variables), up(_variables, _base_clauese) {}

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

 private:
  int variables;
  UnitPropagator up;
  std::vector<Clause> clauses;
  bool solved;
  std::mt19937_64 mte;

  void pre_decide();
  void random_decide();
  void decide();
};

#endif  // SRC_GRASP_SOLVER_H_
