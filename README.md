# "ReMarkable" — BMP Watermark Project

[![C++ CI](https://github.com/sabure-dev/ReMarkable/actions/workflows/ci.yml/badge.svg)](https://github.com/sabure-dev/ReMarkable/actions/workflows/ci.yml)

A command-line utility written in C++17 that overlays a digital watermark (a second BMP image) onto a base BMP image and saves the result as a new BMP file. The BMP format is parsed and written by hand — no external image libraries are used.

Team project built with Git and GitHub: shared repository, feature branches, code review and automatic builds with GitHub Actions.

## Table of Contents

- ["ReMarkable" — BMP Watermark Project](#remarkable--bmp-watermark-project)
  - [Table of Contents](#table-of-contents)
  - [Project Team](#project-team)
  - [Technologies](#technologies)
  - [Project Structure](#project-structure)
  - [Build Instructions](#build-instructions)
    - [Prerequisites](#prerequisites)
    - [Build Steps](#build-steps)
  - [Usage](#usage)
    - [Command Syntax](#command-syntax)
    - [Examples](#examples)
  - [How It Works](#how-it-works)
    - [BMP format](#bmp-format)
    - [Blending algorithm](#blending-algorithm)
    - [Watermark placement](#watermark-placement)
  - [Input Validation and Error Handling](#input-validation-and-error-handling)
  - [Testing](#testing)
  - [Results](#results)
  - [Web Demo](#web-demo)
  - [BMP Module API Example](#bmp-module-api-example)
  - [Possible Improvements](#possible-improvements)

---

## Project Team

| Member | Role |
|--------|------|
| **Boris Paramonov** | Student 1 — BMP classes: reading and writing 24-bit BMP files |
| **Ivan Ovsyannikov** | Student 2 — watermarking (pixel blending) algorithm |
| **Bulat Sadreev** | Student 3 — integration (`main`), CMake, CI/CD, input validation, documentation |

## Technologies

- C++17 (`std::ifstream` / `std::ofstream` in binary mode, `<filesystem>`)
- CMake + CTest
- Git / GitHub (branches, pull requests, code review)
- GitHub Actions (build + tests on every push and pull request)
- Emscripten / WebAssembly + GitHub Pages (optional web UI)

## Project Structure

```text
.
├── main.cpp                 # CLI: argument parsing, validation, pipeline
├── BMP-info/                # BMP reading/writing
│   ├── BMPHeader.h          #   BITMAPFILEHEADER / BITMAPINFOHEADER structs
│   ├── BMPReader.{h,cpp}    #   low-level read/save (padding, bottom-up order)
│   ├── BMPEditor.{h,cpp}    #   pixel editing helpers
│   └── BMPClass.{h,cpp}     #   BMPImage class (public API)
├── Watermark/
│   └── overlay.{h,cpp}      # overlayBMPFiles(): pixel blending
├── tests/test_bmp.cpp       # unit tests
├── examples/                # sample inputs and outputs
├── docs/                    # PNG previews used in this README
├── web/                     # browser UI (WebAssembly)
├── CMakeLists.txt
└── .github/workflows/       # ci.yml (build + tests), pages.yml (web demo)
```

---

## Build Instructions

### Prerequisites

- CMake 3.10+
- A C++17 compatible compiler (GCC, Clang or MSVC)

### Build Steps

```bash
cmake -B build -S .
cmake --build build --config Release
```

The binary `remarkable` (`remarkable.exe` on Windows) is placed in the `build/` directory (`build/Release/` for multi-config generators such as Visual Studio).

---

## Usage

### Command Syntax

```bash
./build/remarkable <input.bmp> <watermark.bmp> <output.bmp>
```

| Parameter | Description |
|-----------|-------------|
| `<input.bmp>` | Base image: 24-bit uncompressed BMP. |
| `<watermark.bmp>` | Watermark image: 24-bit uncompressed BMP. |
| `<output.bmp>` | Where to save the result. Must differ from both input files. |

Flags: `--help` / `-h` — print usage and exit.

### Examples

```bash
# Show help
./build/remarkable --help

# Small watermark (200x200) on a 1000x1000 image
./build/remarkable examples/case1/source1000x1000.bmp \
                   examples/case1/watermark200x200.bmp \
                   result.bmp
```

Expected console output:

```text
Loading base image: examples/case1/source1000x1000.bmp...
Loading watermark: examples/case1/watermark200x200.bmp...
Base image: 1000x1000, watermark: 200x200
Applying watermark...
Saving result as: result.bmp...
Done.
```

The exit code is `0` on success and `1` on any error.

---

## How It Works

### BMP format

Only **uncompressed 24-bit BMP** is supported. A file consists of a 14-byte file header, an info header (at least 40 bytes) and the pixel array:

- each pixel is 3 bytes in **B, G, R** order;
- rows are stored **bottom-up** when the header height is positive; the reader converts them to top-down order in memory, so the picture never ends up flipped (negative height, i.e. already top-down files, is handled too);
- every row is padded with zero bytes to a multiple of 4 bytes — the padding is skipped on reading and added on writing;
- the start of the pixel data is taken from the `bfOffBits` field, so files with extended headers (e.g. BITMAPV5HEADER) are read correctly. The output is always saved with a standard 54-byte header.

### Blending algorithm

For every pixel of the overlapped area:

```text
result = source * (1 - alpha) + watermark * alpha        alpha = 0.4
```

applied independently to the B, G and R channels. With `alpha = 0.4` the watermark is clearly visible, while the original image stays readable (60 % of the original colour is kept). The arithmetic is done in floating point, so there is no 8-bit overflow, and the value is cast back to `uint8_t` at the end.

### Watermark placement

The watermark is placed in the **top-left corner** of the base image, pixel to pixel:

| Case | Behaviour |
|------|-----------|
| Watermark **smaller** than the base | Only the top-left rectangle of the base image is blended; the rest is unchanged. |
| Sizes **equal** | The whole image is blended. |
| Watermark **larger** than the base | A warning is printed; only the intersection (the top-left part of the watermark) is used. The output has the size of the base image. |

The inputs are never modified: the result is a new image.

---

## Input Validation and Error Handling

All command-line paths are checked **before** any image is loaded. On a problem the program prints a message to `stderr` and returns exit code `1`; it never crashes.

| Situation | Behaviour |
|-----------|-----------|
| Wrong number of arguments | Error + usage hint |
| Empty path | Error |
| Base image / watermark missing or not a regular file | Error naming the file |
| Output path equals the base image or the watermark (same file, even via different path spelling) | Error — protects the originals from being overwritten |
| Output path is a directory, or its directory does not exist | Error |
| Output has no `.bmp` extension | Warning, the file is still written as BMP |
| Not a BMP (wrong signature), empty or truncated file | Error from the BMP reader |
| Not 24-bit or compressed BMP | Error from the BMP reader |
| Watermark larger than the base image | Warning, watermark is cropped |
| Out-of-memory / unexpected exception (e.g. corrupted header with huge dimensions) | Error message, exit code `1` |

---

## Testing

Run the unit tests and the CLI check with CTest:

```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build --output-on-failure
```

| Test | What it checks |
|------|----------------|
| `CLIHelpCheck` | `remarkable --help` runs and exits with code 0 |
| `BMPUnitTests` → `test_padding_roundtrip` | Write → read of a 3x3 image (row padding) keeps the pixels |
| `BMPUnitTests` → `test_orientation` | The image is not flipped after a save/load round trip |
| `BMPUnitTests` → `test_non_existent_file` | Reading a missing file fails gracefully |
| `BMPUnitTests` → `test_overlay_blend_values` | Blended colours match `0.6 * source + 0.4 * mark` |
| `BMPUnitTests` → `test_overlay_outside_mark_unchanged` | Pixels outside the watermark are untouched |
| `BMPUnitTests` → `test_overlay_does_not_modify_inputs` | Input images are not modified |
| `BMPUnitTests` → `test_overlay_mark_bigger_than_base` | Oversized watermark is cropped, output size = base size |
| `BMPUnitTests` → `test_overlay_different_widths` | Correct indexing when image widths differ |

On every push and pull request to `main`, GitHub Actions configures, builds and runs these tests (see `.github/workflows/ci.yml`).

Manual check: open the result in any image viewer — the watermark must be visible in the top-left corner and the original image must stay recognisable.

---

## Results

The watermark is cropped to the size of the base image; the output keeps the base image dimensions.

Full-size BMP files for all cases (`case1`, `case2`, `case3` with equal sizes) are in [`examples/`](examples/). The PNG files in `docs/` are downscaled previews for this README.

---

## Web Demo

The same C++ code is compiled to WebAssembly (Emscripten) and published with GitHub Pages (`.github/workflows/pages.yml`). The page in `web/` lets you pick two BMP files in the browser and download the result. All processing happens locally in the browser.

---

## BMP Module API Example

```cpp
#include <iostream>
#include "BMPClass.h"
#include "overlay.h"

int main() {
    BMPImage base, mark;
    if (!base.read("examples/example.bmp") || !mark.read("examples/case1/watermark200x200.bmp")) {
        std::cerr << "Failed to read images\n";
        return 1;
    }

    base.change_pixel(100, 100, 20, 20, 20);

    BMPImage result = overlayBMPFiles(base, mark);
    return result.save("output.bmp") ? 0 : 1;
}
```

---

## Possible Improvements

- Configurable opacity (`alpha`) and watermark position (corner / centre / tiling) via CLI options.
- Support for 8-, 32-bit and compressed BMP, or other formats (PNG, JPEG).
- Transparency key colour, so a watermark with a solid background does not tint the whole rectangle.
- Larger automated test suite: golden-image comparison with `expected.bmp` in CI, tests for corrupted files.
