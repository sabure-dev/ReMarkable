#include <cassert>
#include <iostream>
#include <cstdio>
#include "BMPClass.h"

void test_padding_roundtrip() {
    BMPImage original(3, 3);
    
    original.change_pixel(0, 0, 255, 0, 0);
    original.change_pixel(2, 2, 0, 255, 0);
    original.change_pixel(1, 1, 0, 0, 255);

    const std::string filename = "test_padding_temp.bmp";
    assert(original.save(filename) && "Failed to save BMP with padding");

    BMPImage loaded;
    assert(loaded.read(filename) && "Failed to load BMP with padding");
    assert(loaded.get_width() == 3);
    assert(loaded.get_height() == 3);

    assert(loaded.img.data[0] == 255);
    assert(loaded.img.data[1] == 0);
    assert(loaded.img.data[2] == 0);

    size_t idx_2_2 = (2 * 3 + 2) * 3;
    assert(loaded.img.data[idx_2_2] == 0);
    assert(loaded.img.data[idx_2_2 + 1] == 255);
    assert(loaded.img.data[idx_2_2 + 2] == 0);

    std::remove(filename.c_str());
    std::cout << "[PASS] test_padding_roundtrip\n";
}

void test_orientation() {
    BMPImage img(2, 2);
    img.change_pixel(0, 0, 10, 20, 30);
    
    const std::string filename = "test_orient_temp.bmp";
    assert(img.save(filename));

    BMPImage loaded;
    assert(loaded.read(filename));

    assert(loaded.img.data[0] == 10);
    assert(loaded.img.data[1] == 20);
    assert(loaded.img.data[2] == 30);

    std::remove(filename.c_str());
    std::cout << "[PASS] test_orientation\n";
}

void test_non_existent_file() {
    BMPImage img;
    assert(!img.read("non_existent_image_12345.bmp") && "Read should fail for non-existent file");
    std::cout << "[PASS] test_non_existent_file\n";
}

int main() {
    test_padding_roundtrip();
    test_orientation();
    test_non_existent_file();
    std::cout << "All BMP unit tests passed successfully!\n";
    return 0;
}