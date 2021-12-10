#pragma once

#include <Mat4.hpp>
#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <SDL2/SDL_render.h>
class Engine
{
public:
    // todo those should be constant, review the operators overloading
    static Mat4 matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up);
    static Mat4 matrix_InversePointAt(Mat4& m);

    // todo review those 2. replace with Cohen-Sutherland / Sutherland-Hodgman algo 
    static Vec3d Vector_IntersectPlane(Vec3d& plane_p, Vec3d& plane_n, Vec3d& lineStart, Vec3d& lineEnd);
    static int Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);

    // todo replace with bresenham line algorithm
    static void drawTriangle(SDL_Renderer* renderer, const Triangle& triangle);
    /** @brief Fill a triangle - Bresenham method
     * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
     * @see https://www.avrfreaks.net/sites/default/files/triangles.c
     */
    static void fillTriangle(SDL_Renderer* renderer, const Triangle& triangle);

    static void draw_hline(SDL_Renderer* renderer, int x1, int x2, const int y) noexcept;
private:
    //static inline void compute_int_coord(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3) noexcept;
};
