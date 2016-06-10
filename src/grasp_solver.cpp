/* Copyright 2016 Authors */

#include "../src/grasp_solver.h"

#include <tuple>
#include <vector>

#include "../src/restarts.h"

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

  Variable x = up.active_variable();
  if (up.model().last_value(x))
    up.assume(-x);
  else
    up.assume(+x);
  return;
}

void GraspSolver::pre_decide() {
  double expected = -1;
  Literal x;

  for (Variable v : up.model().variables()) {
    if (up.model().defined(v)) continue;
    Literal y = +v;
    for (int r = 0; r < 2; ++r) {
      double current = 0;
      for (const Clause &c : up.clauses()) {
        double p = 0;
        for (const Literal z : c) {
          if (z != -y) {
            if (z == y) {
              p = 1;
            } else if (!up.model().defined(z)) {
              p = 0.5 + 0.5 * p;
            } else if (up.model().value(z)) {
              p = 1;
            }
          }
        }
        current += p;
      }
      if (current > expected) {
        expected = current;
        x = y;
      }
      y = -y;
    }
  }
  up.assume(x);
  return;
}

void GraspSolver::solve(std::vector<Clause> _clauses) {
  GeometricRestarts rs(32, 1.1);

  clauses = _clauses;
  up.add_clauses(clauses);
  const Model& model = up.model();

  while (!model.all_assigned()) {
    up.garbage_clauses_minisat();
    assert(!up.failed());
    decide();
    while (up.failed()) {
      int beta = up.diagnose();
      if (beta == 0) {
        solved = false;
        return;
      } else {
        up.revert(beta);
      }
    }
    assert(!up.failed());
    bool block = (up.model().agility() > 0.20);
    if (rs.restart(block)) {
      up.revert(1);
    }
  }
  solved = true;
}
