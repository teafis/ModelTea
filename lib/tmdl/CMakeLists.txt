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
        src/tmdl/tmdl.cpp

        src/tmdl/io_ports.cpp

        src/tmdl/codegen.cpp
        src/tmdl/codegen_component.cpp
        src/tmdl/codegen_generator.cpp

        src/tmdl/data_dictionary.cpp
        src/tmdl/data_parameter.cpp

        src/tmdl/values/mod.cpp
        src/tmdl/values/data_type.cpp
        src/tmdl/values/identifier.cpp
        src/tmdl/values/parameter.cpp
        src/tmdl/values/value.cpp
        src/tmdl/values/value_array.cpp

        src/tmdl/block_interface.cpp
        src/tmdl/connection.cpp
        src/tmdl/connection_manager.cpp
        src/tmdl/execution_state.cpp

        src/tmdl/library.cpp
        src/tmdl/library_manager.cpp
        src/tmdl/library_model.cpp
        src/tmdl/library_stdlib.cpp

        src/tmdl/model.cpp
        src/tmdl/model_block.cpp
        src/tmdl/model_exception.cpp
        src/tmdl/variable_manager.cpp
)

# Define the C++ Standard Required
set_property(TARGET tmdl PROPERTY CXX_STANDARD 23)
set_property(TARGET tmdl PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET tmdl PROPERTY CXX_EXTENSIONS OFF)

# Link Libraries
target_link_libraries(tmdl PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(tmdl PUBLIC fmt::fmt)

target_link_libraries(tmdl PRIVATE mtstdlib)

# Include Directories
target_include_directories(tmdl PUBLIC src)
