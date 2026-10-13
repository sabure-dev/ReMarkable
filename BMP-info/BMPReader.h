#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>
#include <vector>
#include <fstream>
#include "../BMP-info/BMPHeader.h"

#pragma once
struct Image{
    int32_t width{0};
    int32_t height{0};
    int32_t channels{0};
    std::vector<uint8_t> data; // pixels data
};

/*
Usage:
    "filepath - file_path, image - Image img.data"
*/

bool BMPread(const std::string& file_path, Image& img){
    // open with binary format
    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open()){
        std::cerr << "err 1" << '\n';
        return false;
    }

    BMPFileHeader file_header;
    // reading file_header with casting to char format 
    file.read(reinterpret_cast<char*> (&file_header), sizeof(file_header));

    if (file_header.file_type != 0x4D42){
        std::cerr << "err 2" << '\n';
        return false;
    }

    BMPInfoHeader info_header;
    // reading info_header with casting to char format
    file.read(reinterpret_cast<char*> (&info_header), sizeof(info_header));

    if (info_header.bit_count != 24 || info_header.compression != 0){
        std::cerr << "err 3" << '\n';
        return false;
    }
    if (info_header.size == 0){
        std::cerr << "err 4" << "\n";
        return false;
    }

    int32_t width = info_header.width;
    int32_t height = std::abs(info_header.height);
    bool is_bottom = (info_header.height > 0);
    int32_t channels = 3;

    int32_t padding = (4-(width*channels)%4)%4;
    

    // saving bmp image in array format
    img.width = width;
    img.height = height;
    img.channels = channels;
    img.data.resize(width*height*channels);

    for (int32_t y = 0; y < height; y++){
        int32_t targ_row = is_bottom ? (height-1-y) : y;
        size_t row_offset = static_cast<size_t>(targ_row*width*channels);

        file.read(reinterpret_cast<char*>(&img.data[row_offset]), width*channels);
    
        if (padding > 0){
            file.ignore(padding);
        }
    }

    return true;
}

/*
Usage:
    "filepath - file_path, image - Image img.data"
*/

bool BMPsave(const std::string& file_path, const Image& img){

    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()){
        std::cerr << "err 5" << '\n';
        return false;
    }

    int32_t padding = (4 - (img.width * img.channels) % 4) % 4;
    uint32_t row_stride = img.width * img.channels + padding;
    uint32_t image_size = row_stride * img.height;


    // filling new headers
    BMPFileHeader file_header;
    file_header.file_type = 0x4D42; // "BM"
    file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    file_header.file_size = file_header.offset_data + image_size;

    BMPInfoHeader info_header;
    info_header.size = sizeof(BMPInfoHeader);
    info_header.width = img.width;
    info_header.height = img.height; 
    info_header.planes = 1;
    info_header.bit_count = 24;
    info_header.compression = 0;
    info_header.size_image = image_size;

    // writing header to the new file
    file.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    file.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    // adding padding
    const uint8_t pad_bytes[3] = {0, 0, 0};
    for (int y = img.height - 1; y >= 0; --y) {
        // offset to new image
        size_t row_offset = static_cast<size_t>(y * img.width * img.channels);
        file.write(reinterpret_cast<const char*>(&img.data[row_offset]), img.width * img.channels);
        if (padding > 0) {
            file.write(reinterpret_cast<const char*>(pad_bytes), padding);
        }
    }

    return true;
}