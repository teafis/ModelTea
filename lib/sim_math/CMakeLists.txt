# SPDX-License-Identifier: GPL-3.0-only
# CMakeList.txt : CMake project for TeaFIS MapGenerator
cmake_minimum_required (VERSION 3.18)

set(VCPKG_FEATURE_FLAGS "versions")

# Define the project information
project (
    TeaFIS-SimMath
    VERSION 0.1
    LANGUAGES CXX
)

# Define the C++ Standard Required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(
    TEAFIS_SIM_MATH_SOURCES
    src/sim_math/bus.hpp
    src/sim_math/codegen_state.hpp
    src/sim_math/data_types.hpp
    src/sim_math/parameter.hpp
    src/sim_math/parameter.cpp
    src/sim_math/signal.hpp
    src/sim_math/signal.cpp
    src/sim_math/blocks/base_block.hpp
    src/sim_math/blocks/block_exception.hpp
    src/sim_math/blocks/constant.hpp
    src/sim_math/blocks/delay.hpp
    src/sim_math/blocks/integrator.hpp
    src/sim_math/blocks/limiter.hpp
    src/sim_math/blocks/logical_operation.hpp
    src/sim_math/blocks/math_operation.hpp
)

# Define the library
add_library(
    TeaFIS-SimMath
    ${TEAFIS_SIM_MATH_SOURCES}
)

# Include Directories
target_include_directories(TeaFIS-SimMath PUBLIC src)

# Add compiler options
if (MSVC)
    target_compile_options(TeaFIS-SimMath PUBLIC /W4 /WX)
else()
    target_compile_options(TeaFIS-SimMath PUBLIC -Wall -Wextra -pedantic -Werror)

    if (TEAFIS_USE_ASAN)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()

    if (TEAFIS_USE_LSAN)
        set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()
endif()
