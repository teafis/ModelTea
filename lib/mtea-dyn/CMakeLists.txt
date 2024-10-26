# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

set(VCPKG_FEATURE_FLAGS "versions")

# Define the project information
project(
    mtea-dyn
    VERSION 0.1
    LANGUAGES CXX
)

set(
    TEAFIS_MODEL_SOURCES
    include/block_interface.hpp src/block_interface.cpp
    include/connection.hpp src/connection.cpp
    include/connection_manager.hpp src/connection_manager.cpp
    include/data_dictionary.hpp src/data_dictionary.cpp
    include/data_parameter.hpp src/data_parameter.cpp
    include/execution_state.hpp src/execution_state.cpp
    include/library.hpp src/library.cpp
    include/model_manager.hpp src/model_manager.cpp
    include/model.hpp src/model.cpp
    include/model_block.hpp src/model_block.cpp
    include/model_exception.hpp src/model_exception.cpp
    include/data_type.hpp
    include/identifier.hpp src/identifier.cpp
    include/parameter.hpp src/parameter.cpp
    include/value.hpp src/value.cpp
    include/value_array.hpp src/value_array.cpp
    include/variable_manager.hpp src/variable_manager.cpp
    include/block_io_ports.hpp src/block_io_ports.cpp
    include/library_model.hpp src/library_model.cpp
    include/library_stdlib.hpp src/library_stdlib.cpp
    include/codegen.hpp src/codegen.cpp
    include/codegen_generator.hpp src/codegen_generator.cpp
    include/codegen_component.hpp src/codegen_component.cpp
)

# Define the library
add_library(
    mtea-dyn
    ${TEAFIS_MODEL_SOURCES}
)

# Include Directories
target_include_directories(mtea-dyn PUBLIC include)

# Define the C++ Standard Required
set_property(TARGET mtea-dyn PROPERTY CXX_STANDARD 23)
set_property(TARGET mtea-dyn PROPERTY CXX_STANDARD_REQUIRED ON)

# Link Libraries
target_link_libraries(mtea-dyn PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(mtea-dyn PUBLIC fmt::fmt)

target_link_libraries(mtea-dyn PUBLIC mtea)
