#pragma once

namespace simd
{
    // TEST
    inline Vec4 vector_add(const Vec4& v1, const Vec4& v2) { return Vec4(); }
    inline Vec4 vector_sub(const Vec4& v1, const Vec4& v2) { return Vec4(); }
    inline Vec4 vector_mul(const Vec4& v1, const float k) { return Vec4(); }
    inline Vec4 vector_div(const Vec4& v1, const float k) { return Vec4(); }

    inline float vector_dotProd(const Vec4& v1, const Vec4& v2) { return 0.0f; }
    inline float vector_magnitude(const Vec4& v1, const Vec4& v2) { return 0.0f; }

    inline Vec4 vector_normalize(const Vec4& v) { return Vec4(); }

    inline Vec4 vector_crossProd(const Vec4& v1, const Vec4& v2) {
        return Vec4();
    }

    inline Vec4 vector_normByW(const Vec4& v) { return Vec4(); }

    inline Vec4 vector_intersect_plane(const Vec4& v, const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) {
        return Vec4();
    }
}
