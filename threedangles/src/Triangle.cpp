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

Triangle Triangle::operator*(const Mat4& m) const noexcept
{
    Triangle t(*this);

    t.a = m * a;
    t.b = m * b;
    t.c = m * c;

    return t;
}

Triangle& Triangle::operator*=(const Mat4& m) noexcept
{
    //a *= m;
    //b *= m;
    //c *= m;
    a = m * a;
    b = m * b;
    c = m * c;
    
    return *this;
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
