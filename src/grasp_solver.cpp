/* Copyright 2016 Authors */

#include "../src/grasp_solver.h"

#include <vector>

void GraspSolver::decide(int dl) {
  const Model& model = up.model();
  if (std::all_of(clauses.begin(), clauses.end(),
      [&model](Clause& c) { return model.satisfied(c); })) {
    for (int i=1; i <= variables; ++i) {
      if (!model.is_set(i)) {
         up.assume(i, true, dl);
      }
    }
    return;
  }
  for (Clause clause : clauses) {
    int witness;
    if (model.ambivalent(clause, &witness)) {
      if (clause.has(witness)) {
        up.assume(witness, true, dl);
      } else if (clause.has(-witness)) {
        up.assume(witness, false, dl);
      } else {
        exit(-1);
      }
      return;
    }
  }
  assert(false);
}

void GraspSolver::solve(std::vector<Clause> _clauses) {
  clauses = _clauses;
  up.add_clauses(clauses);
  const Model& model = up.model();

  if (!up.propagate(-1)) {
      solved = false;
      return;
  }
  // decision level
  int dl = 0;
  while (!model.all_assigned()) {
    std::cerr << "current model " << model << std::endl;
    decide(dl);
    dl++;
    if (!up.propagate(dl)) {
      int beta = up.diagnose();
      if (beta < 0) {
        solved = false;
        return;
      } else {
        up.backtrack(beta);
        dl = beta;
      }
    }
    std::cerr << std::endl;
  }
  solved = true;
}
