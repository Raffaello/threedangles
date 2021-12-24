#include <sdl/Image_SDL.hpp>


sdl::Image_SDL::~Image_SDL()
{
    if (_locked)
        SDL_UnlockSurface(_image);

    if (_image != nullptr) {
        SDL_FreeSurface(_image);
    }
}

bool sdl::Image_SDL::loadPNG(const std::string& filename) noexcept
{
    if (IMG_Init(IMG_INIT_PNG) == 0)
        return false;

    _image = IMG_Load(filename.c_str());
    if (nullptr == _image)
        return false;

    SDL_LockSurface(_image);
    _locked = true;
    
    _pxl = reinterpret_cast<uint8_t*>(_image->pixels);

    IMG_Quit();
    return true;
}

bool sdl::Image_SDL::getPixel(const int x, const int y, Color& c_out) noexcept
{
    if (nullptr == _pxl)
        return false;

    const uint32_t col = reinterpret_cast<uint32_t*>(_pxl + y * _image->pitch)[x];
    SDL_GetRGBA(col, _image->format, &c_out.r, &c_out.g, &c_out.b, &c_out.a);
}
