/* Copyright 2016 Authors */

#include "../src/grasp_solver.h"

#include <tuple>
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
    assert(!model.spoiled(clause));

    bool ambivalent;
    Literal witness;
    std::tie(ambivalent, witness) = model.ambivalent(clause);
    if (ambivalent) {
      up.assume(witness);
      return;
    }
  }
  assert(false);
}

void GraspSolver::solve(std::vector<Clause> _clauses) {
  clauses = _clauses;
  up.add_clauses(clauses);
  const Model& model = up.model();

  while (!model.all_assigned()) {
    assert(!up.failed());
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
    assert(!up.failed());
  }
  solved = true;
}
