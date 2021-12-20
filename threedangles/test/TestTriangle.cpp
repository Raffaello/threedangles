#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Triangle.hpp>

TEST(Triangle, assignment)
{
    Triangle A;

    A.faceNormal_ = Vec4(4, 3, 2, 1);
    
    A.a.v = Vec4(2, 2, 2, 2);
    A.b.v = Vec4(3, 3, 3, 3);
    A.c.v = Vec4(4, 4, 4, 4);

    A.a.col = Color(3, 0, 0, 255);
    A.b.col = Color(0, 3, 0, 255);
    A.c.col = Color(0, 0, 3, 255);

    Triangle B = A;

    EXPECT_TRUE(B.a == A.a);
    EXPECT_TRUE(B.b == A.b);
    EXPECT_TRUE(B.c == A.c);

    EXPECT_EQ(B.a.v, A.a.v);
    EXPECT_EQ(B.b.v, A.b.v);
    EXPECT_EQ(B.c.v, A.c.v);

    auto ca = A.getColor();
    auto cb = B.getColor();
    EXPECT_EQ(cb, ca);
    EXPECT_EQ(cb.r, ca.r);
    EXPECT_EQ(cb.g, ca.g);
    EXPECT_EQ(cb.b, ca.b);
    EXPECT_EQ(cb.a, ca.a);
    EXPECT_EQ(cb.r, 1);
    EXPECT_EQ(cb.g, 1);
    EXPECT_EQ(cb.b, 1);
    EXPECT_EQ(cb.a, 255);

    EXPECT_EQ(B.faceNormal_, A.faceNormal_);
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
