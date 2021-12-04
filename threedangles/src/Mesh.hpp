#pragma once

#include <vector>
#include <Triangle.hpp>
#include <string>

class Mesh
{
public:
    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh() = default;

    /**
     * @brief Load .OBJ file
     * @link  https://en.wikipedia.org/wiki/Wavefront_.obj_file
     * @param filename 
     * @return 
    */
    bool loadFromOBJFile(const std::string& filename);

    std::vector<Triangle> tris; // ?
};
