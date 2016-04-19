/* Copyright 2016 Authors */

#ifndef SRC_OUTPUT_H_
#define SRC_OUTPUT_H_

#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>

#include "../src/model.h"

class Output {
 public:
  static void read(FILE* file, int n, Model* model) {
    *model = Model(n);
    for (int i = 1; i <= n; i++) {
      int var, val;
      assert(fscanf(file, "%d=%d ", &var, &val) == 2);
      if (val == 0) {
        model->set(Literal(-var));
      } else if (val == 1) {
        model->set(Literal(var));
      } else {
        std::cerr << "Problem is not binary!" << std::endl;
        assert(false);
      }
    }
  }
};

#endif  // SRC_OUTPUT_H_
