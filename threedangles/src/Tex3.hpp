#pragma once

#include <Image.hpp>
#include <memory>

class Tex3
{
public:
    Tex3(const float u, const float v, const float w);
    Tex3(const float u, const float v);
    Tex3();
    ~Tex3() = default;

    float u;
    float v;
    float w;

    Tex3 normByW() const noexcept;

    bool operator==(const Tex3& t) const noexcept;
};
