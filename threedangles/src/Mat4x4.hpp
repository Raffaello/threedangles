#pragma once

#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <array>

class Mat4x4
{
public:
    //void MulMatVec(Vec3d& input, Vec3d& output);
    //void MulMatVec(Triangle& input, Triangle& output);

    Mat4x4 operator*(const Mat4x4& m_);
    Vec3d operator*(const Vec3d& i);
    Triangle operator*(const Triangle& i);

    //float m[4][4] = { 0 };
    std::array<std::array<float, 4>, 4> m = {};
private:
    
};
