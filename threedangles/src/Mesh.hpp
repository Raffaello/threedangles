#pragma once

#include <vector>
#include <Triangle.hpp>
#include <string>

class Mesh
{
public:
    Mesh() = default;

    // TODO add mesh position in the world. ?

    // TODO replace with vertex buffer?...
    std::vector<Triangle> tris; // ?
    std::string name;
};
