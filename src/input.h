/* Copyright 2016 Authors */

#ifndef SRC_INPUT_H_
#define SRC_INPUT_H_

#include <cassert>
#include <iostream>
#include <vector>

#include "../src/clause.h"

class Input {
 public:
  static void read(int *n, std::vector<Clause> *clauses) {
    int m;
    char buff[4096];
    do {
      assert(scanf("%[^\n] ", buff) == 1);
    }while (buff[0] == 'c');
    assert(sscanf(buff, "p cnf %d %d", n, &m) == 2);

    for (int i = 0; i < m; ++i) {
      Clause clause(*n);
      for (;;) {
        int v;
        assert(scanf("%d", &v) == 1);
        if (v == 0) break;
        clause.add(v);
      }
      if (!clause.trivial())
        clauses->push_back(clause);
    }
    m = static_cast<int>(clauses->size());
    std::cerr << "Got " << *n << " variables and "
      << m << " non-trivial clauses\n";
  }
};

#endif  // SRC_INPUT_H_
