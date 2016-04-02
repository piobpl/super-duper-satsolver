/* Copyright 2016 Authors */

#include "../src/model.h"

std::ostream& operator<<(std::ostream &out, const Model &m) {
  for (int i = 1; i < m.size(); ++i)
    if (m.is_set(i))
      out << i << "=" << m.value(i) << " ";
  if (m.is_set(m.size()))
    out << m.size() << "=" << m.value(m.size());
  return out;
}
