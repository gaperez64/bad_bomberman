cmake_minimum_required(VERSION 3.16)
project(Bomberman VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy / IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(logic)
add_subdirectory(representation)
add_subdirectory(tests)

find_package(X11 REQUIRED)
add_executable(bomberman src/main.cpp)
target_link_libraries(bomberman PRIVATE bomberman-rep X11)
target_compile_options(bomberman PRIVATE -Wall -Wextra -Wpedantic)
