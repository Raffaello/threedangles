#include <Light.hpp>
#include <cmath>
#include <algorithm>

Light::Light(const Vec4& direction, const Color& color) :
    direction(direction),
    direction_normalized(direction.normalize()), col(color)
{
}

Color Light::flatShading(const Vec4& triangleFaceNormal) const noexcept
{
    float dp = triangleFaceNormal.dotProd(direction_normalized);
    Color c;
    
    c.r = static_cast<uint8_t>(std::clamp(std::round(dp * col.r), 0.0f, 255.0f));
    c.g = static_cast<uint8_t>(std::clamp(std::round(dp * col.g), 0.0f, 255.0f));
    c.b = static_cast<uint8_t>(std::clamp(std::round(dp * col.b), 0.0f, 255.0f));
    //c.a = 255;

    return c;
}

Color Light::gouraudShading(const Triangle& triangle) const noexcept
{
    return {};
}
