#include <sdl/Screen_SDL.hpp>
#include <cassert>

namespace sdl
{
    Screen_SDL::Screen_SDL(const std::string& title, const int width, const int height) : Screen(title, width, height)
    {
    }


    Screen_SDL::~Screen_SDL()
    {
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

        // disable 3d acceleration
        SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, nullptr);

        window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        if (nullptr == window) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
            return false;
        }
        
        surface = SDL_GetWindowSurface(window);
        if (nullptr == surface) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, SDL_GetError());
            return false;
        }

        if (surface->format->BytesPerPixel != 4) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "screen is not 32bit (SDL surface has not 4 bytes per pixel)");
            return false;
        }

        if (SDL_MUSTLOCK(surface)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL screen surface must be locked (not implemented)");
            return false;
        }

        pxl = reinterpret_cast<uint8_t*>(surface->pixels);
        fmt = surface->format;
        pitch = surface->pitch;

        return true;
    }

    void Screen_SDL::setDrawColor(const color_t& c) noexcept
    {
        col = SDL_MapRGBA(fmt, c.r, c.g, c.b, c.a);
    }

    inline void Screen_SDL::drawPixel(const int x, const int y) noexcept
    {
        assert(x >= 0 && x < width&& y >= 0 && y < height);
        // writing multiple horizontal pixels at time could
        // be more performant, like using a memcpy
        //uint32_t* buf = reinterpret_cast<uint32_t*>(pxl + y * pitch);
        //buf[x] = col; x=[x1,x2[
        // ---
        // writing multiple vertical pixels at time could be
        // more performant, like increasing the y by pitch (width)
        //uint32_t* buf = reinterpret_cast<uint32_t*>(pxl + x);
        //buf[y * pitch] = col; y =[y1,y2[

        reinterpret_cast<uint32_t*>(pxl + y * pitch)[x] = col;
    }

    void Screen_SDL::setTitle(const std::string& title) noexcept
    {
        _title = title;
        SDL_SetWindowTitle(window, title.c_str());
    }

    void Screen_SDL::clear(const color_t& c) noexcept
    {
        setDrawColor(c);
        SDL_FillRect(surface, nullptr, col);
    }

    void Screen_SDL::flip() noexcept
    {
        SDL_UpdateWindowSurface(window);
    }
}
