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
        ret = -1;
        goto quit;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, r_flags);
    if (nullptr == renderer) {
        cerr << SDL_GetError() << endl;
        ret = -1;
        goto quit;
    }

    
    if (!mesh.loadFromOBJFile("plain_cube.obj")) {
        cerr << "Can't load OBJ file";
        ret = -2;
        goto quit;
    }

    // Projection Matrix
    const float fov = 90.0f;
    const float zfar = 1000.0f;
    const float znear = 0.1f;
    Mat4x4 matProj = Engine::matrix_createProjection(width, height, fov, zfar, znear);

    Vec3d cam;

    bool showHiddenVertexes = false;
    bool illuminationOn = true;

    // offset params
    float offset = 3.0f;

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
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        }

        // Clear the screen/buffer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Rotation
        float alpha = 1.0f * SDL_GetTicks() / 1000.0f;
        Mat4x4 matRotZ = Engine::matrix_createRotationZ(alpha);
        Mat4x4 matRotX = Engine::matrix_createRotationX(alpha);

        // Translation
        Mat4x4 matTrans = Engine::matrix_createTranslation({ 0.0f, 0.0f, offset });

        // World Matrix
        //Mat4x4 matWorld = Engine::matrix_createIdentity();
        // do the matrix multiplication


        // Process the triangles.
        std::vector<Triangle> trianglesToRaster;
        for (auto& tri : mesh.tris)
        {
            Triangle triProj;
            Triangle triRotZ, triRotZX;
            Triangle triTranslated;

            // Rotate Z
            matRotZ.MulMatVec(tri, triRotZ);
            // Rotate X
            matRotX.MulMatVec(triRotZ, triRotZX);

            // offset into the screen
            triTranslated = triRotZX;
            triTranslated.a.z = triRotZX.a.z + offset;
            triTranslated.b.z = triRotZX.b.z + offset;
            triTranslated.c.z = triRotZX.c.z + offset;

            // Normals
            Vec3d normal, line1, line2;

            line1 = triTranslated.b - triTranslated.a;
            line2 = triTranslated.c - triTranslated.a;
            normal = line1.crossProd(line2).normalize();
            float norm_dp = normal.dotProd(triTranslated.a - cam);

            if (!showHiddenVertexes && norm_dp >= 0.0f)
                continue;

            // Illumination
            if (illuminationOn)
            {
                Vec3d light_direction(0.0f, 0.0f,-1.0f);
                light_direction.normalize();
                // Dot Product
                float dp = normal.dotProd(light_direction);
                uint8_t r, g, b;
                r = g = b = static_cast<uint8_t>(std::round(dp * 0xFF));
                triTranslated.setColor(r, g, b, SDL_ALPHA_OPAQUE);
                // end Illuminiation
            }

            // Projection 3D -> 2D
            matProj.MulMatVec(triTranslated, triProj);

            // Scale into view
            triProj.a.x += 1.0f; triProj.a.y += 1.0f;
            triProj.b.x += 1.0f; triProj.b.y += 1.0f;
            triProj.c.x += 1.0f; triProj.c.y += 1.0f;

            triProj.a.x *= 0.5f * static_cast<float>(width);
            triProj.a.y *= 0.5f * static_cast<float>(height);
            triProj.b.x *= 0.5f * static_cast<float>(width);
            triProj.b.y *= 0.5f * static_cast<float>(height);
            triProj.c.x *= 0.5f * static_cast<float>(width);
            triProj.c.y *= 0.5f * static_cast<float>(height);
            // copy the color from the other translated triangle to the projected one (this should be optimized)
            triProj.setColor(triTranslated);
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
        uint32_t endTicks = SDL_GetTicks();
        //uint64_t endPerf = SDL_GetPerformanceCounter();
        uint32_t frameDelay = frameTime_ms - (endTicks - startTicks);

        SDL_SetWindowTitle(window, (title + " FPS: " + std::to_string(1000.0f/frameDelay)).c_str());
        SDL_Delay(frameDelay);
        //SDL_Log("s=%d -- e=%d, d=%d", startTicks, endTicks, frameDelay);
    }

quit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return ret;
}
