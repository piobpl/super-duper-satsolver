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

  Variable x = bohm.pick(model, clauses);
  up.assume(+x);
  return;
}

void GraspSolver::solve(std::vector<Clause> _clauses) {
  std::cerr << "SOLVING SHIT" << std::endl;
  clauses = _clauses;
  up.add_clauses(clauses);
  up.set_cl_num(static_cast<int>(clauses.size()));
  const Model& model = up.model();
  int i = 1;
  while (!model.all_assigned()) {
    up.garbage_clauses_grasp();
    assert(!up.failed());
    decide();
    while (up.failed()) {
      int beta = up.diagnose();
      ++i;
      if (beta == 0) {
        solved = false;
        std::cerr << up.available_clauses().size() << std::endl << std::flush;
        return;
      } else {
        up.revert(beta);
      }
    }
    assert(!up.failed());
  }
  std::cerr << up.available_clauses().size() << std::endl << std::flush;
  solved = true;
}
