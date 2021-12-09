#include <Engine.hpp>

Mat4 Engine::matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up)
{
    /// @see https://www.3dgep.com/understanding-the-view-matrix/
    Vec3d forward = (target - pos).normalize();

    Vec3d t = (forward * up.dotProd(forward));
    Vec3d newUp = (up - t).normalize();
    Vec3d newRight = newUp.crossProd(forward);

    //Dimensioning & Translation Matrix
    Mat4 m;
    m.m[0][0] = newRight.x; m.m[0][1] = newUp.x; m.m[0][2] = forward.x; m.m[0][3] = pos.x;
    m.m[1][0] = newRight.y; m.m[1][1] = newUp.y; m.m[1][2] = forward.y; m.m[1][3] = pos.y;
    m.m[2][0] = newRight.z; m.m[2][1] = newUp.z; m.m[2][2] = forward.z; m.m[2][3] = pos.z;
    m.m[3][0] = 0.0f;       m.m[3][1] = 0.0f;    m.m[3][2] = 0.0f;      m.m[3][3] = 1.0f;

    return m;
}

Mat4 Engine::matrix_InversePointAt(Mat4& m)
{
    Mat4 matrix;

    matrix.m[0][0] = m.m[0][0];
    matrix.m[1][0] = m.m[0][1];
    matrix.m[2][0] = m.m[0][2];
    matrix.m[3][0] = 0.0f;
    matrix.m[0][1] = m.m[1][0];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[3][1] = 0.0f;
    matrix.m[0][2] = m.m[2][0];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[3][2] = 0.0f;

    matrix.m[0][3] = -(m.m[0][3] * matrix.m[0][0] + m.m[1][3] * matrix.m[0][1] + m.m[2][3] * matrix.m[0][2]);
    matrix.m[1][3] = -(m.m[0][3] * matrix.m[1][0] + m.m[1][3] * matrix.m[1][1] + m.m[2][3] * matrix.m[1][2]);
    matrix.m[2][3] = -(m.m[0][3] * matrix.m[2][0] + m.m[1][3] * matrix.m[2][1] + m.m[2][3] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;

    return matrix;
}




Vec3d Engine::Vector_IntersectPlane(Vec3d& plane_p, Vec3d& plane_n, Vec3d& lineStart, Vec3d& lineEnd)
{
    plane_n = plane_n.normalize();
    float plane_d = -plane_n.dotProd(plane_p);
    float ad = lineStart.dotProd(plane_n);
    float bd = lineEnd.dotProd(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vec3d lineStartToEnd = lineEnd - lineStart; 
    Vec3d lineToIntersect = lineStartToEnd * t;
    return lineStart + lineToIntersect;
}

int Engine::Triangle_ClipAgainstPlane(Vec3d plane_p, Vec3d plane_n, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n = plane_n.normalize();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vec3d& p)
    {
        Vec3d n = p.normalize();
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dotProd(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vec3d* inside_points[3]{};  int nInsidePointCount = 0;
    Vec3d* outside_points[3]{}; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.a);
    float d1 = dist(in_tri.b);
    float d2 = dist(in_tri.c);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.a; }
    else { outside_points[nOutsidePointCount++] = &in_tri.a; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.b; }
    else { outside_points[nOutsidePointCount++] = &in_tri.b; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.c; }
    else { outside_points[nOutsidePointCount++] = &in_tri.c; }

    // Now classify triangle points, and break the input triangle into 
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.setColor(in_tri);
        //out_tri1.setColor(64, 0, 0, 255);

        // The inside point is valid, so keep that...
        out_tri1.a = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.b = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.setColor(in_tri);
        //out_tri1.setColor(0, 64, 0, 255);
        out_tri2.setColor(in_tri);
        //out_tri2.setColor(0, 0, 64, 255);
        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.a = *inside_points[0];
        out_tri1.b = *inside_points[1];
        out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.a = *inside_points[1];
        out_tri2.b = out_tri1.c;
        out_tri2.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }

    return -1;
}
