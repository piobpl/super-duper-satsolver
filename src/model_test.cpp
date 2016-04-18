/* Copyright 2016 Authors */

#include <gtest/gtest.h>

#include "../src/clause.h"
#include "../src/model.h"

/*
TEST(ClauseTest, ClauseManipulation) {
  Clause c(100);
  ASSERT_EQ(100, c.maximal_variable());

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

  c.remove(-7);
  c.add(-1);
  c.add(3);
  c.add(-2);
  Clause::Iterator it = c.begin();
  ASSERT_FALSE(it.end());
  ASSERT_EQ(3, *it);
  ++it;
  ASSERT_FALSE(it.end());
  ASSERT_EQ(-1, *it);
  ++it;
  ASSERT_FALSE(it.end());
  ASSERT_EQ(-2, *it);
  ++it;
  ASSERT_TRUE(it.end());

  Clause d(9, {1, -5, 7});
  ASSERT_TRUE(d.has(-5));
  ASSERT_TRUE(d.has(7));
  ASSERT_TRUE(d.has(1));
  ASSERT_FALSE(d.has(2));
}

TEST(ModelTest, ModelManipulation) {
  Model m(100);

  ASSERT_EQ(m.size(), 100);

  for (int i = 1; i <= 100; ++i) {
    SCOPED_TRACE(i);

    ASSERT_FALSE(m.is_set(i));

    m.set(i, 0);
    ASSERT_TRUE(m.is_set(i));
    ASSERT_TRUE(m.is_set(-i));
    ASSERT_FALSE(m.value(i));

    m.set(-i, 0);
    ASSERT_TRUE(m.is_set(i));
    ASSERT_TRUE(m.is_set(-i));
    ASSERT_TRUE(m.value(i));

    if (i&1)
      m.unset(i);
    else
      m.unset(-i);
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

  Model mp(3);
  mp.set(1, 1);
  mp.set(-2, 1);
  ASSERT_FALSE(mp.all_assigned());

  Clause cp(3);
  cp.add(-1);
  cp.add(2);
  int i;
  ASSERT_FALSE(mp.ambivalent(cp, &i));
  cp.add(3);
  ASSERT_TRUE(mp.ambivalent(cp, &i));
  ASSERT_EQ(i, 3);

  mp.set(-3, 0);
  ASSERT_TRUE(mp.all_assigned());
  ASSERT_FALSE(mp.ambivalent(cp, &i));
}
*/
