# "ReMarkable" — BMP Watermark Project

[![C++ CI](https://github.com/sabure-dev/ReMarkable/actions/workflows/ci.yml/badge.svg)](https://github.com/sabure-dev/ReMarkable/actions/workflows/ci.yml)

A CLI utility written in C++17 to overlay a digital watermark image onto a base BMP image without using external image libraries.

## Project Team

- **Boris Paramonov** — Classes to work with BMP
- **Ivan Ovsyannikov** — Watermarking algorithm
- **Bulat Sadreev** — Integration, CMake, and CI/CD

---

## Build Instructions

### Prerequisites

- CMake 3.10+
- C++17 compatible compiler (GCC, Clang, or MSVC)

### Build Steps

```bash
cmake -B build -S .
cmake --build build --config Release
```

The compiled binary `remarkable` will be located in the `build/` directory.

---

## Usage

### Command Syntax

```bash
./build/remarkable <input.bmp> <watermark.bmp> <output.bmp>
```

### Arguments

| Parameter | Type | Description |
|-----------|------|-------------|
| `<input.bmp>` | Path | Path to the base 24-bit uncompressed BMP image. |
| `<watermark.bmp>` | Path | Path to the watermark 24-bit uncompressed BMP image. |
| `<output.bmp>` | Path | Destination path where the blended BMP image will be saved. |

### Flags

- `--help` — display usage instructions and exit.

### Execution Example

```bash
# Display help message
./build/remarkable --help

# Apply watermark to an image
./build/remarkable examples/original.bmp examples/watermark.bmp output.bmp
```

### Expected Output

During standard execution, the program outputs status messages for each pipeline stage:

```text
Loading base image: examples/original.bmp...
Loading watermark: examples/watermark.bmp...
Applying watermark...
Saving result as: output.bmp...
Done.
```

If an error occurs (e.g. missing file, invalid header signature, non-24-bit BMP), the program outputs an error message to `stderr` and exits with a non-zero status code:

```text
Error: cannot open file for reading: non_existent.bmp
```

---

## Testing

Run unit tests via `ctest`:

```bash
ctest --test-dir build --output-on-failure
```

---

## BMP Module Usage Example

The following example demonstrates how to use the underlying C++ API directly to load an image, modify a pixel, and save the result:

```cpp
#include <iostream>
#include "BMPClass.h"

int main() {
    BMPImage img;

    if (!img.read("example.bmp")) {
        std::cerr << "Failed to read image\n";
        return 1;
    }

    // Change pixel color at (column=100, row=100) to RGB(20, 20, 20)
    img.change_pixel(100, 100, 20, 20, 20);

    if (!img.save("output.bmp")) {
        std::cerr << "Failed to save image\n";
        return 1;
    }

    return 0;
}
```