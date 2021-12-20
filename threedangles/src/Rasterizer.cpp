#include <Rasterizer.hpp>
#include <algorithm>


Rasterizer::Rasterizer(const std::shared_ptr<Screen> screen) : _screen(screen)
{
}

inline void Rasterizer::draw_hline(int x1, int x2, const int y) const noexcept
{
    // TODO: for the depth buffer the z value need to be interpolated?
    //       or would be enough the z at the line?
    // if (x1,y,z1), (x2,y,z2) z1 can be different z2
    // it needs a linear interpolation, i think
    // std::lerp(z1,z2, p) p =(x-x1)/(x2-x1) x = [x1,x2[?
    // TODO: do first color interpolation
    if (x1 > x2) std::swap(x1, x2);
    for (; x1 <= x2; x1++) {
        // this could be done more efficientily
        // addressing the hline directly on the buffer.
        // instead of recomputing each time the y coord
        _screen->drawPixel(x1, y);
    }
}

inline void Rasterizer::draw_hline(int x1, int x2, const int y, const Color& c) const noexcept
{
    _screen->setDrawColor(c);
    draw_hline(x1, x2, y);
}

void Rasterizer::draw_hline(int x1, int x2, const int y, Color c1, Color c2) const noexcept
{
    if (x1 == x2)
    {
        // should be c1 50% and c2 50% ?
        _screen->drawPixel(x1, y, Color::lerpRGB(c1, c2, 0.5));
        return;
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(c1, c2);
    }

    const int dx = x2 - x1;
    const float tstep = 1.0f / sqrt(dx * dx);
    float t = 0.0f;
    Color c = c1;
    _screen->drawPixel(x1, y, c);
    for (int x = x1 + 1; x < x2; x++)
    {
        // TODO: color lerp can be linearized
        t += tstep;
        c = Color::lerpRGB(c1, c2, t);
        _screen->drawPixel(x, y, c);
    }
    _screen->drawPixel(x2, y, c2);
}

inline void Rasterizer::drawLine(int x1, int y1, const int x2, const int y2) const noexcept
{
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;  // error value e_xy

    while (true)
    {
        _screen->drawPixel(x1, y1);
        if (x1 == x2 && y1 == y2)
            break;

        int e2 = err << 1;

        if (e2 >= dy)
        {
            // e_xy+e_x > 0
            err += dy;
            x1 += sx;
        }

        if (e2 <= dx)
        {
            // e_xy+e_y < 0
            err += dx;
            y1 += sy;
        }
    }
}

inline void Rasterizer::drawLine(int x1, int y1, const int x2, const int y2, const Color& c) const noexcept
{
    _screen->setDrawColor(c);
    drawLine(x1, y1, x2, y2);
}

void Rasterizer::drawLine(const int x1, const int y1, const int x2, const int y2, const Color& c1, const Color c2) const noexcept
{
    const int dx = abs(x2 - x1);
    const int sx = x1 < x2 ? 1 : -1;
    const int dy = -abs(y2 - y1);
    const int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;  // error value e_xy

    const float tstep = 1.0f / sqrt(dx * dx + dy * dy);
    float t = 0.0f;
    int x = x1;
    int y = y1;
    Color c = c1;

    while (true)
    {
        _screen->drawPixel(x, y, c);
        if (x == x2 && y == y2)
            break;

        const int e2 = err << 1;

        if (e2 >= dy)
        {
            // e_xy+e_x > 0
            err += dy;
            x += sx;
        }

        if (e2 <= dx)
        {
            // e_xy+e_y < 0
            err += dx;
            y += sy;
        }

        // TODO: the color step could be linearized
        t += tstep;
        c = Color::lerpRGB(c1, c2, t);
    }
}

void Rasterizer::drawTriangle(const Triangle& triangle) const noexcept
{
    Color c = triangle.getColor();
    drawTriangle(triangle, c);
}

void Rasterizer::drawTriangle(const Triangle& triangle, const Color& c) const noexcept
{
    int x1 = static_cast<int>(std::round(triangle.a.v.x));
    int y1 = static_cast<int>(std::round(triangle.a.v.y));
    int x2 = static_cast<int>(std::round(triangle.b.v.x));
    int y2 = static_cast<int>(std::round(triangle.b.v.y));
    int x3 = static_cast<int>(std::round(triangle.c.v.x));
    int y3 = static_cast<int>(std::round(triangle.c.v.y));

    _screen->setDrawColor(c);
    drawLine(x1, y1, x2, y2, triangle.a.col, triangle.b.col);
    drawLine(x2, y2, x3, y3, triangle.b.col, triangle.c.col);
    drawLine(x3, y3, x1, y1, triangle.c.col, triangle.a.col);
}

