#pragma once

#include <Vec4.hpp>
#include <Mat4.hpp>
#include <cstdint>
#include <types.hpp>

class Triangle
{
public:
    Triangle(const Vec4& a_, const Vec4& b_, const Vec4& c_);
    Triangle() = default;
    ~Triangle() = default;

    Vec4 a, b, c;

    Triangle operator+(const Vec4& v) const noexcept;
    Triangle operator*(const Mat4& m) const noexcept;
    Triangle& operator*=(const Mat4& m) noexcept;

    Triangle normByW() const noexcept;

    // TODO: the triangle color should be the average of the vertexes' color
    // BODY: can't really set it up, unless replale the vertexes color.
    // BODY: it would have conflicts when using a vertex buffer?
    // BODY: need to resolve shared vertexes with different color
    void setColor(const color_t& c) noexcept;
    void setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha) noexcept;
    void setColor(const Triangle& that) noexcept;
    void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const noexcept;
    color_t getColor() const noexcept;

    Vec4 faceNormal() const noexcept;
private:
    // TODO move the colors to vertex
    color_t col;
};

typedef struct raster_t
{
    Triangle t;
    Vec4 faceNormal;
} raster_t;
