#include <Vec4.hpp>
#include <cmath>
#include <cassert>

Vec4::Vec4(const float x, const float y, const float z) : Vec4(x, y, z, 1.0f)
{
}

Vec4::Vec4(const float x, const float y, const float z, const float w) :x(x), y(y), z(z), w(w)
{
}

Vec4 Vec4::operator+(const Vec4& v) const
{
    return Vec4(x + v.x, y + v.y, z + v.z);
}

Vec4 Vec4::operator-(const Vec4& v) const
{
    return Vec4(x - v.x, y - v.y, z - v.z);
}

Vec4 Vec4::operator*(const float k) const
{
    return Vec4(x * k, y * k, z * k);
}

Vec4 Vec4::operator/(const float k) const
{
    return Vec4(x / k, y / k, z / k);
}

float Vec4::dotProd(const Vec4& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

float Vec4::magnitude(const Vec4& v) const
{
    return std::sqrt(dotProd(v));
}

Vec4 Vec4::normalize() const
{
    return *this / magnitude(*this);
}

Vec4 Vec4::crossProd(const Vec4& v) const
{
    return Vec4(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
}

Vec4 Vec4::normByW() const
{
    return Vec4(x / w, y / w, z / w, 1.0f / w );
}

bool Vec4::operator==(const Vec4& v) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

Vec4& Vec4::operator=(const Vec4& v) noexcept
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    col = v.col;
    
    return *this;
}

Vec4& Vec4::operator+=(const Vec4& v) noexcept
{
    x = x + v.x;
    y = y + v.y;
    z = z + v.z;
    
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
    assert(plane_n == plane_n.normalize());

    const float plane_d = -plane_n.dotProd(*this);
    const float ad = lineStart.dotProd(plane_n);
    const float bd = lineEnd.dotProd(plane_n);
    const float t = (-plane_d - ad) / (bd - ad);
    const Vec4 lineStartToEnd = lineEnd - lineStart;
    const Vec4 lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}
