// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cmath>
#include <string>

#include <Mesh.hpp>
#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <algorithm>
#include <Engine.hpp>

#include <SDL2/SDL.h>


#include <list>

using std::cerr;
using std::endl;
using std::cout;

void quit_sdl(SDL_Renderer* renderer, SDL_Window* window)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    int ret = 0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << SDL_GetError() << endl;
        return -1;
    }
    
    int width = 640;
    int height = 480;
    const float w2 = 0.5f * static_cast<float>(width);
    const float h2 = 0.5f * static_cast<float>(height);
    int flags = 0; // SDL_WINDOW_FULLSCREEN
    int r_flags = SDL_RENDERER_SOFTWARE;
    std::string title = "ThreeDangles";
    int FPS = 60;
    int frameTime_ms = 1000 / FPS;
    Mesh mesh;

    SDL_Log("FPS CAP ~= %d", FPS);
    SDL_Log("frame_time = %d", frameTime_ms);

    SDL_Window* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (nullptr == window) {
        cerr << SDL_GetError() << endl;
        quit_sdl(nullptr, nullptr);
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, r_flags);
    if (nullptr == renderer) {
        cerr << SDL_GetError() << endl;
        quit_sdl(nullptr, window);
        return - 1;
    }

    if (!mesh.loadFromOBJFile("plain_cube.obj")) {
        cerr << "Can't load OBJ file";
        quit_sdl(renderer, window);
        return -2;
    }

    // Projection Matrix
    const float fov = 90.0f;
    const float zfar = 100.0f;
    const float znear = .5f;
    
    Mat4 matProj = Mat4::createScale(w2, h2, 1.0f)
        * Mat4::createTranslation({ 1.0f, 1.0f, 0.0f })
        * Mat4::createProjection(width, height, fov, zfar, znear);

    Vec3d cam(0.0f, 0.0f, -1.0f);
    Vec3d lookAt(0.0f, 0.0f, 0.0f);
    float cam_yaw = 0.0f;

    bool showHiddenVertexes = false;
    // 0 wire, 1 filled, 2 filled+wire
    short filled = 1;
    bool illuminationOn = true;
    bool clipping = true;
    // offset params
    float offset = 5.0f;

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        SDL_PollEvent(&e);

        uint32_t startTicks = SDL_GetTicks();
        //uint64_t start_perf = SDL_GetPerformanceCounter();

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
                SDL_Log("cam (%f, %f, %f)", cam.x, cam.y, cam.z);
                break;
            case SDLK_d:
                cam_yaw += 0.1f;
                SDL_Log("cam (%f, %f, %f)", cam.x, cam.y, cam.z);
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

        // Clear the screen/buffer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

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
        //lookAt = { 0.0f, 0.0f, 1.0f };
        Vec3d up(0.0f, 1.0f, 0.0f);
        Vec3d target(0.0f, 0.0f, 1.0f);
        Mat4 matCamRot = Mat4::createRotationY(cam_yaw);
        lookAt = matCamRot * target;
        target = cam + lookAt;

        Mat4 matCam = Engine::matrix_pointAt(cam, target, up);
        Mat4 matView = Engine::matrix_InversePointAt(matCam);

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
            Vec3d normal, line1, line2;

            line1 = triTransformed.b - triTransformed.a;
            line2 = triTransformed.c - triTransformed.a;
            normal = line1.crossProd(line2).normalize();
            float norm_dp = normal.dotProd(triTransformed.a - cam);
            
            if (!showHiddenVertexes && norm_dp >= 0.0f)
                continue;

            // Illumination (flat shading)
            // todo: it should be computed during the rasterization?
            if (illuminationOn)
            {
                Vec3d light_direction(0.0f, 0.0f,-1.0f);
                float dp = normal.dotProd(light_direction.normalize());
                uint8_t r, g, b;
                r = g = b = static_cast<uint8_t>(std::round(dp * 0xFF));
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

            std::vector<Triangle> clips;
            if (clipping) {
                // Clipping on Znear plane (triViewd -> clipped[2])
                int nClippedTriangles = 0;
                Triangle clipped[2];
                nClippedTriangles = Engine::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, znear }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);
                // clipping on Zfar plane (clipped[2] -> vector<clippped>)
                for (int i = 0; i < nClippedTriangles; i++)
                {
                    Triangle clippedFar[2];
                    int nClippedTrianglesFar = Engine::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, zfar }, { 0.0f, 0.0f, -1.0f }, clipped[i], clippedFar[0], clippedFar[1]);
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

        // Z-depth sorting
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(),
            [](Triangle& t1, Triangle& t2) {
                // divsion by 3.0f can be skipped
                float z1 = (t1.a.z + t1.b.z + t1.c.z); // / 3.0f;
                float z2 = (t2.a.z + t2.b.z + t2.c.z); // / 3.0f;
                return z1 > z2;
            }
        );

        if (filled >= 1) {
            for (auto& t : trianglesToRaster) {
                if (!clipping) {
                    t.fill(renderer);
                    // Wire frame debug
                    if (filled == 2) {
                        t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                        t.draw(renderer);
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
                int nNewTriangles = 1;

                for (int p = 0; p < 4; p++)
                {
                    int nTrisToAdd = 0;
                    while (nNewTriangles > 0)
                    {
                        // Take triangle from front of queue
                        Triangle test = listTriangles.front();
                        listTriangles.pop_front();
                        nNewTriangles--;

                        // Clip it against a plane. We only need to test each 
                        // subsequent plane, against subsequent new triangles
                        // as all triangles after a plane clip are guaranteed
                        // to lie on the inside of the plane. I like how this
                        // comment is almost completely and utterly justified
                        switch (p)
                        {
                        case 0:
                            nTrisToAdd = Engine::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]);
                            break;
                        case 1:
                            nTrisToAdd = Engine::Triangle_ClipAgainstPlane({ 0.0f, (float)height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]);
                            break;
                        case 2:
                            nTrisToAdd = Engine::Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]);
                            break;
                        case 3:
                            nTrisToAdd = Engine::Triangle_ClipAgainstPlane({ (float)width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]);
                            break;
                        default:
                            break;
                        }

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
                    t.fill(renderer);
                    //DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_SOLID, FG_BLACK);
                    if (filled == 2) {
                        t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                        t.draw(renderer);
                    }
                }
            }
        }
        else {
            for (auto& t : trianglesToRaster) {
                t.draw(renderer);
            }
        }

        // Swap buffers
        SDL_RenderPresent(renderer);

        // FPS frame rate cap
        const uint32_t endTicks = SDL_GetTicks();
        //uint64_t endPerf = SDL_GetPerformanceCounter();
        const uint32_t totTicks = (endTicks - startTicks);
        uint32_t frameDelay = 0;

        if (totTicks < frameTime_ms)
            frameDelay = frameTime_ms - totTicks;
        
        SDL_SetWindowTitle(window, (title + " FPS: ~" + std::to_string(1000.0f/totTicks)).c_str());
        //SDL_Log("s=%d -- e=%d, d=%u", startTicks, endTicks, frameDelay);
        SDL_Delay(frameDelay);
    }

    quit_sdl(renderer, window);
    return ret;
}


