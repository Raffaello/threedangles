#pragma once

#include <Mat4.hpp>
#include <Vec3d.hpp>

class Engine
{
public:
    // todo those should be constant, review the operators overloading
    static Mat4 matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up);
    static Mat4 matrix_InversePointAt(Mat4& m);

    // todo review those 2. replace with Cohen-Sutherland / Sutherland-Hodgman algo 
    static Vec3d Vector_IntersectPlane(Vec3d& plane_p, Vec3d& plane_n, Vec3d& lineStart, Vec3d& lineEnd);
    static int Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);
};
