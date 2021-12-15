#include <Mesh.hpp>

//void Mesh::render(const Mat4& matWorld, const Cam& cam, const bool showHiddenVertexes, const Mat4& matView, const float near, const float far) noexcept
//{
//    for (const auto& tri : tris)
//    {
//        Triangle triTransformed;
//
//        triTransformed = matWorld * tri;
//
//        // Normals (back-face culling)
//        Vec4 normal = triTransformed.faceNormal();
//        float norm_dp = normal.dotProd(triTransformed.a - cam.position);
//
//        if (!showHiddenVertexes && norm_dp >= 0.0f)
//            continue;
//
//        // World Space -> View Space
//        triTransformed = matView * triTransformed;
//        // Clipping section 
//        const Vec4 plane_p_near(0.0f, 0.0f, near);
//        const Vec4 plane_n_near = Vec4(0.0f, 0.0f, 1.0f).normalize();
//        const Vec4 plane_p_far(0.0f, 0.0f, far);
//        const Vec4 plane_n_far = Vec4(0.0f, 0.0f, -1.0f).normalize();
//        std::vector<Triangle> clips;
//        // Clipping on Znear plane (triViewd -> clipped[2])
//        int nClippedTriangles = 0;
//        Triangle clipped[2];
//        nClippedTriangles = triTransformed.clipAgainstPlane(plane_p_near, plane_n_near, clipped[0], clipped[1]);
//        // clipping on Zfar plane (clipped[2] -> vector<clippped>)
//        for (int i = 0; i < nClippedTriangles; i++)
//        {
//            Triangle clippedFar[2];
//            int nClippedTrianglesFar = clipped[i].clipAgainstPlane(plane_p_far, plane_n_far, clippedFar[0], clippedFar[1]);
//            for (int n = 0; n < nClippedTrianglesFar; n++)
//                clips.push_back(clippedFar[n]);
//        }
//
//        for (const auto& c : clips)
//        {
//            // Projection 3D -> 2D & Scale into view (viewport)
//            Engine::raster_t r;
//            r.t = (matProjection * c).normByW();
//            r.faceNormal = normal;
//            trianglesToRaster.push_back(r);
//        }
//    }
//}
