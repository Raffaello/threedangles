#pragma once

#include <string>
#include <Color.hpp>

class Screen
{
public:
    const int width;
    const int height;
    const int screenSize;

    Screen(const std::string& title, const int width, const int height) :
        width(width), height(height), screenSize(width* height), _title(title) {}
    virtual ~Screen() = default;

    virtual bool init() = 0;
    virtual void setDrawColor(const Color& c)noexcept = 0;
    virtual void drawPixel(const int x, const int y) noexcept = 0;
    inline void drawPixel(const int x, const int y, const Color& c) {
        setDrawColor(c);
        drawPixel(x, y);
    }
    inline std::string getTitle() const noexcept { return _title; }
    virtual void setTitle(const std::string& title) noexcept = 0;
    virtual void clear(const Color& c) noexcept = 0;
    virtual void flip() noexcept = 0;

protected:
    std::string _title; // it could be redundant
};
