#include <sdl/Screen_SDL.hpp>

namespace sdl
{
    Screen_SDL::Screen_SDL(const std::string& title, const int width, const int height) : Screen(title, width, height)
    {
    }


    Screen_SDL::~Screen_SDL()
    {
        if (renderer != nullptr)
            SDL_DestroyRenderer(renderer);

        if(window != nullptr)
            SDL_DestroyWindow(window);

        if(inited_sdl)
            SDL_Quit();
    }

    bool Screen_SDL::init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_VIDEO, SDL_GetError());
            return false;
        }

        inited_sdl = true;

        int flags = 0; // SDL_WINDOW_FULLSCREEN
        int r_flags = SDL_RENDERER_SOFTWARE;

        window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        if (nullptr == window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, r_flags);
        if (nullptr == renderer) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
            return false;
        }

        return true;
    }

    void Screen_SDL::setDrawColor(const color_t& c) noexcept
    {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    }

    void Screen_SDL::drawPixel(const int x, const int y) noexcept
    {
        //if (x >= 0 && x < width && y >= 0 && y < height)
            SDL_RenderDrawPoint(renderer, x, y);
    }

    void Screen_SDL::setTitle(const std::string& title) noexcept
    {
        _title = title;
        SDL_SetWindowTitle(window, title.c_str());
    }

    void Screen_SDL::clear(const color_t& c) noexcept
    {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderClear(renderer);
    }

    void Screen_SDL::flip() noexcept
    {
        SDL_RenderPresent(renderer);
    }

    SDL_Renderer* Screen_SDL::getRenderer() const noexcept
    {
        return renderer;
    }
}