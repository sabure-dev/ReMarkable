# "ReMarkable" - BMP Watermark Project

![C++ CI](https://github.com/sabure-dev/ReMarkable/actions/workflows/ci.yml/badge.svg)

## Project team:
- Boris Paramonov: classes to work with BMP
- Ivan Ovsyannikov: Watermarking algorithm 
- Bulat Sadreev: Integration, CMake and CI/CD

## BMP Module Usage Example

The following example demonstrates how to load an image, modify a pixel, and save the result:

```cpp
#include "BMPClass.h"
#include <iostream>

int main() {
    BMPClass img;

    if (!img.read("example.bmp")) {
        std::cerr << "Failed to read image\n";
        return 1;
    }

    // Change pixel color at (x=100, y=100) to RGB(20, 20, 20)
    img.change_pixel(100, 100, 20, 20, 20);

    if (!img.save("output.bmp")) {
        std::cerr << "Failed to save image\n";
        return 1;
    }

    return 0;
}