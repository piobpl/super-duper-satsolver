/* Copyright 2016 Authors */

#include <algorithm>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>

#include "../src/input.h"
#include "../src/grasp_solver.h"

int main() {
  int n;
  std::vector<Clause> clauses;

  Input::read(stdin, &n, &clauses);

  GraspSolver solver = GraspSolver(n, static_cast<int>(clauses.size()));
  solver.solve(clauses);

  if (solver.success())
    std::cout << solver.solution() << std::endl;
  else
    std::cout << "UNSATISFIABLE" << std::endl;
  return 0;
}
