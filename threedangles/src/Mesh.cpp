#include <Mesh.hpp>

void Mesh::render(const Mat4& matProj, const Mat4& matWorld, const Mat4& matView,
    const bool showHiddenVertexes, const Cam& cam, const std::shared_ptr<Clipping>& clipping,
    std::vector<raster_t>& out) const noexcept
{
    for (const auto& tri : tris)
    {
        Triangle triTransformed = matWorld * tri;
        // Normals (back-face culling)
        Vec4 normal = triTransformed.faceNormal();
        float norm_dp = normal.dotProd(triTransformed.a - cam.position);

        if (!showHiddenVertexes && norm_dp >= 0.0f)
            continue;

        // World Space -> View Space
        triTransformed = matView * triTransformed;
        // Clipping section 
        std::vector<Triangle> clips;
        clipping->clipZ(triTransformed, clips);

        for (const auto& c : clips)
        {
            // Projection 3D -> 2D & Scale into view (viewport)
            raster_t r;
            r.t = (matProj * c).normByW();
            r.faceNormal = normal;
            out.push_back(r);
        }
    }
}
