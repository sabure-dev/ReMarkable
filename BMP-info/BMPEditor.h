#pragma once

#include <cstdint>
#include "BMPReader.h"

void change_pixel(Image& img, int32_t column, int32_t row, uint8_t blue, uint8_t green, uint8_t red);