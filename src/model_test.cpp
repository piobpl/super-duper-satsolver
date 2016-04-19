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

    ASSERT_TRUE(c.emty());

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

    ASSERT_TRUE(c.emty());
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

    m.set(-i);
    ASSERT_TRUE(m.defined(i));
    ASSERT_TRUE(m.defined(-i));
    ASSERT_FALSE(m.value(i));

    m.set(i);
    ASSERT_TRUE(m.defined(i));
    ASSERT_TRUE(m.defined(-i));
    ASSERT_TRUE(m.value(i));

    if (_i&1)
      m.unset(i);
    else
      m.unset(-i);
    ASSERT_FALSE(m.defined(i));

    if (_i % 3 == 0) {
      if (_i % 5 < 2)
        m.set(+Variable(_i));
      else
        m.set(-Variable(_i));
    }
  }

  Clause c0{+Variable(3), -Variable(51), Literal(99)};
  Clause c1 = make_clause({3, -51, 99, -33});

  ASSERT_FALSE(m.satisfied(c0));
  ASSERT_TRUE(m.spoiled(c0));

  ASSERT_TRUE(m.satisfied(c1));
  ASSERT_FALSE(m.spoiled(c1));

  m.unset(Variable(51));
  ASSERT_FALSE(m.satisfied(c0));
  ASSERT_FALSE(m.spoiled(c0));

  m.set(Literal(51));
  ASSERT_FALSE(m.satisfied(c0));
  ASSERT_TRUE(m.spoiled(c0));
}

TEST(ModelTest, ModelUtilities) {
  Model m(3);
  m.set(+Variable(1));
  m.set(-Variable(2));
  ASSERT_FALSE(m.all_assigned());

  Clause cp0{Literal(-1), Literal(2)};
  bool ambivalent;
  Literal witness(1);
  std::tie(ambivalent, witness) = m.ambivalent(cp0);
  ASSERT_FALSE(ambivalent);
  Clause cp1{Literal(-1), Literal(2), Literal(3)};
  std::tie(ambivalent, witness) = m.ambivalent(cp1);
  ASSERT_TRUE(ambivalent);
  ASSERT_EQ(Literal(3), witness);

  m.set(-Literal(-3));
  ASSERT_TRUE(m.all_assigned());
  std::tie(ambivalent, witness) = m.ambivalent(cp1);
  ASSERT_FALSE(ambivalent);
}

TEST(ModelTest, ModelOstream) {
  Model m(3);
  m.set(+Variable(1));
  m.set(-Variable(2));
  m.set(Literal(3));
  std::stringstream s;
  s << m;
  ASSERT_EQ("1=1 2=0 3=1", s.str());
}

TEST(ModelTest, ModelVariables) {
  Model m(3);
  m.set(+Variable(1));
  m.set(-Variable(2));
  int ind = 0;
  for (Variable v : m.variables()) {
    ++ind;
    ASSERT_EQ(Variable(ind), v);
  }
}
