# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.28)

set(VCPKG_FEATURE_FLAGS "versions")

# Define the project information
project(
    tmdl
    VERSION 0.1
    LANGUAGES CXX
)

# Define the library
add_library(tmdl)

target_sources(tmdl
  PUBLIC
    FILE_SET CXX_MODULES FILES
        src/block_interface.cpp
        src/codegen.cpp
        src/codegen_component.cpp
        src/codegen_generator.cpp
        src/connection.cpp
        src/connection_manager.cpp
        src/data_dictionary.cpp
        src/data_parameter.cpp
        src/data_type.cpp
        src/execution_state.cpp
        src/identifier.cpp
        src/io_ports.cpp
        src/library.cpp
        src/library_manager.cpp
        src/library_model.cpp
        src/library_stdlib.cpp
        src/model.cpp
        src/model_block.cpp
        src/model_exception.cpp
        src/parameter.cpp
        src/tmdl.cpp
        src/value.cpp
        src/value_array.cpp
        src/variable_manager.cpp
)

# Define the C++ Standard Required
set_property(TARGET tmdl PROPERTY CXX_STANDARD 20)
set_property(TARGET tmdl PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET tmdl PROPERTY CXX_EXTENSIONS OFF)

# Link Libraries
target_link_libraries(tmdl PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(tmdl PUBLIC fmt::fmt)

target_link_libraries(tmdl PRIVATE mtstdlib)

# Include Directories
target_include_directories(tmdl PUBLIC src)
