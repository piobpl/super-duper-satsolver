/* Copyright 2016 Authors */

#include "../src/model.h"

const double Model::_agility_grow = 0.9999;

Literal operator+(const Variable &v) {
  return Literal(v._i);
}

Literal operator-(const Variable &v) {
  return Literal(-v._i);
}

std::ostream& operator<<(std::ostream &out, const Literal &x) {
  out << x._i;
  return out;
}

Clause make_clause(std::initializer_list<int> lits) {
  Clause c;
  c.reserve(lits.size());
  for (int x : lits)
    c.push_back(Literal(x));
  return c;
}

std::ostream& operator<<(std::ostream &out, const Clause &c) {
  bool st = true;
  for (Literal x : c) {
    if (!st) out << " v ";
    st = false;
    out << x;
  }
  return out;
}

std::ostream& operator<<(std::ostream &out, const Model &m) {
  bool st = true;
  for (unsigned i = 0; i < m._def.size(); ++i)
    if (m._def[i]) {
      if (!st) out << " ";
      st = false;
      out << (i+1) << "=" << (m._val[i]?1:0);
    }
  return out;
}
