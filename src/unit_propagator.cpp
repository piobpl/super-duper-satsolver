/* Copyright 2016 Authors */

#include "../src/unit_propagator.h"

void UnitPropagator::add_clause(const Clause &clause) {
  int c = static_cast<int>(clauses.size());
  clauses.push_back(clause);

  Clause::Iterator st = clauses[c].begin();
  while (!st.end()) {
    if (!model.isset(*st) || model.value(*st)) break;
    ++st;
  }
  if (st.end()) {
    failed = true;
    clauses.pop_back();
    return;
  }

  Clause::Iterator nd = st;
  ++nd;
  while (!nd.end()) {
    if (!model.isset(*nd) || model.value(*nd)) break;
    ++nd;
  }
  if (nd.end()) {
    if (!model.isset(*st)) {
      model.set(*st, true);
      propagation_queue.push(*st);
    }
    clauses.pop_back();
    return;
  }

  obs.push_back(Observators{st, nd});
  observed(*st).push_back(c);
  observed(*nd).push_back(c);
}

bool UnitPropagator::propagate() {
  if (failed) return false;;
  while (!propagation_queue.empty()) {
    int L = propagation_queue.front();
    propagation_queue.pop();
    for (int c : observed(-L)) {
      if (*obs[c][0] == -L) {
        obs[c][0] = obs[c][1];
      }
      ++obs[c][1];
      while (!obs[c][1].end()) {
        if (!model.isset(*obs[c][1]) || model.value(*obs[c][1])) break;
        ++obs[c][1];
      }
      int NL = *obs[c][0];
      if (!obs[c][1].end()) {
        observed(*obs[c][1]).push_back(c);
      } else if (model.isset(NL) && !model.value(NL)) {
        failed = true;
        return false;
      } else if (!model.isset(NL)) {
        model.set(NL, true);
        propagation_queue.push(NL);
      }
    }
  }
  return true;
}
