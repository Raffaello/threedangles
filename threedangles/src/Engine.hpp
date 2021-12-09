#pragma once

#include <Mat4.hpp>
#include <Vec3d.hpp>

class Engine
{
public:
    // todo these should be moved in Mat4x4 class
    static Mat4 matrix_createProjection(const int w, const int h, const float fov, const float zfar, const float znear);
    static Mat4 matrix_createRotationZ(const float theta);
    static Mat4 matrix_createRotationX(const float theta);
    static Mat4 matrix_createRotationY(const float theta);
    static Mat4 matrix_createTranslation(const Vec3d& v);
    static Mat4 matrix_createIdentity();
    //static Mat4x4 matrix_createTranslation(const float k);
    static Mat4 matrix_createScale(const float a, const float b, const float c);
    // todo those should be constant, review the operators overloading
    static Mat4 matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up);
    static Mat4 matrix_InversePointAt(Mat4& m);


    // todo review those 2. replace with Cohen-Sutherland / Sutherland-Hodgman algo 
    static Vec3d Vector_IntersectPlane(Vec3d& plane_p, Vec3d& plane_n, Vec3d& lineStart, Vec3d& lineEnd);
    static int Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);
};
