#include <Tex3.hpp>

Tex3::Tex3(const float u, const float v, const float w) : u(u), v(v), w(w)
{
}

bool Tex3::operator==(const Tex3& t) const noexcept
{
    return u == t.u && v == t.v && w == t.w;
}
