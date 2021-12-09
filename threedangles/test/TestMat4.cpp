#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Mat4.hpp>
#include <limits>

void cmp_matrices(Mat4& a, Mat4& b)
{
    constexpr float e = std::numeric_limits<float>::epsilon();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            EXPECT_NEAR(a.m[i][j], b.m[i][j], e);
        }
    }
}
TEST(Mat4, mulVec)
{
    Vec3d v(1,2,3);
    Mat4 m;
    m.m[0][0] = 1;
    m.m[0][1] = 2;
    m.m[0][2] = 3;

    m.m[1][0] = 4;
    m.m[1][1] = 5;
    m.m[1][2] = 6;

    m.m[2][0] = 7;
    m.m[2][1] = 8;
    m.m[2][2] = 9;

    m.m[3][3] = 1.0f;
    
    Vec3d u = m * v;
    Vec3d w(1 * 1 + 2 * 2 + 3 * 3, 1 * 4 + 2 * 5 + 3 * 6, 1 * 7 + 2 * 8 + 3 * 9);

    EXPECT_TRUE(u == w);
}

TEST(Mat4, mulMat)
{
    Mat4 a;
    a.m[0][0] = 1;
    a.m[0][1] = 2;
    a.m[0][2] = 3;

    a.m[1][0] = 4;
    a.m[1][1] = 5;
    a.m[1][2] = 6;

    a.m[2][0] = 7;
    a.m[2][1] = 8;
    a.m[2][2] = 9;

    Mat4 b;
    b.m[0][0] = 1;
    b.m[0][1] = 2;
    b.m[0][2] = 3;

    b.m[1][0] = 1;
    b.m[1][1] = 2;
    b.m[1][2] = 3;

    b.m[2][0] = 1;
    b.m[2][1] = 2;
    b.m[2][2] = 3;

    Mat4 c;
    c.m[0][0] = 1 * 1 + 2 * 1 + 3 * 1;
    c.m[0][1] = 1 * 2 + 2 * 2 + 3 * 2;
    c.m[0][2] = 1 * 3 + 2 * 3 + 3 * 3;

    c.m[1][0] = 4 * 1 + 5 * 1 + 6 * 1;
    c.m[1][1] = 4 * 2 + 5 * 2 + 6 * 2;
    c.m[1][2] = 4 * 3 + 5 * 3 + 6 * 3;

    c.m[2][0] = 7 * 1 + 8 * 1 + 9 * 1;
    c.m[2][1] = 7 * 2 + 8 * 2 + 9 * 2;
    c.m[2][2] = 7 * 3 + 8 * 3 + 9 * 3;

    EXPECT_TRUE(a * b == c);
}

TEST(Mat4, identity)
{
    Mat4 a = Mat4::createIdentity();
    Mat4 b;
    b.m[0][0] = b.m[1][1] = b.m[2][2] = b.m[3][3] = 1.0f;
    EXPECT_TRUE(a == b);
}

TEST(Mat4, Translation)
{
    Vec3d v(1, 2, 3);
    Mat4 b;
    b.m[0][3] = v.x;
    b.m[1][3] = v.y;
    b.m[2][3] = v.z;
    b.m[0][0] = b.m[1][1] = b.m[2][2] = b.m[3][3] = 1.0f;

    EXPECT_TRUE(Mat4::createTranslation(v)  == b);

    b.m[0][3] = b.m[1][3] = b.m[2][3] = 0.5f;
    EXPECT_TRUE(Mat4::createTranslation(0.5f) == b);
}

TEST(Mat4, Scale)
{
    Mat4 a = Mat4::createScale(2, 4, 6);
    Mat4 b;
    
    b.m[0][0] = 2;
    b.m[1][1] = 4;
    b.m[2][2] = 6;
    b.m[3][3] = 1.0f;

    EXPECT_TRUE(a == b);
}

TEST(Mat4, RotX)
{
    Mat4 a = Mat4::createRotationX(Mat4::deg2rad(90));
    Mat4 b;
    b.m[0][0] = 1;
    b.m[1][1] = 0;
    b.m[1][2] = 1;
    b.m[2][1] = -1;
    b.m[2][2] = 0;
    b.m[3][3] = 1;
    cmp_matrices(a, b);
}

TEST(Mat4, RotY)
{
    Mat4 a = Mat4::createRotationY(Mat4::deg2rad(90));
    Mat4 b;
    b.m[0][0] = 0;
    b.m[0][2] = -1;
    b.m[1][1] = 1;
    b.m[2][0] = 1;
    b.m[2][2] = 0;
    b.m[3][3] = 1;
    cmp_matrices(a, b);
}

TEST(Mat4, RotZ)
{
    Mat4 a = Mat4::createRotationZ(Mat4::deg2rad(90));
    Mat4 b;
    b.m[0][0] = 0;
    b.m[0][1] = 1;
    b.m[1][0] = -1;
    b.m[1][1] = 0;
    b.m[2][2] = 1;
    b.m[3][3] = 1;
    cmp_matrices(a, b);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
