#include <Light.hpp>
#include <cmath>
#include <algorithm>

Light::Light(const Vec4& direction, const color_t& color) :
    direction_normalized(direction.normalize()), col(color)
{
}

color_t Light::flatShading(const Vec4& triangleFaceNormal) const noexcept
{
    float dp = triangleFaceNormal.dotProd(direction_normalized);
    color_t c;
    
    c.r = static_cast<uint8_t>(std::max(0.0f, std::round(dp * col.r)));
    c.g = static_cast<uint8_t>(std::max(0.0f, std::round(dp * col.g)));
    c.b = static_cast<uint8_t>(std::max(0.0f, std::round(dp * col.b)));
    //c.a = 255;

    return c;
}
