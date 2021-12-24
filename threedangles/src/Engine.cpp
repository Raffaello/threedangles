#include <Engine.hpp>
#include <sdl/Screen_SDL.hpp>
#include <algorithm>
#include <list>

Engine::Engine(const std::shared_ptr<Screen> screen) : _screen(screen)
{
    _rasterizer = std::make_shared<Rasterizer>(_screen);
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

void Engine::processFrame(const Cam& cam, const Color& bg_col) noexcept
{
    _trianglesToRaster.clear();
    // Clear the screen/buffer
    _screen->clear(bg_col);
    // TODO: notice that is -far instead of +far, it looks something not ok with left/right hand coordinates
    std::fill(_screen->_depthBuffer.get(), _screen->_depthBuffer.get() + _screen->screenSize, -far);

    for (const auto& mesh : _meshes)
    {
        if (!mesh->visible)
            continue;
        // matWorld can be copied in the Mesh and concatenated to other Mesh transformation
        // and then compute the "MeshTransformed already" to be ready to be reused
        // unless something changes ?
        mesh->render(_matProjection, _matWorld, _matView, showHiddenVertexes, cam, _clipping, _trianglesToRaster);
    }

    // Z-depth reverse-sorting (Painter's Algorithm reverse)
    sortZReverse();

    // Triangle Rasterization
    raster();

    // Swap buffers
    _screen->flip();
}

void Engine::addMesh(const std::shared_ptr<Mesh>& mesh)
{
    if (nullptr == mesh)
        return;

    _meshes.push_back(mesh);
}

void Engine::addLight(const Light& light)
{
    _lights.push_back(light);
    _lightCounts = _lights.size();
}

inline void Engine::sortZ() noexcept
{
    std::sort(_trianglesToRaster.begin(), _trianglesToRaster.end(),
        [](const Triangle& t1, const Triangle& t2)
        {
            // divsion by 3.0f can be skipped
            const float z1 = t1.a.v.z + t1.b.v.z + t1.c.v.z;
            const float z2 = t2.a.v.z + t2.b.v.z + t2.c.v.z;
            return z1 > z2;
        }
    );
}

void Engine::sortZReverse() noexcept
{
    std::sort(_trianglesToRaster.begin(), _trianglesToRaster.end(),
        [](const Triangle& t1, const Triangle& t2)
        {
            // divsion by 3.0f can be skipped
            const float z1 = t1.a.v.z + t1.b.v.z + t1.c.v.z;
            const float z2 = t2.a.v.z + t2.b.v.z + t2.c.v.z;
            return z1 < z2;
        }
    );
}

void Engine::raster() noexcept
{
    for (const auto& t : _trianglesToRaster)
    {
        // @todo: this can be done with pineda algorithm instead,
        //       producing less shape morphing and improving performances
        std::list<Triangle> listTriangles;
        _clipping->clipScreen(t, listTriangles);

        for (auto& t : listTriangles)
        {
            //if (filled >= 1)
            //{
            //    // fill triangle
            //    _rasterizer->fillTriangle3(t, illuminationOn, _lights);
            //    if (filled == 2)
            //    {
            //        // wireframe debug
            //        _rasterizer->drawTriangle(t, Color(255, 255, 255));
            //    }
            //}
            //else {
            //    // wireframe
            //    _rasterizer->drawTriangle(t);
            //}
            

            // TEST
            _rasterizer->TexTriangle3(t);
            _rasterizer->drawTriangle(t, Color(255, 255, 255));
        }
    }
}
