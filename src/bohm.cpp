/* Copyright 2016 Authors */

#include "../src/bohm.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <utility>

Variable BOHM::pick(const Model &model, const std::vector<Clause> &clauses) {
  std::vector<std::vector<int>> h;
  int n = model.variable_count();
  for (const Clause &c : clauses) {
    bool unresolved = !model.satisfied(c);
    if (unresolved) {
      auto is_defined =
        static_cast<bool(Model::*)(Literal) const> (&Model::defined);

      std::size_t i = std::count_if(c.begin(), c.end(),
        std::bind(is_defined, model, std::placeholders::_1));

      if (h.size() < i + 1) {
        h.resize(i+1);
      }

      if (h[i].size() == 0) {
        h[i].resize(2 * n);
      }

      for (Literal l : c) {
        if (!model.defined(l)) {
          h[i][l.index()]++;
        }
      }
    }
  }

  auto variables = model.variables();

  std::vector<std::pair<Variable, double>> candidates(n);
  std::transform(variables.begin(), variables.end(), candidates.begin(),
    [](Variable x){
      return std::make_pair(x, 0.);
    });

  for (const auto &hi : h) if (!hi.empty()) {
    for (auto &xHi : candidates) {
        Variable x = xHi.first;
        double mnhil, mxhil;
        std::tie(mnhil, mxhil) =
          std::minmax(hi[(+x).index()], hi[(-x).index()]);
        xHi.second = _alpha * mxhil + _beta * mnhil;
    }

    double maxHi = std::max_element(candidates.begin(), candidates.end(),
      [](const std::pair<Variable, double> &xHi1,
         const std::pair<Variable, double> &xHi2){
        return xHi1.second < xHi2.second;
      })->second;

    auto sepIt = std::partition(candidates.begin(), candidates.end(),
      [maxHi, this](const std::pair<Variable, double> &xHi){
        return xHi.second >= maxHi - _eps;
      });

    candidates.resize(sepIt - candidates.begin());
  }

  return candidates[0].first;
}
