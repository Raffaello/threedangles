#pragma once

#include <Vec3d.hpp>
#include <cstdint>
#include <types.hpp>

class Triangle
{
public:
    Triangle(const Vec3d& a_, const Vec3d& b_, const Vec3d& c_);
    Triangle() = default;

    Vec3d a, b, c;
    

    Triangle operator+(const Vec3d& v);

    Triangle normByW();

    // TODO: the triangle color should be the average of the vertexes' color
    // BODY: can't really set it up, unless replale the vertexes color.
    // BODY: it would have conflicts when using a vertex buffer?
    // BODY: need to resolve shared vertexes with different color
    void setColor(const color_t& c) noexcept;
    void setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha);
    void setColor(const Triangle& that);
    void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;
    color_t getColor() const noexcept;

    Vec3d faceNormal() const noexcept;

private:
    // TODO move the colors to vertex
    color_t col;
};
