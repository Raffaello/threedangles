#include <../examples/common.h>

int main(int argc, char* argv[])
{
    return Example::run("vertex-coloring", 640, 480, "plain_triangle.obj", true, false, "",
        true, false, true,
        2, 0
    );
}
