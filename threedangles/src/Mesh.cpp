#include <Mesh.hpp>

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
        float norm_dp = triTransformed.faceNormal_.dotProd(triTransformed.a - cam.position);

        if (!showHiddenVertexes && norm_dp >= 0.0f)
            continue;

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
