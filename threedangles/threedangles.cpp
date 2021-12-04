// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <SDL2/SDL.h>

using std::cerr;
using std::endl;
using std::cout;

#include <vector>

constexpr float PI = 3.14159;//2653589793f;

struct vec3d {
    float x, y, z;
};

struct triangle {
    vec3d a, b, c;
};

struct mesh {
    std::vector<triangle> tris; // ?
};

struct mat4x4 {
    float m[4][4] = { 0 };
};

void MulMatVec(mat4x4& m, vec3d& input, vec3d& output)
{
    output.x = input.x * m.m[0][0] + input.y * m.m[1][0] + input.z * m.m[2][0] + m.m[3][0];
    output.y = input.x * m.m[0][1] + input.y * m.m[1][1] + input.z * m.m[2][1] + m.m[3][1];
    output.z = input.x * m.m[0][2] + input.y * m.m[1][2] + input.z * m.m[2][2] + m.m[3][2];
    float t  = input.x * m.m[0][3] + input.y * m.m[1][3] + input.z * m.m[2][3] + m.m[3][3];

    // Zero epsilon?
    if (t != 0.0f) {
        output.x /= t;
        output.y /= t;
        output.z /= t;
    }
}

// TODO: doing rasterization later on.
void drawTriangle(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3)
{
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}



int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << SDL_GetError() << endl;
        return -1;
    }
    
    int width = 640;
    int height = 480;
    int flags = 0; // SDL_WINDOW_FULLSCREEN
    int r_flags = SDL_RENDERER_SOFTWARE;


    SDL_Window* window = SDL_CreateWindow("ThreeDangles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (nullptr == window) {
        cerr << SDL_GetError() << endl;
        return -1;
    }

    //SDL_Surface* surface = SDL_CreateRGBSurface()
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, r_flags);
    if (nullptr == renderer) {
        cerr << SDL_GetError() << endl;
        return -1;
    }

    /* Enable Unicode translation */
    //SDL_EnableUNICODE(1);

    
    mesh meshCube;
    meshCube.tris = {
        // SOUTH
        {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        // EAST
        {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        {{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
        // NORTH
        {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
        {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // WEST
        {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
        // TOP
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}},
        // BOTTOM
        {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
        {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    };


    // Projection Matrix
    const float ar = static_cast<float>(width) / static_cast<float>(height);
    const float theta = 90.0f;
    const float fov = 1.0f / std::tanf(theta * 0.5f / 180.0f * PI);
    const float zfar = 1000.0f;
    const float znear = 0.1f;
    const float q = zfar / (zfar - znear);

    SDL_Log("ASPECT RATIO = %f", ar);
    SDL_Log("FOV          = %f", fov);

    mat4x4 matProj;
    matProj.m[0][0] = ar * fov;
    matProj.m[1][1] = fov;
    matProj.m[2][2] = q;
    //matProj.m[2][2] = zfar / (zfar - znear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][2] = -(znear * q);
    //matProj.m[3][2] = (-zfar * znear) / (zfar - znear);
    matProj.m[3][3] = 0.0f;


    bool quit = false;
    while (!quit) {
        SDL_Event e;
        SDL_PollEvent(&e);

        switch (e.type)
        {
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                SDL_Event esc;
                esc.type = SDL_QUIT;
                SDL_PushEvent(&esc);
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
        mat4x4 matRotZ, matRotX;
        float alpha = 1.0f * SDL_GetTicks()/1000.0f;
        // Rotation Z
        matRotZ.m[0][0] = std::cosf(alpha);
        matRotZ.m[0][1] = std::sinf(alpha);
        matRotZ.m[1][0] = -std::sinf(alpha);
        matRotZ.m[1][1] = std::cosf(alpha);
        matRotZ.m[2][2] = 1.0f;
        matRotZ.m[3][3] = 1.0f;

        // Rotation X
        matRotX.m[0][0] = 1.0f;
        matRotX.m[1][1] = std::cosf(alpha * 0.5f);
        matRotX.m[1][2] = std::sinf(alpha * 0.5f);
        matRotX.m[2][1] = -std::sinf(alpha * 0.5f);
        matRotX.m[2][2] = std::cosf(alpha * 0.5f);
        matRotX.m[3][3] = 1.0f;

        // offset params
        float offset = 3.0f;
        
        // draw the triangles.
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
        for (auto& tri : meshCube.tris) {
            triangle triProj;
            triangle triRotZ, triRotZX;
            triangle triTranslated;

            // Rotate Z
            MulMatVec(matRotZ, tri.a, triRotZ.a);
            MulMatVec(matRotZ, tri.b, triRotZ.b);
            MulMatVec(matRotZ, tri.c, triRotZ.c);
            // Rotate X
            MulMatVec(matRotX, triRotZ.a, triRotZX.a);
            MulMatVec(matRotX, triRotZ.b, triRotZX.b);
            MulMatVec(matRotX, triRotZ.c, triRotZX.c);

            // offset into the screen
            triTranslated = triRotZX;
            triTranslated.a.z = triRotZX.a.z + offset;
            triTranslated.b.z = triRotZX.b.z + offset;
            triTranslated.c.z = triRotZX.c.z + offset;

            // Proejection 3D -> 2D
            MulMatVec(matProj, triTranslated.a, triProj.a);
            MulMatVec(matProj, triTranslated.b, triProj.b);
            MulMatVec(matProj, triTranslated.c, triProj.c);

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
            drawTriangle(renderer,
                triProj.a.x, triProj.a.y,
                triProj.b.x, triProj.b.y,
                triProj.c.x, triProj.c.y
            );
        }

        

        // Swap buffers
        SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
