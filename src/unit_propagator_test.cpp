/* Copyright 2016 Authors */

#include <gtest/gtest.h>
#include <vector>

#include "../src/model.h"
#include "../src/unit_propagator.h"

TEST(UnitPropagatorTest, AddEmptyClause) {
  int max_var = 0;
  UnitPropagator unit_propagator(max_var);
  unit_propagator.add_clause(Clause(max_var));
  ASSERT_TRUE(unit_propagator.has_failed());
}

TEST(UnitPropagatorTest, AddFailedClause) {
  int max_var = 1;
  UnitPropagator unit_propagator(max_var);

  Clause clause1(max_var);
  clause1.add(1);
  unit_propagator.add_clause(clause1);
  Clause clause2(max_var);
  clause2.add(-1);
  unit_propagator.add_clause(clause2);
  ASSERT_TRUE(unit_propagator.has_failed());
}

TEST(UnitPropagatorTest, SuccessfulPropagator) {
  int max_var = 3;
  UnitPropagator unit_propagator(max_var);
  std::vector<Clause> clauses = {
    Clause(max_var, {1, 2, 3}),
    Clause(max_var, {-1}),
    Clause(max_var, {1, -3})
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.propagate();
  ASSERT_FALSE(unit_propagator.has_failed());
  Model model = unit_propagator.get_model();
  ASSERT_TRUE(model.isset(1));
  ASSERT_FALSE(model.value(1));
  ASSERT_TRUE(model.isset(2));
  ASSERT_TRUE(model.value(2));
  ASSERT_TRUE(model.isset(3));
  ASSERT_FALSE(model.value(3));
}

TEST(UnitPropagatorTest, FailedPropagator) {
  int max_var = 2;
  UnitPropagator unit_propagator(max_var);
  std::vector<Clause> clauses = {
    Clause(max_var, {1, -2}),
    Clause(max_var, {2}),
    Clause(max_var, {-1, -2})
  };
  unit_propagator.add_clauses(clauses);
  unit_propagator.propagate();
  ASSERT_TRUE(unit_propagator.has_failed());
}
