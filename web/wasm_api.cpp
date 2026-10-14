// Thin bridge between the browser and the existing C++ code.
// The page writes the uploaded BMP bytes into Emscripten's in-memory
// filesystem, calls apply_watermark() with three paths, and reads the
// result file back.

#include <emscripten/emscripten.h>

#include "BMPClass.h"
#include "overlay.h"

extern "C" {

// Return codes: 0 = ok, 1 = base unreadable, 2 = watermark unreadable, 3 = save failed.
EMSCRIPTEN_KEEPALIVE
int apply_watermark(const char* base_path, const char* mark_path, const char* out_path) {
    BMPImage base;
    if (!base.read(base_path)) {
        return 1;
    }

    BMPImage mark;
    if (!mark.read(mark_path)) {
        return 2;
    }

    const BMPImage result = overlayBMPFiles(base, mark);
    return result.save(out_path) ? 0 : 3;
}

}  // extern "C"