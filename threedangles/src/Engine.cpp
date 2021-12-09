#include <Engine.hpp>
#include <cmath>

constexpr float PI = 3.141592653589793f;
constexpr float deg2rad = (PI / 180.0f);

Mat4x4 Engine::matrix_createProjection(const int w, const int h, const float fov, const float zfar, const float znear)
{
    // Perspective Projection Matrix
    // @see https://solarianprogrammer.com/2013/05/22/opengl-101-matrices-projection-view-model/
    Mat4x4 m;
    const float ar = static_cast<float>(w) / static_cast<float>(h);
    /*
    const float q = zfar / (zfar - znear);
    const float fov_rad = 1.0f / std::tan(fov * 0.5f / 180.0f * PI);

    m.m[0][0] = ar * fov_rad;
    m.m[1][1] = fov_rad;
    m.m[2][2] = q;
    m.m[2][3] = 1.0f;
    m.m[3][2] = -(znear * q);
    m.m[3][3] = 0.0f;
    */
    
    const float top = znear * tan(fov * 0.5f * deg2rad );
    const float bottom = -top;
    const float tb = top - bottom;
    //const float right = top * ar;
    const float right = static_cast<float>(w) * tb / (2.0f * static_cast<float>(h));
    //const float right = ar / tan(fov * 0.5f * deg2rad);
    const float left = -right;

    const float rl = right - left;

    const float zfn = zfar - znear;

    // right hand, frustum
    m.m[0][0] = 2.0f * znear / rl;
    m.m[0][2] = (right + left) / rl;
    m.m[1][1] = 2.0f * znear / tb;
    m.m[1][2] = (top + bottom) / tb;
    m.m[2][2] = -(zfar + znear) / zfn;
    m.m[2][3] = -2.0f * zfar * znear / zfn;
    m.m[3][2] = -1.0f;
    
    // left handed
    m.m[0][2] *= -1.0f;
    m.m[1][2] *= -1.0f;
    m.m[2][2] *= -1.0f;
    m.m[3][2] *= -1.0f;

    
    return m;
}

Mat4x4 Engine::matrix_createRotationZ(const float theta)
{
    Mat4x4 matRotZ;

    matRotZ.m[0][0] = std::cos(theta);
    matRotZ.m[0][1] = std::sin(theta);
    matRotZ.m[1][0] = -std::sin(theta);
    matRotZ.m[1][1] = std::cos(theta);
    matRotZ.m[2][2] = 1.0f;
    matRotZ.m[3][3] = 1.0f;

    return matRotZ;
}

Mat4x4 Engine::matrix_createRotationX(const float theta)
{
    Mat4x4 matRotX;

    matRotX.m[0][0] = 1.0f;
    matRotX.m[1][1] = std::cos(theta);
    matRotX.m[1][2] = std::sin(theta);
    matRotX.m[2][1] = -std::sin(theta);
    matRotX.m[2][2] = std::cos(theta);
    matRotX.m[3][3] = 1.0f;

    return matRotX;
}

Mat4x4 Engine::matrix_createRotationY(const float theta)
{
    Mat4x4 matRotY;

    matRotY.m[0][0] = std::cos(theta);
    matRotY.m[0][2] = -std::sin(theta);
    matRotY.m[2][0] = std::sin(theta);
    matRotY.m[1][1] = 1.0f;
    matRotY.m[2][2] = std::cos(theta);
    matRotY.m[3][3] = 1.0f;

    return matRotY;
    
}

Mat4x4 Engine::matrix_createTranslation(const Vec3d& v)
{
    Mat4x4 m;

    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;
    m.m[3][0] = v.x;
    m.m[3][1] = v.y;
    m.m[3][2] = v.z;

    return m;
}

Mat4x4 Engine::matrix_createIdentity()
{
    Mat4x4 m;

    m.m[0][0] = m.m[1][1] = m.m[2][2] = m.m[3][3] = 1.0f;
    
    return m;
}

Mat4x4 Engine::matrix_createScale(const float a, const float b, const float c)
{
    Mat4x4 m;

    m.m[0][0] = a;
    m.m[1][1] = b;
    m.m[2][2] = c;
    m.m[3][3] = 1.0f;

    return m;
}

Mat4x4 Engine::matrix_pointAt(Vec3d& pos, Vec3d& target, Vec3d& up)
{
    Vec3d forward = (target - pos).normalize();

    Vec3d t = (forward * up.dotProd(forward));
    Vec3d newUp = (up - t).normalize();
    Vec3d newRight = newUp.crossProd(forward);

    //Dimensioning & Translation Matrix
    Mat4x4 m;
    m.m[0][0] = newRight.x; m.m[0][1] = newRight.y; m.m[0][2] = newRight.z; m.m[0][3] = 0.0f;
    m.m[1][0] = newUp.x; m.m[1][1] = newUp.y; m.m[1][2] = newUp.z; m.m[1][3] = 0.0f;
    m.m[2][0] = forward.x; m.m[2][1] = forward.y; m.m[2][2] = forward.z; m.m[2][3] = 0.0f;
    m.m[3][0] = pos.x; m.m[3][1] = pos.y; m.m[3][2] = pos.z; m.m[3][3] = 1.0f;

    return m;
}

Mat4x4 Engine::matrix_InversePointAt(Mat4x4& m)
{
    Mat4x4 matrix;

    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;

    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
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
}
