cmake_minimum_required(VERSION 3.10)
project(ReMarkable CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(BMP-info)
include_directories(Watermark)

set(BMP_SOURCES
    BMP-info/BMPReader.cpp
    BMP-info/BMPEditor.cpp
    BMP-info/BMPClass.cpp
)

set(WATERMARK_SOURCES
    Watermark/overlay.cpp
)

add_executable(remarkable
    main.cpp
    ${BMP_SOURCES}
    ${WATERMARK_SOURCES}
)

enable_testing()
add_test(NAME CLIHelpCheck COMMAND remarkable --help)

add_executable(bmp_tests tests/test_bmp.cpp ${BMP_SOURCES} ${WATERMARK_SOURCES})
add_test(NAME BMPUnitTests COMMAND bmp_tests)