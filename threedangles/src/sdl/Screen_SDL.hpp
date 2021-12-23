#pragma once

#include <Screen.hpp>
#include <SDL2/SDL.h>

namespace sdl
{
    class Screen_SDL final : public Screen
    {
    public:
        Screen_SDL(const std::string& title, const int width, const int height);
        ~Screen_SDL() override;

        bool init() override;
        void setDrawColor(const Color& c) noexcept override;
        void drawPixel(const int x, const int y) noexcept override;
        void setTitle(const std::string& title) noexcept override;
        void clear(const Color& c) noexcept override;
        void flip() noexcept override;
    private:
        bool inited_sdl = false;
        SDL_Window* window = nullptr;
        
        SDL_Surface* surface = nullptr;
        uint8_t* pxl = nullptr;
        SDL_PixelFormat* fmt = nullptr;
        int pitch = 0;
        uint32_t col = 0;
    };
}
