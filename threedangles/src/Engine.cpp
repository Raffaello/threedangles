#include <Engine.hpp>
#include <cmath>
#include <sdl/Screen_SDL.hpp>
#include <fstream>
#include <strstream> // todo remove as it has been deprecated
#include <vector>
#include <exception>
#include <algorithm>
#include <list>
#include <cassert>

Engine::Engine(const std::shared_ptr<Screen> screen) : _screen(screen)
{
    //_depthBuffer = std::make_shared<float[]>(_screen->screenSize);
}

std::shared_ptr<Engine> Engine::createEngineSDL(const std::string& title, const int width, const int height) noexcept
{
    std::shared_ptr<Screen> s = std::make_shared<sdl::Screen_SDL>(title, width, height);

    if (!s->init())
        return nullptr;

    Engine* e = new Engine(s);
    std::shared_ptr<Engine> se;
    se.reset(e);

    return se;
}

void Engine::setMatrixProjection(const Mat4& matProj) noexcept
{
    _matProjection = matProj;
}

void Engine::setMatrixWorld(const Mat4& matWorld) noexcept
{
    this->_matWorld = matWorld;
}

void Engine::setMatrixView(const Mat4& matView) noexcept
{
    this->_matView = matView;
}

void Engine::initPerspectiveProjection(const float fov, const float far, const float near) noexcept
{
    this->fov = fov;
    this->far = far;
    this->near = near;
    _matProjection = Mat4::createScale(_screen->width / 2.0f, _screen->height / 2.0f, 1.0f)
        * Mat4::createTranslation({ 1.0f, 1.0f, 0.0f })
        * Mat4::createProjection(_screen->width, _screen->height, fov, far, near);
    _clipping = std::make_shared<Clipping>(near, far, _screen->width, _screen->height); 
}

void Engine::processFrame(const Cam& cam, const color_t& bg_col) noexcept
{
    _trianglesToRaster.clear();
    // Clear the screen/buffer
    _screen->clear(bg_col);
    //for (int i = 0; i < _screen->screenSize; ++i) _depthBuffer[i] = far;

    for (const auto& mesh : _meshes)
    {
        // matWorld can be copied in the Mesh and concatenated to other Mesh transformation
        // and then compute the "MeshTransformed already" to be ready to be reused
        // unless something changes ?
        mesh.render(_matProjection, _matWorld, _matView, showHiddenVertexes, cam, _clipping, _trianglesToRaster);
    }

    // Z-depth sorting (Painter's Algorithm)
    sortZ();

    // Triangle Rasterization
    raster();

    // Swap buffers
    _screen->flip();
}

bool Engine::addMeshFromOBJFile(const std::string& filename)
{
    std::ifstream file(filename, std::ifstream::in);
    std::string line;
    std::vector<Vec4> vertexes;

    Mesh m;
    if (!file.is_open())
        return false;

    m.name = filename;
    while (std::getline(file, line))
    {
        std::strstream ss;
        std::string type;

        ss << line;
        ss >> type;
        if (type == "#")
        {
            // comment
            continue;
        }
        else if (type == "v")
        {
            // vertex
            Vec4 v;
            ss >> v.x >> v.y >> v.z;
            vertexes.push_back(v);
        }
        else if (type == "f")
        {
            // TODO: store vt and vn values

            // face
            // can be:
            // - int
            // - int/int
            // - int/int/int
            //  int//int
            std::string fs;
            int v_[3];
            //int vt_[3];
            //int vn_[3];
            for (int i = 0; i < 3; ++i)
            {
                ss >> fs;
                std::strstream ssf;
                ssf << fs;
                std::string sv, svt, svn;
                std::getline(ssf, sv, '/');
                std::getline(ssf, svt, '/');
                std::getline(ssf, svn, '/');
                // they could be negative..
                int v = std::stoi(sv);
                v = v >= 0 ? v : vertexes.size() + v;
                v_[i] = v;
                try
                {
                    int vt = std::stoi(svt);
                    //vt_[i] = vt;
                }
                catch (const std::invalid_argument&) {}
                try
                {
                    int vn = std::stoi(svn);
                    //vn_[i] = vn;
                }
                catch (const std::invalid_argument& e) {}
            }

            m.tris.emplace_back(vertexes.at(v_[0] - 1), vertexes.at(v_[1] - 1), vertexes.at(v_[2] - 1));
        }
        else if (type == "vt")
        {
            // vertex texture
        }
        else if (type == "vn")
        {
            // vertex normal
        }
        else if (type == "vp")
        {
            // vertex parameter
        }
        else if (type == "l")
        {
            // line element
        }
        else if (type == "s")
        {
            // smooth shading
        }
        else if (type == "o")
        {
            ss >> m.name;
        }
        else if (type == "g")
        {
            // group name
        }
    }

    file.close();
    vertexes.clear();
    _meshes.push_back(m);

    return true;
}

