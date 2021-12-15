#include <Clipping.hpp>
#include <cassert>


Clipping::Clipping(const float near, const float far, const int width, const int height) :
    plane_p_near(0.0f, 0.0f, near),
    plane_n_near(0.0f, 0.0f, 1.0f),
    plane_p_far(0.0f, 0.0f, far),
    plane_n_far(0.0f, 0.0f, -1.0f),

    plane_p0(0.0f, 0.0f, 0.0f),
    plane_n0(Vec4(0.0f, 1.0f, 0.0f).normalize()),
    plane_p1(0.0f, static_cast<float>(height) - 1.0f, 0.0f),
    plane_n1(Vec4(0.0f, -1.0f, 0.0f).normalize()),
    plane_p2(plane_p0),
    plane_n2(Vec4(1.0f, 0.0f, 0.0f).normalize()),
    plane_p3(static_cast<float>(width) - 1.0f, 0.0f, 0.0f),
    plane_n3(Vec4(-1.0f, 0.0f, 0.0f).normalize()),

    planes_p { plane_p0, plane_p1, plane_p2, plane_p3 },
    planes_n { plane_n0, plane_n1, plane_n2, plane_n3 }
{
}

void Clipping::clipZ(const Triangle& t, std::vector<Triangle>& out) const noexcept
{
    // Clipping on Znear plane (triViewd -> clipped[2])
    int nClippedTriangles = 0;
    std::array<Triangle, 2> clipped;
    nClippedTriangles = t.clipAgainstPlane(plane_p_near, plane_n_near, clipped[0], clipped[1]);
    // clipping on Zfar plane (clipped[2] -> vector<clippped>)
    for (int i = 0; i < nClippedTriangles; i++)
    {
        std::array<Triangle,2> clippedFar;
        int nClippedTrianglesFar = againstPlane(clipped[i], plane_p_far, plane_n_far, clippedFar[0], clippedFar[1]);
        for (int n = 0; n < nClippedTrianglesFar; n++)
            out.push_back(clippedFar[n]);
    }
}

void Clipping::clipScreen(const raster_t& tri, std::list<raster_t>& out) const noexcept
{
    // CLIPPING on Screen size
    std::array<Triangle,2> clipped;
    // Add initial triangle
    out.push_back(tri);
    size_t nNewTriangles = 1;
    for (int p = 0; p < 4; p++)
    {
        while (nNewTriangles > 0)
        {
            int nTrisToAdd = 0;
            // Take triangle from front of queue
            raster_t r = out.front();
            out.pop_front();
            nNewTriangles--;

            // Clip it against a plane. We only need to test each 
            // subsequent plane, against subsequent new triangles
            // as all triangles after a plane clip are guaranteed
            // to lie on the inside of the plane. I like how this
            // comment is almost completely and utterly justified
            nTrisToAdd = againstPlane(r.t, planes_p[p], planes_n[p], clipped[0], clipped[1]);

            // Clipping may yield a variable number of triangles, so
            // add these new ones to the back of the queue for subsequent
            // clipping against next planes
            for (int w = 0; w < nTrisToAdd; w++) {
                raster_t rr;
                rr.t = clipped[w];
                rr.faceNormal = r.faceNormal;
                out.push_back(rr);
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
    std::array<const Vec4*, 3> inside_points = {};
    int nInsidePointCount = 0;
    std::array<const Vec4*, 3> outside_points = {};
    int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    if (dist(in.a) >= 0)
        inside_points[nInsidePointCount++] = &in.a;
    else
        outside_points[nOutsidePointCount++] = &in.a;
    if (dist(in.b) >= 0)
        inside_points[nInsidePointCount++] = &in.b;
    else
        outside_points[nOutsidePointCount++] = &in.b;
    if (dist(in.c) >= 0)
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
        out_tri1.setColor(in);

        return 1;
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.setColor(in);
        //out_tri1.setColor(64, 0, 0, 255);

        // The inside point is valid, so keep that...
        out_tri1.a = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.b = plane_p.intersectPlane(plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.c = plane_p.intersectPlane(plane_n, *inside_points[0], *outside_points[1]);

        return 1;
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.setColor(in);
        //out_tri1.setColor(0, 64, 0, 255);
        out_tri2.setColor(in);
        //out_tri2.setColor(0, 0, 64, 255);
        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.a = *inside_points[0];
        out_tri1.b = *inside_points[1];
        out_tri1.c = plane_p.intersectPlane(plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.a = *inside_points[1];
        out_tri2.b = out_tri1.c;
        out_tri2.c = plane_p.intersectPlane(plane_n, *inside_points[1], *outside_points[0]);

        return 2;
    }

    return -1;
}
