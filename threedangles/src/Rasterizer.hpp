#pragma once

#include <Color.hpp>
#include <Triangle.hpp>
#include <Screen.hpp>
#include <memory>
#include <Light.hpp>
#include <vector>

/**
 * @brief Rasterization primitives
*/
class Rasterizer
{
public:
    // todo: remove
    bool perspectiveCorrection = false;
    
    Rasterizer() = delete;
    explicit Rasterizer(const std::shared_ptr<Screen> screen);

    void draw_hline(int x1, int x2, const int y) const noexcept;
    void draw_hline(int x1, int x2, const int y, const Color& c) const noexcept;
    void draw_hline(int x1, int x2, const int y, Color c1, Color c2) const noexcept;
    /**
     * @brief Triangle is already normalized by w. Only x,y coord will be used
     * @param triangle
    */
    void drawTriangle(const Triangle& triangle) const noexcept;
    void drawTriangle(const Triangle& triangle, const Color& c) const noexcept;


    /** @brief Fill a triangle - Bresenham method
     *         Triangle is already normalized by w. Only x,y coord will be usd
     * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
     * @see https://www.avrfreaks.net/sites/default/files/triangles.c
     */
    void fillTriangleNoInterpolation(const Triangle& triangle, const int illuminationType, const std::vector<Light>& lights) const noexcept;
    
    /**
     * @brief Fill a triangle - Pineda method
     *        Triangle is already normalized by w.
     * @link  "doc/A Parallel Algorithm for Polygon Rasterization.pdf"
     * THIS IS a lot slower than 'fillTriangle', need improvements
    */
    void fillTriangle3(const Triangle& triangle, const int illuminationType, const std::vector<Light>& lights) const noexcept;

    // @todo color vertex interpolation? let see later...
    void drawLine(int x1, int y1, const int x2, const int y2) const noexcept;
    void drawLine(int x1, int y1, const int x2, const int y2, const Color& c) const noexcept;
    void drawLine(const int x1, const int y1, const int x2, const int y2, const Color& c1, const Color c2) const noexcept;

private:
    std::shared_ptr<Screen> _screen;
};
