#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Vertex.hpp>

TEST(Vertex, assigment)
{
    Vertex v1;

    v1.v = Vec4(1.f, 2.f, 3.f, 4.f);
    v1.col = Color(5, 6, 7, 8);
    Vertex v2;

    v2 = v1;

    EXPECT_EQ(v2, v1);
    EXPECT_EQ(v2.v, v1.v);
    EXPECT_EQ(v2.col, v1.col);
}
TEST(Vertex, equality)
{
    Vertex v;
    EXPECT_EQ(v, v);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
