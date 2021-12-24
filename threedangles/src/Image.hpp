#pragma once

#include <cstdint>
#include <Color.hpp>

class Image
{
public:
    Image() = default;
    ~Image() = default;

    virtual bool loadPNG(const std::string& filename) noexcept = 0;
    virtual bool getPixel(const int x, const int y, Color& c_out) noexcept = 0;
    //virtual bool setPixel(const int x, const int y, const Color& c) noexcept = 0;

};
