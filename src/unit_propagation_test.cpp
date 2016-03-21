/* Copyright 2016 Authors */

#include <gtest/gtest.h>
#include <vector>

#include "../src/model.h"
#include "../src/unit_propagation.h"

TEST(UnitPropagationTest, AddEmptyClause) {
  int max_var = 0;
  UnitPropagation unit_propagation(max_var);
  unit_propagation.add_clause(Clause(max_var));
  ASSERT_TRUE(unit_propagation.has_failed());
}

TEST(UnitPropagationTest, AddFailedClause) {
  int max_var = 1;
  UnitPropagation unit_propagation(max_var);

  Clause clause1(max_var);
  clause1.add(1);
  unit_propagation.add_clause(clause1);
  Clause clause2(max_var);
  clause2.add(-1);
  unit_propagation.add_clause(clause2);
  ASSERT_TRUE(unit_propagation.has_failed());
}

TEST(UnitPropagationTest, SuccessfulPropagation) {
  int max_var = 3;
  UnitPropagation unit_propagation(max_var);
  std::vector<Clause> clauses = {
    Clause(max_var, {1, 2, 3}),
    Clause(max_var, {-1}),
    Clause(max_var, {1, -3})
  };
  unit_propagation.add_clauses(clauses);
  unit_propagation.propagate();
  ASSERT_FALSE(unit_propagation.has_failed());
  Model model = unit_propagation.get_model();
  ASSERT_TRUE(model.isset(1));
  ASSERT_FALSE(model.value(1));
  ASSERT_TRUE(model.isset(2));
  ASSERT_TRUE(model.value(2));
  ASSERT_TRUE(model.isset(3));
  ASSERT_FALSE(model.value(3));
}

TEST(UnitPropagationTest, FailedPropagation) {
  int max_var = 2;
  UnitPropagation unit_propagation(max_var);
  std::vector<Clause> clauses = {
    Clause(max_var, {1, -2}),
    Clause(max_var, {2}),
    Clause(max_var, {-1, -2})
  };
  unit_propagation.add_clauses(clauses);
  unit_propagation.propagate();
  ASSERT_TRUE(unit_propagation.has_failed());
}
