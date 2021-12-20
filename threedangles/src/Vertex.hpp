#pragma once

#include <Vec4.hpp>
#include <Color.hpp>

class Vertex
{
public:
    Vertex() = default;
    ~Vertex() = default;
    
    Vec4 v;
    Color col;
    Vec4 normal; // should it change in a Vertex * Matrix  or Triangle * Matrix operation?

    bool operator==(const Vertex& vx) const noexcept;
};
