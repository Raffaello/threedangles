#include <Mesh.hpp>
#include <fstream>
#include <strstream> // todo remove as it has been deprecated

void Mesh::render(const Mat4& matProj, const Mat4& matWorld, const Mat4& matView,
    const bool showHiddenVertexes, const Cam& cam, const std::shared_ptr<Clipping>& clipping,
    std::vector<Triangle>& out) const noexcept
{
    // TODO:
    // wouldn't be better process all triangles producing:
    // 1. triTransformed first and their "faceNormal and store in the vertex if not culled
    // 2. clip all of them in Z
    // 3. projecting all of them
    // the advantage would be that the mesh can store those 3 macro operation
    // and appling to the mesh itself instead of passing them as parameters
    for (const auto& tri : tris)
    {
        Triangle triTransformed = tri * matWorld;
        // Normals (back-face culling)
        triTransformed.faceNormal_ = triTransformed.faceNormal();

        const float norm_dp = triTransformed.faceNormal_.dotProd(triTransformed.a.v - cam.position);
        if (!showHiddenVertexes && norm_dp >= 0.0f)
            continue;

        // TODO: Vertex normals


        // World Space -> View Space
        triTransformed *= matView;
        // Clipping section 
        std::vector<Triangle> clips;
        clipping->clipZ(triTransformed, clips);

        for (const auto& c : clips)
        {
            // Projection 3D -> 2D & Scale into view (viewport)
            out.emplace_back((c * matProj).normByW());
        }
    }
}

std::shared_ptr<Mesh>  Mesh::loadFromOBJFile(const std::string& filename)
{
    std::ifstream file(filename, std::ifstream::in);
    std::string line;
    std::vector<Vertex> vertexes;

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    if (!file.is_open())
        return nullptr;

    mesh->name = filename;
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
            Vertex v;
            ss >> v.v.x >> v.v.y >> v.v.z;
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
            for (int i = 0; i < 3; ++i)
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
                catch (const std::invalid_argument&) {}
                try
                {
                    int vn = std::stoi(svn);
                    //vn_[i] = vn;
                }
                catch (const std::invalid_argument& e) {}
            }

            mesh->tris.emplace_back(vertexes.at(v_[0] - 1), vertexes.at(v_[1] - 1), vertexes.at(v_[2] - 1));
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
            ss >> mesh->name;
        }
        else if (type == "g")
        {
            // group name
        }
    }

    file.close();
    vertexes.clear();

    return mesh;
}
