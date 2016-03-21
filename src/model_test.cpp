/* Copyright 2016 Authors */

#include <gtest/gtest.h>

#include "../src/clause.h"
#include "../src/model.h"

TEST(ModelTest, ModelManipulation) {
  Model m(100);
  Model::time_type start = m.time();

  ASSERT_EQ(m.size(), 100);

  for (int i = 1; i <= 100; ++i) {
    SCOPED_TRACE(i);

    ASSERT_FALSE(m.isset(i));

    m.set(i, 0);
    ASSERT_TRUE(m.isset(i));
    ASSERT_FALSE(m.value(i));

    m.set(i, 1);
    ASSERT_TRUE(m.isset(i));
    ASSERT_TRUE(m.value(i));

    m.unset(i);
    ASSERT_FALSE(m.isset(i));

    if (i % 3 == 0) m.set(i, (i % 5) < 2);
  }
  Model::time_type postinit = m.time();

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

  m.recall(postinit);
  for (int i = 1; i <= 100; ++i) {
    SCOPED_TRACE(i);

    if (i % 3 == 0) {
      ASSERT_TRUE(m.isset(i));
      ASSERT_EQ((i % 5) < 2, m.value(i));
    } else {
      ASSERT_FALSE(m.isset(i));
    }
  }

  m.recall(start);
  for (int i = 1; i <= 100; ++i) {
    SCOPED_TRACE(i);

    ASSERT_FALSE(m.isset(i));
  }
}
