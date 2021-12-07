#include <Engine.hpp>
#include <cmath>

constexpr float PI = 3.14159f;//2653589793f;

Mat4x4 Engine::matrix_createProjection(const int w, const int h, const float fov, const float zfar, const float znear)
{
    // Perspective Projection Matrix
    // @see https://solarianprogrammer.com/2013/05/22/opengl-101-matrices-projection-view-model/
    Mat4x4 m;
    //const float ar = static_cast<float>(w) / static_cast<float>(h);
    //const float q = zfar / (zfar - znear);
    //const float fov_rad = 1.0f / std::tan(fov * 0.5f / 180.0f * PI);

   /* m.m[0][0] = ar * fov_rad;
    m.m[1][1] = fov_rad;
    m.m[2][2] = q;
    m.m[2][3] = 1.0f;
    m.m[3][2] = -(znear * q);
    m.m[3][3] = 0.0f;*/

    const float top = znear * tan(PI / 180.0f * fov * 0.5f);
    const float bottom = -top;
    const float tb = top - bottom;
    //const float right = top * ar;
    //const float left = -right;
    const float right = static_cast<float>(w) * tb / (2*static_cast<float>(h));
    const float left = -right;

    const float rl = right - left;
    
    const float zfn = zfar - znear;

    m.m[0][0] = 2 * znear / rl;
    m.m[0][2] = (right + left) / rl;
    m.m[1][1] = 2 * znear / tb;
    m.m[1][2] = (top + bottom) / tb;
    m.m[2][2] = -(zfar + znear) / zfn;
    m.m[2][3] = -2 * zfar * znear / zfn;
    m.m[3][2] = -1;

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

Mat4x4 Engine::matrix_createRotationY(const float alpha)
{
    Mat4x4 matRotY;

    matRotY.m[0][0] = std::cos(alpha);
    matRotY.m[0][2] = std::sin(alpha);
    matRotY.m[2][0] = -std::sin(alpha);
    matRotY.m[1][1] = 1.0f;
    matRotY.m[2][2] = std::cos(alpha);
    matRotY.m[3][3] = 1.0f;

    return matRotY;
    
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

Mat4x4 Engine::matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up)
{
    Vec3d forward = (target - pos).normalize();

    Vec3d t = (forward * up.dotProd(forward));
    Vec3d newUp = (up - t).normalize();
    Vec3d newRight = newUp.crossProd(forward);

    //Dimensioning & Translation Matrix
    Mat4x4 m;
    m.m[0][0] = newRight.x; m.m[0][1] = newRight.y; m.m[0][2] = newRight.z; m.m[0][3] = 0.0f;
    m.m[1][0] = newUp.x; m.m[1][1] = newUp.y; m.m[1][2] = newUp.z; m.m[1][3] = 0.0f;
    m.m[2][0] = forward.x; m.m[2][1] = forward.y; m.m[2][2] = forward.z; m.m[2][3] = 0.0f;
    m.m[3][0] = pos.x; m.m[3][1] = pos.y; m.m[3][2] = pos.z; m.m[3][3] = 1.0f;

    return m;
}

Mat4x4 Engine::matrix_InversePointAt(Mat4x4& m)
{
    Mat4x4 matrix;

    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;

    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;

    return matrix;
}
