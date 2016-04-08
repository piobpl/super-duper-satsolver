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
  int vars = 8;
  UnitPropagator up(vars);

  Clause clause1(vars);
  clause1.add(1); clause1.add(8); clause1.add(-2);
  Clause clause2(vars);
  clause2.add(1); clause2.add(-3);
  Clause clause3(vars);
  clause3.add(2); clause3.add(3); clause3.add(4);
  Clause clause4(vars);
  clause4.add(-4); clause4.add(-5);
  Clause clause5(vars);
  clause5.add(7); clause5.add(-4); clause5.add(-6);
  Clause clause6(vars);
  clause6.add(5); clause6.add(6);
  std::vector<Clause> clauses;
  clauses.push_back(clause1); clauses.push_back(clause2);
  clauses.push_back(clause3); clauses.push_back(clause4);
  clauses.push_back(clause5); clauses.push_back(clause6);

  up.add_clauses(clauses);
  up.assume(7, false, 0);
  up.assume(8, false, 0);
  up.assume(1, false, 0);

  up.propagate(0);

  ASSERT_EQ(up.diagnose(), 0);
}



