/* Copyright 2016 Authors */

#ifndef SRC_INPUT_H_
#define SRC_INPUT_H_

#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>

#include "../src/model.h"

class Input {
 public:
  static void read(FILE* file, int *n, std::vector<Clause> *clauses) {
    int m, t;
    char buff[4096];
    do {
      t = fscanf(file, "%[^\n] ", buff);
      assert(t == 1);
    }while (buff[0] == 'c');
    t = sscanf(buff, "p cnf %d %d ", n, &m);
    assert(t == 2);

    for (int i = 0; i < m; ++i) {
      Clause clause;
      for (;;) {
        int v;
        t = fscanf(file, "%d ", &v);
        assert(t == 1);
        if (v == 0) break;
        clause.push_back(Literal(v));
      }
      clauses->push_back(clause);
    }
    m = static_cast<int>(clauses->size());
  }
};

#endif  // SRC_INPUT_H_
