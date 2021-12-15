#pragma once

#include <types.hpp>
#include <Vec4.hpp>
#include <Triangle.hpp>
#include <vector>
#include <list>
#include <array>

class Clipping
{
public:
    Clipping() = delete;
    Clipping(const float near, const float far, const int width, const int height);

    void clipZ(const Triangle& t, std::vector<Triangle>& out) const noexcept;
    void clipScreen(const raster_t& tri, std::list<raster_t>& out) const noexcept;

    int againstPlane(const Triangle& in, const Vec4& plane_p, const Vec4& plane_n, Triangle& out_tri1, Triangle& out_tri2) const noexcept;
private:
    const Vec4 plane_p_near;
    const Vec4 plane_n_near;
    const Vec4 plane_p_far;
    const Vec4 plane_n_far;
    const Vec4 plane_p0;
    const Vec4 plane_n0;
    const Vec4 plane_p1;
    const Vec4 plane_n1;
    const Vec4 plane_p2;
    const Vec4 plane_n2;
    const Vec4 plane_p3;
    const Vec4 plane_n3;

    const std::array<const Vec4, 4> plane_p;
    const std::array<const Vec4, 4> plane_n;
};
