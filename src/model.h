/* Copyright 2016 Authors */

#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

class Literal;

class Variable {
 public:
  // i in {1, 2, ...}
  Variable() {}
  explicit Variable(int i) : _i(i) {}

  int index() const { return _i-1; }

  bool operator==(const Variable &b) const { return _i == b._i; }
  bool operator!=(const Variable &b) const { return _i == b._i; }

  friend Literal operator+(const Variable &v);
  friend Literal operator-(const Variable &v);
 private:
  int _i;
};

class Literal {
 public:
  // i in {..., -2, -1} u {1, 2, ...}
  Literal() {}
  explicit Literal(int i) : _i(i) {}
  explicit Literal(Variable v) : _i(v.index()+1) {}

  Variable variable() const { return Variable(std::abs(_i)); }

  bool pos() const { return _i > 0; }
  bool neg() const { return _i < 0; }

  Literal operator~() const { return Literal(-_i); }

  bool operator==(const Literal &b) const { return _i == b._i; }
  bool operator!=(const Literal &b) const { return _i == b._i; }
 private:
  int _i;
};

Literal operator+(const Variable &v) {
  return Literal(v._i);
}

Literal operator-(const Variable &v) {
  return Literal(-v._i);
}

typedef std::vector<Literal> Clause;

class Model {
 public:
  explicit Model(int variables) : _def(variables), _val(variables) {}

  bool defined(Variable v) const {
    return _def[v.index()];
  }

  bool defined(Literal v) const {
    return _def[v.variable().index()];
  }

  bool value(Variable x) const {
    assert(_def[x.index()]);
    _val[x.index()];
  }

  bool value(Literal x) const {
    assert(_def[x.variable().index()]);
    if (x.pos())
      return _val[x.variable().index()];
    return !_val[x.variable().index()];
  }

  void set(Variable x, bool v) {
    _def[x.index()] = true;
    _val[x.index()] = v;
  }

  void set(Literal x, bool v) {
    _def[x.variable().index()] = true;
    if (x.pos())
      _val[x.variable().index()] = v;
    else
      _val[x.variable().index()] = !v;
  }

  void unset(Variable x) {
    _def[x.index()] = false;
  }

  void unset(Literal x) {
    _def[x.variable().index()] = false;
  }

  bool satisfied(const Clause &c) const {
    for (Literal x : c)
      if (defined(x) && value(x))
        return 1;
    return 0;
  }

  bool spoiled(const Clause &c) const {
    for (Literal x : c)
      if (!defined(x) || value(x))
        return 0;
    return 1;
  }

  std::pair<bool, Literal> ambivalent(const Clause& c) const {
    for (Literal x : c)
      if (!defined(x))
        return std::pair<bool, Literal>(true, x);
    return std::pair<bool, Literal>(false, Literal(1));
  }

  bool all_assigned() const {
    for (unsigned i = 0; i < _def.size(); ++i)
      if (!_def[i]) return 0;
    return 1;
  }

  friend std::ostream& operator<<(std::ostream &out, const Model &m);

 private:
  std::vector<bool> _def, _val;
};

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

#endif  // SRC_MODEL_H_
