#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Color.hpp>

TEST(Color, assigment)
{
    Color c1(1,2,3,4);

    Color c2 = c1;

    EXPECT_EQ(c2, c1);
    EXPECT_EQ(c2.r, c1.r);
    EXPECT_EQ(c2.g, c1.g);
    EXPECT_EQ(c2.b, c1.b);
    EXPECT_EQ(c2.a, c1.a);
}

TEST(Color, equality)
{
    Color c;

    EXPECT_EQ(c, c);
}

class ColorLerpTest : public ::testing::TestWithParam<std::tuple<Color, Color, float, Color>> {};
TEST_P(ColorLerpTest, lerp)
{
    Color c1 = std::get<0>(GetParam());
    Color c2 = std::get<1>(GetParam());
    float t = std::get<2>(GetParam());
    Color e = std::get<3>(GetParam());

    EXPECT_EQ(Color::lerpRGB(c1, c2, t), e);
}
INSTANTIATE_TEST_CASE_P(Color, ColorLerpTest, ::testing::Values(
    std::make_tuple<>(Color(255), Color(0,255), 0, Color(255)),
    std::make_tuple<>(Color(255), Color(0,255), 1.0f, Color(0,255)),
    std::make_tuple<>(Color(255), Color(0,255), 0.5f, Color(128,128))
));

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
