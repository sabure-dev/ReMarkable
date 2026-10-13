#include <cstdint>
#include <vector>
#include "../BMP-info/BMPReader.h"

#pragma once
void change_pixel(Image& img, int32_t column, int32_t row, uint8_t blue, uint8_t green, uint8_t red){
    if (column < 0 || column >= img.width || row < 0 || row >= img.height) return;

    size_t idx = (static_cast<size_t>(row) * img.width + column) * img.channels;

    // BGR changes
    img.data[idx]     = blue;
    img.data[idx + 1] = green; 
    img.data[idx + 2] = red; 
}
