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
        virtual void drawPixel(const int x, const int y, const color_t& c) override;
        virtual void setTitle(const std::string& title) noexcept override;
        virtual void clear(const color_t& c) noexcept override;
        virtual void flip() noexcept override;


        // method only for Screen_SDL
        SDL_Renderer* getRenderer() const noexcept;

    protected:
        bool inited_sdl = false;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
    };
}
