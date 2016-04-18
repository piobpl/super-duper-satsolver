/* Copyright 2016 Authors */

#include <gtest/gtest.h>
#include <vector>

#include "../src/model.h"
#include "../src/unit_propagator.h"

TEST(UnitPropagatorTest, AddEmptyClause) {
  UnitPropagator unit_propagator(0);

  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.add_clause(Clause{});
  ASSERT_TRUE(unit_propagator.failed());
}

TEST(UnitPropagatorTest, AddFailedClause) {
  UnitPropagator unit_propagator(1);

  unit_propagator.add_clause(make_clause({1}));
  unit_propagator.add_clause(make_clause({-1}));
  ASSERT_TRUE(unit_propagator.failed());
}

TEST(UnitPropagatorTest, SuccessfulPropagator) {
  UnitPropagator unit_propagator(4);
  std::vector<Clause> clauses = {
    make_clause({1, 2, 3}),
    make_clause({1, -2, 3, -4}),
    make_clause({1, -3})
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.assume(Literal(-1));
  ASSERT_FALSE(unit_propagator.failed());
  Model model = unit_propagator.model();
  ASSERT_TRUE(model.defined(Variable(1)));
  ASSERT_FALSE(model.value(Variable(1)));
  ASSERT_TRUE(model.defined(Variable(2)));
  ASSERT_TRUE(model.value(Variable(2)));
  ASSERT_TRUE(model.defined(Variable(3)));
  ASSERT_FALSE(model.value(Variable(3)));
  ASSERT_TRUE(model.defined(Variable(4)));
  ASSERT_FALSE(model.value(Variable(4)));
}

TEST(UnitPropagatorTest, FailedPropagator) {
  UnitPropagator unit_propagator(2);
  std::vector<Clause> clauses = {
    make_clause({1, -2}),
    make_clause({-1, -2})
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.assume(Literal(2));
  ASSERT_TRUE(unit_propagator.failed());
}

/*
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
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(5, 1, 0);
  unit_propagator.propagate();
  ASSERT_TRUE(unit_propagator.failed());
  unit_propagator.backtrack(0);
  Model model = unit_propagator.model();
  ASSERT_FALSE(model.defined(1));
  ASSERT_FALSE(model.defined(2));
  ASSERT_FALSE(model.defined(3));
  unit_propagator.assume(5, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  model = unit_propagator.model();
  ASSERT_TRUE(model.defined(1));
  ASSERT_FALSE(model.value(1));
  ASSERT_TRUE(model.defined(2));
  ASSERT_TRUE(model.value(2));
  ASSERT_TRUE(model.defined(3));
  ASSERT_FALSE(model.value(3));
  ASSERT_TRUE(model.defined(4));
  ASSERT_FALSE(model.value(4));
  ASSERT_TRUE(model.defined(5));
  ASSERT_FALSE(model.value(5));
}

TEST(UnitPropagatorTest, FailedAssumptions) {
  int variables = 6;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    Clause(variables, {1, 2, 3, 4}),
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(2, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(3, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(1, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(4, 0, 0);
  unit_propagator.propagate();
  ASSERT_TRUE(unit_propagator.failed());
  unit_propagator.backtrack(0);
  unit_propagator.assume(2, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(1, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(3, 0, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(4, 1, 0);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.failed());
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

  up.propagate();
  ASSERT_EQ(up.diagnose(), 0);
}
*/
