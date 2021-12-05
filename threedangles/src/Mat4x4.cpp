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
    output.w = input.x * m[0][3] + input.y * m[1][3] + input.z * m[2][3] + m[3][3];

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

Mat4x4 Mat4x4::operator*(const Mat4x4& m)
{
    Mat4x4 matrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix.m[i][j] = this->m[i][0] * m.m[0][j] + this->m[i][1] * m.m[1][j] + this->m[i][2] * m.m[2][j] + this->m[i][3] * m.m[3][j];
        }
    }

    return matrix;
}

Vec3d Mat4x4::operator*(const Vec3d& i)
{
    Vec3d v;

    v.x = i.x * this->m[0][0] + i.y * this->m[1][0] + i.z * this->m[2][0] + i.w * this->m[3][0];
    v.y = i.x * this->m[0][1] + i.y * this->m[1][1] + i.z * this->m[2][1] + i.w * this->m[3][1];
    v.z = i.x * this->m[0][2] + i.y * this->m[1][2] + i.z * this->m[2][2] + i.w * this->m[3][2];
    v.w = i.x * this->m[0][3] + i.y * this->m[1][3] + i.z * this->m[2][3] + i.w * this->m[3][3];
    
    return v;
}

Triangle Mat4x4::operator*(const Triangle& i)
{
    return { *this * i.a, *this * i.b, *this * i.c };
}
