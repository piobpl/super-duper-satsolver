/* Copyright 2016 Authors */

#ifndef SRC_CLAUSE_H_
#define SRC_CLAUSE_H_

#include <algorithm>
#include <iostream>
#include <vector>

typedef uint64_t BLOCK;

int var(int v);

class Clause {
 public:
  int n, b;
  std::vector<BLOCK> data;

  Clause() {}

  explicit Clause(int _n) : n(_n), b((2*n + 63)/64), data(b) {}

  Clause(const Clause &o) {
    data = o.data;
  }

  bool has(int v) const {
    v = var(v);
    return data[v / 64] & (1ULL << (v % 64));
  }

  void add(int v) {
    v = var(v);
    data[v / 64] |= (1ULL << (v % 64));
  }

  void remove(int v) {
    v = var(v);
    data[v / 64] &= ~(1ULL << (v % 64));
  }

  bool empty() const {
    for (int i = 0; i < b; ++i)
      if (data[i])
        return 0;
    return 1;
  }

  bool trivial() const {
    for (int i = 0; i < b; ++i)
      if (data[i] & (data[i] >> 1))
        return 1;
    return 0;
  }
};

bool operator<(const Clause &a, const Clause &b);

bool operator==(const Clause &a, const Clause &b);

Clause operator|(const Clause &a, const Clause &b);

std::ostream& operator<<(std::ostream &out, const Clause &c);

#endif  // SRC_CLAUSE_H_
