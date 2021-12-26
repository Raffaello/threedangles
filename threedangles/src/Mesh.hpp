#pragma once

#include <vector>
#include <string>
#include <memory>

#include <Vertex.hpp>
#include <Mat4.hpp>
#include <Triangle.hpp>
#include <Cam.hpp>
#include <Clipping.hpp>

class Mesh
{
public:
    Mesh() = default;
    ~Mesh() = default;

    // TODO this on Mesh.. not sure.. Should be on the engine?
    void render(const Mat4& matProj, const Mat4& matWorld, const Mat4& matView,
        const bool showHiddenVertexes, const Cam& cam, const std::shared_ptr<Clipping>& clipping,
        std::vector<Triangle>& out) const noexcept;

    bool visible = true;
    bool showTexture = false;

    // This might not be the best iterate through all triangles
    // to change that, but at the moment is acceptable.
    void setShowTexture(const bool show) noexcept;
    
    
    /**
     * @brief Load .OBJ file
     * @link  https://en.wikipedia.org/wiki/Wavefront_.obj_file
     * @param filename
     * @return
    */
    static std::shared_ptr<Mesh> loadFromOBJFile(const std::string& filename);

    void computeAdjacencyList();
    void computeVertextNormals();
    void setTexture(const std::shared_ptr<Image> texture) noexcept;

    // @todo add mesh position in the world. ?

    std::vector<Triangle> tris;
    std::string name;
    std::vector<std::array<std::vector<unsigned short>,3>> adjacency_index;
private:
    std::shared_ptr<Image> _texture = nullptr;
};
