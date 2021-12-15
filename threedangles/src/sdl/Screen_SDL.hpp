#pragma once

#include <Screen.hpp>
#include <SDL2/SDL.h>

namespace sdl
{
    class Screen_SDL final : public Screen
    {
    public:
        Screen_SDL(const std::string& title, const int width, const int height);
        virtual ~Screen_SDL();

        virtual bool init() override;
        virtual void setDrawColor(const color_t& c) noexcept override;
        virtual void drawPixel(const int x, const int y) noexcept override;
        virtual void setTitle(const std::string& title) noexcept override;
        virtual void clear(const color_t& c) noexcept override;
        virtual void flip() noexcept override;
    protected:
        bool inited_sdl = false;
        SDL_Window* window = nullptr;
        
        SDL_Surface* surface = nullptr;
        uint8_t* pxl = nullptr;
        SDL_PixelFormat* fmt = nullptr;
        int pitch = 0;
        uint32_t col = 0;
    };
}
