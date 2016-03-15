/* Copyright 2016 Authors */

#ifndef SRC_CLAUSE_H_
#define SRC_CLAUSE_H_

#include <algorithm>
#include <iostream>
#include <vector>

typedef uint64_t BLOCK;

int var(int v) {
  if (v > 0)
    return 2*v - 2;
  return -2*v - 1;
}

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

bool operator<(const Clause &a, const Clause &b) {
  if (a.n != b.n) return a.n < b.n;
  for (int i = 0; i < a.b; ++i)
    if (a.data[i] != b.data[i])
      return a.data[i] < b.data[i];
  return 0;
}

bool operator==(const Clause &a, const Clause &b) {
  if (a.n != b.n) return 0;
  for (int i = 0; i < a.n; ++i)
    if (a.data[i] != b.data[i])
      return 0;
  return 1;
}

Clause operator|(const Clause &a, const Clause &b) {
  Clause c(std::max(a.n, b.n));
  for (int i = 0; i < c.b; ++i)
    c.data[i] = (a.data[i] | b.data[i]);
  return c;
}

std::ostream& operator<<(std::ostream &out, const Clause &c) {
  bool st = 1;
  out << "(";
  for (int i = 1; i <= c.n; ++i) {
    if (c.has(i)) {
      if (!st) out << " v ";
      st = 0;
      out << i;
    }
    if (c.has(-i)) {
      if (!st) out << " v ";
      st = 0;
      out << "~" << i;
    }
  }
  out << ")";
  return out;
}

#endif  // SRC_CLAUSE_H_
