/* Copyright 2016 Authors */

#include "../src/model.h"

std::ostream& operator<<(std::ostream &out, const Model &m) {
  for (int i = 1; i < m.size(); ++i)
    out << m.value(i) << " ";
  out << m.value(m.size());
  return out;
}
