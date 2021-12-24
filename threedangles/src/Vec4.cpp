#include <Vec4.hpp>
#include <cmath>
#include <cassert>
#if 0
#include <cpu/Vec4CPU.hpp>
#include <simd/Vec4SSE.hpp>

Vec4::eImpl Vec4::impl = Vec4::eImpl::CPU;
#endif

Vec4::Vec4(const float x, const float y, const float z, const float w) :x(x), y(y), z(z), w(w)
{
    // This is an overhead for every single Vector...
    // Must be done at compile time with MACROs so no need of using function pointers
    // the macro can just put the right namespace (cpu::, sse::) for eg.
    // cons only at compile time a feature is enabled, can't fallback on CPU without SSE.
    // unless there is a specifc pattern to do it, 
    // but anyway it is compiled already with SSE even the CPU implementation due
    // to compiler optimization
    // probably can just be removed then.
    // ----------------------------------------------------------------------------
    // To do it properly:
    // these routines should be done as separated shared objects:
    // load them dynamically based on CPUID instruction set detection.
    // Using a plug-in interface architecture for them, sharing same interface.
    // So there will be a vec4cpu.dll (so), vec4sse.dll (so), etc..
    // the engine will load the required dll (so) based on cpuid info.
    // ----------------------------------------------------------------------------
    // alternatively use static function pointers and initialize the class from the engine
    // to associate the respective function once.

#if 0
    switch (impl)
    {
    case eImpl::CPU:
        add = &cpu::vector_add;
        sub = &cpu::vector_sub;
        mul = &cpu::vector_mul;
        div = &cpu::vector_div;
        dot = &cpu::vector_dotProd;
        mag = &cpu::vector_magnitude;
        nor = &cpu::vector_normalize;
        crp = &cpu::vector_crossProd;
        nrw = &cpu::vector_normByW;
        inp = &cpu::vector_intersect_plane;
        break;
    case eImpl::SSE:
        add = &simd::vector_add;
        sub = &simd::vector_sub;
        mul = &simd::vector_mul;
        div = &simd::vector_div;
        dot = &simd::vector_dotProd;
        mag = &simd::vector_magnitude;
        nor = &simd::vector_normalize;
        crp = &simd::vector_crossProd;
        nrw = &simd::vector_normByW;
        inp = &simd::vector_intersect_plane;
        break;
    default:
        break;
    }
#endif
}

Vec4::Vec4(const float x, const float y, const float z) : Vec4(x, y, z, 1.0f)
{
}

Vec4::Vec4() : Vec4(0.0f, 0.0f, 0.0f, 1.0f)
{}

Vec4 Vec4::operator+(const Vec4& v) const
{
    //Vec4 u(*this);
    //u.x += v.x; u.y += v.y; u.z += v.z;
    //return u;
    return Vec4(x + v.x, y + v.y, z + v.z);
#if 0
    //return cpu::vector_add(*this, v);
    return add(*this, v);
#endif
}

Vec4 Vec4::operator-(const Vec4& v) const
{
    //Vec4 u(*this);
    //u.x -= v.x; u.y -= v.y;  u.z -= v.z;
    //return u;
    return Vec4(x - v.x, y - v.y, z - v.z);
#if 0
    //return cpu::vector_sub(*this, v);
    return sub(*this, v);
#endif
}

Vec4 Vec4::operator*(const float k) const
{
    Vec4 u(*this);
    u.x *= k; u.y *= k; u.z *= k;
    return u;
#if 0
    //return cpu::vector_mul(*this, k);
    return mul(*this, k);
#endif
}

Vec4 Vec4::operator/(const float k) const
{
    Vec4 u(*this);
    u.x /= k; u.y /= k; u.z /= k;
    return u;
#if 0
    //return cpu::vector_div(*this, k);
    return div(*this, k);
#endif
}

float Vec4::dotProd(const Vec4& v) const
{
    return x * v.x + y * v.y + z * v.z;
#if 0
    //return cpu::vector_dotProd(*this, v);
    return dot(*this, v);
#endif
}

float Vec4::magnitude(const Vec4& v) const
{
    return std::sqrt(dotProd(v));
#if 0
    //return cpu::vector_magnitude(*this, v);
    return mag(*this, v);
#endif
}

Vec4 Vec4::normalize() const
{
    return *this / magnitude(*this);
#if 0
    //return cpu::vector_normalize(*this);
    return nor(*this);
#endif
}

Vec4 Vec4::crossProd(const Vec4& v) const
{
    return Vec4(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
#if 0
    //return cpu::vector_crossProd(*this, v);
    return crp(*this, v);
#endif
}

Vec4 Vec4::normByW() const
{
    Vec4 u(*this);
    u.x /= w; u.y /= w; u.z /= w; u.w = 1.0f / w;
    return u;
#if 0
    //return cpu::vector_normByW(*this);
    return nrw(*this);
#endif
}

bool Vec4::operator==(const Vec4& v) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

Vec4& Vec4::operator+=(const Vec4& v) noexcept
{
    x += v.x;
    y += v.y;
    z += v.z;
    
    return *this;
}

Vec4& Vec4::operator*=(const Vec4& v) noexcept
{
    x *= v.x;
    y *= v.y;
    z *= v.z;

    return *this;
}

//Vec4& Vec4::operator*=(const Mat4& m) noexcept
//{
//    x = x * m.m[0][0] + y * m.m[0][1] + z * m.m[0][2] + w * m.m[0][3];
//    y = x * m.m[1][0] + y * m.m[1][1] + z * m.m[1][2] + w * m.m[1][3];
//    z = x * m.m[2][0] + y * m.m[2][1] + z * m.m[2][2] + w * m.m[2][3];
//    w = x * m.m[3][0] + y * m.m[3][1] + z * m.m[3][2] + w * m.m[3][3];
//
//    return *this;
//}



Vec4 Vec4::intersectPlane(const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) const noexcept
{
    float t = 0.0f;
    return intersectPlane(plane_n, lineStart, lineEnd, t);
}

Vec4 Vec4::intersectPlane(const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd, float& out_t) const noexcept
{
    assert(plane_n == plane_n.normalize());

    const float plane_d = -plane_n.dotProd(*this);
    const float ad = lineStart.dotProd(plane_n);
    const float bd = lineEnd.dotProd(plane_n);
    out_t = (-plane_d - ad) / (bd - ad);
    const Vec4 lineStartToEnd = lineEnd - lineStart;
    const Vec4 lineToIntersect = lineStartToEnd * out_t;
    return lineStart + lineToIntersect;
#if 0
    //return cpu::vector_intersect_plane(*this, plane_n, lineStart, lineEnd);
    return inp(*this, plane_n, lineStart, lineEnd);
#endif
}
