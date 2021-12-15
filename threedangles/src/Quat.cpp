#include <Quat.hpp>
#include <cmath>

Quat::Quat(const float s, const float x, const float y, const float z): s(s), x(x), y(y), z(z)
{
}

Quat Quat::operator+(const Quat& q) const noexcept
{
    return Quat(s + q.s, q.x + x, q.y + y, q.z + z);
}

Quat Quat::operator-(const Quat& q) const noexcept
{
    return Quat(s - q.s, q.x - x, q.y - y, q.z - z);
}

Quat Quat::operator*(const Quat& q) const noexcept
{
    return Quat(s * q.s - x * q.x - y * q.y - z * q.z,
        s * q.x + x * q.s + y * q.z - z * q.y,
        s * q.y - x * q.z + y * q.s + z * q.x,
        s * q.z + x * q.y - y * q.x + z * q.s
    );
}

Quat Quat::operator/(const Quat& q) const noexcept
{
    const float qNorm = q.dotProd(q); //q.s* q.s + q.x * q.x + q.y * q.y + q.z * q.z;

    return Quat((s * q.s + x * q.x + y * q.y + z * q.z) / qNorm,
        (-s * q.x + x * q.s - y * q.z + z * q.y) / qNorm,
        (-s * q.y + x * q.z + y * q.s - z * q.x) / qNorm,
        (-s * q.z - x * q.y + y * q.x + z * q.s) / qNorm
    );
}

Quat Quat::operator*(const float k) const noexcept
{
    return Quat(s * k, x * k, y * k, z * k);
}

Quat Quat::operator/(const float k) const noexcept
{
    return Quat(s / k, x / k, y / k, z / k);
}

float Quat::magnitude() const noexcept
{
    return std::sqrt(s * s + x * x + y * y + z * z);
}

Quat Quat::normalize() const noexcept
{
    return Quat(*this / magnitude());
}

float Quat::dotProd(const Quat& q) const noexcept
{
    return s * q.s + x * q.x + y * q.y + z * q.z;
}

