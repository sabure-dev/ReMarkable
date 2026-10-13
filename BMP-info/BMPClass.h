#pragma once

#include <cstdint>
#include <string>
#include "BMPEditor.h"
#include "BMPHeader.h"
#include "BMPReader.h"

class BMPImage {
public:
    Image img;

    BMPImage() = default;
    BMPImage(int32_t width, int32_t height);
    explicit BMPImage(const std::string& file_path);

    bool read(const std::string& file_path);
    bool save(const std::string& file_path) const;
    void change_pixel(int32_t column, int32_t row, uint8_t blue, uint8_t green, uint8_t red);

    int32_t get_width() const;
    int32_t get_height() const;
};