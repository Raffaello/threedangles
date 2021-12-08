// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cmath>
#include <string>

#include <Mesh.hpp>
#include <Mat4x4.hpp>
#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <algorithm>
#include <Engine.hpp>

#include <SDL2/SDL.h>

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
    
    int width = 512;
    int height = 512;
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
    const float znear = 0.1f;
    // TODO unify Projection, offsetview and matScale
    // BODY into 1 matrix only instead of 3 distinct operations.
    Mat4x4 matProj = Engine::matrix_createProjection(width, height, fov, zfar, znear);
    const Vec3d offsetView(1.0f, 1.0f, 0.0f);
    Mat4x4 matScale = Engine::matrix_createScale(w2, h2, 1.0f);

    Vec3d cam(0.0f, 0.0f, -1.0f);
    Vec3d lookAt(0.0f, 0.0f, 0.0f);
    float cam_yaw = 0.0f;

    bool showHiddenVertexes = false;
    // 0 wire, 1 filled, 2 filled+wire
    short filled = 1;
    bool illuminationOn = true;

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
                offset -= 0.5f;
                SDL_Log("offset = %f", offset);
                break;
            case SDLK_KP_MINUS:
                offset += 0.5f;
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
                break;
            case SDLK_d:
                cam_yaw += 0.1f;
                break;
            case SDLK_w:
                cam = cam + lookAt * 0.5f;
                break;
            case SDLK_s:
                cam = cam - lookAt * 0.5f;
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
        Mat4x4 matRotZ = Engine::matrix_createRotationZ(alpha);
        Mat4x4 matRotX = Engine::matrix_createRotationX(alpha);

        // Translation
        Mat4x4 matTrans = Engine::matrix_createTranslation({ 0.0f, 0.0f, offset });

        // World Matrix
        Mat4x4 matWorld; // = Engine::matrix_createIdentity();
        // do the matrix multiplication
        matWorld = matRotZ * matRotX * matTrans;

        // Camera Matrix
        //lookAt = { 0.0f, 0.0f, 1.0f };
        Vec3d up(0.0f, 1.0f, 0.0f);
        Vec3d target(0.0f, 0.0f, 1.0f);
        Mat4x4 matCamRot = Engine::matrix_createRotationY(cam_yaw);
        lookAt = matCamRot * target;
        target = cam + lookAt;

        Mat4x4 matCam = Engine::matrix_pointAt(cam, target, up);
        Mat4x4 matView = Engine::matrix_InversePointAt(matCam);

        // Process the triangles.
        std::vector<Triangle> trianglesToRaster;
        for (auto& tri : mesh.tris)
        {
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

            // Projection 3D -> 2D
            triProj = (matProj * triViewed).normByW();
            // copy the color from the other translated triangle to the projected one (this should be optimized)
            triProj.setColor(triViewed);
            // Scale into view
            triProj = triProj + offsetView;
            triProj = matScale * triProj;
            
            // Triangle Rasterization
            trianglesToRaster.push_back(triProj);
        }

        //sorting like a "depth buffer", Z-depth sorting
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
                t.fill(renderer);
                // Wire frame debug
                if (filled == 2) {
                    t.setColor(0, 0, 0, SDL_ALPHA_OPAQUE);
                    t.draw(renderer);
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
        
        //SDL_SetWindowTitle(window, (title + " FPS: ~" + std::to_string(1000.0f/frameDelay)).c_str());
        //SDL_Log("s=%d -- e=%d, d=%u", startTicks, endTicks, frameDelay);
        SDL_Delay(frameDelay);
    }

    quit_sdl(renderer, window);
    return ret;
}


