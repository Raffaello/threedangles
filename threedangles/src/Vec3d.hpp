#pragma once

class Vec3d
{
public:
    Vec3d(const float x, const float y, const float z);
    Vec3d();

    Vec3d operator+(const Vec3d& v);
    Vec3d operator-(const Vec3d& v);
    //Vec3d operator*(const Vec3d& v); // dotProd
    Vec3d operator/(const float k);
    float dotProd(const Vec3d& v);
    float magnitude(const Vec3d& v);
    Vec3d normalize();
    Vec3d crossProd(const Vec3d& v);

    float x, y, z, w;
};
