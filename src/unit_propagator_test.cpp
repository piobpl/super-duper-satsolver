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
  unit_propagator.propagate(-1);
  ASSERT_FALSE(unit_propagator.has_failed());
  Model model = unit_propagator.model();
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
  unit_propagator.propagate(-1);
  ASSERT_TRUE(unit_propagator.has_failed());
}

TEST(UnitPropagatorTest, MakingAssumptions) {
  int variables = 6;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    Clause(variables, {1, 2, 3, 5}),
    Clause(variables, {1, -2, 3, -4, 5}),
    Clause(variables, {-1, 5}),
    Clause(variables, {1, -3, 5}),
    Clause(variables, {6, -5}),
    Clause(variables, {-6, -5}),
  };
  unit_propagator.add_clauses(clauses);
  ASSERT_TRUE(unit_propagator.propagate(-1));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(5, 1, 0);
  ASSERT_FALSE(unit_propagator.propagate(0));
  ASSERT_TRUE(unit_propagator.has_failed());
  unit_propagator.backtrack(0);
  Model model = unit_propagator.model();
  ASSERT_FALSE(model.is_set(1));
  ASSERT_FALSE(model.is_set(2));
  ASSERT_FALSE(model.is_set(3));
  unit_propagator.assume(5, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  model = unit_propagator.model();
  ASSERT_TRUE(model.is_set(1));
  ASSERT_FALSE(model.value(1));
  ASSERT_TRUE(model.is_set(2));
  ASSERT_TRUE(model.value(2));
  ASSERT_TRUE(model.is_set(3));
  ASSERT_FALSE(model.value(3));
  ASSERT_TRUE(model.is_set(4));
  ASSERT_FALSE(model.value(4));
  ASSERT_TRUE(model.is_set(5));
  ASSERT_FALSE(model.value(5));
}

TEST(UnitPropagatorTest, FailedAssumptions) {
  int variables = 6;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    Clause(variables, {1, 2, 3, 4}),
  };
  unit_propagator.add_clauses(clauses);
  ASSERT_TRUE(unit_propagator.propagate(-1));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(2, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(3, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(1, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(4, 0, 0);
  ASSERT_FALSE(unit_propagator.propagate(0));
  ASSERT_TRUE(unit_propagator.has_failed());
  unit_propagator.backtrack(0);
  unit_propagator.assume(2, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(1, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(3, 0, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
  unit_propagator.assume(4, 1, 0);
  ASSERT_TRUE(unit_propagator.propagate(0));
  ASSERT_FALSE(unit_propagator.has_failed());
}

TEST(UnitPropagatorTest, DiagnoseCorectness) {
  UnitPropagator up(8);

  up.add_clauses(std::vector<Clause>{
    Clause(8, {1, 8, -2}),
    Clause(8, {1, -3}),
    Clause(8, {2, 3, 4}),
    Clause(8, {-4, -5}),
    Clause(8, {7, -4, -6}),
    Clause(8, {5, 6}),
  });

  up.assume(7, false, 0);
  up.assume(8, false, 0);
  up.assume(1, false, 0);

  up.propagate(0);
  ASSERT_EQ(up.diagnose(), 0);
}
