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

  c.add(3);
  c.add(-7);
  c.add(13);

  int i = 0;
  for (int v : c) {
    if (i == 0) ASSERT_EQ(3, v);
    if (i == 1) ASSERT_EQ(13, v);
    if (i == 2) ASSERT_EQ(-7, v);
    ++i;
  }
  ASSERT_EQ(i, 3);

  c.remove(3);
  c.remove(13);
  i = 0;
  for (int v : c) {
    if (i == 0) ASSERT_EQ(-7, v);
    ++i;
  }
  ASSERT_EQ(i, 1);

  Clause d(9, {1, -5, 7});
  ASSERT_TRUE(d.has(-5));
  ASSERT_TRUE(d.has(7));
  ASSERT_TRUE(d.has(1));
  ASSERT_FALSE(d.has(2));
}
