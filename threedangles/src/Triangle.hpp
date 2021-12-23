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
    //std::array<Vertex, 3> p;

    Triangle operator*(const Mat4& m) const noexcept;
    Triangle& operator*=(const Mat4& m) noexcept;

    Triangle normByW() const noexcept;

    void setColor(const Triangle& that) noexcept;
    Color getColor() const noexcept;

    // Used for back-face culling and flat-shading
    Vec4 faceNormal() const noexcept;
    Vec4 faceNormal_;
};
