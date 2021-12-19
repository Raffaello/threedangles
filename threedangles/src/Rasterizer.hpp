#pragma once

#include <types.hpp>
#include <Triangle.hpp>
#include <Screen.hpp>
#include <memory>
#include <Light.hpp>
#include <vector>

/**
 * @brief Rasterization primitives
 * 
 * TODO: How about lights interpolation at pixel level?
 *       Does it requires to pass lights into it?
 * 
*/
class Rasterizer
{
public:
    Rasterizer() = delete;
    explicit Rasterizer(const std::shared_ptr<Screen> screen);

    void draw_hline(int x1, int x2, const int y) const noexcept;
    void draw_hline(int x1, int x2, const int y, const color_t& c) const noexcept;
    void draw_hline(int x1, int x2, const int y, color_t c1, color_t c2) const noexcept;
    /**
     * @brief Triangle is already normalized by w. Only x,y coord will be used
     * @param triangle
    */
    void drawTriangle(const Triangle& triangle) const noexcept;

    /** @brief Fill a triangle - Bresenham method
     *         Triangle is already normalized by w. Only x,y coord will be usd
     * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
     * @see https://www.avrfreaks.net/sites/default/files/triangles.c
     */
    void fillTriangle(const Triangle& triangle, const int illuminationType, const std::vector<Light>& lights) const noexcept;
    
    /**
     * @brief Fill a triangle - Pineda method
     *        Triangle is already normalized by w.
     * @link  "doc/A Parallel Algorithm for Polygon Rasterization.pdf"
    */
    void fillTriangle3(const Triangle& triangle) const noexcept;

    // TODO color vertex interpolation? let see later...
    void drawLine(int x1, int y1, const int x2, const int y2) const noexcept;
    void drawLine(int x1, int y1, const int x2, const int y2, const color_t& c) const noexcept;
    void drawLine(const int x1, const int y1, const int x2, const int y2, const color_t& c1, const color_t c2) const noexcept;

private:
    std::shared_ptr<Screen> _screen;
};
