#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Vec3d.hpp>


TEST(Vec3d, equality)
{
    Vec3d a;
    EXPECT_TRUE(a == a);
}

TEST(Vec3d, sum)
{
    Vec3d a(1, 1, 1);
    Vec3d b(2, 2, 2);
    Vec3d c(3, 3, 3);

    EXPECT_TRUE(a + b == c);
    EXPECT_TRUE(b + a == c);
}

TEST(Vec3d, dif)
{
    Vec3d a(1, 1, 1);
    Vec3d b(2, 2, 2);
    Vec3d c(3, 3, 3);

    EXPECT_TRUE(c - b == a);
}

TEST(Vec3d, mul)
{
    Vec3d a(1, 1, 1);
    Vec3d b(2, 2, 2);

    EXPECT_TRUE(a * 2 == b);
}

TEST(Vec3d, div)
{
    Vec3d a(1, 1, 1);
    Vec3d b(2, 2, 2);

    EXPECT_TRUE(b / 2 == a);
}

TEST(Vec3d, dotProd)
{
    Vec3d a(1, 2, 3);
    Vec3d b(3, 4, 5);
    float c = 3 + 8 + 15;
    
    EXPECT_TRUE(a.dotProd(b) == c);
    EXPECT_TRUE(b.dotProd(a) == c);
}

TEST(Vec3d, magnitude)
{
    Vec3d a(1, 2, 5);
    Vec3d b(3, 4, 5);
    float c = 6; // 11+25 = 36 -> sqrt = 6

    EXPECT_TRUE(a.magnitude(b) == c);
    EXPECT_TRUE(b.magnitude(a) == c);
}

TEST(Vec3d, normalize)
{
    Vec3d a(1, 2, 3);
    Vec3d b(3, 4, 5);

    EXPECT_TRUE(a.normalize() == a / sqrt(1 * 1 + 2 * 2 + 3 * 3));
    EXPECT_TRUE(b.normalize() == b / sqrt(3 * 3 + 4 * 4 + 5 * 5));
}

TEST(Vec3d, crossProd)
{
    Vec3d a(1, 2, 3);
    Vec3d b(3, 4, 5);
    Vec3d c(2 * 5 - 3 * 4, 3 * 3 - 1 * 5, 1 * 4 - 2 * 3);
    Vec3d d(4 * 3 - 5 * 2, 5 * 1 - 3 * 3, 3 * 2 - 4 * 1);
    Vec3d z;

    EXPECT_TRUE(a.crossProd(a) == z);
    EXPECT_TRUE(b.crossProd(b) == z);
    EXPECT_TRUE(a.crossProd(b) == c);
    EXPECT_TRUE(b.crossProd(a) == d);
}

TEST(Vec3d, normByW)
{
    Vec3d a(1, 2, 3, 2);
    Vec3d b(1, 1, 1, 1);
    Vec3d c(0.5, 1.0, 3. / 2., 0.5);
    Vec3d d(1, 1, 1, 1);
    Vec3d z;

    EXPECT_TRUE(a.normByW() == c);
    EXPECT_TRUE(b.normByW() == d);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
