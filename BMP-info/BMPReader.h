#pragma once

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "BMPHeader.h"

struct Image {
    int32_t width{0};
    int32_t height{0};
    int32_t channels{3};
    std::vector<uint8_t> data; // Pixel data in top-down order (BGR)
};

inline bool BMPread(const std::string& file_path, Image& img) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file for reading: " << file_path << '\n';
        return false;
    }

    BMPFileHeader file_header;
    if (!file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header))) {
        std::cerr << "Error: failed to read BMP file header from: " << file_path << '\n';
        return false;
    }

    if (file_header.file_type != 0x4D42) {
        std::cerr << "Error: invalid BMP signature in file: " << file_path << '\n';
        return false;
    }

    BMPInfoHeader info_header;
    if (!file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header))) {
        std::cerr << "Error: failed to read BMP info header from: " << file_path << '\n';
        return false;
    }

    if (info_header.bit_count != 24 || info_header.compression != 0) {
        std::cerr << "Error: only uncompressed 24-bit BMP files are supported (" << file_path << ")\n";
        return false;
    }

    if (info_header.size < sizeof(BMPInfoHeader) || info_header.width <= 0 || info_header.height == 0) {
        std::cerr << "Error: invalid BMP dimensions or header size in: " << file_path << '\n';
        return false;
    }

    const int32_t width = info_header.width;
    const int32_t height = std::abs(info_header.height);
    const bool is_bottom_up = (info_header.height > 0);
    const int32_t channels = 3;
    const int32_t padding = (4 - (width * channels) % 4) % 4;

    // Move stream position directly to the start of pixel data (bfOffBits)
    file.seekg(file_header.offset_data, std::ios::beg);
    if (!file) {
        std::cerr << "Error: invalid pixel data offset in: " << file_path << '\n';
        return false;
    }

    img.width = width;
    img.height = height;
    img.channels = channels;
    img.data.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * channels);

    const std::streamsize row_bytes = static_cast<std::streamsize>(width) * channels;

    for (int32_t y = 0; y < height; ++y) {
        const int32_t target_row = is_bottom_up ? (height - 1 - y) : y;
        const size_t row_offset = static_cast<size_t>(target_row) * static_cast<size_t>(width) * channels;

        if (!file.read(reinterpret_cast<char*>(&img.data[row_offset]), row_bytes)) {
            std::cerr << "Error: unexpected end of file while reading pixels from: " << file_path << '\n';
            return false;
        }

        if (padding > 0) {
            file.ignore(padding);
        }
    }

    return true;
}

inline bool BMPsave(const std::string& file_path, const Image& img) {
    if (img.width <= 0 || img.height <= 0 || img.channels != 3) {
        std::cerr << "Error: cannot save empty or invalid image to: " << file_path << '\n';
        return false;
    }

    const size_t expected_size = static_cast<size_t>(img.width) * static_cast<size_t>(img.height) * img.channels;
    if (img.data.size() < expected_size) {
        std::cerr << "Error: pixel buffer size is smaller than image dimensions\n";
        return false;
    }

    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file for writing: " << file_path << '\n';
        return false;
    }

    const int32_t padding = (4 - (img.width * img.channels) % 4) % 4;
    const uint32_t row_stride = static_cast<uint32_t>(img.width * img.channels + padding);
    const uint32_t image_size = row_stride * static_cast<uint32_t>(img.height);

    BMPFileHeader file_header;
    file_header.file_type = 0x4D42; // "BM"
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    file_header.file_size = file_header.offset_data + image_size;

    BMPInfoHeader info_header;
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = img.width;
    info_header.height = img.height; // Positive height -> standard bottom-up storage
    info_header.planes = 1;
    info_header.bit_count = 24;
    info_header.compression = 0;
    info_header.size_image = image_size;

    file.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    file.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    const uint8_t pad_bytes[3] = {0, 0, 0};
    const std::streamsize row_bytes = static_cast<std::streamsize>(img.width) * img.channels;

    for (int32_t y = img.height - 1; y >= 0; --y) {
        const size_t row_offset = static_cast<size_t>(y) * static_cast<size_t>(img.width) * img.channels;
        file.write(reinterpret_cast<const char*>(&img.data[row_offset]), row_bytes);
        if (padding > 0) {
            file.write(reinterpret_cast<const char*>(pad_bytes), padding);
        }
    }

    if (!file.good()) {
        std::cerr << "Error: failed while writing data to: " << file_path << '\n';
        return false;
    }

    return true;
}