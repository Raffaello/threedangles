#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Vec4.hpp>


TEST(Vec4, equality)
{
    Vec4 a;
    EXPECT_TRUE(a == a);
}

TEST(Vec4, sum)
{
    Vec4 a(1, 1, 1);
    Vec4 b(2, 2, 2);
    Vec4 c(3, 3, 3);

    EXPECT_TRUE(a + b == c);
    EXPECT_TRUE(b + a == c);
}

TEST(Vec4, dif)
{
    Vec4 a(1, 1, 1);
    Vec4 b(2, 2, 2);
    Vec4 c(3, 3, 3);

    EXPECT_TRUE(c - b == a);
}

TEST(Vec4, mul)
{
    Vec4 a(1, 1, 1);
    Vec4 b(2, 2, 2);

    EXPECT_TRUE(a * 2 == b);
}

TEST(Vec4, div)
{
    Vec4 a(1, 1, 1);
    Vec4 b(2, 2, 2);

    EXPECT_TRUE(b / 2 == a);
}

TEST(Vec4, dotProd)
{
    Vec4 a(1, 2, 3);
    Vec4 b(3, 4, 5);
    float c = 3 + 8 + 15;
    
    EXPECT_TRUE(a.dotProd(b) == c);
    EXPECT_TRUE(b.dotProd(a) == c);
}

TEST(Vec4, magnitude)
{
    Vec4 a(1, 2, 5);
    Vec4 b(3, 4, 5);
    float c = 6; // 11+25 = 36 -> sqrt = 6

    EXPECT_TRUE(a.magnitude(b) == c);
    EXPECT_TRUE(b.magnitude(a) == c);
}

TEST(Vec4, normalize)
{
    Vec4 a(1, 2, 3);
    Vec4 b(3, 4, 5);

    EXPECT_TRUE(a.normalize() == a / sqrt(1.0f * 1.0f + 2.0f * 2.0f + 3.0f * 3.0f));
    EXPECT_TRUE(b.normalize() == b / sqrt(3.0f * 3.0f + 4.0f * 4.0f + 5.0f * 5.0f));
}

TEST(Vec4, crossProd)
{
    Vec4 a(1, 2, 3);
    Vec4 b(3, 4, 5);
    Vec4 c(2 * 5 - 3 * 4, 3 * 3 - 1 * 5, 1 * 4 - 2 * 3);
    Vec4 d(4 * 3 - 5 * 2, 5 * 1 - 3 * 3, 3 * 2 - 4 * 1);
    Vec4 z;

    EXPECT_TRUE(a.crossProd(a) == z);
    EXPECT_TRUE(b.crossProd(b) == z);
    EXPECT_TRUE(a.crossProd(b) == c);
    EXPECT_TRUE(b.crossProd(a) == d);
}

TEST(Vec4, normByW)
{
    Vec4 a(1, 2, 3, 2);
    Vec4 b(1, 1, 1, 1);
    Vec4 c(0.5, 1.0, 3. / 2., 0.5);
    Vec4 d(1, 1, 1, 1);
    Vec4 z;

    EXPECT_TRUE(a.normByW() == c);
    EXPECT_TRUE(b.normByW() == d);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
