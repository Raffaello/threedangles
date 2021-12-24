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

    bool operator==(const Tex3& t) const noexcept;
    inline Tex3 operator*(const float k) const noexcept;
    inline Tex3 operator/(const float k) const noexcept;
};

inline Tex3 Tex3::operator*(const float k) const noexcept
{
    return Tex3(u * k, v * k, w * k);
}

inline Tex3 Tex3::operator/(const float k) const noexcept
{
    return Tex3(u / k, v / k, w / k);
}
