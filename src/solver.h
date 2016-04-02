/* Copyright 2016 Authors */

#ifndef SRC_SOLVER_H_
#define SRC_SOLVER_H_

#include <vector>

#include "../src/model.h"

class Solver {
 public:
  virtual void solve(std::vector<Clause> clauses) = 0;

  virtual bool success() = 0;

  virtual Model solution() = 0;
};

#endif  // SRC_SOLVER_H_
