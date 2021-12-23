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
    const float dp = triangleFaceNormal.dotProd(direction_normalized);

    return Color(
        static_cast<uint8_t>(std::clamp(std::round(dp * col.r), 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(std::round(dp * col.g), 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(std::round(dp * col.b), 0.0f, 255.0f))
    );
}

Color Light::gouraudShading(const Triangle& triangle, const int w1, const int w2, const int w3, const int wn) const noexcept
{
    return Color();
}
