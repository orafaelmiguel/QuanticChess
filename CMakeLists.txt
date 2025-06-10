cmake_minimum_required(VERSION 3.16)

# Define the project
project(QuantumChess
    VERSION 1.0.0
    DESCRIPTION "A quantum chess game implemented in C++"
    LANGUAGES CXX
)

# Configures the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Build flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Includes directories
include(FetchContent)

FetchContent_Declare(
    klein
    GIT_REPOSITORY https://github.com/jeremyong/klein.git
    GIT_TAG v2.2.0
)

FetchContent_MakeAvailable(klein)

include_directories(include)

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

# Creates the main executable
add_executable(quantum_chess ${SOURCES} ${HEADERS})

# Specific configurations for the executable
target_include_directories(quantum_chess PRIVATE include)
target_link_libraries(quantum_chess PRIVATE klein::klein)

# Enable tests if requested
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation information
install(TARGETS quantum_chess 
    RUNTIME DESTINATION bin
)

# Additional debugging configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(quantum_chess PRIVATE DEBUG_MODE=1)
endif() 