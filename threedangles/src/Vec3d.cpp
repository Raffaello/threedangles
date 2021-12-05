#include <Vec3d.hpp>
#include <cmath>

Vec3d::Vec3d(const float x, const float y, const float z) : x(x), y(y), z(z)
{
}

Vec3d::Vec3d()
{
}

Vec3d Vec3d::operator+(const Vec3d& v)
{
    return { x + v.x, y + v.y, z + v.z };
}

Vec3d Vec3d::operator-(const Vec3d& v)
{
    return { x - v.x, y - v.y, z - v.z };
}

Vec3d Vec3d::operator*(const float k)
{
    return { x * k, y * k, z * k };
}

Vec3d Vec3d::operator/(const float k)
{
    return { x / k, y / k, z / k };
}

float Vec3d::dotProd(const Vec3d& v)
{
    return x * v.x + y * v.y + z * v.z;
}

float Vec3d::magnitude(const Vec3d& v)
{
    return std::sqrt(dotProd(v));
}

Vec3d Vec3d::normalize()
{
    return *this / magnitude(*this);
}

Vec3d Vec3d::crossProd(const Vec3d& v)
{
    return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
}
