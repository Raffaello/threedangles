#pragma once

#include <Mat4.hpp>
#include <Vec4.hpp>
#include <Triangle.hpp>
#include <Screen.hpp>
#include <memory>
#include <string>

class Engine
{
private:
    explicit Engine(const std::shared_ptr<Screen> screen);
public:
    
    static std::shared_ptr<Engine> createEngineSDL(const std::string& title, const int width, const int height) noexcept;

    std::shared_ptr<Screen> getScreen() const noexcept;

    // todo those should be constant, review the operators overloading
    // body move to a Cam class object as are for that.
    Mat4 matrix_pointAt(const Vec4& pos, const Vec4& target, const Vec4& up) const;
    Mat4 matrix_InversePointAt(const Mat4& m) const;

    /**
     * @brief Triangle is already normalized by w. Only x,y coord will be used
     * @param triangle 
    */
    void drawTriangle(const Triangle& triangle);

    /** @brief Fill a triangle - Bresenham method
     *         Triangle is already normalized by w. Only x,y coord will be usd 
     * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
     * @see https://www.avrfreaks.net/sites/default/files/triangles.c
     */
    void fillTriangle(const Triangle& triangle);

    // TODO color vertex interpolation? let see later...
    void draw_hline(int x1, int x2, const int y) noexcept;
    void draw_hline(int x1, int x2, const int y, const color_t& c) noexcept;

    void drawLine(int x1, int y1, const int x2, const int y2) noexcept;
    void drawLine(int x1, int y1, const int x2, const int y2, const color_t& c) noexcept;
    //void drawLine(int x1, int y1, const int x2, const int y2, const color_t& c1, const color_t& c2) noexcept;
private:
    //static inline void compute_int_coord(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3) noexcept;
    std::shared_ptr<Screen> _screen;
};
