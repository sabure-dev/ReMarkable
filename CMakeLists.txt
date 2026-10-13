cmake_minimum_required(VERSION 3.10)
project(ReMarkable CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(BMP-info)

add_executable(remarkable
    main.cpp
)

enable_testing()

add_test(NAME CLIHelpCheck COMMAND remarkable --help)

add_executable(bmp_tests tests/test_bmp.cpp)
add_test(NAME BMPUnitTests COMMAND bmp_tests)