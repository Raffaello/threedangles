#include <Engine.hpp>
#include <cmath>

constexpr float PI = 3.14159f;//2653589793f;

Mat4x4 Engine::matrix_createProjection(const int w, const int h, const float fov, const float zfar, const float znear)
{
    Mat4x4 m;
    const float ar = static_cast<float>(w) / static_cast<float>(h);
    const float q = zfar / (zfar - znear);
    const float fov_rad = 1.0f / std::tan(fov * 0.5f / 180.0f * PI);

    m.m[0][0] = ar * fov_rad;
    m.m[1][1] = fov_rad;
    m.m[2][2] = q;
    m.m[2][3] = 1.0f;
    m.m[3][2] = -(znear * q);
    m.m[3][3] = 0.0f;

    return m;
}

Mat4x4 Engine::matrix_createRotationZ(const float alpha)
{
    Mat4x4 matRotZ;

    matRotZ.m[0][0] = std::cos(alpha);
    matRotZ.m[0][1] = std::sin(alpha);
    matRotZ.m[1][0] = -std::sin(alpha);
    matRotZ.m[1][1] = std::cos(alpha);
    matRotZ.m[2][2] = 1.0f;
    matRotZ.m[3][3] = 1.0f;

    return matRotZ;
}

Mat4x4 Engine::matrix_createRotationX(const float alpha)
{
    Mat4x4 matRotX;

    matRotX.m[0][0] = 1.0f;
    matRotX.m[1][1] = std::cos(alpha * 0.5f);
    matRotX.m[1][2] = std::sin(alpha * 0.5f);
    matRotX.m[2][1] = -std::sin(alpha * 0.5f);
    matRotX.m[2][2] = std::cos(alpha * 0.5f);
    matRotX.m[3][3] = 1.0f;

    return matRotX;
}

Mat4x4 Engine::matrix_createTranslation(const Vec3d& v)
{
    Mat4x4 m;

    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;
    m.m[3][0] = v.x;
    m.m[3][1] = v.y;
    m.m[3][2] = v.z;

    return m;
}

Mat4x4 Engine::matrix_createIdentity()
{
    Mat4x4 m;

    m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;
    
    return m;
}
