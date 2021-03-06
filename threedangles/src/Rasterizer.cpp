#include <Rasterizer.hpp>
#include <algorithm>
#include <cassert>
#include <Tex3.hpp>


Rasterizer::Rasterizer(const std::shared_ptr<Screen> screen) : _screen(screen)
{
}

inline void Rasterizer::draw_hline(int x1, int x2, const int y) const noexcept
{
    // @todo: for the depth buffer the z value need to be interpolated?
    //       or would be enough the z at the line?
    // if (x1,y,z1), (x2,y,z2) z1 can be different z2
    // it needs a linear interpolation, i think
    // std::lerp(z1,z2, p) p =(x-x1)/(x2-x1) x = [x1,x2[?
    // @todo: do first color interpolation
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
        // @todo: color lerp can be linearized
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

void Rasterizer::drawLine(int x1, int y1, const int x2, const int y2, const Color& c) const noexcept
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

        // @todo: the color step could be linearized
        t += tstep;
        c = Color::lerpRGB(c1, c2, t);
    }
}

void Rasterizer::drawTriangle(const Triangle& triangle) const noexcept
{
    int x1 = static_cast<int>(std::round(triangle.a.v.x));
    int y1 = static_cast<int>(std::round(triangle.a.v.y));
    int x2 = static_cast<int>(std::round(triangle.b.v.x));
    int y2 = static_cast<int>(std::round(triangle.b.v.y));
    int x3 = static_cast<int>(std::round(triangle.c.v.x));
    int y3 = static_cast<int>(std::round(triangle.c.v.y));

    drawLine(x1, y1, x2, y2, triangle.a.col, triangle.b.col);
    drawLine(x2, y2, x3, y3, triangle.b.col, triangle.c.col);
    drawLine(x3, y3, x1, y1, triangle.c.col, triangle.a.col);
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
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

void Rasterizer::fillTriangleNoInterpolation(const Triangle& triangle, const int illuminationType, const std::vector<Light>& lights) const noexcept
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
        _screen->setDrawColor(c);
    }
    //else if (illuminationOn == 2) {
    //}

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

