#pragma once

/**
 * @brief Quaternion
 * @link https://www.3dgep.com/understanding-quaternions/
 * Quite similar to Vec4
 * @todo: see if Quat and Vec4 can be generalized (Vec3 would be enough to use here)
*/
class Quat
{
public:
    Quat() = default;
    Quat(const float s, const float x, const float y, const float z);

    Quat operator+(const Quat& q) const noexcept;
    Quat operator-(const Quat& q) const noexcept;
    Quat operator*(const Quat& q) const noexcept;
    Quat operator/(const Quat& q) const noexcept;
    Quat operator*(const float k) const noexcept;
    Quat operator/(const float k) const noexcept;
    float magnitude() const noexcept;
    Quat normalize() const noexcept;
    float dotProd(const Quat& q) const noexcept;


    float s = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

