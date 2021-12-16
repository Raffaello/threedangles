// threedangles.cpp : Defines the entry point for the application.
//

#include <cstdint>
#include <iostream>
#include <string>

#include <Vec4.hpp>
#include <Engine.hpp>
#include <Cam.hpp>
#include <Light.hpp>
#include <types.hpp>

#include <SDL2/SDL.h>

#ifdef _WIN32
#include <intrin.h>
#endif
#ifdef linux
#include <cpuid.h>
#endif

#include <bitset>

using std::cerr;
using std::endl;
using std::cout;

void cpu_features()
{
    std::vector<std::array<int, 4>> data_;
    std::vector<std::array<int, 4>> exData_;
    std::array<int, 4> cpui;

    std::bitset<32> f_1_ECX_;
    std::bitset<32> f_1_EDX_;
    std::bitset<32> f_7_EBX_;
    std::bitset<32> f_7_ECX_;
    std::bitset<32> f_81_ECX_;
    std::bitset<32> f_81_EDX_;

    // Calling __cpuid with 0x0 as the function_id argument
    // gets the number of the highest valid function ID.
#ifdef _WIN32
    __cpuid(cpui.data(), 0);
#endif
#ifdef linux
    __cpuid(0, cpui[0], cpui[1], cpui[2], cpui[3]);
#endif
    int nIds_ = cpui[0];

    for (int i = 0; i < nIds_; i++) {
        __cpuidex(cpui.data(), i, 0);
        data_.push_back(cpui);
    }

    // Capture vendor string
    char vendor[0x20];
    memset(vendor, 0, sizeof(vendor));
    *reinterpret_cast<int*>(vendor) = data_[0][1];
    *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
    *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
    cout << "CPU vendor: " << vendor << endl;
    // load bitset with flags for function 0x00000001
    if (nIds_ >= 1)
    {
        f_1_ECX_ = data_[1][2];
        f_1_EDX_ = data_[1][3];
    }

    // load bitset with flags for function 0x00000007
    if (nIds_ >= 7)
    {
        f_7_EBX_ = data_[7][1];
        f_7_ECX_ = data_[7][2];
    }

    // --------------------------------------

    // Calling __cpuid with 0x80000000 as the function_id argument
    // gets the number of the highest valid extended ID.
#ifdef _WIN32
    __cpuid(cpui.data(), 0x80000000);
#endif
#ifdef linux
    __cpuid(0x80000000, cpui[0], cpui[1], cpui[2], cpui[3]);
#endif
    int nExIds_ = cpui[0];

    char brand[0x40];
    memset(brand, 0, sizeof(brand));

    for (int i = 0x80000000; i <= nExIds_; ++i)
    {
        __cpuidex(cpui.data(), i, 0);
        exData_.push_back(cpui);
    }

    // load bitset with flags for function 0x80000001
    if (nExIds_ >= 0x80000001)
    {
        f_81_ECX_ = exData_[1][2];
        f_81_EDX_ = exData_[1][3];
    }

    // Interpret CPU brand string if reported
    if (nExIds_ >= 0x80000004)
    {
        memcpy(brand, exData_[2].data(), sizeof(cpui));
        memcpy(brand + 16, exData_[3].data(), sizeof(cpui));
        memcpy(brand + 32, exData_[4].data(), sizeof(cpui));
        //brand_ = brand;
    }

    cout << "CPU Brand: " << brand << endl;
    // ----------------------------------------------------------

    bool MMX = f_1_EDX_[23];
    bool SSE = f_1_EDX_[25];
    bool SSE2 = f_1_EDX_[26];
    bool SSE3 = f_1_ECX_[0];
    bool SSSE3 = f_1_ECX_[9];
    bool SSE41 = f_1_ECX_[19];
    bool SSE42 = f_1_ECX_[20];
    bool AVX = f_1_ECX_[28];
    bool AVX2 = f_7_EBX_[5];
    bool AVX512F = f_7_EBX_[16];
    bool AVX512PF = f_7_EBX_[26];
    bool AVX512ER = f_7_EBX_[27];
    bool AVX512CD = f_7_EBX_[28];

    cout << "Instrcuctions Sets:" << endl;
    if (MMX) cout << "MMX" << endl;
    if (SSE) cout << "SSE" << endl;
    if (SSE2) cout << "SSE2" << endl;
    if (SSE3) cout << "SSE3" << endl;
    if (SSSE3) cout << "SSSE3" << endl;
    if (SSE41) cout << "SSE41" << endl;
    if (SSE42) cout << "SSE42" << endl;
    if (AVX) cout << "AVX" << endl;
    if (AVX2) cout << "AVX2" << endl;
    if (AVX512F) cout << "AVX512F" << endl;
    if (AVX512PF) cout << "AVX512PF" << endl;
    if (AVX512ER) cout << "AVX512ER" << endl;
    if (AVX512CD) cout << "AVX512CD" << endl;
}

int main(int argc, char* argv[])
{
    cpu_features();


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

    if (!engine->addMeshFromOBJFile("plain_mountains.obj")) {
        cerr << "Can't load OBJ file";
        return -2;
    }

    // Projection Matrix
    const float fov = 50.0f;
    const float zfar = 100.0f;
    const float znear = .5f;
    
    engine->initPerspectiveProjection(fov, zfar, znear);
    // Cam
    Cam cam(Vec4(0.0f, 0.0f, -5.0f), Vec4(0.0f, 1.0f, 0.0f));
    // Light
    Light light(Vec4(1.0f, 3.0f, -1.0f), { 80, 32, 64, 255 });
    
    // offset params
    Vec4 translation(0.0f, 0.0f, 0.0f);
    bool quit = false;
    unsigned int tot_frames = 0;
    uint32_t frame_start_ticks = SDL_GetTicks();
    while (!quit)
    {
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
                    engine->showHiddenVertexes = !engine->showHiddenVertexes;
                    SDL_Log("Show Hidden Vertexes = %d", engine->showHiddenVertexes);
                    break;
                case SDLK_l:
                    engine->illuminationOn = !engine->illuminationOn;
                    SDL_Log("Illumination ON = %d", engine->illuminationOn);
                    break;
                case SDLK_f:
                    engine->filled++; engine->filled %= 3;
                    SDL_Log("Filling Triangles = %d", engine->filled);
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
        // TODO there is a bug on the normal and light when "mounted on the cam"
        //light.direction_normalized = cam.position.normalize();
        // Process the triangles.
        engine->processFrame(cam, light, black);
        tot_frames++;
        // FPS frame rate cap
        const uint32_t endTicks = SDL_GetTicks();
        //uint64_t endPerf = SDL_GetPerformanceCounter();
        const uint32_t totTicks = (endTicks - startTicks);
        const uint32_t frameTicks = endTicks - frame_start_ticks;
        uint32_t frameDelay = 0;

        if (totTicks < frameTime_ms)
            frameDelay = frameTime_ms - totTicks;
        
        screen->setTitle(title + " FPS: ~" + std::to_string(1000.0f / totTicks) + " AVG: " + std::to_string(tot_frames * 1000.0f / frameTicks));
        //SDL_Log("s=%d -- e=%d, d=%u", startTicks, endTicks, frameDelay);
        SDL_Delay(frameDelay);
        if (frameTicks >= 1000) {
            tot_frames = 0;
            frame_start_ticks = SDL_GetTicks();
        }
    }

    return 0;
}
