cmake_minimum_required(VERSION 3.16)
project(arch C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(ARCH_BUILD_TOOLS "Build archiver command-line tools" ON)

add_subdirectory(external/zlib)

file(GLOB_RECURSE ARCH_SOURCES
    src/*.c
)

file(GLOB CONFIGURE_DEPENDS ARCH_HEADERS
    include/arch/*.h
)

add_library(arch STATIC ${ARCH_SOURCES} ${ARCH_HEADERS})

target_include_directories(arch PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(arch PRIVATE zlibstatic)

if (ARCH_BUILD_TOOLS)
    file(GLOB ARCH_TOOL_SOURCES
        tools/*.c
    )

    add_executable(arch_cli ${ARCH_TOOL_SOURCES} ${ARCH_HEADERS})

    target_link_libraries(arch_cli PRIVATE arch)
endif()
