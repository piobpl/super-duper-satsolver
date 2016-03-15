/* Copyright 2016 Authors */

#include <gtest/gtest.h>

#include "../src/clause.h"

TEST(ClauseTest, ClauseManipulation) {
  Clause c(100);
  for (int i = 1; i <= 100; ++i) {
    SCOPED_TRACE(i);

    ASSERT_TRUE(c.empty());

    ASSERT_FALSE(c.has(i));
    c.add(i);
    ASSERT_TRUE(c.has(i));
    c.remove(i);
    ASSERT_FALSE(c.has(i));

    ASSERT_FALSE(c.has(-i));
    c.add(-i);
    ASSERT_TRUE(c.has(-i));
    c.remove(-i);
    ASSERT_FALSE(c.has(-i));

    ASSERT_FALSE(c.trivial());
    c.add(i);
    c.add(-i);
    ASSERT_TRUE(c.trivial());
    c.remove(i);
    c.remove(-i);
    ASSERT_FALSE(c.trivial());

    ASSERT_TRUE(c.empty());
  }
}
