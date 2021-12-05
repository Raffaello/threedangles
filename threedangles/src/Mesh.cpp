#include <Mesh.hpp>
#include <fstream>
#include <strstream> // todo remove as it has been deprecated
#include <vector>
#include <exception>

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
        std::strstream ss;
        std::string type;

        ss << line;
        ss >> type;
        if (type == "#")
        {
            // comment
            continue;
        }
        else if (type == "v")
        {
            // vertex
            Vec3d v;
            ss >> v.x >> v.y >> v.z;
            vertexes.push_back(v);
        }
        else if (type == "f")
        {
            // TODO: store vt and vn values
            
            // face
            // can be:
            // - int
            // - int/int
            // - int/int/int
            //  int//int
            std::string fs;
            int v_[3];
            //int vt_[3];
            //int vn_[3];
            for(int i =0; i<3; ++i)
            {
                ss >> fs;
                std::strstream ssf;
                ssf << fs;
                std::string sv, svt, svn;
                std::getline(ssf, sv, '/');
                std::getline(ssf, svt, '/');
                std::getline(ssf, svn, '/');
                // they could be negative..
                int v = std::stoi(sv);
                v = v >= 0 ? v : vertexes.size() + v;
                v_[i] = v;
                try 
                {
                    int vt = std::stoi(svt);
                    //vt_[i] = vt;
                }
                catch (const std::invalid_argument& e) {}
                try
                {
                    int vn = std::stoi(svn);
                    //vn_[i] = vn;
                }
                catch (const std::invalid_argument& e) {}
            }

            tris.emplace_back(vertexes.at(v_[0] - 1), vertexes.at(v_[1] - 1), vertexes.at(v_[2] - 1));
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
