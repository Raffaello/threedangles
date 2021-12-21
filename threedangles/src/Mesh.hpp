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

    // @todo
    // store vertexes and face as it is in the OBJ file
    // and as well store triangles
    // after use a #define to switch among them at compile time to decide.
    // storing the vertexes and faces can just generate the "transformedTriangle"
    // on the fly and can help find in O(n^2) the adiacent faces for each vertex
    // that can be improved to store them in adjacency list for better performances.
    // it would also be required to stare the "original "surface normals of each face to
    // avoid recomputing them every time.
    //
    // The Triangle data structure instead can save the initial surface normal
    // and just apply the world matrix transformation that should save some time.
    // it could seems slightly more organized.
    // It could also allow to store the adiacenjy list inside the Triangle class
    // like for a triangle having an association with the one that are adiajenct to it.
    // this can be very useful when have only the Triangle information left.
    //
    // Implementing both cases can decide understand better what would be
    // better for vertex normals, lighting and textures.
    // ---------------------------------------------------------------------------------

    
    // @todo add mesh position in the world. ?

    // Store the list of vertexes and the faces like the OBJ file model.
    // from this point can derive the adiajency list for each vertex related to faces.

    // compute the surface normals of each triangle too as a post-loading operation.
    
    // compute the vertex normals too
    
    // A mesh instead of being composed of triangle should be composed by vertexes to be processed?

    // @todo replace with vertex buffer?...
    std::vector<Triangle> tris; // ?
    std::string name;
};
