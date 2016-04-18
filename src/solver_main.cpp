/* Copyright 2016 Authors */

#include <algorithm>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>

#include "../src/clause.h"
#include "../src/input.h"
#include "../src/random_solver.h"
#include "../src/grasp_solver.h"

int main() {
  int n;
  std::vector<Clause> clauses;

  Input::read(stdin, &n, &clauses);

  GraspSolver solver = GraspSolver(n);
  solver.solve(clauses);

  std::cout << solver.solution() << std::endl;
  return 0;
}
