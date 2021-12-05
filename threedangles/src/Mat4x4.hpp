#pragma once

#include <Vec3d.hpp>
#include <Triangle.hpp>

class Mat4x4
{
public:
    void MulMatVec(Vec3d& input, Vec3d& output);
    void MulMatVec(Triangle& input, Triangle& output);

    Mat4x4 operator*(const Mat4x4& m);
    Vec3d operator*(const Vec3d& i);
    Triangle operator*(const Triangle& i);

    float m[4][4] = { 0 };
private:
    
};
