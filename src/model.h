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
  bool operator!=(const Variable &b) const { return _i != b._i; }

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

  Variable variable() const { return Variable(std::abs(_i)); }

  bool pos() const { return _i > 0; }
  bool neg() const { return _i < 0; }
  int index() const {
    if (pos()) return 2*(_i-1);
    return 2*(-_i-1) + 1;
  }

  Literal operator-() const { return Literal(-_i); }

  bool operator==(const Literal &b) const { return _i == b._i; }
  bool operator!=(const Literal &b) const { return _i != b._i; }

  friend std::ostream& operator<<(std::ostream &out, const Literal &x);

 private:
  int _i;
};

typedef std::vector<Literal> Clause;

Clause make_clause(std::initializer_list<int> lits);

std::ostream& operator<<(std::ostream &out, const Clause &c);

class VariableSet {
 public:
  class Iterator {
   public:
    explicit Iterator(int i) : _i(i) {}

    bool operator==(const Iterator &other) const { return _i == other._i; }
    bool operator!=(const Iterator &other) const { return _i != other._i; }

    Variable operator*() { return Variable(_i); }

    Iterator& operator++() { ++_i; return *this; }
   private:
    int _i;
  };

  explicit VariableSet(int cnt) : _cnt(cnt) {}

  Iterator begin() { return Iterator(1); }

  Iterator end() { return Iterator(_cnt + 1); }

 private:
  int _cnt;
};

class Model {
 public:
  explicit Model(int var_count)
    : _var_count(var_count),
      _def(var_count),
      _val(var_count) {}

  int variable_count() const {
    return _var_count;
  }

  VariableSet variables() const {
    return VariableSet(_var_count);
  }

  bool defined(Variable v) const {
    return _def[v.index()];
  }

  bool defined(Literal v) const {
    return _def[v.variable().index()];
  }

  bool value(Variable x) const {
    assert(_def[x.index()]);
    return _val[x.index()];
  }

  bool value(Literal x) const {
    assert(_def[x.variable().index()]);
    if (x.pos())
      return _val[x.variable().index()];
    return !_val[x.variable().index()];
  }

  void set(Literal x) {
    _def[x.variable().index()] = true;
    _val[x.variable().index()] = x.pos();
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
  int _var_count;
  std::vector<bool> _def, _val;
};

#endif  // SRC_MODEL_H_
