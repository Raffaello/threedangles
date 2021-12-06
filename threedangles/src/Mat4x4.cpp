#include <Mat4x4.hpp>

Mat4x4 Mat4x4::operator*(const Mat4x4& m_)
{
    Mat4x4 matrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix.m[i][j] = m[i][0] * m_.m[0][j] + m[i][1] * m_.m[1][j] + m[i][2] * m_.m[2][j] + m[i][3] * m_.m[3][j];
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
