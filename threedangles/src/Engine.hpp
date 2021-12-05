#pragma once

#include <Mat4x4.hpp>
#include <Vec3d.hpp>

class Engine
{
public:
    // todo these should be moved in Mat4x4 class
    static Mat4x4 matrix_createProjection(const int w, const int h, const float fov, const float zfar, const float znear);
    static Mat4x4 matrix_createRotationZ(const float alpha);
    static Mat4x4 matrix_createRotationX(const float alpha);
    static Mat4x4 matrix_createRotationY(const float alpha);
    static Mat4x4 matrix_createTranslation(const Vec3d& v);
    static Mat4x4 matrix_createIdentity();
    // todo those should be constant, review the operators overloading
    static Mat4x4 matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up);
    static Mat4x4 matrix_InversePointAt(Mat4x4& m);
};
