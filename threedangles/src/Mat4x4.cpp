#include <Mat4x4.hpp>

//Mat4::Mat4()
//{
//}

void Mat4x4::MulMatVec(Vec3d& input, Vec3d& output)
{
    output.x = input.x * m[0][0] + input.y * m[1][0] + input.z * m[2][0] + m[3][0];
    output.y = input.x * m[0][1] + input.y * m[1][1] + input.z * m[2][1] + m[3][1];
    output.z = input.x * m[0][2] + input.y * m[1][2] + input.z * m[2][2] + m[3][2];
    float t = input.x * m[0][3] + input.y * m[1][3] + input.z * m[2][3] + m[3][3];
    //output.w = input.x * m[0][3] + input.y * m[1][3] + input.z * m[2][3] + m[3][3];

    // Zero epsilon?
    if (t != 0.0f) {
        output.x /= t;
        output.y /= t;
        output.z /= t;
    }
}

void Mat4x4::MulMatVec(Triangle& input, Triangle& output)
{
    MulMatVec(input.a, output.a);
    MulMatVec(input.b, output.b);
    MulMatVec(input.c, output.c);
}
