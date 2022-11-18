# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

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
    TEAFIS_MODEL_SOURCES
    src/tmdl/block.hpp src/tmdl/block.cpp
    src/tmdl/block_interface.hpp src/tmdl/block_interface.cpp
    src/tmdl/connection.hpp src/tmdl/connection.cpp
    src/tmdl/io_ports.hpp src/tmdl/io_ports.cpp
    src/tmdl/library.hpp
    src/tmdl/model.hpp src/tmdl/model.cpp
    src/tmdl/model_exception.hpp src/tmdl/model_exception.cpp
    src/tmdl/parameter.hpp src/tmdl/parameter.cpp
    src/tmdl/sim_state.hpp
    src/tmdl/value.hpp
    src/tmdl/value.cpp
    src/tmdl/stdlib/tmdl_stdlib.hpp src/tmdl/stdlib/tmdl_stdlib.cpp
    src/tmdl/stdlib/clock.hpp src/tmdl/stdlib/clock.cpp
    src/tmdl/stdlib/limiter.hpp src/tmdl/stdlib/limiter.cpp
    src/tmdl/stdlib/trig.hpp src/tmdl/stdlib/trig.cpp
)

# Define the library
add_library(
    TeaFIS-Model
    ${TEAFIS_MODEL_SOURCES}
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
