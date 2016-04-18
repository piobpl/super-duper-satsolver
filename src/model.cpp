/* Copyright 2016 Authors */

#include "../src/model.h"

Literal operator+(const Variable &v) {
  return Literal(v._i);
}

Literal operator-(const Variable &v) {
  return Literal(-v._i);
}

std::ostream& operator<<(std::ostream &out, const Model &m) {
  bool st = true;
  for (unsigned i = 0; i < m._def.size(); ++i)
    if (m._def[i]) {
      if (!st) out << " ";
      st = false;
      out << i << "=" << (m._val[i]?1:0);
    }
  return out;
}
