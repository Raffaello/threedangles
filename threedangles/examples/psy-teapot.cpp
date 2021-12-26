#include <common.h>

int main(int argc, char* argv[])
{
    return Example::run("Psychedelic Teapot", 640, 480,
        "plain_teapot.obj", true, false, "",
        false, true, true,
        1, 0
    );
}
