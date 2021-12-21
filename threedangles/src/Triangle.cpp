#include <Triangle.hpp>
#include <cassert>


Triangle::Triangle(const Vertex& a_, const Vertex& b_, const Vertex& c_) : a(a_), b(b_), c(c_)//, faceNormal_(faceNormal())
{
}

Triangle Triangle::operator*(const Mat4& m) const noexcept
{
    Triangle t(*this);

    t.a.v = m * a.v;
    t.b.v = m * b.v;
    t.c.v = m * c.v;

    return t;
}

Triangle& Triangle::operator*=(const Mat4& m) noexcept
{
    a.v = m * a.v;
    b.v = m * b.v;
    c.v = m * c.v;
    
    return *this;
}

Triangle Triangle::normByW() const noexcept
{
    Triangle t(*this);

    t.a.v = a.v.normByW();
    t.b.v = b.v.normByW();
    t.c.v = c.v.normByW();

    return t;
}

void Triangle::setColor(const Triangle& that) noexcept
{
    a.col = that.a.col;
    b.col = that.b.col;
    c.col = that.c.col;
}

Color Triangle::getColor() const noexcept
{
    return Color(
        (a.col.r + b.col.r + c.col.r) / 3,
        (a.col.g + b.col.g + c.col.g) / 3,
        (a.col.b + b.col.b + c.col.b) / 3,
        (a.col.a + b.col.a + c.col.a) / 3
    );
}

Vec4 Triangle::faceNormal() const noexcept
{
    Vec4 line1(b.v);
    Vec4 line2(c.v);

    line1 = line1 - a.v; // b - a
    line2 = line2 - a.v; // c - a

    return line1.crossProd(line2).normalize();
}
