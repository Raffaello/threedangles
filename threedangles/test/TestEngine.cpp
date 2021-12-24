#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Engine.hpp>

//class EngineLerpTest : public ::testing::TestWithParam<std::tuple<float, float, float, float>> {};
//TEST_P(EngineLerpTest, lerp)
//{
//    float a = std::get<0>(GetParam());
//    float b = std::get<1>(GetParam());
//    float t = std::get<2>(GetParam());
//    float e = std::get<3>(GetParam());
//
//    EXPECT_EQ(Engine::lerp(a, b, t), e);
//}
//INSTANTIATE_TEST_CASE_P(Engine, EngineLerpTest, ::testing::Values(
//    std::make_tuple<>(0, 100, 0, 0),
//    std::make_tuple<>(0, 100, 1, 100),
//    std::make_tuple<>(0, 100, .5, 50),
//    std::make_tuple<>(10, 40, 0, 10),
//    std::make_tuple<>(10, 40, 1, 40),
//    std::make_tuple<>(10, 40, 0.5, 25),
//    std::make_tuple<>(10, 40, 0.9, 37),
//    std::make_tuple<>(40, 10, 1.0-0.9, 37)
//));

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
