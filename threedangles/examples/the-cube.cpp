#include <../examples/common.h>

int main(int argc, char* argv[])
{
    return Example::run("The Cube", 640, 480,
        "texture_cube.obj", false, true, "wood.png",
        true, true, true,
        0, 0
    );
}
