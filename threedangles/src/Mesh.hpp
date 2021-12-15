#pragma once

#include <vector>
#include <Triangle.hpp>
#include <string>

class Mesh
{
public:
    Mesh() = default;

    // TODO replace with vertex buffer...
    std::vector<Triangle> tris; // ?
    std::string name;
};
