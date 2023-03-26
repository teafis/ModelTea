# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

set(VCPKG_FEATURE_FLAGS "versions")

# Define the project information
project (
    tmdlstd
    VERSION 0.1
    LANGUAGES CXX
)

set(
    TEAFIS_STD_MODEL_SOURCES
    src/tmdlstd/arith_block.hpp
)

# Define the library
add_library(
    tmdlstd
    ${TEAFIS_STD_MODEL_SOURCES}
)

# Define the C++ Standard Required
set_property(TARGET tmdlstd PROPERTY CXX_STANDARD 20)
set_property(TARGET tmdlstd PROPERTY CXX_STANDARD_REQUIRED ON)

# Define libraries
target_link_libraries(tmdlstd PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(tmdlstd PUBLIC fmt::fmt)

# Include Directories
target_include_directories(tmdlstd PUBLIC src)

# Add compiler options
if (MSVC)
    target_compile_options(tmdlstd PUBLIC /W4 /WX)
else()
    target_compile_options(tmdlstd PUBLIC -Wall -Wextra -pedantic -Werror)

    if (TEAFIS_USE_ASAN)
        set_property(TARGET tmdlstd PROPERTY CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()

    if (TEAFIS_USE_LSAN)
        set_property(TARGET tmdlstd PROPERTY CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()
endif()
