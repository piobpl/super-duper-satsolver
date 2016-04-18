/* Copyright 2016 Authors */

#include "../src/grasp_solver.h"

#include <vector>

void GraspSolver::decide(int dl) {
  const Model& model = up.model();
  if (std::all_of(clauses.begin(), clauses.end(),
      [&model](Clause& c) { return model.satisfied(c); })) {
    for (int i=1; i <= variables; ++i) {
      if (!model.defined(Variable(i))) {
         up.assume(Literal(i), dl);
      }
    }
    return;
  }
  for (Clause clause : clauses) {
    if (model.spoiled(clause)) {
      std::cerr << "ERRROR" << std::endl;
    }
    bool ambivalent;
    Literal witness(1);
    std::tie(ambivalent, witness) = model.ambivalent(clause);
    if (ambivalent) up.assume(witness, dl);
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
  int dl = 0;
  while (!model.all_assigned()) {
    std::cerr << "current model " << model << std::endl << std::flush;
    std::cerr << up.clauses_size() << std::endl;
    decide(dl);
    dl++;
    while (up.failed()) {
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
