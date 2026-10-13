#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "BMPHeader.h"

struct Image {
    int32_t width{0};
    int32_t height{0};
    int32_t channels{3};
    std::vector<uint8_t> data; // Pixel data in top-down order (BGR)
};

bool BMPread(const std::string& file_path, Image& img);
bool BMPsave(const std::string& file_path, const Image& img);