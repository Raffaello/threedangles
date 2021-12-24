#pragma once

#include <Vec4.hpp>
#include <Color.hpp>
#include <Tex3.hpp>

class Vertex
{
public:
    Vertex() = default;
    ~Vertex() = default;
    
    Vec4 v;
    Color col;
    Vec4 normal; // should it change in a Vertex * Matrix  or Triangle * Matrix operation?
    Tex3 texture;

    inline bool operator==(const Vertex& vx) const noexcept;
};

inline bool Vertex::operator==(const Vertex& vx) const noexcept
{
    return v == vx.v && col == vx.col;
}

