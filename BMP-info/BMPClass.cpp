#include "BMPClass.h"

#include <cstddef>

BMPImage::BMPImage(int32_t width, int32_t height) {
    if (width > 0 && height > 0) {
        img.width = width;
        img.height = height;
        img.channels = 3;
        img.data.assign(static_cast<size_t>(width) * static_cast<size_t>(height) * 3, 0);
    }
}

BMPImage::BMPImage(const std::string& file_path) {
    read(file_path);
}

bool BMPImage::read(const std::string& file_path) {
    return BMPread(file_path, img);
}

bool BMPImage::save(const std::string& file_path) const {
    return BMPsave(file_path, img);
}

void BMPImage::change_pixel(int32_t column, int32_t row, uint8_t blue, uint8_t green, uint8_t red) {
    ::change_pixel(img, column, row, blue, green, red);
}

int32_t BMPImage::get_width() const {
    return img.width;
}

int32_t BMPImage::get_height() const {
    return img.height;
}