#pragma once

#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <array>

class Mat4x4
{
public:
    Mat4x4 operator*(const Mat4x4& m_);
    Vec3d operator*(const Vec3d& i);
    Triangle operator*(const Triangle& i);

    std::array<std::array<float, 4>, 4> m = {};
private:
    
};
