#include <Mesh.hpp>
#include <fstream>
#include <strstream> // @todo remove as it has been deprecated
#include <cassert>

void Mesh::render(const Mat4& matProj, const Mat4& matWorld, const Mat4& matView,
    const bool showHiddenVertexes, const Cam& cam, const std::shared_ptr<Clipping>& clipping,
    std::vector<Triangle>& out) const noexcept
{
    for (const auto& tri : tris)
    {
        Triangle triTransformed = tri * matWorld;
        // Normals (back-face culling)
        triTransformed.faceNormal_ = matWorld * tri.faceNormal_;
        const float norm_dp = triTransformed.faceNormal_.dotProd(triTransformed.a.v - cam.position);
        if (!showHiddenVertexes && norm_dp >= 0.0f)
            continue;

        // Vertex normals
        triTransformed.a.normal = matWorld * triTransformed.a.normal;
        triTransformed.b.normal = matWorld * triTransformed.b.normal;
        triTransformed.c.normal = matWorld * triTransformed.c.normal;

        // Texture info assertion
        assert(triTransformed.a.texture == tri.a.texture);
        assert(triTransformed.b.texture == tri.b.texture);
        assert(triTransformed.c.texture == tri.c.texture);

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
    std::vector<Vec4> vns;

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
            // @todo: store vt and vn values

            // face
            // can be:
            // - int
            // - int/int
            // - int/int/int
            //  int//int
            std::string fs;
            std::array<int,3> v_;
            std::array<int,3> vt_;
            std::array<int, 3> vn_;
            bool has_vt = true;
            bool has_vn = true;
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
                    vt_[i] = vt;
                }
                catch (const std::invalid_argument&) {
                    has_vt = false;
                }
                try
                {
                    int vn = std::stoi(svn);
                    vn_[i] = vn;
                }
                catch (const std::invalid_argument& e) {
                    has_vn = false;
                }
            }
            
            const std::array<unsigned short, 3> face_index = { v_[0] - 1, v_[1] - 1, v_[2] - 1 };
            //mesh->faces_index.push_back(face_index);

            // Vertex normals, if presents
            if (has_vn) {
                const std::array<unsigned short, 3> fni = { vn_[0] - 1, vn_[1] - 1, vn_[2] - 1 };
                for (int i = 0; i < 3; i++) {
                    vertexes.at(face_index[i]).normal = vns.at(fni[i]);
                }
            }

            // Vertex textures, if presents
            if (has_vt) {
                // todo
            }

            mesh->tris.emplace_back(vertexes.at(face_index[0]), vertexes.at(face_index[1]), vertexes.at(face_index[2]));
        }
        else if (type == "vt")
        {
            // vertex texture
        }
        else if (type == "vn")
        {
            // vertex normal
            Vec4 v;
            ss >> v.x >> v.y >> v.z;
            vns.push_back(v);
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
    // compute surface normals
    for (auto& t : mesh->tris)
        t.faceNormal_ = t.faceNormal();

    // compute adjancency list for triangles (naive approach)
    mesh->computeAdjacencyList();

    // compute vertex normals if not present in the obj,
    // otherwise use the one provided in the obj
    if (vns.size() == 0)
        mesh->computeVertextNormals();

    return mesh;
}

void Mesh::computeAdjacencyList()
{
    adjacency_index.resize(tris.size());
    for (int i = 0; i < tris.size(); i++)
    {
        auto& t1 = tris[i];
        for (int j = 0; j < tris.size(); j++)
        {
            if (i == j)
                continue;

            auto& t2 = tris[j];

            if (t1.a.v == t2.a.v
                || t1.a.v == t2.b.v
                || t1.a.v == t2.c.v)
                adjacency_index[i][0].push_back(j);
            if (t1.b.v == t2.a.v
                || t1.b.v == t2.b.v
                || t1.b.v == t2.c.v)
                adjacency_index[i][1].push_back(j);
            if (t1.c.v == t2.a.v
                || t1.c.v == t2.b.v
                || t1.c.v == t2.c.v)
                adjacency_index[i][2].push_back(j);
        }
    }
}

void Mesh::computeVertextNormals()
{
    if (adjacency_index.size() != tris.size()) {
        computeAdjacencyList();
    }

    for (int i = 0; i < tris.size(); i++)
    {
        std::array<Vec4, 3> vns;
        for (int j = 0; j < 3; j++)
        {
            Vec4 vn = tris[i].faceNormal_;
            for (const auto& tn : adjacency_index[i][j]) {
                vn += tris[tn].faceNormal_;
            }
            vns[j] = vn.normalize();
        }

        tris[i].a.normal = vns[0];
        tris[i].b.normal = vns[1];
        tris[i].c.normal = vns[2];
    }
}
