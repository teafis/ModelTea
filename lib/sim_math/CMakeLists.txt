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
    TEAFIS_MAPPING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_bus.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_codegen_state.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_constant.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_gain.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_integrator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_limiter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_logical_operation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_math_operation.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_signal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sim_math/sim_types.hpp
)

# Define the library
add_library(
    TeaFIS-SimMath
    ${TEAFIS_MAPPING_SOURCES}
)

# Include Directories
target_include_directories(TeaFIS-SimMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

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
