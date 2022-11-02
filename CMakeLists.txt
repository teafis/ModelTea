# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.5)

project(TSim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        windows/main_window.cpp
        windows/main_window.h
        windows/main_window.ui
        windows/main_window_icon.cpp
        windows/main_window_icon.h
        widgets/block_graphics_view.cpp
        widgets/block_graphics_view.h
        widgets/state/block_drag_state.cpp
        widgets/state/block_drag_state.h
        blocks/base_block.cpp
        blocks/base_block.h
        blocks/block_io_port.cpp
        blocks/block_io_port.h
)

# Add library directories
add_subdirectory(lib/sim_math)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TSim
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(TSim
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(TSim PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(TSim PRIVATE TeaFIS-SimMath)

set_target_properties(TSim PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.teafis.tsim
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TSim)
endif()
