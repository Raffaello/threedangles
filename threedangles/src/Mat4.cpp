#include <Mat4.hpp>

Mat4 Mat4::operator*(const Mat4& m_)
{
    Mat4 matrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrix.m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                matrix.m[i][j] += m[i][k] * m_.m[k][j];
            }
            //matrix.m[i][j] = m[i][0] * m_.m[0][j] + m[i][1] * m_.m[1][j] + m[i][2] * m_.m[2][j] + m[i][3] * m_.m[3][j];
        }
    }

    return matrix;
}

Vec3d Mat4::operator*(const Vec3d& i)
{
    Vec3d v;

    v.x = i.x * this->m[0][0] + i.y * this->m[0][1] + i.z * this->m[0][2] + i.w * this->m[0][3];
    v.y = i.x * this->m[1][0] + i.y * this->m[1][1] + i.z * this->m[1][2] + i.w * this->m[1][3];
    v.z = i.x * this->m[2][0] + i.y * this->m[2][1] + i.z * this->m[2][2] + i.w * this->m[2][3];
    v.w = i.x * this->m[3][0] + i.y * this->m[3][1] + i.z * this->m[3][2] + i.w * this->m[3][3];
    
    return v;
}

Triangle Mat4::operator*(const Triangle& i)
{
    //return { *this * i.a, *this * i.b, *this * i.c };
    Triangle t(i);

    t.a = *this * i.a;
    t.b = *this * i.b;
    t.c = *this * i.c;

    return t;
}
