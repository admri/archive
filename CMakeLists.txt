cmake_minimum_required(VERSION 3.16)
project(archiver_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directories
include_directories(include)

# Add external dependencies (zlib)
add_subdirectory(external/zlib)

# Add the archiver library
add_library(archiver STATIC
    src/archiver.c
    src/archive_internal.c
    src/archive_format.c
    src/archive_file.c
    src/file.c
    src/unarchiver.c
)

# Link zlib to archiver
target_link_libraries(archiver PRIVATE zlibstatic)  # or zlib depending on your zlib build

# Add executable
add_executable(archiver_main tools/archiver_main.c)

# Link executable to archiver library
target_link_libraries(archiver_main PRIVATE archiver)