void Rasterizer::fillTriangle3(const Triangle& triangle, const int illuminationType, const std::vector<std::shared_ptr<Light>>& lights) const noexcept
{
    // Pineda algorithm, traversal bounding box, not incremental edge function
    // Not efficient implementation.

    // It is doing already the back-face culling if drawing only when area is positive
    // this can be useful to avoid some computation.. but at the same time
    // the faceback culling is useful to reduce the number of triangle to
    // process

    // NOTE ON BARYCENTRIC COORDINATES:
    //           (y2-y3)(x-x3)+(x3-x2)(y-y3)
    // e1 = ----------------------------------------
    //       (y2 - y3)(x1 - x3) + (x3 - x2)(y1 - y3)
    //
    //       (y3 - y1)(x - x3) + (x1 - x3)(y - y3)
    // e2 = ---------------------------------------
    //      (y2 - y3)(x1 - x3) + (x3 - x2)(y1 - y3)
    //
    // e3 = 1 - e2 - e2
    // ####################################################################
    // Edge function
    // E(x,y) = (x-X)*dY - (y-Y)*dX ==> V1 = (X,Y), V2(X+dY, Y+dY), P(x,y)
    //        = (x-v1.x)*(v2.y-v1.y) - (y-v1.y)*(v2.y-v1.y)
    // @todo: make it inline
    const auto edge = [](const int x1, const int y1, const int x2, const int y2, const int x, const int y) noexcept {
        return (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);
    };

    // V1
    const int x1 = static_cast<int>(std::round(triangle.a.v.x));
    const int y1 = static_cast<int>(std::round(triangle.a.v.y));
    // V2
    const int x2 = static_cast<int>(std::round(triangle.b.v.x));
    const int y2 = static_cast<int>(std::round(triangle.b.v.y));
    // V3
    const int x3 = static_cast<int>(std::round(triangle.c.v.x));
    const int y3 = static_cast<int>(std::round(triangle.c.v.y));

    // @todo: move to Triangle and skip to reach here? 
    //       what about clipping before rasterization process?
    // 
    // when area is negative is a back face, "hidden face" removed already
    // by the back-face culling. Using != 0 so i can see the back of the triangle if wanted.
    // the area == 0 is no triangle
    const int area = edge(x1, y1, x2, y2, x3, y3);
    if (area == 0)
        return;

    // these variables could be not used.
    const float z1 = triangle.a.v.z;
    const float z2 = triangle.b.v.z;
    const float z3 = triangle.c.v.z;
    const float w1 = triangle.a.v.w;
    const float w2 = triangle.b.v.w;
    const float w3 = triangle.c.v.w;

    Color c1;
    Color c2;
    Color c3;
    Color c;

    float c1r, c1g, c1b, c2r, c2g, c2b, c3r, c3g, c3b;
    c1r = c1g = c1b = c2r = c2g = c2b = c3r = c3g = c3b = 1.0f;
    // lights
    const uint8_t lightCounts = static_cast<uint8_t>(lights.size());
    if (illuminationType == 0)
    {
        c1 = triangle.a.col;
        c2 = triangle.b.col;
        c3 = triangle.c.col;
    }
    else if (illuminationType == 1)
    {
        // Illumination (flat shading)
        // blending lights (average)
        int r = 0; int g = 0; int b = 0; int a = 0;
        for (const auto& light : lights)
        {
            Color col = light->flatShading(triangle.faceNormal_);
            r += col.r; g += col.g; b += col.b; a += col.a;
        }

        c.g = g / lightCounts;
        c.b = b / lightCounts;
        c.r = r / lightCounts;
        c.a = a / lightCounts;
    }
    else if (illuminationType == 2)
    {
        // Gouraud
        c1 = lights[0]->flatShading(triangle.a.normal);
        c2 = lights[0]->flatShading(triangle.b.normal);
        c3 = lights[0]->flatShading(triangle.c.normal);
    }

    Tex3 ta;
    Tex3 tb;
    Tex3 tc;
    Color ct;

    if (perspectiveCorrection)
    {
        c1r = w1 * c1.r; c1g = w1 * c1.g; c1b = w1 * c1.b;
        c2r = w2 * c2.r; c2g = w2 * c2.g; c2b = w2 * c2.b;
        c3r = w3 * c3.r; c3g = w3 * c3.g; c3b = w3 * c3.b;

        ta = triangle.a.texture * w1;
        tb = triangle.b.texture * w2;
        tc = triangle.c.texture * w3;
    }
    else
    {
        ta = triangle.a.texture;
        tb = triangle.b.texture;
        tc = triangle.c.texture;
    }

    // textures coord
    const float u1 = ta.u;
    const float v1 = ta.v;
    const float u2 = tb.u;
    const float v2 = tb.v;
    const float u3 = tc.u;
    const float v3 = tc.v;

    const float tw1 = ta.w;
    const float tw2 = tb.w;
    const float tw3 = tc.w;

    const bool showTex = triangle.texImg != nullptr && triangle.showTexture;

    // bounding box (no clipping)
    const int ymin = std::min(y1, std::min(y2, y3));
    const int ymax = std::max(y1, std::max(y2, y3));
    const int xmin = std::min(x1, std::min(x2, x3));
    const int xmax = std::max(x1, std::max(x2, x3));
    const int sa = area > 0 ? +1 : -1;

    for (int y = ymin; y <= ymax; y++)
    {
        const int yw = y * _screen->width;
        for (int x = xmin; x <= xmax; x++)
        {
            //const int e3 = edge(x1, y1, x2, y2, x, y); // c3
            const int e1 = edge(x2, y2, x3, y3, x, y); // c1
            const int e2 = edge(x3, y3, x1, y1, x, y); // c2
            const int e3 = area - e2 - e1;

            if (e1 * sa < 0 || e2 * sa < 0 || e3 * sa < 0)
                continue;

            // inside the triangle
            const float z = (z1 * e1 + z2 * e2 + z3 * e3) / static_cast<float>(area);
            if (_screen->_depthBuffer[yw + x] > z && depthBuffer)
                continue;

            _screen->_depthBuffer[yw + x] = z;

            // TODO store the results into fragments objects
            //      and process them at the end.. 
            //
            // TODO detach into fragments to be rendered at the end
            // it could also help parallelize it, while processing fragment (next pixel)
            // another thread is writing on the buffer for eg. etc...

            // Texture Fragment
            if (showTex)
            {
                float u;
                float v;
                if (perspectiveCorrection)
                {
                    const float w = 1.0f / (e1 * tw1 + e2 * tw2 + e3 * tw3);
                    u = w * (e1 * u1 + e2 * u2 + e3 * u3);
                    v = w * (e1 * v1 + e2 * v2 + e3 * v3);
                }
                else
                {
                    u = (u1 * e1 + u2 * e2 + u3 * e3) / static_cast<float>(area);
                    v = (v1 * e1 + v2 * e2 + v3 * e3) / static_cast<float>(area);
                }

                triangle.texImg->getPixel(u, v, ct);
            }

            // Lights Fragment
            if (illuminationType == 0)
            {
                if (perspectiveCorrection)
                {
                    const float w = 1.0f / (e1 * w1 + e2 * w2 + e3 * w3);
                    c.r = static_cast<uint8_t>(std::round(w * (e1 * c1r + e2 * c2r + e3 * c3r)));
                    c.g = static_cast<uint8_t>(std::round(w * (e1 * c1g + e2 * c2g + e3 * c3g)));
                    c.b = static_cast<uint8_t>(std::round(w * (e1 * c1b + e2 * c2b + e3 * c3b)));
                }
                else
                {
                    c.r = static_cast<uint8_t>((e1 * c1.r + e2 * c2.r + e3 * c3.r) / area);
                    c.g = static_cast<uint8_t>((e1 * c1.g + e2 * c2.g + e3 * c3.g) / area);
                    c.b = static_cast<uint8_t>((e1 * c1.b + e2 * c2.b + e3 * c3.b) / area);
                }
            }
            else if (illuminationType == 1) {
                // using the precomputed Color C, doing nothing
            }
            else if (illuminationType == 2)
            {
                if (perspectiveCorrection)
                {
                    const float w = 1.0f / (e1 * w1 + e2 * w2 + e3 * w3);
                    c.r = static_cast<uint8_t>(std::round(w * (e1 * c1r + e2 * c2r + e3 * c3r)));
                    c.g = static_cast<uint8_t>(std::round(w * (e1 * c1g + e2 * c2g + e3 * c3g)));
                    c.b = static_cast<uint8_t>(std::round(w * (e1 * c1b + e2 * c2b + e3 * c3b)));
                }
                else
                {
                    c.r = (e1 * c1.r + e2 * c2.r + e3 * c3.r) / area;
                    c.g = (e1 * c1.g + e2 * c2.g + e3 * c3.g) / area;
                    c.b = (e1 * c1.b + e2 * c2.b + e3 * c3.b) / area;
                }
            }

            // Blending Texture & Light fragment
            if (showTex)
                c = Color::lerpRGBA(ct, c, 0.5);

            _screen->drawPixel(x, y, c);
        }
    }
}
