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

    /**
     * @brief Load .OBJ file
     * @link  https://en.wikipedia.org/wiki/Wavefront_.obj_file
     * @param filename
     * @return
    */
    static std::shared_ptr<Mesh> loadFromOBJFile(const std::string& filename);

    void computeAdjacencyList();
    void computeVertextNormals();

    // @todo add mesh position in the world. ?

    // Store the list of vertexes and the faces like the OBJ file model.
    // from this point can derive the adiajency list for each vertex related to faces.

    // compute the surface normals of each triangle too as a post-loading operation.
    
    // compute the vertex normals too
    

    std::vector<Triangle> tris; // ?
    std::string name;
    
    std::vector<std::array<std::vector<unsigned short>,3>> adjacency_index;
};
