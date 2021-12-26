// threedangles.cpp : Defines the entry point for the application.
//

#include <cstdint>
#include <iostream>
#include <string>

#include <Engine.hpp>
#include <Cam.hpp>
#include <Light.hpp>
#include <Color.hpp>

#include <SDL2/SDL.h>

#include <CPUID.hpp>

#include <cassert>
#include <sdl/Image_SDL.hpp>

#ifdef WITH_CUDA
#include <cuda/GPUInfo.cuh>
#endif

using std::cerr;
using std::endl;
using std::cout;

void cpu_features()
{
    CPUID cpuid;
    cout << cpuid.vendor() << endl;
    cout << cpuid.brand() << endl;
    cout << "Instrcuctions Sets:" << endl;
    if (cpuid.MMX()) cout << "MMX" << endl;
    if (cpuid.SSE()) cout << "SSE" << endl;
    if (cpuid.SSE2()) cout << "SSE2" << endl;
    if (cpuid.SSE3()) cout << "SSE3" << endl;
    if (cpuid.SSSE3()) cout << "SSSE3" << endl;
    if (cpuid.SSE41()) cout << "SSE41" << endl;
    if (cpuid.SSE42()) cout << "SSE42" << endl;
    if (cpuid.AVX()) cout << "AVX" << endl;
    if (cpuid.AVX2()) cout << "AVX2" << endl;
    if (cpuid.AVX512F()) cout << "AVX512F" << endl;
    if (cpuid.AVX512PF()) cout << "AVX512PF" << endl;
    if (cpuid.AVX512ER()) cout << "AVX512ER" << endl;
    if (cpuid.AVX512CD()) cout << "AVX512CD" << endl;
}

void gpu_features()
{
#ifdef WITH_CUDA
    cuda::GPUInfo gpuInfo;

    cout << "CUDART VERSION : " << gpuInfo.cudart_version << endl;
    cout << "THRUST VERSION : " << gpuInfo.thrust_version << endl;
    int err = gpuInfo.getErrorsCount();
    if (err > 0) {
        for (int i = 0; i < err; i++)
            cerr << "ERROR " << i << ": " << gpuInfo.getErrors()[i] << endl;
    }

    int devCount = gpuInfo.getDeviceCount();
    assert(gpuInfo.getDeviceProperties().size() == devCount);
    cout << "Total devices  : " << devCount << endl;
    for (int i = 0; i < devCount; i++)
    {
        cudaDeviceProp devProp = gpuInfo.getDeviceProperties()[i];
        cout
            << "Device:     " << i << endl
            << "Name:       " << devProp.name << " - " << devProp.major << "." << devProp.minor << endl
            << "Global Mem: " << devProp.totalGlobalMem << endl
            << "Shared Mem: " << devProp.sharedMemPerBlock << endl
            ;
    }
#else
    cout << "No CUDA 11 available" << endl;
#endif
}

int main(int argc, char* argv[])
{
    cpu_features();
    gpu_features();

    int width = 640;
    int height = 480;
    uint32_t FPS = 60;
    uint32_t frameTime_ms = 1000 / FPS;
    Color black = { 0, 0, 0, SDL_ALPHA_OPAQUE };

    std::string title = "The Cube";
    auto engine = Engine::createEngineSDL(title, width, height);
    if (engine == nullptr) {
        cerr << "can't init engine" << endl;
        return -1;
    }
    auto screen = engine->getScreen();

    SDL_Log("FPS CAP ~= %d", FPS);
    SDL_Log("frame_time = %d", frameTime_ms);

    // Mesh
    auto mesh = Mesh::loadFromOBJFile("texture_cube.obj");
    if (nullptr == mesh) {
        cerr << "Can't load OBJ file";
        return -2;
    }

    for (auto& t : mesh->tris)
    {
        t.a.col.r = 255;
        t.b.col.b = 255;
        t.c.col.g = 255;
    }
    // Mesh - add texture
    std::shared_ptr<Image> image = std::make_shared<sdl::Image_SDL>();
    if (!image->loadPNG("wood.png"))
    {
        cerr << "Can't load texture";
        return -3;
    }

    mesh->setTexture(image);

    engine->addMesh(mesh);
    // Projection Matrix
    const float fov = 50.0f;
    const float zfar = 100.0f;
    const float znear = .5f;

    engine->initPerspectiveProjection(fov, zfar, znear);
    engine->showHiddenVertexes = false;
    engine->filled = 0;
    engine->illuminationType = 0;

    // Cam
    Cam cam(Vec4(0.0f, 0.0f, -5.0f), Vec4(0.0f, 1.0f, 0.0f));
    // Light
    Light light(Vec4(.0f, 0.0f, -1.0f), { 80, 32, 64, 255 });
    engine->addLight(light);
    // offset params
    Vec4 translation(0.0f, 0.0f, 0.0f);
    bool quit = false;
    unsigned int tot_frames = 0;
    uint32_t frame_start_ticks = SDL_GetTicks();
    bool perspectiveCorrection = false;
    bool rotation = false;
    while (!quit)
    {
        uint32_t startTicks = SDL_GetTicks();
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
                    engine->showHiddenVertexes = !engine->showHiddenVertexes;
                    SDL_Log("Show Hidden Vertexes = %d", engine->showHiddenVertexes);
                    break;
                case SDLK_l:
                    engine->illuminationType++;
                    engine->illuminationType %= 3;
                    SDL_Log("Illumination ON = %d", engine->illuminationType);
                    break;
                case SDLK_f:
                    engine->filled++; engine->filled %= 3;
                    SDL_Log("Filling Triangles = %d", engine->filled);
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
                case SDLK_p:
                    perspectiveCorrection = !perspectiveCorrection;
                    engine->setPerpsectiveCorrection(perspectiveCorrection);
                    SDL_Log("perspective Correction %d", perspectiveCorrection);
                    break;
                case SDLK_r:
                    rotation = !rotation;
                    SDL_Log("rotation on %d", rotation);
                    break;
                case SDLK_t:
                    mesh->setShowTexture(!mesh->showTexture);
                    SDL_Log("show texture %d", mesh->showTexture);
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

        // Rotation
        float alpha = 0.0f;
        if (rotation)
            alpha = 1.0f * SDL_GetTicks() / 1000.0f;
        Mat4 matRotZ = Mat4::createRotationZ(alpha);
        Mat4 matRotX = Mat4::createRotationX(alpha * 0.5f);

        // Translation
        Mat4 matTrans = Mat4::createTranslation(translation);
        // World Matrix
        engine->setMatrixWorld(matTrans * matRotZ * matRotX);
        // Camera Matrix
        engine->setMatrixView(cam.matrixView());
        engine->processFrame(cam, black);
        tot_frames++;
        // FPS frame rate cap
        const uint32_t endTicks = SDL_GetTicks();
        const uint32_t totTicks = (endTicks - startTicks);
        const uint32_t frameTicks = endTicks - frame_start_ticks;
        uint32_t frameDelay = 0;

        if (totTicks < frameTime_ms)
            frameDelay = frameTime_ms - totTicks;

        screen->setTitle(title + " FPS: ~" + std::to_string(1000.0f / totTicks) + " AVG: " + std::to_string(tot_frames * 1000.0f / frameTicks));
        SDL_Delay(frameDelay);
        if (frameTicks >= 5000) {
            tot_frames = 0;
            frame_start_ticks = SDL_GetTicks();
        }
    }

    return 0;
}