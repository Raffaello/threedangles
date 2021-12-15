#pragma once

#include <Vec4.hpp>
#include <types.hpp>

class Light
{
public:
    Light(const Vec4& direction, const color_t& color);
    //const Vec4 direction; // (0.0f, 0.0f, -1.0f);
    Vec4 direction_normalized; // = light_direction.normalize();
    color_t col;

    color_t flatShading(const Vec4& triangleFaceNormal) const noexcept;
};