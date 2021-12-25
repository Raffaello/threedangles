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

    if (_image->format->BytesPerPixel != 3)
        return false;

    SDL_LockSurface(_image);
    _locked = true;
    
    _pxl = reinterpret_cast<uint8_t*>(_image->pixels);

    IMG_Quit();
    return true;
}

bool sdl::Image_SDL::getPixel(const int x, const int y, Color& c_out) const noexcept
{
    if (nullptr == _pxl)
        return false;
    
    SDL_assert(x >= 0 && x < _image->w);
    SDL_assert(y >= 0 && y < _image->h);

    const int x3 = x * 3;
    const int yw = y * _image->pitch;
    const uint8_t b1 = _pxl[yw + x3];
    const uint8_t b2 = _pxl[yw + x3 + 1];
    const uint8_t b3 = _pxl[yw + x3 + 2];

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    const uint32_t col = b1 << 16 | b2[1] << 8 | b3[2];
#else
    const uint32_t col = b1 | b2 << 8 | b3 << 16;
#endif
    SDL_GetRGBA(col, _image->format, &c_out.r, &c_out.g, &c_out.b, &c_out.a);

    return true;
}

bool sdl::Image_SDL::getPixel(const float u, const float v, Color& c_out) const noexcept
{
    return getPixel(
        static_cast<int>(std::round(u * (_image->w - 1))),
        static_cast<int>(std::round(v * (_image->h - 1))),
        c_out
    );
}

int sdl::Image_SDL::getWidth() const noexcept
{
    return _image->w;
}

int sdl::Image_SDL::getHeight() const noexcept
{
    return _image->h;
}
