#include <Triangle.hpp>
#include <cassert>


Triangle::Triangle(const Vec4& a_, const Vec4& b_, const Vec4& c_) : a(a_), b(b_), c(c_)
{
}

Triangle Triangle::operator+(const Vec4& v) const noexcept
{
    Triangle t(*this);

    t.a = a + v;
    t.b = b + v;
    t.c = c + v;

    return t;
}

Triangle Triangle::normByW() const noexcept
{
    Triangle t(*this);

    t.a = a.normByW();
    t.b = b.normByW();
    t.c = c.normByW();

    return t;
}

void Triangle::setColor(const color_t& c) noexcept
{
    col = c;
}

void Triangle::setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha) noexcept
{
    col = { r,g,blue,alpha };
}

void Triangle::setColor(const Triangle& that) noexcept
{
    col = that.col;
}

void Triangle::getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const noexcept
{
    r = col.r; g = col.g; b = col.b; a = col.a;
}

color_t Triangle::getColor() const noexcept
{
    return col;
}

Vec4 Triangle::faceNormal() const noexcept
{
    Vec4 line1(b), line2(c);

    line1 = line1-a; //b-a
    line2 = line2 -a; //c-a

    return line1.crossProd(line2).normalize();
}

int Triangle::clipAgainstPlane(const Vec4& plane_p, const Vec4& plane_n, Triangle& out_tri1, Triangle& out_tri2) const noexcept
{
    // Make sure plane normal is indeed normal
    //plane_n = plane_n.normalize();
    assert(plane_n == plane_n.normalize());

    // Return signed shortest distance from point to plane, plane normal must be normalised
    const float dpnp = plane_n.dotProd(plane_p);
    auto dist = [&](const Vec4& p)
    {
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - dpnp);
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    const Vec4* inside_points[3]{};  int nInsidePointCount = 0;
    const Vec4* outside_points[3]{}; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(this->a);
    float d1 = dist(this->b);
    float d2 = dist(this->c);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &this->a; }
    else { outside_points[nOutsidePointCount++] = &this->a; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &this->b; }
    else { outside_points[nOutsidePointCount++] = &this->b; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &this->c; }
    else { outside_points[nOutsidePointCount++] = &this->c; }

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
        out_tri1 = *this;
        out_tri1.setColor(*this);

        return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.setColor(*this);
        //out_tri1.setColor(64, 0, 0, 255);

        // The inside point is valid, so keep that...
        out_tri1.a = *inside_points[0];

        // but the two new points are at the locations where the 
        // original sides of the triangle (lines) intersect with the plane
        //out_tri1.b = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.b = plane_p.intersectPlane(plane_n, *inside_points[0], *outside_points[0]);
        //out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);
        out_tri1.c = plane_p.intersectPlane(plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.setColor(*this);
        //out_tri1.setColor(0, 64, 0, 255);
        out_tri2.setColor(*this);
        //out_tri2.setColor(0, 0, 64, 255);
        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.a = *inside_points[0];
        out_tri1.b = *inside_points[1];
        //out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.c = plane_p.intersectPlane(plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the 
        // triangle and the plane, and the newly created point above
        out_tri2.a = *inside_points[1];
        out_tri2.b = out_tri1.c;
        //out_tri2.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);
        out_tri2.c = plane_p.intersectPlane(plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }

    return -1;
}
