#pragma once

#include <Vec4.hpp>
#include <Color.hpp>
#include <Triangle.hpp>

class Light
{
public:
    Light(const Vec4& direction, const Color& color);
    const Vec4 direction; // (0.0f, 0.0f, -1.0f);
    Vec4 direction_normalized; // = light_direction.normalize();
    Color col;

    Color flatShading(const Vec4& triangleFaceNormal) const noexcept;
    Color gouraudShading(const Triangle& triangle) const noexcept;
};
