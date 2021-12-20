#include <Vertex.hpp>

bool Vertex::operator==(const Vertex& vx) const noexcept
{
	return v == vx.v && col == vx.col;
}
