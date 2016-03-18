/* Copyright 2016 Authors */

#include <algorithm>

#include "../src/clause.h"

int var(int v) {
  if (v > 0)
    return 2*v - 2;
  return -2*v - 1;
}

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
