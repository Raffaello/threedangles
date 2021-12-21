#pragma once

#include <Vertex.hpp>
#include <Mat4.hpp>
#include <cstdint>
#include <Color.hpp>

class Triangle
{
public:
    Triangle(const Vertex& a_, const Vertex& b_, const Vertex& c_);
    Triangle() = default;
    ~Triangle() = default;

    Vertex a;
    Vertex b;
    Vertex c;

    Triangle operator*(const Mat4& m) const noexcept;
    Triangle& operator*=(const Mat4& m) noexcept;

    Triangle normByW() const noexcept;

    void setColor(const Triangle& that) noexcept;
    Color getColor() const noexcept;

    // TODO method make it private and const the value, compute it at constructor level.
    // cant be private as Vertex might change for a triangle and then need to be recompute
    // rename it as surfaceNormal
     
    // Used for back-face culling and flat-shading
    Vec4 faceNormal() const noexcept;
    Vec4 faceNormal_;
};
