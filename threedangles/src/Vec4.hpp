#pragma once

#include <types.hpp>

class Vec4
{
public:
    // disabling experimneting code branch feature for SIMD and CUDA
#if 0
    enum class eImpl
    {
        CPU = 0,
        SSE = 1
    };

    static Vec4::eImpl impl;
#endif
    Vec4(const float x, const float y, const float z, const float w);
    Vec4(const float x, const float y, const float z);
    Vec4();
    ~Vec4() = default;

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

    Vec4& operator+=(const Vec4& v) noexcept;
    //Vec4& operator*=(const Vec4& m) noexcept;
    //Vec4& operator*=(const Mat4& m) noexcept;

    Vec4 intersectPlane(const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) const noexcept;

//private:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    color_t col;

private:
#if 0
    Vec4(*add)(const Vec4& v1, const Vec4& v2) = nullptr;
    Vec4(*sub)(const Vec4& v1, const Vec4& v2) = nullptr;
    Vec4(*mul)(const Vec4& v1, const float k) = nullptr;
    Vec4(*div)(const Vec4& v1, const float k) = nullptr;

    float(*dot)(const Vec4& v1, const Vec4& v2) = nullptr;
    float(*mag)(const Vec4& v1, const Vec4& v2) = nullptr;
    
    Vec4(*nor)(const Vec4& v) = nullptr;

    Vec4(*crp)(const Vec4& v1, const Vec4& v2) = nullptr;
    Vec4(*nrw)(const Vec4& v) = nullptr;
    Vec4(*inp)(const Vec4& v, const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) = nullptr;
#endif

};


