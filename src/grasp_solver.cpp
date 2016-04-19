/* Copyright 2016 Authors */

#include "../src/grasp_solver.h"

#include <vector>

void GraspSolver::decide() {
  const Model& model = up.model();
  if (std::all_of(clauses.begin(), clauses.end(),
      [&model](Clause& c) { return model.satisfied(c); })) {
    for (int i=1; i <= variables; ++i) {
      if (!model.defined(Variable(i))) {
         up.assume(Literal(i));
      }
    }
    return;
  }
  for (Clause clause : clauses) {
    if (model.spoiled(clause)) {
      std::cerr << "ERROR" << std::endl;
    }
    bool ambivalent;
    Literal witness;
    std::tie(ambivalent, witness) = model.ambivalent(clause);
    if (ambivalent) up.assume(witness);
  }
  assert(false);
}

void GraspSolver::solve(std::vector<Clause> _clauses) {
  clauses = _clauses;
  up.add_clauses(clauses);
  const Model& model = up.model();

  /*if (!up.propagate(-1)) {
      solved = false;
      return;
  }*/
  // decision level
  while (!model.all_assigned()) {
    decide();
    while (up.failed()) {
      int beta = up.diagnose();
      if (beta < 0) {
        solved = false;
        return;
      } else {
        up.revert(beta);
      }
    }
    std::cerr << std::endl;
  }
  solved = true;
}
