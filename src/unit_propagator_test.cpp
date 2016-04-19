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

TEST(UnitPropagatorTest, MakingAssumptions) {
  int variables = 6;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    make_clause({1, 2, 3, 5}),
    make_clause({1, -2, 3, -4, 5}),
    make_clause({-1, 5}),
    make_clause({1, -3, 5}),
    make_clause({6, -5}),
    make_clause({-6, -5}),
  };
  unit_propagator.add_clauses(clauses);
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(5));
  ASSERT_TRUE(unit_propagator.failed());
  unit_propagator.revert(1);
  Model model = unit_propagator.model();
  ASSERT_FALSE(model.defined(Variable(1)));
  ASSERT_FALSE(model.defined(Variable(2)));
  ASSERT_FALSE(model.defined(Variable(3)));
  unit_propagator.assume(Literal(-5));
  ASSERT_FALSE(unit_propagator.failed());
  model = unit_propagator.model();
  ASSERT_TRUE(model.defined(Variable(1)));
  ASSERT_FALSE(model.value(Variable(1)));
  ASSERT_TRUE(model.defined(Variable(2)));
  ASSERT_TRUE(model.value(Variable(2)));
  ASSERT_TRUE(model.defined(Variable(3)));
  ASSERT_FALSE(model.value(Variable(3)));
  ASSERT_TRUE(model.defined(Variable(4)));
  ASSERT_FALSE(model.value(Variable(4)));
  ASSERT_TRUE(model.defined(Variable(5)));
  ASSERT_FALSE(model.value(Variable(5)));
  ASSERT_FALSE(model.defined(Variable(6)));
}

TEST(UnitPropagatorTest, FailedAssumptions) {
  int variables = 6;
  UnitPropagator unit_propagator(variables);
  std::vector<Clause> clauses = {
    make_clause({3, 1, 4, 2}),
  };
  unit_propagator.add_clauses(clauses);
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(-2));
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(-3));
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(-1));
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(-4));
  ASSERT_TRUE(unit_propagator.failed());
  unit_propagator.revert(0);
  unit_propagator.assume(Literal(-2));
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(-1));
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(-3));
  ASSERT_FALSE(unit_propagator.failed());
  unit_propagator.assume(Literal(4));
  ASSERT_FALSE(unit_propagator.failed());
}

TEST(UnitPropagatorTest, DiagnoseCorectness) {
  UnitPropagator up(8);

  up.add_clauses(std::vector<Clause>{
    make_clause({1, 8, -2}),
    make_clause({1, -3}),
    make_clause({2, 3, 4}),
    make_clause({-4, -5}),
    make_clause({7, -4, -6}),
    make_clause({5, 6}),
  });

  up.assume(Literal(-7));
  up.assume(Literal(-8));
  up.assume(Literal(-1));

  ASSERT_TRUE(up.failed());
  ASSERT_EQ(3, up.diagnose());
}
