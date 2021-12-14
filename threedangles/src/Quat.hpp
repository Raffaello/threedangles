#pragma once

/**
 * @brief Quaternion
 * @link https://www.3dgep.com/understanding-quaternions/
 * Quite similar to Vec4
 * TODO: see if Quat and Vec4 can be generalized (Vec3 would be enough to use here)
*/
class Quat
{
public:
    Quat() = default;
    Quat(const float s, const float x, const float y, const float z);

    inline Quat operator+(const Quat& q) const noexcept;
    inline Quat operator-(const Quat& q) const noexcept;
    inline Quat operator*(const Quat& q) const noexcept;
    inline Quat operator/(const Quat& q) const noexcept;
    inline Quat operator*(const float k) const noexcept;
    inline Quat operator/(const float k) const noexcept;
    inline float magnitude() const noexcept;
    inline Quat normalize() const noexcept;
    inline float dotProd(const Quat& q) const noexcept;


    float s = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

