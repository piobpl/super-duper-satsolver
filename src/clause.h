/* Copyright 2016 Authors */

#ifndef SRC_CLAUSE_H_
#define SRC_CLAUSE_H_

#include <algorithm>
#include <iostream>

#include "boost/dynamic_bitset.hpp"

#define VAR(v) (v > 0 ? 2*v - 2 : -2*v - 1)

class Clause {
  typedef boost::dynamic_bitset<>::size_type size_type;

  static const size_type npos = boost::dynamic_bitset<>::npos;

  boost::dynamic_bitset<> pos, neg;

 public:
  class Iterator {
   public:
    const Clause *c;
    bool pos;
    size_type i;

    Iterator(const Clause *_c, bool _pos, size_type _i)
       : c(_c), pos(_pos), i(_i) {}

    bool operator==(const Iterator& other) const {
      return pos == other.pos && i == other.i;
    }

    bool operator!=(const Iterator& other) const {
      return pos != other.pos || i != other.i;
    }

    int operator*() const {
      return pos ? static_cast<int>(i) + 1 : -static_cast<int>(i) - 1;
    }

    bool end() {
      return *this == c->end();
    }

    const Iterator& operator++() {
      if (pos) {
        i = c->pos.find_next(i);
        if (i == npos) {
          pos = 0;
          i = c->neg.find_first();
        }
      } else {
        i = c->neg.find_next(i);
      }
      return *this;
    }
  };

  explicit Clause(size_type maxvar) : pos(maxvar), neg(maxvar) {}

  explicit Clause(size_type maxvar, std::initializer_list<int> vars)
    : Clause(maxvar) {
    for (int v : vars) add(v);
  }

  bool has(int v) const {
    if (v > 0) return pos.test(v-1);
    return neg.test(-v-1);
  }

  bool is_pos(int v) {
    return pos.test(v-1);
  }

  bool is_neg(int v) {
    return neg.test(-v-1);
  }

  void add(int v) {
    if (v > 0)
      pos.set(v-1);
    else
      neg.set(-v-1);
  }

  void remove(int v) {
    if (v > 0)
      pos.reset(v-1);
    else
      neg.reset(-v-1);
  }

  size_type count() const {
    return pos.count() + neg.count();
  }

  size_type maximal_variable() const {
    return pos.size();
  }

  Iterator begin() const {
    size_type i = pos.find_first();
    if (i == npos) return Iterator(this, 0, neg.find_first());
    return Iterator(this, 1, i);
  }

  Iterator end() const {
    return Iterator(this, 0, npos);
  }

  bool empty() const {
    return pos.none() && neg.none();
  }

  bool trivial() const {
    return (pos & neg).any();
  }

  friend bool operator<(const Clause &a, const Clause &b);

  friend bool operator==(const Clause &a, const Clause &b);

  friend Clause operator|(const Clause &a, const Clause &b);

  friend std::ostream& operator<<(std::ostream &out, const Clause &c);
};

#endif  // SRC_CLAUSE_H_
