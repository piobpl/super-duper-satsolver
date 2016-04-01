/* Copyright 2016 Authors */

#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#include <vector>
#include <iostream>

#include "../src/clause.h"

#include "boost/container/vector.hpp"

class Model {
  struct Recall {
    bool set, val;
    int var;
  };

  int n;
  boost::container::vector<bool> used, data;
  boost::container::vector<Recall> history;

 public:
  typedef boost::dynamic_bitset<>::size_type time_type;

  explicit Model(int _n) : n(_n), used(n+1), data(n+1) {}

  int size() const {
    return n;
  }

  bool isset(int x) const {
    if (x < 0) x = -x;
    return used[x];
  }

  void set(int x, bool v) {
    history.push_back(Recall{isset(x), value(x), x});
    if (x < 0) {
      x = -x;
      v = !v;
    }
    used[x] = 1;
    data[x] = v;
  }

  void unset(int x) {
    history.push_back(Recall{isset(x), value(x), x});
    used[x] = 0;
  }

  time_type time() {
    return history.size();
  }

  void recall(time_type past) {
    while (history.size() > past) {
      Recall r = history.back();
      history.pop_back();

      used[r.var] = r.set;
      data[r.var] = r.val;
    }
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
  
  bool ambivalent(const Clause& c, int* vitness){
    if(satisfied(c)){
      return false;
    }
    else{
      (*vitness)=-1;
      for(int i=1; i<=n; ++i){
        if(!used[i]){
          (*vitness) = i;
          break;
        }
      }
      if((*vitness)==-1){
        return false;
      }
	  return true;
    }
  }
  
  bool all_assigned(){
    for(int i=1; i<=n; ++i){
      if(!used[i]){
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
