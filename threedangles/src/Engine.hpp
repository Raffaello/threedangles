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

    //void processFrame(const Cam& cam) noexcept;

    bool showHiddenVertexes = false;
    bool illuminationOn = true;

    typedef struct raster_t
    {
        Triangle t;
        Vec4 faceNormal;
    } raster_t;

    std::vector<raster_t> trianglesToRaster;
    /**
     * @brief Load .OBJ file
     * @link  https://en.wikipedia.org/wiki/Wavefront_.obj_file
     * @param filename
     * @return
    */
    bool loadMeshFromOBJFile(const std::string& filename);

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
public: // todo make them private again after..
    Mat4 matProjection;
    Mat4 matWorld;
    Mat4 matView;

    std::vector<Mesh> meshes;
};
