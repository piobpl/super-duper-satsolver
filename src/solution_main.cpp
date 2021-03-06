/* Copyright 2016 Authors */

#include <algorithm>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>

#include "../src/input.h"
#include "../src/grasp_solver.h"

void solve() {
  int n;
  std::vector<Clause> clauses;

  Input::read(stdin, &n, &clauses);

  GraspSolver solver = GraspSolver(n, static_cast<int>(clauses.size()));
  solver.solve(clauses);

  if (solver.success()) {
    std::cout << "SAT" << std::endl;
    const Model &solution = solver.solution();
    for (Variable v : solution.variables())
      std::cout << (solution.value(v) ? +v : -v) << " ";
    std::cout << "0" << std::endl;
  } else {
    std::cout << "UNSAT" << std::endl;
  }
}

int main() {
//  int z;
//  scanf("%d ", &z);
//  while (z--) solve();
  solve();
  return 0;
}
