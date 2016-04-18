/* Copyright 2016 Authors */

#include <gtest/gtest.h>

#include <tuple>

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
*/

TEST(ModelTest, ModelManipulation) {
  Model m(100);

  for (int _i = 1; _i <= 100; ++_i) {
    Literal i(_i);

    SCOPED_TRACE(_i);

    ASSERT_FALSE(m.defined(i));

    m.set(i, false);
    ASSERT_TRUE(m.defined(i));
    ASSERT_TRUE(m.defined(~i));
    ASSERT_FALSE(m.value(i));

    m.set(~i, false);
    ASSERT_TRUE(m.defined(i));
    ASSERT_TRUE(m.defined(~i));
    ASSERT_TRUE(m.value(i));

    if (_i&1)
      m.unset(i);
    else
      m.unset(~i);
    ASSERT_FALSE(m.defined(i));

    if (_i % 3 == 0) m.set(Variable(_i), (_i % 5) < 2);
  }

  Clause c0{+Variable(3), -Variable(51), Literal(99)};
  Clause c1{Literal(3), Literal(-51), Literal(99), Literal(-33)};

  ASSERT_FALSE(m.satisfied(c0));
  ASSERT_TRUE(m.spoiled(c0));

  ASSERT_TRUE(m.satisfied(c1));
  ASSERT_FALSE(m.spoiled(c1));

  m.unset(Variable(51));
  ASSERT_FALSE(m.satisfied(c0));
  ASSERT_FALSE(m.spoiled(c0));

  m.set(Literal(51), true);
  ASSERT_FALSE(m.satisfied(c0));
  ASSERT_TRUE(m.spoiled(c0));

  Model mp(3);
  mp.set(Variable(1), true);
  mp.set(-Variable(2), true);
  ASSERT_FALSE(mp.all_assigned());

  Clause cp0{Literal(-1), Literal(2)};
  bool ambivalent;
  Literal witness(1);
  std::tie(ambivalent, witness) = mp.ambivalent(cp0);
  ASSERT_FALSE(ambivalent);
  Clause cp1{Literal(-1), Literal(2), Literal(3)};
  std::tie(ambivalent, witness) = mp.ambivalent(cp1);
  ASSERT_TRUE(ambivalent);
  ASSERT_EQ(witness, Literal(3));

  mp.set(Literal(-3), false);
  ASSERT_TRUE(mp.all_assigned());
  std::tie(ambivalent, witness) = mp.ambivalent(cp1);
  ASSERT_FALSE(ambivalent);

  std::stringstream s;
  s << mp;
  ASSERT_EQ(s.str(), "0=1 1=0 2=1");
}
