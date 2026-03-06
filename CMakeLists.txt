cmake_minimum_required(VERSION 3.16)
project(Windminer VERSION 1.0.0 LANGUAGES CXX)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build Options
option(BUILD_RUST "Build Rust integration modules" OFF)
option(BUILD_ALN "Build ALN-Blockchain integration" OFF)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Compiler Warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
endif()

# Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src/cpp/include
    ${CMAKE_SOURCE_DIR}/src/aln/include
)

# Core Library
add_library(windminer_core STATIC
    src/cpp/WindnetGeometry.cpp
    src/cpp/WindClimatePhoenix.cpp
    src/cpp/WindnetCaptureModel.cpp
    src/cpp/WindnetEcoImpact.cpp
    src/cpp/ShardIo.cpp
)

target_include_directories(windminer_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src/cpp/include
)

# Main Planning Tool
add_executable(windminer_plan
    src/cpp/MainPlanDemo.cpp
)

target_link_libraries(windminer_plan PRIVATE windminer_core)

# Optional Rust Integration
if(BUILD_RUST)
    find_package(Rust REQUIRED)
    add_subdirectory(src/rust)
endif()

# Optional ALN-Blockchain Integration
if(BUILD_ALN)
    add_subdirectory(src/aln)
endif()

# Test Suite
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS windminer_plan DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/windminer/data)
install(DIRECTORY docs/ DESTINATION share/windminer/docs)

# Print Configuration Summary
message(STATUS "Windminer Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Rust: ${BUILD_RUST}")
message(STATUS "  Build ALN: ${BUILD_ALN}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
