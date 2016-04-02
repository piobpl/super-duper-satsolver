/* Copyright 2016 Authors */

#include <gtest/gtest.h>
#include <vector>

#include "../src/model.h"
#include "../src/unit_propagator.h"

TEST(UnitPropagatorTest, AddEmptyClause) {
  int variables = 0;
  UnitPropagator unit_propagator(variables);
  unit_propagator.add_clause(Clause(variables));
  ASSERT_TRUE(unit_propagator.has_failed());
}

TEST(UnitPropagatorTest, AddFailedClause) {
  int variables = 1;
  UnitPropagator unit_propagator(variables);

  Clause clause1(variables);
  clause1.add(1);
  unit_propagator.add_clause(clause1);
  Clause clause2(variables);
  clause2.add(-1);
  unit_propagator.add_clause(clause2);
  ASSERT_TRUE(unit_propagator.has_failed());
}

TEST(UnitPropagatorTest, SuccessfulPropagator) {
  int variables = 4;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    Clause(variables, {1, 2, 3}),
    Clause(variables, {1, -2, 3, -4}),
    Clause(variables, {-1}),
    Clause(variables, {1, -3})
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.has_failed());
  Model model = unit_propagator.get_model();
  ASSERT_TRUE(model.is_set(1));
  ASSERT_FALSE(model.value(1));
  ASSERT_TRUE(model.is_set(2));
  ASSERT_TRUE(model.value(2));
  ASSERT_TRUE(model.is_set(3));
  ASSERT_FALSE(model.value(3));
  ASSERT_TRUE(model.is_set(4));
  ASSERT_FALSE(model.value(4));
}

TEST(UnitPropagatorTest, FailedPropagator) {
  int variables = 2;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    Clause(variables, {1, -2}),
    Clause(variables, {2}),
    Clause(variables, {-1, -2})
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.propagate();
  ASSERT_TRUE(unit_propagator.has_failed());
}
