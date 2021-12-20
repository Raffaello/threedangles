#pragma once

// TODO this file coudl be restructured as a class Color.
#include <cstdint>
#include <cmath>

class Color
{
public:
    Color(const uint8_t r = 0, const uint8_t g = 0, const uint8_t b = 0, const uint8_t a = 255) : r(r), g(g), b(b), a(a) {};

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    inline bool operator==(const Color& c) const noexcept {
        return r == c.r && g == c.g && b == c.b && a == c.a;
    }
        // TODO: the color should be possible to compute incrementally and efficiently in hline
    inline static Color lerpRGB(const Color& c1, const Color& c2, const float t)
    {
        Color c;
        c.r = static_cast<uint8_t>(std::round(c1.r + t * (c2.r - c1.r)));
        c.g = static_cast<uint8_t>(std::round(c1.g + t * (c2.g - c1.g)));
        c.b = static_cast<uint8_t>(std::round(c1.b + t * (c2.b - c1.b)));

        return c;
    }

    inline static Color lerpRGBA(const Color& c1, const Color& c2, const float t)
    {
        Color c;
        c = Color::lerpRGB(c1, c2, t);
        c.a = static_cast<uint8_t>(std::round(c1.a + t * (c2.a - c1.a)));

        return c;
    }
};
