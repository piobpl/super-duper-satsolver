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
       int v;
       assert(fscanf(file, "%d ", &v) == 1);
       model->set(i, v);
    }
    std::cerr << "Got " << n << " values\n";
  }
};

#endif  // SRC_OUTPUT_H_
