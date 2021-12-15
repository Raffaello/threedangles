#include <Vec4.hpp>
#include <cmath>

Vec4::Vec4(const float x, const float y, const float z) : Vec4(x, y, z, 1.0f)
{
}

Vec4::Vec4(const float x, const float y, const float z, const float w) :x(x), y(y), z(z), w(w)
{
}

Vec4 Vec4::operator+(const Vec4& v) const
{
    return { x + v.x, y + v.y, z + v.z };
}

Vec4 Vec4::operator-(const Vec4& v) const
{
    return { x - v.x, y - v.y, z - v.z };
}

Vec4 Vec4::operator*(const float k) const
{
    return { x * k, y * k, z * k };
}

Vec4 Vec4::operator/(const float k) const
{
    return { x / k, y / k, z / k };
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
    return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
}

Vec4 Vec4::normByW() const
{
    return { x / w, y / w, z / w, 1.0f / w };
}

bool Vec4::operator==(const Vec4& v) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

Vec4 Vec4::intersectPlane(const Vec4& plane_n, const Vec4& lineStart, const Vec4& lineEnd) const noexcept
{
    //plane_n = plane_n.normalize();
    float plane_d = -plane_n.dotProd(*this);
    float ad = lineStart.dotProd(plane_n);
    float bd = lineEnd.dotProd(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vec4 lineStartToEnd = lineEnd - lineStart;
    Vec4 lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}
