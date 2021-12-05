// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cmath>
#include <string>

#include <Mesh.hpp>
#include <Mat4.hpp>
#include <Vec3d.hpp>
#include <Triangle.hpp>
#include <algorithm>

#include <SDL2/SDL.h>

using std::cerr;
using std::endl;
using std::cout;

constexpr float PI = 3.14159f;//2653589793f;

int main(int argc, char* argv[])
{
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

    SDL_Log("FPS CAP ~= %d", FPS);
    SDL_Log("frame_time = %d", frameTime_ms);

    SDL_Window* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (nullptr == window) {
        cerr << SDL_GetError() << endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, r_flags);
    if (nullptr == renderer) {
        cerr << SDL_GetError() << endl;
        return -1;
    }

    Mesh meshCube;
    //meshCube.tris = {
    //    // SOUTH
    //      1                         2                 3
    //    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
    //       1                         3                 4
    //    {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    //    // EAST
    //        4                        3                    5
    //    {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
    //          4                      5                  6
    //    {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
    //    // NORTH
    //        6                     5                    7
    //    {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
    //        6                      7                    8
    //    {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    //    // WEST
    //            8                    7                  2
    //    {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    //            8                    2                   1
    //    {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
    //    // TOP
    //          2                      7                  5
    //    {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
    //           2                     5                  3
    //    {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
    //    // BOTTOM
    //           6                     8                    1
    //    {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
    //           6                     1                    4
    //    {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    //};
    if (!meshCube.loadFromOBJFile("plain_cube.obj")) {
        cerr << "Can't load OBJ file";
        // todo review these dubplicated "destructors"
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -2;
    }


    // Projection Matrix
    const float ar = static_cast<float>(width) / static_cast<float>(height);
    const float theta = 90.0f;
    const float fov = 1.0f / std::tan(theta * 0.5f / 180.0f * PI);
    const float zfar = 1000.0f;
    const float znear = 0.1f;
    const float q = zfar / (zfar - znear);

    SDL_Log("ASPECT RATIO = %f", ar);
    SDL_Log("FOV          = %f", fov);

    Mat4 matProj;
    matProj.m[0][0] = ar * fov;
    matProj.m[1][1] = fov;
    matProj.m[2][2] = q;
    matProj.m[2][3] = 1.0f;
    matProj.m[3][2] = -(znear * q);
    matProj.m[3][3] = 0.0f;

    Vec3d cam = { 0 };

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
        Mat4 matRotZ, matRotX;
        float alpha = 1.0f * SDL_GetTicks()/1000.0f;
        // Rotation Z
        matRotZ.m[0][0] = std::cos(alpha);
        matRotZ.m[0][1] = std::sin(alpha);
        matRotZ.m[1][0] = -std::sin(alpha);
        matRotZ.m[1][1] = std::cos(alpha);
        matRotZ.m[2][2] = 1.0f;
        matRotZ.m[3][3] = 1.0f;

        // Rotation X
        matRotX.m[0][0] = 1.0f;
        matRotX.m[1][1] = std::cos(alpha * 0.5f);
        matRotX.m[1][2] = std::sin(alpha * 0.5f);
        matRotX.m[2][1] = -std::sin(alpha * 0.5f);
        matRotX.m[2][2] = std::cos(alpha * 0.5f);
        matRotX.m[3][3] = 1.0f;

        
        
        // draw the triangles.
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
        for (auto& tri : meshCube.tris) {
            Triangle triProj;
            Triangle triRotZ, triRotZX;
            Triangle triTranslated;

            // Rotate Z
            matRotZ.MulMatVec(tri.a, triRotZ.a);
            matRotZ.MulMatVec(tri.b, triRotZ.b);
            matRotZ.MulMatVec(tri.c, triRotZ.c);
            // Rotate X
            matRotX.MulMatVec(triRotZ.a, triRotZX.a);
            matRotX.MulMatVec(triRotZ.b, triRotZX.b);
            matRotX.MulMatVec(triRotZ.c, triRotZX.c);

            // offset into the screen
            triTranslated = triRotZX;
            triTranslated.a.z = triRotZX.a.z + offset;
            triTranslated.b.z = triRotZX.b.z + offset;
            triTranslated.c.z = triRotZX.c.z + offset;

            // Normals
            Vec3d normal, line1, line2;
            line1.x = triTranslated.b.x - triTranslated.a.x;
            line1.y = triTranslated.b.y - triTranslated.a.y;
            line1.z = triTranslated.b.z - triTranslated.a.z;

            line2.y = triTranslated.c.y - triTranslated.a.y;
            line2.x = triTranslated.c.x - triTranslated.a.x;
            line2.z = triTranslated.c.z - triTranslated.a.z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            float nl = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= nl;
            normal.y /= nl;
            normal.z /= nl;

            float norm_dp = normal.x * (triTranslated.a.x - cam.x)
                + normal.y * (triTranslated.a.y - cam.y)
                + normal.z * (triTranslated.a.z - cam.z)
            ;

            if (!showHiddenVertexes && norm_dp >= 0.0f)
                continue;

            // Illumination
            if (illuminationOn)
            {
                Vec3d light_direction = { 0.0f, 0.0f,-1.0f };
                float ll = std::sqrt(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
                light_direction.x /= ll;
                light_direction.y /= ll;
                light_direction.z /= ll;

                // Dot Product
                float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
                uint8_t r, g, b;
                r = g = b = static_cast<uint8_t>(std::round(dp * 0xFF));
                SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
                // end Illuminiation
            }


            // Projection 3D -> 2D
            matProj.MulMatVec(triTranslated.a, triProj.a);
            matProj.MulMatVec(triTranslated.b, triProj.b);
            matProj.MulMatVec(triTranslated.c, triProj.c);

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

            // Triangle Rasterization
            /*fillTriangle(renderer,
                triProj.a.x, triProj.a.y,
                triProj.b.x, triProj.b.y,
                triProj.c.x, triProj.c.y
            );*/

            triProj.fill(renderer);

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


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
