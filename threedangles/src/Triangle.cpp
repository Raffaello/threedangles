#include <Triangle.hpp>


Triangle::Triangle(const Vec3d& a_, const Vec3d& b_, const Vec3d& c_) : a(a_), b(b_), c(c_)
{
}

Triangle Triangle::operator+(const Vec3d& v)
{
    Triangle t(*this);

    t.a = a + v;
    t.b = b + v;
    t.c = c + v;

    return t;
}

Triangle Triangle::normByW()
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

void Triangle::setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha)
{
    col = { r,g,blue,alpha };
}

void Triangle::setColor(const Triangle& that)
{
    col = that.col;
}

void Triangle::getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
{
    r = col.r; g = col.g; b = col.b; a = col.a;
}

color_t Triangle::getColor() const noexcept
{
    return col;
}

Vec3d Triangle::faceNormal() const noexcept
{
    Vec3d line1(b), line2(c);

    line1 = line1 - a;
    line2 = line2 - a;

    return line1.crossProd(line2).normalize();
}
