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
        Image_SDL() = default;
        virtual ~Image_SDL();


        virtual bool loadPNG(const std::string& filename) noexcept override;
        virtual bool getPixel(const int x, const int y, Color& c_out) noexcept override;
        
        //static std::shared_ptr<Image> loadPNG(const std::string& filename) noexcept;
    private:
        SDL_Surface* _image = nullptr;
        bool _locked = false;
        uint8_t* _pxl = nullptr;
    };
}
