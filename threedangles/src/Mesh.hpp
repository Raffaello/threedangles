#pragma once

#include <vector>
#include <string>
#include <memory>

#include <Vec4.hpp>
#include <Mat4.hpp>
#include <Triangle.hpp>
#include <Cam.hpp>
#include <Clipping.hpp>

class Mesh
{
public:
    Mesh() = default;

    void render(const Mat4& matProj, const Mat4& matWorld, const Mat4& matView,
        const bool showHiddenVertexes, const Cam& cam, const std::shared_ptr<Clipping>& clipping,
        std::vector<Triangle>& out) const noexcept;
    
    /**
     * @brief Load .OBJ file
     * @link  https://en.wikipedia.org/wiki/Wavefront_.obj_file
     * @param filename
     * @return
    */
    static std::shared_ptr<Mesh> loadFromOBJFile(const std::string& filename);
    
    // TODO add mesh position in the world. ?



    // TODO replace with vertex buffer?...
    std::vector<Triangle> tris; // ?
    std::string name;
};
