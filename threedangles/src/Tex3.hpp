#pragma once

class Tex3
{
public:
    Tex3(const float u, const float v, const float w);
    Tex3() = default;
    ~Tex3() = default;

    float u;
    float v;
    float w;

    bool operator==(const Tex3& t) const noexcept;
};
