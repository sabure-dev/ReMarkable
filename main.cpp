#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "BMPClass.h"
#include "overlay.h"

namespace fs = std::filesystem;

namespace {

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name
              << " <input.bmp> <watermark.bmp> <output.bmp>\n"
              << "       " << prog_name << " --help\n";
}

bool is_same_file(const fs::path& a, const fs::path& b) {
    std::error_code ec;
    return fs::exists(a, ec) && fs::exists(b, ec) && fs::equivalent(a, b, ec);
}

bool validate_paths(const std::string& input, const std::string& watermark,
                    const std::string& output) {
    std::error_code ec;

    if (input.empty() || watermark.empty() || output.empty()) {
        std::cerr << "Error: file paths must not be empty.\n";
        return false;
    }
    if (!fs::is_regular_file(input, ec)) {
        std::cerr << "Error: base image not found (or not a regular file): "
                  << input << '\n';
        return false;
    }
    if (!fs::is_regular_file(watermark, ec)) {
        std::cerr << "Error: watermark file not found (or not a regular file): "
                  << watermark << '\n';
        return false;
    }
    if (is_same_file(output, input)) {
        std::cerr << "Error: output path must differ from the base image path "
                     "(the original would be overwritten).\n";
        return false;
    }
    if (is_same_file(output, watermark)) {
        std::cerr << "Error: output path must differ from the watermark path "
                     "(the watermark would be overwritten).\n";
        return false;
    }
    if (fs::is_directory(output, ec)) {
        std::cerr << "Error: output path is a directory: " << output << '\n';
        return false;
    }
    const fs::path out_dir = fs::path(output).parent_path();
    if (!out_dir.empty() && !fs::is_directory(out_dir, ec)) {
        std::cerr << "Error: output directory does not exist: "
                  << out_dir.string() << '\n';
        return false;
    }
    return true;
}

void warn_if_not_bmp_extension(const std::string& output) {
    std::string ext = fs::path(output).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (ext != ".bmp") {
        std::cerr << "Warning: output file does not have a .bmp extension, "
                     "but it will be written in BMP format.\n";
    }
}

int run(const std::string& input_path, const std::string& watermark_path,
        const std::string& output_path) {
    if (!validate_paths(input_path, watermark_path, output_path)) {
        return 1;
    }
    warn_if_not_bmp_extension(output_path);

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

    std::cout << "Base image: " << base_img.get_width() << "x" << base_img.get_height()
              << ", watermark: " << watermark_img.get_width() << "x"
              << watermark_img.get_height() << '\n';

    if (watermark_img.get_width() > base_img.get_width() ||
        watermark_img.get_height() > base_img.get_height()) {
        std::cerr << "Warning: watermark is larger than the base image "
                     "and will be cropped.\n";
    }

    std::cout << "Applying watermark...\n";
    const BMPImage result_img = overlayBMPFiles(base_img, watermark_img);

    std::cout << "Saving result as: " << output_path << "...\n";
    if (!result_img.save(output_path)) {
        return 1;
    }

    std::cout << "Done.\n";
    return 0;
}

}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        const std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        }
    }

    if (argc != 4) {
        std::cerr << "Error: expected exactly 3 arguments, got " << (argc - 1) << ".\n";
        print_usage(argv[0]);
        return 1;
    }

    try {
        return run(argv[1], argv[2], argv[3]);
    } catch (const std::bad_alloc&) {
        std::cerr << "Error: not enough memory (image dimensions are too large or the file is corrupted).\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: unexpected failure: " << e.what() << '\n';
    }
    return 1;
}