#include <Mesh.hpp>
#include <fstream>
#include <strstream> // @todo remove as it has been deprecated

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

        // World Space -> View Space
        triTransformed *= matView;
        // Clipping section (this could be done at the rasterizer level?)
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
    int vni = 0;

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
            
            const std::array<unsigned short, 3> face_index = { v_[0] - 1, v_[1] - 1, v_[2] - 1 };
            //mesh->faces_index.push_back(face_index);
            mesh->tris.emplace_back(vertexes.at(face_index[0]), vertexes.at(face_index[1]), vertexes.at(face_index[2]));
        }
        else if (type == "vt")
        {
            // vertex texture
        }
        else if (type == "vn")
        {
            // vertex normal
            Vertex v;
            ss >> v.normal.x >> v.normal.y >> v.normal.z;
            vertexes[vni++].normal = v.normal;
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
    /*mesh->adjacency_index.resize(mesh->tris.size());
    for (int i = 0; i < mesh->tris.size(); i++)
    {
        auto& t1 = mesh->tris[i];
        for (int j = 0; j < mesh->tris.size(); j++)
        {
            if (i == j)
                continue;

            auto& t2 = mesh->tris[j];

            if (t1.a.v == t2.a.v
                || t1.a.v == t2.b.v
                || t1.a.v == t2.c.v)
                mesh->adjacency_index[i][0].push_back(j);
            else if (t1.b.v == t2.a.v
                    || t1.b.v == t2.b.v
                    || t1.b.v == t2.c.v)
                mesh->adjacency_index[i][1].push_back(j);
            else if (t1.c.v == t2.a.v
                    || t1.c.v == t2.b.v
                    || t1.c.v == t2.c.v)
                mesh->adjacency_index[i][2].push_back(j);
        }
    }*/
    if (vni > 0) {
        // compute vertex normals
    }

    // compute vertex normals
    /*for(int i =0; i<mesh->tris.size();i++)
    {
        std::array<Vec4, 3> vns;
        for (int j = 0; j < 3; j++)
        {
            Vec4 vn = mesh->tris[i].faceNormal_;
            for (const auto& tn : mesh->adjacency_index[i][j]) {
                vn += mesh->tris[tn].faceNormal_;
            }
            vns[j] = vn.normalize();
        }

        mesh->tris[i].a.normal = vns[0];
        mesh->tris[i].b.normal = vns[1];
        mesh->tris[i].c.normal = vns[2];
    }*/

    // compute vertex normal very slow and naive
    //for (auto& t : mesh->tris)
    //    t.a.normal = t.b.normal = t.c.normal = t.faceNormal_;

    //for (int i = 0; i < mesh->tris.size(); i++)
    //{
    //    auto& ti = mesh->tris[i];
    //    //ti.a.normal = ti.b.normal = ti.c.normal = ti.faceNormal_;
    //    for (int j = i+1; j < mesh->tris.size(); j++)
    //    {
    //        auto& tj = mesh->tris[j];
    //        if      (ti.a == tj.a) ti.a.normal += tj.a.normal;
    //        else if (ti.a == tj.b) ti.a.normal += tj.b.normal;
    //        else if (ti.a == tj.c) ti.a.normal += tj.c.normal;

    //        if      (ti.b == tj.a) ti.b.normal += tj.a.normal;
    //        else if (ti.b == tj.b) ti.b.normal += tj.b.normal;
    //        else if (ti.b == tj.c) ti.b.normal += tj.c.normal;

    //        if      (ti.c == tj.a) ti.c.normal += tj.a.normal;
    //        else if (ti.c == tj.b) ti.c.normal += tj.b.normal;
    //        else if (ti.c == tj.c) ti.c.normal += tj.c.normal;
    //            
    //       
    //    }

    //    ti.a.normal = ti.a.normal.normalize();
    //    ti.b.normal = ti.b.normal.normalize();
    //    ti.c.normal = ti.c.normal.normalize();
    //}

    return mesh;
}
