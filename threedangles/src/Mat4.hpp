#pragma once

#include <Vec4.hpp>
#include <Triangle.hpp>
#include <array>

/**
 * @brief Matrix 4x4
 * @ling  https://butterflyofdream.wordpress.com/2016/07/05/converting-rotation-matrices-of-left-handed-coordinate-system/
*/
class Mat4
{
public:
    Mat4 operator*(const Mat4& m_) const noexcept;
    Vec4 operator*(const Vec4& i) const noexcept;
    Triangle operator*(const Triangle& i) const noexcept;

    bool operator==(const Mat4& m) const noexcept;
    
    static float deg2rad(const float deg);
    
    static Mat4 createIdentity();
    static Mat4 createTranslation(const Vec4& v);
    static Mat4 createTranslation(const float k);
    static Mat4 createScale(const float a, const float b, const float c);
    static Mat4 createRotationX(const float theta);
    static Mat4 createRotationY(const float theta);
    static Mat4 createRotationZ(const float theta);
    static Mat4 createProjection(const int w, const int h, const float fov, const float zfar, const float znear);
    static Mat4 createRotation(const float theta, const Vec4& n) noexcept;


    // todo move to private
    std::array<std::array<float, 4>, 4> m = {};
private:
    
};
