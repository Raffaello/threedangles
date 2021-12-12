#pragma once

#include <string>
#include <types.hpp>

class Screen
{
public:
    const int width;
    const int height;

    Screen(const std::string& title, const int width, const int height) : _title(title), width(width), height(height) {}
    virtual ~Screen() = default;

    virtual bool init() = 0;
    virtual void setDrawColor(const color_t& c)noexcept = 0;
    virtual void drawPixel(const int x, const int y) noexcept = 0;
    virtual void drawPixel(const int x, const int y, const color_t& c) {
        setDrawColor(c);
        drawPixel(x, y);
    }
    inline std::string getTitle() const noexcept { return _title; }
    virtual void setTitle(const std::string& title) noexcept = 0;
    virtual void clear(const color_t& c) noexcept = 0;
    virtual void flip() noexcept = 0;

protected:
    std::string _title; // it could be redundant
};
