#pragma once

// TODO this file coudl be restructured as a class Color.
#include <cstdint>
#include <cmath>

typedef struct color_t
{
    uint8_t r = 0,
        g = 0,
        b = 0,
        a = 255;
    inline bool operator==(const color_t& c) const noexcept {
        return r == c.r && g == c.g && b == c.b && a == c.a;
    }
} color_t;

// TODO: the color should be possible to compute incrementally and efficiently in hline
inline color_t color_lerpRGB(const color_t& c1, const color_t& c2, const float t)
{
    color_t c;
    c.r = static_cast<uint8_t>(std::round(c1.r + t * (c2.r - c1.r)));
    c.g = static_cast<uint8_t>(std::round(c1.g + t * (c2.g - c1.g)));
    c.b = static_cast<uint8_t>(std::round(c1.b + t * (c2.b - c1.b)));

    return c;
}

inline color_t color_lerpRGBA(const color_t& c1, const color_t& c2, const float t)
{
    color_t c;
    c = color_lerpRGB(c1, c2, t);
    c.a = static_cast<uint8_t>(std::round(c1.a + t * (c2.a - c1.a)));

    return c;
}
