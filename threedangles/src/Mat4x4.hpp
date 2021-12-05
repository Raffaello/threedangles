#pragma once

#include <Vec3d.hpp>
#include <Triangle.hpp>

class Mat4x4
{
public:
    void MulMatVec(Vec3d& input, Vec3d& output);
    void MulMatVec(Triangle& input, Triangle& output);

    float m[4][4] = { 0 };
private:
    
};
