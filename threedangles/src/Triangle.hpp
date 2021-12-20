#pragma once

#include <Vertex.hpp>
#include <Mat4.hpp>
#include <cstdint>
#include <Color.hpp>

class Triangle
{
public:
    Triangle(const Vertex& a_, const Vertex& b_, const Vertex& c_);
    Triangle(const Vec4& a_, const Vec4& b_, const Vec4& c_);
    Triangle() = default;
    ~Triangle() = default;

    Vertex a, b, c;

    Triangle operator*(const Mat4& m) const noexcept;
    Triangle& operator*=(const Mat4& m) noexcept;

    Triangle normByW() const noexcept;

    void setColor(const Triangle& that) noexcept;
    Color getColor() const noexcept;

    Vec4 faceNormal() const noexcept;
    Vec4 faceNormal_;
};
