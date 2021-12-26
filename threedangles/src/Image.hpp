#pragma once

#include <cstdint>
#include <Color.hpp>
#include <string>

class Image
{
public:
    Image() = default;
    ~Image() = default;

    Image(Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(const Image&) = delete;

    virtual bool loadPNG(const std::string& filename) noexcept = 0;
    virtual bool getPixel(const int x, const int y, Color& c_out) const noexcept = 0;
    virtual bool getPixel(const float u, const float v, Color& c_out) const noexcept = 0;

    virtual int getWidth() const noexcept = 0;
    virtual int getHeight() const noexcept = 0;
};