void Engine::addLight(const Light& light)
{
    _lights.push_back(light);
    _lightCounts = _lights.size();
}

float Engine::lerp(const float a, const float b, const float t) noexcept
{
    return a + t * (b - a);
}

inline void Engine::drawTriangle(const Triangle& triangle) const noexcept
{
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));

    color_t c = triangle.getColor();

    _screen->setDrawColor(c);
    drawLine(x1, y1, x2, y2, triangle.a.col, triangle.b.col);
    drawLine(x2, y2, x3, y3, triangle.b.col, triangle.c.col);
    drawLine(x3, y3, x1, y1, triangle.c.col, triangle.a.col);
}

inline void Engine::fillTriangle(const Triangle& triangle) const noexcept
{
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));
    
    // Illumination (flat shading)
    color_t c;
    if (illuminationOn == 0) {
        c.r = 255; c.g = 255; c.b = 255; c.a = 255;
        _screen->setDrawColor(c);
    }
    else if (illuminationOn == 1) {
        // blending lights (average)
        int r = 0; int g = 0; int b = 0; int a = 0;
        for (const auto& light : _lights)
        {
            color_t col = light.flatShading(triangle.faceNormal_);
            r += col.r; g += col.g; b += col.b; a += col.a;
        }
        
        c.g = g / _lightCounts;
        c.b = b / _lightCounts;
        c.r = r / _lightCounts;
        c.a = a / _lightCounts;
        
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
    /*color_t c1 = triangle.a.col;
    color_t c2 = triangle.b.col;
    color_t c3 = triangle.c.col;
    color_t tc1;
    color_t tc2;
    const float t1step = 1.0f / sqrt(dx1 * dx1 + dy1 * dy1);
    float t1 = 0.0f;
    const float t2step = 1.0f / sqrt(dx2 * dx2 + dy2 * dy2);
    float t2 = 0.0f;*/
    // interpolate the color of the line1 and line2 (tc1, tc2)
   /* t1 += t1step;
    t2 += t2step;
    color_t tc1 = color_lerpRGB(c1, c2, t1);
    color_t tc2 = color_lerpRGB(c1, c3, t2);
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

void Engine::fillTriangle2(const Triangle& triangle) const noexcept
{
    // TODO: Parallel implementation and color interpolation
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    float z1 = triangle.a.z;
    color_t c1 = triangle.a.col;
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    float z2 = triangle.b.z;
    color_t c2 = triangle.b.col;
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));
    float z3 = triangle.c.z;
    color_t c3 = triangle.c.col;

    // Illumination (flat shading)
    // TODO if illumination off should just interpolate vertex color
    //      if flat shading interpolate with the flat shading light value
    //      if gouraud interpolate vertex colors with light values
    color_t c = triangle.getColor();
    if (illuminationOn == 0) {
        c.r = 255; c.g = 255; c.b = 255; c.a = 255;
        _screen->setDrawColor(c);
    }
    else if (illuminationOn == 1) {
        // blending lights (average)
        unsigned int r = 0;
        unsigned int g = 0;
        unsigned int b = 0;
        unsigned int a = 0;
        for (const auto& light : _lights)
        {
            color_t col = light.flatShading(triangle.faceNormal_);
            r += col.r;
            g += col.g;
            b += col.b;
            a += col.a;
        }

        c.g = g / _lightCounts;
        c.b = b / _lightCounts;
        c.r = r / _lightCounts;
        c.a = a / _lightCounts;
        _screen->setDrawColor(c);
    }
    //else if (illuminationOn == 2) {
    //    // TODO
    //}

    // at first sort the three vertices by y-coordinate ascending so v1 is the topmost vertice
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(z1, z2);
        std::swap(c1, c2);
    }
    if (y1 > y3) {
        std::swap(y1, y3);
        std::swap(x1, x3);
        std::swap(z1, z3);
        std::swap(c1, c3);
    }
    if (y2 > y3) {
        std::swap(y2, y3);
        std::swap(x2, x3);
        std::swap(z2, z3);
        std::swap(c2, c3);
    }


    int dx1 = x2 - x1;
    int dy1 = y2 - y1;
    int sx1 = x1 <= x2 ? 1 : -1;
    int sy1 = y1 <= y2 ? 1 : -1; // always 1 as it has been swapped.
    assert(sy1 == 1);
    int err1 = dx1 + dy1;
    //const int e21 = err << 1;

    const int dx2 = x3 - x1;
    const int dy2 = y3 - y1;
    const int sx2 = x1 <= x3 ? 1 : -1;
    const int sy2 = y1 <= y3 ? 1 : -1; // always 1 as it has been swapped;
    assert(sy2 == 1);
    int err2 = dx2 + dy2;
    //const int e22 = err << 1;
    bool l1yChanged = false;
    bool l2yChanged = false;
    // top half
    int tx1 = x1;
    int tx2 = x1;
    int ty = y1;

    float t1step = 1.0f / sqrt(dx1 * dx1 + dy1 * dy1);
    float t1 = 0.0f;
    float t2step = 1.0f / sqrt(dx2 * dx2 + dy2 * dy2);
    float t2 = 0.0f;
    // todo, remove these and do in integer instead.
    float l1_step = dx1 / static_cast<float>(dy1);
    float l2_step = dx2 / static_cast<float>(dy2);

    // Top-half triangle
    for (; ty < y2; ty++) {
        tx1 = x1 + static_cast<int>(std::round(l1_step * (ty-y1)));
        tx2 = x1 + static_cast<int>(std::round(l2_step * (ty-y1)));

        t1 = t1step * (ty-y1);
        t2 = t2step * (ty-y1);
        color_t tc1 = color_lerpRGB(c1, c2, t1);
        color_t tc2 = color_lerpRGB(c1, c3, t2);

        draw_hline(tx1, tx2, ty, tc1, tc2);
    }

    //// 2nd half
    dy1 = y3 - y2;
    dx1 = x3 - x2;
    sx1 = x2 <= x3 ? 1 : -1;
    t1step = 1.0f / sqrt(dx1 * dx1 + dy1 * dy1);
    t1 = 0.0f;
    t2step = 1.0f / sqrt(dx2 * dx2 + dy2 * dy2);
    t2 = 0.0f;
    
    if (dy1 > 0.0f) {
        l1_step = dx1 / static_cast<float>(dy1);
        for (ty = y2; ty <= y3; ty++)
        {
            tx1 = x2 + static_cast<int>(std::round(l1_step * (ty-y2)));
            tx2 = x1 + static_cast<int>(std::round(l2_step * (ty-y1)));

            t1 = t1step * (ty-y2);
            t2 = t2step * (ty-y1);
            color_t tc1 = color_lerpRGB(c2, c3, t1);
            color_t tc2 = color_lerpRGB(c1, c3, t2);

            draw_hline(tx1, tx2, ty, tc1, tc2);
        }
    }
    else {
        // horizontal line
        draw_hline(x2, x3, y3, c2, c3);
    }

    //_screen->drawPixel(x1, y1, c1); // is it ok?
    //while (ty1 < y2 && tx1 != x2 && tx2 != x3)
    //{
    //    l1yChanged = false;
    //    while (!l1yChanged && tx1 != x2)
    //    {
    //        const int e21 = err1 << 1;
    //        if (e21 >= dy1) {
    //            err1 += dy1;
    //            tx1 += sx1;
    //        }

    //        if (e21 <= dx1) {
    //            err1 += dx1;
    //            ty1++;
    //            l1yChanged = true;
    //        }
    //    }

    //    l2yChanged = false;
    //    while (!l2yChanged && tx2 != x3)
    //    {
    //        const int e22 = err2 << 1;
    //        if (e22 >= dy2) {
    //            err2 += dy2;
    //            tx2 += sx2;
    //        }

    //        if (e22 <= dx2) {
    //            err2 += dx2;
    //            ty2++;
    //            l2yChanged = true;
    //        }
    //    }

    //    // interpolate the color of the line1 and line2 (tc1, tc2)
    //    t1 += t1step;
    //    t2 += t2step;
    //    color_t tc1 = color_lerpRGB(c1, c2, t1);
    //    color_t tc2 = color_lerpRGB(c1, c3, t2);
    //    assert(ty1 == ty2);
    //    draw_hline(tx1, tx2, ty1, tc1, tc2);
    //}

    // Bottom-half triangle

}

void Engine::fillTriangle3(const Triangle& triangle) const noexcept
{
    // Pined alogorithm, traversal bounding box, not incremental edge function
    // Not efficient implementation.

    // It is doing already the back-face culling if drawing only when area is positive
    // this can be useful to avoid some computation.. but at the same time
    // the faceback culling is useful to reduce the number of triangle to
    // process

    // Edge function
    // E(x,y) = (x-X)*dY - (y-Y)*dX ==> V1 = (X,Y), V2(X+dY, Y+dY), P(x,y)
    //        = (x-v1.x)*(v2.y-v1.y) - (y-v1.y)*(v2.y-v1.y)
    const auto edge = [](const int x1, const int y1, const int x2, const int y2, const int x, const int y) noexcept {
        return (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);
    };

    // V1
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    float z1 = triangle.a.z;
    color_t c1 = triangle.a.col;
    // V2
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    float z2 = triangle.b.z;
    color_t c2 = triangle.b.col;
    // V3
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));
    float z3 = triangle.c.z;
    color_t c3 = triangle.c.col;

    // bounding box (no clipping)
    int ymin = std::min(y1, std::min(y2, y3));
    int ymax = std::max(y1, std::max(y2, y3));
    int xmin = std::min(x1, std::min(x2, x3));
    int xmax = std::max(x1, std::max(x2, x3));
    int area = edge(x1, y1, x2, y2, x3, y3);
    int sa = area > 0 ? +1 : -1;

    // when area is negative is a back face, "hidden face" removed already
    // by the back-face culling. Using != 0 so i can see the back of the triangle if wanted.
    // the area == 0 is just a flat triangle, a line, no area and it is like a line color interpolation
    if (area != 0) {
        for (int y = ymin; y <= ymax; y++) {
            for (int x = xmin; x <= xmax; x++) {
                int e1 = edge(x1, y1, x2, y2, x, y); // c3
                int e2 = edge(x2, y2, x3, y3, x, y); // c1
                int e3 = edge(x3, y3, x1, y1, x, y); // c2

                if (e1 * sa >= 0 && e2 * sa >= 0 && e3 * sa >= 0) {
                    // inside the triangle
                    color_t c;
                    c.r = (e1 * c3.r + e2 * c1.r + e3 * c2.r) / area;
                    c.g = (e1 * c3.g + e2 * c1.g + e3 * c2.g) / area;
                    c.b = (e1 * c3.b + e2 * c1.b + e3 * c2.b) / area;
                    _screen->drawPixel(x, y, c);
                }
            }
        }
    }
    else /*if (area == 0)*/ {
        // horizontal line
        color_t c1_;
        color_t c2_;
        if (x1 == xmin) c1_ = c1;
        else if (x2 == xmin) c1_ = c2;
        else if (x3 == xmin) c1_ = c3;
        if (x1 == xmax) c2_ = c1;
        else if (x2 == xmax) c2_ = c2;
        else if (x3 == xmax) c2_ = c3;

        draw_hline(xmin, xmax, ymax,c1,c2);
    }
}

