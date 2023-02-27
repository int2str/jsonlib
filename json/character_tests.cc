#include <mutex>

#include "character_utils.hh"
#include "testrunner/testrunner.h"

TEST(CharacterUtils_EmptyNumbersStringIsEmpty) {
  auto res = json::internal::extractNumber("");
  ASSERT_TRUE(res.empty());
}

TEST(CharacterUtils_NumbersWithLeadingZeros) {
  auto res = json::internal::extractNumber("0");
  EXPECT_EQ(res.size(), 1);
  EXPECT_EQ(res, "0");

  res = json::internal::extractNumber("00");
  ASSERT_TRUE(res.empty());

  // 0.0 +
  // 0.00 +
  // 00.0 -
}

TEST(CharacterUtils_NonSenseNumbers) {
  auto res = json::internal::extractNumber("-");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber("--");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber(".");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber("-1.");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber("-1...2");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber("-1E");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber("-E1");
  ASSERT_TRUE(res.empty());

  res = json::internal::extractNumber("-0.1EE1");
  ASSERT_TRUE(res.empty());
}

TEST(CharacterUtils_Integers) {
  auto res = json::internal::extractNumber("1 Hello");
  EXPECT_EQ(res.size(), 1);
  EXPECT_EQ(res, "1");

  res = json::internal::extractNumber("123 World");
  EXPECT_EQ(res.size(), 3);
  EXPECT_EQ(res, "123");

  res = json::internal::extractNumber("-456 Minus");
  EXPECT_EQ(res.size(), 4);
  EXPECT_EQ(res, "-456");

  res = json::internal::extractNumber("-987654321");
  EXPECT_EQ(res.size(), 10);
  EXPECT_EQ(res, "-987654321");
}

TEST(CharacterUtils_RealNumbers) {
  auto res = json::internal::extractNumber("1.23");
  EXPECT_EQ(res.size(), 4);
  EXPECT_EQ(res, "1.23");

  res = json::internal::extractNumber("-4.5678");
  EXPECT_EQ(res.size(), 7);
  EXPECT_EQ(res, "-4.5678");

  res = json::internal::extractNumber("-0.001");
  EXPECT_EQ(res.size(), 6);
  EXPECT_EQ(res, "-0.001");

  res = json::internal::extractNumber("12345.6789");
  EXPECT_EQ(res.size(), 10);
  EXPECT_EQ(res, "12345.6789");
}

TEST(CharacterUtils_NumbersWithExponent) {
  auto res = json::internal::extractNumber("1.2E3 Expo");
  EXPECT_EQ(res.size(), 5);
  EXPECT_EQ(res, "1.2E3");

  res = json::internal::extractNumber("-0.0E-3");
  EXPECT_EQ(res.size(), 7);
  EXPECT_EQ(res, "-0.0E-3");

  res = json::internal::extractNumber("1E10");
  EXPECT_EQ(res.size(), 4);
  EXPECT_EQ(res, "1E10");

  res = json::internal::extractNumber("10000E-10");
  EXPECT_EQ(res.size(), 9);
  EXPECT_EQ(res, "10000E-10");
}
