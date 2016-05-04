/* Copyright 2016 Authors */

#ifndef SRC_BOHM_H_
#define SRC_BOHM_H_

#include <vector>

#include "../src/model.h"

class BOHM {
 public:
  BOHM(double alpha, double beta, double eps):
    _alpha(alpha), _beta(beta), _eps(eps) {}
  Variable pick(const Model &Model, const std::vector<Clause> &clauses);
 private:
  double _alpha;
  double _beta;
  double _eps;
};

#endif  // SRC_BOHM_H_
