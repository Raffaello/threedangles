// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <list>

#include <Vec4.hpp>
#include <Triangle.hpp>
#include <Mesh.hpp>
#include <Engine.hpp>
#include <Cam.hpp>
#include <Light.hpp>

#include <SDL2/SDL.h>
#include <array>

using std::cerr;
using std::endl;
using std::cout;


int main(int argc, char* argv[])
{
    int width = 640;
    int height = 480;
    const float w2 = 0.5f * static_cast<float>(width);
    const float h2 = 0.5f * static_cast<float>(height);
    uint32_t FPS = 60;
    uint32_t frameTime_ms = 1000 / FPS;
    color_t black = { 0, 0, 0, SDL_ALPHA_OPAQUE };
    
    std::string title = "ThreeDangles";
    auto engine = Engine::createEngineSDL(title, width, height);
    if (engine == nullptr) {
        cerr << "can't init engine" << endl;
        return -1;
    }
    auto screen = engine->getScreen();
    
    SDL_Log("FPS CAP ~= %d", FPS);
    SDL_Log("frame_time = %d", frameTime_ms);

    if (!engine->loadMeshFromOBJFile("plain_teapot.obj")) {
        cerr << "Can't load OBJ file";
        return -2;
    }

    // Projection Matrix
    // TODO move to Engine
    const float fov = 50.0f;
    const float zfar = 100.0f;
    const float znear = .5f;
    
    engine->initPerspectiveProjection(fov, zfar, znear);
    // Cam
    Cam cam(Vec4(0.0f, 0.0f, -5.0f), Vec4(0.0f, 1.0f, 0.0f));
    // Light
    Light light(Vec4(0.0f, 0.0f, -1.0f), { 80, 32, 64, 255 });

    bool showHiddenVertexes = false;
    // 0 wire, 1 filled, 2 filled+wire
    short filled = 1;
    bool illuminationOn = true;
    // offset params
    Vec4 translation(0.0f, 0.0f, 0.0f);

    bool quit = false;
    while (!quit) {
        uint32_t startTicks = SDL_GetTicks();
        //uint64_t start_perf = SDL_GetPerformanceCounter();
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                {
                    SDL_Event esc = { 0 };
                    esc.type = SDL_QUIT;
                    SDL_PushEvent(&esc);
                    break;
                }
                case SDLK_h:
                    showHiddenVertexes = !showHiddenVertexes;
                    SDL_Log("Show Hidden Vertexes = %d", showHiddenVertexes);
                    break;
                case SDLK_l:
                    illuminationOn = !illuminationOn;
                    SDL_Log("Illumination ON = %d", illuminationOn);
                    break;
                case SDLK_f:
                    filled++; filled %= 3;
                    SDL_Log("Filling Triangles = %d", filled);
                    break;
                case SDLK_KP_PLUS:
                    translation.z += 0.5f;
                    SDL_Log("offset = %f", translation.z);
                    break;
                case SDLK_KP_MINUS:
                    translation.z -= 0.5f;
                    SDL_Log("offset = %f", translation.z);
                    break;
                case SDLK_UP:
                    cam.position.y += 1.0f;
                    break;
                case SDLK_DOWN:
                    cam.position.y -= 1.0f;
                    break;
                case SDLK_LEFT:
                    cam.turnUp();
                    SDL_Log("cam (%f, %f, %f, %f)", cam.position.x, cam.position.y, cam.position.z, cam.pitch);
                    break;
                case SDLK_RIGHT:
                    cam.turnDown();
                    SDL_Log("cam (%f, %f, %f, %f)", cam.position.x, cam.position.y, cam.position.z, cam.pitch);
                    break;
                case SDLK_a:
                    cam.turnLeft();
                    SDL_Log("cam (%f, %f, %f, %f)", cam.position.x, cam.position.y, cam.position.z, cam.yaw);
                    break;
                case SDLK_d:
                    cam.turnRight();
                    SDL_Log("cam (%f, %f, %f, %f)", cam.position.x, cam.position.y, cam.position.z, cam.yaw);
                    break;
                case SDLK_w:
                    cam.moveForward();
                    SDL_Log("cam (%f, %f, %f)", cam.position.x, cam.position.y, cam.position.z);
                    break;
                case SDLK_s:
                    cam.moveBackward();
                    SDL_Log("cam (%f, %f, %f)", cam.position.x, cam.position.y, cam.position.z);
                    break;
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
            }
        }
        // Clear the screen/buffer
        screen->clear(black);

        // Rotation
        float alpha = 1.0f * SDL_GetTicks() / 1000.0f;
        alpha = 0.0f;
        Mat4 matRotZ = Mat4::createRotationZ(alpha);
        Mat4 matRotX = Mat4::createRotationX(alpha * 0.5f);

        // Translation
        Mat4 matTrans = Mat4::createTranslation(translation);
        // World Matrix
        engine->setMatrixWorld(matTrans * matRotZ * matRotX);
        // Camera Matrix
        engine->setMatrixView(cam.matrixView());
        
        // TODO move Engine
        // Process the triangles.
        //std::vector<Triangle> trianglesToRaster;
        for (auto& mesh : engine->meshes) {
            for (auto& tri : mesh.tris)
            {
                Triangle triTransformed;

                triTransformed = engine->matWorld * tri;

                // Normals (back-face culling)
                // TODO: move to Triangle or Engine class
                // BODY: probably better stored in the same "vector" of triangle to raster
                //       as a component of the triangle
                Vec4 normal = triTransformed.faceNormal();
                float norm_dp = normal.dotProd(triTransformed.a - cam.position);

                if (!showHiddenVertexes && norm_dp >= 0.0f)
                    continue;

                // World Space -> View Space
                triTransformed = engine->matView * triTransformed;
                // TODO avoid to setColor ...
                // BODY color from the mesh instead, and traingle is "private" for rasterization?

                // Clipping section 
                // TODO move to engine
                const Vec4 plane_p_near(0.0f, 0.0f, znear);
                const Vec4 plane_n_near = Vec4(0.0f, 0.0f, 1.0f).normalize();
                const Vec4 plane_p_far(0.0f, 0.0f, zfar);
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

                for (auto& c : clips)
                {
                    // Projection 3D -> 2D & Scale into view (viewport)
                    triTransformed = (engine->matProjection * c);
                    // copy the color from the other translated triangle to the projected one (this should be optimized)
                    triTransformed = triTransformed.normByW();
                    // Triangle Rasterization
                    Engine::raster_t r;
                    r.t = triTransformed;
                    r.faceNormal = normal;
                    engine->trianglesToRaster.push_back(r);
                }
            }
        }

        // Z-depth sorting (Painter's Algorithm)
        // TODO: add a Z buffer when rasterizing to minimize redrawing pixels more then once per frame.
        std::sort(engine->trianglesToRaster.begin(), engine->trianglesToRaster.end(),
            [](const Engine::raster_t& r1, const Engine::raster_t& r2) {
                // divsion by 3.0f can be skipped
                float z1 = (r1.t.a.z + r1.t.b.z + r1.t.c.z); // / 3.0f;
                float z2 = (r2.t.a.z + r2.t.b.z + r2.t.c.z); // / 3.0f;
                return z1 < z2;
            }
        );

        for (auto& t : engine->trianglesToRaster)
        {
            // CLIPPING on Screen size
            // ---
            // Clip triangles against all four screen edges, this could yield
            // a bunch of triangles, so create a queue that we traverse to 
            // ensure we only test new triangles generated against planes
            Triangle clipped[2];
            std::list<Engine::raster_t> listTriangles;

            // Add initial triangle
            listTriangles.push_back(t);
            size_t nNewTriangles = 1;
            const Vec4 plane_p0(0.0f, 0.0f, 0.0f);
            const Vec4 plane_n0 = Vec4(0.0f, 1.0f, 0.0f).normalize();
            const Vec4 plane_p1(0.0f, static_cast<float>(height) - 1.0f, 0.0f);
            const Vec4 plane_n1 = Vec4(0.0f, -1.0f, 0.0f).normalize();
            const Vec4 plane_p2 = plane_p0;
            const Vec4 plane_n2 = Vec4(1.0f, 0.0f, 0.0f).normalize();
            const Vec4 plane_p3(static_cast<float>(width) - 1.0f, 0.0f, 0.0f);
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
            light.direction_normalized = cam.position.normalize();
            for (auto& t : listTriangles)
            {
                // Illumination (flat shading)
                // todo: it should be computed during the rasterization?
                // body create also a Light interface / class
                // If more lights? this need to be moved to the rasterization phase
                if (illuminationOn) t.t.setColor(light.flatShading(t.faceNormal));
                else t.t.setColor(255, 255, 255, SDL_ALPHA_OPAQUE);

                if (filled >= 1) {
                    engine->fillTriangle(t.t);
                    if (filled == 2) {
                        t.t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                        engine->drawTriangle(t.t);
                    }
                }
                else {
                    engine->drawTriangle(t.t);
                }
            }
        }

        // Swap buffers
        screen->flip();
        engine->trianglesToRaster.clear();

        // FPS frame rate cap
        const uint32_t endTicks = SDL_GetTicks();
        //uint64_t endPerf = SDL_GetPerformanceCounter();
        const uint32_t totTicks = (endTicks - startTicks);
        uint32_t frameDelay = 0;

        if (totTicks < frameTime_ms)
            frameDelay = frameTime_ms - totTicks;
        
        screen->setTitle(title + " FPS: ~" + std::to_string(1000.0f / totTicks));
        //SDL_Log("s=%d -- e=%d, d=%u", startTicks, endTicks, frameDelay);
        SDL_Delay(frameDelay);
    }

    return 0;
}
