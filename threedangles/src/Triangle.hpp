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

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;

    void draw_hline(SDL_Renderer* renderer, int x1, int x2, const int y) const noexcept;
    inline void compute_int_coord() noexcept;

    int t1x = 0, t2x = 0, y = 0, minx = 0, maxx = 0, t1xp = 0, t2xp = 0;
    bool changed1 = false;
    bool changed2 = false;

    int8_t signx1 = 0, signx2 = 0;

    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
    int e1 = 0, e2 = 0;

    void fill_top_flat(SDL_Renderer* renderer);
    void fill_bottom_flat(SDL_Renderer* renderer);
    void fill_update_minmax();
};
