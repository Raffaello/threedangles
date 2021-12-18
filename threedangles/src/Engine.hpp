#pragma once

#include <Mat4.hpp>
#include <Vec4.hpp>
#include <Triangle.hpp>
#include <Screen.hpp>
#include <Mesh.hpp>
#include <memory>
#include <string>
#include <vector>
#include <Cam.hpp>
#include <Light.hpp>
#include <Clipping.hpp>

class Engine
{
private:
    explicit Engine(const std::shared_ptr<Screen> screen);
public:
    static std::shared_ptr<Engine> createEngineSDL(const std::string& title, const int width, const int height) noexcept;

    inline std::shared_ptr<Screen> getScreen() const noexcept {
        return _screen;
    }

    void setMatrixProjection(const Mat4& matProj) noexcept;
    void setMatrixWorld(const Mat4& matWorld) noexcept;
    void setMatrixView(const Mat4& matView) noexcept;
    void initPerspectiveProjection(const float fov, const float far, const float near) noexcept;
    // TODO remove light as parameter and create a vector of lights in the engine itself.
    void processFrame(const Cam& cam, const Light& light, const color_t& bg_col) noexcept;

    bool showHiddenVertexes = false;
    bool illuminationOn = true;
    // 0 wire, 1 filled, 2 filled+wire, replace with an enum
    short filled = 1;
    
    /**
     * @brief Load .OBJ file
     * @link  https://en.wikipedia.org/wiki/Wavefront_.obj_file
     * @param filename
     * @return
    */
    bool addMeshFromOBJFile(const std::string& filename);
private:
    std::shared_ptr<Screen> _screen;
    std::shared_ptr<Clipping> _clipping;

    float fov = 0.0f;
    float far = 0.0f;
    float near = 0.0f;

    /**
     * @brief  Z-depth sorting (Painter's Algorithm)
    */
    void sortZ() noexcept;
    void raster(const Light& light) noexcept;

    //static inline void compute_int_coord(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3) noexcept;
    
    void draw_hline(int x1, int x2, const int y) const noexcept;
    void draw_hline(int x1, int x2, const int y, const color_t& c) const noexcept;
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
    void fillTriangle(const Triangle& triangle) const noexcept;

    // TODO color vertex interpolation? let see later...
    void drawLine(int x1, int y1, const int x2, const int y2) const noexcept;
    void drawLine(int x1, int y1, const int x2, const int y2, const color_t& c) const noexcept;

    Mat4 _matProjection;
    Mat4 _matWorld;
    Mat4 _matView;

    std::vector<Mesh> _meshes;
    std::vector<Triangle> _trianglesToRaster;
};
