/* Copyright 2016 Authors */

#include "../src/grasp_solver.h"

#include <vector>

bool GraspSolver::decide(int dl) {
  const Model& model = up.get_model();
  if (std::all_of(clauses.begin(), clauses.end(),
      [&model](Clause& c) {return model.satisfied(c);})) {
    return true;
  }
  for (Clause clause : clauses) {
    int witness;
    if (model.ambivalent(clause, &witness)) {
      if (clause.has(witness)) {
        up.notice(witness, true, dl);
      } else if (clause.has(-witness)) {
        up.notice(witness, false, dl);
      } else {
        std::cerr << "grasp_solver.h. ERROR.";
        exit(-1);
      }
      return false;
    }
  }
  // jesli jestesmy tutaj to znaczy, ze zdanie jest na pewno niespelnione przez
  // biezace wartosciowanie co nie powinno sie zdazyc (UP powinien wykryc)
  return false;
}

void GraspSolver::solve(std::vector<Clause> _clauses) {
  clauses = _clauses;
  up.add_clauses(clauses);
  const Model& model = up.get_model();

  if (!up.propagate()) {
      solved = false;
      return;
  }
  // decision level
  int dl = 0;
  /*due to Marques Silva - for future features 
   * check should be done in this way*/
  while (!model.all_assigned()) {
      decide(dl);
      dl++;
      if (up.propagate()) {
        int beta = up.diagnose();
        if ( beta < 0 ) {
          solved = false;
          return;
      } else {
        up.backtrack(beta);
        dl = beta;
      }
    }
  }
  solved = true;
}
