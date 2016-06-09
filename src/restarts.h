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

#endif  // SRC_RESTARTS_H_
