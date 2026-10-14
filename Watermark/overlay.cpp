#include <cstdint>
#include <algorithm>
#include "BMPClass.h"
#include "overlay.h"

namespace{
    const double kAlpha = 0.4;
    const int32_t kCntChannels = 3;

    std::size_t PixelIndex(int32_t file_width, int32_t row, int32_t column){
        std::size_t result = kCntChannels * (row * file_width + column);
        return result;
    }

    uint8_t Blend(uint8_t color_deg_source, uint8_t color_deg_watermark){
        uint8_t color_deg_result = color_deg_source * (1 - kAlpha) + color_deg_watermark * kAlpha;
        return color_deg_result;
    }
}

BMPImage overlayBMPFiles(const BMPImage& source_img, const BMPImage& watermark_img) {
    BMPImage output_img = source_img;

    const int32_t source_w = source_img.get_width();
    const int32_t watermark_w  = watermark_img.get_width();
    const int32_t overlay_height = std::min(source_img.get_height(), watermark_img.get_height());
    const int32_t overlay_width  = std::min(source_w, watermark_w);

    const auto& source  = source_img.img.data;
    const auto& mark = watermark_img.img.data;

    for (int32_t row = 0; row < overlay_height; ++row) {
        for (int32_t column = 0; column < overlay_width; ++column) {
            const int32_t i_source = PixelIndex(source_w, row, column);
            const int32_t i_watermark  = PixelIndex(watermark_w,  row, column);

            output_img.change_pixel(column, row,
                Blend(source[i_source],     mark[i_watermark]),       // blue
                Blend(source[i_source + 1], mark[i_watermark + 1]),   // green
                Blend(source[i_source + 2], mark[i_watermark + 2]));  // red
        }
    }
    return output_img;
}