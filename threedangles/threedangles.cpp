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
    example.cam.position.y += 12.0f;
    //example.translation.z -= 10.f; // bug

    const std::string obj_fn = "plain_mountains.obj";
    const bool colv = false;
    const bool hastex = false;
    const std::string obj_tex_fn = "";
        
    example.addMesh(obj_fn, colv, hastex, obj_tex_fn);

    example.mainLoop();

    return 0;
}
