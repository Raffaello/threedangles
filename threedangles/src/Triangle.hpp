#pragma once

#include <Vec3d.hpp>
#include <cstdint>

class Triangle
{
public:
    Triangle(const Vec3d& a_, const Vec3d& b_, const Vec3d& c_);
    Triangle() = default;

    Vec3d a, b, c;

    Triangle operator+(const Vec3d& v);

    Triangle normByW();

    void setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha);
    void setColor(const Triangle& that);
    void getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;

private:
    // TODO move the colors to vertex
    uint8_t _r = 0;
    uint8_t _g = 0;
    uint8_t _b = 0;
    uint8_t _a = 0xFF;
};
