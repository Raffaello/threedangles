#pragma once

#include <Mat4.hpp>
#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <Screen.hpp>
#include <memory>
#include <string>

class Engine
{
private:
    Engine(const std::shared_ptr<Screen> screen);
public:
    
    static std::shared_ptr<Engine> createEngineSDL(const std::string& title, const int width, const int height);

    std::shared_ptr<Screen> getScreen() const noexcept;

    // todo those should be constant, review the operators overloading
    // body move to a Cam class object as are for that.
    Mat4 matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up);
    Mat4 matrix_InversePointAt(Mat4& m);

    // todo review those 2. replace with Cohen-Sutherland / Sutherland-Hodgman algo 
    Vec3d Vector_IntersectPlane(Vec3d& plane_p, Vec3d& plane_n, Vec3d& lineStart, Vec3d& lineEnd);
    int Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);

    
    void drawTriangle(const Triangle& triangle);
    /** @brief Fill a triangle - Bresenham method
     * @see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
     * @see https://www.avrfreaks.net/sites/default/files/triangles.c
     */
    void fillTriangle(const Triangle& triangle);

    // TODO color vertex interpolation? let see later...
    void draw_hline(int x1, int x2, const int y, const color_t& c) noexcept;
    void drawLine(int x1, int y1, const int x2, const int y2, const color_t& c) noexcept;
    //void drawLine(int x1, int y1, const int x2, const int y2, const color_t& c1, const color_t& c2) noexcept;
private:
    //static inline void compute_int_coord(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3) noexcept;
    std::shared_ptr<Screen> _screen;
};
