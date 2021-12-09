#pragma once

#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <array>

class Mat4
{
public:
    Mat4 operator*(const Mat4& m_);
    Vec3d operator*(const Vec3d& i);
    Triangle operator*(const Triangle& i);

    std::array<std::array<float, 4>, 4> m = {};
private:
    
};
