#pragma once

#include <cstdint>

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

