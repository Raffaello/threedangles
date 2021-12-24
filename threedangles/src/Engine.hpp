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
#include <Rasterizer.hpp>

class Engine
{
private:
    explicit Engine(const std::shared_ptr<Screen> screen);
public:
    // @todo create a factory class (friend of engine) to decouple from SDL
    static std::shared_ptr<Engine> createEngineSDL(const std::string& title, const int width, const int height) noexcept;

    inline std::shared_ptr<Screen> getScreen() const noexcept {
        return _screen;
    }

    void setPerpsectiveCorrection(const bool on) const noexcept {
        _rasterizer->perspectiveCorrection = on;
    }

    void setZBuffer(const bool on) const noexcept {
        _rasterizer->depthBuffer = on;
    }

    void setMatrixProjection(const Mat4& matProj) noexcept;
    void setMatrixWorld(const Mat4& matWorld) noexcept;
    void setMatrixView(const Mat4& matView) noexcept;
    void initPerspectiveProjection(const float fov, const float far, const float near) noexcept;
    
    void processFrame(const Cam& cam, const Color& bg_col) noexcept;

    bool showHiddenVertexes = false;
    // 0 = off, 1 = flat, 2=gouraud
    short illuminationOn = 1;
    // 0 wire, 1 filled, 2 filled+wire, replace with an enum
    short filled = 1;

    void addMesh(const std::shared_ptr<Mesh>& mesh);
    void addLight(const Light& light);
    // @todo add the "remove/update/set method" for mesh and light

private:
    std::shared_ptr<Screen> _screen;
    std::shared_ptr<Clipping> _clipping;
    std::shared_ptr<Rasterizer> _rasterizer;

    float fov = 0.0f;
    float far = 0.0f;
    float near = 0.0f;

    /**
     * @brief  Z-depth sorting (Painter's Algorithm)
    */
    void sortZ() noexcept;
    void sortZReverse() noexcept;
    void raster() noexcept;

    Mat4 _matProjection;
    Mat4 _matWorld;
    Mat4 _matView;

    std::vector<Triangle> _trianglesToRaster;
    std::vector<Light> _lights;
    uint8_t _lightCounts = 1;
    std::vector<std::shared_ptr<Mesh>> _meshes;
};
