#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Triangle.hpp>


TEST(Triangle, assignment)
{
    Triangle A;

    A.a = Vec4();
    A.b = Vec4();
    A.c = Vec4();
    A.setColor(1, 2, 3, 4);

    Triangle B = A;

    EXPECT_TRUE(B.a == A.a);
    EXPECT_TRUE(B.b == A.b);
    EXPECT_TRUE(B.c == A.c);
    auto ca = A.getColor();
    auto cb = B.getColor();
    EXPECT_EQ(cb.r, ca.r);
    EXPECT_EQ(cb.g, ca.g);
    EXPECT_EQ(cb.b, ca.b);
    EXPECT_EQ(cb.a, ca.a);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
