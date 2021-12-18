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
    A.faceNormal_ = Vec4(4, 3, 2, 1);

    A.a.col = { 2,0,0,0 };
    A.b.col = { 0,2,0,0 };
    A.c.col = { 0,0,2,0 };

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

    EXPECT_EQ(B.faceNormal_.x, A.faceNormal_.x);
    EXPECT_EQ(B.faceNormal_.y, A.faceNormal_.y);
    EXPECT_EQ(B.faceNormal_.z, A.faceNormal_.z);
    EXPECT_EQ(B.faceNormal_.w, A.faceNormal_.w);
    
    EXPECT_EQ(B.a.col, A.a.col);
    EXPECT_EQ(B.b.col, A.b.col);
    EXPECT_EQ(B.c.col, A.c.col);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
