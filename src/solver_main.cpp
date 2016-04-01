/* Copyright 2016 Authors */

#include <algorithm>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>

#include "../src/solver_type.h"
#include "../src/clause.h"
#include "../src/input.h"
#include "../src/random_solver.h"
#include "../src/grasp_solver.h"

int main() {
  int n;
  std::vector<Clause> clauses;

  Input::read(stdin, &n, &clauses);
  
  //Acording to define in solver_type.h which is untracked.
  //Now GraspSolver is default
  #ifdef _SOLVER_RANDOM
  RandomSolver rs(10000);
  Solver& solver = rs;
  #else
  GraspSolver gp(n);
  Solver& solver = gp;
  #endif

  do {
    solver.solve(n, clauses);
  }while(!solver.success());

  std::cout << solver.solution() << std::endl;
}
