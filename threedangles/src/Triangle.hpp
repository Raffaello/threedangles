#pragma once

#include <Vec3d.hpp>
#include <SDL2/SDL_render.h>

class Triangle
{
public:
    Triangle(const Vec3d& a_, const Vec3d& b_, const Vec3d& c_);
    Triangle() = default;

    Vec3d a, b, c;

    Triangle operator+(const Vec3d& v);

    void setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha);
    void setColor(const Triangle& that);

    void draw( SDL_Renderer* renderer);
    /** @brief Fill a triangle - Bresenham method
      * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
      * @see https://www.avrfreaks.net/sites/default/files/triangles.c
      */
    void fill(SDL_Renderer* renderer);

private:
    uint8_t _r = 0;
    uint8_t _g = 0;
    uint8_t _b = 0;
    uint8_t _a = 0xFF;

    void draw_hline(SDL_Renderer* renderer, int x1, int x2, const int y) const noexcept;
    void fill_update_minmax(int& minx, int& maxx, int& t1x, int& t2x);
};
