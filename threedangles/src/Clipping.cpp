#include <Clipping.hpp>
#include <cassert>


Clipping::Clipping(const float near, const float far, const int width, const int height) :
    plane_p_near(0.0f, 0.0f, near),
    plane_n_near(0.0f, 0.0f, 1.0f),
    plane_p_far(0.0f, 0.0f, far),
    plane_n_far(0.0f, 0.0f, -1.0f),

    planes_p {
        Vec4(0.0f, 0.0f, 0.0f),
        Vec4(0.0f, static_cast<float>(height) - 1.0f, 0.0f),
        Vec4(0.0f, 0.0f, 0.0f),
        Vec4(static_cast<float>(width) - 1.0f, 0.0f, 0.0f)
    },
    planes_n {
        Vec4(0.0f, 1.0f, 0.0f).normalize(),
        Vec4(0.0f, -1.0f, 0.0f).normalize(),
        Vec4(1.0f, 0.0f, 0.0f).normalize(),
        Vec4(-1.0f, 0.0f, 0.0f).normalize()
    }
{
}

void Clipping::clipZ(const Triangle& t, std::vector<Triangle>& out) const noexcept
{
    // Clipping on Znear plane (triViewd -> clipped[2])
    int nClippedTriangles = 0;
    std::array<Triangle, 2> clipped;
    nClippedTriangles = againstPlane(t, plane_p_near, plane_n_near, clipped[0], clipped[1]);
    // clipping on Zfar plane (clipped[2] -> vector<clippped>)
    for (int i = 0; i < nClippedTriangles; i++)
    {
        std::array<Triangle,2> clippedFar;
        int nClippedTrianglesFar = againstPlane(clipped[i], plane_p_far, plane_n_far, clippedFar[0], clippedFar[1]);
        for (int n = 0; n < nClippedTrianglesFar; n++)
            out.push_back(clippedFar[n]);
    }
}

void Clipping::clipScreen(const Triangle& tri, std::list<Triangle>& out) const noexcept
{
    // CLIPPING on Screen size
    std::array<Triangle,2> clipped;
    // Add initial triangle
    out.push_back(tri);
    std::size_t nNewTriangles = 1;
    for (int p = 0; p < 4; p++)
    {
        while (nNewTriangles > 0)
        {
            int nTrisToAdd = 0;
            // Take triangle from front of queue
            Triangle r = out.front();
            out.pop_front();
            nNewTriangles--;

            // Clip it against a plane. We only need to test each
            // subsequent plane, against subsequent new triangles
            // as all triangles after a plane clip are guaranteed
            // to lie on the inside of the plane.
            nTrisToAdd = againstPlane(r, planes_p[p], planes_n[p], clipped[0], clipped[1]);

            // Clipping may yield a variable number of triangles, so
            // add these new ones to the back of the queue for subsequent
            // clipping against next planes
            for (int w = 0; w < nTrisToAdd; w++)
            {
                out.push_back(clipped[w]);
            }
        }

        nNewTriangles = out.size();
    }
}

int Clipping::againstPlane(const Triangle& in, const Vec4& plane_p, const Vec4& plane_n, Triangle& out_tri1, Triangle& out_tri2) const noexcept
{
    // Make sure plane normal is indeed normal
    assert(plane_n == plane_n.normalize());

    // Return signed shortest distance from point to plane, plane normal must be normalised
    const float dpnp = plane_n.dotProd(plane_p);
    auto dist = [&](const Vec4& p)
    {
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - dpnp);
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    std::array<const Vertex*, 3> inside_points = {};
    int nInsidePointCount = 0;
    std::array<const Vertex*, 3> outside_points = {};
    int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    if (dist(in.a.v) >= 0)
        inside_points[nInsidePointCount++] = &in.a;
    else
        outside_points[nOutsidePointCount++] = &in.a;
    if (dist(in.b.v) >= 0)
        inside_points[nInsidePointCount++] = &in.b;
    else
        outside_points[nOutsidePointCount++] = &in.b;
    if (dist(in.c.v) >= 0)
        inside_points[nInsidePointCount++] = &in.c;
    else
        outside_points[nOutsidePointCount++] = &in.c;

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist
        return 0;
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in;

        return 1;
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        float tb = 0.0f;
        float tc = 0.0f;

        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.faceNormal_ = in.faceNormal_;

        // The inside point is valid, so keep that...
        out_tri1.a = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.b.v = plane_p.intersectPlane(plane_n, inside_points[0]->v, outside_points[0]->v, tb);
        out_tri1.c.v = plane_p.intersectPlane(plane_n, inside_points[0]->v, outside_points[1]->v, tc);

        out_tri1.b.col = Color::lerpRGB(inside_points[0]->col, outside_points[0]->col, tb);
        out_tri1.c.col = Color::lerpRGB(inside_points[0]->col, outside_points[1]->col, tc);

        out_tri1.b.normal = Clipping::lerp(inside_points[0]->normal, outside_points[0]->normal, tb);
        out_tri1.c.normal = Clipping::lerp(inside_points[0]->normal, outside_points[1]->normal, tc);

        return 1;
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        float tc = 0.0f;

        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.faceNormal_ = out_tri2.faceNormal_ = in.faceNormal_;
        
        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.a = *inside_points[0];
        out_tri1.b = *inside_points[1];
        
        out_tri1.c.v = plane_p.intersectPlane(plane_n, inside_points[0]->v, outside_points[0]->v, tc);

        out_tri1.c.col = Color::lerpRGB(inside_points[0]->col, outside_points[0]->col, tc);
        out_tri1.c.normal = Clipping::lerp(inside_points[0]->normal, outside_points[0]->normal, tc);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.a = *inside_points[1];
        out_tri2.b = out_tri1.c;
        out_tri2.c.v = plane_p.intersectPlane(plane_n, inside_points[1]->v, outside_points[0]->v, tc);

        out_tri2.c.col = Color::lerpRGB(inside_points[1]->col, outside_points[0]->col, tc);
        out_tri2.c.normal = Clipping::lerp(inside_points[1]->normal, outside_points[0]->normal, tc);

        return 2;
    }

    return -1;
}
