/* Copyright 2016 Authors */

#include <algorithm>

#include "../src/clause.h"

bool operator<(const Clause &a, const Clause &b) {
  if (a.pos == b.pos) return a.neg < b.neg;
  return a.pos < b.pos;
}

bool operator==(const Clause &a, const Clause &b) {
  return a.pos == b.pos && a.neg == b.neg;
}

Clause operator|(const Clause &a, const Clause &b) {
  Clause c;
  c.pos = a.pos | b.pos;
  c.neg = a.neg | b.neg;
  return c;
}

std::ostream& operator<<(std::ostream &out, const Clause &c) {
  bool st = 1;
  out << "(";
  for (int v : c) {
    if (!st) out << " v ";
    st = 0;
    out << v;
  }
  out << ")";
  return out;
}
