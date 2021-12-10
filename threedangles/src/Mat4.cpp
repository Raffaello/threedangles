#include <Mat4.hpp>
#include <cmath>

constexpr float PI = 3.141592653589793f;
constexpr float DEG2RAD = ((PI) / 180.0f);

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

bool Mat4::operator==(const Mat4& m)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (this->m[i][j] != m.m[i][j])
                return false;
        }
    }

    return true;
}

float Mat4::deg2rad(const float deg)
{
    return deg * DEG2RAD;
}

Mat4 Mat4::createIdentity()
{
    Mat4 m;

    m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;

    return m;
}

Mat4 Mat4::createTranslation(const Vec3d& v)
{
    Mat4 m;

    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;
    m.m[0][3] = v.x;
    m.m[1][3] = v.y;
    m.m[2][3] = v.z;

    return m;
}

Mat4 Mat4::createTranslation(const float k)
{
    return createTranslation({ k, k, k });
}

Mat4 Mat4::createScale(const float a, const float b, const float c)
{
    Mat4 m;

    m.m[0][0] = a;
    m.m[1][1] = b;
    m.m[2][2] = c;
    m.m[3][3] = 1.0f;

    return m;
}

Mat4 Mat4::createRotationX(const float theta)
{
    Mat4 matRotX;

    matRotX.m[0][0] = 1.0f;
    matRotX.m[1][1] = std::cos(theta);
    matRotX.m[1][2] = std::sin(theta);
    matRotX.m[2][1] = -matRotX.m[1][2];
    matRotX.m[2][2] = matRotX.m[1][1];
    matRotX.m[3][3] = 1.0f;

    return matRotX;
}

Mat4 Mat4::createRotationY(const float theta)
{
    Mat4 matRotY;

    matRotY.m[0][0] = std::cos(theta);
    matRotY.m[0][2] = -std::sin(theta);
    matRotY.m[1][1] = 1.0f;
    matRotY.m[2][0] = -matRotY.m[0][2];
    matRotY.m[2][2] = matRotY.m[0][0];
    matRotY.m[3][3] = 1.0f;

    return matRotY;
}

Mat4 Mat4::createRotationZ(const float theta)
{
    Mat4 matRotZ;

    matRotZ.m[0][0] = std::cos(theta);
    matRotZ.m[0][1] = std::sin(theta);
    matRotZ.m[1][0] = -matRotZ.m[0][1];
    matRotZ.m[1][1] = matRotZ.m[0][0];
    matRotZ.m[2][2] = 1.0f;
    matRotZ.m[3][3] = 1.0f;

    return matRotZ;
}

Mat4 Mat4::createProjection(const int w, const int h, const float fov, const float zfar, const float znear)
{
    // Perspective Projection Matrix
    // @see https://solarianprogrammer.com/2013/05/22/opengl-101-matrices-projection-view-model/
    // @see https://cglearn.codelight.eu/pub/computer-graphics/frames-of-reference-and-projection
    Mat4 m;
    //const float ar = static_cast<float>(w) / static_cast<float>(h);
    /*
    const float q = zfar / (zfar - znear);
    const float fov_rad = 1.0f / std::tan(fov * 0.5f / 180.0f * PI);

    m.m[0][0] = ar * fov_rad;
    m.m[1][1] = fov_rad;
    m.m[2][2] = q;
    m.m[2][3] = 1.0f;
    m.m[3][2] = -(znear * q);
    m.m[3][3] = 0.0f;
    */
    /*
    const float top = znear * tan(fov * 0.5f * DEG2RAD);
    const float bottom = -top;
    const float tb = top - bottom;
    //const float right = top * ar;
    const float right = static_cast<float>(w) * tb / (2.0f * static_cast<float>(h));
    //const float right = ar / tan(fov * 0.5f * deg2rad);
    const float left = -right;

    const float rl = right - left;

    const float zfn = zfar - znear;

    // right hand, frustum
    m.m[0][0] = 2.0f * znear / rl;
    m.m[0][2] = (right + left) / rl;
    m.m[1][1] = 2.0f * znear / tb;
    m.m[1][2] = (top + bottom) / tb;
    m.m[2][2] = -(zfar + znear) / zfn;
    m.m[2][3] = -2.0f * zfar * znear / zfn;
    m.m[3][2] = -1.0f;
    */
    /*
    // left handed?
    m.m[0][2] *= -1.0f;
    m.m[1][2] *= -1.0f;
    m.m[2][2] *= -1.0f;
    m.m[3][2] *= -1.0f;
    */
    
    const float ar = static_cast<float>(w) / static_cast<float>(h);
    
    m.m[0][0] = 1.0f / (ar * tan(fov * 0.5f * DEG2RAD));
    m.m[1][1] = 1.0f / tan(fov * 0.5f * DEG2RAD);
    m.m[2][2] = (znear + zfar) / (znear - zfar);
    m.m[2][3] = (2.0f * zfar * znear) / (znear - zfar);
    m.m[3][2] = -1.0f;
    


    return m;
}
