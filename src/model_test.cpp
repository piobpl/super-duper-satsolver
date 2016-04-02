/* Copyright 2016 Authors */

#include <gtest/gtest.h>

#include "../src/clause.h"
#include "../src/model.h"

TEST(ModelTest, ModelManipulation) {
  Model m(100);

  ASSERT_EQ(m.size(), 100);

  for (int i = 1; i <= 100; ++i) {
    SCOPED_TRACE(i);

    ASSERT_FALSE(m.is_set(i));

    m.set(i, 0);
    ASSERT_TRUE(m.is_set(i));
    ASSERT_FALSE(m.value(i));

    m.set(i, 1);
    ASSERT_TRUE(m.is_set(i));
    ASSERT_TRUE(m.value(i));

    m.unset(i);
    ASSERT_FALSE(m.is_set(i));

    if (i % 3 == 0) m.set(i, (i % 5) < 2);
  }

  Clause c(100);
  c.add(3);
  c.add(-51);
  c.add(99);

  ASSERT_FALSE(m.satisfied(c));
  ASSERT_TRUE(m.spoiled(c));

  c.add(-33);
  ASSERT_TRUE(m.satisfied(c));
  ASSERT_FALSE(m.spoiled(c));

  c.remove(-33);
  m.unset(51);
  ASSERT_FALSE(m.satisfied(c));
  ASSERT_FALSE(m.spoiled(c));

  m.set(51, 1);
  ASSERT_FALSE(m.satisfied(c));
  ASSERT_TRUE(m.spoiled(c));
}
