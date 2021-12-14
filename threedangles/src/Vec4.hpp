#pragma once

#include <types.hpp>

class Vec4
{
public:
    Vec4(const float x, const float y, const float z);
    Vec4(const float x, const float y, const float z, const float w);
    Vec4() = default;

    // TODO: review the const to allow mutability
    //       in the this ptr, so can be more
    //       performant as well less mem consuming
    //       and having more cpu cache hits
    Vec4 operator+(const Vec4& v) const;
    Vec4 operator-(const Vec4& v) const;
    Vec4 operator*(const float k) const;
    Vec4 operator/(const float k) const;
    float dotProd(const Vec4& v) const;
    float magnitude(const Vec4& v) const;
    Vec4 normalize() const;
    Vec4 crossProd(const Vec4& v) const;
    Vec4 normByW() const;

    bool operator==(const Vec4& v) const;

    Vec4 intersectPlane(const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) const noexcept;

//private:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    color_t col;
};