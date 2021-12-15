#include <Engine.hpp>
#include <cmath>
#include <sdl/Screen_SDL.hpp>
#include <fstream>
#include <strstream> // todo remove as it has been deprecated
#include <vector>
#include <exception>
#include <algorithm>
#include <list>

Engine::Engine(const std::shared_ptr<Screen> screen) : _screen(screen)
{
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
    matProjection = matProj;
}

void Engine::setMatrixWorld(const Mat4& matWorld) noexcept
{
    this->matWorld = matWorld;
}

void Engine::setMatrixView(const Mat4& matView) noexcept
{
    this->matView = matView;
}

void Engine::initPerspectiveProjection(const float fov, const float far, const float near) noexcept
{
    this->fov = fov;
    this->far = far;
    this->near = near;
    matProjection = Mat4::createScale(_screen->width / 2.0f, _screen->height / 2.0f, 1.0f)
        * Mat4::createTranslation({ 1.0f, 1.0f, 0.0f })
        * Mat4::createProjection(_screen->width, _screen->height, fov, far, near);
}

void Engine::processFrame(const Cam& cam, const Light& light, const color_t& bg_col) noexcept
{
    trianglesToRaster.clear();
    // Clear the screen/buffer
    _screen->clear(bg_col);
    for (const auto& mesh : meshes)
    {
        for (const auto& tri : mesh.tris)
        {
            Triangle triTransformed;

            triTransformed = matWorld * tri;

            // Normals (back-face culling)
            Vec4 normal = triTransformed.faceNormal();
            float norm_dp = normal.dotProd(triTransformed.a - cam.position);

            if (!showHiddenVertexes && norm_dp >= 0.0f)
                continue;

            // World Space -> View Space
            triTransformed = matView * triTransformed;
            // Clipping section 
            const Vec4 plane_p_near(0.0f, 0.0f, near);
            const Vec4 plane_n_near = Vec4(0.0f, 0.0f, 1.0f).normalize();
            const Vec4 plane_p_far(0.0f, 0.0f, far);
            const Vec4 plane_n_far = Vec4(0.0f, 0.0f, -1.0f).normalize();
            std::vector<Triangle> clips;
            // Clipping on Znear plane (triViewd -> clipped[2])
            int nClippedTriangles = 0;
            Triangle clipped[2];
            nClippedTriangles = triTransformed.clipAgainstPlane(plane_p_near, plane_n_near, clipped[0], clipped[1]);
            // clipping on Zfar plane (clipped[2] -> vector<clippped>)
            for (int i = 0; i < nClippedTriangles; i++)
            {
                Triangle clippedFar[2];
                int nClippedTrianglesFar = clipped[i].clipAgainstPlane(plane_p_far, plane_n_far, clippedFar[0], clippedFar[1]);
                for (int n = 0; n < nClippedTrianglesFar; n++)
                    clips.push_back(clippedFar[n]);
            }

            for (const auto& c : clips)
            {
                // Projection 3D -> 2D & Scale into view (viewport)
                Engine::raster_t r;
                r.t = (matProjection * c).normByW();
                r.faceNormal = normal;
                trianglesToRaster.push_back(r);
            }
        }
    }

    // Z-depth sorting (Painter's Algorithm)
        // TODO: add a Z buffer when rasterizing to minimize redrawing pixels more then once per frame.
    std::sort(trianglesToRaster.begin(), trianglesToRaster.end(),
        [](const Engine::raster_t& r1, const Engine::raster_t& r2) {
            // divsion by 3.0f can be skipped
            float z1 = (r1.t.a.z + r1.t.b.z + r1.t.c.z); // / 3.0f;
            float z2 = (r2.t.a.z + r2.t.b.z + r2.t.c.z); // / 3.0f;
            return z1 < z2;
        }
    );

    // Triangle Rasterization
    for (const auto& t : trianglesToRaster)
    {
        // CLIPPING on Screen size
        // ---
        // Clip triangles against all four screen edges, this could yield
        // a bunch of triangles, so create a queue that we traverse to 
        // ensure we only test new triangles generated against planes
        Triangle clipped[2];
        std::list<raster_t> listTriangles;

        // Add initial triangle
        listTriangles.push_back(t);
        size_t nNewTriangles = 1;
        const Vec4 plane_p0(0.0f, 0.0f, 0.0f);
        const Vec4 plane_n0 = Vec4(0.0f, 1.0f, 0.0f).normalize();
        const Vec4 plane_p1(0.0f, static_cast<float>(_screen->height) - 1.0f, 0.0f);
        const Vec4 plane_n1 = Vec4(0.0f, -1.0f, 0.0f).normalize();
        const Vec4 plane_p2 = plane_p0;
        const Vec4 plane_n2 = Vec4(1.0f, 0.0f, 0.0f).normalize();
        const Vec4 plane_p3(static_cast<float>(_screen->width) - 1.0f, 0.0f, 0.0f);
        const Vec4 plane_n3 = Vec4(-1.0f, 0.0f, 0.0f).normalize();

        const std::array<const Vec4, 4> plane_p = { plane_p0, plane_p1, plane_p2, plane_p3 };
        const std::array<const Vec4, 4> plane_n = { plane_n0, plane_n1, plane_n2, plane_n3 };

        for (int p = 0; p < 4; p++)
        {
            while (nNewTriangles > 0)
            {
                int nTrisToAdd = 0;
                // Take triangle from front of queue
                Engine::raster_t r = listTriangles.front();
                listTriangles.pop_front();
                nNewTriangles--;

                // Clip it against a plane. We only need to test each 
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                nTrisToAdd = r.t.clipAgainstPlane(plane_p[p], plane_n[p], clipped[0], clipped[1]);

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++) {
                    Engine::raster_t rr;
                    rr.faceNormal = r.faceNormal;
                    rr.t = clipped[w];
                    listTriangles.push_back(rr);
                }
            }
            nNewTriangles = listTriangles.size();
        }

        // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
        for (auto& t : listTriangles)
        {
            // Illumination (flat shading)
            // todo: it should be computed during the rasterization?
            // body create also a Light interface / class
            // If more lights? this need to be moved to the rasterization phase
            if (illuminationOn) t.t.setColor(light.flatShading(t.faceNormal));
            else t.t.setColor(255, 255, 255, SDL_ALPHA_OPAQUE);

            if (filled >= 1) {
                fillTriangle(t.t);
                if (filled == 2) {
                    t.t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                    drawTriangle(t.t);
                }
            }
            else {
                drawTriangle(t.t);
            }
        }
    }

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
                catch (const std::invalid_argument& e) {}
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
            // object name
        }
        else if (type == "g")
        {
            // group name
        }
    }

    file.close();
    vertexes.clear();
    meshes.push_back(m);

    return true;
}

