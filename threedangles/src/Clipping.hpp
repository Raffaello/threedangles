#pragma once

#include <types.hpp>
#include <Vec4.hpp>
#include <Triangle.hpp>
#include <vector>
#include <list>
#include <array>

class Clipping
{
    // TODO clipping is breaking "lerp"
    //      as it "cut" off the end point for the color
    //      on the clipped triangle with new vertex
    //      need to interpolate the new color
public:
    Clipping() = delete;
    Clipping(const float near, const float far, const int width, const int height);

    void clipZ(const Triangle& t, std::vector<Triangle>& out) const noexcept;
    void clipScreen(const Triangle& tri, std::list<Triangle>& out) const noexcept;

    int againstPlane(const Triangle& in, const Vec4& plane_p, const Vec4& plane_n, Triangle& out_tri1, Triangle& out_tri2) const noexcept;
private:
    const Vec4 plane_p_near;
    const Vec4 plane_n_near;
    const Vec4 plane_p_far;
    const Vec4 plane_n_far;

    const std::array<const Vec4, 4> planes_p;
    const std::array<const Vec4, 4> planes_n;
};