inline void Engine::sortZ() noexcept
{
    std::sort(_trianglesToRaster.begin(), _trianglesToRaster.end(),
        [](const Triangle& t1, const Triangle& t2)
        {
            // divsion by 3.0f can be skipped
            const float z1 = t1.a.z + t1.b.z + t1.c.z;
            const float z2 = t2.a.z + t2.b.z + t2.c.z;
            return z1 < z2;
        }
    );
}

void Engine::raster() noexcept
{
    for (const auto& t : _trianglesToRaster)
    {
        std::list<Triangle> listTriangles;
        _clipping->clipScreen(t, listTriangles);

        // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
        for (auto& t : listTriangles)
        {
            if (filled >= 1) {
                fillTriangle3(t);
                if (filled == 2) {
                    t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                    drawTriangle(t);
                }
            }
            else {
                drawTriangle(t);
            }
        }
    }
}

inline void Engine::draw_hline(int x1, int x2, const int y) const noexcept
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

inline void Engine::draw_hline(int x1, int x2, const int y, const color_t& c) const noexcept
{
    _screen->setDrawColor(c);
    draw_hline(x1, x2, y);
}

void Engine::draw_hline(int x1, int x2, const int y, color_t c1, color_t c2) const noexcept
{
    if (x1 == x2)
    {
        // should be c1 50% and c2 50% ?
        _screen->drawPixel(x1, y, color_lerpRGB(c1, c2, 0.5));
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
    color_t c = c1;
    _screen->drawPixel(x1, y, c);
    for (int x = x1 + 1; x < x2; x++)
    {
        t += tstep;
        c = color_lerpRGB(c1, c2, t);
        //c.r = static_cast<uint8_t>(std::round(Engine::lerp(c1.r, c2.r, t)));
        //c.g = static_cast<uint8_t>(std::round(Engine::lerp(c1.g, c2.g, t)));
        //c.b = static_cast<uint8_t>(std::round(Engine::lerp(c1.b, c2.b, t)));
        //c.a = 255;
        _screen->drawPixel(x, y, c);
    }
    _screen->drawPixel(x2, y, c2);
}

inline void Engine::drawLine(int x1, int y1, const int x2, const int y2) const noexcept
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

inline void Engine::drawLine(int x1,  int y1, const int x2, const int y2, const color_t& c) const noexcept
{
    _screen->setDrawColor(c);
    drawLine(x1, y1, x2, y2);
}

void Engine::drawLine(const int x1, const int y1, const int x2, const int y2, const color_t& c1, const color_t c2) const noexcept
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
    color_t c = c1;

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
        
        t += tstep;
        c = color_lerpRGB(c1, c2, t);
    }
}
