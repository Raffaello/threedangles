#pragma once

#include <Image.hpp>
#include <memory>
#include <string>
#include <SDL2/SDL_image.h>

namespace sdl
{
    class Image_SDL : public Image
    {
    public:
        Image_SDL(Image_SDL&) = delete;
        Image_SDL(Image_SDL&&) = delete;
        Image_SDL& operator=(const Image_SDL&) = delete;

        Image_SDL() = default;
        virtual ~Image_SDL();

        bool loadPNG(const std::string& filename) noexcept override;
        bool getPixel(const int x, const int y, Color& c_out) noexcept override;
    private:
        SDL_Surface* _image = nullptr;
        bool _locked = false;
        uint8_t* _pxl = nullptr;
    };
}
