#include <iostream>
#include <string>
#include "BMPClass.h"

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name
              << " <input.bmp> <watermark.bmp> <output.bmp>\n";
}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        print_usage(argv[0]);
        return 0;
    }

    if (argc < 4) {
        std::cerr << "Error: insufficient arguments.\n";
        print_usage(argv[0]);
        return 1;
    }

    const std::string input_path = argv[1];
    const std::string watermark_path = argv[2];
    const std::string output_path = argv[3];

    std::cout << "Loading base image: " << input_path << "...\n";
    BMPImage base_img;
    if (!base_img.read(input_path)) {
        return 1;
    }

    std::cout << "Loading watermark: " << watermark_path << "...\n";
    BMPImage watermark_img;
    if (!watermark_img.read(watermark_path)) {
        return 1;
    }

    // TODO - Student 2

    std::cout << "Saving result as: " << output_path << "...\n";
    if (!base_img.save(output_path)) {
        return 1;
    }

    std::cout << "Done.\n";
    return 0;
}