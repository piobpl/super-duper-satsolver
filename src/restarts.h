/* Copyright 2016 Authors */

#ifndef SRC_RESTARTS_H_
#define SRC_RESTARTS_H_

class GeometricRestarts {
 public:
  GeometricRestarts(int n, double a) : _n(n), _i(0), _a(a) {}

  bool restart(bool blocked) {
    ++_i;
    if (!blocked && _i >= _n) {
      _i = 0;
      _n = static_cast<int>(_n*_a);
      return true;
    }
    return false;
  }
 private:
  int _n, _i;
  double _a;
};

class LubyRestarts {
 public:
  LubyRestarts() : i(0), k(0), a{32} {}

  bool restart(bool blocked) {
    ++k;
    if(!blocked && k >= a[i]){
      k = 0;
      ++i;
      if (i == static_cast<int>(a.size())){
        a.insert(a.end(), a.begin(), a.end());
        a.push_back(a.back()*2);
      }
      return 1;
    }
    return 0;
  }
 private:
  int i, k;
  std::vector<int> a;
};

#endif  // SRC_RESTARTS_H_
