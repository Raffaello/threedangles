#pragma once

#include <Vec3d.hpp>
#pragma once

class Mat4
{
public:
    //Mat4();
    void MulMatVec(Vec3d& input, Vec3d& output);
    float m[4][4] = { 0 };
private:
    
};
