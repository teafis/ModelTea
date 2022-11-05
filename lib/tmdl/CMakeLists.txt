# SPDX-License-Identifier: GPL-3.0-only
# CMakeList.txt : CMake project for TeaFIS MapGenerator
cmake_minimum_required (VERSION 3.18)

set(VCPKG_FEATURE_FLAGS "versions")

# Define the project information
project (
    TeaFIS-Model
    VERSION 0.1
    LANGUAGES CXX
)

# Define the C++ Standard Required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(
    TEAFIS_SIM_MATH_SOURCES
    src/tmdl/block.hpp
    src/tmdl/connection.hpp
    src/tmdl/model.hpp
    src/tmdl/parameter.hpp
    src/tmdl/value.hpp
)

# Define the library
add_library(
    TeaFIS-Model
    ${TEAFIS_SIM_MATH_SOURCES}
)

# Include Directories
target_include_directories(TeaFIS-Model PUBLIC src)

# Add compiler options
if (MSVC)
    target_compile_options(TeaFIS-Model PUBLIC /W4 /WX)
else()
    target_compile_options(TeaFIS-Model PUBLIC -Wall -Wextra -pedantic -Werror)

    if (TEAFIS_USE_ASAN)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()

    if (TEAFIS_USE_LSAN)
        set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()
endif()
