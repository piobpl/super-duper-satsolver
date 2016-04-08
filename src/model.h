/* Copyright 2016 Authors */

#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#include <vector>
#include <iostream>

#include "../src/clause.h"

#include "boost/container/vector.hpp"

class Model {
  int variables;
  boost::container::vector<bool> used, data;

 public:
  typedef boost::dynamic_bitset<>::size_type time_type;

  explicit Model(int _variables)
    : variables(_variables),
      used(variables+1),
      data(variables+1) {}

  int size() const {
    return variables;
  }

  bool is_set(int x) const {
    if (x < 0) x = -x;
    return used[x];
  }

  void set(int x, bool v) {
    if (x < 0) {
      x = -x;
      v = !v;
    }
    used[x] = 1;
    data[x] = v;
  }

  void unset(int x) {
    used[x] = 0;
  }

  bool satisfied(const Clause &c) const {
    for (int i = 1; i <= variables; ++i)
      if (used[i])
        if (c.has(data[i] ? i : -i))
          return 1;
    return 0;
  }

  bool spoiled(const Clause &c) const {
    for (int i = 1; i <= variables; ++i)
      if (used[i]) {
        if (c.has(data[i] ? i : -i))
          return 0;
      } else if (c.has(i) || c.has(-i)) {
        return 0;
      }
    return 1;
  }

  bool ambivalent(const Clause& c, int* witness) const {
    if (satisfied(c)) {
      return false;
    } else {
      for (int i = 1; i <= variables; ++i) {
        if (!used[i] && (c.has(i) || c.has(-i))) {
          (*witness) = i;
          return true;
        }
      }
      return false;
    }
  }

  bool all_assigned() const {
    for (int i = 1; i <= variables; ++i) {
      if (!used[i]) {
        return false;
      }
    }
    return true;
  }

  bool value(int x) const {
    return data[x];
  }
};

std::ostream& operator<<(std::ostream &out, const Model &m);

#endif  // SRC_MODEL_H_
