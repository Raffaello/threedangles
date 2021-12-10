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

void Triangle::setColor(const uint8_t r, const uint8_t g, const uint8_t blue, const uint8_t alpha)
{
    _r = r;
    _g = g;
    _b = blue;
    _a = alpha;
}

void Triangle::setColor(const Triangle& that)
{
    _r = that._r;
    _g = that._g;
    _b = that._b;
    _a = that._a;
}

void Triangle::getColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
{
    r = _r; g = _g; b = _b; a = _a;
}
