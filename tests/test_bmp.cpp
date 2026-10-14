#include <cassert>
#include <iostream>
#include <cstdio>
#include "BMPClass.h"
#include "overlay.h"

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

// тесты к функции overlayBMPFiles 
// Допуск в 1 единицу цвета: на округление (+0.5 или обрезка) не завязываемся
static bool near(int actual, int expected) {
    return std::abs(actual - expected) <= 1;
}

// kAlpha = 0.4: результат = исходник * 0.6 + знак * 0.4
void test_overlay_blend_values() {
    BMPImage base(2, 2);
    BMPImage mark(1, 1);
    base.change_pixel(0, 0, 100, 100, 100);
    mark.change_pixel(0, 0, 200, 0, 50);

    BMPImage out = overlayBMPFiles(base, mark);

    assert(near(out.img.data[0], 140));  // 100*0.6 + 200*0.4
    assert(near(out.img.data[1], 60));   // 100*0.6 + 0*0.4
    assert(near(out.img.data[2], 80));   // 100*0.6 + 50*0.4
    std::cout << "[PASS] test_overlay_blend_values\n";
}

void test_overlay_outside_mark_unchanged() {
    BMPImage base(3, 3);
    BMPImage mark(1, 1);
    base.change_pixel(2, 2, 10, 20, 30);
    base.change_pixel(1, 0, 40, 50, 60);
    mark.change_pixel(0, 0, 255, 255, 255);

    BMPImage out = overlayBMPFiles(base, mark);

    size_t i_22 = (2 * 3 + 2) * 3;
    assert(out.img.data[i_22]     == 10);
    assert(out.img.data[i_22 + 1] == 20);
    assert(out.img.data[i_22 + 2] == 30);

    size_t i_10 = (0 * 3 + 1) * 3;
    assert(out.img.data[i_10]     == 40);
    assert(out.img.data[i_10 + 1] == 50);
    assert(out.img.data[i_10 + 2] == 60);
    std::cout << "[PASS] test_overlay_outside_mark_unchanged\n";
}

void test_overlay_does_not_modify_inputs() {
    BMPImage base(2, 2);
    BMPImage mark(2, 2);
    base.change_pixel(0, 0, 100, 100, 100);
    mark.change_pixel(0, 0, 200, 200, 200);

    BMPImage out = overlayBMPFiles(base, mark);

    assert(base.img.data[0] == 100);
    assert(mark.img.data[0] == 200);
    assert(out.get_width() == 2 && out.get_height() == 2);
    std::cout << "[PASS] test_overlay_does_not_modify_inputs\n";
}

void test_overlay_mark_bigger_than_base() {
    BMPImage base(2, 2);
    BMPImage mark(5, 5);
    base.change_pixel(1, 1, 100, 100, 100);
    mark.change_pixel(1, 1, 200, 200, 200);

    BMPImage out = overlayBMPFiles(base, mark);

    assert(out.get_width() == 2);
    assert(out.get_height() == 2);
    size_t i = (1 * 2 + 1) * 3;
    assert(near(out.img.data[i], 140));
    std::cout << "[PASS] test_overlay_mark_bigger_than_base\n";
}

void test_overlay_different_widths() {
    BMPImage base(4, 3);
    BMPImage mark(2, 2);
    // знак: пиксель (0, 1) = 200, остальные 0
    mark.change_pixel(0, 1, 200, 200, 200);

    BMPImage out = overlayBMPFiles(base, mark);

    size_t i_01 = (1 * 4 + 0) * 3;       // (x=0, y=1) в основной шириной 4
    assert(near(out.img.data[i_01], 80)); // 0*0.6 + 200*0.4
    size_t i_10 = (0 * 4 + 1) * 3;       // (x=1, y=0): знак там 0
    assert(near(out.img.data[i_10], 0));
    std::cout << "[PASS] test_overlay_different_widths\n";
}

int main() {
    test_padding_roundtrip();
    test_orientation();
    test_non_existent_file();

    test_overlay_blend_values();
    test_overlay_outside_mark_unchanged();
    test_overlay_does_not_modify_inputs();
    test_overlay_mark_bigger_than_base();
    test_overlay_different_widths();
    std::cout << "All BMP unit tests passed successfully!\n";
    return 0;
}