inline void Rasterizer::fillTriangle(const Triangle& triangle, const int illuminationType, const std::vector<Light>& lights) const noexcept
{
    int x1 = static_cast<int>(std::round(triangle.a.v.x));
    int y1 = static_cast<int>(std::round(triangle.a.v.y));
    int x2 = static_cast<int>(std::round(triangle.b.v.x));
    int y2 = static_cast<int>(std::round(triangle.b.v.y));
    int x3 = static_cast<int>(std::round(triangle.c.v.x));
    int y3 = static_cast<int>(std::round(triangle.c.v.y));

    const uint8_t lightCounts = static_cast<uint8_t>(lights.size());
    // Illumination (flat shading)
    Color c;
    if (illuminationType == 0) {
        c.r = 255; c.g = 255; c.b = 255; c.a = 255;
        _screen->setDrawColor(c);
    }
    else if (illuminationType == 1) {
        // blending lights (average)
        int r = 0; int g = 0; int b = 0; int a = 0;
        for (const auto& light : lights)
        {
            Color col = light.flatShading(triangle.faceNormal_);
            r += col.r; g += col.g; b += col.b; a += col.a;
        }

        c.g = g / lightCounts;
        c.b = b / lightCounts;
        c.r = r / lightCounts;
        c.a = a / lightCounts;

    }
    //else if (illuminationOn == 2) {
    //}
    _screen->setDrawColor(c);

    // at first sort the three vertices by y-coordinate ascending so v1 is the topmost vertice
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

    // -----
    int t1x = x1;
    int t2x = x1;
    int y = y1;
    bool changed1 = false;
    bool changed2 = false;
    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    int8_t signx1;
    int8_t signx2;
    int e1;
    int e2;
    int minx;
    int maxx;
    int t1xp;
    int t2xp = 0;
    /*Color c1 = triangle.a.col;
    Color c2 = triangle.b.col;
    Color c3 = triangle.c.col;
    Color tc1;
    Color tc2;
    const float t1step = 1.0f / sqrt(dx1 * dx1 + dy1 * dy1);
    float t1 = 0.0f;
    const float t2step = 1.0f / sqrt(dx2 * dx2 + dy2 * dy2);
    float t2 = 0.0f;*/
    // interpolate the color of the line1 and line2 (tc1, tc2)
   /* t1 += t1step;
    t2 += t2step;
    Color tc1 = Color::lerpRGB(c1, c2, t1);
    Color tc2 = Color::lerpRGB(c1, c3, t2);
    assert(ty1 == ty2);
    draw_hline(tx1, tx2, ty1, tc1, tc2);*/

    if (dx1 < 0) {
        dx1 = -dx1;
        signx1 = -1;
    }
    else {
        signx1 = 1;
    }

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

    // here we know that v1.y <= v2.y <= v3.y
    // check for trivial case of bottom-flat triangle

    e2 = dx2 >> 1;
    // Flat top, just process the second half
    if (y1 == y2)
        goto NEXT;

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
                    goto NEXT1;
            }

            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
    NEXT1:
        // process second line until y value is about to change
        while (true)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto NEXT2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    NEXT2:
        //fill_update_minmax();
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;
        // Draw line from min to max points found on the y
        draw_hline(minx, maxx, y);
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
NEXT:
    // Second half
    //fill_bottom_flat(renderer);
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
                //if (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    //break;
                }//t1x += signx1;
                else
                    goto NEXT3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
    NEXT3:
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
                    goto NEXT4;
            }

            if (changed2)
                break;
            else
                t2x += signx2;
        }
    NEXT4:
        //fill_update_minmax();
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;
        // Draw line from min to max points found on the y
        draw_hline(minx, maxx, y);
        // Now increase y
        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            break;
    }
}

void Rasterizer::fillTriangle3(const Triangle& triangle) const noexcept
{
    // Pineda algorithm, traversal bounding box, not incremental edge function
    // Not efficient implementation.

    // It is doing already the back-face culling if drawing only when area is positive
    // this can be useful to avoid some computation.. but at the same time
    // the faceback culling is useful to reduce the number of triangle to
    // process

    // Edge function
    // E(x,y) = (x-X)*dY - (y-Y)*dX ==> V1 = (X,Y), V2(X+dY, Y+dY), P(x,y)
    //        = (x-v1.x)*(v2.y-v1.y) - (y-v1.y)*(v2.y-v1.y)
    // TODO: make it inline
    const auto edge = [](const int x1, const int y1, const int x2, const int y2, const int x, const int y) noexcept {
        return (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);
    };

    // V1
    int x1 = static_cast<int>(std::round(triangle.a.v.x));
    int y1 = static_cast<int>(std::round(triangle.a.v.y));
    float z1 = triangle.a.v.z;
    Color c1 = triangle.a.col;
    // V2
    int x2 = static_cast<int>(std::round(triangle.b.v.x));
    int y2 = static_cast<int>(std::round(triangle.b.v.y));
    float z2 = triangle.b.v.z;
    Color c2 = triangle.b.col;
    // V3
    int x3 = static_cast<int>(std::round(triangle.c.v.x));
    int y3 = static_cast<int>(std::round(triangle.c.v.y));
    float z3 = triangle.c.v.z;
    Color c3 = triangle.c.col;

    int area = edge(x1, y1, x2, y2, x3, y3);
    if (area == 0)
        return;

    // bounding box (no clipping)
    int ymin = std::min(y1, std::min(y2, y3));
    int ymax = std::max(y1, std::max(y2, y3));
    int xmin = std::min(x1, std::min(x2, x3));
    int xmax = std::max(x1, std::max(x2, x3));
    int sa = area > 0 ? +1 : -1;

    // when area is negative is a back face, "hidden face" removed already
    // by the back-face culling. Using != 0 so i can see the back of the triangle if wanted.
    // the area == 0 is just a flat triangle, a line, no area and it is like a line color interpolation
    for (int y = ymin; y <= ymax; y++)
    {
        for (int x = xmin; x <= xmax; x++)
        {
            const int e1 = edge(x1, y1, x2, y2, x, y); // c3
            const int e2 = edge(x2, y2, x3, y3, x, y); // c1
            const int e3 = edge(x3, y3, x1, y1, x, y); // c2

            if (e1 * sa >= 0 && e2 * sa >= 0 && e3 * sa >= 0)
            {
                // inside the triangle
                Color c;
                c.r = std::clamp((e1 * c3.r + e2 * c1.r + e3 * c2.r) / area, 0, 255);
                c.g = std::clamp((e1 * c3.g + e2 * c1.g + e3 * c2.g) / area, 0, 255);
                c.b = std::clamp((e1 * c3.b + e2 * c1.b + e3 * c2.b) / area, 0, 255);
                _screen->drawPixel(x, y, c);
            }
        }
    }

}
