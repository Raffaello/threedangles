#pragma once

#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <array>

class Mat4
{
public:
    Mat4 operator*(const Mat4& m_);
    Vec3d operator*(const Vec3d& i);
    Triangle operator*(const Triangle& i);

    bool operator==(const Mat4& m);
    static float deg2rad(const float deg);
    static Mat4 createIdentity();
    static Mat4 createTranslation(const Vec3d& v);
    static Mat4 createTranslation(const float k);
    static Mat4 createScale(const float a, const float b, const float c);
    static Mat4 createRotationX(const float theta);
    static Mat4 createRotationY(const float theta);
    static Mat4 createRotationZ(const float theta);
    static Mat4 createProjection(const int w, const int h, const float fov, const float zfar, const float znear);

    // todo move to private
    std::array<std::array<float, 4>, 4> m = {};
private:
    
};
