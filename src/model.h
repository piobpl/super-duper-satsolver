/* Copyright 2016 Authors */

#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#include <vector>
#include <iostream>

class Model {
  int n;
  std::vector<bool> used, data;

 public:
  Model() : Model(0) {}

  explicit Model(int _n) : n(_n), used(n+1), data(n+1) {}

  int size() const {
    return n;
  }

  void resize(int _n) {
    n = _n;
    used.resize(n+1);
    data.resize(n+1);
  }

  bool isset(int x) const {
    return used[x];
  }

  void set(int x, bool v) {
    used[x] = 1;
    data[x] = v;
  }

  void unset(int x) {
    used[x] = 0;
  }

  void clear() {
    used.clear();
    used.resize(n+1);
  }

  bool satisfied(const Clause &c) const {
    for (int i = 1; i <= n; ++i)
      if (used[i])
        if (c.has(data[i] ? i : -i))
          return 1;
    return 0;
  }

  bool spoiled(const Clause &c) const {
    for (int i = 1; i <= n; ++i)
      if (used[i]) {
        if (c.has(data[i] ? i : -i))
          return 0;
      } else if (c.has(i) || c.has(-i)) {
        return 0;
      }
    return 1;
  }

  bool value(int x) const {
    return data[x];
  }
};

std::ostream& operator<<(std::ostream &out, const Model &m) {
  for (int i = 1; i < m.size(); ++i)
    out << m.value(i) << " ";
  out << m.value(m.size());
  return out;
}

#endif  // SRC_MODEL_H_