void Engine::drawTriangle(const Triangle& triangle)
{
    // triangle.normByW();
    //compute_int_coord();
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));

    auto c = triangle.getColor();
    // rasterization clipping,
    // not working when the triangle is out of the screen
    // as it could still draw something
    // as the coords are just capped in the max and min values
    // instead should just not be drawn
    /*if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 < 0) x2 = 0;
    if (y2 < 0) y2 = 0;
    if (x3 < 0) x3 = 0;
    if (y3 < 0) y3 = 0;

    if (x1 > _screen->width)  x1 = _screen->width;
    if (y1 > _screen->height) y1 = _screen->height;
    if (x2 > _screen->width)  x2 = _screen->width;
    if (y2 > _screen->height) y2 = _screen->height;
    if (x3 > _screen->width)  x3 = _screen->width;
    if (y3 > _screen->height) y3 = _screen->height;*/

    _screen->setDrawColor(c);
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);

}

void Engine::fillTriangle(const Triangle& triangle)
{
    // triangle.normByW();
    //compute_int_coord();
    int x1 = static_cast<int>(std::round(triangle.a.x));
    int y1 = static_cast<int>(std::round(triangle.a.y));
    int x2 = static_cast<int>(std::round(triangle.b.x));
    int y2 = static_cast<int>(std::round(triangle.b.y));
    int x3 = static_cast<int>(std::round(triangle.c.x));
    int y3 = static_cast<int>(std::round(triangle.c.y));
    
    _screen->setDrawColor(triangle.getColor());

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

    int t1x = 0, t2x = 0, y = 0, minx = 0, maxx = 0, t1xp = 0, t2xp = 0;
    bool changed1 = false;
    bool changed2 = false;

    int8_t signx1 = 0, signx2 = 0;

    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
    int e1 = 0, e2 = 0;

    // -----

    t1x = t2x = x1;
    y = y1;
    changed1 = changed2 = false;

    //fill_top_flat(renderer);
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
        while (true)
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
next:
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

void Engine::draw_hline(int x1, int x2, const int y) const noexcept
{
    if (x1 >= x2) std::swap(x1, x2);
    for (; x1 <= x2; x1++) {
        _screen->drawPixel(x1, y);
    }
}

void Engine::draw_hline(int x1, int x2, const int y, const color_t& c) noexcept
{
    _screen->setDrawColor(c);
    draw_hline(x1, x2, y);
}

void Engine::drawLine(int x1, int y1, const int x2, const int y2) noexcept
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

void Engine::drawLine(int x1,  int y1, const int x2, const int y2, const color_t& c) noexcept
{
    _screen->setDrawColor(c);
    drawLine(x1, y1, x2, y2);
}
