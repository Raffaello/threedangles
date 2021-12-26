#include <Tex3.hpp>

Tex3::Tex3(const float u, const float v, const float w) : u(u), v(v), w(w)
{
}

Tex3::Tex3(const float u, const float v) : Tex3(u, v, 1.0f)
{
}

Tex3::Tex3() : Tex3(0.0f, 0.0f)
{
}

bool Tex3::operator==(const Tex3& t) const noexcept
{
    return u == t.u && v == t.v && w == t.w;
}
