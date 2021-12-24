#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Tex3.hpp>

TEST(Tex3, assigment)
{
    Tex3 a(1.0f, 2.0f, 3.0f);
    Tex3 b;
    b = a;

    EXPECT_EQ(b, a);
    EXPECT_EQ(b.u, a.u);
    EXPECT_EQ(b.v, a.v);
    EXPECT_EQ(b.w, a.w);
}
TEST(Tex3, equality)
{
    Tex3 a;
    EXPECT_EQ(a, a);
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
