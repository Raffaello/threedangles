#pragma once

#include <Vec3d.hpp>
#include <SDL2/SDL_render.h>

class Triangle
{
public:
    Triangle(const Vec3d& a, const Vec3d& b, const Vec3d& c);
    Triangle() = default;

    Vec3d a, b, c;

    void draw( SDL_Renderer* renderer);
    /** @brief Fill a triangle - Bresenham method
      * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
      */
    void fill(SDL_Renderer* renderer);

private:
    void draw_hline(SDL_Renderer* renderer, int x1, int x2, const int y);
};
