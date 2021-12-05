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

    if (!mesh.loadFromOBJFile("plain_axis.obj")) {
        cerr << "Can't load OBJ file";
        quit_sdl(renderer, window);
        return -2;
    }

    // Projection Matrix
    const float fov = 90.0f;
    const float zfar = 1000.0f;
    const float znear = 0.1f;
    Mat4x4 matProj = Engine::matrix_createProjection(width, height, fov, zfar, znear);

    Vec3d cam;
    Vec3d lookAt(0.0f, 0.0f, 1.0f);

    bool showHiddenVertexes = false;
    bool illuminationOn = true;

    // offset params
    float offset = 16.0f;

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
            case SDLK_KP_PLUS:
                offset += 0.5f;
                SDL_Log("offset = %f", offset);
                break;
            case SDLK_KP_MINUS:
                offset -= 0.5f;
                SDL_Log("offset = %f", offset);
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
        //float alpha = 1.0f * SDL_GetTicks() / 1000.0f;
        float alpha = 0.0f;
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
        Vec3d target = cam + lookAt;
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

            // Normals
            Vec3d normal, line1, line2;

            line1 = triTransformed.b - triTransformed.a;
            line2 = triTransformed.c - triTransformed.a;
            normal = line1.crossProd(line2).normalize();
            float norm_dp = normal.dotProd(triTransformed.a - cam);

            if (!showHiddenVertexes && norm_dp >= 0.0f)
                continue;

            // Illumination
            if (illuminationOn)
            {
                Vec3d light_direction(0.0f, 0.0f,-1.0f);
                float dp = normal.dotProd(light_direction.normalize());
                uint8_t r, g, b;
                r = g = b = static_cast<uint8_t>(std::round(dp * 0xFF));
                triTransformed.setColor(r, g, b, SDL_ALPHA_OPAQUE);
                // end Illuminiation
            }

            // World Space -> View Space
            matView.MulMatVec(triTransformed, triViewed);
            triViewed.setColor(triTransformed);
            // Projection 3D -> 2D
            matProj.MulMatVec(triViewed, triProj);
            // copy the color from the other translated triangle to the projected one (this should be optimized)
            triProj.setColor(triViewed);

            // Scale into view
            const Vec3d offsetView(1.0f, 1.0f, 0.0f);
            triProj = triProj + offsetView;
            const float w2 = 0.5f * static_cast<float>(width);
            const float h2 = 0.5f * static_cast<float>(height);
            
            triProj.a.x *= w2;
            triProj.a.y *= h2;
            triProj.b.x *= w2;
            triProj.b.y *= h2;
            triProj.c.x *= w2;
            triProj.c.y *= h2;
            
            // Triangle Rasterization
            trianglesToRaster.push_back(triProj);
        }

        //sorting like a "depth buffer", Z-depth sorting
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(),
            [](Triangle& t1, Triangle& t2) {
                float z1 = (t1.a.z + t1.b.z + t1.c.z) / 3.0f;
                float z2 = (t2.a.z + t2.b.z + t2.c.z) / 3.0f;
                return z1 > z2;
            }
        );

        for (auto& t : trianglesToRaster) {
            t.fill(renderer);
            // Wire frame debug
           /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
           triProj.draw(renderer);*/
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


