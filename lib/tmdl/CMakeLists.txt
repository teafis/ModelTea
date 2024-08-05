# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.28)

set(VCPKG_FEATURE_FLAGS "versions")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

# Define the project information
project(
    tmdl
    VERSION 0.1
    LANGUAGES CXX
)

set(
    TEAFIS_MODEL_SOURCES
    src/tmdl/block_interface.hpp src/tmdl/block_interface.cpp
    src/tmdl/connection.hpp src/tmdl/connection.cpp
    src/tmdl/connection_manager.hpp src/tmdl/connection_manager.cpp
    src/tmdl/dictionary/data_dictionary.hpp src/tmdl/dictionary/data_dictionary.cpp
    src/tmdl/dictionary/data_parameter.hpp src/tmdl/dictionary/data_parameter.cpp
    src/tmdl/execution_state.hpp src/tmdl/execution_state.cpp
    src/tmdl/library.hpp src/tmdl/library.cpp
    src/tmdl/library_manager.hpp src/tmdl/library_manager.cpp
    src/tmdl/model.hpp src/tmdl/model.cpp
    src/tmdl/model_block.hpp src/tmdl/model_block.cpp
    src/tmdl/model_exception.hpp src/tmdl/model_exception.cpp
    src/tmdl/values/data_types.hpp
    src/tmdl/values/identifiers.hpp src/tmdl/values/identifiers.cpp
    src/tmdl/values/parameter.hpp src/tmdl/values/parameter.cpp
    src/tmdl/values/value.hpp src/tmdl/values/value.cpp
    src/tmdl/values/value_array.hpp src/tmdl/values/value_array.cpp
    src/tmdl/variable_manager.hpp src/tmdl/variable_manager.cpp
    #src/tmdl/blocks/io_ports.hpp src/tmdl/blocks/io_ports.cpp
    src/tmdl/libraries/model_library.hpp src/tmdl/libraries/model_library.cpp
    src/tmdl/libraries/stdlib.hpp src/tmdl/libraries/stdlib.cpp
    src/tmdl/codegen/generator.hpp src/tmdl/codegen/generator.cpp
    src/tmdl/codegen/component.hpp src/tmdl/codegen/component.cpp
    src/tmdl/codegen/codegen.hpp src/tmdl/codegen/codegen.cpp
)

# Define the library
add_library(
    tmdl

)

target_sources(tmdl
    PUBLIC
    ${TEAFIS_MODEL_SOURCES}qt
)

target_sources(tmdl
  PUBLIC
    FILE_SET CXX_MODULES FILES
        src/tmdl/tmdl.cpp
        src/tmdl/blocks/io_ports.cpp
        src/tmdl/codegen/codegen.cpp
        src/tmdl/codegen/component.cpp
        src/tmdl/codegen/generator.cpp
        src/tmdl/dictionary/data_dictionary.cpp
        src/tmdl/dictionary/data_parameter.cpp
        src/tmdl/dictionary/dictionary/cpp
        src/tmdl/libraries/libraries.cpp
        src/tmdl/libraries/model_library.cpp
        src/tmdl/libraries/stdlib.cpp
        src/tmdl/values/data_type.cpp
        src/tmdl/values/identifier.cpp
        src/tmdl/values/parameter.cpp
        src/tmdl/values/value.cpp
        src/tmdl/values/value_array.cpp
        src/tmdl/values/values.cpp
)

# Define the C++ Standard Required
set_property(TARGET tmdl PROPERTY CXX_STANDARD 23)
set_property(TARGET tmdl PROPERTY CXX_STANDARD_REQUIRED ON)

# Link Libraries
target_link_libraries(tmdl PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(tmdl PUBLIC fmt::fmt)

target_link_libraries(tmdl PRIVATE mtstdlib)

# Include Directories
target_include_directories(tmdl PUBLIC src)
