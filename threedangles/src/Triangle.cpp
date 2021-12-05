#include <Triangle.hpp>
#include <cmath>
#include <algorithm>

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

void Triangle::setColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
    _r = r;
    _g = g;
    _b = b;
    _a = a;
}

void Triangle::setColor(const Triangle& that)
{
    _r = that._r;
    _g = that._g;
    _b = that._b;
    _a = that._a;
}



void Triangle::draw(SDL_Renderer* renderer)
{
    int x1 = static_cast<int>(std::round(a.x));
    int y1 = static_cast<int>(std::round(a.y));
    int x2 = static_cast<int>(std::round(b.x));
    int y2 = static_cast<int>(std::round(b.y));
    int x3 = static_cast<int>(std::round(c.x));
    int y3 = static_cast<int>(std::round(c.y));

    SDL_SetRenderDrawColor(renderer, _r, _g, _b, _a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

void Triangle::fill(SDL_Renderer* renderer)
{
    int x1 = static_cast<int>(std::round(a.x));
    int y1 = static_cast<int>(std::round(a.y));
    int x2 = static_cast<int>(std::round(b.x));
    int y2 = static_cast<int>(std::round(b.y));
    int x3 = static_cast<int>(std::round(c.x));
    int y3 = static_cast<int>(std::round(c.y));
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, _a);

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;

    int8_t signx1, signx2;

    int dx1, dy1, dx2, dy2;
    int e1, e2;

    // at first sort the three vertices by y-coordinate ascending so v1 is the topmost vertice
    // sortVerticesAscendingByY();
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }
    if (y1 > y3) {
        std::swap(y1, y3);
        std::swap(x1, x3);
    }
    if (y2 > y3) {
        std::swap(y2, y3);
        std::swap(x2, x3);
    }

    t1x = t2x = x1;
    y = y1;

    dy1 = y2 - y1;
    dx1 = x2 - x1;
    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    }
    else {
        signx1 = 1;
    }

    dy2 = y3 - y1;
    dx2 = x3 - x1;
    if (dx2 < 0) {
        dx2 = -dx2;
        signx2 = -1;
    }
    else
        signx2 = 1;

    // swap values
    if (dy1 > dx1) {
        std::swap(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {
        std::swap(dy2, dx2);
        changed2 = true;
    }


    /* here we know that v1.y <= v2.y <= v3.y */
    /* check for trivial case of bottom-flat triangle */

    e2 = dx2 >> 1;
    // Flat top, just process the second half
    if (y1 == y2)
        goto next;
    
    e1 = dx1 >> 1;
    for (int i = 0; i < dx1;)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else {
            minx = t2x;
            maxx = t1x;
        }

        // process first line until y value is about to change
        while (i < dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                    t1xp = signx1;
                else
                    goto next1;
            }
            
            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next2:
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;
        draw_hline(renderer, minx, maxx, y);    // Draw line from min to max points found on the y
        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2)
            break;
    }

next:
    // Second half
    dy1 = y3 - y2;
    dx1 = x3 - x2;
    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;

    t1x = x2;
    if (dy1 > dx1)
    {   // swap values
        std::swap(dy1, dx1);
        changed1 = true;
    }
    else
        changed1 = false;

    e1 = dx1 >> 1;
    for (int i = 0; i <= dx1; i++)
    {
        t1xp = 0; t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    break;
                }//t1x += signx1;
                else
                    goto next3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next4;
            }

            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next4:
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;
        // Draw line from min to max points found on the y
        draw_hline(renderer, minx, maxx, y);
        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            return;
    }
}

void Triangle::draw_hline(SDL_Renderer* renderer, int x1, int x2, const int y)
{
    if (x1 >= x2) std::swap(x1, x2);
    for (; x1 <= x2; x1++) {
        SDL_RenderDrawPoint(renderer, x1, y);
    }
}
