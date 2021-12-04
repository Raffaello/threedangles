// threedangles.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <SDL2/SDL.h>

using std::cerr;
using std::endl;
using std::cout;

#include <vector>

struct vector3d {
    float x, y, z;
};

struct triangle {
    vector3d a, b, c;
};

struct mesh {
    std::vector<triangle> tris; // ?
};



int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << SDL_GetError() << endl;
        return -1;
    }
    
    int width = 320;
    int height = 200;
    int flags = 0; // SDL_WINDOW_FULLSCREEN

    SDL_Window* window = SDL_CreateWindow("ThreeDangles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (nullptr == window) {
        cerr << SDL_GetError() << endl;
        return -1;
    }
   

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

    /* Enable Unicode translation */
    //SDL_EnableUNICODE(1);


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


        // draw the triangles.


    }

    SDL_Quit();
    return 0;
}
