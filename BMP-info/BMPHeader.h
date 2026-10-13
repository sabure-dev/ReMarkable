#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t file_type{0x4D42}; // Signature "BM"
    uint32_t file_size{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offset_data{0};    // Offset to the pixel array (bfOffBits)
};

struct BMPInfoHeader {
    uint32_t size{0};           // Header size in bytes (at least 40)
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{1};         // Number of color planes (must be 1)
    uint16_t bit_count{0};      // Bits per pixel (24 for uncompressed BGR)
    uint32_t compression{0};    // Compression type (0 = BI_RGB)
    uint32_t size_image{0};
    int32_t  x_pixels_per_meter{0};
    int32_t  y_pixels_per_meter{0};
    uint32_t colors_used{0};
    uint32_t colors_important{0};
};
#pragma pack(pop)