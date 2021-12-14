#pragma once

#include <types.hpp>

// TODO renane Vec4
//      add vertex normal, vertex texture, color etc..
class Vec3d
{
public:
    Vec3d(const float x, const float y, const float z);
    Vec3d(const float x, const float y, const float z, const float w);
    Vec3d() = default;

    // TODO: review the const to allow mutability
    //       in the this ptr, so can be more
    //       performant as well less mem consuming
    //       and having more cpu cache hits
    Vec3d operator+(const Vec3d& v) const;
    Vec3d operator-(const Vec3d& v) const;
    Vec3d operator*(const float k) const;
    Vec3d operator/(const float k) const;
    float dotProd(const Vec3d& v) const;
    float magnitude(const Vec3d& v) const;
    Vec3d normalize() const;
    Vec3d crossProd(const Vec3d& v) const;
    Vec3d normByW() const;

    bool operator==(const Vec3d& v) const;

    Vec3d intersectPlane(const Vec3d& plane_n, const Vec3d& lineStart, const Vec3d& lineEnd) const noexcept;

//private:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    color_t col;
};
