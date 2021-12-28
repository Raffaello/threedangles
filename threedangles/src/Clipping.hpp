#pragma once

#include <Color.hpp>
#include <Vec4.hpp>
#include <Triangle.hpp>
#include <vector>
#include <list>
#include <array>

class Clipping
{
    // @todo clipping is breaking "lerp"
    //      as it "cut" off the end point for the color
    //      on the clipped triangle with new vertex
    //      need to interpolate the new color?
    //
    //      not sure if clipping should be done only at raster phase per pixel instead
    //      and here discard only the triangle that are completely outside the screen,
    //      but keep the one that are partially out, not clipping them but they will be
    //      clipped during rasterization, so they will be rendered as they should be,
    //      but stopped to be rendered when out the screen......... 
public:
    Clipping() = delete;
    Clipping(const float near, const float far, const int width, const int height);

    void clipZ(const Triangle& t, std::vector<Triangle>& out) const noexcept;
    void clipScreen(const Triangle& tri, std::list<Triangle>& out) const noexcept;

    // TODO: This doesn't count w.
    //       clipping in the screen should use against a line instead of a plane ...
    int againstPlane(const Triangle& in, const Vec4& plane_p, const Vec4& plane_n, Triangle& out_tri1, Triangle& out_tri2) const noexcept;

    /**
     * Not used yet 
     * @brief fast clipping
     * @param n normal plane
     * @param v0 triangle.a
     * @param v1 triangle.b
     * @param v2 triangle.c
     * @param v3 the extra vertex for the 2nd triangle eventually
     * @return number of vectors(vertexes) 0, 3, 4
     * 0 = all clipped
     * 3 = none clipped | v0,v1',v2' => 1 triangle clipped to a new one
     * 4 = quad clipped | v0,v1,v2',v3 => 1 triangle returning 2 triangles => (v0,v1,v2'), (v0,v1,v3)
     * -1 = error unreachable step.
    */
    int clip3(const Vec4& n, Vec4& v0, Vec4& v1, Vec4& v2, Vec4& v3) const noexcept;

    // @todo replace with std::lerp when C++20
    static inline float lerp(const float a, const float b, const float t) noexcept;
    static inline Vec4 lerp(const Vec4& a, const Vec4& b, const float t) noexcept;
    static inline Tex3 lerp(const Tex3& a, const Tex3& v, const float t) noexcept;
private:
    const Vec4 plane_p_near;
    const Vec4 plane_n_near;
    const Vec4 plane_p_far;
    const Vec4 plane_n_far;

    const std::array<const Vec4, 4> planes_p;
    const std::array<const Vec4, 4> planes_n;
};

inline float Clipping::lerp(const float a, const float b, const float t) noexcept
{
    return a + t * (b - a);
}

inline Vec4 Clipping::lerp(const Vec4& a, const Vec4& b, const float t) noexcept
{
    return Vec4(
        Clipping::lerp(a.x, b.x, t),
        Clipping::lerp(a.y, b.y, t),
        Clipping::lerp(a.z, b.z, t),
        Clipping::lerp(a.w, b.w, t)
    );
}

inline Tex3 Clipping::lerp(const Tex3& a, const Tex3& b, const float t) noexcept
{
    return Tex3(
        Clipping::lerp(a.u, b.u, t),
        Clipping::lerp(a.v, b.v, t),
        Clipping::lerp(a.w, b.w, t)
    );
}
