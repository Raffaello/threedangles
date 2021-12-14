// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <list>

#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <Mesh.hpp>
#include <Engine.hpp>

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
    Mesh mesh;
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

    if (!mesh.loadFromOBJFile("plain_teapot.obj")) {
        cerr << "Can't load OBJ file";
        return -2;
    }

    // Projection Matrix
    // TODO move to Engine
    const float fov = 50.0f;
    const float zfar = 100.0f;
    const float znear = .5f;
    
    Mat4 matProj = Mat4::createScale(w2, h2, 1.0f)
        * Mat4::createTranslation({ 1.0f, 1.0f, 0.0f })
        * Mat4::createProjection(width, height, fov, zfar, znear);

    // Cam
    Vec3d cam(0.0f, 0.0f, -1.0f);
    Vec3d lookAt(0.0f, 0.0f, 0.0f);
    float cam_yaw = 0.0f;
    const Vec3d up(0.0f, 1.0f, 0.0f);
    Vec3d target(0.0f, 0.0f, 1.0f);
    // Light
    const Vec3d light_direction(0.0f, 0.0f, -1.5f);
    const Vec3d light_direction_normalized = light_direction.normalize();

    bool showHiddenVertexes = false;
    // 0 wire, 1 filled, 2 filled+wire
    short filled = 1;
    bool illuminationOn = true;
    bool clipping = true;
    // offset params
    float offset = 5.0f;

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
                    offset += 0.5f;
                    SDL_Log("offset = %f", offset);
                    break;
                case SDLK_KP_MINUS:
                    offset -= 0.5f;
                    SDL_Log("offset = %f", offset);
                    break;
                case SDLK_UP:
                    cam.y += 1.0f;
                    break;
                case SDLK_DOWN:
                    cam.y -= 1.0f;
                    break;
                case SDLK_LEFT:
                    cam.x += 1.0f;
                    break;
                case SDLK_RIGHT:
                    cam.x -= 1.0f;
                    break;
                case SDLK_a:
                    cam_yaw -= 0.1f;
                    SDL_Log("cam (%f, %f, %f, %f)", cam.x, cam.y, cam.z, cam_yaw);
                    break;
                case SDLK_d:
                    cam_yaw += 0.1f;
                    SDL_Log("cam (%f, %f, %f, %f)", cam.x, cam.y, cam.z, cam_yaw);
                    break;
                case SDLK_w:
                    cam = cam + lookAt * 0.5f;
                    SDL_Log("cam (%f, %f, %f)", cam.x, cam.y, cam.z);
                    break;
                case SDLK_s:
                    cam = cam - lookAt * 0.5f;
                    SDL_Log("cam (%f, %f, %f)", cam.x, cam.y, cam.z);
                    break;
                case SDLK_c:
                    clipping = !clipping;
                    SDL_Log("clipping = %d", clipping);
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
        //alpha = 0.0f;
        Mat4 matRotZ = Mat4::createRotationZ(alpha);
        Mat4 matRotX = Mat4::createRotationX(alpha * 0.5f);

        // Translation
        Mat4 matTrans = Mat4::createTranslation({ 0.0f, 0.0f, offset });

        // World Matrix
        Mat4 matWorld; // = Engine::matrix_createIdentity();
        // do the matrix multiplication
        matWorld = matTrans * matRotZ * matRotX;

        // Camera Matrix
        Mat4 matCamRot = Mat4::createRotationY(cam_yaw);
        // TODO: this doesn't look right
        target.x = 0.0f; target.y = 0.0f; target.z = 1.0f;
        lookAt = matCamRot * target;
        target = cam + lookAt;
        Mat4 matCam = engine->matrix_pointAt(cam, target, up);
        Mat4 matView = engine->matrix_InversePointAt(matCam);

        // Process the triangles.
        std::vector<Triangle> trianglesToRaster;
        for (auto& tri : mesh.tris)
        {
            // todo: keep only triProj as the other temporary are not useful
            Triangle triProj;
            Triangle triTransformed;
            Triangle triViewed;

            triTransformed = matWorld * tri;

            // Normals (back-face culling)
            // TODO: move to Triangle or Engine class
            Vec3d normal = triTransformed.faceNormal();
            float norm_dp = normal.dotProd(triTransformed.a - cam);

            if (!showHiddenVertexes && norm_dp >= 0.0f)
                continue;

            // Illumination (flat shading)
            // todo: it should be computed during the rasterization?
            // body create also a Light interface / class
            if (illuminationOn)
            {
                float dp = normal.dotProd(light_direction_normalized);
                uint8_t r, g, b;
                r = static_cast<uint8_t>(std::round(dp * 64));
                g = static_cast<uint8_t>(std::round(dp * 64));
                b = static_cast<uint8_t>(std::round(dp * 64));
                triTransformed.setColor(r, g, b, SDL_ALPHA_OPAQUE);
            }
            else {
                triTransformed.setColor(255, 255, 255, SDL_ALPHA_OPAQUE);
            }

            // World Space -> View Space
            triViewed = matView * triTransformed;
            // TODO avoid to setColor ...
            // BODY color from the mesh instead, and traingle is "private" for rasterization?
            triViewed.setColor(triTransformed);

            const Vec3d plane_p_near(0.0f, 0.0f, znear);
            const Vec3d plane_n_near = Vec3d(0.0f, 0.0f, 1.0f).normalize();
            const Vec3d plane_p_far(0.0f, 0.0f, zfar);
            const Vec3d plane_n_far = Vec3d(0.0f, 0.0f, -1.0f).normalize();
            std::vector<Triangle> clips;
            if (clipping) {
                // Clipping on Znear plane (triViewd -> clipped[2])
                int nClippedTriangles = 0;
                Triangle clipped[2];
                nClippedTriangles = triViewed.clipAgainstPlane(plane_p_near, plane_n_near, clipped[0], clipped[1]);
                // clipping on Zfar plane (clipped[2] -> vector<clippped>)
                for (int i = 0; i < nClippedTriangles; i++)
                {
                    Triangle clippedFar[2];
                    int nClippedTrianglesFar = clipped[i].clipAgainstPlane(plane_p_far, plane_n_far, clippedFar[0], clippedFar[1]);
                    for (int n = 0; n < nClippedTrianglesFar; n++)
                        clips.push_back(clippedFar[n]);
                }
            }
            else {
                clips.push_back(triViewed);
            }

            for (auto& c: clips)
            {
                // Projection 3D -> 2D & Scale into view (viewport)
                triProj = (matProj * c);
                // copy the color from the other translated triangle to the projected one (this should be optimized)
                triProj.setColor(c);
                triProj = triProj.normByW();

                // Triangle Rasterization
                trianglesToRaster.push_back(triProj);
            }
        }

        // Z-depth sorting (Painter's Algorithm)
        // TODO: add a Z buffer when rasterizing to minimize redrawing pixels nore then once per frame.
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(),
            [](Triangle& t1, Triangle& t2) {
                // divsion by 3.0f can be skipped
                float z1 = (t1.a.z + t1.b.z + t1.c.z); // / 3.0f;
                float z2 = (t2.a.z + t2.b.z + t2.c.z); // / 3.0f;
                return z1 < z2;
            }
        );

        if (filled >= 1) {
            for (auto& t : trianglesToRaster) {
                if (!clipping) {
                    engine->fillTriangle(t);
                    
                    // Wire frame debug
                    if (filled == 2) {
                        t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                        engine->drawTriangle(t);
                    }
                    continue;
                }

                // Clip triangles against all four screen edges, this could yield
                // a bunch of triangles, so create a queue that we traverse to 
                //  ensure we only test new triangles generated against planes
                Triangle clipped[2];
                std::list<Triangle> listTriangles;

                // Add initial triangle
                listTriangles.push_back(t);
                size_t nNewTriangles = 1;
                const Vec3d plane_p0(0.0f, 0.0f, 0.0f);
                const Vec3d plane_n0 = Vec3d(0.0f, 1.0f, 0.0f).normalize();
                const Vec3d plane_p1(0.0f, static_cast<float>(height) - 1.0f, 0.0f);
                const Vec3d plane_n1 = Vec3d(0.0f, -1.0f, 0.0f).normalize();
                const Vec3d plane_p2 = plane_p0;
                const Vec3d plane_n2 = Vec3d(1.0f, 0.0f, 0.0f).normalize();
                const Vec3d plane_p3(static_cast<float>(width) - 1.0f, 0.0f, 0.0f);
                const Vec3d plane_n3 = Vec3d(-1.0f, 0.0f, 0.0f).normalize();

                const std::array<const Vec3d, 4> plane_p = { plane_p0, plane_p1, plane_p2, plane_p3 };
                const std::array<const Vec3d, 4> plane_n = { plane_n0, plane_n1, plane_n2, plane_n3 };
                
                for (int p = 0; p < 4; p++)
                {
                    while (nNewTriangles > 0)
                    {
                        int nTrisToAdd = 0;
                        // Take triangle from front of queue
                        Triangle tri = listTriangles.front();
                        listTriangles.pop_front();
                        nNewTriangles--;

                        // Clip it against a plane. We only need to test each 
                        // subsequent plane, against subsequent new triangles
                        // as all triangles after a plane clip are guaranteed
                        // to lie on the inside of the plane. I like how this
                        // comment is almost completely and utterly justified
                        nTrisToAdd = tri.clipAgainstPlane(plane_p[p], plane_n[p], clipped[0], clipped[1]);

                        // Clipping may yield a variable number of triangles, so
                        // add these new ones to the back of the queue for subsequent
                        // clipping against next planes
                        for (int w = 0; w < nTrisToAdd; w++)
                            listTriangles.push_back(clipped[w]);
                    }
                    nNewTriangles = listTriangles.size();
                }

                // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
                for (auto& t : listTriangles)
                {
                    engine->fillTriangle(t);
                    if (filled == 2) {
                        t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                        engine->drawTriangle(t);
                    }
                }
            }
        }
        else {
            for (auto& t : trianglesToRaster) {
                engine->drawTriangle(t);
            }
        }

        // Swap buffers
        screen->flip();

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
