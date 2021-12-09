#pragma once

class Vec3d
{
public:
    Vec3d(const float x, const float y, const float z);
    Vec3d(const float x, const float y, const float z, const float w);
    Vec3d() = default;

    Vec3d operator+(const Vec3d& v);
    Vec3d operator-(const Vec3d& v);
    Vec3d operator*(const float k);
    Vec3d operator/(const float k);
    float dotProd(const Vec3d& v);
    float magnitude(const Vec3d& v);
    Vec3d normalize();
    Vec3d crossProd(const Vec3d& v);
    Vec3d normByW();

    bool operator==(const Vec3d& v);


    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};
