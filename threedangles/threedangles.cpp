// threedangles.cpp : Defines the entry point for the application.
//

#include <../examples/common.h>



int main(int argc, char* argv[])
{
    std::string title = "thredangles";
    const int width = 640;
    const int height = 480;
    const float fov = 50.0f;
    const float zfar = 100.0f;
    const float znear = .5f;

    Example example(title, width, height, fov, zfar, znear);
    example.engine->filled = 1;
    example.engine->illuminationType = 2;
    example.engine->showHiddenVertexes = false;
    //example.cam.position.y += 12.0f;
    //example.translation.z -= 10.f; // bug
    bool light_move = true;
    float light_step = 0.05f;
    uint32_t light_ticks_speed = 100;
    uint32_t light_tick_last_step = 0;

    const std::string obj_fn = "plain_teapot.obj";
    const bool colv = false;
    const bool hastex = false;
    const std::string obj_tex_fn = "";
        
    example.addMesh(obj_fn, colv, hastex, obj_tex_fn);

    //example.mainLoop();
    SDL_Log("h: toggles hidden vertexes");
    SDL_Log("w,a,s,d, up,dw,lf,rg arrows: move example.camera");
    SDL_Log("r: toggles rotation");
    SDL_Log("p: toggles perspective correction");
    SDL_Log("f: switch fill modes");
    SDL_Log("l: switch lights");
    SDL_Log("Esc: quit");
    SDL_Log("t: toggle texture (when available)");

    example.quit = false;
    example.tot_frames = 0;
    example.frame_start_ticks = SDL_GetTicks();
    while (!example.quit)
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
                    example.engine->showHiddenVertexes = !example.engine->showHiddenVertexes;
                    SDL_Log("Show Hidden Vertexes = %d", example.engine->showHiddenVertexes);
                    break;
                case SDLK_l:
                    example.engine->illuminationType++;
                    example.engine->illuminationType %= 3;
                    SDL_Log("Illumination ON = %d", example.engine->illuminationType);
                    break;
                case SDLK_f:
                    example.engine->filled++; example.engine->filled %= 3;
                    SDL_Log("Filling Triangles = %d", example.engine->filled);
                    break;
                case SDLK_UP:
                    example.cam.position.y += 1.0f;
                    break;
                case SDLK_DOWN:
                    example.cam.position.y -= 1.0f;
                    break;
                case SDLK_LEFT:
                    example.cam.turnUp();
                    SDL_Log("cam (%f, %f, %f, %f)", example.cam.position.x, example.cam.position.y, example.cam.position.z, example.cam.pitch);
                    break;
                case SDLK_RIGHT:
                    example.cam.turnDown();
                    SDL_Log("cam (%f, %f, %f, %f)", example.cam.position.x, example.cam.position.y, example.cam.position.z, example.cam.pitch);
                    break;
                case SDLK_a:
                    example.cam.turnLeft();
                    SDL_Log("cam (%f, %f, %f, %f)", example.cam.position.x, example.cam.position.y, example.cam.position.z, example.cam.yaw);
                    break;
                case SDLK_d:
                    example.cam.turnRight();
                    SDL_Log("cam (%f, %f, %f, %f)", example.cam.position.x, example.cam.position.y, example.cam.position.z, example.cam.yaw);
                    break;
                case SDLK_w:
                    example.cam.moveForward();
                    SDL_Log("cam (%f, %f, %f)", example.cam.position.x, example.cam.position.y, example.cam.position.z);
                    break;
                case SDLK_s:
                    example.cam.moveBackward();
                    SDL_Log("cam (%f, %f, %f)", example.cam.position.x, example.cam.position.y, example.cam.position.z);
                    break;
                case SDLK_p:
                    example.perspectiveCorrection = !example.perspectiveCorrection;
                    example.engine->setPerpsectiveCorrection(example.perspectiveCorrection);
                    SDL_Log("perspective Correction %d", example.perspectiveCorrection);
                    break;
                case SDLK_r:
                    example.rotation = !example.rotation;
                    SDL_Log("rotation on %d", example.rotation);
                    break;
                case SDLK_t:
                    example.mesh->setShowTexture(!example.mesh->showTexture);
                    SDL_Log("show texture %d", example.mesh->showTexture);
                    break;
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                example.quit = true;
                break;
            default:
                break;
            }
        }

        // Rotation
        float alpha = 0.0f;
        if (example.rotation)
            alpha = 1.0f * startTicks / 1000.0f;
        Mat4 matRotZ = Mat4::createRotationZ(alpha);
        Mat4 matRotX = Mat4::createRotationX(alpha * 0.5f);
        
        if (light_tick_last_step < startTicks)
        {
            light_tick_last_step = startTicks + light_ticks_speed;
            // move light
            if (light_move)
            {
                if (example.light->direction.x > 2.0f || example.light->direction.x < -2.0f) {
                    light_step *= -1;
                    
                }
                /*else {
                    example.light->direction.x += light_step;
                }*/
                example.light->direction.x += light_step;
                example.light->direction_normalized = example.light->direction.normalize();
                SDL_Log("light x: %f", example.light->direction.x);
            }
        }
        
        // Translation
        Mat4 matTrans = Mat4::createTranslation(example.translation);
        // World Matrix
        example.engine->setMatrixWorld(matTrans * matRotZ * matRotX);
        // example.camera Matrix
        example.engine->setMatrixView(example.cam.matrixView());
        example.engine->processFrame(example.cam, example.black);
        example.tot_frames++;
        // FPS frame rate cap
        const uint32_t endTicks = SDL_GetTicks();
        const uint32_t totTicks = (endTicks - startTicks);
        const uint32_t frameTicks = endTicks - example.frame_start_ticks;
        uint32_t frameDelay = 0;

        if (totTicks < example.frameTime_ms)
            frameDelay = example.frameTime_ms - totTicks;

        example.screen->setTitle(title + " FPS: ~" + std::to_string(1000.0f / totTicks) + " AVG: " + std::to_string(example.tot_frames * 1000.0f / frameTicks));
        SDL_Delay(frameDelay);
        if (frameTicks >= 5000) {
            example.tot_frames = 0;
            example.frame_start_ticks = SDL_GetTicks();
        }
    }

    return 0;
}
