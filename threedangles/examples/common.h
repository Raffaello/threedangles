#include <cstdint>
#include <string>

#include <Engine.hpp>
#include <Cam.hpp>
#include <Light.hpp>
#include <Color.hpp>
#include <CPUID.hpp>
#include <sstream>

#include <SDL2/SDL.h>
#include <sdl/Image_SDL.hpp>

#ifdef WITH_CUDA
#include <cuda/GPUInfo.cuh>
#endif
#include <cassert>
#include <stdexcept>

class Example
{
public:
    void cpu_features()
    {
        CPUID cpuid;
        SDL_Log(cpuid.vendor().c_str());
        SDL_Log(cpuid.brand().c_str());
        SDL_Log("Instrcuctions Sets:");
        if (cpuid.MMX()) SDL_Log("MMX");
        if (cpuid.SSE()) SDL_Log("SSE");
        if (cpuid.SSE2()) SDL_Log("SSE2");
        if (cpuid.SSE3()) SDL_Log("SSE3");
        if (cpuid.SSSE3()) SDL_Log("SSSE3");
        if (cpuid.SSE41()) SDL_Log("SSE41");
        if (cpuid.SSE42()) SDL_Log("SSE42");
        if (cpuid.AVX()) SDL_Log("AVX");
        if (cpuid.AVX2()) SDL_Log("AVX2");
        if (cpuid.AVX512F()) SDL_Log("AVX512F");
        if (cpuid.AVX512PF()) SDL_Log("AVX512PF");
        if (cpuid.AVX512ER()) SDL_Log("AVX512ER");
        if (cpuid.AVX512CD()) SDL_Log("AVX512CD");
    }
    void gpu_features()
    {
#ifdef WITH_CUDA
        cuda::GPUInfo gpuInfo;
        using std::endl;

        SDL_Log("CUDART VERSION : %d", gpuInfo.cudart_version);
        SDL_Log("THRUST VERSION : %d", gpuInfo.thrust_version);
        int err = gpuInfo.getErrorsCount();
        if (err > 0) {
            for (int i = 0; i < err; i++)
                SDL_Log("ERROR %d : %s", i, gpuInfo.getErrors()[i].c_str());
        }

        int devCount = gpuInfo.getDeviceCount();
        assert(gpuInfo.getDeviceProperties().size() == devCount);
        SDL_Log("Total devices  : %d", devCount);
        for (int i = 0; i < devCount; i++)
        {
            cudaDeviceProp devProp = gpuInfo.getDeviceProperties()[i];
            std::stringstream ss;
            ss << "Device:     " << i << endl
                << "Name:       " << devProp.name << " - " << devProp.major << "." << devProp.minor << endl
                << "Global Mem: " << devProp.totalGlobalMem << endl
                << "Shared Mem: " << devProp.sharedMemPerBlock << endl
                ;
            SDL_Log(ss.str().c_str());
        }
#else
        SDL_Log("No CUDA 11 available");
#endif
}

    int width = 640;
    int height = 480;
    uint32_t FPS = 60;
    uint32_t frameTime_ms = 1000 / FPS;
    Color black = { 0, 0, 0, SDL_ALPHA_OPAQUE };
    std::string title;
    std::shared_ptr<Engine> engine;
    std::shared_ptr<Screen> screen;

    // Cam
    Cam cam;
    // Light
    std::shared_ptr<Light> light;
    // Projection Matrix
    const float fov = 50.0f;
    const float zfar = 100.0f;
    const float znear = .5f;
    // offset params
    Vec4 translation;

    std::shared_ptr<Mesh> mesh;

    bool quit = false;
    unsigned int tot_frames = 0;
    uint32_t frame_start_ticks;
    bool perspectiveCorrection = false;
    bool rotation = false;

    void logAndThrow(const char* msg)
    {
        SDL_Log(msg);
        throw std::runtime_error(msg);
    }

    Example(const std::string& title, const int width, const int height, const float fov, const float zfar, const float znear)
        : title(title), width(width), height(height),
        fov(fov),zfar(zfar),znear(znear),
        cam(Vec4(0.0f, 0.0f, -5.0f), Vec4(0.0f, 1.0f, 0.0f)),
        //light(Vec4(.0f, 0.0f, -1.0f), Color(80, 32, 64)),
        translation(0.0f, 0.0f, 0.0f)
    {
        engine = Engine::createEngineSDL(title, width, height);
        if (engine == nullptr)
            logAndThrow("can't init engine");

        screen = engine->getScreen();
        SDL_Log("FPS CAP ~= %d", FPS);
        SDL_Log("frame_time = %d", frameTime_ms);
        
        light = std::make_shared<Light>(Vec4(.0f, 0.0f, -1.0f), Color(80, 32, 64));
        engine->addLight(light);
        
        engine->initPerspectiveProjection(fov, zfar, znear);
        engine->showHiddenVertexes = false;
        engine->filled = 0;
        engine->illuminationType = 0;
    }

    void addMesh(const std::string& filename, bool hasVertexColored, bool hasTexture, const std::string& tex_filename)
    {
        // Mesh
        mesh = Mesh::loadFromOBJFile(filename);
        if (nullptr == mesh)
            logAndThrow("Can't load OBJ file");

        if (hasVertexColored) {
            for (auto& t : mesh->tris)
            {
                t.a.col.r = 255;
                t.b.col.b = 255;
                t.c.col.g = 255;
            }
        }
        else {
            Color c(255, 255, 255);
            for (auto& t : mesh->tris)
                t.a.col = t.b.col = t.c.col = c;
        }

        // Mesh - add texture
        if (hasTexture)
        {
            std::shared_ptr<Image> image = std::make_shared<sdl::Image_SDL>();
            if (!image->loadPNG(tex_filename))
                logAndThrow("Can't load texture");

            mesh->setTexture(image);
        }

        engine->addMesh(mesh);
    }

    void mainLoop()
    {
        SDL_Log("h: toggles hidden vertexes");
        SDL_Log("w,a,s,d, up,dw,lf,rg arrows: move camera");
        SDL_Log("r: toggles rotation");
        SDL_Log("p: toggles perspective correction");
        SDL_Log("f: switch fill modes");
        SDL_Log("l: switch lights");
        SDL_Log("Esc: quit");
        SDL_Log("t: toggle texture (when available)");

        quit = false;
        tot_frames = 0;
        frame_start_ticks = SDL_GetTicks();
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
    }

    static int run(const std::string& title, const int width, const int height,
        const std::string& obj_filename, const bool hasVertexColored, const bool hasTexture, const std::string& obj_tex_filename,
        const bool showHiddenVertexes, const bool rotation, const bool perspectiveCorrection,
        const short filled, const short illuminationType)
    {
        // Projection Matrix
        const float fov = 50.0f;
        const float zfar = 100.0f;
        const float znear = .5f;

        Example example(title, width, height, fov, zfar, znear);
        example.engine->showHiddenVertexes = showHiddenVertexes;
        example.rotation = rotation;
        example.perspectiveCorrection = perspectiveCorrection;
        example.engine->filled = filled;
        example.engine->illuminationType = illuminationType;
        
        example.cpu_features();
        example.gpu_features();
        example.addMesh(obj_filename, hasVertexColored, hasTexture, obj_tex_filename);
        example.mainLoop();

        return 0;
    }
};
