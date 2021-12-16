#pragma once

#include <Vec4.hpp>
#include <cmath>
#include <cassert>

namespace cpu
{
    inline Vec4 vector_add(const Vec4& v1, const Vec4& v2) { return Vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
    inline Vec4 vector_sub(const Vec4& v1, const Vec4& v2) { return Vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
    inline Vec4 vector_mul(const Vec4& v1, const float k)  { return Vec4(v1.x * k, v1.y * k, v1.z * k); }
    inline Vec4 vector_div(const Vec4& v1, const float k)  { return Vec4(v1.x / k, v1.y / k, v1.z / k); }

    inline float vector_dotProd(const Vec4& v1, const Vec4& v2)   { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
    inline float vector_magnitude(const Vec4& v1, const Vec4& v2) { return std::sqrt(vector_dotProd(v1, v2)); }

    //inline Vec4 vector_normalize(const Vec4& v) { return (v /  v.magnitude(v)); }
    
    inline Vec4 vector_crossProd(const Vec4& v1, const Vec4& v2) {
        return Vec4(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

    inline Vec4 vector_normByW(const Vec4& v) { return Vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1.0f / v.w); }

    inline Vec4 vector_intersect_plane(const Vec4& v, const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) {
        assert(plane_n == plane_n.normalize());

        const float plane_d = -vector_dotProd(plane_n, v);
        const float ad = vector_dotProd(lineStart, plane_n);
        const float bd = vector_dotProd(lineEnd, plane_n);
        const float t = (-plane_d - ad) / (bd - ad);
        const Vec4 lineStartToEnd = vector_sub(lineEnd, lineStart);
        const Vec4 lineToIntersect = vector_mul(lineStartToEnd, t);
        return vector_add(lineStart, lineToIntersect);
    }
}
