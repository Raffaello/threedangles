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

    //Triangle operator+(const Vec4& v) const noexcept;
    Triangle operator*(const Mat4& m) const noexcept;
    Triangle& operator*=(const Mat4& m) noexcept;

    Triangle normByW() const noexcept;

    // TODO: the triangle color should be the average of the vertexes' color
    // BODY: can't really set it up, unless replace the vertexes color.
    // BODY: it would have conflicts when using a vertex buffer?
    // BODY: need to resolve shared vertexes with different color
    void setColor(const Color& c) noexcept;
    void setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha) noexcept;
    void setColor(const Triangle& that) noexcept;
    void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const noexcept;
    Color getColor() const noexcept;

    Vec4 faceNormal() const noexcept;
    Vec4 faceNormal_;
private:
    // TODO move the colors to vertex
    Color col;
};
