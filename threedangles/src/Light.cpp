#include <Light.hpp>
#include <cmath>
#include <algorithm>

Light::Light(const Vec4& direction, const color_t& color) :
    direction(direction),
    direction_normalized(direction.normalize()), col(color)
{
}

color_t Light::flatShading(const Vec4& triangleFaceNormal) const noexcept
{
    float dp = triangleFaceNormal.dotProd(direction_normalized);
    color_t c;
    
    c.r = static_cast<uint8_t>(std::clamp(std::round(dp * col.r), 0.0f, 255.0f));
    c.g = static_cast<uint8_t>(std::clamp(std::round(dp * col.g), 0.0f, 255.0f));
    c.b = static_cast<uint8_t>(std::clamp(std::round(dp * col.b), 0.0f, 255.0f));
    //c.a = 255;

    return c;
}

color_t Light::gouraudShading(const Triangle& triangle) const noexcept
{
    return {};
}
