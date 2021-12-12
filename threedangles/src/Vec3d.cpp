#include <Vec3d.hpp>
#include <cmath>

Vec3d::Vec3d(const float x, const float y, const float z) : Vec3d(x, y, z, 1.0f)
{
}

Vec3d::Vec3d(const float x, const float y, const float z, const float w) :x(x), y(y), z(z), w(w)
{
}

Vec3d Vec3d::operator+(const Vec3d& v) const
{
    return { x + v.x, y + v.y, z + v.z };
}

Vec3d Vec3d::operator-(const Vec3d& v) const
{
    return { x - v.x, y - v.y, z - v.z };
}

Vec3d Vec3d::operator*(const float k) const
{
    return { x * k, y * k, z * k };
}

Vec3d Vec3d::operator/(const float k) const
{
    return { x / k, y / k, z / k };
}

float Vec3d::dotProd(const Vec3d& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

float Vec3d::magnitude(const Vec3d& v) const
{
    return std::sqrt(dotProd(v));
}

Vec3d Vec3d::normalize() const
{
    return *this / magnitude(*this);
}

Vec3d Vec3d::crossProd(const Vec3d& v) const
{
    return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
}

Vec3d Vec3d::normByW() const
{
    return { x / w, y / w, z / w, 1.0f / w };
}

bool Vec3d::operator==(const Vec3d& v) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

Vec3d Vec3d::intersectPlane(const Vec3d& plane_n, const Vec3d& lineStart, const Vec3d& lineEnd) const noexcept
{
    //plane_n = plane_n.normalize();
    float plane_d = -plane_n.dotProd(*this);
    float ad = lineStart.dotProd(plane_n);
    float bd = lineEnd.dotProd(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vec3d lineStartToEnd = lineEnd - lineStart;
    Vec3d lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}
