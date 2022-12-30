# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

set(VCPKG_FEATURE_FLAGS "versions")

# Define the project information
project (
    tmdl
    VERSION 0.1
    LANGUAGES CXX
)

# Define the C++ Standard Required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(
    TEAFIS_MODEL_SOURCES
    src/tmdl/block_interface.hpp src/tmdl/block_interface.cpp
    src/tmdl/connection.hpp src/tmdl/connection.cpp
    src/tmdl/connection_manager.hpp src/tmdl/connection_manager.cpp
    src/tmdl/execution_state.hpp src/tmdl/execution_state.cpp
    src/tmdl/library.hpp
    src/tmdl/library_manager.hpp src/tmdl/library_manager.cpp
    src/tmdl/model.hpp src/tmdl/model.cpp
    src/tmdl/model_block.hpp src/tmdl/model_block.cpp
    src/tmdl/model_exception.hpp src/tmdl/model_exception.cpp
    src/tmdl/parameter.hpp src/tmdl/parameter.cpp
    src/tmdl/sim_state.hpp src/tmdl/sim_state.cpp
    src/tmdl/value.hpp src/tmdl/value.cpp
    src/tmdl/variable_manager.hpp src/tmdl/variable_manager.cpp
    src/tmdl/blocks/arithmetic.hpp src/tmdl/blocks/arithmetic.cpp
    src/tmdl/blocks/clock.hpp src/tmdl/blocks/clock.cpp
    src/tmdl/blocks/constant.hpp src/tmdl/blocks/constant.cpp
    src/tmdl/blocks/delay.hpp src/tmdl/blocks/delay.cpp
    src/tmdl/blocks/derivative.hpp src/tmdl/blocks/derivative.cpp
    src/tmdl/blocks/integrator.hpp src/tmdl/blocks/integrator.cpp
    src/tmdl/blocks/io_ports.hpp src/tmdl/blocks/io_ports.cpp
    src/tmdl/blocks/limiter.hpp src/tmdl/blocks/limiter.cpp
    src/tmdl/blocks/relational.hpp src/tmdl/blocks/relational.cpp
    src/tmdl/blocks/trig.hpp src/tmdl/blocks/trig.cpp
    src/tmdl/libraries/model_library.hpp src/tmdl/libraries/model_library.cpp
    src/tmdl/libraries/stdlib.hpp src/tmdl/libraries/stdlib.cpp
    src/tmdl/util/identifiers.hpp src/tmdl/util/identifiers.cpp
)

# Define the library
add_library(
    tmdl
    ${TEAFIS_MODEL_SOURCES}
)

target_link_libraries(tmdl PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(tmdl PUBLIC fmt::fmt)

# Include Directories
target_include_directories(tmdl PUBLIC src)

# Add compiler options
if (MSVC)
    target_compile_options(tmdl PUBLIC /W4 /WX)
else()
    target_compile_options(tmdl PUBLIC -Wall -Wextra -pedantic -Werror)

    if (TEAFIS_USE_ASAN)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()

    if (TEAFIS_USE_LSAN)
        set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak")
    endif()
endif()
