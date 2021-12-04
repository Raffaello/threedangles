#include <Mesh.hpp>
#include <fstream>
#include <strstream>
#include <vector>

bool Mesh::loadFromOBJFile(const std::string& filename)
{
    std::ifstream file(filename, std::ifstream::in);
    std::string line;
    std::vector<Vec3d> vertexes;

    tris.clear();
    if (!file.is_open())
        return false;

    while (std::getline(file, line))
    {
        std::strstream s;
        std::string type;

        s << line;
        s >> type;
        if (type == "#")
        {
            // comment
            continue;
        }
        else if (type == "v")
        {
            // vertex
            Vec3d v;
            s >> v.x >> v.y >> v.z;
            vertexes.push_back(v);
        }
        else if (type == "f")
        {
            // face
            // can be:
            // - int
            // - int/int
            // - int/int/int
            // - int//int
            auto a = s.str();
            int f[3]; // it could be negative..
            s >> f[0] >> f[1] >> f[2];

            tris.push_back({ vertexes.at(f[0] - 1), vertexes.at(f[1] - 1), vertexes.at(f[2] - 1) });
        }
        else if (type == "vt")
        {
            // vertex texture
        }
        else if (type == "vn")
        {
            // vertex normal
        }
        else if (type == "vp")
        {
            // vertex parameter
        }
        else if (type == "l")
        {
            // line element
        }
        else if (type == "s")
        {
            // smooth shading
        }
        else if (type == "o")
        {
            // object name
        }
        else if (type == "g")
        {
            // group name
        }

    }

    file.close();
    vertexes.clear();

    return true;
}
